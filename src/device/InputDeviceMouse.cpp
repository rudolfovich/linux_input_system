/* 
 * File:   InputDeviceMouse.cpp
 * Author: shestakov
 * 
 * Created on 30 Июль 2013 г., 17:38
 */

#include "../../include/device/InputDeviceMouse.h"

InputDeviceMouse::InputDeviceMouse(InputDriverAbstract *device)
	: InputDeviceAbstract(device)
{
}

InputDeviceMouse::~InputDeviceMouse()
{
}

int InputDeviceMouse::initialize()
{
	return 0;
}

int InputDeviceMouse::finalize()
{
	return 0;
}