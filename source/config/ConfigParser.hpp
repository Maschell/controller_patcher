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
#ifndef _ConfigParser_H_
#define _ConfigParser_H_

#include <string>
#include <vector>
#include <map>

#include <stdio.h>

#include "../ControllerPatcherIncludes.hpp"

#include <utils/logger.h>

enum PARSE_TYPE{
    PARSE_CONTROLLER,
    PARSE_GAMEPAD,
    PARSE_MOUSE,
    PARSE_KEYBOARD
};

class ConfigParser{
    friend class ConfigReader;
    friend class ControllerPatcher;
private:
    //!Constructor
    ConfigParser(std::string configData);
    //!Destructor
    ~ConfigParser();

    PARSE_TYPE getType();
    void setType(PARSE_TYPE newType);

    uint16_t getSlot();
    void setSlot(uint16_t newSlot);

    BOOL parseIni();

    BOOL Init();

    BOOL parseConfigString(std::string content);

    int32_t getSlotController(std::string identify);

    int32_t checkExistingController(int32_t vid, int32_t pid);

    int32_t getValueFromKeyValue(std::string value_pair,std::string expectedKey,std::string delimiter);

    BOOL resetConfig();

    void parseSingleLine(std::string line);
    uint16_t slot_b;
    PARSE_TYPE type_b;

    uint16_t vid;
    uint16_t pid;

    std::string content;
    std::vector<std::string> contentLines;
};
#endif
