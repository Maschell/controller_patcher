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

#ifndef _CONTROLLER_PATCHER_H_
#define _CONTROLLER_PATCHER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <gctypes.h>
#include "dynamic_libs/os_functions.h"
#include "dynamic_libs/syshid_functions.h"
#include "dynamic_libs/vpad_functions.h"

#define HID_DEBUG 0

typedef int HID_DEVICE_LIST;
#define HID_ALL_CONNECTED_DEVICES 0x10

#define HID_INIT_DONE   1
#define HID_SDCARD_READ 2

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Don't forget to change this data!
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
enum Controller_Stick_Defines
{
    STICK_CONF_MAGIC_VERSION,
    STICK_CONF_BYTE,
    STICK_CONF_DEFAULT,
    STICK_CONF_DEADZONE,
    STICK_CONF_INVERT,
    STICK_CONF_MIN,
    STICK_CONF_MAX,
    STICK_CONF_ENUM_MAXVALUE
};

#define STICK_CONF_MAGIC_VALUE 0xF0 // When you change the enum above, Dont forget to change the magic version!!!!

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! End
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define HID_MAX_DATA_LENGTH_PER_PAD             10
#define HID_MAX_PADS_COUNT                      4

typedef struct _HID_Data {
    u8 rumbleActive;
    u8 hid_data[HID_MAX_DATA_LENGTH_PER_PAD*2];
} HID_Data;

//! most data has the format: byte,value (byte starting at 0)
enum Controller_Patcher_Settings
{
    CONTRPS_VID,                          //! pid: 0x451d would be 0x45,0x1d
    CONTRPS_PID,                          //! vid: 0x488d would be 0x48,0x8d
    CONTRPS_BUF_SIZE,                     //! To set: CONTROLLER_PATCHER_VALUE_SET, BUF_SIZE (default is 64)
    CONTRPS_VPAD_BUTTON_A,
    CONTRPS_VPAD_BUTTON_B,
    CONTRPS_VPAD_BUTTON_X,
    CONTRPS_VPAD_BUTTON_Y,
    CONTRPS_DPAD_MODE,                     //! To set mode: CONTROLLER_PATCHER_VALUE_SET, Controller_Patcher_DPAD_MODE (default is normal mode)
    CONTRPS_DPAD_MASK,                     //! Mask needed for hat mode: CONTROLLER_PATCHER_VALUE_SET, mask
    /* Normal DPAD */
    CONTRPS_VPAD_BUTTON_LEFT,
    CONTRPS_VPAD_BUTTON_RIGHT,
    CONTRPS_VPAD_BUTTON_UP,
    CONTRPS_VPAD_BUTTON_DOWN,
    /* DPAD hat mode */
    CONTRPS_VPAD_BUTTON_DPAD_N,
    CONTRPS_VPAD_BUTTON_DPAD_NE,
    CONTRPS_VPAD_BUTTON_DPAD_E,
    CONTRPS_VPAD_BUTTON_DPAD_SE,
    CONTRPS_VPAD_BUTTON_DPAD_S,
    CONTRPS_VPAD_BUTTON_DPAD_SW,
    CONTRPS_VPAD_BUTTON_DPAD_W,
    CONTRPS_VPAD_BUTTON_DPAD_NW,
    CONTRPS_VPAD_BUTTON_DPAD_NEUTRAL,
    /* DPAD Absolute mode */
    CONTRPS_VPAD_BUTTON_DPAD_ABS_UP,
    CONTRPS_VPAD_BUTTON_DPAD_ABS_DOWN,
    CONTRPS_VPAD_BUTTON_DPAD_ABS_LEFT,
    CONTRPS_VPAD_BUTTON_DPAD_ABS_RIGHT,
    /* */
    CONTRPS_VPAD_BUTTON_ZL,
    CONTRPS_VPAD_BUTTON_ZR,
    CONTRPS_VPAD_BUTTON_L,
    CONTRPS_VPAD_BUTTON_R,
    CONTRPS_VPAD_BUTTON_PLUS,
    CONTRPS_VPAD_BUTTON_MINUS,
    CONTRPS_VPAD_BUTTON_HOME,
    CONTRPS_VPAD_BUTTON_SYNC,
    CONTRPS_VPAD_BUTTON_STICK_R,
    CONTRPS_VPAD_BUTTON_STICK_L,

    /*
    Currently not needed

    CONTRPS_VPAD_STICK_R_EMULATION_LEFT,
    CONTRPS_VPAD_STICK_R_EMULATION_RIGHT,
    CONTRPS_VPAD_STICK_R_EMULATION_UP,
    CONTRPS_VPAD_STICK_R_EMULATION_DOWN,
    CONTRPS_VPAD_STICK_L_EMULATION_LEFT,
    CONTRPS_VPAD_STICK_L_EMULATION_RIGHT,
    CONTRPS_VPAD_STICK_L_EMULATION_UP,
    CONTRPS_VPAD_STICK_L_EMULATION_DOWN,

    */
    CONTRPS_VPAD_BUTTON_L_STICK_X,          //! byte, default value
    CONTRPS_VPAD_BUTTON_L_STICK_X_INVERT,   //! To invert: CONTROLLER_PATCHER_VALUE_SET, 0x01
    CONTRPS_VPAD_BUTTON_L_STICK_X_DEADZONE, //! Deadzone
    CONTRPS_VPAD_BUTTON_L_STICK_X_MINMAX,   //! min,max
    CONTRPS_VPAD_BUTTON_L_STICK_Y,          //! byte, default value
    CONTRPS_VPAD_BUTTON_L_STICK_Y_INVERT,   //! To invert: CONTROLLER_PATCHER_VALUE_SET, 0x01
    CONTRPS_VPAD_BUTTON_L_STICK_Y_DEADZONE, //! Deadzone
    CONTRPS_VPAD_BUTTON_L_STICK_Y_MINMAX,   //! min,max
    CONTRPS_VPAD_BUTTON_R_STICK_X,          //! byte, default value
    CONTRPS_VPAD_BUTTON_R_STICK_X_INVERT,   //! To invert: CONTROLLER_PATCHER_VALUE_SET, 0x01
    CONTRPS_VPAD_BUTTON_R_STICK_X_DEADZONE, //! Deadzone
    CONTRPS_VPAD_BUTTON_R_STICK_X_MINMAX,   //! min,max
    CONTRPS_VPAD_BUTTON_R_STICK_Y,          //! byte, default value
    CONTRPS_VPAD_BUTTON_R_STICK_Y_INVERT,   //! To invert: CONTROLLER_PATCHER_VALUE_SET, 0x01
    CONTRPS_VPAD_BUTTON_R_STICK_Y_DEADZONE, //! Deadzone
    CONTRPS_VPAD_BUTTON_R_STICK_Y_MINMAX,   //! min,max
    CONTRPS_VPAD_BUTTON_TV,
    CONTRPS_DOUBLE_USE,                     //!When used: e.g. CONTROLLER_PATCHER_VALUE_SET, CONTROLLER_PATCHER_GC_DOUBLE_USE
    CONTRPS_DOUBLE_USE_BUTTON_ACTIVATOR,
    CONTRPS_DOUBLE_USE_BUTTON_1_PRESSED,
    CONTRPS_DOUBLE_USE_BUTTON_2_PRESSED,
    CONTRPS_DOUBLE_USE_BUTTON_3_PRESSED,
    CONTRPS_DOUBLE_USE_BUTTON_4_PRESSED,
    CONTRPS_DOUBLE_USE_BUTTON_5_PRESSED,
    CONTRPS_DOUBLE_USE_BUTTON_1_RELEASED,
    CONTRPS_DOUBLE_USE_BUTTON_2_RELEASED,
    CONTRPS_DOUBLE_USE_BUTTON_3_RELEASED,
    CONTRPS_DOUBLE_USE_BUTTON_4_RELEASED,
    CONTRPS_DOUBLE_USE_BUTTON_5_RELEASED,
    CONTRPS_PAD_COUNT,                      //!
    CONTRPS_INPUT_FILTER,                   //!
    CONTRPS_MOUSE_STICK,
    CONTRPS_MAX_VALUE
};

enum Controller_Patcher_DPAD_MODE
{
    CONTRPDM_Normal,
    CONTRPDM_Hat,
    CONTRPDM_Absolute_2Values,
};
enum Controller_Patcher_DPAD_Settings
{
    CONTRDPAD_MODE,
    CONTRDPAD_MASK,
};

#define CONTROLLER_PATCHER_VALUE_SET            0x01
#define CONTROLLER_PATCHER_GC_DOUBLE_USE        0x01
#define CONTROLLER_PATCHER_INVALIDVALUE         0xFF

typedef struct _my_cb_user{
	unsigned char *buf;
	unsigned int transfersize;
	unsigned int rumblestatus[4];
	unsigned int handle;
	unsigned int hid;
	unsigned int deviceSlot;
}my_cb_user;

typedef struct _HID_DEVICE_DATA {
    my_cb_user * user_data;
    HID_Data pad_data[HID_MAX_PADS_COUNT];
} HID_DEVICE_DATA;



typedef struct _HID_Data_Struct{
	unsigned char *src;
	int hid;
}HID_Data_Struct;

void setConfigValue(u8 * dest , u8 first, u8 second);
void init_config_controller();
void init_button_remapping();
void deinit_config_controller();
u32 getNextDeviceSlot();
void printButtons(VPADData * buffer);
void buttonRemapping(VPADData * buffer);
void setButtonRemappingData(VPADData * buffer, VPADData * new_data,u32 VPADButton, int CONTRPS_Value);
void setHoldButtonData(VPADData * buffer, VPADData * new_data,u32 oldVPADButton,u32 newVPADButton);

void setControllerReleasePressData(HID_Data_Struct data_cur, HID_Data_Struct data_last,VPADData * buffer,int VPADButton);
void setControllerDataFromHID(VPADData * buffer,HID_DEVICE_LIST hid_devices);
int checkDoubleUseGC();
void checkMouseMode(HID_Data_Struct data, HID_Data_Struct data_last);

void my_read_cb(unsigned int handle, int error, unsigned char *buf, unsigned int bytes_transfered, void *p_user);

int wasInKeyboard(unsigned char * src,int key);

const char *byte_to_binary(int x);

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! GC-Adapter
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define HID_GC_BUTTON_A_VALUE                 0x01
#define HID_GC_BUTTON_B_VALUE                 0x02
#define HID_GC_BUTTON_X_VALUE                 0x04
#define HID_GC_BUTTON_Y_VALUE                 0x08
#define HID_GC_BUTTON_LEFT_VALUE              0x10
#define HID_GC_BUTTON_RIGHT_VALUE             0x20
#define HID_GC_BUTTON_DOWN_VALUE              0x40
#define HID_GC_BUTTON_UP_VALUE                0x80

#define HID_GC_BUTTON_START_VALUE             0x01
#define HID_GC_BUTTON_L_VALUE                 0x80
#define HID_GC_BUTTON_R_VALUE                 0x80
#define HID_GC_BUTTON_Z_VALUE                 0x02

#define HID_GC_PAD_COUNT             4

void HIDGCRumble(unsigned int handle,my_cb_user *usr,int rumble, u32 pad);
void my_gc_write_cb(unsigned int handle, int error, unsigned char *buf, unsigned int bytes_transfered, void *p_user);

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! DS3
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define PS3_F4_REPORT_LEN               4
#define PS3_F5_REPORT_LEN               8
#define PS3_01_REPORT_LEN               48
#define HID_REPORT_FEATURE              3
#define HID_REPORT_OUTPUT               2
#define PS3_F4_REPORT_ID                0xF4
#define PS3_01_REPORT_ID                0x01
#define PS3_F5_REPORT_ID                0xF5

#define HID_DS3_BUTTON_CROSS_VALUE            0x40 // 3
#define HID_DS3_BUTTON_CIRCLE_VALUE           0x20 // 3
#define HID_DS3_BUTTON_SQUARE_VALUE           0x80 // 3
#define HID_DS3_BUTTON_TRIANGLE_VALUE         0x10 // 3
#define HID_DS3_BUTTON_L1_VALUE               0x04 // 3
#define HID_DS3_BUTTON_L2_VALUE               0x01 // 3
#define HID_DS3_BUTTON_R1_VALUE               0x08 // 3
#define HID_DS3_BUTTON_R2_VALUE               0x02 // 3

#define HID_DS3_BUTTON_L3_VALUE               0x02 // 2
#define HID_DS3_BUTTON_R3_VALUE               0x04 // 2
#define HID_DS3_BUTTON_SELECT_VALUE           0x01 // 2
#define HID_DS3_BUTTON_START_VALUE            0x08 // 2
#define HID_DS3_BUTTON_LEFT_VALUE             0x80 // 2
#define HID_DS3_BUTTON_RIGHT_VALUE            0x20 // 2
#define HID_DS3_BUTTON_UP_VALUE               0x10 // 2
#define HID_DS3_BUTTON_DOWN_VALUE             0x40 // 2
#define HID_DS3_BUTTON_GUIDE_VALUE            0x01 // 4


#define HID_DS3_PAD_COUNT             1

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! DS4
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define HID_DS4_BUTTON_CROSS_VALUE            0x20 // 5
#define HID_DS4_BUTTON_SQUARE_VALUE           0x10 // 5
#define HID_DS4_BUTTON_CIRCLE_VALUE           0x40 // 5
#define HID_DS4_BUTTON_TRIANGLE_VALUE         0x80 // 5
#define HID_DS4_BUTTON_L1_VALUE               0x01 // 6
#define HID_DS4_BUTTON_L2_VALUE               0x04 // 6
#define HID_DS4_BUTTON_L3_VALUE               0x40 // 6
#define HID_DS4_BUTTON_R1_VALUE               0x02 // 6
#define HID_DS4_BUTTON_R2_VALUE               0x08 // 6
#define HID_DS4_BUTTON_R3_VALUE               0x80 // 6
#define HID_DS4_BUTTON_SHARE_VALUE            0x10 // 6
#define HID_DS4_BUTTON_OPTIONS_VALUE          0x20 // 6

#define HID_DS4_BUTTON_DPAD_MASK_VALUE        0x0F

#define HID_DS4_BUTTON_DPAD_N_VALUE           0x00 // 5
#define HID_DS4_BUTTON_DPAD_NE_VALUE          0x01 // 5
#define HID_DS4_BUTTON_DPAD_E_VALUE           0x02 // 5
#define HID_DS4_BUTTON_DPAD_SE_VALUE          0x03 // 5
#define HID_DS4_BUTTON_DPAD_S_VALUE           0x04 // 5
#define HID_DS4_BUTTON_DPAD_SW_VALUE          0x05 // 5
#define HID_DS4_BUTTON_DPAD_W_VALUE           0x06 // 5
#define HID_DS4_BUTTON_DPAD_NW_VALUE          0x07 // 5
#define HID_DS4_BUTTON_DPAD_NEUTRAL_VALUE     0x08 // 5

#define HID_DS4_BUTTON_GUIDE_VALUE            0x01 // 7
#define HID_DS4_BUTTON_T_PAD_CLICK_VALUE      0x02 // 7

#define HID_DS4_PAD_COUNT               1

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Keyboard (Full list is on: http://www.freebsddiary.org/APC/usb_hid_usages.php)
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define HID_KEYBOARD_BUTTON_SHIFT             0x02

#define HID_KEYBOARD_BUTTON_A                 0x04
#define HID_KEYBOARD_BUTTON_B                 0x05
#define HID_KEYBOARD_BUTTON_C                 0x06
#define HID_KEYBOARD_BUTTON_D                 0x07
#define HID_KEYBOARD_BUTTON_E                 0x08
#define HID_KEYBOARD_BUTTON_F                 0x09
#define HID_KEYBOARD_BUTTON_G                 0x0A
#define HID_KEYBOARD_BUTTON_H                 0x0B
#define HID_KEYBOARD_BUTTON_I                 0x0C
#define HID_KEYBOARD_BUTTON_J                 0x0D
#define HID_KEYBOARD_BUTTON_K                 0x0E
#define HID_KEYBOARD_BUTTON_L                 0x0F
#define HID_KEYBOARD_BUTTON_M                 0x10
#define HID_KEYBOARD_BUTTON_N                 0x11
#define HID_KEYBOARD_BUTTON_O                 0x12
#define HID_KEYBOARD_BUTTON_P                 0x13
#define HID_KEYBOARD_BUTTON_Q                 0x14
#define HID_KEYBOARD_BUTTON_R                 0x15
#define HID_KEYBOARD_BUTTON_S                 0x16
#define HID_KEYBOARD_BUTTON_T                 0x17
#define HID_KEYBOARD_BUTTON_U                 0x18
#define HID_KEYBOARD_BUTTON_V                 0x19
#define HID_KEYBOARD_BUTTON_W                 0x1A
#define HID_KEYBOARD_BUTTON_X                 0x1B
#define HID_KEYBOARD_BUTTON_Y                 0x1C
#define HID_KEYBOARD_BUTTON_Z                 0x1D
#define HID_KEYBOARD_BUTTON_F1                0x3A
#define HID_KEYBOARD_BUTTON_F2                0x3B
#define HID_KEYBOARD_BUTTON_F3                0x3C
#define HID_KEYBOARD_BUTTON_F4                0x3D
#define HID_KEYBOARD_BUTTON_F5                0x3E
#define HID_KEYBOARD_BUTTON_F6                0x3F
#define HID_KEYBOARD_BUTTON_F7                0x40
#define HID_KEYBOARD_BUTTON_F8                0x41
#define HID_KEYBOARD_BUTTON_F9                0x42
#define HID_KEYBOARD_BUTTON_F10               0x43
#define HID_KEYBOARD_BUTTON_F11               0x44
#define HID_KEYBOARD_BUTTON_F12               0x45
#define HID_KEYBOARD_BUTTON_1                 0x1E
#define HID_KEYBOARD_BUTTON_2                 0x1F
#define HID_KEYBOARD_BUTTON_3                 0x20
#define HID_KEYBOARD_BUTTON_4                 0x21
#define HID_KEYBOARD_BUTTON_5                 0x22
#define HID_KEYBOARD_BUTTON_6                 0x23
#define HID_KEYBOARD_BUTTON_7                 0x24
#define HID_KEYBOARD_BUTTON_8                 0x25
#define HID_KEYBOARD_BUTTON_9                 0x26
#define HID_KEYBOARD_BUTTON_0                 0x27

#define HID_KEYBOARD_BUTTON_RETURN            0x28
#define HID_KEYBOARD_BUTTON_ESCAPE            0x29
#define HID_KEYBOARD_BUTTON_DELETE            0x2A
#define HID_KEYBOARD_BUTTON_TAB               0x2B
#define HID_KEYBOARD_BUTTON_SPACEBAR          0x2C
#define HID_KEYBOARD_BUTTON_CAPSLOCK          0x39
#define HID_KEYBOARD_BUTTON_PRINTSCREEN       0x46
#define HID_KEYBOARD_BUTTON_SCROLLLOCK        0x47
#define HID_KEYBOARD_BUTTON_PAUSE             0x48
#define HID_KEYBOARD_BUTTON_INSERT            0x49
#define HID_KEYBOARD_BUTTON_HOME              0x4A
#define HID_KEYBOARD_BUTTON_PAGEUP            0x4B
#define HID_KEYBOARD_BUTTON_PAGEDOWN          0x4E
#define HID_KEYBOARD_BUTTON_DELETEFORWARD     0x4C
#define HID_KEYBOARD_BUTTON_END               0x4D
#define HID_KEYBOARD_BUTTON_LEFT_CONTROL      0xE0
#define HID_KEYBOARD_BUTTON_LEFT_ALT          0xE2
#define HID_KEYBOARD_BUTTON_RIGHT_CONTROL     0xE4
#define HID_KEYBOARD_BUTTON_RIGHT_SHIFT       0xE5
#define HID_KEYBOARD_BUTTON_RIGHT_ALT         0xE6

#define HID_KEYBOARD_BUTTON_LEFT              0x50
#define HID_KEYBOARD_BUTTON_RIGHT             0x4f
#define HID_KEYBOARD_BUTTON_DOWN              0x51
#define HID_KEYBOARD_BUTTON_UP                0x52

#define HID_KEYBOARD_KEYPAD_BUTTON_1          0x59
#define HID_KEYBOARD_KEYPAD_BUTTON_2          0x5A
#define HID_KEYBOARD_KEYPAD_BUTTON_3          0x5B
#define HID_KEYBOARD_KEYPAD_BUTTON_4          0x5C
#define HID_KEYBOARD_KEYPAD_BUTTON_5          0x5D
#define HID_KEYBOARD_KEYPAD_BUTTON_6          0x5E
#define HID_KEYBOARD_KEYPAD_BUTTON_7          0x5F
#define HID_KEYBOARD_KEYPAD_BUTTON_8          0x60
#define HID_KEYBOARD_KEYPAD_BUTTON_9          0x61
#define HID_KEYBOARD_KEYPAD_BUTTON_0          0x62
#define HID_KEYBOARD_KEYPAD_BUTTON_NUMLOCK    0x53
#define HID_KEYBOARD_KEYPAD_BUTTON_MINUS      0x56
#define HID_KEYBOARD_KEYPAD_BUTTON_PLUS       0x57

#define HID_KEYBOARD_PAD_COUNT             1
#define HID_KEYBOARD_DATA_LENGTH           8

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Mouse
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define HID_MOUSE_BUTTON_LEFTCLICK                 0x04
#define HID_MOUSE_BUTTON_RIGHTCLICK                0x05


#define HID_MOUSE_PAD_COUNT             1

#define HID_MOUSE_MODE_AIM                0x01
#define HID_MOUSE_MODE_TOUCH              0x02

struct _HID_Mouse_Data {
    u8 left_click;
    u8 right_click;
    s16 X;
    s16 Y;
    s16 deltaX;
    s16 deltaY;
    u8 valuedChanged;
} __attribute__ ((packed));

typedef struct _HID_Mouse_Data HID_Mouse_Data;

typedef struct _HID_Mouse_Data_c {
    HID_Mouse_Data data[2];
} HID_Mouse_Data_c;

typedef struct _HID_Mouse {
    my_cb_user * user_data;
    HID_Mouse_Data_c pad_data[HID_MOUSE_PAD_COUNT];
} HID_Mouse;

void my_ms_read_cb(unsigned int handle, int error, unsigned char *buf, unsigned int bytes_transfered, void *p_user);
void my_ms_write_cb(unsigned int handle, int error, unsigned char *buf, unsigned int bytes_transfered, void *p_user);
void my_ms_report_cb(unsigned int handle, int error, unsigned char *buf, unsigned int bytes_transfered, void *p_user);

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! Shared
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------


int my_attach_cb(HIDClient *p_client, HIDDevice *p_device, unsigned int attach);

int getPadCount(int hid);
HID_Data_Struct * getLastData(HID_Data_Struct data);
int getButtonPressed(HID_Data_Struct data, int VPADButton);
void setLastData(HID_Data_Struct data,HID_Data_Struct data_last);
int getActivePad(int hid);
void setRumble(int hid,int rumble);
void convertAnalogSticks(HID_Data_Struct data,VPADData * buffer);
void setTouch(HID_Data_Struct data,VPADData * buffer);

HID_Data_Struct * getHIDDataAll(int hid,int * size);

#ifdef __cplusplus
}
#endif

#endif /* _CONTROLLER_PATCHER_H_ */
