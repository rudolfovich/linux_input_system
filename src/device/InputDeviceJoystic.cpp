/* 
 * File:   InputDeviceJoystic.cpp
 * Author: shestakov
 * 
 * Created on 30 Июль 2013 г., 17:38
 */

#include "../../include/device/InputDeviceJoystic.h"

InputDeviceJoystic::InputDeviceJoystic(InputDriverAbstract *device)
	: InputDeviceAbstract(device)
{
}

InputDeviceJoystic::~InputDeviceJoystic()
{
}

int InputDeviceJoystic::initialize()
{
	return 0;
}

int InputDeviceJoystic::finalize()
{
	return 0;
}
