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

#ifndef _CONTROLLER_PATCHER_INCLUDES_H_
#define _CONTROLLER_PATCHER_INCLUDES_H_

#define HID_DEBUG 0

#include <dynamic_libs/os_types.h>

#include "./ConfigReader.hpp"
#include "./ControllerPatcher.hpp"

#include <system/CThread.h>
#include "./utils/CPRetainVars.hpp"
#include "./utils/PadConst.hpp"
#include <utils/StringTools.h>

#include "./patcher/ControllerPatcherHID.hpp"
#include "./patcher/ControllerPatcherUtils.hpp"

#include "./config/ConfigValues.hpp"
#include "./config/ConfigParser.hpp"

#include "./network/CPTCPServer.hpp"
#include "./network/UDPServer.hpp"
#include "./network/UDPClient.hpp"

#endif /* _CONTROLLER_PATCHER_INCLUDES_H_ */
