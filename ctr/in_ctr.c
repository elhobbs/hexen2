
#include "quakedef.h"

void IN_Init(void) {
}


void IN_Shutdown(void) {
}


void _3ds_controls(void);

void IN_Commands(void) {
	_3ds_controls();
}

// oportunity for devices to stick commands on the script buffer

void IN_Move(usercmd_t *cmd) {
}

// add additional movement on top of the keyboard move cmd

void IN_ClearStates(void) {
}

