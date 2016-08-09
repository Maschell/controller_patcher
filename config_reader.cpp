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

#include "config_reader.h"
#include "config_parser.h"
#include "config_values.h"
#include "string_tools.hpp"

int ConfigReader::numberValidFiles = 0;
ConfigReader *ConfigReader::instance = NULL;

ConfigReader::ConfigReader()
{
    InitOSFunctionPointers();
    InitFSFunctionPointers();
    int status = 0;
    if((status = InitSDCard()) == 0){
        log_printf("SD Card mounted for controller config!\n");
        std::vector<std::string> fileList;
        if((fileList = ScanFolder()).size() > 0){
            log_printf("Found %d config files\n",fileList.size());
            processFileList(fileList);
        }
	}else{
        log_printf("SD mounting failed! %d\n",status);
	}
}

void ConfigReader::processFileList(std::vector<std::string> path){
    for(std::vector<std::string>::iterator it = path.begin(); it != path.end(); ++it) {
        log_printf("Reading %s\n",it->c_str());
        std::string result = loadFileToString(*it);

        ConfigParser parser(result);
        parser.parseIni();
    }
}

std::string ConfigReader::loadFileToString(std::string path){
    int handle = 0;
    int status = 0;
    std::string strBuffer;
    FSStat stats;
    if((status = FSGetStat(this->pClient,this->pCmd,path.c_str(),&stats,-1)) == FS_STATUS_OK){
        char * file  = (char *) malloc((sizeof(char)*stats.size)+1);
		if(!file){
			log_print("Failed to allocate space for reading the file\n");
			return false;
		}
        file[stats.size] = '\0';
        if((status = FSOpenFile(this->pClient,this->pCmd,path.c_str(),"r",&handle,-1)) == FS_STATUS_OK){
            int total_read = 0;
			int ret2 = 0;
			while ((ret2 = FSReadFile(pClient,  pCmd, file+total_read, 1, stats.size-total_read, handle, 0, FS_RET_ALL_ERROR)) > 0){
				total_read += ret2;
            }

        }else{
            log_printf("(FSOpenFile) Couldn't open file (%s), error: %d",path.c_str(),status);
            free(file);
            file=NULL;
            return false;
        }

        FSCloseFile(this->pClient,this->pCmd,handle,-1);

        strBuffer = std::string(file);
        free(file);
        file = NULL;

        //! remove all windows crap signs
        size_t position;
        while(1)
        {
            position = strBuffer.find('\r');
            if(position == std::string::npos)
                break;
            strBuffer.erase(position, 1);
        }
        while(1)
        {
            position = strBuffer.find(' ');
            if(position == std::string::npos)
                break;
            strBuffer.erase(position, 1);
        }
        while(1)
        {
            position = strBuffer.find('\t');
            if(position == std::string::npos)
                break;
            strBuffer.erase(position, 1);
        }
    }else{
        log_printf("(GetStat) Couldn't open file (%s), error: %d",path.c_str(),status);
    }

    return strBuffer;
}


ConfigReader::~ConfigReader()
{
    if(HID_DEBUG) log_printf("~ConfigReader\n");
    freeFSHandles();

    if(HID_DEBUG) log_printf("~destroy the ConfigValues\n");
    ConfigValues::destroyInstance();
}
std::vector<std::string> ConfigReader::ScanFolder()
{
    std::string path = CONTROLLER_PATCHER_PATH;
    int dirhandle = 0;
    if(HID_DEBUG) log_printf("Opening %s\n",path.c_str());
    std::vector<std::string> config_files;
    if (this->pClient && this->pCmd){
        int status = 0;
        if((status = FSOpenDir(this->pClient,this->pCmd,path.c_str(),&dirhandle,-1)) == FS_STATUS_OK){
            FSDirEntry dir_entry;
            while (FSReadDir(this->pClient,  this->pCmd, dirhandle, &dir_entry, FS_RET_ALL_ERROR) == FS_STATUS_OK){
                std::string full_path = path + "/" +  dir_entry.name;
                if((dir_entry.stat.flag&FS_STAT_FLAG_IS_DIRECTORY) != FS_STAT_FLAG_IS_DIRECTORY){
                    if(EndsWith(std::string(dir_entry.name),".ini")){
                        config_files.push_back(full_path);
                        if(HID_DEBUG) log_printf("%s \n",full_path.c_str());
                    }
                }
            }
            FSCloseDir(this->pClient,this->pCmd,dirhandle,-1);
        }else{
            log_printf("Failed to open %s!\n",path.c_str());
        }
    }
    return config_files;
}

void ConfigReader::freeFSHandles(){
    if(this->pClient != NULL){
        FSDelClient(this->pClient);
        free(this->pClient);
        this->pClient = NULL;
    }
     if(this->pCmd != NULL){
        free(this->pCmd);
        this->pCmd = NULL;
    }
}

int ConfigReader::InitSDCard(){
    if(HID_DEBUG) log_printf("InitSDCard\n");

    char mountSrc[FS_MOUNT_SOURCE_SIZE];
    char mountPath[FS_MAX_MOUNTPATH_SIZE];

    freeFSHandles();

    this->pClient = malloc(FS_CLIENT_SIZE);
    this->pCmd = malloc(FS_CMD_BLOCK_SIZE);

    int status = 0;

    if (this->pClient && this->pCmd)
    {
        // Do an FSInit first
        FSInit();
        // Add client to FS.
        FSAddClientEx(this->pClient, FS_RET_NO_ERROR,-1);

        // Init command block.
        FSInitCmdBlock(this->pCmd);

        // Mount sdcard
        if ((status = FSGetMountSource(this->pClient, this->pCmd, FS_SOURCETYPE_EXTERNAL, &mountSrc, FS_RET_NO_ERROR)) == FS_STATUS_OK)
        {
            if ((status = FSMount(this->pClient, this->pCmd, &mountSrc, mountPath, FS_MAX_MOUNTPATH_SIZE, FS_RET_UNSUPPORTED_CMD)) == FS_STATUS_OK)
            {
                return 0;
            }else{
                log_printf("FSMount failed %d\n",status);
                return status;
            }
        }else{
            log_printf("FSGetMountSource failed %d\n",status);
            return status;
        }
    }
    return -1;
}
