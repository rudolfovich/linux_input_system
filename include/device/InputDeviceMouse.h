/* 
 * File:   InputDeviceMouse.h
 * Author: shestakov
 *
 * Created on 30 Июль 2013 г., 17:38
 */

#ifndef INPUTDEVICEMOUSE_H
#define	INPUTDEVICEMOUSE_H

#include "InputDeviceAbstract.h"

class InputDeviceMouse
	: public InputDeviceAbstract
{
	friend class InputManager;
	
protected:
	InputDeviceMouse(InputDriverAbstract *device);
	virtual ~InputDeviceMouse();
	
public:
	virtual int Initialize();
	virtual int Finalize();

private:

};

#endif	/* INPUTDEVICEMOUSE_H */

