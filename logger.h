#ifndef OSTIS_LOGGER_H
#define OSTIS_LOGGER_H

#include <stdio.h>
#include <stdlib.h>

#define FATAL(msg) do { fprintf(stderr, "%s\n", msg); exit(99); } while(0)
#define DEBUG(msg) do { fprintf(stderr, "DEBUG: %s\n", msg); } while(0)

#endif /* OSTIS_LOGGER_H */
