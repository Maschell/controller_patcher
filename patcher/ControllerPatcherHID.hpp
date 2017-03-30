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

#include <gctypes.h>
#include <vector>
#include "dynamic_libs/syshid_functions.h"
#include "dynamic_libs/vpad_functions.h"
#include "./ControllerPatcherUtils.hpp"
#include "./ControllerPatcherDefs.h"

#define SWAP16(x) ((x>>8) | ((x&0xFF)<<8))
#define SWAP8(x) ((x>>4) | ((x&0xF)<<4))

class ControllerPatcherHID{
        friend class ControllerPatcher;
        friend class ControllerPatcherUtils;
    public:
        static int  externAttachDetachCallback(HIDDevice *p_device, unsigned int attach);
        static void externHIDReadCallback(unsigned int handle, unsigned char *buf, unsigned int bytes_transfered, my_cb_user * usr);

    private:
        static CONTROLLER_PATCHER_RESULT_OR_ERROR setVPADControllerData(VPADData * buffer,std::vector<HID_Data *>& data);
        static std::vector<HID_Data *> getHIDDataAll();
        static CONTROLLER_PATCHER_RESULT_OR_ERROR getHIDData(int hidmask, int pad,  HID_Data ** data);

/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * Rumble
 *---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

        static void HIDRumble(unsigned int handle,my_cb_user *usr,u32 pad);

        static void HIDGCRumble(unsigned int handle,my_cb_user *usr);

        static void HIDDS3Rumble(unsigned int handle,my_cb_user *usr,int rumble);

/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 * HID Callbacks
 *--------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
        static int myAttachDetachCallback(HIDClient *p_client, HIDDevice *p_device, unsigned int attach);

        static void myHIDMouseReadCallback(unsigned int handle, int error, unsigned char *buf, unsigned int bytes_transfered, void *p_user);
        static void myHIDReadCallback(unsigned int handle, int error, unsigned char *buf, unsigned int bytes_transfered, void *p_user);

        static int AttachDetachCallback(HIDClient *p_client, HIDDevice *p_device, unsigned int attach);
        static void HIDReadCallback(unsigned int handle, unsigned char *buf, unsigned int bytes_transfered, my_cb_user * usr);
};

#endif /* _CONTROLLER_PATCHER_HID_H_ */
