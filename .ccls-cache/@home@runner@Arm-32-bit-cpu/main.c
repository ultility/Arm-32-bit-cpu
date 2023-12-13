#include "main.h"

int main(void) {
  struct cpu cpu;
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
    cpu->registers[pc] += sizeof(HALF_WORD);
  }
  else
  {
    WORD instruction = cpu_load_word(cpu, cpu->registers[pc]);
  
    cpu_execute_arm_instruction(cpu, instruction);
    cpu->registers[pc] += sizeof(WORD);
  }
  
}

void cpu_execute_arm_instruction( struct cpu *cpu, WORD instruction)
{
  if((instruction & STATUS_MASK) != (cpu->cpsr & STATUS_MASK))
  {
    return;
  }
}

void cpu_execute_thumb_instruction( struct cpu *cpu, HALF_WORD instruction)
{
  if((instruction & STATUS_MASK) != (cpu->cpsr & STATUS_MASK))
  {
    return;
  }
}
