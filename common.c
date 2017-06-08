#include "common.h"

/* Allocate or DIE! */
void *ostis_alloc(size_t size) {
  void *ptr;

  ptr = malloc(size);
  if(!ptr) {
    FATAL("Unable to allocate");
    exit(-99);
  }
  return ptr;
}

/* Free or DIE?!?! */
void ostis_free(void *ptr) {
  free(ptr);
}
