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

/**
 * @file ControllerPatcherHID.hpp
 * @author Maschell
 * @date 25 Aug 2016
 * \brief This files contain useful all function for the direct HID Access
 *
 * @see https://github.com/Maschell/controller_patcher
 */

#ifndef _CONTROLLER_PATCHER_HID_H_
#define _CONTROLLER_PATCHER_HID_H_

#include <vector>

#include "../ControllerPatcherIncludes.hpp"

//! Own definitions
#define VPAD_BUTTON_TOUCH               0x00080000
#define VPAD_MASK_EMULATED_STICKS       0x7F800000
#define VPAD_MASK_BUTTONS ~VPAD_MASK_EMULATED_STICKS


#define SWAP16(x) ((x>>8) | ((x&0xFF)<<8))
#define SWAP8(x) ((x>>4) | ((x&0xF)<<4))

class ControllerPatcherHID {
    friend class ControllerPatcher;
    friend class ControllerPatcherUtils;
public:
    static int32_t  externAttachDetachCallback(HIDDevice *p_device, HIDAttachEvent attach);
    static void externHIDReadCallback(uint32_t handle, unsigned char *buf, uint32_t bytes_transfered, my_cb_user * usr);

private:
    static CONTROLLER_PATCHER_RESULT_OR_ERROR setVPADControllerData(VPADStatus * buffer,std::vector<HID_Data *>& data);
    static std::vector<HID_Data *> getHIDDataAll();
    static CONTROLLER_PATCHER_RESULT_OR_ERROR getHIDData(uint32_t hidmask, int32_t pad,  HID_Data ** data);

    /*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
     * Rumble
     *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

    static void HIDRumble(uint32_t handle,my_cb_user *usr,uint32_t pad);

    static void HIDGCRumble(uint32_t handle,my_cb_user *usr);

    static void HIDDS3Rumble(uint32_t handle,my_cb_user *usr,int32_t rumble);

    /*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
     * HID Callbacks
     *--------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
    static int32_t myAttachDetachCallback(HIDClient *p_client, HIDDevice *p_device, HIDAttachEvent attach);

    static void myHIDMouseReadCallback(uint32_t handle, int32_t error, unsigned char *buf, uint32_t bytes_transfered, void *p_user);
    static void myHIDReadCallback(uint32_t handle, int32_t error, unsigned char *buf, uint32_t bytes_transfered, void *p_user);

    static int32_t AttachDetachCallback(HIDClient *p_client, HIDDevice *p_device, HIDAttachEvent attach);
    static void HIDReadCallback(uint32_t handle, unsigned char *buf, uint32_t bytes_transfered, my_cb_user * usr);
};

#endif /* _CONTROLLER_PATCHER_HID_H_ */
