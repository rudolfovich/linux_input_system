/* 
 * File:   InputSubsystem.h
 * Author: shestakov
 *
 * Created on 1 Август 2013 г., 18:32
 */

#ifndef __INPUT_SUBSYSTEM_H_INC__
#define	__INPUT_SUBSYSTEM_H_INC__

#include "InputTypes.h"

class InputSubsystem
{
public:
	InputSubsystem();
	virtual ~InputSubsystem();
	
public:
	int initialize();
	int finalize();
	int listen();

public:
	virtual int registerConnectedCallback(InputSubsystemConnectedCallback callback, void *data);
	virtual int registerDisconnectedCallback(InputSubsystemDisconnectedCallback callback, void *data);

protected:
	virtual int fireConnectedCallback(InputSubsystemEventConnected *event);
	virtual int fireDisconnectedCallback(InputSubsystemEventDisconnected *event);
	
protected:
	int parseMessage();
	int onMessage(const char *type, const char *device);
	int onMessageAdd(const char *device);
	int onMessageRemove(const char *device);

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

#endif	/* __INPUT_SUBSYSTEM_H_INC__ */
