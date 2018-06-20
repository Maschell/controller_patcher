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
#include "./ConfigReader.hpp"
#include <fs/FSUtils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#include <dirent.h>

#include <utils/logger.h>

int32_t ConfigReader::numberValidFiles = 0;
ConfigReader *ConfigReader::instance = NULL;

ConfigReader::ConfigReader() {
}

BOOL ConfigReader::ReadConfigs(std::string path) {
    std::vector<std::string> fileList = ScanFolder(path);
    if(fileList.size() == 1 && fileList[0].compare("ERROR") == 0) {
        return false;
    }

    if(fileList.size() > 0) {
        if(HID_DEBUG) {
            DEBUG_FUNCTION_LINE("Found %d config files\n",fileList.size());
        }
        processFileList(fileList);
    }
    return true;
}


ConfigReader::~ConfigReader() {
    if(HID_DEBUG) {
        DEBUG_FUNCTION_LINE("~ConfigReader\n");
    }
}

std::vector<std::string> ConfigReader::ScanFolder(std::string path) {
    std::vector<std::string> config_files;

    struct dirent *dirent = NULL;
    DIR *dirHandle = opendir(path.c_str());
    if (dirHandle == NULL) {
        DEBUG_FUNCTION_LINE("Failed to open dir %s\n",path.c_str());
        config_files.push_back("ERROR"); //TODO: Find a proper solution
        return config_files;
    }
    while ((dirent = readdir(dirHandle)) != 0) {
        BOOL isDir = dirent->d_type & DT_DIR;
        const char *filename = dirent->d_name;

        if(strcmp(filename,".") == 0 || strcmp(filename,"..") == 0) {
            continue;
        }

        std::string newPath = path + "/" + std::string(filename);

        if(!isDir && StringTools::EndsWith(std::string(filename),".ini")) {
            config_files.push_back(newPath);
            if(HID_DEBUG) {
                DEBUG_FUNCTION_LINE("Found ini: %s \n",newPath.c_str());
            }
        }
    }

    return config_files;
}

void ConfigReader::processFileList(std::vector<std::string> path) {
    for(std::vector<std::string>::iterator it = path.begin(); it != path.end(); ++it) {
        DEBUG_FUNCTION_LINE("Reading %s\n",it->c_str());
        std::string result = loadFileToString(*it);

        ConfigParser parser(result);
        parser.parseIni();
    }
}

std::string ConfigReader::loadFileToString(std::string path) {
    std::string strBuffer = "";
    uint8_t * buffer = NULL;
    if(FSUtils::LoadFileToMem(path.c_str(),&buffer,NULL) > 0) {
        strBuffer = std::string((char *)buffer);
        strBuffer = StringTools::removeCharFromString(strBuffer,'\r');
        strBuffer = StringTools::removeCharFromString(strBuffer,' ');
        strBuffer = StringTools::removeCharFromString(strBuffer,'\t');
    }
    return strBuffer;
}
