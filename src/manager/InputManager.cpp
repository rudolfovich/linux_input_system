/* 
 * File:   InputManager.cpp
 * Author: shestakov
 * 
 * Created on 25 Июль 2013 г., 18:40
 */

#include "../../include/manager/InputManager.h"
#include "../../include/device/InputDeviceAbstract.h"
#include "../../include/device/InputDeviceKeyboard.h"
#include "../../include/device/InputDeviceMouse.h"
#include "../../include/device/InputDeviceJoystic.h"
#include "../../include/device/InputDeviceRemoteControl.h"
#include "../../include/driver/InputDriverEvdev.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <memory.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/eventfd.h>
#include <linux/input.h>
#include <dirent.h>

InputManager::InputManager()
	: mOnDeviceInput(NULL)
	, mOnDeviceInputData(NULL)
	, mOnDeviceDisconnected(NULL)
	, mOnDeviceDisconnectedData(NULL)
{
	mSearchDevicePaths.push_back("/dev/input");
}

InputManager::InputManager(const InputManager& orig)
{
}

InputManager::~InputManager()
{
}

int InputManager::Initialize()
{
	mInputSubsystem.Initialize();
	mInputSubsystem.RegisterConnectedCallback(InputSubsystemConnectedCallback, this);
	mInputSubsystem.RegisterDisconnectedCallback(InputSubsystemDisconnectedCallback, this);

	std::list<std::string>::iterator it;

	for (it = mSearchDevicePaths.begin(); it != mSearchDevicePaths.end(); it++) {
		if (ReadDeviceList((*it).c_str())) {
			return -1;
		}
	}
	return 0;
}

int InputManager::Finalize()
{
	FreeDevices();
	mInputSubsystem.RegisterConnectedCallback(NULL, NULL);
	mInputSubsystem.RegisterDisconnectedCallback(NULL, NULL);
	mInputSubsystem.Finalize();
	return 0;
}

void InputManager::Log(const char *fmt, ...)
{
	va_list va;

	va_start(va, fmt);
	vprintf(fmt, va);
	va_end(va);
}

int InputManager::ReadDeviceList(const char *directory)
{
	DIR *dir;
	class dirent *ent;
	class stat st;

	dir = opendir(directory);
	while ((ent = readdir(dir)) != NULL) {
		std::string file_name = ent->d_name;
		std::string full_file_name = directory;
		full_file_name.append("/");
		full_file_name.append(file_name);

		if (file_name[0] == '.')
			continue;

		if (stat(full_file_name.c_str(), &st) == -1)
			continue;

		const bool is_directory = (st.st_mode & S_IFDIR) != 0;

		if (is_directory)
			continue;

		if (ReadDevice(full_file_name.c_str())) {
			//failed
		}
	}
	closedir(dir);

	return 0;
}

int InputManager::ReadDevice(const char *device_path)
{
	InputDeviceAbstract *device;

	Log("\n");
	Log("Creating device %s...\n", device_path);
	device = CreateDevice(device_path);
	Log("Creating device %s...", device_path);
	if (!device) {
		Log("failed!\n");
		return 1;
	}
	Log("success!\n");
	Log("\n");
	AddDevice(device);
	return 0;
}

void InputManager::FreeDevices()
{
	while (false == mDeviceList.empty()) {
		RemoveDevice(*mDeviceList.begin());
	}
}

InputDeviceAbstract *InputManager::FindDeviceByPath(const char *device_path)
{
	device_list_t::const_iterator it;

	for (it = mDeviceList.begin(); it != mDeviceList.end(); it++) {
		if (0 == strcmp((*it)->GetDevicePath(), device_path)) {
			return *it;
		}
	}
	return NULL;
}

InputDeviceAbstract *InputManager::CreateDevice(const char *device_path)
{
	char *name;
	InputDriverAbstract *driver = NULL;
	InputDeviceAbstract *device = NULL;

	device = FindDeviceByPath(device_path);
	if (NULL != device) {
		return device;
	}

	name = strrchr((char *) device_path, '/');
	if (NULL == name) {
		name = (char *) device_path;
	}
	else {
		if (strlen(name)) {
			name++;
		}
	}

	if (0 == strncmp(name, "event", strlen("event"))) {
		driver = new InputDriverEvdev(device_path);
	}

	if (NULL == driver) {
		return NULL;
	}

	switch (driver->GetDeviceType()) {
	case IDT_KEYBOARD:
		device = new InputDeviceKeyboard(driver);
		break;
	case IDT_MOUSE:
	case IDT_TRACKPAD:
		device = new InputDeviceMouse(driver);
		break;
	case IDT_JOYSTIK:
	case IDT_GAMEPAD:
	case IDT_WHEEL:
		device = new InputDeviceJoystic(driver);
		break;
	case IDT_REMOTE_CONTROL:
		device = new InputDeviceRemoteControl(driver);
		break;
	case IDT_UNDEFINED:
		device = new InputDeviceKeyboard(driver);
		break;
	case IDT_GRAPHICS_TABLET:
	default:
		delete driver;
		return NULL;
	}

	device->RegisterInputEventCallback(InputEventCallback, this);
	device->RegisterDisconnetedEventCallback(DisconnetedEventCallback, this);

	return device;
}

void InputManager::AddDevice(InputDeviceAbstract *device)
{
	device_list_t::const_iterator it;

	for (it = mDeviceList.begin(); it != mDeviceList.end(); it++) {
		if (*it == device) {
			return;
		}
		if (0 == strcmp(device->GetDevicePath(), (*it)->GetDevicePath())) {
			return;
		}
	}

	device->RegisterInputEventCallback(InputEventCallback, this);
	device->RegisterDisconnetedEventCallback(DisconnetedEventCallback, this);

	mDeviceList.push_back(device);
}

void InputManager::RemoveDevice(InputDeviceAbstract *device)
{
	device_list_t::const_iterator it;

	for (it = mDeviceList.begin(); it != mDeviceList.end(); it++) {
		if (*it == device) {
			break;
		}
	}
	if (*it != device) {
		// устройство не найдено 
		return;
	}

	device->RegisterInputEventCallback(NULL, NULL);
	device->RegisterDisconnetedEventCallback(NULL, NULL);
	mDeviceList.remove(device);
	delete device;
}

int InputManager::Listen()
{
	int ret;
	InputDeviceAbstract *device;
	device_list_t::const_iterator it;

	ret = mInputSubsystem.Listen();
	if (ret < 0) {
		return -1;
	}

	it = mDeviceList.begin();
	while (it != mDeviceList.end()) {
		for (; it != mDeviceList.end(); it++) {
			device = *it;
			ret = device->Listen();
			if (ret < 0) {
				Log("Listening device failed: %s(%s), %d\n\n", device->GetDeviceName(), device->GetDevicePath(), ret);
				// Отключаем при ошибке
				RemoveDevice(device);
				// Начинаем перебор сначала
				ret = 2;
			}
			if (ret == 2) {
				RemoveDevice(device);
				it = mDeviceList.begin();
				break;
			}
		}
	}
	return 0;
}

void InputManager::InputEventCallback(InputDeviceAbstract *device, InputDriverEventInput *event, void *data)
{
	InputManager *manager = (InputManager *) data;
	manager->OnDeviceInput(device, event);
}

void InputManager::DisconnetedEventCallback(InputDeviceAbstract *device, InputDriverEventDisconnected *event, void *data)
{
	InputManager *manager = (InputManager *) data;
	manager->OnDeviceDisconneted(device, event);
}

void InputManager::OnDeviceInput(InputDeviceAbstract *device, InputDriverEventInput *event)
{
	FireInputCallback(device, event);
}

void InputManager::OnDeviceDisconneted(InputDeviceAbstract *device, InputDriverEventDisconnected *event)
{
	printf("DEVICE DISCONNECTED: [%s]!!!\n", device->GetDeviceName());
	FireDisconnetedCallback(device, event);
	RemoveDevice(device);
}

void InputManager::InputSubsystemConnectedCallback(InputSubsystem *system, InputSubsystemEventConnected *event, void *data)
{
	InputManager *self = (InputManager *) data;
	char *name;
	std::string device_name, device_path;
	std::list<std::string>::iterator it;

	printf("DEVICE CONNECTED: [%s]!!!\n", event->path);

	name = strrchr((char *) event->path, '/');
	if (NULL == name) {
		return;
	}
	if (strlen(name)) {
		name++;
	}
	if (0 == strncmp(name, "event", strlen("event"))) {
		device_name = name;
	}
	if (!device_name.size()) {
		return;
	}
	for (it = self->mSearchDevicePaths.begin(); it != self->mSearchDevicePaths.end(); it++) {
		device_path = (*it).c_str();
		device_path.append("/");
		device_path.append(device_name);

		self->ReadDevice(device_path.c_str());
	}
}

void InputManager::InputSubsystemDisconnectedCallback(InputSubsystem *system, InputSubsystemEventDisconnected *event, void *data)
{
	InputManager *self = (InputManager *) data;
	char *name;
	std::string device_name, device_path;
	std::list<std::string>::iterator it;
	InputDeviceAbstract *device;

	printf("DEVICE DISCONNECTED: [%s]!!!\n", event->path);

	name = strrchr((char *) event->path, '/');
	if (NULL == name) {
		return;
	}
	if (strlen(name)) {
		name++;
	}
	if (0 == strncmp(name, "event", strlen("event"))) {
		device_name = name;
	}
	if (!device_name.size()) {
		return;
	}

	for (it = self->mSearchDevicePaths.begin(); it != self->mSearchDevicePaths.end(); it++) {
		device_path = (*it).c_str();
		device_path.append("/");
		device_path.append(device_name);

		device = self->FindDeviceByPath(device_path.c_str());
		if (device) {
			self->RemoveDevice(device);
		}
	}
}

int InputManager::RegisterInputCallback(InputDeviceInputCallback callback, void *data)
{
	mOnDeviceInput = callback;
	mOnDeviceInputData = data;
	return 0;
}

int InputManager::RegisterDisconnetedCallback(InputDeviceDisconnectedCallback callback, void *data)
{
	mOnDeviceDisconnected = callback;
	mOnDeviceDisconnectedData = data;
	return 0;
}

int InputManager::FireInputCallback(InputDeviceAbstract *device, InputDriverEventInput *event)
{
	if (mOnDeviceInput) {
		if (!event->time.tv_sec && !event->time.tv_usec) {
			gettimeofday(&event->time, NULL);
		}
		mOnDeviceInput(device, event, mOnDeviceInputData);
	}
	return 0;
}

int InputManager::FireDisconnetedCallback(InputDeviceAbstract *device, InputDriverEventDisconnected *event)
{
	if (mOnDeviceDisconnected) {
		if (!event->time.tv_sec && !event->time.tv_usec) {
			gettimeofday(&event->time, NULL);
		}
		mOnDeviceDisconnected(device, event, mOnDeviceDisconnectedData);
	}
	return 0;
}
