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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ConfigReader.hpp"
#include "config/ConfigParser.hpp"
#include "config/ConfigValues.hpp"
#include "utils/CPStringTools.hpp"

int ConfigReader::numberValidFiles = 0;
ConfigReader *ConfigReader::instance = NULL;

ConfigReader::ConfigReader(){
    InitOSFunctionPointers();
    InitFSFunctionPointers();
}

void ConfigReader::ReadAllConfigs(){
    std::vector<std::string> fileList = ScanFolder();
    if(fileList.size() > 0){
        if(HID_DEBUG) log_printf("ConfigReader::ConfigReader(): Found %d config files\n",fileList.size());
        processFileList(fileList);
    }
}


ConfigReader::~ConfigReader(){
    if(HID_DEBUG) log_printf("~ConfigReader\n");
    freeFSHandles();
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

// Mounting the sdcard without any external lib to be portable
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
                log_printf("ConfigReader::InitSDCard() error: FSMount failed %d\n",status);
                return status;
            }
        }else{
            log_printf("ConfigReader::InitSDCard() error: FSGetMountSource failed %d\n",status);
            return status;
        }
    }
    return -1;
}

std::vector<std::string> ConfigReader::ScanFolder(){
    std::string path = CONTROLLER_PATCHER_PATH;
    int dirhandle = 0;
    if(HID_DEBUG) log_printf("ConfigReader::ScanFolder(): Opening %s\n",path.c_str());
    std::vector<std::string> config_files;
    if (this->pClient && this->pCmd){
        int status = 0;
        if((status = FSOpenDir(this->pClient,this->pCmd,path.c_str(),&dirhandle,-1)) == FS_STATUS_OK){
            FSDirEntry dir_entry;
            while (FSReadDir(this->pClient,  this->pCmd, dirhandle, &dir_entry, FS_RET_ALL_ERROR) == FS_STATUS_OK){
                std::string full_path = path + "/" +  dir_entry.name;
                if((dir_entry.stat.flag&FS_STAT_FLAG_IS_DIRECTORY) != FS_STAT_FLAG_IS_DIRECTORY){
                    if(CPStringTools::EndsWith(std::string(dir_entry.name),".ini")){
                        config_files.push_back(full_path);
                        if(HID_DEBUG) log_printf("%s \n",full_path.c_str());
                    }
                }
            }
            FSCloseDir(this->pClient,this->pCmd,dirhandle,-1);
        }else{
            log_printf("ConfigReader::ScanFolder(): Failed to open %s!\n",path.c_str());
        }
    }
    return config_files;
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
			log_print("ConfigReader::loadFileToString error: Failed to allocate space for reading the file\n");
			return "";
		}
        file[stats.size] = '\0';
        if((status = FSOpenFile(this->pClient,this->pCmd,path.c_str(),"r",&handle,-1)) == FS_STATUS_OK){
            int total_read = 0;
			int ret2 = 0;
			while ((ret2 = FSReadFile(pClient,  pCmd, file+total_read, 1, stats.size-total_read, handle, 0, FS_RET_ALL_ERROR)) > 0){
				total_read += ret2;
            }

        }else{
            log_printf("ConfigReader::loadFileToString error: (FSOpenFile) Couldn't open file (%s), error: %d",path.c_str(),status);
            free(file);
            file=NULL;
            return "";
        }

        FSCloseFile(this->pClient,this->pCmd,handle,-1);

        strBuffer = std::string(file);
        free(file);
        file = NULL;

        //! remove all windows crap signs
        strBuffer = CPStringTools::removeCharFromString(strBuffer,'\r');
        strBuffer = CPStringTools::removeCharFromString(strBuffer,' ');
        strBuffer = CPStringTools::removeCharFromString(strBuffer,'\t');

    }else{
        log_printf("ConfigReader::loadFileToString error: (GetStat) Couldn't open file (%s), error: %d",path.c_str(),status);
    }

    return strBuffer;
}
