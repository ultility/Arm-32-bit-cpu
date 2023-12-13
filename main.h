#pragma once
#include <stdint.h>
#include <stdio.h>

#define MHz 1000000
#define KHz 1000
#define KB 1024
#define WORD uint32_t
#define HALF_WORD uint16_t
#define BYTE uint8_t
// placeholder values
static const int BIOS_SIZE = 256;
static const int RAM_SIZE = 32 * KB;

static const int MAX_CLOCK_SPEED = 16.78 * MHz;

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
  STATUS_MASK = 0b1111 << 27
};

enum arm_instruction_formats {
  BRANCH_AND_EXHANGE_FORMAT = 0b100101111111111110001<<5,
  BLOCK_DATA_TRANSFER_FORMAT = 0b100<<25,
  BRANCH_FORMAT = 0b1010<<24,
  BRANCH_AND_LINK_FORMAT = 0b1011<<24,
  SOFTWARE_INTERRUPT_FORMAT = 0b1111<<24,
  UNDEFINED_FORMAT = 0b011 << 25 | 0b1 << 4,
  SINGLE_DATA_TRANSFER_FORMAT = 0b01 << 26,
  SINGLE_DATA_SWAP_FORMAT = 0b00010 << 23 | 0b00001001 << 4,
  MULTIPLY_FORMAT = 0b00000 << 23 | 0b1001 << 4,
  MULTIPLY_LONG_FORMAT = 0b00001 << 23 | 0b1001 << 4,
  HALFWORD_DATA_TRANSFER_FORMAT = 0b000 << 25 | 0b1 << 7 | 0b1 << 4,
  PSR_TRANSFER_FORMAT = 0b00 << 26 | 0b10 << 23,
  DATA_PROCCESSING_FORMAT = 0b00 << 26
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
  pc,
  reg_count
};

struct cpu {
  uint32_t registers[reg_count];

  BYTE rom[BIOS_SIZE];
  BYTE ram[RAM_SIZE];
  uint32_t cpsr;
};

struct bus {};

WORD cpu_load_word(struct cpu *cpu, WORD address);

void cpu_save_word(struct cpu *cpu, WORD address, WORD value);

void cpu_save_halfword(struct cpu *cpu, WORD address, HALF_WORD value);

HALF_WORD cpu_load_halfword(struct cpu *cpu, HALF_WORD address);

void cpu_execute_thumb_instruction( struct cpu *cpu, HALF_WORD instruction);

void cpu_execute_arm_instruction( struct cpu *cpu, WORD instruction);