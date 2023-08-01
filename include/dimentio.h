#pragma once
#include "helper.h"
typedef struct {
  s32 x;
  s32 y;
  s32 z;
} VEC3;
typedef struct {
  int v1;
  int v2;
  int v3;
  COLOR *clr;
  int flags;
} POLY;
// poly flags:
// -------- -------- -------- ---FWVRR
// R root override (00: center, 01-11: vertex)
// V invisible
// W wall collision
// F floor collision
typedef struct {
  int num_vecs;
  int num_polys;
  const VEC3 *vecs;
  const POLY *polys;
} MODEL;
#define VECLIST_BASE ((VEC3 *)0x02010000)
#define VECLIST_TEST 0x00010000
#define FLEX_WIDTH 320
#define FLEX_HEIGHT 32
// todo: figure out a way to make sure the veclist stays open
#define FOVSCALEX SCREEN_HEIGHT
#define FOVSCALEY SCREEN_WIDTH
extern POLY polylist[256];
extern int num_polys;
extern int num_culled_polys;
extern int proj_xs[64];
extern int proj_ys[64];
extern int do_bfc;
extern int gregx, gregy;
extern int do_wireframe;
extern VEC3 *veclist_end;
void clear_lists();                          // clears the vector and poly lists
void push_vec(VEC3 vec);                     // adds the vec to veclist
void push_poly(POLY poly);                   // adds the poly to the poly list
void push_model(MODEL model, VEC3 position); // adds the model to the list
void push_model_xform(MODEL model, VEC3 position,
                      s32 *matrix); // adds the model to the list, transformed
void project();                     // projects all vectors into screen space
void translate(VEC3 camera);        // translates all vectors to camera
void rot_matrix(s32 *matrix, int yaw, int pitch, int roll); // generate rot mat
void IWRAM_CODE ARM_CODE
cull_polys(); // backface cull and remove too close/behind polys
void IWRAM_CODE ARM_CODE showtime(); // it's showtime
