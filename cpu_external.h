#ifndef OSTIS_CPU_EXTERNAL_H
#define OSTIS_CPU_EXTERNAL_H

#include "common.h"

/* Part of data in use
 * 
 * Low byte
 * High byte
 * Full word
 */
enum data_part {
  CPU_DATA_LOW=0,
  CPU_DATA_HIGH,
  CPU_DATA_WORD
};

/* Types of operation
 * 
 * Read
 * Write
 */
enum op_types {
  CPU_READ=0,
  CPU_WRITE
};
  
/* struct cpu_external:
 * Mainly data corresponding to the pins of the CPU chip
 *
 * Address:        24 Address lines (with the non-existant A0 filled in using UDS/LDS)
 * Data:           16 Data lines
 * Op Type:        Type of operation (READ, WRITE)
 * Data Part:      Part of data lines used for operation (LOW, HIGH, WORD)
 * Data Available: Signal from MMU indicating that data is available for reading or writing (DTACK)
 * Halt:           CPU is halted
 */

struct cpu_external {
  LONG address;
  WORD data;
  enum op_types op_type;
  enum data_part data_part;
  int data_available;
  int halt;
};

#endif /* OSTIS_CPU_EXTERNAL_H */
