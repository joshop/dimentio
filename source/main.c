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
COLOR checker_flex[FLEX_WIDTH * FLEX_HEIGHT];
POLY cube_polys[] = {
    {0, 1, 3, checker_flex}, {0, 3, 2, checker_flex}, {4, 7, 5, checker_flex},
    {4, 6, 7, checker_flex}, {0, 5, 1, checker_flex}, {0, 4, 5, checker_flex},
    {2, 3, 7, checker_flex}, {2, 7, 6, checker_flex}, {0, 2, 6, checker_flex},
    {0, 6, 4, checker_flex}, {1, 7, 3, checker_flex}, {1, 5, 7, checker_flex}};
MODEL cube_model = {8, 12, cube_vecs, cube_polys};
u16 key_state;
#define NUMAVGDIFFS 20
int main() {
  init_bmp();
  int tPitch = 0;
  int tYaw = 64;
  int tRoll = 0;
  int ctrlMode = 0;
  int oldSelect = 0;
  s32 rotMatrix[] = {1 << 16, 0, 0, 0, 1 << 16, 0, 0, 0, 1 << 16};
  VEC3 camPos = {-0x80000, 0, 0};
  VEC3 origin = {0, 0, 0};
  VEC3 origin2 = {0, 0, 0x30000};
  VEC3 origin3 = {0, 0x60000, 0};
  VEC3 origin4 = {0x30000, 0, 0};
  REG_TM0CNT = 0x0082;
  for (int i = 0; i < FLEX_HEIGHT; i++) {
    for (int j = 0; j < FLEX_WIDTH; j++) {
      checker_flex[i * FLEX_WIDTH + j] =
          ((i >> 1) + (j >> 1)) & 1 ? CLR_RED : CLR_BLUE;
    }
  }
  int old_tval, time_diff;
  time_diff = 0;
  old_tval = REG_TM0D;
  int avg_diffs[NUMAVGDIFFS];
  int avgi = 0;
  s32 movespeed = 0x2000;
  while (1) {
    if (REG_TM0D > old_tval) {
      time_diff = REG_TM0D - old_tval;
    }
    old_tval = REG_TM0D;
    key_state = ~REG_KEYINPUT;
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
    if (key_state & KEY_LEFT) {
      tYaw--;
    } else if (key_state & KEY_RIGHT) {
      tYaw++;
    }

    if (key_state & KEY_UP) {
      camPos.x += fixed_mul(SIN_LUT[tYaw & 0xff], movespeed);
      camPos.z += fixed_mul(COS_LUT[tYaw & 0xff], movespeed);
    } else if (key_state & KEY_DOWN) {
      camPos.x -= fixed_mul(SIN_LUT[tYaw & 0xff], movespeed);
      camPos.z -= fixed_mul(COS_LUT[tYaw & 0xff], movespeed);
    } else if (key_state & KEY_A) {
      camPos.y += movespeed;
    } else if (key_state & KEY_B) {
      camPos.y -= movespeed;
    }
    if ((key_state & KEY_SELECT) > oldSelect) {
      do_bfc ^= 1;
    }
    oldSelect = key_state & KEY_SELECT;
    clear_lists();

    push_model(cube_model, origin);
    push_model(cube_model, origin2);
    push_model(cube_model, origin3);
    push_model(cube_model, origin4);
    translate(camPos);
    rot_matrix(rotMatrix, (-tRoll) & 0xff, (-tYaw) & 0xff, (-tPitch) & 0xff);
    matmul(rotMatrix, veclist_end);
    cull_polys();
    project();
    showtime();

    show_number(65536 * NUMAVGDIFFS / total, 60, 8);
    show_digit(10, 65, 8);
    show_digit(11, 70, 8);
    show_digit(5, 75, 8);

    show_number(num_polys, 60, 14);

    show_number(num_culled_polys, 60, 20);

    // flip the buffers
    flip_buffers();
    // wait for the next vblank
    // wait_blank();
  }

  return 0;
}
