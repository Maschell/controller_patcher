#ifndef _CPFS_UTILS_H_
#define _CPFS_UTILS_H_
#include <string>
#include <vector>
#include <gctypes.h>

class CPFSUtils{
    public:
        static s32 LoadFileToMem(const char *filepath, u8 **inbuffer, u32 *size);
};

#endif /* _CPFS_UTILS_H_ */
