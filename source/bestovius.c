#include "bestovius.h"
#include "asmfunc.h"
#include "dimentio.h"
#include "graphics.h"
EWRAM_VAR u16 oam_shadow[512];
u16 *oam_ptr;
int num_aff_mats = 0;
void init_oam() {
  oam_ptr = oam_shadow;
  num_aff_mats = 0;
}
void prep_sprite(SPRITE *sprite) {
  // this loads the sprite into buffers
  push_vec(sprite->position);
  sprite->veci = veclist_end - VECLIST_BASE - 1;

  /*POLY fake;
  fake.v1 = -2;
  fake.v2 = sprite->veci;
  fake.v3 = (u32)sprite;
  push_poly(fake);*/
}
void load_aff_mat(int idx, s16 pa, s16 pb, s16 pc, s16 pd) {
  u16 *ptr = &oam_ptr[(idx << 4) + 3];
  *ptr = pa;
  ptr += 4;
  *ptr = pb;
  ptr += 4;
  *ptr = pc;
  ptr += 4;
  *ptr = pd;
}
void copy_bitmap(const u8 *bitmap, int tile, int size) {
  // 64 bytes so 32 words per tile
  memcpy16(((u16 *)0x06014000) + tile, bitmap, size * 32);
}
void push_sprite(SPRITE *sprite) {
  // this actually displays the sprite
  // normal x coordinates run 0-128 and y coordinates 0-160
  //
  // plot_pixel(proj_xs[sprite->veci], proj_ys[sprite->veci], CLR_YELLOW);
  int proj_x = (proj_xs[sprite->veci] * 480) >> 8;
  int proj_y = proj_ys[sprite->veci];
  // proj_x = 240 - 16;
  // proj_y = 160 - 32;
  proj_x -= 8;
  proj_y -= 20;
  // if (proj_x < 0 || proj_x >= 240 || proj_y < 0 || proj_y >= 160)
  //   return;
  int z = VECLIST_BASE[sprite->veci].z;
  if (z < 0)
    return;
  proj_y -= sprite->height >> 1;
  proj_x -= sprite->width >> 1;
  s16 pa, pb, pc, pd;
  // scale operation
  pa = (z << 8) / (sprite->size);
  // pa = 0x100;
  pb = 0;
  pc = 0;
  // pd = 0x100;
  pd = (z << 8) / (sprite->size);
  load_aff_mat(num_aff_mats++, pa, pb, pc, pd);
  // attr0
  // ss100001 YYYYYYYY
  *(oam_ptr++) = (proj_y & 0xff) | ((sprite->dim & 0xc) << 12) |
                 0x2300; // 0x2100 for affine
  // attr1
  // SSiiiiiX XXXXXXXX
  *(oam_ptr++) = (proj_x & 0x1ff) | ((sprite->dim & 0x3) << 14) |
                 ((num_aff_mats - 1) << 9);
  *(oam_ptr++) = sprite->tile | 0x0200;
  // treat the affine matrices with respect
  oam_ptr++;
}
void cap_oam() {
  while (oam_ptr - oam_shadow < 512) {
    *oam_ptr = 0x0200; // disable sprite rendering
    oam_ptr += 4;
  }
}