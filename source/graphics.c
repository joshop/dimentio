#include "asmfunc.h"
#include "bmp.h"
#include "helper.h"
INLINE void plot_pixel(int y, int x, COLOR clr) {
  back_buffer[y * SCREEN_WIDTH + x] = clr;
}

void bmp16_line(int y1, int x1, int y2, int x2, u32 clr, void *dstBase,
                u32 dstPitch) {
  int ii, dx, dy, xstep, ystep, dd;
  u16 *dst = (u16 *)(dstBase + y1 * dstPitch + x1 * 2);
  dstPitch /= 2;

  // --- Normalization ---
  if (x1 > x2) {
    xstep = -1;
    dx = x1 - x2;
  } else {
    xstep = +1;
    dx = x2 - x1;
  }

  if (y1 > y2) {
    ystep = -dstPitch;
    dy = y1 - y2;
  } else {
    ystep = +dstPitch;
    dy = y2 - y1;
  }

  // --- Drawing ---

  if (dy == 0) // Horizontal
  {
    for (ii = 0; ii <= dx; ii++)
      dst[ii * xstep] = clr;
  } else if (dx == 0) // Vertical
  {
    for (ii = 0; ii <= dy; ii++)
      dst[ii * ystep] = clr;
  } else if (dx >= dy) // Diagonal, slope <= 1
  {
    dd = 2 * dy - dx;

    for (ii = 0; ii <= dx; ii++) {
      *dst = clr;
      if (dd >= 0) {
        dd -= 2 * dx;
        dst += ystep;
      }

      dd += 2 * dy;
      dst += xstep;
    }
  } else // Diagonal, slope > 1
  {
    dd = 2 * dx - dy;

    for (ii = 0; ii <= dy; ii++) {
      *dst = clr;
      if (dd >= 0) {
        dd -= 2 * dy;
        dst += xstep;
      }

      dd += 2 * dx;
      dst += ystep;
    }
  }
}

#define SWAP_T(a, b)                                                           \
  t = a;                                                                       \
  a = b;                                                                       \
  b = t
void draw_span(int x1, int x2, int y, int color) {
  int t;
  if (y < 0 || y >= SCREEN_HEIGHT)
    return;
  if (x1 < 0)
    x1 = 0;
  if (x1 >= SCREEN_WIDTH)
    x1 = SCREEN_WIDTH - 1;
  if (x2 < 0)
    x2 = 0;
  if (x2 >= SCREEN_WIDTH)
    x2 = SCREEN_WIDTH - 1;
  if (x1 > x2) {
    SWAP_T(x1, x2);
  }
  memset16(back_buffer + (y * SCREEN_WIDTH + x1), color, x2 - x1);
}
void fill_tri(int y1, int x1, int y2, int x2, int y3, int x3, int color) {
  int t;
  s32 s1, s2, s3;
  s32 ss, se;
  s32 start, end;
  int xmid;
  int xa, xb;
  int y;
  if (x1 < -SCREEN_WIDTH || x1 > 2 * SCREEN_WIDTH || x2 < -SCREEN_WIDTH ||
      x2 > 2 * SCREEN_WIDTH || x3 < -SCREEN_WIDTH || x3 > 2 * SCREEN_WIDTH ||
      y1 < -SCREEN_HEIGHT || y1 > 2 * SCREEN_HEIGHT || y2 < -SCREEN_HEIGHT ||
      y2 > 2 * SCREEN_HEIGHT || y3 < -SCREEN_HEIGHT || y3 > 2 * SCREEN_HEIGHT) {
    // todo: this is a little sketchy, and could become obviously so eventually
    // figure out actually why this is necessary, and how to fix it
    return;
  }
  // envision the triangle as a long side and two short sides
  // the long side is the one from the top pt to the bottom pt
  // the other part contains a breakpoint where the direction changes
  // sort points first, top to bottom
  if (y1 > y2) {
    SWAP_T(x1, x2);
    SWAP_T(y1, y2);
  }
  if (y2 > y3) {
    SWAP_T(x2, x3);
    SWAP_T(y2, y3);
  }
  if (y1 > y2) {
    SWAP_T(x1, x2);
    SWAP_T(y1, y2);
  }
  y = y1;
  start = int_fixed(x1);
  end = start;
  // start and end are bounds for our slice
  // if (y1 == y2 || y1 == y3 || y2 == y3) return;
  if (y1 == y3)
    return;
  s1 = y1 == y2 ? 0 : fixed_div(int_fixed(x2 - x1), int_fixed(y2 - y1));
  s3 = fixed_div(int_fixed(x3 - x1), int_fixed(y3 - y1));
  s2 = y3 == y2 ? 0 : fixed_div(int_fixed(x3 - x2), int_fixed(y3 - y2));
  // the slopes for each segment
  if (y1 == y2) {
    // hardcode case
    if (x1 < x2) {
      start = int_fixed(x1);
      end = int_fixed(x2);
    } else {
      start = int_fixed(x2);
      end = int_fixed(x1);
    }
    while (y < y3) {
      xa = fixed_int(start);
      xb = fixed_int(end);
      // s1 is A to B, that's bad
      // s3 is A to C
      // s2 is B to C
      draw_span(xa, xb, y, color);
      start += (x1 < x2 ? s3 : s2);
      end += (x1 < x2 ? s2 : s3);
      y++;
    }
  }
  // xmid is the x coordinate of the long side, at the middle point
  xmid = x1 + fixed_int(fixed_mul(s3, int_fixed(y2 - y1)));

  // if xmid is less than x2, end sees the break point
  // else, start will see it
  if (xmid < x2) {
    se = s1;
    ss = s3;
  } else {
    se = s3;
    ss = s1;
  }
  // when we hit y = y2, we're at the break point
  // we want to set the one that was s1 to s2
  // that will probably change the sign
  // though not necessarily
  while (y < y3) {
    if (y == y2) {
      if (xmid < x2) {
        se = s2;
      } else {
        ss = s2;
      }
    }
    // fill from start to end
    // todo: don't do as much bounds checking
    xa = fixed_int(start);
    xb = fixed_int(end);
    draw_span(xa, xb, y, color);
    start += ss;
    end += se;
    y++;
  }
  /*draw_line(x1, y1, x2, y2, 191);// 0xe0);
  draw_line(x2, y2, x3, y3, 191);// 0xe0);
  draw_line(x1, y1, x3, y3, 191);// 0xe0);*/
}