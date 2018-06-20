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

#include "../ControllerPatcherIncludes.hpp"

ControllerMapping gControllerMapping __attribute__((section(".data")));

uint8_t gConfig_done __attribute__((section(".data"))) = 0;
uint8_t gButtonRemappingConfigDone __attribute__((section(".data"))) = 0;

uint32_t gHIDAttached __attribute__((section(".data"))) = 0;
uint32_t gHIDCurrentDevice __attribute__((section(".data"))) = 0;

uint16_t gHIDRegisteredDevices __attribute__((section(".data"))) = 0;

HIDClient gHIDClient __attribute__((section(".data")));

HID_DEVICE_DATA gHID_Devices[gHIDMaxDevices] __attribute__((section(".data")));

uint8_t gHID_Mouse_Mode __attribute__((section(".data"))) = HID_MOUSE_MODE_TOUCH;

uint8_t gMouseModeCoolDown __attribute__((section(".data"))) = 0;

uint32_t gGamePadValues[CONTRPS_MAX_VALUE] __attribute__((section(".data")));

uint8_t  config_controller[gHIDMaxDevices][CONTRPS_MAX_VALUE][2] __attribute__((section(".data")));
uint32_t config_controller_hidmask[gHIDMaxDevices] __attribute__((section(".data")));

uint32_t gHID_LIST_GC __attribute__((section(".data"))) = 0;
uint32_t gHID_LIST_DS3 __attribute__((section(".data"))) = 0;
uint32_t gHID_LIST_DS4 __attribute__((section(".data"))) = 0;
uint32_t gHID_LIST_KEYBOARD __attribute__((section(".data"))) = 0;
uint32_t gHID_LIST_SWITCH_PRO __attribute__((section(".data"))) = 0;
uint32_t gHID_LIST_MOUSE __attribute__((section(".data"))) = 0;

uint16_t gGamePadSlot __attribute__((section(".data"))) = 0;
uint16_t gHID_SLOT_GC __attribute__((section(".data"))) = 0;
uint16_t gHID_SLOT_KEYBOARD __attribute__((section(".data"))) = 0;
uint16_t gMouseSlot __attribute__((section(".data"))) = 0;

uint8_t gOriginalDimState __attribute__((section(".data"))) = 0;
uint8_t gOriginalAPDState __attribute__((section(".data"))) = 0;

uint16_t gNetworkController[gHIDMaxDevices][HID_MAX_PADS_COUNT][4] __attribute__((section(".data")));
int32_t gHIDNetworkClientID __attribute__((section(".data"))) = 0;
uint8_t gUsedProtocolVersion  __attribute__((section(".data"))) = WIIU_CP_TCP_HANDSHAKE;

WPADConnectCallback gWPADConnectCallback[4] __attribute__((section(".data")));
WPADConnectCallback gKPADConnectCallback[4] __attribute__((section(".data")));
WPADExtensionCallback gExtensionCallback[4] __attribute__((section(".data")));
WPADSamplingCallback gSamplingCallback __attribute__((section(".data"))) = 0;
uint8_t gCallbackCooldown __attribute__((section(".data"))) = 0;
uint8_t gNetworkControllerActivated __attribute__((section(".data"))) = 1;

uint8_t gGlobalRumbleActivated __attribute__((section(".data"))) = 0;

uint32_t gUDPClientip __attribute__((section(".data"))) = 0;
ControllerMappingPADInfo* gProPadInfo[4] __attribute__((section(".data"))) = {&gControllerMapping.proController[0].pad_infos[0],
                                                                              &gControllerMapping.proController[1].pad_infos[0],
                                                                              &gControllerMapping.proController[2].pad_infos[0],
                                                                              &gControllerMapping.proController[3].pad_infos[0]
                                                                             } ;
