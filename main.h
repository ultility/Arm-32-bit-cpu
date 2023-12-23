#pragma once
#include <stdint.h>
#include <stdio.h>

#define MHz 1000000
#define KHz 1000
#define KB 1024
#define WORD uint32_t
#define HALF_WORD uint16_t
#define BYTE uint8_t

static const int BIOS_SIZE = 16 * KB;
static const int RAM_SIZE = 288 * KB;

static const int STACK_SIZE = 1 * KB;
static const int STACK_START = RAM_SIZE - STACK_SIZE - 1;

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
  BRANCH_AND_EXCHANGE_FORMAT = 0b100101111111111110001<<5,
  BLOCK_DATA_TRANSFER_FORMAT = 0b100<<25,
  BRANCH_FORMAT = 0b101<<25,
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

enum arm_instruction_masks {
    BRANCH_AND_EXCHANGE_MASK = 0b111111111111111111111<<5,
  BLOCK_DATA_TRANSFER_MASK = 0b111<<25,
  BRANCH_MASK = 0b111<<25,
  SOFTWARE_INTERRUPT_MASK = 0b1111<<24,
  UNDEFINED_MASK = 0b111 << 25 | 0b1 << 4,
  SINGLE_DATA_TRANSFER_MASK = 0b11 << 26,
  SINGLE_DATA_SWAP_MASK = 0b11111 << 23 | 0b11111111 << 4,
  MULTIPLY_MASK = 0b11111 << 23 | 0b1111 << 4,
  MULTIPLY_LONG_MASK = 0b11111 << 23 | 0b1111 << 4,
  HALFWORD_DATA_TRANSFER_MASK = 0b111 << 25 | 0b1 << 7 | 0b1 << 4,
  PSR_TRANSFER_MASK = 0b11 << 26 | 0b11 << 23,
  DATA_PROCCESSING_MASK = 0b11 << 26
};

enum arm_instructions {
  BRANCH_AND_EXCHANGE,
  BLOCK_DATA_TRANSFER,
  BRANCH,
  SOFTWARE_INTERRUPT,
  UNDEFINED,
  SINGLE_DATA_TRANSFER,
  SINGLE_DATA_SWAP,
  MULTIPLY,
  HALFWORD_DATA_TRANSFER,
  PSR_TRANSFER,
  DATA_PROCCESSING,
  UNIMPLEMENTED_INSTRUCTION
};

enum thumb_instruction_format : uint16_t {
  THUMB_SOFTWARE_INTERRUPT_FORMAT = 0b11011111 << 8,
  THUMB_UNCONDITIONAL_BRANCH_FORMAT = 0b11100 << 11,
  THUMB_CONDITIONAL_BRANCH_FORMAT = 0b1101 << 12,
  THUMB_MULTI_LOAD_STORE_FORMAT = 0b1100 << 12,
  THUMB_LONG_BRANCH_LINK_FORMAT = 0b1111 << 12,
  THUMB_OFFSET_SP_FORMAT = 0b10110000 << 8,
  THUMB_PUSH_POP_STACK_FORMAT = 0b1011 << 12 | 0b10 << 9,
  THUMB_LOAD_STORE_HALFWORD_FORMAT = 0b1000 << 12,
  THUMB_SP_RELATIVE_LOAD_STORE_FORMAT = 0b1001,
  THUMB_LOAD_ADDRESS_FORMAT = 0b1010 << 12,
  THUMB_IMMEDIATE_OFFSET_LOAD_STORE_FORMAT = 0b011 << 13,
  THUMB_REG_OFFSET_LOAD_STORE_FORMAT = 0b0101 << 12 | 0b0 << 9,
  //THUMB_LOAD_STORE_SIGN_EXTENDED_FORMAT 0b <<
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
  lr = ra,
  pc,
  reg_count,
  sp_svc = 0,
  lr_svc,
  spsr_svc,
  svc_reg_count,
  r8_fiq = 0,
  r9_fiq,
  r10_fiq,
  r11_fiq,
  r12_fiq,
  sp_fiq,
  lr_fiq,
  spsr_fiq,
  fiq_reg_count,
  sp_abt = 0,
  lr_abt,
  spsr_abt,
  abt_reg_count,
  sp_irq = 0,
  lr_irq,
  spsr_irq,
  irq_reg_count,
  sp_und = 0,
  lr_und,
  spsr_und,
  und_reg_count,
};

struct cpu {
  uint32_t registers[reg_count];
  uint32_t svc_regs[svc_reg_count];
  uint32_t fiq_regs[fiq_reg_count];
  uint32_t abt_regs[abt_reg_count];
  uint32_t irq_regs[irq_reg_count];
  uint32_t und_regs[und_reg_count];
  BYTE rom[BIOS_SIZE];
  BYTE ram[RAM_SIZE];
  uint32_t cpsr;
};

struct bus {};

void cpu_loop(struct cpu *cpu);

WORD cpu_load_word(struct cpu *cpu, WORD address);

void cpu_save_word(struct cpu *cpu, WORD address, WORD value);

void cpu_save_halfword(struct cpu *cpu, WORD address, HALF_WORD value);

HALF_WORD cpu_load_halfword(struct cpu *cpu, HALF_WORD address);

int decode_arm_instruction(WORD instruction);

int decode_thumb_instruction(HALF_WORD instruction);

void cpu_execute_thumb_instruction( struct cpu *cpu, HALF_WORD instruction);

void cpu_execute_arm_instruction( struct cpu *cpu, WORD instruction);

void arm_branch_and_exchange(struct cpu *cpu, WORD instruction);

void arm_block_data_transfer(struct cpu *cpu, WORD instruction);

void arm_branch(struct cpu *cpu, WORD instruction);

void arm_software_interrupt(struct cpu *cpu, WORD instruction);

void cpu_change_mode(struct cpu *cpu, enum cpsr_mode mode);

void arm_data_proccessing(struct cpu *cpu, WORD instruction);

void add_overflow_check(struct cpu *cpu, int op1, int op2);
