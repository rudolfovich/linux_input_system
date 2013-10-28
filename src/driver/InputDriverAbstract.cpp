/* 
 * File:   InputDriverAbstract.cpp
 * Author: shestakov
 * 
 * Created on 1 Август 2013 г., 11:31
 */

#include "../../include/driver/InputDriverAbstract.h"

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <memory.h>
#include <sys/eventfd.h>
#include <errno.h>
#include <aio.h>

#define UNITIALIZED_DEVICE_STRING		"<null>"

InputDriverAbstract::InputDriverAbstract(const char *device_path)
	: mFileDescriptor(-1)
	, mDeviceType(IDT_UNITIALIZED)
	, mDeviceStatus(IDS_UNITIALIZED)
	, mDeviceName(UNITIALIZED_DEVICE_STRING)
	, mDeviceFilePath(device_path)
	, mDevicePhisicalPath(UNITIALIZED_DEVICE_STRING)
	, mDeviceUnique(UNITIALIZED_DEVICE_STRING)
	, mAsyncIOStatus(0)
	, mAsyncIOControlBlock((struct aiocb *) new struct aiocb)
	, mAsyncIOEventBuffer(NULL)
	, mAsyncIOEventBufferSize(0)
	, mSupportedEvents(IDET_UNITIALIZED)
	, mMapKeys(NULL)
	, mMapAbs(NULL)
	, mMapRel(NULL)
	, mEventInputCallback(NULL)
	, mEventInputCallbackData(NULL)
	, mEventDisconnectedCallback(NULL)
	, mEventDisconnectedCallbackData(NULL)
{
}

InputDriverAbstract::~InputDriverAbstract()
{
	finalize();
}

int InputDriverAbstract::initialize()
{
	int ret;

	ret = openDevice();
	if (ret < 0) {
		finalize();
		return ret;
	}
	ret = guessDeviceType();
	if (ret < 0) {
		finalize();
		return ret;
	}
	ret = initMapping();
	if (ret < 0) {
		finalize();
		return ret;
	}
	ret = validate();
	if (ret < 0) {
		finalize();
		return ret;
	}
	return 0;
}

int InputDriverAbstract::finalize()
{
	int ret;

	ret = closeDevice();

	mDeviceStatus = IDS_UNITIALIZED;
	mDeviceType = IDT_UNITIALIZED;
	mSupportedEvents = IDET_UNITIALIZED;
	mDeviceName = UNITIALIZED_DEVICE_STRING;
	mDevicePhisicalPath = UNITIALIZED_DEVICE_STRING;
	mDeviceUnique = UNITIALIZED_DEVICE_STRING;

	if (mMapKeys) {
		delete mMapKeys;
		mMapKeys = NULL;
	}
	if (mMapAbs) {
		delete mMapAbs;
		mMapAbs = NULL;
	}
	if (mMapRel) {
		delete mMapRel;
		mMapRel = NULL;
	}
	return ret;
}

int InputDriverAbstract::listen()
{
	int ret;

	ret = readEvents();
	if (ret < 0) {
		// Failed
	}
	return ret;
}

int InputDriverAbstract::openDevice()
{
	if (mDeviceStatus == IDS_OPENED) {
		return 1;
	}

	if (mFileDescriptor == -1 || mFileDescriptor == 0) {
		mFileDescriptor = open(getDevicePath(), O_RDWR); // | O_NONBLOCK | O_EXCL
		if (mFileDescriptor == -1) {
			perror("InputDriverAbstract::openDevice(): open device");
			return -1;
		}
	}

	mDeviceStatus = IDS_OPENED;

	return 0;
}

int InputDriverAbstract::closeDevice()
{
	if (mDeviceStatus <= IDS_CLOSED)
		return 1;

	if (mFileDescriptor && mFileDescriptor != -1) {
		if (close(mFileDescriptor)) {
			perror("close device");
			return -1;
		}
		mFileDescriptor = -1;
	}

	mDeviceStatus = IDS_CLOSED;

	return 0;
}

int InputDriverAbstract::validate() const
{
	if (mDeviceType == IDT_UNITIALIZED)
		return -10;

	if (mDeviceStatus < IDS_OPENED)
		return -20;

	if (mSupportedEvents & IDET_UNITIALIZED)
		return -30;

	if (!mFileDescriptor || mFileDescriptor == -1)
		return -40;

	if (0 == mDeviceFilePath.length())
		return -50;

	return 0;
}

InputDeviceType InputDriverAbstract::getDeviceType() const
{
	return mDeviceType;
}

const char *InputDriverAbstract::getDevicePath() const
{
	return mDeviceFilePath.c_str();
}

const char *InputDriverAbstract::getDeviceName() const
{
	return mDeviceName.c_str();
}

int InputDriverAbstract::readEvents()
{
	ssize_t numBytes;
	struct aiocb *mAIOCB = (struct aiocb *) mAsyncIOControlBlock;

	if (0 == mAsyncIOStatus) {
		memset(mAIOCB, 0, sizeof (*mAIOCB));
		mAIOCB->aio_nbytes = mAsyncIOEventBufferSize;
		mAIOCB->aio_fildes = mFileDescriptor;
		mAIOCB->aio_offset = 0;
		mAIOCB->aio_buf = mAsyncIOEventBuffer;
		if (aio_read(mAIOCB) == -1) {
			return -1;
		}
	}
	mAsyncIOStatus = aio_error(mAIOCB);
	switch (mAsyncIOStatus) {
	case 0:
		// Всё прочитано, продолжаем...
		break;
	case EINPROGRESS:
		// Ещё не готово
		return 1;
	case ECANCELED:
		// Отменено пользователем
		return 2;
	case ENODEV:
		// Устройство было отключено
		InputDriverEventDisconnected evDisconnect;
		evDisconnect.error = ENODEV;
		gettimeofday(&evDisconnect.time, NULL);
		fireDisconnetedEventCallback(&evDisconnect);
		return 2;
	default:
		perror("aio_error");
		fprintf(stderr, "aio_error returns %d!\n", mAsyncIOStatus);
		return -1;
	}

	numBytes = aio_return(mAIOCB);
	if (numBytes < 0)
		return -1;
	if ((size_t)numBytes != mAIOCB->aio_nbytes)
		return -5;

	mLastInputEventTriggered = false;
	memset(&mLastInputEvent, 0, sizeof(mLastInputEvent));

	int ret = processEvent();
	if (ret < 0) {
		return -1;
	}
	if (mLastInputEventTriggered) {
		fireInputEventCallback(&mLastInputEvent);
		mLastInputEventTriggered = false;
	}

	return 0;
}

int InputDriverAbstract::guessDeviceType()
{

	mDeviceType = IDT_UNDEFINED;

	// Нужно ли проверять индикаторы? 
	// && hasLed()
	if (hasRepeater() && hasKeysKeyboard()) {
		mDeviceType = IDT_KEYBOARD;
	}
	else
		if (hasKeysGraphicsTablet() && hasAbsolute()) {
		mDeviceType = IDT_GRAPHICS_TABLET;
	}
	else
		if (hasRelative() && hasKeysMouse()) {
		if (hasKeysTrackpad()) {
			mDeviceType = IDT_TRACKPAD;
		}
		else {
			mDeviceType = IDT_MOUSE;
		}
	}
	else
		if (hasKeysWheel() && hasAbsolute()) {
		mDeviceType = IDT_WHEEL;
	}
	else
		if (hasKeysJoystic() && hasAbsolute()) {
		mDeviceType = IDT_JOYSTIK;
	}
	else
		if (hasKeysGamepad() && hasAbsolute()) {
		mDeviceType = IDT_GAMEPAD;
	}
	else
		if (hasKeysRemoteControl()) {
		mDeviceType = IDT_REMOTE_CONTROL;
	}

	char buf[1024];
	InputDeviceTypeToString(mDeviceType, buf, sizeof (buf));
	printf("  - device type [%s]\n", buf);

	return 0;
}

int InputDriverAbstract::initMapping()
{
	if (mSupportedEvents & IDET_KEY) {
		mMapKeys = new InputDeviceMapKeys();
	}
	if (mSupportedEvents & IDET_ABS) {
		mMapAbs = new InputDeviceMapAbsolute();
	}
	if (mSupportedEvents & IDET_REL) {
		mMapRel = new InputDeviceMapRelative();
	}
	return 0;
}

keyvalue_t InputDriverAbstract::getMappedKey(keyvalue_t key)
{
	InputDeviceMapKeys::iterator it;
	it = mMapKeys->find(key);
	if (mMapKeys->end() == it) {
		return key;
	}
	return it->second;
}

InputDeviceAbsolute InputDriverAbstract::getMappedAbsolute(InputDeviceAbsolute axis)
{
	InputDeviceMapAbsolute::iterator it;
	it = mMapAbs->find(axis);
	if (mMapAbs->end() == it) {
		return axis;
	}
	return it->second;
}

InputDeviceRelative InputDriverAbstract::getMappedRelative(InputDeviceRelative axis)
{
	InputDeviceMapRelative::iterator it;
	it = mMapRel->find(axis);
	if (mMapRel->end() == it) {
		return axis;
	}
	return it->second;
}

int InputDriverAbstract::registerInputEventCallback(InputDriverInputCallback callback, void *data)
{
	mEventInputCallback = callback;
	mEventInputCallbackData = data;
	return 0;
}

int InputDriverAbstract::fireInputEventCallback(InputDriverEventInput *event)
{
	if (mEventInputCallback) {
		if (! event->time.tv_sec && !event->time.tv_usec) {
			gettimeofday(&event->time, NULL);
		}
		mEventInputCallback(this, event, mEventInputCallbackData);
	}
	return 0;
}

int InputDriverAbstract::registerDisconnetedEventCallback(InputDriverDisconnectedCallback callback, void *data)
{
	mEventDisconnectedCallback = callback;
	mEventDisconnectedCallbackData = data;
	return 0;
}

int InputDriverAbstract::fireDisconnetedEventCallback(InputDriverEventDisconnected *event)
{
	if (mEventDisconnectedCallback) {
		if (! event->time.tv_sec && !event->time.tv_usec) {
			gettimeofday(&event->time, NULL);
		}
		mEventDisconnectedCallback(this, event, mEventDisconnectedCallbackData);
	}
	return 0;
}
