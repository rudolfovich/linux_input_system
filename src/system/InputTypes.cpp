#include "../../include/system/InputTypes.h"
#include <string.h>

int InputDeviceTypeToString(InputDeviceType type, char *buffer, int size)
{
	int len, cnt;
	const char *name;

	switch (type) 
	{
	case IDT_UNITIALIZED:
		name = "<uninitialized>";
		break;
	case IDT_UNDEFINED:
		name = "<undefined>";
		break;
	case IDT_KEYBOARD:
		name = "keyboard";
		break;
	case IDT_MOUSE:
		name = "mouse";
		break;
	case IDT_JOYSTIK:
		name = "joystic";
		break;
	case IDT_GAMEPAD:
		name = "gamepad";
		break;
	case IDT_GRAPHICS_TABLET:
		name = "graphics tablet";
		break;
	case IDT_WHEEL:
		name = "wheel";
		break;
	case IDT_REMOTE_CONTROL:
		name = "remote control";
		break;
	case IDT_TRACKPAD:
		name = "trackpad";
		break;
	default:
		name = "<bad type>";
	}
	len = strlen(name) + 1;
	cnt = size < len ? size : len;
	strncpy(buffer, name, cnt);
	buffer[size - 1] = '\0';
	return cnt;
}