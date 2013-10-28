/* 
 * File:   InputDeviceRemoteControl.h
 * Author: shestakov
 *
 * Created on 30 Июль 2013 г., 17:41
 */

#ifndef INPUTDEVICEREMOTECONTROL_H
#define	INPUTDEVICEREMOTECONTROL_H

#include "InputDeviceAbstract.h"

class InputDeviceRemoteControl
	: public InputDeviceAbstract
{
	friend class InputManager;
	
protected:
	InputDeviceRemoteControl(InputDriverAbstract *device);
	virtual ~InputDeviceRemoteControl();
	
public:
	virtual int initialize();
	virtual int finalize();

private:

};

#endif	/* INPUTDEVICEREMOTECONTROL_H */

