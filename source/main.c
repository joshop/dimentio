#include "asmfunc.h"
#include "bestovius.h"
#include "bmp.h"
#include "bmp_files.h"
#include "collision.h"
#include "dimentio.h"
#include "graphics.h"
#include "helper.h"
#include "triglut.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

const VEC3 cube_vecs[] = {
    {1 << 16, 1 << 16, 1 << 16},   {1 << 16, 1 << 16, -1 << 16},
    {1 << 16, -1 << 16, 1 << 16},  {1 << 16, -1 << 16, -1 << 16},
    {-1 << 16, 1 << 16, 1 << 16},  {-1 << 16, 1 << 16, -1 << 16},
    {-1 << 16, -1 << 16, 1 << 16}, {-1 << 16, -1 << 16, -1 << 16}};
COLOR checker_flex[FLEX_WIDTH * FLEX_HEIGHT];

const POLY cube_polys[] = {
    {0, 1, 3, checker_flex}, {0, 3, 2, checker_flex}, {4, 7, 5, checker_flex},
    {4, 6, 7, checker_flex}, {0, 5, 1, checker_flex}, {0, 4, 5, checker_flex},
    {2, 3, 7, checker_flex}, {2, 7, 6, checker_flex}, {0, 2, 6, checker_flex},
    {0, 6, 4, checker_flex}, {1, 7, 3, checker_flex}, {1, 5, 7, checker_flex}};
const MODEL cube_model = {8, 12, cube_vecs, cube_polys};

const VEC3 icosa_vecs[] = {
    {0x0, -0x8696, 0xd9c4},  {0xd9c4, 0x0, 0x8696},  {0xd9c4, 0x0, -0x8696},
    {-0xd9c4, 0x0, -0x8696}, {-0xd9c4, 0x0, 0x8696}, {-0x8696, 0xd9c4, 0x0},
    {0x8696, 0xd9c4, 0x0},   {0x8696, -0xd9c4, 0x0}, {-0x8696, -0xd9c4, 0x0},
    {0x0, -0x8696, -0xd9c4}, {0x0, 0x8696, -0xd9c4}, {0x0, 0x8696, 0xd9c4}};
#define icosa_flex (COLOR *)&bmp_papermario
const POLY icosa_polys[] = {
    {1, 6, 2, icosa_flex},  {1, 2, 7, icosa_flex},  {3, 5, 4, icosa_flex},
    {4, 8, 3, icosa_flex},  {6, 11, 5, icosa_flex}, {5, 10, 6, icosa_flex},
    {9, 2, 10, icosa_flex}, {10, 3, 9, icosa_flex}, {7, 9, 8, icosa_flex},
    {8, 0, 7, icosa_flex},  {11, 1, 0, icosa_flex}, {0, 4, 11, icosa_flex},
    {6, 10, 2, icosa_flex}, {1, 11, 6, icosa_flex}, {3, 10, 5, icosa_flex},
    {5, 11, 4, icosa_flex}, {2, 9, 7, icosa_flex},  {7, 0, 1, icosa_flex},
    {3, 8, 9, icosa_flex},  {4, 0, 8, icosa_flex}};
const MODEL icosa_model = {12, 20, icosa_vecs, icosa_polys};

const VEC3 hexagon_vecs[] = {{0x0, 0x0, 0x0},
                             {0x10000 * 4, 0x0, 0x0},
                             {0x8000 * 4, 0x0, 0xddb3 * 4},
                             {-0x8000 * 4, 0x0, 0xddb3 * 4},
                             {-0x10000 * 4, 0x0, 0x0},
                             {-0x8000 * 4, 0x0, -0xddb3 * 4},
                             {0x8000 * 4, 0x0, -0xddb3 * 4}};
const POLY hexagon_polys[] = {
    {0, 2, 1, (COLOR *)&bmp_grass, 17}, {0, 3, 2, (COLOR *)&bmp_grass, 17},
    {0, 4, 3, (COLOR *)&bmp_grass, 17}, {0, 5, 4, (COLOR *)&bmp_grass, 17},
    {0, 6, 5, (COLOR *)&bmp_grass, 17}, {0, 1, 6, (COLOR *)&bmp_grass, 17}};
const MODEL hexagon_model = {7, 6, hexagon_vecs, hexagon_polys};
u16 key_state;
void slowmatmul(s32 *matrix, VEC3 *vectors) {
  VEC3 *vec;
  VEC3 newvec;
  for (vec = vectors - 1; (s32)vec & VECLIST_TEST; vec--) {
    newvec.x = fixed_mul(vec->x, matrix[0]) + fixed_mul(vec->y, matrix[1]) +
               fixed_mul(vec->z, matrix[2]);
    newvec.y = fixed_mul(vec->x, matrix[3]) + fixed_mul(vec->y, matrix[4]) +
               fixed_mul(vec->z, matrix[5]);
    newvec.z = fixed_mul(vec->x, matrix[6]) + fixed_mul(vec->y, matrix[7]) +
               fixed_mul(vec->z, matrix[8]);
    *vec = newvec;
  }
}
#define NUMAVGDIFFS 20
int main() {
  init_bmp();
  int tPitch = 0;
  int tYaw = 0;
  int tRoll = 0;
  int oldSelect = 0;
  int oldStart = 0;
  int useBG = 1;
  int useRealSprites = 1;
  int camPose = 0;
  do_bfc = 1;
  do_wireframe = 0;
  s32 rotMatrix[] = {1 << 16, 0, 0, 0, 1 << 16, 0, 0, 0, 1 << 16};
  s32 rotMatrix2[9];
  VEC3 camPos = {0, 0x10000, -0x90000};
  VEC3 origin = {0x000, 0, 1};
  VEC3 origin2 = {0, 0, 0x30000};
  VEC3 origin3 = {0, 0x30000, 0};
  REG_TM0CNT = 0x0082;
  for (int i = 0; i < FLEX_HEIGHT; i++) {
    for (int j = 0; j < FLEX_WIDTH; j++) {
      checker_flex[i * FLEX_WIDTH + j] = ((i >> 1)) & 1 ? CLR_RED : CLR_BLUE;
    }
  }
  int old_tval, time_diff;
  time_diff = 0;
  old_tval = REG_TM0D;
  int avg_diffs[NUMAVGDIFFS];
  int avgi = 0;
  s32 movespeed = 0x2000;
  char buffer[128];
  SPRITE marioSprite = {origin, 0xa, 16, 32, 0, 0x60000};
  //  volatile SPRITE marioSprite;
  //  temporary palette solution, not good design
  COLOR marioPal[] = {0x0,    0x0,    0xa,    0x44c4, 0x30b6, 0xd71,  0x391f,
                      0x4e08, 0x35df, 0x1e9b, 0x6770, 0x635f, 0x3b7f, 0x7fff};
  memcpy16((COLOR *)0x05000200, &marioPal, 14);
  // copy_bitmap((const u8 *)&bmps_mario, 0, 8);
  while (1) {
    if (REG_TM0D > old_tval) {
      time_diff = REG_TM0D - old_tval;
    }
    old_tval = REG_TM0D;
    key_state = ~REG_KEYINPUT;
    // clear out the back buffer
    // memset32(back_buffer, (u32)&bmp_background,
    //         SCREEN_WIDTH * SCREEN_HEIGHT / 2);
    if (useBG)
      memcpy32(back_buffer, &bmp_background, SCREEN_WIDTH * SCREEN_HEIGHT / 2);
    else
      memset32(back_buffer, 0, SCREEN_WIDTH * SCREEN_HEIGHT / 2);
    //    each time_diff is 15.26 microseconds
    avg_diffs[avgi++] = time_diff;
    if (avgi == NUMAVGDIFFS)
      avgi = 0;
    int total = 0;
    for (int i = 0; i < NUMAVGDIFFS; i++) {
      total += avg_diffs[i];
    }
    if (key_state & KEY_LEFT) {
      // tYaw--;
      marioSprite.position.x -= movespeed;

    } else if (key_state & KEY_RIGHT) {
      // tYaw++;
      marioSprite.position.x += movespeed;
    }

    if (key_state & KEY_UP) {
      // camPos.x += fixed_mul(SIN_LUT[tYaw & 0xff], movespeed);
      // camPos.z += fixed_mul(COS_LUT[tYaw & 0xff], movespeed);
      marioSprite.position.z += movespeed;

    } else if (key_state & KEY_DOWN) {
      // camPos.x -= fixed_mul(SIN_LUT[tYaw & 0xff], movespeed);
      // camPos.z -= fixed_mul(COS_LUT[tYaw & 0xff], movespeed);
      marioSprite.position.z -= movespeed;

    } else if (key_state & KEY_A) {
      camPos.y += movespeed;
    } else if (key_state & KEY_B) {
      camPos.y -= movespeed;
    }
    if (key_state & KEY_L) {

      tPitch--;
    } else if (key_state & KEY_R) {
      tPitch++;
    }
    if ((key_state & KEY_SELECT) > oldSelect) {
      // do_wireframe ^= 1;
      camPose++;
      if (camPose == 3)
        camPose = 0;
      switch (camPose) {
      case 0:
        camPos.x = 0;
        camPos.y = 0x10000;
        camPos.z = -0x90000;
        tPitch = 0;
        tYaw = 0;
        tRoll = 0;
        break;
      case 1:
        camPos.x = 0;
        camPos.y = -0x90000;
        camPos.z = 0;
        tPitch = 192;
        tYaw = 0;
        tRoll = 0;
        break;
      case 2:
        camPos.x = 0;
        camPos.y = 0x28000;
        camPos.z = -0x90000;
        tPitch = 0;
        tYaw = 0;
        tRoll = 0;
        break;
      }
    }
    oldSelect = key_state & KEY_SELECT;

    if ((key_state & KEY_START) > oldStart) {
      useRealSprites ^= 1;
    }
    oldStart = key_state & KEY_START;
    step_mario_physics(&marioSprite);
    init_oam();
    clear_lists();

    // push_model(cube_model, origin);
    // push_model(cube_model, origin2);
    push_model(hexagon_model, origin3);
    // push_model(cube_model, origin4);
    // rot_matrix(rotMatrix2, (spinny >> 2) & 0xff, 0, 0);
    // spinny++;
    // push_model_xform(icosa_model, origin, rotMatrix2);
    // push_model(cube_model, origin);
    // push_model(cube_model, origin2);
    prep_sprite(&marioSprite);
    stop_right_there_criminal_scum(&marioSprite);
    translate(camPos);
    rot_matrix(rotMatrix, (-tYaw) & 0xff, (-tPitch) & 0xff, (-tRoll) & 0xff);
    /*posprintf(buffer, "%5d %5d %5d", rotMatrix[0] >> 8, rotMatrix[1] >> 8,
              rotMatrix[2] >> 8);
    show_string(buffer, 5, 7);
    posprintf(buffer, "%5d %5d %5d", rotMatrix[3] >> 8, rotMatrix[4] >> 8,
              rotMatrix[5] >> 8);
    show_string(buffer, 5, 13);
    posprintf(buffer, "%5d %5d %5d", rotMatrix[6] >> 8, rotMatrix[7] >> 8,
              rotMatrix[8] >> 8);
    show_string(buffer, 5, 19);*/
    matmul(rotMatrix, veclist_end);
    cull_polys();
    project();
    showtime();
    push_sprite(&marioSprite);
    if (onGround) {
      copy_bitmap((const u8 *)&bmps_mario, 0, 8);
    } else {
      copy_bitmap((const u8 *)&bmps_mario_fall, 0, 8);
    }
    cap_oam();

    posprintf(buffer, "%3dFPS %d/%dTRIS %dVERTS", 65536 * NUMAVGDIFFS / total,
              num_polys, num_culled_polys, veclist_end - VECLIST_BASE);
    show_string(buffer, 5, 1);

    posprintf(buffer, "%l %l %l", marioSprite.position.x,
              marioSprite.position.y, marioSprite.position.z);
    show_string(buffer, 5, 7);

    // flip the buffers
    flip_buffers();
    // wait for the next vblank
    // NOTE!! likely, mGBA isn't simulating OAM write blockin
    // find a better emulator
    // wait_blank();
    // OAM copy
    if (useRealSprites)
      memcpy32(OAM_BASE, oam_shadow, 256);
  }

  return 0;
}
