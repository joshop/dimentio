#include "helper.h"
#define BUFFER_PAGEFLIP 0xA000
u16 *back_buffer = (u16 *)MEM_VRAM;

void flip_buffers() {
  REG_DISPCNT ^= DCNT_PAGEFLIP;
  back_buffer = (u16 *)((u32)back_buffer ^ BUFFER_PAGEFLIP);
}

void wait_blank() {
  while (REG_VCOUNT >= 160)
    ;
  while (REG_VCOUNT < 160)
    ;
}

void init_bmp() {
  REG_DISPCNT = DCNT_MODE5 | DCNT_BG2 | DCNT_PAGEFLIP | DCNT_OBJ | DCNT_OBJ_1D;
  REG_BG2X = 0;
  REG_BG2Y = 0;
  REG_BG2PA = 0;
  REG_BG2PB = 256;
  REG_BG2PC = 137;
  REG_BG2PD = 0;
}