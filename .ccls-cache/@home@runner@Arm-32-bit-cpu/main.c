#include "main.h"

int main(void) {
  struct cpu cpu;
  cpu.registers[pc] = 0x0;
  cpu_save_word(&cpu, 0, BRANCH_AND_EXCHANGE_FORMAT | 0b00010000);
  cpu.registers[r0] = 0x52;
  cpu_loop(&cpu);
  printf("%u, %o\n", cpu.registers[pc], cpu.cpsr);
  return 0;
}

WORD cpu_load_word(struct cpu *cpu, WORD address)
{
  WORD memory = cpu->ram[address+3];
  memory <<= 8;
  memory |= cpu->ram[address+2];
  memory <<= 8;
  memory |= cpu->ram[address+1];
  memory <<= 8;
  memory |= cpu->ram[address];
  return memory;
}

void cpu_save_word(struct cpu *cpu, WORD address, WORD value){
  for (int i = 0; i < 4; i++)
    {
      cpu->ram[address+i] = value >> sizeof(BYTE)*i*8;
      printf("byte %d saved as %u\n", i, cpu->ram[address+i]);
    } 
}

HALF_WORD cpu_load_halfword(struct cpu *cpu, HALF_WORD address)
{
  HALF_WORD memory = cpu->ram[address+1];
  memory <<= sizeof(BYTE)*8;
  memory |= cpu->ram[address];
  return memory;
}

void cpu_save_halfword(struct cpu *cpu, WORD address, HALF_WORD value){
  for (int i = 0; i < 2; i++)
    {
      cpu->ram[address+i] = value >> sizeof(BYTE)*i*8;
      printf("byte %d saved as %u\n", i, cpu->ram[address+i]);
    } 
}

void cpu_loop(struct cpu *cpu)
{
  if((cpu->cpsr & THUMB_MASK) == THUMB_MASK)
  {
  HALF_WORD instruction = cpu_load_halfword(cpu, cpu->registers[pc]);
  
    cpu_execute_thumb_instruction(cpu, instruction);
    
  }
  else
  {
    WORD instruction = cpu_load_word(cpu, cpu->registers[pc]);
  
    cpu_execute_arm_instruction(cpu, instruction);
    
  }
  
}

void cpu_execute_arm_instruction( struct cpu *cpu, WORD instruction)
{
  switch(decode_arm_instruction(instruction))
    {
      case BRANCH_AND_EXCHANGE:
      arm_branch_and_exchange(cpu, instruction);
        break;
      case BLOCK_DATA_TRANSFER:
        arm_block_data_transfer(cpu, instruction);
        break;
      case BRANCH:
        arm_branch(cpu, instruction);
        break;
    }
}

void cpu_execute_thumb_instruction( struct cpu *cpu, HALF_WORD instruction)
{
  if((instruction & STATUS_MASK) != (cpu->cpsr & STATUS_MASK))
  {
    return;
  }
}

int decode_arm_instruction(WORD instruction) {
  if ((instruction & BRANCH_AND_EXCHANGE_MASK) == BRANCH_AND_EXCHANGE_FORMAT)
  {
    return BRANCH_AND_EXCHANGE;
  }
  if ((instruction & BLOCK_DATA_TRANSFER_MASK) == BLOCK_DATA_TRANSFER_FORMAT)
  {
    return BLOCK_DATA_TRANSFER;
  }
  if ((instruction & BRANCH_MASK) == BRANCH_FORMAT)
  {
    return BRANCH;
  }
  if ((instruction & SOFTWARE_INTERRUPT_MASK) == SOFTWARE_INTERRUPT_FORMAT)
  {
    return SOFTWARE_INTERRUPT;
  }
  if ((instruction & UNDEFINED_MASK) == UNDEFINED_FORMAT)
  {
    return UNDEFINED;
  }
  if ((instruction & SINGLE_DATA_TRANSFER_MASK) == SINGLE_DATA_TRANSFER_FORMAT)
  {
    return SINGLE_DATA_TRANSFER;
  }
  if ((instruction & SINGLE_DATA_SWAP_MASK) == SINGLE_DATA_SWAP_FORMAT)
  {
    return SINGLE_DATA_SWAP;
  }
  if ((instruction & MULTIPLY_MASK) == MULTIPLY_FORMAT)
  {
    return MULTIPLY;
  }
  if ((instruction & HALFWORD_DATA_TRANSFER_MASK) == HALFWORD_DATA_TRANSFER_FORMAT)
  {
    return HALFWORD_DATA_TRANSFER;
  }
  if ((instruction & PSR_TRANSFER_MASK) == PSR_TRANSFER_FORMAT)
  {
    return PSR_TRANSFER;
  }
  if ((instruction & DATA_PROCCESSING_MASK) == DATA_PROCCESSING_FORMAT)
  {
    return DATA_PROCCESSING;
  }
  return UNIMPLEMENTED_INSTRUCTION;
}

int decode_thumb_instruction(HALF_WORD instruction);

void arm_branch_and_exchange(struct cpu *cpu, WORD instruction)
{
  if ((instruction & STATUS_MASK) != (cpu->cpsr & STATUS_MASK))
  {
    cpu->registers[pc] += sizeof(WORD);
    return;
  }
  BYTE opcode = instruction & 0b11110000;
  BYTE reg = instruction & 0b1111;
  switch (opcode >> 4){
    case 0b0011:
      cpu->registers[lr] = cpu->registers[pc] + (sizeof(WORD) * 2);
    case 0b0001:
      cpu->registers[pc] = cpu->registers[reg]; 
      cpu->cpsr |= THUMB_MASK;
      break;
    case 0b0010:
      cpu->registers[pc] = cpu->registers[reg]; 
      cpu->cpsr &= JAZZELE_MASK;
      break;
      
  }
}

void arm_software_interrupt(struct cpu *cpu, WORD instruction)
{
  BYTE opcode = instruction >> 24;
  opcode &= 0b1111;
  switch (opcode)
    {
      case 0b1111: // SWI
        {
          if ((instruction & STATUS_MASK) != (cpu->cpsr & STATUS_MASK))
  {
    cpu->registers[pc] += sizeof(WORD);
    return;
  }
          cpu_change_mode(cpu, MODE_SUPERVISOR);
          // implement software interrupt mode
        }
      case 0b0001: // BKPT
        if ((instruction >> 20 & 0b1111) == 0b0010 && (instruction >> 4 & 0b1111) == 0b0111)
        {
          // implement arm cpu debugging
        }
    }
}

void arm_branch(struct cpu *cpu, WORD instruction)
{
  BYTE opcode = instruction >> 24 & 0b1;
  // getting rid of the top 8 bits
  int32_t offset = instruction << 8;
  offset >>= 8;
  switch (opcode)
    {
      case 0b0: //no link
        cpu->registers[pc] += offset;
      break;
      case 0b1: // link
        cpu->registers[lr] = cpu->registers[pc];
        cpu->registers[pc] += offset;
      break;
    }
}

void cpu_change_mode(struct cpu *cpu, enum cpsr_mode mode)
{
  cpu->cpsr &= mode;
  cpu->cpsr |= mode;
  switch (mode)
    {
      case MODE_USER:
      case MODE_SYSTEM:
        break;
      case MODE_SUPERVISOR:
         for(int i = sp_svc; i < svc_reg_count-1; i++)
           {
             cpu->svc_regs[i] = cpu->registers[i+sp];
           }
          cpu->svc_regs[spsr_svc] = cpu->cpsr;
        break;
      case MODE_FIQ:
        for(int i = r8_fiq; i < fiq_reg_count-1; i++)
           {
             cpu->fiq_regs[i] = cpu->registers[i+r8];
           }
          cpu->fiq_regs[spsr_fiq] = cpu->cpsr;
        break;
      case MODE_ABORT:
        for(int i = sp_abt; i < abt_reg_count-1; i++)
           {
             cpu->abt_regs[i] = cpu->registers[i+sp];
           }
          cpu->abt_regs[spsr_abt] = cpu->cpsr;
        break;
      case MODE_IRQ:
        for(int i = sp_irq; i < irq_reg_count-1; i++)
           {
             cpu->irq_regs[i] = cpu->registers[i+sp];
           }
          cpu->irq_regs[spsr_irq] = cpu->cpsr;
        break;
      case MODE_UNDEFINED:
        for(int i = sp_und; i < und_reg_count-1; i++)
           {
             cpu->und_regs[i] = cpu->registers[i+sp];
           }
          cpu->und_regs[spsr_und] = cpu->cpsr;
        break;



    }
}

void arm_block_data_transfer(struct cpu *cpu, WORD instruction)
{
  uint16_t reg_list = instruction;
  BYTE base_reg = (instruction >> 16) & 0b1111;
  BYTE options = (instruction >> 20) & 0b11111;
  
}

void arm_data_proccessing(struct cpu *cpu, WORD instruction)
{
  BYTE opcode = (instruction >> 21) & 0xf;
  WORD op1 = cpu->registers[(instruction >> 16) & 0b1111];
  BYTE rd = (instruction >> 12) & 0b1111;
  WORD op2 = 0;
  if ((instruction >> 25) & 0b1 == 0b1)
  {
    BYTE ror = instruction >> 7;
    ror &= 0b1111;
    op2 = instruction & 0b11111111;
    ror *= 2;
    for (int i = ror; i > 0; i--)
      {
        op2 = ((op2 & 0b1) << 31) | (op2 >> 1);
      }
  }
    
  switch (opcode)
    {
      case 0x0: // and
        cpu->registers[rd] = op1 & op2;
        break;
      case 0x1: // eor
        cpu->registers[rd] = op1 ^ op2;
        break;
      case 0x2: // sub
        cpu->registers[rd] = op1 - op2;
        break;
      case 0x3: // rsv
        cpu->registers[rd] = op2 - op1;
        break;
      case 0x4: // add
        cpu->registers[rd] = op1 + op2;
        if (cpu->registers[rd] < op1 && cpu->registers[rd] < op2)
        {
          cpu->cpsr |= FLAG_C << 28;
        }
        break;
      case 0x5: // adc
        cpu->registers[rd] = op1 + op2;
        if (((cpu->cpsr >> 28) & FLAG_C) == FLAG_C)
        {
          cpu->registers[rd] += 1;
        }
        if (cpu->registers[rd] < op1 && cpu->registers[rd] < op2)
        {
          cpu->cpsr |= FLAG_C << 28;
        }
        break;
      case 0x6: // sbc
        cpu->registers[rd] = op1 - op2 - 1;
        if (((cpu->cpsr >> 28) & FLAG_C) == FLAG_C)
        {
          cpu->registers[rd] += 1;
        }
        break;
      case 0x7: // rsc
        cpu->registers[rd] = op2 - op1 - 1;
        if (((cpu->cpsr >> 28) & FLAG_C) == FLAG_C)
        {
          cpu->registers[rd] += 1;
        }
        break;
      case 0x8: // tst
        op1 & op2;
        break;
      case 0x9: // teq
        break;
      case 0xa: // cmp
        break;
      case 0xb: // cmn
        break;
      case 0xc: // orr
        break;
      case 0xd: // mov
        break;
      case 0xe: // bic
        break;
      case 0xf: // mvn
        break;
    }
}
