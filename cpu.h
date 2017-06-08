#ifndef OSTIS_CPU_H
#define OSTIS_CPU_H

#include "common.h"
#include "hw.h"
#include "cpu_internal.h"
#include "cpu_external.h"
#include "cpu_instr.h"

struct cpu {
  struct cpu_internal *internal;
  struct cpu_external *external;
  struct cpu_instr *instr;
};

void cpu_tick(struct hw *);
struct cpu *cpu_setup();
void cpu_set_instr_state(struct cpu *, enum instr_states, int counter);

#endif /* OSTIS_CPU_H */
