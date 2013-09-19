/* 
 * File:   InputSubsystem.cpp
 * Author: shestakov
 * 
 * Created on 1 Август 2013 г., 18:32
 */

#include "../../include/system/InputSubsystem.h"

#include <memory.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <asm/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#define UDEV_MONITOR_MAGIC                0xfeedcafe

struct udev_monitor_netlink_header
{
	/* "libudev" prefix to distinguish libudev and kernel messages */
	char prefix[8];
	/*
	 * magic to protect against daemon <-> library message format mismatch
	 * used in the kernel from socket filter rules; needs to be stored in network order
	 */
	unsigned int magic;
	/* total length of header structure known to the sender */
	unsigned int header_size;
	/* properties string buffer */
	unsigned int properties_off;
	unsigned int properties_len;
	/*
	 * hashes of primary device properties strings, to let libudev subscribers
	 * use in-kernel socket filters; values need to be stored in network order
	 */
	unsigned int filter_subsystem_hash;
	unsigned int filter_devtype_hash;
	unsigned int filter_tag_bloom_hi;
	unsigned int filter_tag_bloom_lo;
};

InputSubsystem::InputSubsystem()
	: mNetlinkSocket(-1)
	, mNetlinkMsgBufferSize(0)
	, mNetlinkMsgBuffer(NULL)
	, mOnConnected(NULL)
	, mOnConnectedData(NULL)
	, mOnDisconnected(NULL)
	, mOnDisconnectedData(NULL)
{
}

InputSubsystem::~InputSubsystem()
{
	Finalize();
}

int InputSubsystem::Initialize()
{
	int result = 0;
	struct sockaddr_nl snl;

	mNetlinkMsgBufferSize = 2048;
	mNetlinkMsgBuffer = new char[mNetlinkMsgBufferSize];

	memset(&snl, 0x00, sizeof (struct sockaddr_nl));
	snl.nl_family = AF_NETLINK;
	snl.nl_pid = getpid();
	snl.nl_groups = -1;

	mNetlinkSocket = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
	if (mNetlinkSocket == -1) {
		perror("socket");
		return -1;
	}
	result = bind(mNetlinkSocket, (struct sockaddr *) &snl, sizeof (struct sockaddr_nl));
	if (result < 0) {
		perror("bind");
		close(mNetlinkSocket);
		mNetlinkSocket = -1;
		return -1;
	}
	return 0;
}

int InputSubsystem::Finalize()
{
	if (mNetlinkSocket != -1) {
		close(mNetlinkSocket);
		mNetlinkSocket = -1;
	}
	if (mNetlinkMsgBuffer) {
		delete mNetlinkMsgBuffer;
		mNetlinkMsgBuffer = NULL;
	}
	mNetlinkMsgBufferSize = 0;
	return 0;
}

int InputSubsystem::Listen()
{
	ssize_t received;
	int select_result;
	fd_set mNetlinkSocketFdSet;
	struct timeval mNetlinkReceiveTimeoutStruct;

	received = 0;
	do {
		FD_ZERO(&mNetlinkSocketFdSet);
		FD_SET(mNetlinkSocket, &mNetlinkSocketFdSet);

		mNetlinkReceiveTimeoutStruct.tv_sec = 0;
		mNetlinkReceiveTimeoutStruct.tv_usec = 1;

		select_result = select(FD_SETSIZE, &mNetlinkSocketFdSet, NULL, NULL, &mNetlinkReceiveTimeoutStruct);
		// select_result ==  1 : данные уже присутствуют или получены до истечения таймаута
		// select_result ==  0 : таймаут истёк
		// select_result == -1 : ошибка
		if (select_result < 0) {
			return -1;
		}
		if (select_result == 0) {
			return 0;
		}
		mNetlinkMsgBufferRecieved = recv(mNetlinkSocket, mNetlinkMsgBuffer, mNetlinkMsgBufferSize, 0);
		if (mNetlinkMsgBufferRecieved < 0) {
			return -1;
		}
		if (mNetlinkMsgBufferRecieved) {
			ParseMessage();
		}
	}
	while (received > 0);
	return 0;
}

int InputSubsystem::ParseMessage()
{
	char *at, *type, *device;
	long cnt, len;
	char *str;
	int i;
	struct udev_monitor_netlink_header *nlh = NULL;

	return 0;
	//TODO:
	
	if (0 == strcmp(mNetlinkMsgBuffer, "libudev")) {
		nlh = (struct udev_monitor_netlink_header *) mNetlinkMsgBuffer;

		if (nlh->magic != htonl(UDEV_MONITOR_MAGIC)) {
			//udev_err(udev_monitor->udev, "unrecognized message signature (%x != %x)\n", nlh->magic, htonl(UDEV_MONITOR_MAGIC));
			return -1;
		}
		if (nlh->properties_off + 32 > (size_t) mNetlinkMsgBufferRecieved)
			return -1;
		mNetlinkMsgBuffer += nlh->properties_off;
		mNetlinkMsgBufferRecieved = nlh->properties_len;
	}

	i = 1;
	str = mNetlinkMsgBuffer;
	cnt = mNetlinkMsgBufferRecieved;
	while (cnt > 0) {
		printf("  >> NETLINK MSG #%d : %s\n", i++, str);
		len = strlen(str) + 1;
		str += len;
		cnt -= len;
	}

	if (! nlh) {
		at = strchr(mNetlinkMsgBuffer, '@');
		if (NULL == at) {
			//printf("Kernel message: [%s]\n", mNetlinkMsgBuffer);
			return 1;
		}
		*at = '\0';
		type = mNetlinkMsgBuffer;
		device = at + 1;
	}

	if (! nlh) {
		return 0;
	}
	
	return OnMessage(type, device);
}

int InputSubsystem::OnMessage(const char *type, const char *device)
{
	if (0 == strcmp(mNetlinkMsgBuffer, "add")) {
		return OnMessageAdd(device);
	}
	if (0 == strcmp(mNetlinkMsgBuffer, "remove")) {
		return OnMessageRemove(device);
	}
	printf("Kernel message of unknown type: [%s] [%s]\n", type, device);
	return 1;
}

int InputSubsystem::OnMessageAdd(const char *device)
{
	printf(" + [/sys%s]\n", device);
	InputSubsystemEventConnected event;
	event.path = device;
	event.time.tv_sec = 0;
	event.time.tv_usec = 0;
	FireConnectedCallback(&event);
}

int InputSubsystem::OnMessageRemove(const char *device)
{
	printf(" - [/sys%s]\n", device);
	InputSubsystemEventConnected event;
	event.path = device;
	event.time.tv_sec = 0;
	event.time.tv_usec = 0;
	FireConnectedCallback(&event);
}

int InputSubsystem::RegisterConnectedCallback(InputSubsystemConnectedCallback callback, void *data)
{
	mOnConnected = callback;
	mOnConnectedData = data;
	return 0;
}

int InputSubsystem::RegisterDisconnectedCallback(InputSubsystemDisconnectedCallback callback, void *data)
{
	mOnDisconnected = callback;
	mOnDisconnectedData = data;
	return 0;
}

int InputSubsystem::FireConnectedCallback(InputSubsystemEventConnected *event)
{
	if (mOnConnected) {
		if (!event->time.tv_sec && !event->time.tv_usec) {
			gettimeofday(&event->time, NULL);
		}
		mOnConnected(this, event, mOnConnectedData);
	}
	return 0;
}

int InputSubsystem::FireDisconnectedCallback(InputSubsystemEventDisconnected *event)
{
	if (mOnDisconnected) {
		if (!event->time.tv_sec && !event->time.tv_usec) {
			gettimeofday(&event->time, NULL);
		}
		mOnDisconnected(this, event, mOnDisconnectedData);
	}
	return 0;
}
