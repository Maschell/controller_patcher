/****************************************************************************
 * Copyright (C) 2016,2017 Maschell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#ifndef CP_RETAINS_VARS_H_
#define CP_RETAINS_VARS_H_

#include "dynamic_libs/syshid_functions.h"
#include "../patcher/ControllerPatcherDefs.h"

extern ControllerMapping gControllerMapping;

extern u8 gConfig_done;
extern u8 gButtonRemappingConfigDone;

extern u16 gHIDAttached;
extern u16 gHIDCurrentDevice;
extern HIDClient gHIDClient;

extern u16 gHIDRegisteredDevices;

extern HID_DEVICE_DATA gHID_Devices[gHIDMaxDevices];

extern u8 gHID_Mouse_Mode;

extern u32 gGamePadValues[CONTRPS_MAX_VALUE];

extern u8 config_controller[gHIDMaxDevices][CONTRPS_MAX_VALUE][2];
extern u16 config_controller_hidmask[gHIDMaxDevices];

extern u16 gHID_LIST_GC;
extern u16 gHID_LIST_DS3;
extern u16 gHID_LIST_KEYBOARD;
extern u16 gHID_LIST_MOUSE;
extern u16 gGamePadSlot;

extern u16 gHID_SLOT_GC;
extern u16 gHID_SLOT_KEYBOARD;
extern u16 gMouseSlot;

extern u8 gOriginalDimState;
extern u8 gOriginalAPDState;

extern u16 gNetworkController[gHIDMaxDevices][HID_MAX_PADS_COUNT][4];
extern s32 gHIDNetworkClientID;

#endif // CP_RETAINS_VARS_H_
