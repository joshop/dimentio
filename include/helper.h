#pragma once
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef u16 COLOR;

#define INLINE static inline

#define IWRAM_CODE __attribute__((section(".iwram"), long_call))
#define IWRAM_VAR __attribute__((section(".iwram")))
#define EWRAM_VAR __attribute__((section(".ewram")))
#define ARM_CODE __attribute__((target("arm")))

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 128

#define MEM_IO 0x04000000
#define MEM_VRAM 0x06000000

#define REG_DISPCNT *((volatile u32 *)(MEM_IO + 0x0000))
#define REG_VCOUNT *(volatile u16 *)0x04000006

#define DCNT_MODE0 0x0000
#define DCNT_MODE1 0x0001
#define DCNT_MODE2 0x0002
#define DCNT_MODE3 0x0003
#define DCNT_MODE4 0x0004
#define DCNT_MODE5 0x0005
#define DCNT_BG0 0x0100
#define DCNT_BG1 0x0200
#define DCNT_BG2 0x0400
#define DCNT_BG3 0x0800
#define DCNT_OBJ 0x1000
#define DCNT_PAGEFLIP 0x0010
#define DCNT_OBJ_1D 0x0040

#define REG_BG2CNT *(volatile u16 *)0x0400000C
#define REG_BG2PA *(volatile s16 *)0x04000020
#define REG_BG2PB *(volatile s16 *)0x04000022
#define REG_BG2PC *(volatile s16 *)0x04000024
#define REG_BG2PD *(volatile s16 *)0x04000026
#define REG_BG2X *(volatile s32 *)0x04000028
#define REG_BG2Y *(volatile s32 *)0x0400002C

#define REG_IME *(volatile u16 *)0x04000208

#define REG_TM0D *(volatile u16 *)0x04000100
#define REG_TM0CNT *(volatile u16 *)0x04000102

#define REG_KEYINPUT *(volatile u16 *)0x04000130
#define KEY_A 0x1
#define KEY_B 0x2
#define KEY_SELECT 0x4
#define KEY_START 0x8
#define KEY_RIGHT 0x10
#define KEY_LEFT 0x20
#define KEY_UP 0x40
#define KEY_DOWN 0x80
#define KEY_R 0x100
#define KEY_L 0x200

INLINE COLOR RGB15(u32 red, u32 green, u32 blue) {
  return red | (green << 5) | (blue << 10);
}

#define fixed_float(n) ((n) / 65536.0)
#define fixed_int(n) ((n) >> 16)
#define float_fixed(n) ((n)*65536)
#define int_fixed(n) ((n) << 16)

#define CLR_BLACK 0x0000
#define CLR_RED 0x001F
#define CLR_LIME 0x03E0
#define CLR_YELLOW 0x03FF
#define CLR_BLUE 0x7C00
#define CLR_MAG 0x7C1F
#define CLR_CYAN 0x7FE0
#define CLR_WHITE 0x7FFF

#define OAM_BASE ((u16 *)0x07000000)

INLINE s32 fixed_mul(s32 a, s32 b) { return ((long long)a * b) >> 16; }
INLINE s32 fixed_div(s32 a, s32 b) { return ((long long)a << 16) / b; }