/* 
 * File:   InputDriverEvdev.h
 * Author: shestakov
 *
 * Created on 1 Август 2013 г., 11:36
 */

#ifndef INPUTDRIVEREVDEV_H
#define	INPUTDRIVEREVDEV_H

#include "InputDriverAbstract.h"

#include <aio.h>
#include <linux/input.h>
#include <string>

class InputDriverEvdev
	: public InputDriverAbstract
{
	friend class InputDevice;
	friend class InputManager;
	
protected:
	InputDriverEvdev(const char *device_path);
	virtual ~InputDriverEvdev();

protected:
	virtual int OpenDevice();

protected:
	virtual int ReadDeviceInfo();
	virtual int ReadSupportedEvents();
	virtual int ReadCapabilities();

public:
	virtual bool HasKeysKeyboard();
	virtual bool HasKeysMouse();
	virtual bool HasKeysJoystic();
	virtual bool HasKeysGamepad();
	virtual bool HasKeysRemoteControl();
	virtual bool HasKeysWheel();
	virtual bool HasKeysGraphicsTablet();
	virtual bool HasKeysTrackpad();
	virtual bool HasAbsolute();
	virtual bool HasRelative();
	virtual bool HasRepeater();
	virtual bool HasLed();
	
protected:
	virtual int ProcessEvent();

protected:
	virtual int ProcessKey();
	virtual int ProcessAbs();
	virtual int ProcessRel();
	
private:
	// Буфер для четения событий
	struct input_event		mInputEvent;

private:	
	unsigned char mEventMask[(EV_MAX + 7) / 8];
	unsigned char mKeyMask[(KEY_MAX + 7) / 8];
	unsigned char mRelMask[(REL_MAX + 7) / 8];
	unsigned char mAbsMask[(ABS_MAX + 7) / 8];
	unsigned char mLedMask[(LED_MAX + 7) / 8];
	unsigned char mFeedBackMask[(FF_MAX + 7) / 8];
};

#endif	/* INPUTDRIVEREVDEV_H */

