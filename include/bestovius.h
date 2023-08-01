#pragma once
#include "dimentio.h"
#include "helper.h"
typedef struct {
  VEC3 position;
  int dim; // dimension, ShSz
  int width;
  int height;
  int tile; // for OAM
  int size; // size in 3-space
  int veci;
  COLOR qp1;
  COLOR qp2;
  COLOR qp3;
  COLOR qp4;
  // todo angles
} SPRITE;
// sprite size is also the distance at which the sprite appears "real size"
extern u16 oam_shadow[512];
void init_oam();
void prep_sprite(SPRITE *sprite);
void push_sprite(SPRITE *sprite);
void cap_oam();
void copy_bitmap(const u8 *bitmap, int tile, int size);