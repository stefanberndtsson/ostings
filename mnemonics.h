#ifndef OSTIS_MNEMONICS_H
#define OSTIS_MNEMONICS_H

#include "cpu.h"

char *mnemonics_at(struct cpu *, LONG);
void mnemonics_setup(struct cpu *);
void mnemonics_register(struct cpu *, WORD, WORD, mnemonics_t *);

void mnemonics_unimplemented_setup(struct cpu *);
void mnemonics_boot_setup(struct cpu *);
void mnemonics_reset_setup(struct cpu *);
void mnemonics_nop_setup(struct cpu *);
void mnemonics_move_to_sr_setup(struct cpu *);
void mnemonics_cmpi_setup(struct cpu *);
void mnemonics_bcc_setup(struct cpu *);

#endif /* OSTIS_MNEMONICS_H */
