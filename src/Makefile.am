# what flags you want to pass to the C compiler & linker
CFLAGS = --pedantic -Wall -std=c99 -O2
AM_LDFLAGS = -lrt
AM_CPPFLAGS = -I$(top_srcdir)/include

lib_LIBRARIES = libinputsystem.a
libinputsystem_a_SOURCES = system/InputTypes.cpp system/InputSubsystem.cpp driver/InputDriverAbstract.cpp driver/InputDriverEvdev.cpp device/InputDeviceAbstract.cpp device/InputDeviceJoystic.cpp device/InputDeviceKeyboard.cpp device/InputDeviceMouse.cpp device/InputDeviceRemoteControl.cpp manager/InputManager.cpp

bin_PROGRAMS = inputsystemtest
inputsystemtest_LDADD = libinputsystem.a
inputsystemtest_SOURCES = main.cpp # system/InputTypes.cpp system/InputSubsystem.cpp driver/InputDriverAbstract.cpp driver/InputDriverEvdev.cpp device/InputDeviceAbstract.cpp device/InputDeviceJoystic.cpp device/InputDeviceKeyboard.cpp device/InputDeviceMouse.cpp device/InputDeviceRemoteControl.cpp manager/InputManager.cpp

