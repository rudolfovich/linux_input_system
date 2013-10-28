# what flags you want to pass to the C compiler & linker

AM_CPPFLAGS = -I$(top_srcdir)/include -O2 -std=c++98 -fPIC -DPIC --pedantic -Wall -Wreturn-type -Wno-reorder 
AM_LDFLAGS  = -lrt 

lib_LIBRARIES = libinputsystem.a
libinputsystem_a_SOURCES = \
	system/InputTypes.cpp \
	system/InputSubsystem.cpp \
	driver/InputDriverAbstract.cpp \
	driver/InputDriverEvdev.cpp \
	device/InputDeviceAbstract.cpp \
	device/InputDeviceJoystic.cpp \
	device/InputDeviceKeyboard.cpp \
	device/InputDeviceMouse.cpp \
	device/InputDeviceRemoteControl.cpp \
	manager/InputManager.cpp

libinputsystem_adir = LIS
libinputsystem_a_HEADERS = ../include/InputSystem.h ../include/driver/InputDriverEvdev.h ../include/driver/InputDriverAbstract.h ../include/manager/InputManager.h ../include/system/InputSubsystem.h ../include/system/InputTypes.h ../include/device/InputDeviceMouse.h ../include/device/InputDeviceRemoteControl.h ../include/device/InputDeviceAbstract.h ../include/device/InputDeviceJoystic.h ../include/device/InputDeviceKeyboard.h 

bin_PROGRAMS = inputsystemtest
inputsystemtest_LDADD = libinputsystem.a
inputsystemtest_SOURCES = main.cpp
