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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>

#include "wiiu/fs.h"

#define FS_MOUNT_SOURCE_SIZE            0x300
#define FS_MAX_MOUNTPATH_SIZE           12

#define FS_CLIENT_SIZE                  0x1700
#define FS_CMD_BLOCK_SIZE               0xA80

#define FS_SOURCETYPE_EXTERNAL          0
#define FS_SOURCETYPE_HFIO              1

s32 ConfigReader::numberValidFiles = 0;
ConfigReader *ConfigReader::instance = NULL;

ConfigReader::ConfigReader(){
}

void ConfigReader::ReadAllConfigs(){
    std::vector<std::string> fileList = ScanFolder();
    if(fileList.size() > 0){
        if(HID_DEBUG){ printf("ConfigReader::ConfigReader(line %d): Found %d config files\n",__LINE__,fileList.size()); }
        processFileList(fileList);
    }
}


ConfigReader::~ConfigReader(){
    if(HID_DEBUG){ printf("ConfigReader::~ConfigReader(line %d): ~ConfigReader\n",__LINE__); }
    freeFSHandles();
}

void ConfigReader::freeFSHandles(){
    if(this->pClient != NULL){
        FSDelClient((FSClient *)this->pClient,-1);
        free(this->pClient);
        this->pClient = NULL;
    }
     if(this->pCmd != NULL){
        free(this->pCmd);
        this->pCmd = NULL;
    }
}

// Mounting the sdcard without any external lib to be portable (Currently broken)
s32 ConfigReader::InitSDCard(){
    if(HID_DEBUG){ printf("ConfigReader::InitSDCard(line %d): InitSDCard\n",__LINE__); }

    void *mountSrc = malloc(FS_MOUNT_SOURCE_SIZE);
    if(!mountSrc)
        return -3;

    char* mountPath = (char*) malloc(FS_MAX_MOUNTPATH_SIZE);
    if(!mountPath) {
        free(mountSrc);
        return -4;
    }

    memset(mountSrc, 0, FS_MOUNT_SOURCE_SIZE);
    memset(mountPath, 0, FS_MAX_MOUNTPATH_SIZE);

    freeFSHandles();

    this->pClient = malloc(sizeof(FSClient));
    this->pCmd = malloc(sizeof(FSCmdBlock));

    s32 status = 0;

    if (this->pClient && this->pCmd){
        FSInit();
        FSInitCmdBlock((FSCmdBlock*)pCmd);
        FSAddClientEx((FSClient*)pClient,0, -1);
        if ((status = FSGetMountSource((FSClient*)this->pClient,(FSCmdBlock*)this->pCmd, FS_MOUNT_SOURCE_SD, (FSMountSource *)mountSrc, 0)) == FS_STATUS_OK)
        {
            if(HID_DEBUG){ printf("ConfigReader::InitSDCard(line %d): \n",__LINE__); }
            if ((status = FSMount((FSClient*)this->pClient,(FSCmdBlock*)this->pCmd, (FSMountSource*)mountSrc, mountPath, FS_MAX_MOUNTPATH_SIZE, 0x0400)) == FS_STATUS_OK)
            {
                if(HID_DEBUG){ printf("ConfigReader::InitSDCard(line %d): \n",__LINE__); }
                free(mountSrc);free(mountPath);
                return 0;
            }else{
                printf("ConfigReader::InitSDCard(line %d): error: FSMount failed %d\n",__LINE__,status);
                free(mountSrc);free(mountPath);
                return status;
            }
        }else{
            printf("ConfigReader::InitSDCard(line %d): error: FSGetMountSource failed %d\n",__LINE__,status);
            free(mountSrc);free(mountPath);
            return status;
        }
    }
    free(mountSrc);free(mountPath);
    return -1;
}

std::vector<std::string> ConfigReader::ScanFolder(){
    std::string path = CONTROLLER_PATCHER_PATH;
    s32 dirhandle = 0;
    if(HID_DEBUG){ printf("ConfigReader::ScanFolder(line %d): Opening %s\n",__LINE__,path.c_str()); }
    std::vector<std::string> config_files;
    if (this->pClient && this->pCmd){
        s32 status = 0;
        if((status = FSOpenDir((FSClient*)this->pClient,(FSCmdBlock*)this->pCmd,path.c_str(),(FSDirectoryHandle *)&dirhandle,-1)) == FS_STATUS_OK){
            FSDirectoryEntry dir_entry;
            while (FSReadDir((FSClient*)this->pClient,(FSCmdBlock*)this->pCmd, dirhandle, &dir_entry, -1) == FS_STATUS_OK){
                std::string full_path = path + "/" +  dir_entry.name;
                if((dir_entry.info.flags&FS_STAT_DIRECTORY) != FS_STAT_DIRECTORY){
                    if(CPStringTools::EndsWith(std::string(dir_entry.name),".ini")){
                        config_files.push_back(full_path);
                        if(HID_DEBUG){ printf("ConfigReader::ScanFolder(line %d): %s \n",__LINE__,full_path.c_str()); }
                    }
                }
            }
            FSCloseDir((FSClient*)this->pClient,(FSCmdBlock*)this->pCmd,dirhandle,-1);
        }else{
            printf("ConfigReader::ScanFolder(line %d): Failed to open %s!\n",__LINE__,path.c_str());
        }
    }
    return config_files;
}

void ConfigReader::processFileList(std::vector<std::string> path){
    for(std::vector<std::string>::iterator it = path.begin(); it != path.end(); ++it) {
        printf("ConfigReader::processFileList(line %d): Reading %s\n",__LINE__,it->c_str());
        std::string result = loadFileToString(*it);

        ConfigParser parser(result);
        parser.parseIni();
    }
}

std::string ConfigReader::loadFileToString(std::string path){
    FSFileHandle handle = 0;
    s32 status = 0;
    std::string strBuffer;
    FSStat stats;
    if((status = FSGetStat((FSClient*)this->pClient,(FSCmdBlock*)this->pCmd,path.c_str(),&stats,-1)) == FS_STATUS_OK){
        uint8_t * file  = (uint8_t *) malloc((sizeof(uint8_t)*stats.size)+1);
		if(!file){
			printf("ConfigReader::loadFileToString(line %d): error: Failed to allocate space for reading the file\n",__LINE__);
			return "";
		}
        file[stats.size] = '\0';
        if((status = FSOpenFile((FSClient*)this->pClient,(FSCmdBlock*)this->pCmd,path.c_str(),"r",&handle,-1)) == FS_STATUS_OK){
            s32 total_read = 0;
			s32 ret2 = 0;
			while ((ret2 = FSReadFile((FSClient*)pClient,(FSCmdBlock*)pCmd, file+total_read, 1, stats.size-total_read, handle, 0, -1)) > 0){
				total_read += ret2;
            }

        }else{
            printf("ConfigReader::loadFileToString(line %d): error: (FSOpenFile) Couldn't open file (%s), error: %d",__LINE__,path.c_str(),status);
            free(file);
            file=NULL;
            return "";
        }

        FSCloseFile((FSClient*)this->pClient,(FSCmdBlock*)this->pCmd,handle,-1);

        strBuffer = std::string((char *)file);
        free(file);
        file = NULL;

        //! remove all windows crap signs
        strBuffer = CPStringTools::removeCharFromString(strBuffer,'\r');
        strBuffer = CPStringTools::removeCharFromString(strBuffer,' ');
        strBuffer = CPStringTools::removeCharFromString(strBuffer,'\t');

    }else{
        printf("ConfigReader::loadFileToString(line %d): error: (GetStat) Couldn't open file (%s), error: %d",__LINE__,path.c_str(),status);
    }

    return strBuffer;
}
