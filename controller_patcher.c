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

#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "controller_patcher.h"
#include "pad_const.h"
#include "dynamic_libs/os_functions.h"
#include "dynamic_libs/sys_functions.h"
#include "dynamic_libs/syshid_functions.h"
#include "dynamic_libs/socket_functions.h"
#include "dynamic_libs/padscore_functions.h"
#include "cp_retain_vars.h"
#include "utils/logger.h"

static u32 last_emulatedSticks = 0;
static u32 last_button_hold = 0;

void setConfigValue(u8 * dest, u8 first, u8 second){
    dest[0] = first;
    dest[1] = second;
}

void init_button_remapping(){
    if(HID_DEBUG) log_printf("init_button_remapping! called! \n");
    if(!gButtonRemappingConfigDone){
        if(HID_DEBUG) log_printf("init_button_remapping! Remapping is running! \n");
        gButtonRemappingConfigDone = 1;
        memset(gGamePadValues,0,sizeof(gGamePadValues)); // Init / Invalid everything

        gGamePadValues[CONTRPS_VPAD_BUTTON_A] =                 VPAD_BUTTON_A;
        gGamePadValues[CONTRPS_VPAD_BUTTON_B] =                 VPAD_BUTTON_B;
        gGamePadValues[CONTRPS_VPAD_BUTTON_X] =                 VPAD_BUTTON_X;
        gGamePadValues[CONTRPS_VPAD_BUTTON_Y] =                 VPAD_BUTTON_Y;
        gGamePadValues[CONTRPS_VPAD_BUTTON_LEFT] =              VPAD_BUTTON_LEFT;
        gGamePadValues[CONTRPS_VPAD_BUTTON_RIGHT] =             VPAD_BUTTON_RIGHT;
        gGamePadValues[CONTRPS_VPAD_BUTTON_UP] =                VPAD_BUTTON_UP;
        gGamePadValues[CONTRPS_VPAD_BUTTON_DOWN] =              VPAD_BUTTON_DOWN;
        gGamePadValues[CONTRPS_VPAD_BUTTON_ZL] =                VPAD_BUTTON_ZL;
        gGamePadValues[CONTRPS_VPAD_BUTTON_ZR] =                VPAD_BUTTON_ZR;
        gGamePadValues[CONTRPS_VPAD_BUTTON_L] =                 VPAD_BUTTON_L;
        gGamePadValues[CONTRPS_VPAD_BUTTON_R] =                 VPAD_BUTTON_R;
        gGamePadValues[CONTRPS_VPAD_BUTTON_PLUS] =              VPAD_BUTTON_PLUS;
        gGamePadValues[CONTRPS_VPAD_BUTTON_MINUS] =             VPAD_BUTTON_MINUS;
        gGamePadValues[CONTRPS_VPAD_BUTTON_HOME] =              VPAD_BUTTON_HOME;
        gGamePadValues[CONTRPS_VPAD_BUTTON_SYNC] =              VPAD_BUTTON_SYNC;
        gGamePadValues[CONTRPS_VPAD_BUTTON_STICK_R] =           VPAD_BUTTON_STICK_R;
        gGamePadValues[CONTRPS_VPAD_BUTTON_STICK_L] =           VPAD_BUTTON_STICK_L;
        gGamePadValues[CONTRPS_VPAD_BUTTON_TV] =                VPAD_BUTTON_TV;


        //TODO: Sticks

        /*
            I can't think of any reason why somebody should need this? I think its just confusing.

            gGamePadValues[CONTRPS_VPAD_STICK_R_EMULATION_LEFT] =   VPAD_STICK_R_EMULATION_LEFT;
            gGamePadValues[CONTRPS_VPAD_STICK_R_EMULATION_RIGHT] =  VPAD_STICK_R_EMULATION_RIGHT;
            gGamePadValues[CONTRPS_VPAD_STICK_R_EMULATION_UP] =     VPAD_STICK_R_EMULATION_UP;
            gGamePadValues[CONTRPS_VPAD_STICK_R_EMULATION_DOWN] =   VPAD_STICK_R_EMULATION_DOWN;
            gGamePadValues[CONTRPS_VPAD_STICK_L_EMULATION_LEFT] =   VPAD_STICK_L_EMULATION_LEFT;
            gGamePadValues[CONTRPS_VPAD_STICK_L_EMULATION_RIGHT] =  VPAD_STICK_L_EMULATION_RIGHT;
            gGamePadValues[CONTRPS_VPAD_STICK_L_EMULATION_UP] =     VPAD_STICK_L_EMULATION_UP;
            gGamePadValues[CONTRPS_VPAD_STICK_L_EMULATION_DOWN] =   VPAD_STICK_L_EMULATION_DOWN;

        */
    }
}

void init_config_controller(){
    InitOSFunctionPointers();
    InitSocketFunctionPointers();
    InitSysHIDFunctionPointers();
    InitVPadFunctionPointers();
    InitPadScoreFunctionPointers();
    if(HID_DEBUG) log_printf("init_config_controller called! \n");
    if(!gConfig_done){
        if(HID_DEBUG) log_printf("init_config_controller! Config is running! \n");
        gConfig_done = 1;
        memset(config_controller,CONTROLLER_PATCHER_INVALIDVALUE,sizeof(config_controller)); // Init / Invalid everything
        memset(config_controller_list,0,sizeof(config_controller_list)); // Init / Invalid everything
        memset(config_controller_data_ptr,0,sizeof(config_controller_data_ptr)); // Init / Invalid everything

        gGamePadSlot = gHIDRegisteredDevices;
        u32 gGamePadHid = getNextDeviceSlot();
        if(HID_DEBUG) log_printf("gGamePadHid %s slot: %d\n",byte_to_binary(gGamePadHid),gGamePadSlot);

        gMouseSlot = gHIDRegisteredDevices;
        gHID_LIST_MOUSE = getNextDeviceSlot();
        if(HID_DEBUG) log_printf("gHID_LIST_MOUSE %s slot: %d\n",byte_to_binary(gHID_LIST_MOUSE),gMouseSlot);

        u32 gc_slot = gHIDRegisteredDevices;
        u32 gc_hid = getNextDeviceSlot();
        gHID_LIST_GC = gc_hid;
        gHID_SLOT_GC = gc_slot;
        if(HID_DEBUG) log_printf("gHID_LIST_GC %s slot: %d\n",byte_to_binary(gHID_LIST_GC),gHID_SLOT_GC);

        u32 ds3_slot = gHIDRegisteredDevices;
        u32 ds3_hid = getNextDeviceSlot();
        gHID_LIST_DS3 = ds3_hid;

        if(HID_DEBUG) log_printf("gHID_LIST_DS3 %s slot: %d\n",byte_to_binary(gHID_LIST_DS3),ds3_slot);

        u32 ds4_slot = gHIDRegisteredDevices;
        u32 ds4_hid = getNextDeviceSlot();

        if(HID_DEBUG) log_printf("ds4_hid %s slot: %d\n",byte_to_binary(ds4_hid),ds4_slot);

        /*
        u32 snes_slot = gHIDRegisteredDevices;
        u32 snes_hid = getNextDeviceSlot();

        log_printf("snes_hid %s slot: %d\n",byte_to_binary(snes_hid),snes_slot);*/

        u32 keyboard_slot = gHIDRegisteredDevices;
        u32 keyboard_hid = getNextDeviceSlot();
        gHID_LIST_KEYBOARD = keyboard_hid;
        gHID_SLOT_KEYBOARD = keyboard_slot;

        if(HID_DEBUG) log_printf("gHID_LIST_KEYBOARD %s slot: %d\n",byte_to_binary(gHID_LIST_KEYBOARD),gHID_SLOT_KEYBOARD);

        config_controller_list[gc_slot] =                                                           gHID_LIST_GC;
        config_controller_list[ds3_slot] =                                                          gHID_LIST_DS3;
        config_controller_list[ds4_slot] =                                                          ds4_hid;
        config_controller_list[keyboard_slot] =                                                     gHID_LIST_KEYBOARD;

        //Set data for each pad. Currenty 4 Pads for each device support. May need other extra treatment
        config_controller_data_ptr[gHID_SLOT_GC][0] =                                                (u32)&(gHID_Devices[gHID_SLOT_GC]).pad_data[0]; //GC Pad Port 1
        config_controller_data_ptr[gHID_SLOT_GC][1] =                                                (u32)&(gHID_Devices[gHID_SLOT_GC]).pad_data[1]; //GC Pad Port 2
        config_controller_data_ptr[gHID_SLOT_GC][2] =                                                (u32)&(gHID_Devices[gHID_SLOT_GC]).pad_data[2]; //GC Pad Port 3
        config_controller_data_ptr[gHID_SLOT_GC][3] =                                                (u32)&(gHID_Devices[gHID_SLOT_GC]).pad_data[3]; //GC Pad Port 4
        config_controller_data_ptr[ds3_slot][0] =                                                    (u32)&(gHID_Devices[ds3_slot]).pad_data[0];
        config_controller_data_ptr[ds3_slot][1] =                                                    (u32)&(gHID_Devices[ds3_slot]).pad_data[0];
        config_controller_data_ptr[ds3_slot][2] =                                                    (u32)&(gHID_Devices[ds3_slot]).pad_data[0];
        config_controller_data_ptr[ds3_slot][3] =                                                    (u32)&(gHID_Devices[ds3_slot]).pad_data[0];
        config_controller_data_ptr[ds4_slot][0] =                                                    (u32)&(gHID_Devices[ds4_slot]).pad_data[0];
        config_controller_data_ptr[ds4_slot][1] =                                                    (u32)&(gHID_Devices[ds4_slot]).pad_data[0];
        config_controller_data_ptr[ds4_slot][2] =                                                    (u32)&(gHID_Devices[ds4_slot]).pad_data[0];
        config_controller_data_ptr[ds4_slot][3] =                                                    (u32)&(gHID_Devices[ds4_slot]).pad_data[0];
        config_controller_data_ptr[gHID_SLOT_KEYBOARD][0] =                                          (u32)&(gHID_Devices[gHID_SLOT_KEYBOARD]).pad_data[0];
        config_controller_data_ptr[gHID_SLOT_KEYBOARD][1] =                                          (u32)&(gHID_Devices[gHID_SLOT_KEYBOARD]).pad_data[0];
        config_controller_data_ptr[gHID_SLOT_KEYBOARD][2] =                                          (u32)&(gHID_Devices[gHID_SLOT_KEYBOARD]).pad_data[0];
        config_controller_data_ptr[gHID_SLOT_KEYBOARD][3] =                                          (u32)&(gHID_Devices[gHID_SLOT_KEYBOARD]).pad_data[0];

        //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        //! GamePad
        //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        setConfigValue((u8*)&config_controller[gGamePadSlot][CONTRPS_VID],                            0xAF,0xFE);
        setConfigValue((u8*)&config_controller[gGamePadSlot][CONTRPS_PID],                            0xAA,0xAA);

        //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        //! Mouse
        //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        setConfigValue((u8*)&config_controller[gMouseSlot][CONTRPS_VID],                              0xAF,0xFE);
        setConfigValue((u8*)&config_controller[gMouseSlot][CONTRPS_PID],                              0xAA,0xAB);

        setConfigValue((u8*)&config_controller[gMouseSlot][CONTRPS_VPAD_BUTTON_LEFT],                 CONTROLLER_PATCHER_VALUE_SET,       CONTRPS_VPAD_BUTTON_ZR);
        setConfigValue((u8*)&config_controller[gMouseSlot][CONTRPS_VPAD_BUTTON_RIGHT],                CONTROLLER_PATCHER_VALUE_SET,       CONTRPS_VPAD_BUTTON_R);


        //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        //! GC-Adapter
        //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VID],                            0x05,0x7e);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_PID],                            0x03,0x37);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_A],                  HID_GC_BUTTON_A[0],           HID_GC_BUTTON_A[1]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_B],                  HID_GC_BUTTON_B[0],           HID_GC_BUTTON_B[1]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_X],                  HID_GC_BUTTON_X[0],           HID_GC_BUTTON_X[1]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_Y],                  HID_GC_BUTTON_Y[0],           HID_GC_BUTTON_Y[1]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_DPAD_MODE],                      CONTROLLER_PATCHER_VALUE_SET, HID_GC_BUTTON_DPAD_TYPE[CONTRDPAD_MODE]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_DPAD_MASK],                      CONTROLLER_PATCHER_VALUE_SET, HID_GC_BUTTON_DPAD_TYPE[CONTRDPAD_MASK]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_LEFT],               HID_GC_BUTTON_LEFT[0],        HID_GC_BUTTON_LEFT[1]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_RIGHT],              HID_GC_BUTTON_RIGHT[0],       HID_GC_BUTTON_RIGHT[1]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_DOWN],               HID_GC_BUTTON_DOWN[0],        HID_GC_BUTTON_DOWN[1]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_UP],                 HID_GC_BUTTON_UP[0],          HID_GC_BUTTON_UP[1]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_MINUS],              HID_GC_BUTTON_START[0],       HID_GC_BUTTON_START[1]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_L],                  HID_GC_BUTTON_L[0],           HID_GC_BUTTON_L[1]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_R],                  HID_GC_BUTTON_R[0],           HID_GC_BUTTON_R[1]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_PLUS],               HID_GC_BUTTON_START[0],       HID_GC_BUTTON_START[1]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_ZL],                 HID_GC_BUTTON_L[0],           HID_GC_BUTTON_L[1]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_ZR],                 HID_GC_BUTTON_R[0],           HID_GC_BUTTON_R[1]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_STICK_L],            HID_GC_BUTTON_A[0],           HID_GC_BUTTON_A[1]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_STICK_R],            HID_GC_BUTTON_B[0],           HID_GC_BUTTON_B[1]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_DOUBLE_USE],                     CONTROLLER_PATCHER_VALUE_SET, CONTROLLER_PATCHER_GC_DOUBLE_USE);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_DOUBLE_USE_BUTTON_ACTIVATOR],    HID_GC_BUTTON_Z[0],           HID_GC_BUTTON_Z[1]);

        //Buttons that will be ignored when the CONTRPS_DOUBLE_USE_BUTTON_ACTIVATOR is pressed
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_DOUBLE_USE_BUTTON_1_PRESSED],    CONTROLLER_PATCHER_VALUE_SET, CONTRPS_VPAD_BUTTON_MINUS);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_DOUBLE_USE_BUTTON_2_PRESSED],    CONTROLLER_PATCHER_VALUE_SET, CONTRPS_VPAD_BUTTON_L);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_DOUBLE_USE_BUTTON_3_PRESSED],    CONTROLLER_PATCHER_VALUE_SET, CONTRPS_VPAD_BUTTON_R);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_DOUBLE_USE_BUTTON_4_PRESSED],    CONTROLLER_PATCHER_VALUE_SET, CONTRPS_VPAD_BUTTON_STICK_L);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_DOUBLE_USE_BUTTON_5_PRESSED],    CONTROLLER_PATCHER_VALUE_SET, CONTRPS_VPAD_BUTTON_STICK_R);

         //Buttons that will be ignored when the CONTRPS_DOUBLE_USE_BUTTON_ACTIVATOR is released
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_DOUBLE_USE_BUTTON_1_RELEASED],   CONTROLLER_PATCHER_VALUE_SET, CONTRPS_VPAD_BUTTON_PLUS);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_DOUBLE_USE_BUTTON_2_RELEASED],   CONTROLLER_PATCHER_VALUE_SET, CONTRPS_VPAD_BUTTON_ZL);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_DOUBLE_USE_BUTTON_3_RELEASED],   CONTROLLER_PATCHER_VALUE_SET, CONTRPS_VPAD_BUTTON_ZR);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_DOUBLE_USE_BUTTON_4_RELEASED],   CONTROLLER_PATCHER_VALUE_SET, CONTRPS_VPAD_BUTTON_A);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_DOUBLE_USE_BUTTON_5_RELEASED],   CONTROLLER_PATCHER_VALUE_SET, CONTRPS_VPAD_BUTTON_B);

        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_PAD_COUNT],                      CONTROLLER_PATCHER_VALUE_SET,HID_GC_PAD_COUNT);

        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_L_STICK_X],          HID_GC_STICK_L_X[STICK_CONF_BYTE],HID_GC_STICK_L_X[STICK_CONF_DEFAULT]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_L_STICK_X_DEADZONE], CONTROLLER_PATCHER_VALUE_SET,     HID_GC_STICK_L_X[STICK_CONF_DEADZONE]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_L_STICK_X_MINMAX],   HID_GC_STICK_L_X[STICK_CONF_MIN], HID_GC_STICK_L_X[STICK_CONF_MAX]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_L_STICK_X_INVERT],   CONTROLLER_PATCHER_VALUE_SET,     HID_GC_STICK_L_X[STICK_CONF_INVERT]);

        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_L_STICK_Y],          HID_GC_STICK_L_Y[STICK_CONF_BYTE],HID_GC_STICK_L_Y[STICK_CONF_DEFAULT]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_L_STICK_Y_DEADZONE], CONTROLLER_PATCHER_VALUE_SET,     HID_GC_STICK_L_Y[STICK_CONF_DEADZONE]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_L_STICK_Y_MINMAX],   HID_GC_STICK_L_Y[STICK_CONF_MIN], HID_GC_STICK_L_Y[STICK_CONF_MAX]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_L_STICK_Y_INVERT],   CONTROLLER_PATCHER_VALUE_SET,     HID_GC_STICK_L_Y[STICK_CONF_INVERT]);

        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_R_STICK_X],          HID_GC_STICK_R_X[STICK_CONF_BYTE],HID_GC_STICK_R_X[STICK_CONF_DEFAULT]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_R_STICK_X_DEADZONE], CONTROLLER_PATCHER_VALUE_SET,     HID_GC_STICK_R_X[STICK_CONF_DEADZONE]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_R_STICK_X_MINMAX],   HID_GC_STICK_R_X[STICK_CONF_MIN], HID_GC_STICK_R_X[STICK_CONF_MAX]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_R_STICK_X_INVERT],   CONTROLLER_PATCHER_VALUE_SET,     HID_GC_STICK_R_X[STICK_CONF_INVERT]);

        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_R_STICK_Y],          HID_GC_STICK_R_Y[STICK_CONF_BYTE],HID_GC_STICK_R_Y[STICK_CONF_DEFAULT]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_R_STICK_Y_DEADZONE], CONTROLLER_PATCHER_VALUE_SET,     HID_GC_STICK_R_Y[STICK_CONF_DEADZONE]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_R_STICK_Y_MINMAX],   HID_GC_STICK_R_Y[STICK_CONF_MIN], HID_GC_STICK_R_Y[STICK_CONF_MAX]);
        setConfigValue((u8*)&config_controller[gHID_SLOT_GC][CONTRPS_VPAD_BUTTON_R_STICK_Y_INVERT],   CONTROLLER_PATCHER_VALUE_SET,     HID_GC_STICK_R_Y[STICK_CONF_INVERT]);

        //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        //! DS3
        //!---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VID],                           0x05,0x4c);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_PID],                           0x02,0x68);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_BUF_SIZE],                      CONTROLLER_PATCHER_VALUE_SET,128);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_A],                 HID_DS3_BUTTON_CIRCLE[0],  HID_DS3_BUTTON_CIRCLE[1]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_B],                 HID_DS3_BUTTON_CROSS[0],   HID_DS3_BUTTON_CROSS[1]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_X],                 HID_DS3_BUTTON_TRIANGLE[0],HID_DS3_BUTTON_TRIANGLE[1]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_Y],                 HID_DS3_BUTTON_SQUARE[0],  HID_DS3_BUTTON_SQUARE[1]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_DPAD_MODE],                     CONTROLLER_PATCHER_VALUE_SET,HID_DS3_BUTTON_DPAD_TYPE[CONTRDPAD_MODE]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_DPAD_MASK],                     CONTROLLER_PATCHER_VALUE_SET,HID_DS3_BUTTON_DPAD_TYPE[CONTRDPAD_MASK]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_LEFT],              HID_DS3_BUTTON_LEFT[0],    HID_DS3_BUTTON_LEFT[1]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_RIGHT],             HID_DS3_BUTTON_RIGHT[0],   HID_DS3_BUTTON_RIGHT[1]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_DOWN],              HID_DS3_BUTTON_DOWN[0],    HID_DS3_BUTTON_DOWN[1]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_UP],                HID_DS3_BUTTON_UP[0],      HID_DS3_BUTTON_UP[1]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_PLUS],              HID_DS3_BUTTON_START[0],   HID_DS3_BUTTON_START[1]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_MINUS],             HID_DS3_BUTTON_SELECT[0],  HID_DS3_BUTTON_SELECT[1]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_L],                 HID_DS3_BUTTON_L1[0],      HID_DS3_BUTTON_L1[1]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_R],                 HID_DS3_BUTTON_R1[0],      HID_DS3_BUTTON_R1[1]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_ZL],                HID_DS3_BUTTON_L2[0],      HID_DS3_BUTTON_L2[1]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_ZR],                HID_DS3_BUTTON_R2[0],      HID_DS3_BUTTON_R2[1]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_STICK_L],           HID_DS3_BUTTON_L3[0],      HID_DS3_BUTTON_L3[1]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_STICK_R],           HID_DS3_BUTTON_R3[0],      HID_DS3_BUTTON_R3[1]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_HOME],              HID_DS3_BUTTON_GUIDE[0],   HID_DS3_BUTTON_GUIDE[1]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_PAD_COUNT],                     CONTROLLER_PATCHER_VALUE_SET,HID_DS3_PAD_COUNT);

        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_L_STICK_X],          HID_DS3_STICK_L_X[STICK_CONF_BYTE],HID_DS3_STICK_L_X[STICK_CONF_DEFAULT]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_L_STICK_X_DEADZONE], CONTROLLER_PATCHER_VALUE_SET,      HID_DS3_STICK_L_X[STICK_CONF_DEADZONE]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_L_STICK_X_MINMAX],   HID_DS3_STICK_L_X[STICK_CONF_MIN], HID_DS3_STICK_L_X[STICK_CONF_MAX]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_L_STICK_X_INVERT],   CONTROLLER_PATCHER_VALUE_SET,      HID_DS3_STICK_L_X[STICK_CONF_INVERT]);


        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_L_STICK_X],          HID_DS3_STICK_L_Y[STICK_CONF_BYTE],HID_DS3_STICK_L_Y[STICK_CONF_DEFAULT]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_L_STICK_X_DEADZONE], CONTROLLER_PATCHER_VALUE_SET,      HID_DS3_STICK_L_Y[STICK_CONF_DEADZONE]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_L_STICK_X_MINMAX],   HID_DS3_STICK_L_Y[STICK_CONF_MIN], HID_DS3_STICK_L_Y[STICK_CONF_MAX]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_L_STICK_Y_INVERT],   CONTROLLER_PATCHER_VALUE_SET,      HID_DS3_STICK_L_Y[STICK_CONF_INVERT]);


        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_R_STICK_X],          HID_DS3_STICK_R_X[STICK_CONF_BYTE],HID_DS3_STICK_R_X[STICK_CONF_DEFAULT]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_R_STICK_X_DEADZONE], CONTROLLER_PATCHER_VALUE_SET,      HID_DS3_STICK_R_X[STICK_CONF_DEADZONE]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_R_STICK_X_MINMAX],   HID_DS3_STICK_R_X[STICK_CONF_MIN], HID_DS3_STICK_R_X[STICK_CONF_MAX]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_R_STICK_X_INVERT],   CONTROLLER_PATCHER_VALUE_SET,      HID_DS3_STICK_R_X[STICK_CONF_INVERT]);


        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_R_STICK_Y],          HID_DS3_STICK_R_Y[STICK_CONF_BYTE],HID_DS3_STICK_R_Y[STICK_CONF_DEFAULT]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_R_STICK_Y_DEADZONE], CONTROLLER_PATCHER_VALUE_SET,      HID_DS3_STICK_R_Y[STICK_CONF_DEADZONE]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_R_STICK_Y_MINMAX],   HID_DS3_STICK_R_Y[STICK_CONF_MIN], HID_DS3_STICK_R_Y[STICK_CONF_MAX]);
        setConfigValue((u8*)&config_controller[ds3_slot][CONTRPS_VPAD_BUTTON_R_STICK_Y_INVERT],   CONTROLLER_PATCHER_VALUE_SET,      HID_DS3_STICK_R_Y[STICK_CONF_INVERT]);

        //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        //! DS4
        //!---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VID],                           0x05,0x4c);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_PID],                           0x05,0xc4);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_BUF_SIZE],                      CONTROLLER_PATCHER_VALUE_SET,128);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_A],                 HID_DS4_BUTTON_CIRCLE[0],          HID_DS4_BUTTON_CIRCLE[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_B],                 HID_DS4_BUTTON_CROSS[0],           HID_DS4_BUTTON_CROSS[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_X],                 HID_DS4_BUTTON_TRIANGLE[0],        HID_DS4_BUTTON_TRIANGLE[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_Y],                 HID_DS4_BUTTON_SQUARE[0],          HID_DS4_BUTTON_SQUARE[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_DPAD_MODE],                     CONTROLLER_PATCHER_VALUE_SET,      HID_DS4_BUTTON_DPAD_TYPE[CONTRDPAD_MODE]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_DPAD_MASK],                     CONTROLLER_PATCHER_VALUE_SET,      HID_DS4_BUTTON_DPAD_TYPE[CONTRDPAD_MASK]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_DPAD_N],            HID_DS4_BUTTON_DPAD_N[0],          HID_DS4_BUTTON_DPAD_N[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_DPAD_NE],           HID_DS4_BUTTON_DPAD_NE[0],         HID_DS4_BUTTON_DPAD_NE[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_DPAD_E],            HID_DS4_BUTTON_DPAD_E[0],          HID_DS4_BUTTON_DPAD_E[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_DPAD_SE],           HID_DS4_BUTTON_DPAD_SE[0],         HID_DS4_BUTTON_DPAD_SE[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_DPAD_S],            HID_DS4_BUTTON_DPAD_S[0],          HID_DS4_BUTTON_DPAD_S[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_DPAD_SW],           HID_DS4_BUTTON_DPAD_SW[0],         HID_DS4_BUTTON_DPAD_SW[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_DPAD_W],            HID_DS4_BUTTON_DPAD_W[0],          HID_DS4_BUTTON_DPAD_W[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_DPAD_NW],           HID_DS4_BUTTON_DPAD_NW[0],         HID_DS4_BUTTON_DPAD_NW[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_DPAD_NEUTRAL],      HID_DS4_BUTTON_DPAD_NEUTRAL[0],    HID_DS4_BUTTON_DPAD_NEUTRAL[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_PLUS],              HID_DS4_BUTTON_OPTIONS[0],         HID_DS4_BUTTON_OPTIONS[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_MINUS],             HID_DS4_BUTTON_SHARE[0],           HID_DS4_BUTTON_SHARE[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_L],                 HID_DS4_BUTTON_L1[0],              HID_DS4_BUTTON_L1[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_R],                 HID_DS4_BUTTON_R1[0],              HID_DS4_BUTTON_R1[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_ZL],                HID_DS4_BUTTON_L2[0],              HID_DS4_BUTTON_L2[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_ZR],                HID_DS4_BUTTON_R2[0],              HID_DS4_BUTTON_R2[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_STICK_L],           HID_DS4_BUTTON_L3[0],              HID_DS4_BUTTON_L3[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_STICK_R],           HID_DS4_BUTTON_R3[0],              HID_DS4_BUTTON_R3[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_HOME],              HID_DS4_BUTTON_GUIDE[0],           HID_DS4_BUTTON_GUIDE[1]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_PAD_COUNT],                     CONTROLLER_PATCHER_VALUE_SET,HID_DS4_PAD_COUNT);


        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_L_STICK_X],          HID_DS4_STICK_L_X[STICK_CONF_BYTE],HID_DS4_STICK_L_X[STICK_CONF_DEFAULT]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_L_STICK_X_DEADZONE], CONTROLLER_PATCHER_VALUE_SET,      HID_DS4_STICK_L_X[STICK_CONF_DEADZONE]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_L_STICK_X_MINMAX],   HID_DS4_STICK_L_X[STICK_CONF_MIN], HID_DS4_STICK_L_X[STICK_CONF_MAX]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_L_STICK_X_INVERT],   CONTROLLER_PATCHER_VALUE_SET,      HID_DS4_STICK_L_X[STICK_CONF_INVERT]);

        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_L_STICK_Y],          HID_DS4_STICK_L_Y[STICK_CONF_BYTE],HID_DS4_STICK_L_Y[STICK_CONF_DEFAULT]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_L_STICK_Y_DEADZONE], CONTROLLER_PATCHER_VALUE_SET,      HID_DS4_STICK_L_Y[STICK_CONF_DEADZONE]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_L_STICK_Y_MINMAX],   HID_DS4_STICK_L_Y[STICK_CONF_MIN], HID_DS4_STICK_L_Y[STICK_CONF_MAX]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_L_STICK_Y_INVERT],   CONTROLLER_PATCHER_VALUE_SET,      HID_DS4_STICK_L_Y[STICK_CONF_INVERT]);


        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_R_STICK_X],          HID_DS4_STICK_R_X[STICK_CONF_BYTE],HID_DS4_STICK_R_X[STICK_CONF_DEFAULT]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_R_STICK_X_DEADZONE], CONTROLLER_PATCHER_VALUE_SET,      HID_DS4_STICK_R_X[STICK_CONF_DEADZONE]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_R_STICK_X_MINMAX],   HID_DS4_STICK_R_X[STICK_CONF_MIN], HID_DS4_STICK_R_X[STICK_CONF_MAX]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_R_STICK_X_INVERT],   CONTROLLER_PATCHER_VALUE_SET,      HID_DS4_STICK_R_X[STICK_CONF_INVERT]);


        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_R_STICK_Y],          HID_DS4_STICK_R_Y[STICK_CONF_BYTE],HID_DS4_STICK_R_Y[STICK_CONF_DEFAULT]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_R_STICK_Y_DEADZONE], CONTROLLER_PATCHER_VALUE_SET,      HID_DS4_STICK_R_Y[STICK_CONF_DEADZONE]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_R_STICK_Y_MINMAX],   HID_DS4_STICK_R_Y[STICK_CONF_MIN], HID_DS4_STICK_R_Y[STICK_CONF_MAX]);
        setConfigValue((u8*)&config_controller[ds4_slot][CONTRPS_VPAD_BUTTON_R_STICK_Y_INVERT],   CONTROLLER_PATCHER_VALUE_SET,      HID_DS4_STICK_R_Y[STICK_CONF_INVERT]);


        //!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        //! Keyboard
        //!---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_VID],                           0xAF,0xFE);
        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_PID],                           0xAA,0xAC);
        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_VPAD_BUTTON_A],                 0x00,HID_KEYBOARD_BUTTON_E);
        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_VPAD_BUTTON_B],                 0x00,HID_KEYBOARD_BUTTON_Q);
        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_VPAD_BUTTON_X],                 0x00,HID_KEYBOARD_BUTTON_SPACEBAR);
        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_VPAD_BUTTON_Y],                 0x00,HID_KEYBOARD_BUTTON_R);
        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_DPAD_MODE],                     CONTROLLER_PATCHER_VALUE_SET,CONTRPDM_Normal);
        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_VPAD_BUTTON_LEFT],              0x00,HID_KEYBOARD_BUTTON_LEFT);
        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_VPAD_BUTTON_RIGHT],             0x00,HID_KEYBOARD_BUTTON_RIGHT);
        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_VPAD_BUTTON_DOWN],              0x00,HID_KEYBOARD_BUTTON_DOWN);
        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_VPAD_BUTTON_UP],                0x00,HID_KEYBOARD_BUTTON_UP);
        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_VPAD_BUTTON_PLUS],              0x00,HID_KEYBOARD_BUTTON_RETURN);
        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_VPAD_BUTTON_MINUS],             0x00,HID_KEYBOARD_KEYPAD_BUTTON_MINUS);
        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_VPAD_BUTTON_L],                 0x00,HID_KEYBOARD_BUTTON_V);
        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_VPAD_BUTTON_R],                 0x00,HID_KEYBOARD_BUTTON_B);
        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_VPAD_BUTTON_ZL],                0x00,HID_KEYBOARD_BUTTON_SHIFT);
        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_VPAD_BUTTON_ZR],                0x00,HID_KEYBOARD_BUTTON_N);
        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_VPAD_BUTTON_STICK_L],           0x00,HID_KEYBOARD_BUTTON_F);
        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_VPAD_BUTTON_STICK_R],           0x00,HID_KEYBOARD_BUTTON_TAB);

        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_VPAD_BUTTON_L_STICK_X],         HID_KEYBOARD_BUTTON_A,HID_KEYBOARD_BUTTON_D);
        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_VPAD_BUTTON_L_STICK_Y],         HID_KEYBOARD_BUTTON_W,HID_KEYBOARD_BUTTON_S);

        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_VPAD_BUTTON_R_STICK_X],         HID_KEYBOARD_KEYPAD_BUTTON_4,HID_KEYBOARD_KEYPAD_BUTTON_6);
        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_VPAD_BUTTON_R_STICK_Y],         HID_KEYBOARD_KEYPAD_BUTTON_8,HID_KEYBOARD_KEYPAD_BUTTON_2);

        setConfigValue((u8*)&config_controller[gHID_SLOT_KEYBOARD][CONTRPS_PAD_COUNT],                     CONTROLLER_PATCHER_VALUE_SET,HID_KEYBOARD_PAD_COUNT);

        /*for(int i = 0;i< gHIDMaxDevices;i++){
            for(int j = 0;j< CONTRPS_MAX_VALUE;j++){
                //log_printf("%d %d %02X%02X\n",i,j,config_controller[i][j][0],config_controller[i][j][1]);
            }
            //log_print("------------------\n");
        }*/
    }else{
        if(HID_DEBUG) log_print("Config already done!\n");
    }
    if(!gHIDSetupDone){
        HIDSetup();
        gHIDSetupDone = 1;
    }

    if(!gHIDAttached){
        HIDAddClient(&gHIDClient, my_attach_cb);
    }
}

void deinit_config_controller(){
    if(HID_DEBUG) log_printf("deinit_config_controller! called! \n");
    if(gHIDSetupDone){
        if(gHIDAttached) HIDDelClient(&gHIDClient);
        gHIDAttached = 0;
        gHIDSetupDone = 0;
        gHIDCurrentDevice = 0;
        //HIDTeardown(); // seems to result in a creash sometimes
        log_printf("Exit\n");
    }
    gButtonRemappingConfigDone = 0;
    last_button_hold = 0;
    last_emulatedSticks = 0;
}

u32 getNextDeviceSlot(){
    if(gHIDRegisteredDevices >= gHIDMaxDevices){
        return 1 << 17;
    }
    return (1 << (gHIDRegisteredDevices++));
}

#define SWAP16(x) ((x>>8) | ((x&0xFF)<<8))
#define SWAP8(x) ((x>>4) | ((x&0xF)<<4))
int my_attach_cb(HIDClient *p_client, HIDDevice *p_device, unsigned int attach)
{
    if(attach){
        log_printf("vid %04x pid %04x connected\n", SWAP16(p_device->vid),SWAP16(p_device->pid));
        if(HID_DEBUG) log_printf("interface index  %02x\n", p_device->interface_index);
        if(HID_DEBUG) log_printf("sub class        %02x\n", p_device->sub_class);
        if(HID_DEBUG) log_printf("protocol         %02x\n", p_device->protocol);
        if(HID_DEBUG) log_printf("max packet in    %02x\n", p_device->max_packet_size_rx);
        if(HID_DEBUG) log_printf("max packet out   %02x\n", p_device->max_packet_size_tx);
    }
    if(!attach){
        log_printf("vid %04x pid %04x disconnected\n", SWAP16(p_device->vid),SWAP16(p_device->pid));
    }

    int device = 0;
    int deviceSlot = 0;
    for(int i = 0;i< gHIDMaxDevices;i++){
        u16 vid = config_controller[i][CONTRPS_VID][0] * 0x100 + config_controller[i][CONTRPS_VID][1];
        u16 pid = config_controller[i][CONTRPS_PID][0] * 0x100 + config_controller[i][CONTRPS_PID][1];
        if(SWAP16(p_device->vid) == vid && SWAP16(p_device->pid) == pid){
            device = config_controller_list[i];
            deviceSlot = i;
            //log_printf("Found device: device: %s slot: %d\n",byte_to_binary(device),deviceSlot);
            break;
        }
    }

    if ((p_device->sub_class == 1) && (p_device->protocol == 1)) { //Keyboard
        device = gHID_LIST_KEYBOARD;
        deviceSlot = gHID_SLOT_KEYBOARD;
        //log_printf("Found Keyboard: device: %s slot: %d\n",byte_to_binary(device),deviceSlot);
    }else if ((p_device->sub_class == 1) && (p_device->protocol == 2)){ // MOUSE
        device = gHID_LIST_MOUSE;
        deviceSlot = gMouseSlot;
        //log_printf("Found Mouse: device: %s slot: %d\n",byte_to_binary(device),deviceSlot);
    }

    if(device){
        if(attach){
            int bufSize = 64;
            if(device != gHID_LIST_MOUSE && config_controller[deviceSlot][CONTRPS_BUF_SIZE][0] == CONTROLLER_PATCHER_VALUE_SET){
                bufSize = config_controller[deviceSlot][CONTRPS_BUF_SIZE][1];
            }
            unsigned char *buf = memalign(64,bufSize);
            memset(buf,0,bufSize);
            my_cb_user *usr = memalign(64,sizeof(my_cb_user));
            usr->buf = buf;
            usr->hid = device;

            usr->transfersize = p_device->max_packet_size_rx;
            usr->handle = p_device->handle;

            memset(&gHID_Devices[deviceSlot],0,sizeof(HID_DEVICE_DATA));

            if(device != gHID_LIST_MOUSE ){
                usr->deviceSlot = deviceSlot;
                gHID_Devices[deviceSlot].user_data = usr;
            }else{
                memset(&gHID_Mouse,0,sizeof(HID_Mouse));
                gHID_Mouse.user_data = usr;
            }

            gHIDAttached |= device;
            gHIDCurrentDevice |= device;

            if(HID_DEBUG) log_printf("%08X(%d) attached!\n",device,deviceSlot);

            if(device == gHID_LIST_GC){ // GC PAD
                buf[0] = 0x13;
                HIDWrite(p_device->handle, usr->buf, 1, my_gc_write_cb, usr);
            }else if (device == gHID_LIST_MOUSE){
                HIDSetProtocol(p_device->handle, p_device->interface_index, 0, 0, 0);
                //HIDGetDescriptor(p_device->handle,0x22,0x00,0,my_buf,512,my_foo_cb,NULL);
                HIDSetIdle(p_device->handle,p_device->interface_index,1,NULL,NULL);
                gHID_Mouse_Mode = HID_MOUSE_MODE_TOUCH;
                HIDRead(p_device->handle, buf, p_device->max_packet_size_rx, my_ms_read_cb, usr);
            }else if (device == gHID_LIST_KEYBOARD){
                HIDSetProtocol(p_device->handle, p_device->interface_index, 1, 0, 0);
                HIDSetIdle(p_device->handle, p_device->interface_index, 0, 0, 0);
                HIDRead(p_device->handle, buf, p_device->max_packet_size_rx, my_read_cb, usr);
            }else if (device == gHID_LIST_DS3){
                HIDSetProtocol(p_device->handle, p_device->interface_index, 1, 0, 0);
                buf[0] = 0x42; buf[1] = 0x0c; buf[2] = 0x00; buf[3] = 0x00;
                HIDSetReport(p_device->handle, HID_REPORT_FEATURE, PS3_F4_REPORT_ID, buf, PS3_F4_REPORT_LEN, NULL, NULL);
                HIDRead(p_device->handle, usr->buf, p_device->max_packet_size_rx, my_read_cb, usr);
            }else{
                HIDRead(p_device->handle, usr->buf, p_device->max_packet_size_rx, my_read_cb, usr);
            }
            return HID_DEVICE_ATTACH;

        }else{
            my_cb_user * user_data = NULL;
            if(device == gHID_LIST_MOUSE){
                user_data = (my_cb_user *) gHID_Mouse.user_data;
            }else{
                user_data = (my_cb_user *) gHID_Devices[deviceSlot].user_data;
            }
            if(user_data){
                if(user_data->buf){
                    free(user_data->buf);
                    user_data->buf = NULL;
                }
                free(user_data);
                user_data = NULL;
            }
            gHIDAttached &= ~device;
            gHIDCurrentDevice &= ~device;
            if (device == gHID_LIST_MOUSE){
                gHID_Mouse_Mode = HID_MOUSE_MODE_AIM;
            }
            //log_printf("%d(%d) detached!\n",device,deviceSlot);
        }
    }else{

        log_print("HID-Device currently not supported!\n");
	}
	return HID_DEVICE_DETACH;
}

void setControllerReleasePressData(HID_Data_Struct data_cur, HID_Data_Struct data_last,VPADData * buffer,int VPADButton){
    int press = getButtonPressed(data_cur,VPADButton);
    if(press) buffer->btns_h |= VPADButton;
    if(press != getButtonPressed(data_last,VPADButton)){
        if(!press) buffer->btns_r |= VPADButton;
        if(press) buffer->btns_d |= VPADButton;
    }
}

u32 getEmulatedSticks(VPADData * buffer){
    u32 emulatedSticks = 0;
    int l_x_full = (buffer->lstick.x > 0.5f || buffer->lstick.x < -0.5f)? 1:0;
    int l_y_full = (buffer->lstick.y > 0.5f || buffer->lstick.y < -0.5f)? 1:0;
    int r_x_full = (buffer->rstick.x > 0.5f || buffer->rstick.x < -0.5f)? 1:0;
    int r_y_full = (buffer->rstick.y > 0.5f || buffer->rstick.y < -0.5f)? 1:0;

    if((buffer->lstick.x > 0.5f) || (buffer->lstick.x > 0.1f && !l_y_full)){
        emulatedSticks |= VPAD_STICK_L_EMULATION_RIGHT;
    }
    if((buffer->lstick.x < -0.5f) || (buffer->lstick.x < -0.1f && !l_y_full)){
        emulatedSticks |= VPAD_STICK_L_EMULATION_LEFT;
    }
    if((buffer->lstick.y > 0.5f) || (buffer->lstick.y > 0.1f && !l_x_full)){
        emulatedSticks |= VPAD_STICK_L_EMULATION_UP;
    }
    if((buffer->lstick.y < -0.5f) || (buffer->lstick.y < -0.1f && !l_x_full)){
        emulatedSticks |= VPAD_STICK_L_EMULATION_DOWN;
    }

    if((buffer->rstick.x > 0.5f) || (buffer->rstick.x > 0.1f && !r_y_full)){
        emulatedSticks |= VPAD_STICK_R_EMULATION_RIGHT;
    }
    if((buffer->rstick.x < -0.5f) || (buffer->rstick.x < -0.1f && !r_y_full)){
        emulatedSticks |= VPAD_STICK_R_EMULATION_LEFT;
    }
    if((buffer->rstick.y > 0.5f) || (buffer->rstick.y > 0.1f && !r_x_full)){
        emulatedSticks |= VPAD_STICK_R_EMULATION_UP;
    }
    if((buffer->rstick.y < -0.5f) || (buffer->rstick.y < -0.1f && !r_x_full)){
        emulatedSticks |= VPAD_STICK_R_EMULATION_DOWN;
    }
    return emulatedSticks;
}

void setControllerDataFromHID(VPADData * buffer,HID_DEVICE_LIST hid_devices){
    int hid = 0;
    if(hid_devices == HID_ALL_CONNECTED_DEVICES){
        hid = gHIDCurrentDevice;
    }
    int size = 0;
    HID_Data_Struct * data = getHIDDataAll(hid,&size);
    if(!data || !size || size < 0){
        if(data)free(data);
        return;
    }

    HID_Data_Struct data_cur;
    HID_Data_Struct data_last;

    if(hid & gHID_LIST_MOUSE){
        if(gHID_Mouse.pad_data[0].data[0].valuedChanged == 1){ //Fix for the mouse cursor
            gHID_Mouse.pad_data[0].data[0].valuedChanged = 0;
        }else{
            gHID_Mouse.pad_data[0].data[0].deltaX = 0;
            gHID_Mouse.pad_data[0].data[0].deltaY = 0;
        }

        //log_printf("X:%d Y:%d deltaX: %d deltaY: %d\n",gHID_Mouse.pad_data[0].data[0].X ,gHID_Mouse.pad_data[0].data[0].Y,gHID_Mouse.pad_data[0].data[0].deltaX,gHID_Mouse.pad_data[0].data[0].deltaY);
    }
    for(int i = 0;i<size;i++){

        data_cur = data[i];
        HID_Data_Struct * data_last_ptr = getLastData(data_cur);
        if(!data_last_ptr) return;
        data_last = data_last_ptr[0];

        setControllerReleasePressData(data_cur,data_last,buffer,VPAD_BUTTON_A);
        setControllerReleasePressData(data_cur,data_last,buffer,VPAD_BUTTON_B);
        setControllerReleasePressData(data_cur,data_last,buffer,VPAD_BUTTON_X);
        setControllerReleasePressData(data_cur,data_last,buffer,VPAD_BUTTON_Y);

        setControllerReleasePressData(data_cur,data_last,buffer,VPAD_BUTTON_LEFT);
        setControllerReleasePressData(data_cur,data_last,buffer,VPAD_BUTTON_RIGHT);
        setControllerReleasePressData(data_cur,data_last,buffer,VPAD_BUTTON_DOWN);
        setControllerReleasePressData(data_cur,data_last,buffer,VPAD_BUTTON_UP);

        setControllerReleasePressData(data_cur,data_last,buffer,VPAD_BUTTON_MINUS);
        setControllerReleasePressData(data_cur,data_last,buffer,VPAD_BUTTON_L);
        setControllerReleasePressData(data_cur,data_last,buffer,VPAD_BUTTON_R);

        setControllerReleasePressData(data_cur,data_last,buffer,VPAD_BUTTON_PLUS);
        setControllerReleasePressData(data_cur,data_last,buffer,VPAD_BUTTON_ZL);
        setControllerReleasePressData(data_cur,data_last,buffer,VPAD_BUTTON_ZR);

        setControllerReleasePressData(data_cur,data_last,buffer,VPAD_BUTTON_HOME);
        setControllerReleasePressData(data_cur,data_last,buffer,VPAD_BUTTON_STICK_L);
        setControllerReleasePressData(data_cur,data_last,buffer,VPAD_BUTTON_STICK_R);

        convertAnalogSticks(data_cur,buffer);

        u32 emulatedSticks = getEmulatedSticks(buffer);

        //Setting the emulated sticks
        buffer->btns_h |= emulatedSticks;
        buffer->btns_d |= (emulatedSticks & (~last_emulatedSticks));
        buffer->btns_r |= (last_emulatedSticks & (~emulatedSticks));

        last_emulatedSticks = emulatedSticks;
        checkMouseMode(data_cur,data_last);

        setTouch(data_cur,buffer);

        setLastData(data_cur,data_last);
        if(data_last_ptr){
            free(data_last_ptr);
            data_last_ptr = NULL;
        }

    }

    // Caculates a valid stick position
    if(size){
        f32 max = 0.0f;
        f32 mul = 0.0f;
        if((max = (fabs(buffer->lstick.x)) + fabs(buffer->lstick.y)) > 1.414f){
            mul = 1.414f / max;
            buffer->lstick.x *= mul;
            buffer->lstick.y *= mul;
        }
         if((max = (fabs(buffer->rstick.x)) + fabs(buffer->rstick.y)) > 1.414f){
            mul = 1.414f / max;
            buffer->rstick.x *= mul;
            buffer->rstick.y *= mul;
        }
        if(buffer->lstick.x > 1.0f) buffer->lstick.x = 1.0f;
        if(buffer->lstick.y > 1.0f) buffer->lstick.y = 1.0f;
        if(buffer->lstick.x < -1.0f) buffer->lstick.x = -1.0f;
        if(buffer->lstick.y < -1.0f) buffer->lstick.y = -1.0f;

        if(buffer->rstick.x > 1.0f) buffer->rstick.x = 1.0f;
        if(buffer->rstick.y > 1.0f) buffer->rstick.y = 1.0f;
        if(buffer->rstick.x < -1.0f) buffer->rstick.x = -1.0f;
        if(buffer->rstick.y < -1.0f) buffer->rstick.y = -1.0f;

    }

    if(data){
        free(data);
        data = NULL;
    }
    int current_rumblestatus = !!VPADBASEGetMotorOnRemainingCount(0);

    setRumble(hid,current_rumblestatus);
    //log_printf("hold: %016X down: %016X release: %016X\n",buffer->btns_h,buffer->btns_d,buffer->btns_r);
    //log_printf("new || hold: %016X down: %016X release: %016X \n",buffer->btns_h,buffer->btns_d,buffer->btns_r);
}

void printButtons(VPADData * buffer){
    if(buffer->btns_d != 0){
        char output[100];
        output[0] = 0;
        if((buffer->btns_d & VPAD_BUTTON_A) == VPAD_BUTTON_A){
            strcat(output,"A ");
        }
        if((buffer->btns_d & VPAD_BUTTON_B) == VPAD_BUTTON_B){
            strcat(output,"B ");
        }
        if((buffer->btns_d & VPAD_BUTTON_X) == VPAD_BUTTON_X){
            strcat(output,"X ");
        }
        if((buffer->btns_d & VPAD_BUTTON_Y) == VPAD_BUTTON_Y){
            strcat(output,"Y ");
        }
        if((buffer->btns_d & VPAD_BUTTON_L) == VPAD_BUTTON_L){
            strcat(output,"L ");
        }
        if((buffer->btns_d & VPAD_BUTTON_R) == VPAD_BUTTON_R){
            strcat(output,"R ");
        }
        if((buffer->btns_d & VPAD_BUTTON_ZR) == VPAD_BUTTON_ZR){
            strcat(output,"ZR ");
        }
        if((buffer->btns_d & VPAD_BUTTON_ZL) == VPAD_BUTTON_ZL){
            strcat(output,"ZL ");
        }
        if((buffer->btns_d & VPAD_BUTTON_LEFT) == VPAD_BUTTON_LEFT){
            strcat(output,"Left ");
        }
        if((buffer->btns_d & VPAD_BUTTON_RIGHT) == VPAD_BUTTON_RIGHT){
            strcat(output,"Right ");
        }
        if((buffer->btns_d & VPAD_BUTTON_UP) == VPAD_BUTTON_UP){
            strcat(output,"Up ");
        }
        if((buffer->btns_d & VPAD_BUTTON_DOWN) == VPAD_BUTTON_DOWN){
            strcat(output,"Down ");
        }
        if((buffer->btns_d & VPAD_BUTTON_PLUS) == VPAD_BUTTON_PLUS){
            strcat(output,"+ ");
        }
        if((buffer->btns_d & VPAD_BUTTON_MINUS) == VPAD_BUTTON_MINUS){
            strcat(output,"- ");
        }
        if((buffer->btns_d & VPAD_BUTTON_TV) == VPAD_BUTTON_TV){
            strcat(output,"TV ");
        }
        if((buffer->btns_d & VPAD_BUTTON_HOME) == VPAD_BUTTON_HOME){
            strcat(output,"HOME ");
        }
        if((buffer->btns_d & VPAD_BUTTON_STICK_L) == VPAD_BUTTON_STICK_L){
            strcat(output,"SL ");
        }
        if((buffer->btns_d & VPAD_BUTTON_STICK_R) == VPAD_BUTTON_STICK_R){
            strcat(output,"SR ");
        }
        if((buffer->btns_d & VPAD_STICK_R_EMULATION_LEFT) == VPAD_STICK_R_EMULATION_LEFT){
            strcat(output,"RE_Left ");
        }
        if((buffer->btns_d & VPAD_STICK_R_EMULATION_RIGHT) == VPAD_STICK_R_EMULATION_RIGHT){
            strcat(output,"RE_Right ");
        }
        if((buffer->btns_d & VPAD_STICK_R_EMULATION_UP) == VPAD_STICK_R_EMULATION_UP){
            strcat(output,"RE_Up ");
        }
        if((buffer->btns_d & VPAD_STICK_R_EMULATION_DOWN) == VPAD_STICK_R_EMULATION_DOWN){
            strcat(output,"RE_Down ");
        }
        if((buffer->btns_d & VPAD_STICK_L_EMULATION_LEFT) == VPAD_STICK_L_EMULATION_LEFT){
            strcat(output,"LE_Left ");
        }
        if((buffer->btns_d & VPAD_STICK_L_EMULATION_RIGHT) == VPAD_STICK_L_EMULATION_RIGHT){
            strcat(output,"LE_Right ");
        }
        if((buffer->btns_d & VPAD_STICK_L_EMULATION_UP) == VPAD_STICK_L_EMULATION_UP){
            strcat(output,"LE_Up ");
        }
        if((buffer->btns_d & VPAD_STICK_L_EMULATION_DOWN) == VPAD_STICK_L_EMULATION_DOWN){
            strcat(output,"LE_Down ");
        }

        log_printf("%spressed Sticks: LX %f LY %f RX %f RY %f\n",output,buffer->lstick.x,buffer->lstick.y,buffer->rstick.x,buffer->rstick.y);
    }
}

void buttonRemapping(VPADData * buffer){
    VPADData new_data;
    new_data.btns_h = 0;

    setButtonRemappingData(buffer,&new_data,VPAD_BUTTON_A,                  CONTRPS_VPAD_BUTTON_A);
    setButtonRemappingData(buffer,&new_data,VPAD_BUTTON_B,                  CONTRPS_VPAD_BUTTON_B);
    setButtonRemappingData(buffer,&new_data,VPAD_BUTTON_X,                  CONTRPS_VPAD_BUTTON_X);
    setButtonRemappingData(buffer,&new_data,VPAD_BUTTON_Y,                  CONTRPS_VPAD_BUTTON_Y);
    setButtonRemappingData(buffer,&new_data,VPAD_BUTTON_LEFT,               CONTRPS_VPAD_BUTTON_LEFT);
    setButtonRemappingData(buffer,&new_data,VPAD_BUTTON_RIGHT,              CONTRPS_VPAD_BUTTON_RIGHT);
    setButtonRemappingData(buffer,&new_data,VPAD_BUTTON_UP,                 CONTRPS_VPAD_BUTTON_UP);
    setButtonRemappingData(buffer,&new_data,VPAD_BUTTON_DOWN,               CONTRPS_VPAD_BUTTON_DOWN);
    setButtonRemappingData(buffer,&new_data,VPAD_BUTTON_ZL,                 CONTRPS_VPAD_BUTTON_ZL);
    setButtonRemappingData(buffer,&new_data,VPAD_BUTTON_ZR,                 CONTRPS_VPAD_BUTTON_ZR);
    setButtonRemappingData(buffer,&new_data,VPAD_BUTTON_L,                  CONTRPS_VPAD_BUTTON_L);
    setButtonRemappingData(buffer,&new_data,VPAD_BUTTON_R,                  CONTRPS_VPAD_BUTTON_R);
    setButtonRemappingData(buffer,&new_data,VPAD_BUTTON_PLUS,               CONTRPS_VPAD_BUTTON_PLUS);
    setButtonRemappingData(buffer,&new_data,VPAD_BUTTON_MINUS,              CONTRPS_VPAD_BUTTON_MINUS);
    setButtonRemappingData(buffer,&new_data,VPAD_BUTTON_HOME,               CONTRPS_VPAD_BUTTON_HOME);
    setButtonRemappingData(buffer,&new_data,VPAD_BUTTON_SYNC,               CONTRPS_VPAD_BUTTON_SYNC);
    setButtonRemappingData(buffer,&new_data,VPAD_BUTTON_STICK_R,            CONTRPS_VPAD_BUTTON_STICK_R);
    setButtonRemappingData(buffer,&new_data,VPAD_BUTTON_STICK_L,            CONTRPS_VPAD_BUTTON_STICK_L);
    setButtonRemappingData(buffer,&new_data,VPAD_BUTTON_TV,                 CONTRPS_VPAD_BUTTON_TV);


    //No need to remap the emulated sticks. But we need to keep them
    setHoldButtonData(buffer,&new_data,VPAD_STICK_L_EMULATION_LEFT,VPAD_STICK_L_EMULATION_LEFT);
    setHoldButtonData(buffer,&new_data,VPAD_STICK_L_EMULATION_RIGHT,VPAD_STICK_L_EMULATION_RIGHT);
    setHoldButtonData(buffer,&new_data,VPAD_STICK_L_EMULATION_UP,VPAD_STICK_L_EMULATION_UP);
    setHoldButtonData(buffer,&new_data,VPAD_STICK_L_EMULATION_DOWN,VPAD_STICK_L_EMULATION_DOWN);

    setHoldButtonData(buffer,&new_data,VPAD_STICK_R_EMULATION_LEFT,VPAD_STICK_R_EMULATION_LEFT);
    setHoldButtonData(buffer,&new_data,VPAD_STICK_R_EMULATION_RIGHT,VPAD_STICK_R_EMULATION_RIGHT);
    setHoldButtonData(buffer,&new_data,VPAD_STICK_R_EMULATION_UP,VPAD_STICK_R_EMULATION_UP);
    setHoldButtonData(buffer,&new_data,VPAD_STICK_R_EMULATION_DOWN,VPAD_STICK_R_EMULATION_DOWN);

    /*
    setButtonRemappingData(buffer,&new_data,VPAD_STICK_R_EMULATION_LEFT,    CONTRPS_VPAD_STICK_R_EMULATION_LEFT);
    setButtonRemappingData(buffer,&new_data,VPAD_STICK_R_EMULATION_RIGHT,   CONTRPS_VPAD_STICK_R_EMULATION_RIGHT);
    setButtonRemappingData(buffer,&new_data,VPAD_STICK_R_EMULATION_UP,      CONTRPS_VPAD_STICK_R_EMULATION_UP);
    setButtonRemappingData(buffer,&new_data,VPAD_STICK_R_EMULATION_DOWN,    CONTRPS_VPAD_STICK_R_EMULATION_DOWN);

    setButtonRemappingData(buffer,&new_data,VPAD_STICK_L_EMULATION_LEFT,    CONTRPS_VPAD_STICK_L_EMULATION_LEFT);
    setButtonRemappingData(buffer,&new_data,VPAD_STICK_L_EMULATION_RIGHT,   CONTRPS_VPAD_STICK_L_EMULATION_RIGHT);
    setButtonRemappingData(buffer,&new_data,VPAD_STICK_L_EMULATION_UP,      CONTRPS_VPAD_STICK_L_EMULATION_UP);
    setButtonRemappingData(buffer,&new_data,VPAD_STICK_L_EMULATION_DOWN,    CONTRPS_VPAD_STICK_L_EMULATION_DOWN);*/

    u32 button_hold = new_data.btns_h;

    buffer->btns_h = button_hold;
    buffer->btns_d = (button_hold & (~last_button_hold));
    buffer->btns_r = (last_button_hold & (~button_hold));

    last_button_hold = button_hold;
}

void setButtonRemappingData(VPADData * buffer, VPADData * new_data,u32 VPADButton, int CONTRPS_Value){
    u32 new_value = VPADButton;

    if(config_controller[gGamePadSlot][CONTRPS_Value][0] != CONTROLLER_PATCHER_INVALIDVALUE){ //using new value!
       new_value = gGamePadValues[config_controller[gGamePadSlot][CONTRPS_Value][1]];
    }
    setHoldButtonData(buffer,new_data,VPADButton,new_value);
}

void setHoldButtonData(VPADData * buffer, VPADData * new_data,u32 oldVPADButton,u32 newVPADButton){
    if((buffer->btns_h & oldVPADButton) == oldVPADButton){
        new_data->btns_h |= newVPADButton;
    }
}

int getDevice(int hid){
    for(int i = 0;i < gHIDMaxDevices;i++){
        if(hid & config_controller_list[i]){
              return i;
        }
    }
    return -1;
}

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Shared functions
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int getActivePad(int hid){
     if(hid & gHID_LIST_GC){
        if (!(((gHID_Devices[gHID_SLOT_GC].pad_data[0].hid_data[0] & 0x10) == 0) && ((gHID_Devices[gHID_SLOT_GC].pad_data[0].hid_data[0] & 0x22) != 0x22))) return 0;
        if (!(((gHID_Devices[gHID_SLOT_GC].pad_data[1].hid_data[0] & 0x10) == 0) && ((gHID_Devices[gHID_SLOT_GC].pad_data[1].hid_data[0] & 0x22) != 0x22))) return 1;
        if (!(((gHID_Devices[gHID_SLOT_GC].pad_data[2].hid_data[0] & 0x10) == 0) && ((gHID_Devices[gHID_SLOT_GC].pad_data[2].hid_data[0] & 0x22) != 0x22))) return 2;
        if (!(((gHID_Devices[gHID_SLOT_GC].pad_data[3].hid_data[0] & 0x10) == 0) && ((gHID_Devices[gHID_SLOT_GC].pad_data[3].hid_data[0] & 0x22) != 0x22))) return 3;
        return -1;
     }
     return 0;
}

void setRumble(int hid,int rumble){
    int device = getDevice(hid);
    if(device == -1) return;
    int pad = getActivePad(hid);
    if(pad < 0) return;
    ((HID_Data *)config_controller_data_ptr[device][pad])->rumbleActive = rumble;
}

HID_Data_Struct * getHIDDataAll(int hid,int * size){
    *size = 0;
    int bit = 1;
    for (int i = 0;i < gHIDMaxDevices; i++){
       *size += (hid & bit)>>i;
       bit <<= 1;
    }
    //log_printf("Found %d devices! for hid: %s\n",*size,byte_to_binary(hid));

    HID_Data_Struct * data = malloc(sizeof(HID_Data_Struct)*(*size));
    if(!data) return NULL;
    int i = 0;

    //!Mouse always need extra treatment
    if(hid & gHID_LIST_MOUSE){
        unsigned char * src = (unsigned char *) &(gHID_Mouse.pad_data[getActivePad(gHID_LIST_MOUSE)].data[0]);
        data[i].src = src;
        data[i].hid = gHID_LIST_MOUSE;
        i++;
    }

    for(int j = 0;j < gHIDMaxDevices;j++){
        if(hid & config_controller_list[j]){
            int pad = getActivePad(config_controller_list[j]);
            if(pad < 0){ //Not pad connected to adapter
                (*size)--;
                //log_printf("Not active pads! New size %d\n",*size);
                continue;
            }
            data[i].hid = config_controller_list[j];
            //log_printf("data[%d].hid = %s(config_controller_list[%d])!\n",i,byte_to_binary(config_controller_list[j]),j);
            data[i].src = (u8*)&(((HID_Data *)config_controller_data_ptr[j][pad])->hid_data);
            //log_printf("data[%d].src = %08X(config_controller_list[%d])!\n",i,data[i].src,j);
            if(!data[i].src){
                (*size)--;
                log_printf("src pointer == NULL!New size %d\n",*size);
                continue;
            }
            i++;
        }
    }
    return data;
}

int getPadCount(int hid){
    int device = getDevice(hid);
    if(device == -1) return 0;
    if(config_controller[device][CONTRPS_PAD_COUNT][0] == CONTROLLER_PATCHER_VALUE_SET){
        return config_controller[device][CONTRPS_PAD_COUNT][1];
    }
    log_printf("pad count: 0 for %s\n ",byte_to_binary(hid));

    return 0;
}

void checkMouseMode(HID_Data_Struct data,HID_Data_Struct data_last){
    unsigned char * src = data.src;
    unsigned char * src_last = data_last.src;
    int hid = data.hid;

    if(hid & gHID_LIST_KEYBOARD){
        if(wasInKeyboard(src,HID_KEYBOARD_BUTTON_F1) && (wasInKeyboard(src,HID_KEYBOARD_BUTTON_F1) != wasInKeyboard(src_last,HID_KEYBOARD_BUTTON_F1))){
            if(gHID_Mouse_Mode == HID_MOUSE_MODE_AIM){
                gHID_Mouse_Mode = HID_MOUSE_MODE_TOUCH;
                log_print("Mouse mode changed! to touch \n");
            }else if(gHID_Mouse_Mode == HID_MOUSE_MODE_TOUCH){
                log_print("Mouse mode changed! to aim \n");
                gHID_Mouse_Mode = HID_MOUSE_MODE_AIM;
            }
        }
    }
}

void setLastData(HID_Data_Struct data,HID_Data_Struct data_last){
    unsigned char * src = data.src;
    unsigned char * src_last = data_last.src;
    int hid = data.hid;

    if(!src || !src_last) return;

    if(hid & gHID_LIST_MOUSE){
        memcpy(src_last,src,sizeof(HID_Mouse_Data));
    }

    memcpy(src_last,src,HID_MAX_DATA_LENGTH_PER_PAD);
}

HID_Data_Struct * getLastData(HID_Data_Struct data){
    int hid = data.hid;
    unsigned char * src = data.src;
    HID_Data_Struct * last_data = malloc(sizeof(HID_Data_Struct));
    if(!last_data) return NULL;

    if(hid & gHID_LIST_MOUSE){
        last_data->hid = gHID_LIST_MOUSE;
        last_data->src = (unsigned char *) &(gHID_Mouse.pad_data[0].data[1]);// further testing
        return last_data;
    }

    int device = getDevice(hid);
    if(device == -1) return NULL;

    last_data->hid =       config_controller_list[device];
    last_data->src = src + HID_MAX_DATA_LENGTH_PER_PAD;
    return last_data;
}

int getButtonPressed(HID_Data_Struct data, int VPADButton){
    unsigned char * src = data.src;
    int hid = data.hid;


    if(hid & gHID_LIST_MOUSE){
        if(gHID_Mouse_Mode == HID_MOUSE_MODE_TOUCH){
            HID_Mouse_Data *  ms_data = (HID_Mouse_Data *) src;
            if(VPADButton == VPAD_BUTTON_TOUCH){
                if(ms_data->left_click & 0x01){
                        return 1;
                }
            }
        }else if(gHID_Mouse_Mode == HID_MOUSE_MODE_AIM){
            HID_Mouse_Data *  ms_data = (HID_Mouse_Data *) src;
            if(config_controller[gMouseSlot][CONTRPS_VPAD_BUTTON_LEFT][0] == CONTROLLER_PATCHER_VALUE_SET){
                if(VPADButton == (int)gGamePadValues[config_controller[gMouseSlot][CONTRPS_VPAD_BUTTON_LEFT][1]]){
                    if(ms_data->left_click & 0x01){
                        return 1;
                    }
                }
            }
            if(config_controller[gMouseSlot][CONTRPS_VPAD_BUTTON_RIGHT][0] == CONTROLLER_PATCHER_VALUE_SET){
                if(VPADButton == (int)gGamePadValues[config_controller[gMouseSlot][CONTRPS_VPAD_BUTTON_RIGHT][1]]){
                    if(ms_data->right_click & 0x01){
                        return 1;
                    }
                }
            }
        }
        return 0;
    }

    int device = getDevice(hid);
    if(device == -1) return 0;

    int cur_config = 0;

    if(VPADButton == VPAD_BUTTON_A){
        cur_config = CONTRPS_VPAD_BUTTON_A;
    }else if(VPADButton == VPAD_BUTTON_B){
        cur_config = CONTRPS_VPAD_BUTTON_B;
    }else if(VPADButton == VPAD_BUTTON_X){
        cur_config = CONTRPS_VPAD_BUTTON_X;
    }else if(VPADButton == VPAD_BUTTON_Y){
        cur_config = CONTRPS_VPAD_BUTTON_Y;
    }else if(VPADButton == VPAD_BUTTON_L){
        cur_config = CONTRPS_VPAD_BUTTON_L;
    }else if(VPADButton == VPAD_BUTTON_R){
        cur_config = CONTRPS_VPAD_BUTTON_R;
    }else if(VPADButton == VPAD_BUTTON_ZL){
        cur_config = CONTRPS_VPAD_BUTTON_ZL;
    }else if(VPADButton == VPAD_BUTTON_ZR){
        cur_config = CONTRPS_VPAD_BUTTON_ZR;
    }else if(VPADButton == VPAD_BUTTON_STICK_L){
        cur_config = CONTRPS_VPAD_BUTTON_STICK_L;
    }else if(VPADButton == VPAD_BUTTON_STICK_R){
        cur_config = CONTRPS_VPAD_BUTTON_STICK_R;
    }else if(VPADButton == VPAD_BUTTON_PLUS){
        cur_config = CONTRPS_VPAD_BUTTON_PLUS;
    }else if(VPADButton == VPAD_BUTTON_MINUS){
        cur_config = CONTRPS_VPAD_BUTTON_MINUS;
    }else if(VPADButton == VPAD_BUTTON_HOME){
        cur_config = CONTRPS_VPAD_BUTTON_HOME;
    }

    //! Special DPAD treatment.
    if(config_controller[device][CONTRPS_DPAD_MODE][0] == CONTROLLER_PATCHER_VALUE_SET){
        if(config_controller[device][CONTRPS_DPAD_MODE][1] == CONTRPDM_Hat){
            u8 mask = 0x0F;
            if(config_controller[device][CONTRPS_DPAD_MASK][0] == CONTROLLER_PATCHER_VALUE_SET){
               mask = config_controller[device][CONTRPS_DPAD_MASK][1];
            }
            if(src[config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_NEUTRAL][0]] != config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_NEUTRAL][1]){ // Not neutral
                u8 dir1_0 = 0,dir1_1 = 0;
                u8 dir2_0 = 0,dir2_1 = 0;
                u8 dir3_0 = 0,dir3_1 = 0;
                u8 direction = 0;
                if(VPADButton == VPAD_BUTTON_LEFT){
                    dir1_0 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_W][0];
                    dir2_0 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_NW][0];
                    dir3_0 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_SW][0];
                    dir1_1 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_W][1];
                    dir2_1 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_NW][1];
                    dir3_1 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_SW][1];
                    direction = 1;
                }else if(VPADButton == VPAD_BUTTON_RIGHT){
                    dir1_0 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_E][0];
                    dir2_0 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_SE][0];
                    dir3_0 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_NE][0];
                    dir1_1 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_E][1];
                    dir2_1 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_SE][1];
                    dir3_1 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_NE][1];
                    direction = 1;
                }else if(VPADButton == VPAD_BUTTON_DOWN){
                    dir1_0 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_S][0];
                    dir2_0 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_SE][0];
                    dir3_0 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_SW][0];
                    dir1_1 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_S][1];
                    dir2_1 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_SE][1];
                    dir3_1 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_SW][1];
                    direction = 1;
                }else if(VPADButton == VPAD_BUTTON_UP){
                    dir1_0 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_N][0];
                    dir2_0 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_NW][0];
                    dir3_0 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_NE][0];
                    dir1_1 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_N][1];
                    dir2_1 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_NW][1];
                    dir3_1 = config_controller[device][CONTRPS_VPAD_BUTTON_DPAD_NE][1];
                    direction = 1;
                }
                if(direction && (((src[dir1_0] & mask) == dir1_1) ||
                        ((src[dir2_0] & mask) == dir2_1) ||
                        ((src[dir3_0] & mask) == dir3_1))) return 1;
            }

        }else  if(config_controller[device][CONTRPS_DPAD_MODE][1] == CONTRPDM_Absolute_2Values){
            int contrps_value = 0;
            if(VPADButton == VPAD_BUTTON_LEFT){
                contrps_value = CONTRPS_VPAD_BUTTON_DPAD_ABS_LEFT;
            }else if(VPADButton == VPAD_BUTTON_RIGHT){
                contrps_value = CONTRPS_VPAD_BUTTON_DPAD_ABS_RIGHT;
            }else if(VPADButton == VPAD_BUTTON_UP){
                contrps_value = CONTRPS_VPAD_BUTTON_DPAD_ABS_UP;
            }else if(VPADButton == VPAD_BUTTON_DOWN){
                contrps_value = CONTRPS_VPAD_BUTTON_DPAD_ABS_DOWN;
            }

            if(contrps_value != 0){
                int value_byte = CONTROLLER_PATCHER_INVALIDVALUE;
                if((value_byte = config_controller[device][contrps_value][0]) != CONTROLLER_PATCHER_INVALIDVALUE){
                    if(src[config_controller[device][contrps_value][0]] == config_controller[device][contrps_value][1]){
                        return 1;
                    }
                }
            }
        }
    }

    //! Normal DPAD treatment.
    if(VPADButton == VPAD_BUTTON_LEFT){
        cur_config = CONTRPS_VPAD_BUTTON_LEFT;
    }else if(VPADButton == VPAD_BUTTON_RIGHT){
        cur_config = CONTRPS_VPAD_BUTTON_RIGHT;
    }else if(VPADButton == VPAD_BUTTON_DOWN){
        cur_config = CONTRPS_VPAD_BUTTON_DOWN;
    }else if(VPADButton == VPAD_BUTTON_UP){
        cur_config = CONTRPS_VPAD_BUTTON_UP;
    }

    if(config_controller[device][CONTRPS_DOUBLE_USE][0] == CONTROLLER_PATCHER_VALUE_SET){
        if(config_controller[device][CONTRPS_DOUBLE_USE][1] == CONTROLLER_PATCHER_GC_DOUBLE_USE){
            if(src[config_controller[device][CONTRPS_DOUBLE_USE_BUTTON_ACTIVATOR][0]] & config_controller[device][CONTRPS_DOUBLE_USE_BUTTON_ACTIVATOR][1]){
                if(checkDoubleUseGC(device,cur_config,CONTRPS_DOUBLE_USE_BUTTON_1_RELEASED))return 0;
                if(checkDoubleUseGC(device,cur_config,CONTRPS_DOUBLE_USE_BUTTON_2_RELEASED))return 0;
                if(checkDoubleUseGC(device,cur_config,CONTRPS_DOUBLE_USE_BUTTON_3_RELEASED))return 0;
                if(checkDoubleUseGC(device,cur_config,CONTRPS_DOUBLE_USE_BUTTON_4_RELEASED))return 0;
                if(checkDoubleUseGC(device,cur_config,CONTRPS_DOUBLE_USE_BUTTON_5_RELEASED))return 0;
            }else{
                if(checkDoubleUseGC(device,cur_config,CONTRPS_DOUBLE_USE_BUTTON_1_PRESSED))return 0;
                if(checkDoubleUseGC(device,cur_config,CONTRPS_DOUBLE_USE_BUTTON_2_PRESSED))return 0;
                if(checkDoubleUseGC(device,cur_config,CONTRPS_DOUBLE_USE_BUTTON_3_PRESSED))return 0;
                if(checkDoubleUseGC(device,cur_config,CONTRPS_DOUBLE_USE_BUTTON_4_PRESSED))return 0;
                if(checkDoubleUseGC(device,cur_config,CONTRPS_DOUBLE_USE_BUTTON_5_PRESSED))return 0;
            }
        }
    }
    //log_printf("Check button %08X \n",VPADButton);
    if(config_controller[device][cur_config][0] != CONTROLLER_PATCHER_INVALIDVALUE){ //Invalid data
        if(hid & gHID_LIST_KEYBOARD){
            if(wasInKeyboard(src,config_controller[device][cur_config][1])) return 1;
        }else{
            //log_printf("src[%02X] = %02X & %02X\n",config_controller[device][cur_config][0],src[config_controller[device][cur_config][0]],config_controller[device][cur_config][1]);
            //log_printf("%02X %02X %02X %02X %02X %02X %02X %02X | %d = %08X? \n",src[0],src[1],src[2],src[3],src[4],src[5],src[6],src[7],config_controller[device][cur_config][0],config_controller[device][cur_config][1]);
            if(src[config_controller[device][cur_config][0]] & config_controller[device][cur_config][1]){
                return 1;
            }

        }
    }else{
        //log_printf("Invalid data! device(slot): %d config: %d\n",device,cur_config);
    }
    return 0;
}

int checkDoubleUseGC(int device,int cur_config,int cur_CONTRPS_value){
    if(config_controller[device][cur_CONTRPS_value][0] != CONTROLLER_PATCHER_INVALIDVALUE){
        if(cur_config == config_controller[device][cur_CONTRPS_value][1]) return 1;
    }
    return 0;
}

//TODO: remove hard coded values.
void setTouch(HID_Data_Struct data,VPADData * buffer){
    unsigned char * src = data.src;
    int hid = data.hid;
    if(hid & gHID_LIST_MOUSE && gHID_Mouse_Mode == HID_MOUSE_MODE_TOUCH){
        if(getButtonPressed(data,VPAD_BUTTON_TOUCH)){
           HID_Mouse_Data *  ms_data = (HID_Mouse_Data *) src;
           int x_mouse = 80 + ((int)(((ms_data->X)*1.0f/1280.0)*3890.0f));
           int y_mouse = 3910 - ((int)(((ms_data->Y)*1.0f/720.0)*3760.0f));
           buffer->tpdata.x = x_mouse;
           buffer->tpdata.y = y_mouse;
           buffer->tpdata.touched = 1;
           buffer->tpdata.invalid = 0;
           buffer->tpdata1.x = x_mouse;
           buffer->tpdata1.y = y_mouse;
           buffer->tpdata1.touched = 1;
           buffer->tpdata1.invalid = 0;
           buffer->tpdata2.x = x_mouse;
           buffer->tpdata2.y = y_mouse;
           buffer->tpdata2.touched = 1;
           buffer->tpdata2.invalid = 0;
        }
    }
}

f32 convertAnalogValue(u8 value, u8 default_val, u8 min, u8 max, u8 invert,u8 deadzone){
    s8 new_value = (s8)(value - default_val);
    u8 range = 0;
    if(value >= max){
        if(invert == 0x01) return -1.0f;
        return 1.0f;
    }else if(value <= min){
        if(invert == 0x01) return 1.0f;
        return -1.0f;
    }
    if((value-deadzone) > default_val){
        new_value -= deadzone;
        range = (max - (default_val + deadzone));
    }else if((value+deadzone) < default_val){
        new_value += deadzone;
        range = ((default_val - deadzone) - min);
    }else{
        return 0.0f;
    }
    if(invert != 0x01){
        return (new_value / (1.0f*range));
    }else{
        return -1.0f*(new_value / (1.0f*range));
    }
}

Vec2D getAnalogValueByButtons(int up, int down, int left, int right, int * changed){
    Vec2D stick;
    stick.x = 0.0f;
    stick.y = 0.0f;

    if(up){
        if(!down){
            stick.y = 1.0f;
        }
        if(left || right){
            stick.y = 0.707f;
            if(left) stick.x = -0.707f;
            if(right) stick.x = 0.707f;
        }
        if(changed != NULL) *changed = 1;
        }else if(down){
        if(!up){
            stick.y = -1.0f;
        }
        if(left || right){
            stick.y = -0.707f;
            if(left) stick.x = -0.707f;
            if(right) stick.x = 0.707f;
        }
        if(changed != NULL) *changed = 1;
        }else{
        if(left){
             if(!right){
                stick.x = -1.0f;
                if(changed != NULL) *changed = 1;
             }

        }else if(right){
            if(!down){
                stick.x = 1.0f;
                if(changed != NULL) *changed = 1;
             }
        }
    }
    return stick;
}

void convertAnalogSticks(HID_Data_Struct data, VPADData * buffer){
    unsigned char * src = data.src;
    int hid = data.hid;
    int changed = 0;


    if (hid & gHID_LIST_MOUSE){
        if(gHID_Mouse_Mode == HID_MOUSE_MODE_AIM){ // TODO: tweak values
            HID_Mouse_Data *  ms_data = (HID_Mouse_Data *) src;

            f32 x_value = ms_data->deltaX/10.0f;
            f32 y_value = -1.0f*(ms_data->deltaY/10.0f);

            if(config_controller[gMouseSlot][CONTRPS_MOUSE_STICK][0] != CONTROLLER_PATCHER_INVALIDVALUE){
                if(config_controller[gMouseSlot][CONTRPS_MOUSE_STICK][1] == DEF_L_STICK){
                    buffer->lstick.x += x_value;
                    buffer->lstick.y += y_value;
                    return;
                }
            }

            buffer->rstick.x += x_value;
            buffer->rstick.y += y_value;

            changed = 1;
        }
    }else{
        int device = getDevice(hid);
        if(device == -1) return;

        if (hid & gHID_LIST_KEYBOARD){
            int w = 0;
            int s = 0;
            int a = 0;
            int d = 0;

            //Left Stick
            if(config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_X][0] != CONTROLLER_PATCHER_INVALIDVALUE){
                a = wasInKeyboard(src,config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_X][0]);
            }
            if(config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_X][1] != CONTROLLER_PATCHER_INVALIDVALUE){
                d = wasInKeyboard(src,config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_X][1]);
            }
            if(config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_Y][0] != CONTROLLER_PATCHER_INVALIDVALUE){
                w = wasInKeyboard(src,config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_Y][0]);
            }
            if(config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_Y][1] != CONTROLLER_PATCHER_INVALIDVALUE){
                s = wasInKeyboard(src,config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_Y][1]);
            }

            Vec2D stick = getAnalogValueByButtons(w,s,a,d,&changed);
            buffer->lstick.x += stick.x;
            buffer->lstick.y += stick.y;

            w = 0; s = 0; a = 0; d = 0;

            //Right Stick
            if(config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_X][0] != CONTROLLER_PATCHER_INVALIDVALUE){
                a = wasInKeyboard(src,config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_X][0]);
            }
            if(config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_X][1] != CONTROLLER_PATCHER_INVALIDVALUE){
                d = wasInKeyboard(src,config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_X][1]);
            }
            if(config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_Y][0] != CONTROLLER_PATCHER_INVALIDVALUE){
                w = wasInKeyboard(src,config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_Y][0]);
            }
            if(config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_Y][1] != CONTROLLER_PATCHER_INVALIDVALUE){
                s = wasInKeyboard(src,config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_Y][1]);
            }

            stick = getAnalogValueByButtons(w,s,a,d,&changed);
            buffer->rstick.x += stick.x;
            buffer->rstick.y += stick.y;
        }else{
            int deadzone = 0;

            if( config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_X][0] != CONTROLLER_PATCHER_INVALIDVALUE){
                if(config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_X_DEADZONE][0] == CONTROLLER_PATCHER_VALUE_SET){
                     deadzone = config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_X_DEADZONE][1];
                }

                buffer->lstick.x += convertAnalogValue(src[config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_X][0]],
                                                       config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_X][1],
                                                       config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_X_MINMAX][0],
                                                       config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_X_MINMAX][1],
                                                       config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_X_INVERT][1],
                                                       deadzone);
                changed = 1;
            }

            if( config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_Y][0] != CONTROLLER_PATCHER_INVALIDVALUE){
                deadzone = 0;
                if(config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_Y_DEADZONE][0] == CONTROLLER_PATCHER_VALUE_SET){
                     deadzone = config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_Y_DEADZONE][1];
                }
                buffer->lstick.y += convertAnalogValue(src[config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_Y][0]],
                                                       config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_Y][1],
                                                       config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_Y_MINMAX][0],
                                                       config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_Y_MINMAX][1],
                                                       config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_Y_INVERT][1],
                                                       deadzone);
                changed = 1;
            }

            if( config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_X][0] != CONTROLLER_PATCHER_INVALIDVALUE){
                deadzone = 0;
                if(config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_X_DEADZONE][0] == CONTROLLER_PATCHER_VALUE_SET){
                     deadzone = config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_X_DEADZONE][1];
                }

                buffer->rstick.x += convertAnalogValue(src[config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_X][0]],
                                                       config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_X][1],
                                                       config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_X_MINMAX][0],
                                                       config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_X_MINMAX][1],
                                                       config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_X_INVERT][1],
                                                       deadzone);
                changed = 1;
            }

            if( config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_Y][0] != CONTROLLER_PATCHER_INVALIDVALUE){
                deadzone = 0;
                if(config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_Y_DEADZONE][0] == CONTROLLER_PATCHER_VALUE_SET){
                     deadzone = config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_Y_DEADZONE][1];
                }

                buffer->rstick.y += convertAnalogValue(src[config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_Y][0]],
                                                       config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_Y][1],
                                                       config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_Y_MINMAX][0],
                                                       config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_Y_MINMAX][1],
                                                       config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_Y_INVERT][1],
                                                       deadzone);
                changed = 1;
            }

            /*log_printf("LX %f(%02X) LY %f(%02X) RX %f(%02X) RY %f(%02X)\n",buffer->lstick.x,src[config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_X][0]],
                                                                   buffer->lstick.y,src[config_controller[device][CONTRPS_VPAD_BUTTON_L_STICK_Y][0]],
                                                                   buffer->rstick.x,src[config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_X][0]],
                                                                   buffer->rstick.y,src[config_controller[device][CONTRPS_VPAD_BUTTON_R_STICK_Y][0]]);*/
        }
    }
}

int wasInKeyboard(unsigned char * src,int key){
    for(int i = 0;i<HID_KEYBOARD_DATA_LENGTH;i++){
        if(src[i] == 0 && i > 1){
             break;
        }else if (src[i] == key){
            return 1;
        }
    }
    return 0;
}


//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Mouse
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define MOUSE_XY_TYPE_A 0x01  //1 byte x, 1 byte y
#define MOUSE_XY_TYPE_B 0x02  //1 byte x, 2 byte y

void my_ms_read_cb(unsigned int handle, int error, unsigned char *buf, unsigned int bytes_transfered, void *p_user)
{
	if(error == 0)
	{
        //log_printf("%02X %02X %02X %02X %02X bytes_transfered: %d\n",buf[0],buf[1],buf[2],buf[3],buf[4],bytes_transfered);


        if(buf[0] == 2 && bytes_transfered > 3){ // using the other mouse mode
            buf +=1;
        }

        s8 x_value = 0;
        s8 y_value = 0;

        x_value = buf[1];
        y_value = buf[2];

        gHID_Mouse.pad_data[0].data[0].X += x_value;
        gHID_Mouse.pad_data[0].data[0].deltaX = x_value;

        gHID_Mouse.pad_data[0].data[0].Y += y_value;
        gHID_Mouse.pad_data[0].data[0].deltaY = y_value;

        gHID_Mouse.pad_data[0].data[0].left_click = buf[0];
	    gHID_Mouse.pad_data[0].data[0].right_click = buf[0]>>1;

        if(gHID_Mouse.pad_data[0].data[0].X < 0) gHID_Mouse.pad_data[0].data[0].X = 0;
        if(gHID_Mouse.pad_data[0].data[0].X > 1280) gHID_Mouse.pad_data[0].data[0].X = 1280;

        if(gHID_Mouse.pad_data[0].data[0].Y < 0) gHID_Mouse.pad_data[0].data[0].Y = 0;
        if(gHID_Mouse.pad_data[0].data[0].Y > 720) gHID_Mouse.pad_data[0].data[0].Y = 720;

        gHID_Mouse.pad_data[0].data[0].valuedChanged = 1;

        //log_printf("%02X %02X %02X %02X %02X %02X %02X %02X %d = X: %d Y: %d \n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],bytes_transfered,x_value,y_value);
        my_cb_user *usr = (my_cb_user*)p_user;
        HIDRead(handle, usr->buf, bytes_transfered, my_ms_read_cb, usr);
	}
}


//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! GC-Adapter
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void my_read_cb(unsigned int handle, int error, unsigned char *buf, unsigned int bytes_transfered, void *p_user)
{
	if(error == 0 && p_user != NULL && gHIDAttached)
	{
		my_cb_user *usr = (my_cb_user*)p_user;

		if(usr->hid == gHID_LIST_GC){
            memcpy(&(((HID_Data *)config_controller_data_ptr[usr->deviceSlot][0])->hid_data),&buf[1],9);
            memcpy(&(((HID_Data *)config_controller_data_ptr[usr->deviceSlot][1])->hid_data),&buf[10],9);
            memcpy(&(((HID_Data *)config_controller_data_ptr[usr->deviceSlot][2])->hid_data),&buf[19],9);
            memcpy(&(((HID_Data *)config_controller_data_ptr[usr->deviceSlot][3])->hid_data),&buf[28],9);


            /*
            int i = 0;
            log_printf("GC1: %02X %02X %02X %02X %02X %02X %02X %02X %02X ",buf[i*9+1],buf[i*9+2],buf[i*9+3],buf[i*9+4],buf[i*9+5],buf[i*9+6],buf[i*9+7],buf[i*9+8],buf[i*9+9]);i++;
            log_printf("GC2: %02X %02X %02X %02X %02X %02X %02X %02X %02X ",buf[i*9+1],buf[i*9+2],buf[i*9+3],buf[i*9+4],buf[i*9+5],buf[i*9+6],buf[i*9+7],buf[i*9+8],buf[i*9+9]);i++;
            log_printf("GC3: %02X %02X %02X %02X %02X %02X %02X %02X %02X ",buf[i*9+1],buf[i*9+2],buf[i*9+3],buf[i*9+4],buf[i*9+5],buf[i*9+6],buf[i*9+7],buf[i*9+8],buf[i*9+9]);i++;
            log_printf("GC4: %02X %02X %02X %02X %02X %02X %02X %02X %02X \n",buf[i*9+1],buf[i*9+2],buf[i*9+3],buf[i*9+4],buf[i*9+5],buf[i*9+6],buf[i*9+7],buf[i*9+8],buf[i*9+9]);*/

            HIDRead(handle, usr->buf, bytes_transfered, my_read_cb, usr);
            int pad = getActivePad(gHID_LIST_GC);

            //log_printf("Pad attached: %02X \n",getActivePad(gHID_LIST_GC));

            if((pad >= 0) && (((HID_Data *)config_controller_data_ptr[usr->deviceSlot][pad])->rumbleActive != usr->rumblestatus[pad]))
            {
                usr->rumblestatus[pad] = ((HID_Data *)config_controller_data_ptr[usr->deviceSlot][pad])->rumbleActive;
                HIDGCRumble(handle,usr,usr->rumblestatus[pad],pad);
            }
            else
            {
                HIDRead(handle, usr->buf, bytes_transfered, my_read_cb, usr);
            }

		}else if(usr->hid != 0){ //

            int size = (HID_MAX_DATA_LENGTH_PER_PAD > bytes_transfered)? bytes_transfered : HID_MAX_DATA_LENGTH_PER_PAD;
            int skip = 0;
            if(config_controller[usr->deviceSlot][CONTRPS_INPUT_FILTER][0] != CONTROLLER_PATCHER_INVALIDVALUE){
                if(buf[config_controller[usr->deviceSlot][CONTRPS_INPUT_FILTER][0]] != config_controller[usr->deviceSlot][CONTRPS_INPUT_FILTER][1]){
                    skip = 1;
                }
            }

           if(!skip){
                memcpy(&(((HID_Data *)config_controller_data_ptr[usr->deviceSlot][0])->hid_data),&buf[0],size);
                usleep(5000); //DS4 is way tooo fast (need to check the other pads)
                //log_printf("HID %02X %02X %02X %02X %02X %02X %02X %02X \n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]); //<-- only debug the pokken controller

            }
            HIDRead(handle, usr->buf, bytes_transfered, my_read_cb, usr);
		}
	}
}

void HIDGCRumble(unsigned int handle,my_cb_user *usr,int rumble, u32 pad)
{
	usr->buf[0] = 0x11;
    usr->buf[1] = (pad+1) & rumble;	usr->buf[2] = ((pad+1) >> 1) & rumble;
	usr->buf[3] = ((pad+1) >> 2) & rumble;
	usr->buf[4] = ((pad+1) >> 3) & rumble;
    HIDWrite(handle, usr->buf, 5, my_gc_write_cb, usr);
}

void my_gc_write_cb(unsigned int handle, int error, unsigned char *buf, unsigned int bytes_transfered, void *p_user)
{
	if(error == 0)
	{
		my_cb_user *usr = (my_cb_user*)p_user;
		HIDRead(handle, usr->buf, usr->transfersize, my_read_cb, usr);
	}
}
