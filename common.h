#ifndef OSTIS_COMMON_H
#define OSTIS_COMMON_H

#include <stdint.h>
#include "logger.h"

#define SIGN_EXT_WORD(x) ((x&0x8000) ? ((uint32_t)(x))|0xffff0000 : x)
#define SIGN_EXT_BYTE(x) ((x&0x80) ? ((uint32_t)(x))|0xffffff00 : x)

typedef uint8_t  BYTE;
typedef uint16_t WORD;
typedef uint32_t LONG;

void *ostis_alloc(size_t);
void ostis_free(void *ptr);

#endif /* OSTIS_COMMON_H */

