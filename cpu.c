#include "common.h"
#include "cpu.h"
#include "instr.h"
#include "mnemonics.h"

static char *uops_names[INSTR_UOP_MAX_COUNT] = {
  "END",
  "NOP",
  "NOPCNT",
  "BOOT_PREFETCH",
  "PREFETCH",
  "READ_BYTE",
  "READ_WORD",
  "READ_NEXT_WORD",
  "WRITE_BYTE",
  "WRITE_WORD",
  "WRITE_NEXT_WORD",
  "REG_COPY",
  "REG_SWAP",
  "SPECIAL",
  "EA_SPECIAL",
  "DEC_REG",
  "INC_REG",
  "ADD",
  "REG_COPY_EXT_TO_WORD",
  "REG_COPY_EXT_TO_LONG",
  "SET_BASIC_FLAGS",
  "SUB",
  "SET_SUB_FLAGS"
};

static char *states[3] = {
  "RESET",
  "HALT",
  "RUNNING"
};

void cpu_debug_info(struct cpu *cpu) {
  int i;
  printf("----------------------- [%s] %s -----------------------\n",
         cpu->exec->instr->code, cpu->exec->mnemonic(cpu, cpu->exec->instr_addr));
  for(i=0;i<8;i++) {
    printf("D%d: %08X %-20s A%d: %08X\n", i, cpu->internal->r.d[i], "", i, cpu->internal->r.a[i]);
  }
  printf("PC: %08X\n", cpu->internal->r.pc);
  printf("SR: %04X\n", cpu->internal->r.sr);
  printf("SSP: %08X\n", cpu->internal->r.ssp);
  printf("USP: %08X\n", cpu->internal->r.usp);
  printf("Prefetch: %04X %04X %04X\n", cpu->internal->r.irc, cpu->internal->r.ir, cpu->internal->r.ird);
  printf("Cycles: %ld\n", cpu->internal->cycles);
  printf("ICycle: %d (%d)\n", cpu->internal->icycle, cpu->exec->cycles);
  printf("Current uOP: %s (%d)\n", uops_names[cpu->exec->instr->uops[cpu->exec->uops_pos]->code], cpu->exec->uops_pos);
  printf("Current Instruction: $%08X %04X %s\n",
         cpu->exec->instr_addr, cpu->exec->op, cpu->exec->mnemonic(cpu, cpu->exec->instr_addr));
  printf("State: %s\n", states[cpu->internal->main_state]);
  printf("Values:");
  for(i=0;i<8;i++) {
    printf(" %08X", cpu->internal->r.value[i]);
  }
  printf("\n");
  printf("External: %08x %04x %d\n", cpu->external->address, cpu->external->data, cpu->external->data_available);
  printf("\n");
}

void cpu_set_reset_pin(struct cpu *cpu) {
  unused(cpu);
  /* TODO: All */
  return;
}

void cpu_clr_reset_pin(struct cpu *cpu) {
  unused(cpu);
  /* TODO: All */
  return;
}

void cpu_set_flags(struct cpu *cpu, WORD mask, int x, int n, int z, int v, int c) {
  WORD flags;

  /* Normalise inputs */
  x = !!x;
  n = !!n;
  z = !!z;
  v = !!v;
  c = !!c;

  /* Set all flags */
  flags = (x<<4)|(n<<3)|(z<<2)|(v<<1)|(c<<0);

  /* Mask off unused flags */
  flags &= ~mask;

  /* Mask off SR and set new flags */
  cpu->internal->r.sr = (cpu->internal->r.sr&mask) | flags;
}

struct cpu *cpu_setup(struct hw **hws) {
  struct cpu *cpu;

  cpu = (struct cpu *)ostis_alloc(sizeof(struct cpu));
  cpu->internal = (struct cpu_internal *)ostis_alloc(sizeof(struct cpu_internal));
  cpu->internal->main_state = CPU_RESET;
  
  cpu->external = (struct cpu_external *)ostis_alloc(sizeof(struct cpu_external));
  cpu->exec = (struct cpu_exec *)ostis_alloc(sizeof(struct cpu_exec));

  /* The BOOT instruction does not really exist, but is a special case for
   * the power on state of the machine, where SP, PC and an extra Prefetch
   * step is done.
   */
  cpu->exec->instr = instr_boot_setup(cpu);
  cpu->exec->uops_pos = 0;
  cpu->exec->cycles = 0;
  cpu->exec->instr_addr = 0;
  cpu->external->data_available = 0;

  cpu->hws = hws;

  /* Unimplemented is a fallback instruction reporting the missing op-code and exiting */
  instr_unimplemented_setup(cpu);
  instr_nop_setup(cpu);
  instr_reset_setup(cpu);
  instr_move_to_sr_setup(cpu);
  instr_cmpi_setup(cpu);
  instr_bcc_setup(cpu);
  instr_lea_setup(cpu);
  instr_suba_setup(cpu);
  instr_jmp_setup(cpu);
  instr_move_b_setup(cpu);
  instr_btst_setup(cpu);
  instr_moveq_setup(cpu);

  /* TODO: This will probably move into the the setup of each instruction eventually,
   * but to make it possible for unimplemented instructions to have correct mnemonics
   * it's done here for now
   */
  mnemonics_setup(cpu);
  
  printf("DEBUG-New Instruction: %s\n", cpu->exec->mnemonic(cpu, cpu->exec->instr_addr));
  return cpu;
}

void cpu_initiate_next_instruction(struct cpu *cpu) {
  cpu->exec->op = cpu->internal->r.ird;
  cpu->exec->instr = cpu->internal->instr[cpu->internal->r.ird];
  cpu->exec->mnemonic = cpu->internal->mnemonics[cpu->internal->r.ird];
  cpu->exec->instr_addr = cpu->internal->r.pc-4;
  printf("\n\n===========================================\n");
  printf("DEBUG-New Instruction: %08X %04X %s\n",
         cpu->exec->instr_addr, cpu->exec->op, cpu->exec->mnemonic(cpu, cpu->exec->instr_addr));
  printf("DEBUG: Cycles of last instruction: %d\n", cpu->exec->cycles);
  cpu->exec->uops_pos = 0;
  cpu->exec->cycles = 0;
  cpu->exec->counter = -1;
  cpu->external->data_available = 0;
}

void cpu_step_instr(struct cpu *cpu) {
  int pos;
  struct uop *uop;

  pos = cpu->exec->uops_pos;
  /* Check if previous instruction has ended */
  if(cpu->exec->instr->uops[pos]->code == INSTR_UOP_END) {
    if((cpu->exec->cycles % 2) == 1) {
      /* TODO: Transitionally. Should never end up on an odd cycle after an instruction */
      cpu_debug_info(cpu);
      printf("ERROR! Instruction either wrong or not on 1c uOPs yet: %s\n", cpu->exec->mnemonic(cpu, cpu->exec->instr_addr));
      exit(-104);
    }
    cpu->internal->icycle = cpu->exec->cycles;
    cpu_initiate_next_instruction(cpu);
    pos = cpu->exec->uops_pos;
  }
  uop = cpu->exec->instr->uops[pos];
  if(cpu->internal->cycles > 0) {
    printf("DEBUG: pos == %d\n", pos);
    cpu_debug_info(cpu);
  }
  uop->uop(uop, cpu);
  cpu->internal->cycles++;
  cpu->exec->cycles++;
}

void cpu_tick(struct hw *hw) {
  //  struct cpu *cpu;
  //  cpu = (struct cpu *)hw->data;
  cpu_step_instr(hw->data);
}

void cpu_instr_register(struct cpu *cpu, WORD op, WORD op_mask, struct instr *instr) {
  int i;
  for(i=0;i<65536;i++) {
    if((i&op_mask) == (op&op_mask)) {
      cpu->internal->instr[i] = instr;
      if(!instr->code) {
        printf("%04X missing code\n", i);
        exit(-112);
      }
    }
  }
}

