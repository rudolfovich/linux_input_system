/* 
 * File:   InputDeviceKeyboard.h
 * Author: shestakov
 *
 * Created on 30 Июль 2013 г., 17:37
 */

#ifndef INPUTDEVICEKEYBOARD_H
#define	INPUTDEVICEKEYBOARD_H

#include "InputDeviceAbstract.h"

class InputDeviceKeyboard 
	: public InputDeviceAbstract
{
	friend class InputManager;
	
protected:
	InputDeviceKeyboard(InputDriverAbstract *device);
	virtual ~InputDeviceKeyboard();

public:
	virtual int initialize();
	virtual int finalize();
	
private:

};

#endif	/* INPUTDEVICEKEYBOARD_H */

