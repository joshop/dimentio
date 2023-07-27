#include "dimentio.h"
#include "helper.h"
void IWRAM_CODE ARM_CODE memset32(void *dst, u32 src, u32 wdn);
void memset16(void *dst, u16 src, u32 hwn);
void IWRAM_CODE ARM_CODE memcpy32(void *dst, const void *src, u32 wdcount);
void memcpy16(void *dst, const void *src, u32 hwcount);
void matmul(s32 *matrix, VEC3 *vectors);