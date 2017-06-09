#ifndef OSTIS_UOPS_H
#define OSTIS_UOPS_H

#include "instr.h"

void uop_unop(struct cpu *);
void uop_boot_prefetch(struct cpu *);
void uop_prog_read(struct cpu *);
void uop_read_word(struct cpu *);
void uop_end(struct cpu *);

#endif /* OSTIS_UOPS_H */
