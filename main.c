#include <stdio.h>
#include <stdint.h>

#define WORD uint32_t
#define HALF_WORD uint16_t
#define BYTE uint8_t
// placeholder values
static const int BIOS_SIZE = 256;
static const int RAM_SIZE = 256;

static const int REGISTER_AMOUMT = 16;
enum cspr_masks {
  MODE_MASK = 0b11111,
  THUMB_MASK = 0b1 << 4,
  FIQ_MASK = 0b1 << 5,
  IRQ_MASK = 0b1 << 6,
  ABORT_MASK = 0b1 << 7,
  ENDIAN_MASK = 0b1 << 8,
  IT_MASK = 0b111111 << 9,
  GE_MASK = 0b1111 << 15,
  JAZZELE_MASK = 0b1 << 23,
  THUMBV2_IT_MASK = 0b11 << 24,
  SATURATION_MASK = 0b1 << 26,
  STATUS_MASK = 0b11111 << 27
};

enum cpsr_mode {
  MODE_USER = 0b10000,
  MODE_FIQ = 0b10001,
  MODE_IRQ = 0b10010,
  MODE_SUPERVISOR = 0b10011,
  MODE_ABORT = 0b10111,
  MODE_UNDEFINED = 0b11011,
  MODE_SYSTEM = 0b11111
};

enum cpsr_alu_flag {
  FLAG_V = 0b0001,
  FLAG_C = 0b0010,
  FLAG_Z = 0b0100,
  FLAG_N = 0b1000
};

enum registers {
  r0 = 0,
  r1,
  r2,
  r3,
  r4,
  r5,
  r6,
  r7,
  r8,
  r9,
  r10,
  r11,
  r12,
  sp,
  ra,
  link = ra,
  pc
};

struct cpu {
  uint32_t registers[REGISTER_AMOUMT];

  uint32_t rom[BIOS_SIZE];
  uint32_t ram[RAM_SIZE];
  uint32_t cpsr;

};

struct bus {

};
int main(void) {
  printf("%d, %d\n", ra, link);
  return 0;
}