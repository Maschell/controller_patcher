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

#include "pad_const.h"

const u8 DEF_L_STICK_UP =     201;
const u8 DEF_L_STICK_DOWN =   202;
const u8 DEF_L_STICK_LEFT =   203;
const u8 DEF_L_STICK_RIGHT =  204;

const u8 DEF_R_STICK_UP =     205;
const u8 DEF_R_STICK_DOWN =   206;
const u8 DEF_R_STICK_LEFT =   207;
const u8 DEF_R_STICK_RIGHT =  208;

const u8 DEF_R_STICK =        220;
const u8 DEF_L_STICK =        221;

const u8 DEF_STICK_OFFSET_INVERT =      CONTRPS_VPAD_BUTTON_L_STICK_X_INVERT - CONTRPS_VPAD_BUTTON_L_STICK_X;
const u8 DEF_STICK_OFFSET_DEADZONE =    CONTRPS_VPAD_BUTTON_L_STICK_X_DEADZONE - CONTRPS_VPAD_BUTTON_L_STICK_X;
const u8 DEF_STICK_OFFSET_MINMAX =      CONTRPS_VPAD_BUTTON_L_STICK_X_MINMAX - CONTRPS_VPAD_BUTTON_L_STICK_X;


//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! GC-Adapter
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const u8 HID_GC_BUTTON_A[]      = { 0x01,HID_GC_BUTTON_A_VALUE};
const u8 HID_GC_BUTTON_B[]      = { 0x01,HID_GC_BUTTON_B_VALUE};
const u8 HID_GC_BUTTON_X[]      = { 0x01,HID_GC_BUTTON_X_VALUE};
const u8 HID_GC_BUTTON_Y[]      = { 0x01,HID_GC_BUTTON_Y_VALUE};
const u8 HID_GC_BUTTON_LEFT[]   = { 0x01,HID_GC_BUTTON_LEFT_VALUE};
const u8 HID_GC_BUTTON_RIGHT[]  = { 0x01,HID_GC_BUTTON_RIGHT_VALUE};
const u8 HID_GC_BUTTON_DOWN[]   = { 0x01,HID_GC_BUTTON_DOWN_VALUE};
const u8 HID_GC_BUTTON_UP[]     = { 0x01,HID_GC_BUTTON_UP_VALUE};

const u8 HID_GC_BUTTON_START[]  = { 0x02,HID_GC_BUTTON_START_VALUE};
const u8 HID_GC_BUTTON_Z[]      = { 0x02,HID_GC_BUTTON_Z_VALUE};

const u8 HID_GC_BUTTON_L[]      = { 0x07,HID_GC_BUTTON_L_VALUE};
const u8 HID_GC_BUTTON_R[]      = { 0x08,HID_GC_BUTTON_R_VALUE};

const u8 HID_GC_BUTTON_DPAD_TYPE[]     = { CONTRPDM_Normal,0x00};

const u8 HID_GC_STICK_L_X[STICK_CONF_ENUM_MAXVALUE] =  {    STICK_CONF_MAGIC_VALUE, //STICK_CONF_MAGIC_VERSION
                                                            0x03, //STICK_CONF_BYTE,
                                                            0x80, //STICK_CONF_DEFAULT,
                                                            0x09, //STICK_CONF_DEADZONE,
                                                            0x00, //STICK_CONF_INVERT,
                                                            0x1A, //STICK_CONF_MIN,
                                                            0xE4};//STICK_CONF_MAX,

const u8 HID_GC_STICK_L_Y[STICK_CONF_ENUM_MAXVALUE] =  {    STICK_CONF_MAGIC_VALUE, //STICK_CONF_MAGIC_VERSION
                                                            0x04, //STICK_CONF_BYTE,
                                                            0x80, //STICK_CONF_DEFAULT,
                                                            0x09, //STICK_CONF_DEADZONE,
                                                            0x00, //STICK_CONF_INVERT,
                                                            0x11, //STICK_CONF_MIN,
                                                            0xE1};//STICK_CONF_MAX,

const u8 HID_GC_STICK_R_X[STICK_CONF_ENUM_MAXVALUE] =  {    STICK_CONF_MAGIC_VALUE, //STICK_CONF_MAGIC_VERSION
                                                            0x05, //STICK_CONF_BYTE,
                                                            0x80, //STICK_CONF_DEFAULT,
                                                            0x09, //STICK_CONF_DEADZONE,
                                                            0x00, //STICK_CONF_INVERT,
                                                            0x2B, //STICK_CONF_MIN,
                                                            0xE2};//STICK_CONF_MAX,

const u8 HID_GC_STICK_R_Y[STICK_CONF_ENUM_MAXVALUE] =  {    STICK_CONF_MAGIC_VALUE, //STICK_CONF_MAGIC_VERSION
                                                            0x06, //STICK_CONF_BYTE,
                                                            0x80, //STICK_CONF_DEFAULT,
                                                            0x09, //STICK_CONF_DEADZONE,
                                                            0x00, //STICK_CONF_INVERT,
                                                            0x1D, //STICK_CONF_MIN,
                                                            0xDB};//STICK_CONF_MAX,

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! DS3
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const u8 HID_DS3_BUTTON_CROSS[]     = { 0x03,HID_DS3_BUTTON_CROSS_VALUE};
const u8 HID_DS3_BUTTON_CIRCLE[]    = { 0x03,HID_DS3_BUTTON_CIRCLE_VALUE};
const u8 HID_DS3_BUTTON_SQUARE []   = { 0x03,HID_DS3_BUTTON_SQUARE_VALUE};
const u8 HID_DS3_BUTTON_TRIANGLE[]  = { 0x03,HID_DS3_BUTTON_TRIANGLE_VALUE};

const u8 HID_DS3_BUTTON_L1[]        = { 0x03,HID_DS3_BUTTON_L1_VALUE};
const u8 HID_DS3_BUTTON_L2[]        = { 0x03,HID_DS3_BUTTON_L2_VALUE};
const u8 HID_DS3_BUTTON_R1[]        = { 0x03,HID_DS3_BUTTON_R1_VALUE};
const u8 HID_DS3_BUTTON_R2[]        = { 0x03,HID_DS3_BUTTON_R2_VALUE};

const u8 HID_DS3_BUTTON_L3[]        = { 0x02,HID_DS3_BUTTON_L3_VALUE};
const u8 HID_DS3_BUTTON_R3[]        = { 0x02,HID_DS3_BUTTON_R3_VALUE};
const u8 HID_DS3_BUTTON_SELECT[]    = { 0x02,HID_DS3_BUTTON_SELECT_VALUE};
const u8 HID_DS3_BUTTON_START[]     = { 0x02,HID_DS3_BUTTON_START_VALUE};
const u8 HID_DS3_BUTTON_LEFT[]      = { 0x02,HID_DS3_BUTTON_LEFT_VALUE};
const u8 HID_DS3_BUTTON_RIGHT[]     = { 0x02,HID_DS3_BUTTON_RIGHT_VALUE};
const u8 HID_DS3_BUTTON_UP[]        = { 0x02,HID_DS3_BUTTON_UP_VALUE};
const u8 HID_DS3_BUTTON_DOWN[]      = { 0x02,HID_DS3_BUTTON_DOWN_VALUE};

const u8 HID_DS3_BUTTON_GUIDE[]      = { 0x04,HID_DS3_BUTTON_GUIDE_VALUE};

const u8 HID_DS3_BUTTON_DPAD_TYPE[]     = { CONTRPDM_Normal,0x00};

const u8 HID_DS3_STICK_L_X[STICK_CONF_ENUM_MAXVALUE] =  {   STICK_CONF_MAGIC_VALUE, //STICK_CONF_MAGIC_VERSION
                                                            0x06, //STICK_CONF_BYTE,
                                                            0x80, //STICK_CONF_DEFAULT,
                                                            0x06, //STICK_CONF_DEADZONE,
                                                            0x00, //STICK_CONF_INVERT,
                                                            0x00, //STICK_CONF_MIN,
                                                            0xFF};//STICK_CONF_MAX,

const u8 HID_DS3_STICK_L_Y[STICK_CONF_ENUM_MAXVALUE] =  {   STICK_CONF_MAGIC_VALUE, //STICK_CONF_MAGIC_VERSION
                                                            0x07, //STICK_CONF_BYTE,
                                                            0x80, //STICK_CONF_DEFAULT,
                                                            0x06, //STICK_CONF_DEADZONE,
                                                            0x01, //STICK_CONF_INVERT,
                                                            0x00, //STICK_CONF_MIN,
                                                            0xFF};//STICK_CONF_MAX,

const u8 HID_DS3_STICK_R_X[STICK_CONF_ENUM_MAXVALUE] =  {   STICK_CONF_MAGIC_VALUE, //STICK_CONF_MAGIC_VERSION
                                                            0x08, //STICK_CONF_BYTE,
                                                            0x80, //STICK_CONF_DEFAULT,
                                                            0x06, //STICK_CONF_DEADZONE,
                                                            0x00, //STICK_CONF_INVERT,
                                                            0x00, //STICK_CONF_MIN,
                                                            0xFF};//STICK_CONF_MAX,

const u8 HID_DS3_STICK_R_Y[STICK_CONF_ENUM_MAXVALUE] =  {   STICK_CONF_MAGIC_VALUE, //STICK_CONF_MAGIC_VERSION
                                                            0x09, //STICK_CONF_BYTE,
                                                            0x80, //STICK_CONF_DEFAULT,
                                                            0x06, //STICK_CONF_DEADZONE,
                                                            0x01, //STICK_CONF_INVERT,
                                                            0x00, //STICK_CONF_MIN,
                                                            0xFF};//STICK_CONF_MAX,

//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//! DS4
//!----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const u8 HID_DS4_BUTTON_CROSS[]     = { 0x05,HID_DS4_BUTTON_CROSS_VALUE};
const u8 HID_DS4_BUTTON_CIRCLE[]    = { 0x05,HID_DS4_BUTTON_CIRCLE_VALUE};
const u8 HID_DS4_BUTTON_SQUARE []   = { 0x05,HID_DS4_BUTTON_SQUARE_VALUE};
const u8 HID_DS4_BUTTON_TRIANGLE[]  = { 0x05,HID_DS4_BUTTON_TRIANGLE_VALUE};

const u8 HID_DS4_BUTTON_L1[]        = { 0x06,HID_DS4_BUTTON_L1_VALUE};
const u8 HID_DS4_BUTTON_L2[]        = { 0x06,HID_DS4_BUTTON_L2_VALUE};
const u8 HID_DS4_BUTTON_L3[]        = { 0x06,HID_DS4_BUTTON_L3_VALUE};

const u8 HID_DS4_BUTTON_R1[]        = { 0x06,HID_DS4_BUTTON_R1_VALUE};
const u8 HID_DS4_BUTTON_R2[]        = { 0x06,HID_DS4_BUTTON_R2_VALUE};
const u8 HID_DS4_BUTTON_R3[]        = { 0x06,HID_DS4_BUTTON_R3_VALUE};

const u8 HID_DS4_BUTTON_SHARE[]     = { 0x06,HID_DS4_BUTTON_SHARE_VALUE};
const u8 HID_DS4_BUTTON_OPTIONS[]   = { 0x06,HID_DS4_BUTTON_OPTIONS_VALUE};


const u8 HID_DS4_BUTTON_DPAD_TYPE[]     = { CONTRPDM_Hat,HID_DS4_BUTTON_DPAD_MASK_VALUE};
const u8 HID_DS4_BUTTON_DPAD_N[]        = { 0x05,HID_DS4_BUTTON_DPAD_N_VALUE};
const u8 HID_DS4_BUTTON_DPAD_NE[]       = { 0x05,HID_DS4_BUTTON_DPAD_NE_VALUE};
const u8 HID_DS4_BUTTON_DPAD_E[]        = { 0x05,HID_DS4_BUTTON_DPAD_E_VALUE};
const u8 HID_DS4_BUTTON_DPAD_SE[]       = { 0x05,HID_DS4_BUTTON_DPAD_SE_VALUE};
const u8 HID_DS4_BUTTON_DPAD_S[]        = { 0x05,HID_DS4_BUTTON_DPAD_S_VALUE};
const u8 HID_DS4_BUTTON_DPAD_SW[]       = { 0x05,HID_DS4_BUTTON_DPAD_SW_VALUE};
const u8 HID_DS4_BUTTON_DPAD_W[]        = { 0x05,HID_DS4_BUTTON_DPAD_W_VALUE};
const u8 HID_DS4_BUTTON_DPAD_NW[]       = { 0x05,HID_DS4_BUTTON_DPAD_NW_VALUE};
const u8 HID_DS4_BUTTON_DPAD_NEUTRAL[]  = { 0x05,HID_DS4_BUTTON_DPAD_NEUTRAL_VALUE};

const u8 HID_DS4_BUTTON_GUIDE[]          = { 0x07,HID_DS4_BUTTON_GUIDE_VALUE};
const u8 HID_DS4_BUTTON_T_PAD_CLICK[]    = { 0x07,HID_DS4_BUTTON_T_PAD_CLICK_VALUE};

const u8 HID_DS4_STICK_L_X[STICK_CONF_ENUM_MAXVALUE] =  {   STICK_CONF_MAGIC_VALUE, //STICK_CONF_MAGIC_VERSION
                                                            0x01, //STICK_CONF_BYTE,
                                                            0x80, //STICK_CONF_DEFAULT,
                                                            0x06, //STICK_CONF_DEADZONE,
                                                            0x00, //STICK_CONF_INVERT,
                                                            0x00, //STICK_CONF_MIN,
                                                            0xFF};//STICK_CONF_MAX,

const u8 HID_DS4_STICK_L_Y[STICK_CONF_ENUM_MAXVALUE] =  {   STICK_CONF_MAGIC_VALUE, //STICK_CONF_MAGIC_VERSION
                                                            0x02, //STICK_CONF_BYTE,
                                                            0x80, //STICK_CONF_DEFAULT,
                                                            0x05, //STICK_CONF_DEADZONE,
                                                            0x01, //STICK_CONF_INVERT,
                                                            0x00, //STICK_CONF_MIN,
                                                            0xFF};//STICK_CONF_MAX,

const u8 HID_DS4_STICK_R_X[STICK_CONF_ENUM_MAXVALUE] =  {   STICK_CONF_MAGIC_VALUE, //STICK_CONF_MAGIC_VERSION
                                                            0x03, //STICK_CONF_BYTE,
                                                            0x80, //STICK_CONF_DEFAULT,
                                                            0x07, //STICK_CONF_DEADZONE,
                                                            0x00, //STICK_CONF_INVERT,
                                                            0x00, //STICK_CONF_MIN,
                                                            0xFF};//STICK_CONF_MAX,

const u8 HID_DS4_STICK_R_Y[STICK_CONF_ENUM_MAXVALUE] =  {   STICK_CONF_MAGIC_VALUE, //STICK_CONF_MAGIC_VERSION
                                                            0x04, //STICK_CONF_BYTE,
                                                            0x80, //STICK_CONF_DEFAULT,
                                                            0x09, //STICK_CONF_DEADZONE,
                                                            0x01, //STICK_CONF_INVERT,
                                                            0x00, //STICK_CONF_MIN,
                                                            0xFF};//STICK_CONF_MAX,


