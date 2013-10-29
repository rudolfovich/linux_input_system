/* 
 * File:   InputDeviceAbstract.cpp
 * Author: shestakov
 * 
 * Created on 25 Июль 2013 г., 18:40
 */

#include "../../include/device/InputDeviceAbstract.h"
#include "../../include/driver/InputDriverAbstract.h"

InputDeviceAbstract::InputDeviceAbstract(InputDriverAbstract *device)
	: mDeviceDriver(device)
	, mEventInputCallback(NULL)
	, mEventInputCallbackData(NULL)
	, mEventDisconnectedCallback(NULL)
	, mEventDisconnectedCallbackData(NULL)
{
	device->registerInputEventCallback(inputEventCallback, this);
	device->registerDisconnetedEventCallback(disconnetedEventCallback, this);
}

InputDeviceAbstract::~InputDeviceAbstract()
{
	delete mDeviceDriver;
}

int InputDeviceAbstract::validate() const
{
	return mDeviceDriver->validate();
}

InputDeviceType InputDeviceAbstract::getDeviceType() const
{
	return mDeviceDriver->getDeviceType();
}

const char *InputDeviceAbstract::getDevicePath() const
{
	return mDeviceDriver->getDevicePath();
}

const char *InputDeviceAbstract::getDeviceName() const
{
	return mDeviceDriver->getDeviceName();
}

unsigned short InputDeviceAbstract::getDeviceId() const
{
	return mIdentifier;
}

void InputDeviceAbstract::setDeviceId(unsigned short identifier)
{
	mIdentifier = identifier;
}

int InputDeviceAbstract::listen()
{
	return mDeviceDriver->listen();
}

int InputDeviceAbstract::registerInputEventCallback(InputDeviceInputCallback callback, void *data)
{
	mEventInputCallback = callback;
	mEventInputCallbackData = data;
	return 0;
}

int InputDeviceAbstract::fireInputEventCallback(InputDriverEventInput *event)
{
	if (mEventInputCallback) {
		mEventInputCallback(this, event, mEventInputCallbackData);
	}
	return 0;
}

int InputDeviceAbstract::registerDisconnetedEventCallback(InputDeviceDisconnectedCallback callback, void *data)
{
	mEventDisconnectedCallback = callback;
	mEventDisconnectedCallbackData = data;
	return 0;
}

int InputDeviceAbstract::fireDisconnetedEventCallback(InputDriverEventDisconnected *event)
{
	if (mEventDisconnectedCallback) {
		mEventDisconnectedCallback(this, event, mEventDisconnectedCallbackData);
	}
	return 0;
}

void InputDeviceAbstract::inputEventCallback(InputDriverAbstract *driver, InputDriverEventInput *event, void *data)
{
	InputDeviceAbstract *device = (InputDeviceAbstract *) data;
	device->fireInputEventCallback(event);
}

void InputDeviceAbstract::disconnetedEventCallback(InputDriverAbstract *driver, InputDriverEventDisconnected *event, void *data)
{
	InputDeviceAbstract *device = (InputDeviceAbstract *) data;
	device->fireDisconnetedEventCallback(event);
}
