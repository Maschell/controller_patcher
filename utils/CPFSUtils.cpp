#include "CPFSUtils.hpp"
#include <string>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>

//Copy pasted from dimoks sources. Thanks!!!
s32 CPFSUtils::LoadFileToMem(const char *filepath, u8 **inbuffer, u32 *size){
    //! always initialze input
	*inbuffer = NULL;
    if(size){
        *size = 0;
    }

    s32 iFd = open(filepath, O_RDONLY);
	if (iFd < 0)
		return -1;

	u32 filesize = lseek(iFd, 0, SEEK_END);
    lseek(iFd, 0, SEEK_SET);

	u8 *buffer = (u8 *) malloc(filesize);
	if (buffer == NULL)
	{
        close(iFd);
		return -2;
	}

    u32 blocksize = 0x4000;
    u32 done = 0;
    s32 readBytes = 0;

	while(done < filesize)
    {
        if(done + blocksize > filesize) {
            blocksize = filesize - done;
        }
        readBytes = read(iFd, buffer + done, blocksize);
        if(readBytes <= 0)
            break;
        done += readBytes;
    }

    close(iFd);

	if (done != filesize)
	{
		free(buffer);
		buffer = NULL;
		return -3;
	}

	*inbuffer = buffer;

    //! size is optional input
    if(size){
        *size = filesize;
    }

	return filesize;
}

//CPFSUtils
