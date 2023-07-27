#include "dimentio.h"
#include "graphics.h"
#include "triglut.h"
#include <stdlib.h>
POLY polylist[256];
int num_polys = 0;
VEC3 *veclist_end = VECLIST_BASE;
void clear_lists() {
  num_polys = 0;
  veclist_end = VECLIST_BASE;
}
void push_vec(VEC3 v) { *(veclist_end++) = v; }
void push_poly(POLY poly) { polylist[num_polys++] = poly; }
void push_model(MODEL model, VEC3 position) {
  int vec_offset = veclist_end - VECLIST_BASE;
  for (int i = 0; i < model.num_vecs; i++) {
    push_vec(model.vecs[i]);
    (veclist_end - 1)->x += position.x;
    (veclist_end - 1)->y += position.y;
    (veclist_end - 1)->z += position.z;
  }
  for (int i = 0; i < model.num_polys; i++) {
    push_poly(model.polys[i]);
    polylist[num_polys - 1].v1 += vec_offset;
    polylist[num_polys - 1].v2 += vec_offset;
    polylist[num_polys - 1].v3 += vec_offset;
  }
}
void project() {
  VEC3 *vec;
  for (vec = veclist_end - 1; (s32)vec & VECLIST_TEST; vec--) {
    vec->x = (SCREEN_HEIGHT / 2) + (vec->x * FOVSCALEX) / vec->z;
    vec->y = (SCREEN_WIDTH / 2) + (vec->y * FOVSCALEY) / vec->z;
  }
}
void translate(VEC3 cam) {
  VEC3 *vec;
  for (vec = veclist_end - 1; (s32)vec & VECLIST_TEST; vec--) {
    vec->x -= cam.x;
    vec->y -= cam.y;
    vec->z -= cam.z;
  }
}
void rot_matrix(s32 *matrix, int yaw, int pitch, int roll) {
  matrix[0] = fixed_mul(COS_LUT[yaw], COS_LUT[pitch]);
  matrix[1] =
      fixed_mul(COS_LUT[yaw], fixed_mul(SIN_LUT[pitch], SIN_LUT[roll])) -
      fixed_mul(SIN_LUT[yaw], COS_LUT[roll]);
  matrix[2] =
      fixed_mul(COS_LUT[yaw], fixed_mul(SIN_LUT[pitch], COS_LUT[roll])) +
      fixed_mul(SIN_LUT[yaw], SIN_LUT[roll]);
  matrix[3] = fixed_mul(SIN_LUT[yaw], COS_LUT[pitch]);
  matrix[4] =
      fixed_mul(SIN_LUT[yaw], fixed_mul(SIN_LUT[pitch], SIN_LUT[roll])) +
      fixed_mul(COS_LUT[yaw], COS_LUT[roll]);
  matrix[5] =
      fixed_mul(SIN_LUT[yaw], fixed_mul(SIN_LUT[pitch], COS_LUT[roll])) -
      fixed_mul(COS_LUT[yaw], SIN_LUT[roll]);
  matrix[6] = -SIN_LUT[pitch];
  matrix[7] = fixed_mul(COS_LUT[pitch], SIN_LUT[roll]);
  matrix[8] = fixed_mul(COS_LUT[pitch], COS_LUT[roll]);
}
int zsort(const void *poly1, const void *poly2) {
  int za1, za2, za3, zb1, zb2, zb3;
  za1 = VECLIST_BASE[((POLY *)poly1)->v1].z;
  za2 = VECLIST_BASE[((POLY *)poly1)->v2].z;
  za3 = VECLIST_BASE[((POLY *)poly1)->v3].z;
  zb1 = VECLIST_BASE[((POLY *)poly2)->v1].z;
  zb2 = VECLIST_BASE[((POLY *)poly2)->v2].z;
  zb3 = VECLIST_BASE[((POLY *)poly2)->v3].z;
  return (zb1 + zb2 + zb3) - (za1 + za2 + za3);
}

void showtime() {
  qsort(polylist, num_polys, sizeof(POLY), &zsort);
  for (int i = 0; i < num_polys; i++) {
    int x1, y1, x2, y2, x3, y3, z1, z2, z3;
    x1 = VECLIST_BASE[polylist[i].v1].x;
    x2 = VECLIST_BASE[polylist[i].v2].x;
    x3 = VECLIST_BASE[polylist[i].v3].x;
    y1 = VECLIST_BASE[polylist[i].v1].y;
    y2 = VECLIST_BASE[polylist[i].v2].y;
    y3 = VECLIST_BASE[polylist[i].v3].y;
    z1 = VECLIST_BASE[polylist[i].v1].z;
    z2 = VECLIST_BASE[polylist[i].v2].z;
    z3 = VECLIST_BASE[polylist[i].v3].z;
    if (z1 < 1 << 16 || z2 < 1 << 16 || z3 < 1 << 16)
      return;
    fill_tri(x1, y1, x2, y2, x3, y3, polylist[i].clr);
  }
}