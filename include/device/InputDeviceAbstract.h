/* 
 * File:   InputDeviceAbstract.h
 * Author: shestakov
 *
 * Created on 25 Июль 2013 г., 18:40
 */

#ifndef __INPUT_DEVICE_ABSTRACT_H_INC__
#define	__INPUT_DEVICE_ABSTRACT_H_INC__

#include "../../include/system/InputTypes.h"

class InputDeviceAbstract
{
public:
	InputDeviceAbstract(InputDriverAbstract *device);
	virtual ~InputDeviceAbstract();

public:
	virtual int initialize() = 0;
	virtual int finalize() = 0;
	virtual int listen();
	
public:
	virtual int validate() const;
	virtual InputDeviceType getDeviceType() const;
	virtual const char *getDevicePath() const;
	virtual const char *getDeviceName() const;
	virtual unsigned short getDeviceId() const;
	virtual void setDeviceId(unsigned short identifier);
	
public:
	virtual int registerInputEventCallback(InputDeviceInputCallback callback, void *data);
	virtual int registerDisconnetedEventCallback(InputDeviceDisconnectedCallback callback, void *data);

protected:	
	virtual int fireInputEventCallback(InputDriverEventInput *event);
	virtual int fireDisconnetedEventCallback(InputDriverEventDisconnected *event);
	
private:
	static void inputEventCallback(InputDriverAbstract *driver, InputDriverEventInput *event, void *data);
	static void disconnetedEventCallback(InputDriverAbstract *driver, InputDriverEventDisconnected *event, void *data);

protected:
	InputDriverAbstract *				mDeviceDriver;

protected:
	InputDeviceInputCallback			mEventInputCallback;
	void *								mEventInputCallbackData;
	InputDeviceDisconnectedCallback		mEventDisconnectedCallback;
	void *								mEventDisconnectedCallbackData;
	
private:
	unsigned short						mIdentifier;
};

#endif	/* __INPUT_DEVICE_ABSTRACT_H_INC__ */

