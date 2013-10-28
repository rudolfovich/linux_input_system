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

public:
	virtual bool isKeyPressed(keyvalue_t key);
	virtual bool isLedOn(keyvalue_t led);
	
public:
	virtual bool hasKeysKeyboard();
	virtual bool hasKeysMouse();
	virtual bool hasKeysJoystic();
	virtual bool hasKeysGamepad();
	virtual bool hasKeysRemoteControl();
	virtual bool hasKeysWheel();
	virtual bool hasKeysGraphicsTablet();
	virtual bool hasKeysTrackpad();
	virtual bool hasAbsolute();
	virtual bool hasRelative();
	virtual bool hasRepeater();
	virtual bool hasLed();

protected:
	virtual int openDevice();

protected:
	virtual int readDeviceInfo();
	virtual int readSupportedEvents();
	virtual int readCapabilities();
	
protected:
	virtual int processEvent();

protected:
	virtual int processKey();
	virtual int processAbs();
	virtual int processRel();
	virtual int processLed();
	
private:
	// Буфер для четения событий
	struct input_event		mInputEvent;

private:	
	unsigned char mEventMask[(EV_MAX + 7) / 8];
	unsigned char mKeyMask[(KEY_MAX + 7) / 8];
	unsigned char mKeyPressedMask[(KEY_MAX + 7) / 8];
	unsigned char mRelMask[(REL_MAX + 7) / 8];
	unsigned char mAbsMask[(ABS_MAX + 7) / 8];
	unsigned char mLedMask[(LED_MAX + 7) / 8];
	unsigned char mLedOnMask[(LED_MAX + 7) / 8];
	unsigned char mFeedBackMask[(FF_MAX + 7) / 8];
};

#endif	/* INPUTDRIVEREVDEV_H */

