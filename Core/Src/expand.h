#ifndef EXPAND_H_
#define EXPAND_H_

#include <string.h>
#include <stdlib.h>
#include "api.h"
#include "compress.h"
#include "aes.h"
#include "nistseedexpander.h"
/* just for the expanders constants */
#include "vec_sizes.h"


/* LEGACY FUNCTIONS */
/*
extern void randomSeed(uint8_t *seed);

extern void expandSeeds(uint8_t *seeds, const uint8_t *seed, const int nSeeds);

extern void expandChallenge(uint64_t *chg_c, uint64_t *chg_nc, uint64_t *chg_val, const uint8_t *seed);

extern void expandColumns(uint32_t *col, const uint8_t *seed);

extern void expandATF_vec_copy(uint32_t *atf, const uint8_t *seed, const int nb_copies, const int vec_size);
*/

/* Due to Beullens' attack, set flexible seed_sizes */
extern void randomSeed(uint8_t *seed, const size_t seed_size);

extern void expandSeeds(uint8_t *seeds, const uint8_t *seed, const int nSeeds,
                        const size_t src_seed_size, const size_t out_seed_size);

extern void expandChallenge(uint64_t *chg_c, uint64_t *chg_nc, uint64_t *chg_val,
                            const uint8_t *seed, const size_t seed_size);

extern void expandColumns(uint32_t *col, const uint8_t *seed, const size_t seed_size);

extern void expandATF_vec_copy(uint32_t *atf, const uint8_t *seed, const int nb_copies,
                               const int vec_size, const size_t seed_size);

#endif
