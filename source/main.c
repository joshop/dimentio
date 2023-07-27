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
COLOR checker_flex[512];
POLY cube_polys[] = {
    {0, 1, 3, CLR_RED},  {0, 2, 3, CLR_RED},      {4, 5, 7, CLR_BLUE},
    {4, 6, 7, CLR_BLUE}, {0, 1, 5, CLR_YELLOW},   {0, 4, 5, CLR_YELLOW},
    {2, 3, 7, CLR_LIME}, {2, 6, 7, CLR_LIME},     {0, 2, 6, CLR_MAG},
    {0, 4, 6, CLR_MAG},  {1, 3, 7, checker_flex}, {1, 5, 7, checker_flex}};
MODEL cube_model = {8, 12, cube_vecs, cube_polys};
#define NUMAVGDIFFS 20
int main() {
  init_bmp();
  int tPitch = 0;
  int tYaw = 0;
  int tRoll = 0;
  s32 rotMatrix[] = {1 << 16, 0, 0, 0, 1 << 16, 0, 0, 0, 1 << 16};
  VEC3 camPos = {0, 0, -0x80000};
  VEC3 origin = {0, 0, 0};
  REG_TM0CNT = 0x0082;
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 32; j++) {
      checker_flex[i * 32 + j] =
          ((i >> 1) + (j >> 1)) & 1 ? CLR_WHITE : CLR_BLACK;
    }
  }
  int old_tval, time_diff;
  old_tval = REG_TM0D;
  int avg_diffs[NUMAVGDIFFS];
  int avgi = 0;
  while (1) {
    if (REG_TM0D > old_tval) {
      time_diff = REG_TM0D - old_tval;
    }
    old_tval = REG_TM0D;
    // clear out the back buffer
    memset32(back_buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT / 2);
    // each time_diff is 15.26 microseconds
    avg_diffs[avgi++] = time_diff;
    if (avgi == NUMAVGDIFFS)
      avgi = 0;
    int total = 0;
    for (int i = 0; i < NUMAVGDIFFS; i++) {
      total += avg_diffs[i];
    }
    show_number(65536 * NUMAVGDIFFS / total, 60, 8);
    show_digit(10, 65, 8);
    show_digit(11, 70, 8);
    show_digit(5, 75, 8);

    clear_lists();

    push_model(cube_model, origin);
    rot_matrix(rotMatrix, (tPitch >> 2) & 0xff, (tYaw >> 2) & 0xff,
               (tRoll >> 2) & 0xff);
    matmul(rotMatrix, veclist_end);
    translate(camPos);
    project();
    showtime();
    tPitch++;
    tYaw += 7;
    tRoll += 20;
    // flip the buffers
    flip_buffers();
    // wait for the next vblank
    // wait_blank();
  }

  return 0;
}
