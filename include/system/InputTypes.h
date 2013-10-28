/* 
 * File:   InputTypes.h
 * Author: shestakov
 *
 * Created on 2 Август 2013 г., 11:52
 */

#ifndef __INPUT_TYPES_H_INC__
#define	__INPUT_TYPES_H_INC__

#include <map>
#include <sys/time.h>
#include <stdint.h>

class InputDriverAbstract;
class InputDeviceAbstract;
class InputSubsystem;

typedef int32_t          keyvalue_t;

typedef enum InputDeviceType 
{
	IDT_UNITIALIZED		= 0x0000,
	IDT_UNDEFINED		= 0x0010,
	IDT_KEYBOARD		= 0x0040,
	IDT_MOUSE			= 0x0080,
	IDT_JOYSTIK			= 0x0100,
	IDT_GAMEPAD			= 0x0200,
	IDT_GRAPHICS_TABLET	= 0x0400,
	IDT_WHEEL			= 0x0800,
	IDT_REMOTE_CONTROL	= 0x1000,
	IDT_TRACKPAD		= 0x2000,
	IDT_MAX				= 0xffff
}
InputDeviceType;


int InputDeviceTypeToString(InputDeviceType type, char *buffer, int size);


typedef enum InputDeviceStatus
{
	IDS_DISCONNECTED	= -2,
	IDS_UNITIALIZED		= -1,
	IDS_CLOSED			=  0,
	IDS_OPENED			=  1,
	IDS_LISTEN			=  2
}
InputDeviceStatus;

typedef enum InputDeviceEventType
{
	IDET_NONE			= ( 0       ),
	IDET_UNITIALIZED	= ( 1 << 0  ),
	IDET_SYN			= ( 1 << 1  ),
	IDET_KEY			= ( 1 << 2  ),
	IDET_REL			= ( 1 << 3  ),
	IDET_ABS			= ( 1 << 4  ),
	IDET_MSC			= ( 1 << 5  ),
	IDET_SW 			= ( 1 << 6  ),
	IDET_LED			= ( 1 << 7  ),
	IDET_SND			= ( 1 << 8  ),
	IDET_REP			= ( 1 << 9  ),
	IDET_FF 			= ( 1 << 10 ),
	IDET_PWR			= ( 1 << 11 ),
	IDET_FFS			= ( 1 << 12 ),
	IDET_MAX			= ( 1 << 31 )
}
InputDeviceEventType;

typedef enum InputDeviceAbsolute
{
	IDA_X,
	IDA_Y,
	IDA_Z,
	IDA_RX,
	IDA_RY,
	IDA_RZ,
	IDA_THROTTLE,
	IDA_RUDDER,
	IDA_WHEEL,
	IDA_GAS,
	IDA_BRAKE,
	IDA_HAT0X,
	IDA_HAT0Y,
	IDA_HAT1X,
	IDA_HAT1Y,
	IDA_HAT2X,
	IDA_HAT2Y,
	IDA_HAT3X,
	IDA_HAT3Y,
	IDA_PRESSURE,
	IDA_DISTANCE,
	IDA_TILT_X,
	IDA_TILT_Y,
	IDA_TOOL_WIDTH,
	IDA_VOLUME,
	IDA_MISC,
	IDA_MT_SLOT,	/* MT slot being modified */
	IDA_MT_TOUCH_MAJOR,	/* Major axis of touching ellipse */
	IDA_MT_TOUCH_MINOR,	/* Minor axis (omit if circular) */
	IDA_MT_WIDTH_MAJOR,	/* Major axis of approaching ellipse */
	IDA_MT_WIDTH_MINOR,	/* Minor axis (omit if circular) */
	IDA_MT_ORIENTATION,	/* Ellipse orientation */
	IDA_MT_POSITION_X,	/* Center X touch position */
	IDA_MT_POSITION_Y,	/* Center Y touch position */
	IDA_MT_TOOL_TYPE,	/* Type of touching device */
	IDA_MT_BLOB_ID,	/* Group a set of packets as a blob */
	IDA_MT_TRACKING_ID,	/* Unique ID of initiated contact */
	IDA_MT_PRESSURE,	/* Pressure on contact area */
	IDA_MT_DISTANCE,	/* Contact hover distance */
	IDA_MT_TOOL_X,	/* Center X tool position */
	IDA_MT_TOOL_Y,	/* Center Y tool position */
	IDA_MAX
}
InputDeviceAbsolute;

typedef enum InputDeviceRelative
{
	IDR_X,
	IDR_Y,
	IDR_Z,
	IDR_RX,
	IDR_RY,
	IDR_RZ,
	IDR_HWHEEL,
	IDR_DIAL,
	IDR_WHEEL,
	IDR_MISC,
	IDR_MAX
}
InputDeviceRelative;

typedef struct InputDriverEventInput 
{
	// type of event
	InputDeviceEventType	type;
	// time of event
	struct timeval			time;
	// for key event this is key state
	// for abs or rel 
	unsigned short			code;
	keyvalue_t				value;
}
InputDriverEventInput;

typedef struct InputDriverEventDisconnected 
{
	struct timeval			time;
	unsigned long			error;
}
InputDriverEventDisconnected;

typedef struct InputSubsystemEventConnected
{
	struct timeval			time;
	const char  *			path;
}
InputSubsystemEventConnected;

typedef struct InputSubsystemEventDisconnected
{
	struct timeval			time;
	const char  *			path;
}
InputSubsystemEventDisconnected;

typedef void (* InputDriverInputCallback)(InputDriverAbstract *driver, InputDriverEventInput *event, void *data);
typedef void (* InputDriverDisconnectedCallback)(InputDriverAbstract *driver, InputDriverEventDisconnected *event, void *data);

typedef void (* InputDeviceInputCallback)(InputDeviceAbstract *device, InputDriverEventInput *event, void *data);
typedef void (* InputDeviceDisconnectedCallback)(InputDeviceAbstract *device, InputDriverEventDisconnected *event, void *data);

typedef void (* InputSubsystemConnectedCallback)(InputSubsystem *system, InputSubsystemEventConnected *event, void *data);
typedef void (* InputSubsystemDisconnectedCallback)(InputSubsystem *system, InputSubsystemEventDisconnected *event, void *data);

typedef std::map<keyvalue_t, keyvalue_t> InputDeviceMapKeys;
typedef std::map<InputDeviceAbsolute, InputDeviceAbsolute> InputDeviceMapAbsolute;
typedef std::map<InputDeviceRelative, InputDeviceRelative> InputDeviceMapRelative;

#endif	/* __INPUT_TYPES_H_INC__ */
