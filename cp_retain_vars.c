/****************************************************************************
 * Copyright (C) 2016 Maschell
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

#include <gctypes.h>
#include "controller_patcher.h"
#include "cp_retain_vars.h"

u8 gConfig_done __attribute__((section(".data"))) = 0;
u8 gButtonRemappingConfigDone __attribute__((section(".data"))) = 0;

u8 gHIDSetupDone __attribute__((section(".data"))) = 0;
u16 gHIDAttached __attribute__((section(".data"))) = 0;
u16 gHIDCurrentDevice __attribute__((section(".data"))) = 0;

u16 gHIDRegisteredDevices __attribute__((section(".data"))) = 0;

HIDClient gHIDClient __attribute__((section(".data")));

HID_DEVICE_DATA gHID_Devices[gHIDMaxDevices] __attribute__((section(".data")));

HID_Mouse gHID_Mouse __attribute__((section(".data")));

u8 gHID_Mouse_Mode __attribute__((section(".data"))) = HID_MOUSE_MODE_TOUCH;

u32 gGamePadValues[CONTRPS_MAX_VALUE] __attribute__((section(".data")));

u8 config_controller[gHIDMaxDevices][CONTRPS_MAX_VALUE][2] __attribute__((section(".data")));
u16 config_controller_list[gHIDMaxDevices] __attribute__((section(".data")));
u32 config_controller_data_ptr[gHIDMaxDevices][HID_MAX_PADS_COUNT] __attribute__((section(".data")));

u16 gHID_LIST_GC __attribute__((section(".data"))) = 0;
u16 gHID_LIST_DS3 __attribute__((section(".data"))) = 0;
u16 gHID_LIST_KEYBOARD __attribute__((section(".data"))) = 0;
u16 gHID_LIST_MOUSE __attribute__((section(".data"))) = 0;

u16 gGamePadSlot __attribute__((section(".data"))) = 0;
u16 gHID_SLOT_GC __attribute__((section(".data"))) = 0;
u16 gHID_SLOT_KEYBOARD __attribute__((section(".data"))) = 0;
u16 gMouseSlot __attribute__((section(".data"))) = 0;
