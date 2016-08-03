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
#ifndef _ConfigParser_H_
#define _ConfigParser_H_

#include <string>
#include <vector>
#include <map>

#include <stdio.h>
#include <gctypes.h>

#include "controller_patcher.h"
#include "pad_const.h"

#include "utils/logger.h"
#include "cp_retain_vars.h"

enum PARSE_TYPE{
    PARSE_CONTROLLER,
    PARSE_GAMEPAD,
    PARSE_MOUSE,
    PARSE_KEYBOARD
};

class ConfigParser
{
public:
    //!Constructor
    ConfigParser(std::string configData);
    //!Destructor
    ~ConfigParser();

    PARSE_TYPE getType();
    void setType(PARSE_TYPE newType);

    u16 getSlot();
    void setSlot(u16 newSlot);

    bool parseIni();

private:
    bool Init();

    bool parseConfigString(std::string content);

    int getSlotController(std::string identify);

    int checkExistingController(int vid, int pid);

    int getValueFromKeyValue(std::string value_pair,std::string expectedKey,std::string delimiter);

    bool resetConfig();

    void parseSingleLine(std::string line);
    u16 slot_b;
    PARSE_TYPE type_b;
    std::string content;
    std::vector<std::string> contentLines;
};
#endif
