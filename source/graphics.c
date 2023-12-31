#include "asmfunc.h"
#include "bmp.h"
#include "dimentio.h"
#include "helper.h"
void plot_pixel(int y, int x, COLOR clr) {
  back_buffer[y * SCREEN_WIDTH + x] = clr;
}

COLOR query_pixel(int y, int x) { return back_buffer[y * SCREEN_WIDTH + x]; }

void bmp16_line(int y1, int x1, int y2, int x2, u32 clr, void *dstBase,
                u32 dstPitch) {
  int ii, dx, dy, xstep, ystep, dd;
  if (x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0 || x1 > SCREEN_WIDTH ||
      x2 > SCREEN_WIDTH || y1 > SCREEN_HEIGHT || y2 > SCREEN_HEIGHT)
    return;
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
#define FLEX_MASK 0x1f
#define SWAP_T(a, b)                                                           \
  t = a;                                                                       \
  a = b;                                                                       \
  b = t
void IWRAM_CODE ARM_CODE draw_span(int x1, int x2, int y, COLOR *flex,
                                   int rootx, int rooty) {
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
  if ((u32)flex < 0x10000) {
    memset16(back_buffer + (y * SCREEN_WIDTH + x1), (u16)(u32)flex, x2 - x1);
    return;
  }
  COLOR *flex_point = ((COLOR *)flex) + FLEX_WIDTH * ((y - rooty) & FLEX_MASK) +
                      ((x1 - rootx) & FLEX_MASK);
  memcpy16(back_buffer + (y * SCREEN_WIDTH + x1), flex_point, x2 - x1);
}
void IWRAM_CODE ARM_CODE fill_tri2(int y1, int x1, int y2, int x2, int y3,
                                   int x3, COLOR *color, int rootO) {
  int t;
  s32 s1, s2, s3;
  int rootx = 0;
  int rooty = 0;
  if (x1 < -SCREEN_WIDTH || x1 > 2 * SCREEN_WIDTH || x2 < -SCREEN_WIDTH ||
      x2 > 2 * SCREEN_WIDTH || x3 < -SCREEN_WIDTH || x3 > 2 * SCREEN_WIDTH ||
      y1 < -SCREEN_HEIGHT || y1 > 2 * SCREEN_HEIGHT || y2 < -SCREEN_HEIGHT ||
      y2 > 2 * SCREEN_HEIGHT || y3 < -SCREEN_HEIGHT || y3 > 2 * SCREEN_HEIGHT) {
    return;
  }
  if ((u32)color > 0x10000) {
    switch (rootO) {
    case 0:
      rootx = (x1 + x2 + x3) / 3 - 16;
      rooty = (y1 + y2 + y3) / 3 - 16;
      break;
    case 1:
      rootx = x1 - 16;
      rooty = y1 - 16;
      break;
    case 2:
      rootx = x2 - 16;
      rooty = y2 - 16;
      break;
    case 3:
      rootx = x3 - 16;
      rooty = y3 - 16;
      break;
    }
  }
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
  if (y1 == y3)
    return;
  s1 = y1 == y2 ? 0 : fixed_div(int_fixed(x2 - x1), int_fixed(y2 - y1));
  s3 = fixed_div(int_fixed(x3 - x1), int_fixed(y3 - y1));
  s2 = y3 == y2 ? 0 : fixed_div(int_fixed(x3 - x2), int_fixed(y3 - y2));
  if (y1 != y2) {
    s32 xa, xb;
    xa = x1 << 16;
    xb = x1 << 16;
    for (int y = y1; y < y2; y++) {
      draw_span(xa >> 16, xb >> 16, y, color, rootx, rooty);
      xa += s1;
      xb += s3;
    }
  }
  if (y2 != y3) {
    s32 xa, xb;
    xa = x3 << 16;
    xb = x3 << 16;
    for (int y = y3; y >= y2; y--) {
      draw_span(xa >> 16, xb >> 16, y, color, rootx, rooty);
      xa -= s2;
      xb -= s3;
    }
  }
  // fill_tri2top
}
/*
0000
...0 CF
..0.
.0.. 5F
0000

*/
// u32 digits[12] = {0xF999F, 0xF2262, 0xF8F1F, 0xF1F1F, 0x11F99, 0xF1F8F,
//                   0xF9F8F, 0x1111F, 0xF9F9F, 0xF1F9F, 0x88F8F, 0x88F9F};
const u32 ascii_glyph[128] = {
    0x00000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00000,
    0x00000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00000,
    0x00000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00000,
    0x00000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00000, 0x00000,
    0x00000, 0x40444, 0x00099, 0xAFAFA, 0xF5FCF, 0xA42A0, 0x00000, 0x00011,
    0x48884, 0x21112, 0x00525, 0x04E40, 0x84000, 0x00F00, 0x33000, 0x84210,
    0xF999F, 0xF2262, 0xF8F1F, 0xF1F1F, 0x11F99, 0xF1F8F, 0xF9F8F, 0x1111F,
    0xF9F9F, 0xF1F9F, 0x04040, 0x84040, 0x12421, 0x0F0F0, 0x84248, 0x4071F,
    0xFBB8F, 0x99F9F, 0xF9FAE, 0xF888F, 0xE999E, 0xF8E8F, 0x88E8F, 0xF9B8F,
    0x99F99, 0xF444F, 0xC444F, 0x9ACA9, 0xF8888, 0x999FF, 0x9BDD9, 0xF999F,
    0x88F9F, 0xFB99F, 0x9AF9F, 0xF1F8F, 0x4444F, 0xF9999, 0x25999, 0xFF999,
    0x55255, 0x22255, 0xF421F};
void show_glyph(int glyph, int x, int y) {
  u32 pattern = ascii_glyph[glyph];
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 4; j++) {
      if (pattern & 1)
        plot_pixel(x - j, y + i, CLR_WHITE);
      pattern >>= 1;
    }
  }
}
/*
void show_number(u32 number, int x, int y) {
  if (number == 0) {
    show_digit(0, x, y);
    return;
  }
  while (number) {
    show_digit(number % 10, x, y);
    number /= 10;
    x -= 5;
  }
}*/
void show_string(const char *str, int x, int y) {
  for (; *str; str++) {
    show_glyph(*str, x, y);
    x += 5;
  }
}