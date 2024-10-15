#include "field.h"
#include "vec_sizes.h"

/* Modular multiplication 32 by 32 to 33bits, using 32-bits entries */
uint64_t
multiplicationModuloP(const uint32_t a, const uint32_t b)
{
  uint64_t r=a*(uint64_t)b;
  r=r-PRIME*(r>>LOG_Q);
  return r;
}

/* update a, given b and c to a <- (a + b*c) mod P, with incomplete reduction into 64-bit buffer */
void
addmul_P32_into_64(uint64_t *a, const uint32_t b, const uint32_t c){
  uint64_t r=b;
  r *= c;
  *a += r;
  /* forces reduction to 33-bits ? */
  *a -= PRIME*((*a)>>LOG_Q);
}

/* Modular reduction 61 to 32bits */
uint32_t
reductionModuloP(const uint64_t a)
{
  uint64_t r=a;
  r=r-PRIME*(r>>LOG_Q);
  r=r-PRIME*(r>>LOG_Q);
  return (uint32_t)r;
}

/* Modular reduction 32 to [0,P[ */
uint32_t reductionStrict(const uint64_t a)
{
  uint64_t r=a;
  if (r>=PRIME)
    return (r-PRIME);
  return (uint32_t)r;
}

/* Modular inversion of a field element */
uint32_t inversionModuloP(const uint32_t a)
{
  /* Takagi's algorithm (as advised by J.C Bajard) */
  uint32_t b0,b1,b2,b3;
  int i;
  b0=reductionModuloP(multiplicationModuloP(a,a));
  b0=reductionModuloP(multiplicationModuloP(b0,a));

  b1=reductionModuloP(multiplicationModuloP(b0,b0));
  b1=reductionModuloP(multiplicationModuloP(b1,b1));
  b1=reductionModuloP(multiplicationModuloP(b1,b0));

  b2=b1;
  for(i=0;i<4;i++)
    b2=reductionModuloP(multiplicationModuloP(b2,b2));
  b2=reductionModuloP(multiplicationModuloP(b2,b1));
  b3=b2;

  for(i=0;i<8;i++)
    b3=reductionModuloP(multiplicationModuloP(b3,b3));
  b3=reductionModuloP(multiplicationModuloP(b3,b2));

  for(i=0;i<8;i++)
    b3=reductionModuloP(multiplicationModuloP(b3,b3));
  b3=reductionModuloP(multiplicationModuloP(b3,b2));

  for(i=0;i<4;i++)
    b3=reductionModuloP(multiplicationModuloP(b3,b3));
  b3=reductionModuloP(multiplicationModuloP(b3,b1));

  b3=reductionModuloP(multiplicationModuloP(b3,b3));
  b3=reductionModuloP(multiplicationModuloP(b3,a));

  b3=reductionModuloP(multiplicationModuloP(b3,b3));
  b3=reductionModuloP(multiplicationModuloP(b3,b3));
  b3=reductionModuloP(multiplicationModuloP(b3,b3));
  b3=reductionModuloP(multiplicationModuloP(b3,a));
  return (b3);
}

/* Modular inversion of a set of C fields element */
void setInversionModuloP_vec_C(uint32_t *set)
{
  int i,r;
  uint32_t mul[N*NB32_VEC_C] __attribute__ ((aligned (64)));
  uint32_t inv0[NB32_VEC_C] __attribute__ ((aligned (64)));

  for (r = 0; r < NB32_VEC_C; r++)
    mul[r]=set[r];

  for (i=1;i<N;i++)
    for (r = 0; r < NB32_VEC_C; r++)
      mul[i*NB32_VEC_C+ r]=reductionModuloP(multiplicationModuloP(mul[(i-1)*NB32_VEC_C+ r], set[i*NB32_VEC_C+ r]));

  for (r = 0; r < NB32_VEC_C; r++)
    inv0[r]=inversionModuloP(mul[(N-1)*NB32_VEC_C + r]);

  for (i=N-1;i>0;i--)
    for (r = 0; r < NB32_VEC_C; r++)
      {
        mul[i*NB32_VEC_C+r]=reductionModuloP(multiplicationModuloP(mul[(i-1)*NB32_VEC_C+r], inv0[r]));
        inv0[r]=reductionModuloP(multiplicationModuloP(inv0[r], set[i*NB32_VEC_C+r]));
        set[i*NB32_VEC_C+r]=mul[i*NB32_VEC_C+r];
      }

  for (r = 0; r < NB32_VEC_C; r++)
    set[r]=inv0[r];
}