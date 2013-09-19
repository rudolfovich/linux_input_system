/* 
 * File:   InputDeviceAbstract.h
 * Author: shestakov
 *
 * Created on 25 Июль 2013 г., 18:40
 */

#ifndef INPUTDEVICE_H
#define	INPUTDEVICE_H

#include "../../include/system/InputTypes.h"

class InputDeviceAbstract
{
public:
	InputDeviceAbstract(InputDriverAbstract *device);
	virtual ~InputDeviceAbstract();

public:
	virtual int Initialize() = 0;
	virtual int Finalize() = 0;
	virtual int Listen();
	
public:
	virtual int Validate() const;
	virtual InputDeviceType GetDeviceType() const;
	virtual const char *GetDevicePath() const;
	virtual const char *GetDeviceName() const;
	
public:
	virtual int RegisterInputEventCallback(InputDeviceInputCallback callback, void *data);
	virtual int RegisterDisconnetedEventCallback(InputDeviceDisconnectedCallback callback, void *data);

protected:	
	virtual int FireInputEventCallback(InputDriverEventInput *event);
	virtual int FireDisconnetedEventCallback(InputDriverEventDisconnected *event);
	
private:
	static void InputEventCallback(InputDriverAbstract *driver, InputDriverEventInput *event, void *data);
	static void DisconnetedEventCallback(InputDriverAbstract *driver, InputDriverEventDisconnected *event, void *data);

protected:
	InputDriverAbstract *				mDeviceDriver;

protected:
	InputDeviceInputCallback			mEventInputCallback;
	void *								mEventInputCallbackData;
	InputDeviceDisconnectedCallback		mEventDisconnectedCallback;
	void *								mEventDisconnectedCallbackData;
};

#endif	/* INPUTDEVICE_H */

