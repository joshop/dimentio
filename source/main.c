#include "asmfunc.h"
#include "bmp.h"
#include "dimentio.h"
#include "graphics.h"
#include "helper.h"
#include "triglut.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

VEC3 cube_vecs[] = {
    {1 << 16, 1 << 16, 1 << 16},   {1 << 16, 1 << 16, -1 << 16},
    {1 << 16, -1 << 16, 1 << 16},  {1 << 16, -1 << 16, -1 << 16},
    {-1 << 16, 1 << 16, 1 << 16},  {-1 << 16, 1 << 16, -1 << 16},
    {-1 << 16, -1 << 16, 1 << 16}, {-1 << 16, -1 << 16, -1 << 16}};
POLY cube_polys[] = {
    {0, 1, 3, CLR_RED},  {0, 2, 3, CLR_RED},    {4, 5, 7, CLR_BLUE},
    {4, 6, 7, CLR_BLUE}, {0, 1, 5, CLR_YELLOW}, {0, 4, 5, CLR_YELLOW},
    {2, 3, 7, CLR_LIME}, {2, 6, 7, CLR_LIME},   {0, 2, 6, CLR_MAG},
    {0, 4, 6, CLR_MAG},  {1, 3, 7, CLR_WHITE},  {1, 5, 7, CLR_WHITE}};
MODEL cube_model = {8, 12, cube_vecs, cube_polys};

int main() {
  init_bmp();
  int tPitch = 0;
  int tYaw = 0;
  int tRoll = 0;
  s32 rotMatrix[] = {1 << 16, 0, 0, 0, 1 << 16, 0, 0, 0, 1 << 16};
  VEC3 camPos = {0, 0, -0x80000};
  VEC3 origin = {0, 0, 0};
  while (1) {
    // clear out the back buffer
    memset32(back_buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT / 2);

    clear_lists();

    push_model(cube_model, origin);
    rot_matrix(rotMatrix, (tPitch >> 2) & 0xff, (tYaw >> 2) & 0xff,
               (tRoll >> 2) & 0xff);
    matmul(rotMatrix, veclist_end);
    translate(camPos);
    project();
    showtime();
    tPitch++;
    tYaw += 3;
    tRoll += 2;

    // flip the buffers
    flip_buffers();
    // wait for the next vblank
    wait_blank();
  }

  return 0;
}
