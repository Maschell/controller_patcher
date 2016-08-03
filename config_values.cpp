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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config_values.h"
#include "utils/logger.h"
#include "cp_retain_vars.h"

ConfigValues *ConfigValues::instance = NULL;

ConfigValues::ConfigValues()
{
    InitValues();
}

ConfigValues::~ConfigValues()
{

}

const u8 * ConfigValues::getValuesForPreset(std::map<std::string,const u8*> values,std::string possibleValue){
    std::map<std::string,const u8*>::iterator it;
    it = values.find(possibleValue);
    if (it != values.end()){
        return it->second;
    }
    return NULL;
}

bool ConfigValues::setIfValueIsAControllerPresetEx(std::string value,int slot,int keyslot){
    if(setIfValueIsPreset(presetGCValues,value,slot,keyslot)) return true;
    if(setIfValueIsPreset(presetDS3Values,value,slot,keyslot)) return true;
    if(setIfValueIsPreset(presetDS4Values,value,slot,keyslot)) return true;
    return false;
}

//We need this function here so we can use preset sticks.
bool ConfigValues::setIfValueIsPreset(std::map<std::string,const u8*> values,std::string possibleValue,int slot,int keyslot){
    const u8 * values_ = NULL;
    if( keyslot == CONTRPS_VPAD_BUTTON_L_STICK_X ||
        keyslot == CONTRPS_VPAD_BUTTON_L_STICK_Y ||
        keyslot == CONTRPS_VPAD_BUTTON_R_STICK_X ||
        keyslot == CONTRPS_VPAD_BUTTON_R_STICK_Y){
        if(HID_DEBUG) log_printf("This may be a predefined stick %s\n",possibleValue.c_str());
        if((values_ = ConfigValues::getValuesStickPreset(possibleValue)) != NULL){
            if(HID_DEBUG) log_printf("Found predefined stick!\n");
            config_controller[slot][keyslot][0] =                           values_[STICK_CONF_BYTE];       //CONTRPS_VPAD_BUTTON_L_STICK_X
            config_controller[slot][keyslot][1] =                           values_[STICK_CONF_DEFAULT];
            config_controller[slot][keyslot+DEF_STICK_OFFSET_INVERT][0] =   CONTROLLER_PATCHER_VALUE_SET;   //CONTRPS_VPAD_BUTTON_L_STICK_X_INVERT
            config_controller[slot][keyslot+DEF_STICK_OFFSET_INVERT][1] =   values_[STICK_CONF_INVERT];
            config_controller[slot][keyslot+DEF_STICK_OFFSET_DEADZONE][0] = CONTROLLER_PATCHER_VALUE_SET;   //CONTRPS_VPAD_BUTTON_L_STICK_X_DEADZONE
            config_controller[slot][keyslot+DEF_STICK_OFFSET_DEADZONE][1] = values_[STICK_CONF_DEADZONE];
            config_controller[slot][keyslot+DEF_STICK_OFFSET_MINMAX][0] =   values_[STICK_CONF_MIN];        //CONTRPS_VPAD_BUTTON_L_STICK_X_MINMAX
            config_controller[slot][keyslot+DEF_STICK_OFFSET_MINMAX][1] =   values_[STICK_CONF_MAX];

            return true;
        }
    }

    if((values_ = getValuesForPreset(values,possibleValue)) != NULL){
        config_controller[slot][keyslot][0] = values_[0];
        config_controller[slot][keyslot][1] = values_[1];
        return true;
    }
    return false;
}


int ConfigValues::getValueFromMap(std::map<std::string,int> values,std::string nameOfString){
    std::map<std::string,int>::iterator it;
    it = values.find(nameOfString);
    if (it != values.end()){
       return it->second;
    }

    //Value not found
    return -1;
}

int ConfigValues::getPresetValueEx(std::string possibleString){
    int rightValue = -1;
    if((rightValue = getValueFromMap(gGamePadValuesToCONTRPSString,possibleString))!= -1){
        if(HID_DEBUG) log_printf("Used pre-defined VPAD_VALUE! \"%s\" is %d\n",possibleString.c_str(),rightValue);
    }else if((rightValue = getValueFromMap(presetValues,possibleString))!= -1){
        if(HID_DEBUG) log_printf("Used pre-defined value! \"%s\" is %d\n",possibleString.c_str(),rightValue);
    }
    return rightValue;
}
