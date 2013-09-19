/* 
 * File:   InputDeviceAbstract.cpp
 * Author: shestakov
 * 
 * Created on 25 Июль 2013 г., 18:40
 */

#include "../../include/device/InputDeviceAbstract.h"
#include "../../include/driver/InputDriverAbstract.h"
#include <stdlib.h>

InputDeviceAbstract::InputDeviceAbstract(InputDriverAbstract *device)
	: mDeviceDriver(device)
	, mEventInputCallback(NULL)
	, mEventInputCallbackData(NULL)
	, mEventDisconnectedCallback(NULL)
	, mEventDisconnectedCallbackData(NULL)
{
	device->RegisterInputEventCallback(InputEventCallback, this);
	device->RegisterDisconnetedEventCallback(DisconnetedEventCallback, this);
}

InputDeviceAbstract::~InputDeviceAbstract()
{
	delete mDeviceDriver;
}

int InputDeviceAbstract::Validate() const
{
	return mDeviceDriver->Validate();
}

InputDeviceType InputDeviceAbstract::GetDeviceType() const
{
	return mDeviceDriver->GetDeviceType();
}

const char *InputDeviceAbstract::GetDevicePath() const
{
	return mDeviceDriver->GetDevicePath();
}

const char *InputDeviceAbstract::GetDeviceName() const
{
	return mDeviceDriver->GetDeviceName();
}

int InputDeviceAbstract::Listen()
{
	return mDeviceDriver->Listen();
}

int InputDeviceAbstract::RegisterInputEventCallback(InputDeviceInputCallback callback, void *data)
{
	mEventInputCallback = callback;
	mEventInputCallbackData = data;
}

int InputDeviceAbstract::FireInputEventCallback(InputDriverEventInput *event)
{
	if (mEventInputCallback) {
		mEventInputCallback(this, event, mEventInputCallbackData);
	}
	return 0;
}

int InputDeviceAbstract::RegisterDisconnetedEventCallback(InputDeviceDisconnectedCallback callback, void *data)
{
	mEventDisconnectedCallback = callback;
	mEventDisconnectedCallbackData = data;
}

int InputDeviceAbstract::FireDisconnetedEventCallback(InputDriverEventDisconnected *event)
{
	if (mEventDisconnectedCallback) {
		mEventDisconnectedCallback(this, event, mEventDisconnectedCallbackData);
	}
	return 0;
}

void InputDeviceAbstract::InputEventCallback(InputDriverAbstract *driver, InputDriverEventInput *event, void *data)
{
	InputDeviceAbstract *device = (InputDeviceAbstract *) data;
	device->FireInputEventCallback(event);
}

void InputDeviceAbstract::DisconnetedEventCallback(InputDriverAbstract *driver, InputDriverEventDisconnected *event, void *data)
{
	InputDeviceAbstract *device = (InputDeviceAbstract *) data;
	device->FireDisconnetedEventCallback(event);
}
