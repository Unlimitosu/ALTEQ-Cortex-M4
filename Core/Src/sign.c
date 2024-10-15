#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <stdint.h>

#include "api.h"
#include "compress.h"
#include "expand.h"
#include "matrix.h"
#include "atf.h"
#include "vec_sizes.h"

int crypto_sign_keypair(uint8_t  *pk, uint8_t  *sk)
{
  int i, r;

  uint8_t  seeds[((MAT_SK_SEED_SIZE * C) + PK_SEED_SIZE)];
  uint32_t atfC[LEN * NB32_VEC_C];
  uint32_t cols[N*N * NB32_VEC_C];

  memset(pk, 0x00, CRYPTO_PUBLICKEYBYTES);
  memset(sk, 0x00, CRYPTO_SECRETKEYBYTES);

  randomSeed(sk, SK_SEED_SIZE);

  /* Expanding Secret Key */
  /* Because MAT_SK_SEED_SIZE and PK_SEED_SIZE might differ, I prefer to consider one single large expansion */
  expandSeeds(seeds, sk, 1, SK_SEED_SIZE, ((MAT_SK_SEED_SIZE * C) + PK_SEED_SIZE));

  /* Generating N Columns Matrices such that acting on ATF_i with columns matrices i return atf_C */
  for (r = 0; r < C; r++)
      expandColumns(atfC + r * NB32_VEC_NN, seeds + r * MAT_SK_SEED_SIZE, MAT_SK_SEED_SIZE);
    for(i=0;i<N*N;i++)
      for(r=0; r<NB32_VEC_C; r++)
        cols[i*NB32_VEC_C + r]=atfC[r*NB32_VEC_NN+i];

  /* Expanding ATF_C */
  expandATF_vec_copy(atfC, seeds+C*MAT_SK_SEED_SIZE, NB32_VEC_C, NB32_VEC_C, PK_SEED_SIZE);
  invertingOnATF((uint32_t*)(pk), atfC, cols);

  /* Keeping Seed for ATF_C in both key (appended at the tail) */
  memcpy(pk+C*ALT_SIZE, seeds+C*MAT_SK_SEED_SIZE, PK_SEED_SIZE);

  /* free */


  return 0;
}

__attribute__((section(".sdram"))) uint8_t  hash_insign[(ALT_SIZE * NB32_VEC_ROUND + MSG_HASH_SIZE)];
__attribute__((section(".sdram"))) uint8_t  seeds_sk_insign[((MAT_SK_SEED_SIZE * C) + PK_SEED_SIZE)];
__attribute__((section(".sdram"))) uint32_t atfC_insign[LEN * NB32_VEC_ROUND];
__attribute__((section(".sdram"))) uint8_t  seeds_insign[EXPCOL_SIG_SEED_SIZE * ROUND];
__attribute__((section(".sdram"))) uint32_t cols_rnd_insign[NB32_VEC_NN * NB32_VEC_ROUND];
__attribute__((section(".sdram"))) uint32_t cols_insign[N * N * NB32_VEC_K];
int crypto_sign(uint8_t  *sm, uint64_t *smlen, const uint8_t  *m, uint64_t mlen, const uint8_t  *sk)
{

#if C < K
  uint64_t cols_exp[C];
#endif
  uint64_t chg_c[ROUND - K];
  uint64_t chg_nc[K];
  uint64_t chg_val[K];

  #if USE_SALT == 1
  uint8_t salt_value[SALT_SIZE];
  #endif

  uint32_t* tmp_ptr;
  uint8_t* char_ptr;
  int success;


  int i, r;

  if (!mlen){
    /* free and exit */

    return -1;}

  /* this hold the most space by far, and is often unusued: it is good as a temporary buffer */
  tmp_ptr = (uint32_t *)(hash_insign + MSG_HASH_SIZE);

  //memset(hash_insign, 0x00, ALT_SIZE * ROUND + MSG_HASH_SIZE);
  for(int i = 0; i < ALT_SIZE * ROUND + MSG_HASH_SIZE; i++) {
	  hash_insign[i] = 0;
  }
  //memset(sm, 0x00, CRYPTO_BYTES);
  for(int i = 0; i < CRYPTO_BYTES; i++) {
	  sm[i] = 0;
  }

  /* Expanding Secret Key  */
  /* Because MAT_SK_SEED_SIZE and PK_SEED_SIZE might differ, I prefer to consider one single large expansion */
  expandSeeds(seeds_sk_insign, sk, 1, SK_SEED_SIZE, ((MAT_SK_SEED_SIZE * C) + PK_SEED_SIZE));

  expandATF_vec_copy(atfC_insign, seeds_sk_insign+C*MAT_SK_SEED_SIZE, NB32_VEC_ROUND, NB32_VEC_ROUND, PK_SEED_SIZE);

  hashArray(hash_insign, MSG_HASH_SIZE, m, mlen);

  do
  {
    success = 1;

    /* Creating ROUND random N Columns matrices */
    #if USE_SALT == 0
    randomSeed(seeds_insign, SIG_SEED_SIZE);
    expandSeeds(seeds_insign, seeds_insign, ROUND, SIG_SEED_SIZE, SIG_SEED_SIZE);
    #else
    char_ptr = (uint8_t*)tmp_ptr;
    randomSeed(char_ptr, SIG_SEED_SIZE);
    expandSeeds(char_ptr, char_ptr, ROUND, SIG_SEED_SIZE, SIG_SEED_SIZE);
    /* create a SALT value, then append to every potential signature seed with position i */
    randomSeed(salt_value, SALT_SIZE);
    char_ptr = seeds_insign;
    for (r = 0; r < ROUND; r++){
      /* position for the SEED (not overwritten yet) */
      memcpy(char_ptr, ((uint8_t*)tmp_ptr) + r*SIG_SEED_SIZE, SIG_SEED_SIZE);
      char_ptr += SIG_SEED_SIZE;
      /* position for the SALT */
      memcpy(char_ptr, salt_value, SALT_SIZE);
      char_ptr += SALT_SIZE;
      /* position for the octet id */
      memset(char_ptr, (char)r, 1);
      char_ptr += 1;
    }
    #endif

    //printf("here start...\r\n");
    /* expand then put into vectorized form */
    /* we need to keep a non-vectorized version of cols_rnd because of random challenge picking */
    /* it is either this, or re-expanding the columns later/reorganizing (which is also cheap)*/
    for (r = 0; r < ROUND; r++)
      expandColumns(cols_rnd_insign + r * NB32_VEC_NN, seeds_insign + r * EXPCOL_SIG_SEED_SIZE, EXPCOL_SIG_SEED_SIZE);

    //printf("start next\r\n");

    for (i = 0; i < N * N; i++)
      for (r = 0; r < NB32_VEC_ROUND; r++)
        tmp_ptr[i * NB32_VEC_ROUND + r] = cols_rnd_insign[r * NB32_VEC_NN + i];
    //printf("long for end\r\n");
    /* Acting independently on ATFC ROUND time */
    actingOnATFS(tmp_ptr, atfC_insign, tmp_ptr); /* careful tmp_ptr also contains hash */
    //printf("1\r\n");

    /* Creating Challenge from hash */
    hashArray(sm, CHLG_SIZE, hash_insign, ALT_SIZE * ROUND + MSG_HASH_SIZE);
    //printf("2\r\n");

    expandChallenge(chg_c, chg_nc, chg_val, sm, CHLG_SIZE);
    //printf("3\r\n");

    /* overwrite cols_rnd to align properly necessary matrices for the rest of the program, and vectorize it */
    for (r = 0; r < K; r++)
      memcpy(tmp_ptr + NB32_VEC_NN * r, cols_rnd_insign + NB32_VEC_NN * chg_nc[r], sizeof(uint32_t) * NB32_VEC_NN);
    /* vectorize the result */
    for (i = 0; i < N * N; i++)
      for (r = 0; r < K; r++)
        cols_rnd_insign[i * NB32_VEC_K + r] = tmp_ptr[r * NB32_VEC_NN + i];

    /* Expanding KxN Columns matrices from the CxN from Secret Key: assume duplicates for vectorization */
#if C < K /* many collisions */
    memset(cols_exp, -1, C * 8); /* basically an unreachable value that will be changed */
    for (r = 0; r < K; r++)
      if (cols_exp[chg_val[r]] == -1)
      {
        expandColumns(tmp_ptr + NB32_VEC_NN * r, seeds_sk_insign + chg_val[r] * MAT_SK_SEED_SIZE, MAT_SK_SEED_SIZE);
        cols_exp[chg_val[r]] = r; /* store the position in which it has already been generated */
      }
      else /* already expanded somewhere, position stored in cols_exp[chg_val[r]] */
      {
        memcpy(tmp_ptr + NB32_VEC_NN * r, tmp_ptr + NB32_VEC_NN * cols_exp[chg_val[r]], sizeof(uint32_t) * NB32_VEC_NN);
      }
#else  /* C>K very few or no collisions */
    for (r = 0; r < K; r++)
      expandColumns(tmp_ptr + NB32_VEC_NN * r, seeds_sk_insign + chg_val[r] * MAT_SK_SEED_SIZE, MAT_SK_SEED_SIZE);
#endif /*C<K or K>C */
    /* vectorize the result */
    for (i = 0; i < N * N; i++)
      for (r = 0; r < K; r++)
        cols_insign[i * NB32_VEC_K + r] = tmp_ptr[r * NB32_VEC_NN + i];
    //printf("4\r\n");

    /* Construct K Matrices corresponding to challenge !=C */
    columnsMatrix(tmp_ptr, cols_rnd_insign, cols_insign, NB32_VEC_K);
    success = columnsDecomposition(tmp_ptr, NB32_VEC_K, K);
  } while (!success);

  /* Finally write the content of the signature */
  char_ptr = sm + CHLG_SIZE;

  #if USE_SALT == 1
  /* write the salt */
  memcpy(char_ptr, salt_value, SALT_SIZE);
  char_ptr += SALT_SIZE;
  #endif
  //printf("5\r\n");

  /* Write the ROUND - K seeds_insign corresponding to challenge =C */
  for (r = 0; r < ROUND - K; r++)
    memcpy(char_ptr + r * SIG_SEED_SIZE, seeds_insign + chg_c[r] * EXPCOL_SIG_SEED_SIZE, sizeof(uint8_t) * SIG_SEED_SIZE);

  /* this output is vectorized! Do not load useless elements when NB32_VEC_K > K */
  char_ptr += ((ROUND - K) * SIG_SEED_SIZE);
  for (r = 0; r < N * N; r++)
    memcpy(((uint32_t *)char_ptr) + r * K, tmp_ptr + r * NB32_VEC_K, sizeof(uint32_t) * K);

  *smlen = CRYPTO_BYTES;
  *smlen += mlen;
  memcpy(sm + CRYPTO_BYTES, m, mlen);

  /* free */


  return 0;
}


__attribute__((section(".sdram")))  uint8_t  hash_inopen[(ALT_SIZE * ROUND + MSG_HASH_SIZE)];
__attribute__((section(".sdram")))  uint32_t cols_inopen[N * N * NB32_VEC_ROUND];
__attribute__((section(".sdram")))  uint32_t atfs_inopen[LEN * NB32_VEC_ROUND];
int crypto_sign_open(uint8_t  *m, uint64_t *mlen, const uint8_t  *sm, uint64_t smlen, const uint8_t  *pk)
{
  int correct=0;
  int r, i;


  #if USE_SALT == 0
  const uint8_t* seeds_sig = sm + CHLG_SIZE;
  #else
  uint8_t seeds_sig[(EXPCOL_SIG_SEED_SIZE * (ROUND-K))];
  #endif

  uint8_t  chk[CHLG_SIZE];
  uint64_t chg_c[ROUND-K];
  uint64_t chg_nc[K];
  uint64_t chg_val[K];

  #if USE_SALT == 1
  uint8_t salt_value[SALT_SIZE];
  #endif

  uint8_t* char_ptr;
  uint8_t* char_ptr_out;


  memset(chk, 0x00, CHLG_SIZE);
  memset(hash_inopen, 0x00, ALT_SIZE * ROUND + MSG_HASH_SIZE);

  *mlen = smlen - CRYPTO_BYTES;
  memcpy(m, sm + CRYPTO_BYTES, *mlen);

  /* Expanding Challenge */
  expandChallenge(chg_c, chg_nc, chg_val, sm, CHLG_SIZE);

  #if USE_SALT == 1
  /* recover the salt appended to the end of the signature */
  char_ptr = ((uint8_t*)sm) + CHLG_SIZE;
  memcpy(salt_value, char_ptr, SALT_SIZE);
  char_ptr += SALT_SIZE;
  char_ptr_out = seeds_sig;
  /* set the seeds that have to be expanded for signature column matrices */
  for (r = 0; r < ROUND - K; r++){
    memcpy(char_ptr_out, char_ptr, SIG_SEED_SIZE);
    char_ptr += SIG_SEED_SIZE;
    char_ptr_out += SIG_SEED_SIZE;
    /* position for the SALT */
    memcpy(char_ptr_out, salt_value, SALT_SIZE);
    char_ptr_out += SALT_SIZE;
    /* position for the octet id */
    memset(char_ptr_out, chg_c[r], 1);
    char_ptr_out += 1;
  }
  #endif

  /* Expanding ROUND-K  N Columns Matrices correponding to challenge =C  */
  /* use atfs as a temporary buffer */
  for (r = 0; r < ROUND - K; r++)
    expandColumns(atfs_inopen + r * NB32_VEC_NN, seeds_sig + (r * EXPCOL_SIG_SEED_SIZE), EXPCOL_SIG_SEED_SIZE);
  /* pre-vectorize data before loading the vectorized data after */
  for (i = 0; i < N * N; i++)
    for (r = 0; r < ROUND - K; r++)
      cols_inopen[i * NB32_VEC_ROUND + r] = atfs_inopen[r * NB32_VEC_NN + i];

  /* Extract matrix for challenge <C*/
  /* load vectorized data, packs of K elements */
  char_ptr = (uint8_t*)(sm + CHLG_SIZE + SALT_SIZE + ((ROUND - K) * SIG_SEED_SIZE));
  for (i = 0; i < N * N; i++)
    memcpy(cols_inopen + (i * NB32_VEC_ROUND) + ROUND - K, (uint32_t *)(char_ptr) + i * K, K * sizeof(uint32_t));

  /* check values are all within [0, PRIME [: avoid signature forgeries by just adding values */
  for (i = 0; i < N * N ; i++)
    for (r = ROUND - K; r < ROUND; r++)
      correct |= (cols_inopen[i * NB32_VEC_ROUND + r] >= PRIME);
  /* check diagonal values are non-zero */
  for (i = 0; i < N; i++)
    for (r = ROUND - K; r < ROUND; r++)
      correct |= (cols_inopen[(i * (N + 1)) * NB32_VEC_ROUND + r] == 0);

  /* expanding and copy-pasting of the ATF (ROUND-K copies) need to vectorize properly (use vectorized ATFS) */
  expandATF_vec_copy(atfs_inopen, pk + C * ALT_SIZE, NB32_VEC_ROUNDmK, NB32_VEC_ROUND, PK_SEED_SIZE);
  /* Extract and vectorize corresponding ATF for challenge <C*/
  for (i = 0; i < LEN; i++)
    for (r = 0; r < K; r++)
      atfs_inopen[i * NB32_VEC_ROUND + r + ROUND - K] = ((uint32_t *)(pk + (ALT_SIZE * chg_val[r])))[i];

  actingOnATFS(atfs_inopen, atfs_inopen, cols_inopen);

  /* Preparing Hashing of ROUND-K ATF */
  char_ptr = hash_inopen + MSG_HASH_SIZE;
  for (r = 0; r < ROUND - K; r++)
    memcpy(char_ptr + chg_c[r] * ALT_SIZE, atfs_inopen + r * LEN, sizeof(uint32_t) * LEN);

  /* Preparing Hashing with the K last ATF */
  for (r = ROUND - K; r < ROUND; r++)
    memcpy(char_ptr + chg_nc[r - ROUND + K] * ALT_SIZE, atfs_inopen + r * LEN, sizeof(uint32_t) * LEN);

  hashArray(hash_inopen, MSG_HASH_SIZE, m, *mlen);
  hashArray(chk, CHLG_SIZE, hash_inopen, ALT_SIZE * ROUND + MSG_HASH_SIZE);

  /* Verfication */
  for (i = 0; i < CHLG_SIZE; i++)
    correct |= (sm[i] != chk[i]);

  /* free */

  #if USE_SALT == 1
  //_mm_free(seeds_sig);
  #endif

  return correct;
}
