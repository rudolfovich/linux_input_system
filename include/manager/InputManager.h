/* 
 * File:   InputManager.h
 * Author: shestakov
 *
 * Created on 25 Июль 2013 г., 18:40
 */

#ifndef __INPUT_MANAGER_H_INC__
#define	__INPUT_MANAGER_H_INC__

#include <list>
#include <map>
#include <string>

#include "../system/InputSubsystem.h"
#include "../device/InputDeviceAbstract.h"

class InputManager
{
public:
	InputManager();
	InputManager(const InputManager& orig);
	virtual ~InputManager();

public:
	virtual int initialize();
	virtual int finalize();
	virtual int listen();

public:
	virtual int getDeviceCount();
	virtual int getDeviceList(InputDeviceAbstract **list, int *count);

public:
	virtual unsigned long getTargetDeviceTypeMask() const;
	virtual void setTargetDeviceTypeMask(unsigned long mask);
	
public:
	virtual int registerInputCallback(InputDeviceInputCallback callback, void *data);
	virtual int registerDisconnetedCallback(InputDeviceDisconnectedCallback callback, void *data);

protected:
	void error_printf(const char *fmt, ...);
	void debug_printf(const char *fmt, ...);
	
protected:
	virtual int readDeviceList(const char *directory);
	virtual int readDevice(const char *device_path);
	virtual void freeDevices();
	
protected:
	virtual InputDeviceAbstract *createDevice(const char *device_path);
	virtual void addDevice(InputDeviceAbstract *device);
	virtual void removeDevice(InputDeviceAbstract *device);
	virtual InputDeviceAbstract *FindDeviceByPath(const char* device_path);

protected:
	static void inputEventCallback(InputDeviceAbstract *device, InputDriverEventInput *event, void *data);
	static void disconnetedEventCallback(InputDeviceAbstract *device, InputDriverEventDisconnected *event, void *data);
	static void inputSubsystemConnectedCallback(InputSubsystem *system, InputSubsystemEventConnected *event, void *data);
	static void inputSubsystemDisconnectedCallback(InputSubsystem *system, InputSubsystemEventDisconnected *event, void *data);

protected:
	virtual void onDeviceInput(InputDeviceAbstract *device, InputDriverEventInput *event);
	virtual void onDeviceDisconneted(InputDeviceAbstract *device, InputDriverEventDisconnected *event);

protected:	
	virtual int fireInputCallback(InputDeviceAbstract *device, InputDriverEventInput *event);
	virtual int fireDisconnetedCallback(InputDeviceAbstract *device, InputDriverEventDisconnected *event);

protected:
	typedef std::list<InputDeviceAbstract *>	device_list_t;
	
protected:
	InputSubsystem						mInputSubsystem;
	std::list<std::string>				mSearchDevicePaths;
	device_list_t						mDeviceList;
	
protected:
	InputDeviceInputCallback			mOnDeviceInput;
	void *								mOnDeviceInputData;
	InputDeviceDisconnectedCallback		mOnDeviceDisconnected;
	void *								mOnDeviceDisconnectedData;
	
protected:
	unsigned long						mTargetDeviceTypeMask;
};

#endif	/* __INPUT_MANAGER_H_INC__ */

/*
 * enum device list
 * open device
 * read device info
 * create device
 * add in list
 * listen
 */
