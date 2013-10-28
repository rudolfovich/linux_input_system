/* 
 * File:   InputDeviceKeyboard.cpp
 * Author: shestakov
 * 
 * Created on 30 Июль 2013 г., 17:37
 */

#include "../../include/device/InputDeviceKeyboard.h"

InputDeviceKeyboard::InputDeviceKeyboard(InputDriverAbstract *device)
	: InputDeviceAbstract(device)
{
}

InputDeviceKeyboard::~InputDeviceKeyboard()
{
}

int InputDeviceKeyboard::initialize()
{
	return 0;
}

int InputDeviceKeyboard::finalize()
{
	return 0;
}