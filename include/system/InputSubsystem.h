/* 
 * File:   InputSubsystem.h
 * Author: shestakov
 *
 * Created on 1 Август 2013 г., 18:32
 */

#ifndef INPUTSUBSYSTEM_H
#define	INPUTSUBSYSTEM_H

#include <sys/socket.h>
#include "InputTypes.h"

class InputSubsystem
{
public:
	InputSubsystem();
	virtual ~InputSubsystem();
	
public:
	int Initialize();
	int Finalize();
	int Listen();

public:
	virtual int RegisterConnectedCallback(InputSubsystemConnectedCallback callback, void *data);
	virtual int RegisterDisconnectedCallback(InputSubsystemDisconnectedCallback callback, void *data);

protected:
	virtual int FireConnectedCallback(InputSubsystemEventConnected *event);
	virtual int FireDisconnectedCallback(InputSubsystemEventDisconnected *event);
	
protected:
	int ParseMessage();
	int OnMessage(const char *type, const char *device);
	int OnMessageAdd(const char *device);
	int OnMessageRemove(const char *device);

protected:
	int				mNetlinkSocket;
	char *			mNetlinkMsgBuffer;
	unsigned long	mNetlinkMsgBufferSize;
	unsigned long	mNetlinkMsgBufferRecieved;
	
protected:
	InputSubsystemConnectedCallback		mOnConnected;
	void *								mOnConnectedData;
	InputSubsystemDisconnectedCallback	mOnDisconnected;
	void *								mOnDisconnectedData;
};

#endif	/* INPUTSUBSYSTEM_H */

