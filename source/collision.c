#include "bestovius.h"
#include "dimentio.h"
#include "helper.h"
VEC3 marioVel;
int onGround = 0;
void step_mario_physics(SPRITE *mario) {
  marioVel.y += 0x300;
  mario->position.x += marioVel.x;
  mario->position.y += marioVel.y;
  mario->position.z += marioVel.z;
}
int point_in_tri_2d(int x1, int y1, int x2, int y2, int x3, int y3, int x,
                    int y) {
  int cross1 = fixed_mul(x - x2, y1 - y2) - fixed_mul(x1 - x2, y - y2);
  int cross2 = fixed_mul(x - x3, y2 - y3) - fixed_mul(x2 - x3, y - y3);
  int cross3 = fixed_mul(x - x1, y3 - y1) - fixed_mul(x3 - x1, y - y1);
  if (cross1 > 0 && cross2 > 0 && cross3 > 0)
    return 1;
  if (cross1 < 0 && cross2 < 0 && cross3 < 0)
    return 1;
  return 0;
}
int wall_check(SPRITE *mario, POLY *wall) { return 0; }
int floor_check(SPRITE *mario, POLY *floor) {
  // find equation for plane containing tri
  int x1, y1, x2, y2, x3, y3, z1, z2, z3;
  x1 = VECLIST_BASE[floor->v1].x;
  x2 = VECLIST_BASE[floor->v2].x;
  x3 = VECLIST_BASE[floor->v3].x;
  y1 = VECLIST_BASE[floor->v1].y;
  y2 = VECLIST_BASE[floor->v2].y;
  y3 = VECLIST_BASE[floor->v3].y;
  z1 = VECLIST_BASE[floor->v1].z;
  z2 = VECLIST_BASE[floor->v2].z;
  z3 = VECLIST_BASE[floor->v3].z;
  if (!point_in_tri_2d(x1, z1, x2, z2, x3, z3, mario->position.x,
                       mario->position.z))
    return 0;
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
  int marioFloorY = fixed_div(d - fixed_mul(cx, x1) - fixed_mul(cz, z1), cy);
  int marioHeight = marioFloorY - mario->position.y;
  gregy = marioHeight;
  int collisionVal = 78643 / 2;
  if (marioHeight < collisionVal && marioHeight > 0) {
    mario->position.y = marioFloorY - collisionVal;
    marioVel.y = 0;
    onGround = 1;
    return 1;
  }
  return 0;
}
void stop_right_there_criminal_scum(SPRITE *mario) {
  // mario's position is his center
  // at distance 0x60000, he appears 16x32
  // at a distance of 1, any objects that are size 1 will take up the full
  // screen, thus, mario is 26214x78643 units in size?
  // floors will check if mario is less than 78643/2 units above them but yet
  // not below them, in which case he will be snapped up to that height,
  // vertical velocity arrested and onGround set walls TODO
  onGround = 0;
  for (int i = 0; i < num_polys; i++) {
    POLY *poly = &polylist[i];
    if (poly->flags & 16) {
      if (floor_check(mario, poly))
        break;
    } else if (poly->flags & 8) {
      if (wall_check(mario, poly))
        break;
    }
  }
}