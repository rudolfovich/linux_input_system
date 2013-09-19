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
	virtual int ProcessEvent() = 0;

public:
	virtual int Initialize();
	virtual int Finalize();
	virtual int Listen();

public:
	virtual int Validate() const;
	virtual InputDeviceType GetDeviceType() const;
	virtual const char *GetDevicePath() const;
	virtual const char *GetDeviceName() const;
	
public:
	virtual bool HasKeysKeyboard() = 0;
	virtual bool HasKeysMouse() = 0;
	virtual bool HasKeysJoystic() = 0;
	virtual bool HasKeysGamepad() = 0;
	virtual bool HasKeysRemoteControl() = 0;
	virtual bool HasKeysWheel() = 0;
	virtual bool HasKeysGraphicsTablet() = 0;
	virtual bool HasKeysTrackpad() = 0;
	virtual bool HasAbsolute() = 0;
	virtual bool HasRelative() = 0;
	virtual bool HasRepeater() = 0;
	virtual bool HasLed() = 0;
	
protected:
	virtual int OpenDevice();
	virtual int CloseDevice();
	virtual int ReadEvents();
	virtual int GuessDeviceType();
	
protected:
	virtual int InitMapping();
	virtual keyvalue_t GetMappedKey(keyvalue_t key);
	virtual InputDeviceAbsolute GetMappedAbsolute(InputDeviceAbsolute axis);
	virtual InputDeviceRelative GetMappedRelative(InputDeviceRelative axis);

public:
	virtual int RegisterInputEventCallback(InputDriverInputCallback callback, void *data);
	virtual int RegisterDisconnetedEventCallback(InputDriverDisconnectedCallback callback, void *data);

protected:
	virtual int FireInputEventCallback(InputDriverEventInput *event);
	virtual int FireDisconnetedEventCallback(InputDriverEventDisconnected *event);
	
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

