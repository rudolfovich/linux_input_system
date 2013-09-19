/* 
 * File:   InputDeviceJoystic.h
 * Author: shestakov
 *
 * Created on 30 Июль 2013 г., 17:38
 */

#ifndef INPUTDEVICEJOYSTIC_H
#define	INPUTDEVICEJOYSTIC_H

#include "InputDeviceAbstract.h"

class InputDeviceJoystic
	: public InputDeviceAbstract
{
	friend class InputManager;
	
protected:
	InputDeviceJoystic(InputDriverAbstract *device);
	virtual ~InputDeviceJoystic();
	
public:
	virtual int Initialize();
	virtual int Finalize();

private:

};

#endif	/* INPUTDEVICEJOYSTIC_H */

