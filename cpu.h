#ifndef OSTIS_CPU_H
#define OSTIS_CPU_H

struct cpu;

#include "common.h"
#include "hw.h"
#include "cpu_internal.h"
#include "cpu_external.h"
#include "cpu_exec.h"

struct cpu {
  struct cpu_internal *internal;
  struct cpu_external *external;
  struct cpu_exec *exec;
  struct hw **hws;
};

void cpu_tick(struct hw *);
struct cpu *cpu_setup(struct hw **);
void cpu_set_reset_pin(struct cpu *);
void cpu_clr_reset_pin(struct cpu *);
void cpu_instr_register(struct cpu *, WORD, WORD, struct instr *);

#endif /* OSTIS_CPU_H */
