#include "cpu.h"

/* These EA uOPs are executed prior to reads/writes,
 * mainly to prepare the read/write addresses from register
 * based fetches.
 */

void ea_uop_fetch_dreg(struct cpu *cpu) {
  int reg_num;
  reg_num = cpu->exec->instr->uop_data[cpu->exec->uops_pos];
  cpu->exec->data_in = cpu->internal->d[reg_num];
  cpu->exec->uops_pos++;
}

void ea_uop_fetch_areg(struct cpu *cpu) {
  int reg_num;
  reg_num = cpu->exec->instr->uop_data[cpu->exec->uops_pos];
  cpu->exec->data_in = cpu->internal->a[reg_num];
  cpu->exec->uops_pos++;
}

void ea_uop_fetch_mem(struct cpu *cpu) {
  
}
