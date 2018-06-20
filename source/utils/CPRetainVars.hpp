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

#include <controller_patcher/ControllerPatcherDefs.h>
#include <nsyshid/hid.h>

extern ControllerMapping gControllerMapping;

extern uint8_t gConfig_done;
extern uint8_t gButtonRemappingConfigDone;

extern uint32_t gHIDAttached;
extern uint32_t gHIDCurrentDevice;
extern HIDClient gHIDClient;

extern uint16_t gHIDRegisteredDevices;

extern HID_DEVICE_DATA gHID_Devices[gHIDMaxDevices];

extern uint8_t gHID_Mouse_Mode;

extern uint8_t gMouseModeCoolDown;

extern uint32_t gGamePadValues[CONTRPS_MAX_VALUE];

extern uint8_t config_controller[gHIDMaxDevices][CONTRPS_MAX_VALUE][2];
extern uint32_t config_controller_hidmask[gHIDMaxDevices];

extern uint32_t gHID_LIST_GC;
extern uint32_t gHID_LIST_DS3;
extern uint32_t gHID_LIST_DS4;
extern uint32_t gHID_LIST_KEYBOARD;
extern uint32_t gHID_LIST_SWITCH_PRO;
extern uint32_t gHID_LIST_MOUSE;
extern uint16_t gGamePadSlot;

extern uint16_t gHID_SLOT_GC;
extern uint16_t gHID_SLOT_KEYBOARD;
extern uint16_t gMouseSlot;

extern uint8_t gOriginalDimState;
extern uint8_t gOriginalAPDState;

extern uint16_t gNetworkController[gHIDMaxDevices][HID_MAX_PADS_COUNT][4];
extern int32_t gHIDNetworkClientID;

extern uint8_t gUsedProtocolVersion;

extern WPADConnectCallback gWPADConnectCallback[4];
extern WPADConnectCallback gKPADConnectCallback[4];
extern WPADExtensionCallback gExtensionCallback[4];
extern WPADSamplingCallback gSamplingCallback;
extern uint8_t gCallbackCooldown;
extern uint8_t gGlobalRumbleActivated;
extern uint8_t gNetworkControllerActivated;

extern uint32_t gUDPClientip;
extern ControllerMappingPADInfo* gProPadInfo[4];

#endif // CP_RETAINS_VARS_H_
