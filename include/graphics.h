#include "helper.h"
void bmp16_line(int y1, int x1, int y2, int x2, u32 clr, void *dstBase,
                u32 dstPitch);
void fill_tri(int y1, int x1, int y2, int x2, int y3, int x3, int color);