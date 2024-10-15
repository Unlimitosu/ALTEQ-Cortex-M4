#include "expand.h"
#include <stdio.h>
#include "randombytes.h"
#include "nistseedexpander.h"

/* Randomly generate a seed */
void randomSeed(uint8_t *seed, const size_t seed_size)
{
  randombytes(seed, seed_size);
}

/* Init seed expander with a seed */
static void initExpander(AES_XOF_struct *rng, const uint8_t *seed, const size_t seed_size)
{
  uint8_t diversifier[8];
  uint8_t key[32];

  memset(diversifier, 0x00, 8);

  if (seed_size > 32) {
    fprintf(stderr, "initExpander error, seed size too large: seed_size %lu",seed_size);
    exit(1);
  }
  memset(key+seed_size, 0x00, 32-seed_size);
  memcpy(key, seed, seed_size);

  seedexpander_init_customized(rng, key, diversifier, 4294967295);
}

/* Expand a seed to a set of seeds */
void expandSeeds(uint8_t *seeds, const uint8_t *seed, const int nSeeds, const size_t src_seed_size, const size_t out_seed_size)
{
  #if SHAKE_FOR_EXPANDERS == 0

  AES_XOF_struct rng[1];
  initExpander(rng, seed, src_seed_size);
  seedexpander(rng, seeds, nSeeds*out_seed_size);

  #else /* do not use AES, juse use Keccak instead and one shot everything */
  hashArray(seeds, nSeeds*out_seed_size, seed, src_seed_size);
  #endif
}

/* Evaluate the number of bits required to represent all elements stricly inferior to a given value */
static uint64_t sizeOfValue(uint64_t val)
{
  int val_size=1;
  while ((1lu<<val_size)<val)
    val_size++;
  return val_size;
}

/* Use seed expander and a buffer of already generated random value to generate a random value of [0,max[ */
static uint64_t randomValue(AES_XOF_struct *rng, uint64_t *buf, uint64_t *buf_size, uint64_t max, uint64_t max_size)
{
  uint64_t r;
  do{
    while ((*buf_size)<max_size)
      {
	uint8_t c;
	seedexpander(rng, &c, 1lu);
	(*buf)=((*buf)<<8)+(uint64_t)c;
	(*buf_size)+=8;
      }
    r=(*buf)&((1lu<<max_size)-1lu);
    (*buf)=((*buf)>>max_size);
    (*buf_size)-=max_size;
  } while (r>=max);
  return r;
}

/* Expand a seed to a challenge of ROUND value with K value equal to C and ROUND-K value in [0,C[ */
void expandChallenge(uint64_t *chg_c, uint64_t *chg_nc, uint64_t *chg_val, const uint8_t *seed, const size_t seed_size)
{
  AES_XOF_struct rng[1];
  uint64_t buf=0;
  uint64_t buf_size=0;
  long k0=0;
  long k1=0;
  long r,k,i;
  uint64_t chg[ROUND];
  uint64_t C_SIZE=sizeOfValue(C);

  /* this is still done with AES-256, hopefully not a problem */
  /* just like the old ALTEQ, we use the first bits and not everything */
  if (seed_size < 32){
    fprintf(stderr, "expandChallenge error, seed size too low: seed_size %lu",seed_size);
    exit(1);
  }
  initExpander(rng, seed, 32);

#if ROUND-K<K
  memset(chg, 0x00, ROUND*8);
  /* Pick randomly ROUND-K coefficients of the challenge to be equal to C */
  for(k=0;k<ROUND-K;k++)
    {
      r=randomValue(rng, &buf, &buf_size, ROUND-k,sizeOfValue(ROUND-k));
      for (i=0;i<=r;i++)
	if (chg[i]==C)
	  r++;
      chg[r]=C;
    }
  /* Fix other coefficients of the challenge */
  for (i=0;i<ROUND;i++)
    if (chg[i]==0)
      chg[i]=randomValue(rng, &buf, &buf_size, C, C_SIZE);
#else
  for(k=0;k<ROUND;k++)
    chg[k]=C;
  /* Pick randomly K coefficients of the challenge to be striclty inferior to C */
  for(k=0;k<K;k++)
    {
      r=randomValue(rng, &buf, &buf_size, ROUND-k,sizeOfValue(ROUND-k));
      for (i=0;i<=r;i++)
	if (chg[i]<C)
	  r++;
      chg[r]=randomValue(rng, &buf, &buf_size, C, C_SIZE);
    }
#endif

  for (i=0;i<ROUND;i++)
    if (chg[i]<C)
      {
	chg_nc[k0]=i;
	chg_val[k0++]=chg[i];
      }
    else
      chg_c[k1++]=i;
}

/* Expand a seed to several identical atf */
void
expandATF_vec_copy(uint32_t *atf, const uint8_t *seed, const int nb_copies, const int vec_size, const size_t seed_size)
{
  #if SHAKE_FOR_EXPANDERS == 0

  uint32_t set[16];
  int i=0, r;
  int j=16;
  AES_XOF_struct rng[1];

  initExpander(rng, seed, seed_size);

  while (i<LEN){
    /* refresh values if necessary */
    if (j==16){
	    j=0;
      seedexpander(rng, (uint8_t*)set, 64);
	  }
    /* pick value and fill if acceptable */
    if (set[j]<PRIME){
      for (r = 0; r < nb_copies; r++)
        atf[(i*vec_size)+r] = set[j];
	    i++;
    }
    j++; /* next sampled value */
  }

  #else

  int i=0, j=0, r;
  /* arbitrary storage size */
  const size_t storage_size = NB32_VEC_NN;
  uint32_t storage[NB32_VEC_NN] __attribute__(( aligned(32)));
  hashArray((uint8_t*)storage, storage_size*sizeof(uint32_t), seed, seed_size);

  while (i < LEN)
  {
    /* refresh through rehash: dumb way but probabilistically we should not refresh even once in most cases */
    if (j==storage_size){
      hashArray((uint8_t*)storage, storage_size*sizeof(uint32_t), seed, seed_size);
      j = 0;
    }
    if (storage[j]<PRIME){
      for (r = 0; r < nb_copies; r++)
        atf[(i*vec_size)+r] = storage[j];
	    i++;
    }
    j++;
  }

  #endif
}


/* Expand a seed to n matrix columns inversible */
void expandColumns(uint32_t *col, const uint8_t *seed, const size_t seed_size)
{
  #if SHAKE_FOR_EXPANDERS == 0

  uint32_t set[16];
  int i=0;
  int j=0;
  int k=16;

  AES_XOF_struct rng[1];
  initExpander(rng, seed, seed_size);

  while (i<N)
    {
      if (k==16)
	{
	  k=0;
	  seedexpander(rng, (uint8_t*)set, 64);
	}
      col[i*N+j]=set[k++];
      if (col[i*N+j]<PRIME&&(i!=j||col[i*N+j]>0)&&(++j==N))
	{
	  j=0;
	  i++;
	}
    }

  #else

  int i=0, j=0, k=0;
  /* arbitrary storage size */
  const size_t storage_size = NB32_VEC_NN;
  uint32_t storage[NB32_VEC_NN] __attribute__(( aligned(32)));
  hashArray((uint8_t*)storage, storage_size*sizeof(uint32_t), seed, seed_size);

  while (i < N)
  {
    /* refresh through rehash: dumb way but probabilistically we should not refresh even once in most cases */
    if (k==storage_size){
      hashArray((uint8_t*)storage, storage_size*sizeof(uint32_t), (uint8_t*)storage, storage_size*sizeof(uint32_t));
      k = 0;
    }
    col[i*N+j]=storage[k++];
    if ( col[i*N+j]<PRIME && (i!=j||col[i*N+j]>0) && (++j==N) ){
	    j=0; i++;
	  }
  }

  #endif
}
