/* 
 * File:   InputDriverAbstract.h
 * Author: shestakov
 *
 * Created on 1 Август 2013 г., 11:31
 */

#ifndef INPUTDRIVERABSTRACT_H
#define	INPUTDRIVERABSTRACT_H

#include "../../include/system/InputTypes.h"
#include <string>

class InputDriverAbstract
{
public:
	InputDriverAbstract(const char *device_path);
	virtual ~InputDriverAbstract();

protected:
	virtual int processEvent() = 0;

public:
	virtual int initialize();
	virtual int finalize();
	virtual int listen();

public:
	virtual int validate() const;
	virtual InputDeviceType getDeviceType() const;
	virtual const char *getDevicePath() const;
	virtual const char *getDeviceName() const;
	
public:
	virtual bool isKeyPressed(keyvalue_t key) = 0;
	virtual bool isLedOn(keyvalue_t led) = 0;

public:
	virtual bool hasKeysKeyboard() = 0;
	virtual bool hasKeysMouse() = 0;
	virtual bool hasKeysJoystic() = 0;
	virtual bool hasKeysGamepad() = 0;
	virtual bool hasKeysRemoteControl() = 0;
	virtual bool hasKeysWheel() = 0;
	virtual bool hasKeysGraphicsTablet() = 0;
	virtual bool hasKeysTrackpad() = 0;
	virtual bool hasAbsolute() = 0;
	virtual bool hasRelative() = 0;
	virtual bool hasRepeater() = 0;
	virtual bool hasLed() = 0;

protected:
	virtual int openDevice();
	virtual int closeDevice();
	virtual int readEvents();
	virtual int guessDeviceType();
	
protected:
	virtual int initMapping();
	virtual keyvalue_t getMappedKey(keyvalue_t key);
	virtual InputDeviceAbsolute getMappedAbsolute(InputDeviceAbsolute axis);
	virtual InputDeviceRelative getMappedRelative(InputDeviceRelative axis);

public:
	virtual int registerInputEventCallback(InputDriverInputCallback callback, void *data);
	virtual int registerDisconnetedEventCallback(InputDriverDisconnectedCallback callback, void *data);

protected:
	virtual int fireInputEventCallback(InputDriverEventInput *event);
	virtual int fireDisconnetedEventCallback(InputDriverEventDisconnected *event);
	
protected:
	InputDeviceType						mDeviceType;
	InputDeviceStatus					mDeviceStatus;
	int									mFileDescriptor;
	std::string							mDeviceFilePath;
	std::string							mDeviceName;
	std::string							mDevicePhisicalPath;
	std::string							mDeviceUnique;
	// flags of InputDeviceEventType
	int									mSupportedEvents;

protected:
	InputDriverInputCallback			mEventInputCallback;
	void *								mEventInputCallbackData;
	InputDriverDisconnectedCallback		mEventDisconnectedCallback;
	void *								mEventDisconnectedCallbackData;

protected:
	InputDeviceMapKeys *				mMapKeys;
	InputDeviceMapAbsolute *			mMapAbs;
	InputDeviceMapRelative *			mMapRel;
	InputDriverEventInput				mLastInputEvent;
	bool								mLastInputEventTriggered;
	
protected:
	void *								mAsyncIOEventBuffer;
	unsigned long						mAsyncIOEventBufferSize;
	
private:
	int									mAsyncIOStatus;
	void *								mAsyncIOControlBlock;
};

#endif	/* INPUTDRIVERABSTRACT_H */

