#include "dimentio.h"
#include "graphics.h"
#include "triglut.h"
#include <stdlib.h>
POLY polylist[256];
int num_polys = 0;
int proj_xs[64];
int proj_ys[64];
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
    // for (int i = 8; i < 12; i++) {
    push_poly(model.polys[i]);
    polylist[num_polys - 1].v1 += vec_offset;
    polylist[num_polys - 1].v2 += vec_offset;
    polylist[num_polys - 1].v3 += vec_offset;
  }
}
void project() {
  VEC3 *vec;
  int i = veclist_end - VECLIST_BASE;
  for (vec = veclist_end - 1; (s32)vec & VECLIST_TEST; vec--) {
    proj_xs[--i] = (SCREEN_HEIGHT / 2) + (vec->x * FOVSCALEX) / vec->z;
    proj_ys[i] = (SCREEN_WIDTH / 2) + (vec->y * FOVSCALEY) / vec->z;
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
int IWRAM_CODE ARM_CODE zsort(const void *poly1, const void *poly2) {
  if (((POLY *)poly1)->v1 == -1) {
    // treat -1 as "extremely close"
    return 1;
  }
  if (((POLY *)poly2)->v1 == -1) {
    return -1;
  }
  int za1, za2, za3, zb1, zb2, zb3;
  za1 = VECLIST_BASE[((POLY *)poly1)->v1].z;
  za2 = VECLIST_BASE[((POLY *)poly1)->v2].z;
  za3 = VECLIST_BASE[((POLY *)poly1)->v3].z;
  zb1 = VECLIST_BASE[((POLY *)poly2)->v1].z;
  zb2 = VECLIST_BASE[((POLY *)poly2)->v2].z;
  zb3 = VECLIST_BASE[((POLY *)poly2)->v3].z;
  return ((zb1 + zb2 + zb3) - (za1 + za2 + za3));
}
extern u16 *back_buffer;
int num_culled_polys;
int do_bfc = 0;
void IWRAM_CODE ARM_CODE cull_polys() {
  num_culled_polys = num_polys;
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
    if (z1 < 1 << 16 || z2 < 1 << 16 || z3 < 1 << 16) {
      num_culled_polys--;
      polylist[i].v1 = -1;
      continue;
    }
    int ax, ay, az, bx, by, bz;
    ax = x2 - x1;
    ay = y2 - y1;
    az = z2 - z1;
    bx = x3 - x1;
    by = y3 - y1;
    bz = z3 - z1;
    int cx, cy, cz;
    cx = fixed_mul(ay, bz) - fixed_mul(az, by);
    cy = fixed_mul(az, bx) - fixed_mul(ax, bz);
    cz = fixed_mul(ax, by) - fixed_mul(ay, bx);
    int d;
    d = fixed_mul(cx, x1) + fixed_mul(cy, y1) + fixed_mul(cz, z1);
    if (d < 0 && do_bfc) {
      num_culled_polys--;
      polylist[i].v1 = -1;
    }
  }
}
void IWRAM_CODE ARM_CODE showtime() {
  // num_polys = 2;
  qsort(polylist, num_polys, sizeof(POLY), &zsort);
  for (int i = 0; i < num_culled_polys; i++) {
    int px1, px2, px3, py1, py2, py3;
    px1 = proj_xs[polylist[i].v1];
    px2 = proj_xs[polylist[i].v2];
    px3 = proj_xs[polylist[i].v3];
    py1 = proj_ys[polylist[i].v1];
    py2 = proj_ys[polylist[i].v2];
    py3 = proj_ys[polylist[i].v3];

    fill_tri2(px1, py1, px2, py2, px3, py3,
              polylist[i].clr); // polylist[i].clr
    // bmp16_line(px1, py1, px2, py2, CLR_WHITE, back_buffer, SCREEN_WIDTH * 2);
    // bmp16_line(px3, py3, px2, py2, CLR_WHITE, back_buffer, SCREEN_WIDTH * 2);
    // bmp16_line(px1, py1, px3, py3, CLR_WHITE, back_buffer, SCREEN_WIDTH * 2);
  }
}