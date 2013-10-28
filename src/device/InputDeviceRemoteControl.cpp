/* 
 * File:   InputDeviceRemoteControl.cpp
 * Author: shestakov
 * 
 * Created on 30 Июль 2013 г., 17:41
 */

#include "../../include/device/InputDeviceRemoteControl.h"

InputDeviceRemoteControl::InputDeviceRemoteControl(InputDriverAbstract *device)
	: InputDeviceAbstract(device)
{
}

InputDeviceRemoteControl::~InputDeviceRemoteControl()
{
}

int InputDeviceRemoteControl::initialize()
{
	return 0;
}

int InputDeviceRemoteControl::finalize()
{
	return 0;
}