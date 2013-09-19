/* 
 * File:   main.cpp
 * Author: shestakov
 *
 * Created on 25 Июль 2013 г., 18:40
 */

#if 1

#include <stdio.h>
#include "InputSystem.h"

using namespace std;

static void InputEventCallback(InputDeviceAbstract *device, InputDriverEventInput *event, void *data)
{
	char type[256];
	const char *name;
	
	name = device->GetDeviceName();
	InputDeviceTypeToString(device->GetDeviceType(), type, sizeof(type));
	
	switch (event->type)
	{
	case IDET_KEY:
		printf("%s [%s]: KEY %d %d\n", name, type, event->code, event->value);
		break;
	case IDET_ABS:
		printf("%s [%s]: ABS %d %d\n", name, type, event->code, event->value);
		break;
	case IDET_REL:
		printf("%s [%s]: REL %d %d\n", name, type, event->code, event->value);
		break;
	default:
		//printf("%s [%s]: [%d] %d %d\n", name, type, event->type, event->code, event->value);
		break;
	}
}

int main(int argc, char** argv)
{
	int result = 0;
	InputManager *manager = new InputManager();
	manager->RegisterInputCallback(InputEventCallback, NULL);

	result = manager->Initialize();
	if (result)
		return result;
	
	while (1) {
		result = manager->Listen();
		if (result < 0) {
			break;
		}
	}
	result = manager->Finalize();
	return result;
}

#endif 
