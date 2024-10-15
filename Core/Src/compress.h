#ifndef COMPRESS_H_
#define COMPRESS_H_

#include "api.h"
#include "keccak/keccak.h"

/* LEGACY DEFINITION */
/* extern void hashArray(uint8_t *hash_out, const uint8_t *array_in, const uint64_t length); */

extern void hashArray(uint8_t *hash_out, const uint64_t length_out,
                      const uint8_t *array_in, const uint64_t length_in);
#endif