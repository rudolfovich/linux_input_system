/* 
 * File:   InputDriverEvdev.cpp
 * Author: shestakov
 * 
 * Created on 1 Август 2013 г., 11:36
 */

#include "../../include/driver/InputDriverEvdev.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <memory.h>

#define bit_test(bit, array)   (array [bit / 8] &  (1 << (bit % 8)))
#define bit_set(bit, array)    (array [bit / 8] |= (1 << (bit % 8)))
#define bit_unset(bit, array)  (array [bit / 8] ^= (1 << (bit % 8)))

InputDriverEvdev::InputDriverEvdev(const char *device_path)
	: InputDriverAbstract(device_path)
{
	mAsyncIOEventBufferSize = sizeof (mInputEvent);
	mAsyncIOEventBuffer = &mInputEvent;

	initialize();
}

InputDriverEvdev::~InputDriverEvdev()
{
	finalize();
}

int InputDriverEvdev::openDevice()
{
	int ret;

	ret = InputDriverAbstract::openDevice();
	if (ret < 0) {
		perror("open device");
		closeDevice();
		return -1;
	}
	ret = readDeviceInfo();
	if (ret < 0) {
		perror("open device read info");
		closeDevice();
		return -1;
	}
	ret = readSupportedEvents();
	if (ret < 0) {
		perror("open device read events");
		closeDevice();
		return -1;
	}
	ret = readCapabilities();
	if (ret < 0) {
		perror("open device read caps");
		closeDevice();
		return -1;
	}
	return 0;
}

int InputDriverEvdev::readDeviceInfo()
{
	int ret;
	uint32_t version = 0;
	struct input_id device_info;
	char device_name[256] = "<unknown>";
	char device_phys[256] = "<unknown>";
	char device_uniq[256] = "<unknown>";

	/* ioctl() accesses the underlying driver */
	ret = ioctl(mFileDescriptor, EVIOCGVERSION, &version);
	if (ret < 0) {
		perror("ioctl evdev driver ver");
	}
	else {
		/* the EVIOCGVERSION ioctl() returns an int */
		/* so we unpack it and display it */
		debug_printf("  - driver version [%d.%d.%d]\n",
			version >> 16, (version >> 8) & 0xff,
			version & 0xff);
	}

	memset(&device_info, 0, sizeof (device_info));
	/* suck out some device information */
	ret = ioctl(mFileDescriptor, EVIOCGID, &device_info);
	if (ret < 0) {
		perror("evdev ioctl");
	}
	else {
		/* the EVIOCGID ioctl() returns input_devinfo
		 * structure - see <linux/input.h>
		 * So we work through the various elements,
		 * displaying each of them
		 */
		debug_printf("  - vendor [%04hx] product [%04hx] version [%04hx] is on a bus [",
			device_info.vendor, device_info.product,
			device_info.version);
		switch (device_info.bustype) {
		case BUS_PCI:
			debug_printf("BUS_PCI");
			break;
		case BUS_ISAPNP:
			debug_printf("BUS_ISAPNP");
			break;
		case BUS_USB:
			debug_printf("BUS_USB");
			break;
		case BUS_HIL:
			debug_printf("BUS_HIL");
			break;
		case BUS_BLUETOOTH:
			debug_printf("BUS_BLUETOOTH");
			break;
		case BUS_VIRTUAL:
			debug_printf("BUS_VIRTUAL");
			break;
		case BUS_ISA:
			debug_printf("BUS_ISA");
			break;
		case BUS_I8042:
			debug_printf("BUS_I8042");
			break;
		case BUS_XTKBD:
			debug_printf("BUS_XTKBD");
			break;
		case BUS_RS232:
			debug_printf("BUS_RS232");
			break;
		case BUS_GAMEPORT:
			debug_printf("BUS_GAMEPORT");
			break;
		case BUS_PARPORT:
			debug_printf("BUS_PARPORT");
			break;
		case BUS_AMIGA:
			debug_printf("BUS_AMIGA");
			break;
		case BUS_ADB:
			debug_printf("BUS_ADB");
			break;
		case BUS_I2C:
			debug_printf("BUS_I2C");
			break;
		case BUS_HOST:
			debug_printf("BUS_HOST");
			break;
		case BUS_GSC:
			debug_printf("BUS_GSC");
			break;
		case BUS_ATARI:
			debug_printf("BUS_ATARI");
			break;
		case BUS_SPI:
			debug_printf("BUS_SPI");
			break;
		default:
			debug_printf("UNKNOWN(%d)", device_info.bustype);
		}
		debug_printf("]\n");
	}

	/* suck out some device information */
	ret = ioctl(mFileDescriptor, EVIOCGNAME(sizeof (device_name)), &device_name);
	if (ret < 0) {
		perror("evgname ioctl");
	}
	else {
		debug_printf("  - device name [%s]\n", device_name);
	}
	mDeviceName = device_name;

	ret = ioctl(mFileDescriptor, EVIOCGPHYS(sizeof (device_phys)), device_phys);
	if (ret < 0) {
		perror("evphis ioctl");
	}
	else {
		debug_printf("  - device path [%s]\n", device_phys);
	}
	mDevicePhisicalPath = device_phys;

	ret = ioctl(mFileDescriptor, EVIOCGUNIQ(sizeof (device_uniq)), device_uniq);
	if (ret < 0) {
		//perror("ioctl device_uniq");
	}
	else {
		debug_printf("  - device identity [%s]\n", device_uniq);
	}
	mDeviceUnique = device_uniq;

	return 0;
}

int InputDriverEvdev::readSupportedEvents()
{
	int i, ret;

	mSupportedEvents = IDET_NONE;
	memset(mEventMask, 0, sizeof (mEventMask));

	ret = ioctl(mFileDescriptor, EVIOCGBIT(0, sizeof (mEventMask)), mEventMask);
	if (ret < 0) {
		perror("evdev ioctl");
		return -1;
	}
	debug_printf("  - Supported event types:\n");
	for (i = 0; i < EV_MAX; i++) {
		if (bit_test(i, mEventMask)) {
			/* the bit is set in the event types list */
			debug_printf("       - Event type 0x%02x ", i);
			switch (i) {
			case EV_SYN:
				mSupportedEvents += IDET_SYN;
				debug_printf(" (Synch Events)\n");
				break;
			case EV_KEY:
				mSupportedEvents |= IDET_KEY;
				debug_printf(" (Keys or Buttons)\n");
				break;
			case EV_REL:
				mSupportedEvents |= IDET_REL;
				debug_printf(" (Relative Axes)\n");
				break;
			case EV_ABS:
				mSupportedEvents |= IDET_ABS;
				debug_printf(" (Absolute Axes)\n");
				break;
			case EV_MSC:
				mSupportedEvents |= IDET_MSC;
				debug_printf(" (Miscellaneous)\n");
				break;
			case EV_SW:
				mSupportedEvents |= IDET_SW;
				debug_printf(" (SW)\n");
				break;
			case EV_LED:
				mSupportedEvents |= IDET_LED;
				debug_printf(" (LEDs)\n");
				break;
			case EV_SND:
				mSupportedEvents |= IDET_SND;
				debug_printf(" (Sounds)\n");
				break;
			case EV_REP:
				mSupportedEvents |= IDET_REP;
				debug_printf(" (Repeat)\n");
				break;
			case EV_FF:
				mSupportedEvents |= IDET_FF;
				debug_printf(" (Force Feedback)\n");
				break;
			case EV_FF_STATUS:
				mSupportedEvents |= IDET_FFS;
				debug_printf(" (Force Feedback Status)\n");
				break;
			case EV_PWR:
				mSupportedEvents |= IDET_PWR;
				debug_printf(" (Power Management)\n");
				break;
			default:
				debug_printf(" (Unknown: 0x%04hx)\n", i);
			}
		}
	}
	return 0;
}

int InputDriverEvdev::readCapabilities()
{
	int i, ret;

	memset(mKeyMask, 0, sizeof (mKeyMask));
	memset(mRelMask, 0, sizeof (mRelMask));
	memset(mAbsMask, 0, sizeof (mAbsMask));
	memset(mLedMask, 0, sizeof (mLedMask));
	memset(mFeedBackMask, 0, sizeof (mFeedBackMask));

	/*
	ret = ioctl(mFileDescriptor, EVIOCGKEY(sizeof (mKeyMask)), mKeyMask);
	if (ret < 0) {
		perror("ioctl EVIOCGKEY");
	}
	else {
		debug_printf("  - keys pressed: \n");
		for (i = 0; i < KEY_MAX; i++) {
			if (bit_test(i, mKeyMask)) {
				debug_printf("  Key 0x%02x \n", i);
			}
		}
	}
	 */
	if (mSupportedEvents & IDET_KEY) {
		ret = ioctl(mFileDescriptor, EVIOCGBIT(EV_KEY, sizeof (mKeyMask)), mKeyMask);
		if (ret < 0) {
			perror("EVIOCGBIT EV_KEY ioctl");
		}
		else {
			debug_printf("  - detected keys:  ");
			for (i = 0; i < KEY_MAX; i++) {
				if (bit_test(i, mKeyMask))
					debug_printf("%d, ", i);
			}
			debug_printf("\n");
		}
	}
	if (mSupportedEvents & IDET_REL) {
		ret = ioctl(mFileDescriptor, EVIOCGBIT(EV_REL, sizeof (mRelMask)), mRelMask);
		if (ret < 0) {
			perror("EVIOCGBIT EV_KEY ioctl");
		}
		else {
			debug_printf("  - detected rels:  ");
			for (i = 0; i < REL_MAX; i++) {
				if (bit_test(i, mRelMask))
					debug_printf("%d, ", i);
			}
			debug_printf("\n");
		}
	}
	if (mSupportedEvents & IDET_ABS) {
		ret = ioctl(mFileDescriptor, EVIOCGBIT(EV_ABS, sizeof (mAbsMask)), mAbsMask);
		if (ret < 0) {
			perror("EVIOCGBIT EV_KEY ioctl");
		}
		else {
			debug_printf("  - detected abs:  ");
			for (i = 0; i < ABS_MAX; i++) {
				if (bit_test(i, mAbsMask))
					debug_printf("%d, ", i);
			}
			debug_printf("\n");
		}
	}

	// MAPPER: 
	/*
	int codes[2];
	for (i=0; i<130; i++) {
		codes[0] = i;
		ret = ioctl(mFileDescriptor, EVIOCGKEYCODE, codes);
		if (0 == ret) {
			debug_printf("[0]= %d, [1] = %d\n", codes[0], codes[1]);
		}
	}
	 */

	if (mSupportedEvents & IDET_LED) {
		ret = ioctl(mFileDescriptor, EVIOCGBIT(EV_LED, sizeof (mLedMask)), mLedMask);
		if (ret < 0) {
			perror("EVIOCGBIT ioctl failed");
		}
		else {
			debug_printf("  - detected leds:  ");
			for (i = 0; i < LED_MAX; i++) {
				if (bit_test(i, mLedMask))
					debug_printf("%d, ", i);
			}
			debug_printf("\n");
		}
	}
	if (mSupportedEvents & IDET_FF) {
		ret = ioctl(mFileDescriptor, EVIOCGBIT(EV_FF, sizeof (mFeedBackMask)), mFeedBackMask);
		if (ret < 0) {
			perror("EVIOCGBIT ioctl feedback");
		}
	}

	return 0;
}

int InputDriverEvdev::processEvent()
{
	if (mInputEvent.type == EV_SYN) {
		return 0;
	}
	mLastInputEvent.code = mInputEvent.code;
	mLastInputEvent.time = mInputEvent.time;
	mLastInputEvent.value = mInputEvent.value;
	//debug_printf("   + device event %s: ", mDeviceName.c_str());
	switch (mInputEvent.type) {
	case EV_SYN:
		mLastInputEvent.type = IDET_SYN;
		//debug_printf(" - EV_SYN: %d, %d\n", event.code, event.value);
		break;
	case EV_KEY:
		mLastInputEvent.type = IDET_KEY;
		processKey();
		//debug_printf(" - EV_KEY: %d, %d\n", mInputEvent.code, mInputEvent.value);
		break;
	case EV_REL:
		mLastInputEvent.type = IDET_REL;
		processRel();
		//debug_printf(" - EV_REL: %d, %d\n", mInputEvent.code, mInputEvent.value);
		break;
	case EV_ABS:
		mLastInputEvent.type = IDET_ABS;
		processAbs();
		//debug_printf(" - EV_ABS: %d, %d\n", mInputEvent.code, mInputEvent.value);
		break;
	case EV_MSC:
		mLastInputEvent.type = IDET_MSC;
		//debug_printf(" - EV_MSC: %d, %d\n", mInputEvent.code, mInputEvent.value);
		break;
	case EV_SW:
		mLastInputEvent.type = IDET_SW;
		//debug_printf(" - EV_SW:  %d, %d\n", mInputEvent.code, mInputEvent.value);
		break;
	case EV_LED:
		mLastInputEvent.type = IDET_LED;
		//debug_printf(" - EV_LED: %d : %d", mInputEvent.code, mInputEvent.value);
		processLed();
		break;
	case EV_SND:
		mLastInputEvent.type = IDET_SND;
		//debug_printf(" - EV_SND: %d : %d", mInputEvent.code, mInputEvent.value);
		break;
	case EV_REP:
		mLastInputEvent.type = IDET_REP;
		//debug_printf(" - EV_REP: %d : %d", mInputEvent.code, mInputEvent.value);
		break;
	case EV_FF:
		mLastInputEvent.type = IDET_FF;
		//debug_printf(" - EV_FF: %d : %d", mInputEvent.code, mInputEvent.value);
		break;
	case EV_PWR:
		mLastInputEvent.type = IDET_PWR;
		//debug_printf(" - EV_PWR: %d : %d", mInputEvent.code, mInputEvent.value);
		break;
	case EV_FF_STATUS:
		mLastInputEvent.type = IDET_FFS;
		//debug_printf(" - EV_FF_STATUS: %d : %d", mInputEvent.code, mInputEvent.value);
		break;
	default:
		mLastInputEvent.type = IDET_UNITIALIZED;
		//fprintf(stderr, "\nunknown event type %d, code: %d, value: %d\n", mInputEvent.type, mInputEvent.code, mInputEvent.value);
	}
	if (mLastInputEvent.type != IDET_UNITIALIZED) {
		mLastInputEventTriggered = true;
	}
	return 0;
}

bool InputDriverEvdev::isKeyPressed(keyvalue_t key)
{
	return bit_test(key, mKeyPressedMask);
}

bool InputDriverEvdev::isLedOn(keyvalue_t led)
{
	return bit_test(led, mLedOnMask);
}

bool InputDriverEvdev::hasKeysKeyboard()
{
	if (false == bit_test(KEY_1, mKeyMask)) return false;
	if (false == bit_test(KEY_0, mKeyMask)) return false;
	if (false == bit_test(KEY_A, mKeyMask)) return false;
	if (false == bit_test(KEY_Z, mKeyMask)) return false;
	if (false == bit_test(KEY_ENTER, mKeyMask)) return false;
	if (false == bit_test(KEY_ESC, mKeyMask)) return false;
	if (false == bit_test(KEY_LEFTALT, mKeyMask)) return false;
	if (false == bit_test(KEY_LEFTCTRL, mKeyMask)) return false;
	if (false == bit_test(KEY_LEFTSHIFT, mKeyMask)) return false;
	return true;
}

bool InputDriverEvdev::hasKeysMouse()
{
	if (!bit_test(BTN_MOUSE, mKeyMask)) return false;
	return true;
}

bool InputDriverEvdev::hasKeysJoystic()
{
	if (!bit_test(BTN_JOYSTICK, mKeyMask)) return false;
	return true;
}

bool InputDriverEvdev::hasKeysGamepad()
{
	if (!bit_test(BTN_GAMEPAD, mKeyMask)) return false;
	return true;
}

bool InputDriverEvdev::hasKeysRemoteControl()
{
	if (!bit_test(KEY_NUMERIC_0, mKeyMask)) return false;
	return true;
}

bool InputDriverEvdev::hasKeysWheel()
{
	if (!bit_test(BTN_WHEEL, mKeyMask)) return false;
	return true;
}

bool InputDriverEvdev::hasKeysGraphicsTablet()
{
	if (!bit_test(BTN_DIGI, mKeyMask)) return false;
	return true;
}

bool InputDriverEvdev::hasKeysTrackpad()
{
	if (!bit_test(BTN_TOOL_DOUBLETAP, mKeyMask)) return false;
	return true;
}

bool InputDriverEvdev::hasAbsolute()
{
	for (int i = 0; i < ABS_MAX; i++) {
		if (bit_test(i, mAbsMask)) return true;
	}
	return false;
}

bool InputDriverEvdev::hasRelative()
{
	for (int i = 0; i < REL_MAX; i++) {
		if (bit_test(i, mRelMask)) return true;
	}
	return false;
}

bool InputDriverEvdev::hasRepeater()
{
	return (mSupportedEvents & IDET_REP) ? true : false;
}

bool InputDriverEvdev::hasLed()
{
	for (int i = 0; i < LED_MAX; i++) {
		if (bit_test(i, mLedMask)) return true;
	}
	return false;
}

int InputDriverEvdev::processKey()
{
	if (mLastInputEvent.type == IDET_KEY) {
		switch (mLastInputEvent.value) {
		case 1:
			bit_set(mLastInputEvent.code, mKeyPressedMask);
			break;
		case 0:
			bit_unset(mLastInputEvent.code, mKeyPressedMask);
			break;
		}
	}
	return 0;
}

int InputDriverEvdev::processAbs()
{
	switch (mLastInputEvent.code) {
	case ABS_X:
		mLastInputEvent.code = IDA_X;
		break;
	case ABS_Y:
		mLastInputEvent.code = IDA_Y;
		break;
	case ABS_Z:
		mLastInputEvent.code = IDA_Z;
		break;
	case ABS_RX:
		mLastInputEvent.code = IDA_RX;
		break;
	case ABS_RY:
		mLastInputEvent.code = IDA_RY;
		break;
	case ABS_RZ:
		mLastInputEvent.code = IDA_RZ;
		break;
	case ABS_THROTTLE:
		mLastInputEvent.code = IDA_THROTTLE;
		break;
	case ABS_RUDDER:
		mLastInputEvent.code = IDA_RUDDER;
		break;
	case ABS_WHEEL:
		mLastInputEvent.code = IDA_WHEEL;
		break;
	case ABS_GAS:
		mLastInputEvent.code = IDA_GAS;
		break;
	case ABS_BRAKE:
		mLastInputEvent.code = IDA_BRAKE;
		break;
	case ABS_HAT0X:
		mLastInputEvent.code = IDA_HAT0X;
		break;
	case ABS_HAT0Y:
		mLastInputEvent.code = IDA_HAT0Y;
		break;
	case ABS_HAT1X:
		mLastInputEvent.code = IDA_HAT1X;
		break;
	case ABS_HAT1Y:
		mLastInputEvent.code = IDA_HAT1Y;
		break;
	case ABS_HAT2X:
		mLastInputEvent.code = IDA_HAT2X;
		break;
	case ABS_HAT2Y:
		mLastInputEvent.code = IDA_HAT2Y;
		break;
	case ABS_HAT3X:
		mLastInputEvent.code = IDA_HAT3X;
		break;
	case ABS_HAT3Y:
		mLastInputEvent.code = IDA_HAT3Y;
		break;
	case ABS_PRESSURE:
		mLastInputEvent.code = IDA_PRESSURE;
		break;
	case ABS_DISTANCE:
		mLastInputEvent.code = IDA_DISTANCE;
		break;
	case ABS_TILT_X:
		mLastInputEvent.code = IDA_TILT_X;
		break;
	case ABS_TILT_Y:
		mLastInputEvent.code = IDA_TILT_Y;
		break;
	case ABS_TOOL_WIDTH:
		mLastInputEvent.code = IDA_TOOL_WIDTH;
		break;
	case ABS_VOLUME:
		mLastInputEvent.code = IDA_VOLUME;
		break;
	case ABS_MISC:
		mLastInputEvent.code = IDA_MISC;
		break;
	case ABS_MT_SLOT:
		mLastInputEvent.code = IDA_MT_SLOT;
		break;
	case ABS_MT_TOUCH_MAJOR:
		mLastInputEvent.code = IDA_MT_TOUCH_MAJOR;
		break;
	case ABS_MT_TOUCH_MINOR:
		mLastInputEvent.code = IDA_MT_TOUCH_MINOR;
		break;
	case ABS_MT_WIDTH_MAJOR:
		mLastInputEvent.code = IDA_MT_WIDTH_MAJOR;
		break;
	case ABS_MT_WIDTH_MINOR:
		mLastInputEvent.code = IDA_MT_WIDTH_MINOR;
		break;
	case ABS_MT_ORIENTATION:
		mLastInputEvent.code = IDA_MT_ORIENTATION;
		break;
	case ABS_MT_POSITION_X:
		mLastInputEvent.code = IDA_MT_POSITION_X;
		break;
	case ABS_MT_POSITION_Y:
		mLastInputEvent.code = IDA_MT_POSITION_Y;
		break;
	case ABS_MT_TOOL_TYPE:
		mLastInputEvent.code = IDA_MT_TOOL_TYPE;
		break;
	case ABS_MT_BLOB_ID:
		mLastInputEvent.code = IDA_MT_BLOB_ID;
		break;
	case ABS_MT_TRACKING_ID:
		mLastInputEvent.code = IDA_MT_TRACKING_ID;
		break;
	case ABS_MT_PRESSURE:
		mLastInputEvent.code = IDA_MT_PRESSURE;
		break;
	case ABS_MT_DISTANCE:
		mLastInputEvent.code = IDA_MT_DISTANCE;
		break;
#if defined(ABS_MT_TOOL_X) && defined(ABS_MT_TOOL_Y)
	case ABS_MT_TOOL_X:
		mLastInputEvent.code = IDA_MT_TOOL_X;
		break;
	case ABS_MT_TOOL_Y:
		mLastInputEvent.code = IDA_MT_TOOL_Y;
		break;
#endif
	}
	return 0;
}

int InputDriverEvdev::processRel()
{
	switch (mLastInputEvent.code) {
	case REL_X:
		mLastInputEvent.code = IDR_X;
		break;
	case REL_Y:
		mLastInputEvent.code = IDR_Y;
		break;
	case REL_Z:
		mLastInputEvent.code = IDR_Z;
		break;
	case REL_RX:
		mLastInputEvent.code = IDR_RX;
		break;
	case REL_RY:
		mLastInputEvent.code = IDR_RY;
		break;
	case REL_RZ:
		mLastInputEvent.code = IDR_RZ;
		break;
	case REL_HWHEEL:
		mLastInputEvent.code = IDR_HWHEEL;
		break;
	case REL_DIAL:
		mLastInputEvent.code = IDR_DIAL;
		break;
	case REL_WHEEL:
		mLastInputEvent.code = IDR_WHEEL;
		break;
	case REL_MISC:
		mLastInputEvent.code = IDR_MISC;
		break;
	}
	return 0;
}

int InputDriverEvdev::processLed()
{
	if (mLastInputEvent.type == IDET_LED) {
		switch (mLastInputEvent.value) {
		case 1:
			bit_set(mLastInputEvent.code, mLedOnMask);
			break;
		case 0:
			bit_unset(mLastInputEvent.code, mLedOnMask);
			break;
		}
	}
	return 0;
}
