/* 
 * File:   InputManager.h
 * Author: shestakov
 *
 * Created on 25 Июль 2013 г., 18:40
 */

#ifndef INPUTMANAGER_H
#define	INPUTMANAGER_H

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
	virtual int Initialize();
	virtual int Finalize();
	virtual int Listen();
	
protected:
	void Log(const char *fmt, ...);
	
protected:
	virtual int ReadDeviceList(const char *directory);
	virtual int ReadDevice(const char *device_path);
	virtual void FreeDevices();
	
protected:
	virtual InputDeviceAbstract *CreateDevice(const char *device_path);
	virtual void AddDevice(InputDeviceAbstract *device);
	virtual void RemoveDevice(InputDeviceAbstract *device);
	virtual InputDeviceAbstract *FindDeviceByPath(const char* device_path);

protected:
	static void InputEventCallback(InputDeviceAbstract *device, InputDriverEventInput *event, void *data);
	static void DisconnetedEventCallback(InputDeviceAbstract *device, InputDriverEventDisconnected *event, void *data);
	static void InputSubsystemConnectedCallback(InputSubsystem *system, InputSubsystemEventConnected *event, void *data);
	static void InputSubsystemDisconnectedCallback(InputSubsystem *system, InputSubsystemEventDisconnected *event, void *data);

protected:
	virtual void OnDeviceInput(InputDeviceAbstract *device, InputDriverEventInput *event);
	virtual void OnDeviceDisconneted(InputDeviceAbstract *device, InputDriverEventDisconnected *event);
	
public:
	virtual int RegisterInputCallback(InputDeviceInputCallback callback, void *data);
	virtual int RegisterDisconnetedCallback(InputDeviceDisconnectedCallback callback, void *data);

protected:	
	virtual int FireInputCallback(InputDeviceAbstract *device, InputDriverEventInput *event);
	virtual int FireDisconnetedCallback(InputDeviceAbstract *device, InputDriverEventDisconnected *event);

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
};

#endif	/* INPUTMANAGER_H */

/*
 * enum device list
 * open device
 * read device info
 * create device
 * add in list
 * listen
 */
