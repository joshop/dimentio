#include "dimentio.h"
#include "helper.h"
void bmp16_line(int y1, int x1, int y2, int x2, u32 clr, void *dstBase,
                u32 dstPitch);
void fill_tri2(int y1, int x1, int y2, int x2, int y3, int x3, COLOR *color,
               int rootO);
void show_string(const char *str, int x, int y);
void plot_pixel(int y, int x, COLOR clr);
COLOR query_pixel(int y, int x);