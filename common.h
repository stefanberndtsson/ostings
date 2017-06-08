#ifndef OSTIS_COMMON_H
#define OSTIS_COMMON_H

#include <stdint.h>
#include "logger.h"

typedef uint16_t WORD;
typedef uint32_t LONG;

void *ostis_alloc(size_t);
void ostis_free(void *ptr);

#endif /* OSTIS_COMMON_H */

