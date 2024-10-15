#include "compress.h"

/* Hashing of an array */
void hashArray(uint8_t *hash_out, const uint64_t length_out, const uint8_t *array_in, const uint64_t length_in)
{
  /* old version has constantly LAMBDA/4 for the challenge size */
  /* Keccak(1600-4*LAMBDA, 4*LAMBDA, array_in, length, 0x06, hash_out, LAMBDA/4);*/
  Keccak(1600-4*LAMBDA, 4*LAMBDA, array_in, length_in, 0x06, hash_out, length_out);
}