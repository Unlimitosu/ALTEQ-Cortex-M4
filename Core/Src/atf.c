#include "atf.h"
#include <stdio.h>
#include <string.h>

#include "vec_sizes.h"

static void compressATFS_32_to_32_vec(uint32_t *catf_out, const uint32_t *atf_in, const int nb_atf, const int vec_size)
{
  int index=0;
  int i,j,k,r;

  for(r=0; r<nb_atf; r++)
    for(i=0; i<N-2; i++)
      for(j=i+1; j<N-1; j++)
	      for(k=j+1; k<N; k++){
          catf_out[index]=reductionStrict(atf_in[(i*(N*N)+j*N+k)*vec_size+r]);
          index++;
        }
}

/* Assuming entries are already vectorized, just decompress per pack/vectors */
static void decompressATFS_32_to_32_vecpacks(uint32_t *datf_out, const uint32_t *atf_in, const int vec_size)
{
  int index=0;
  int i,j,k,r;

  for(i=0; i<N-2; i++)
    for(j=i+1; j<N-1; j++)
      for(k=j+1; k<N; k++){
	      for(r=0; r<vec_size; r++)
	        datf_out[(i*(N*N)+j*N+k)*vec_size+r]=atf_in[index+r];
	      index+=vec_size;
	    }
}

/* macros for multiplication, multiplication and add/sub, and modular reduction */
#define _MUL_(bid,x,id) for (r = 0; r < vec_size; r++) buf[(bid)*vec_size + r]=multiplicationModuloP(atf[(x)*vec_size + r],col[(id)*vec_size + r]);
#define _MADD_(bid,x,id) for (r = 0; r < vec_size; r++) buf[(bid)*vec_size + r]+=multiplicationModuloP(atf[(x)*vec_size + r],col[(id)*vec_size + r]);
#define _MSUB_(bid,x,id) for (r = 0; r < vec_size; r++) buf[(bid)*vec_size + r]+=multiplicationModuloP(atf[(x)*vec_size + r], PRIME - col[(id)*vec_size + r]);
#define _MOD_RED_(x, bid) for (r = 0; r < vec_size; r++) atf[(x)*vec_size + r]=reductionModuloP(buf[(bid)*vec_size + r]);

static void actingOnATFwColumn_vec32(uint32_t *atf, const uint32_t* col, const int j, const int vec_size, uint64_t* buf)
{
  int i,k,l,r;

  /* i=j */
  /*k<j<l*/
  for(k=0; k<j; k++)
    for(l=j+1; l<N; l++)
      _MUL_(k*N + l, k*(N*N)+j*N+l, j);

  /* i!=j */
  /*k<i<j<l*/ /* opn_3 */
  for(k=0;k<j-1;k++)
    for(l=j+1;l<N;l++)
      for(i=k+1;i<j;i++)
        _MADD_(k*N + l, k*(N*N)+i*N+l, i);

  /*i<k<j<l*/ /* opn_5 */
  for(k=1;k<j;k++)
    for(l=j+1;l<N;l++)
      for(i=0;i<k;i++)
        _MSUB_(k*N + l,  i*(N*N)+k*N+l, i );

  /*k<j<l<i*/ /* opn_8 */
  for(k=0;k<j;k++)
    for(l=j+1;l<N-1;l++)
      for(i=l+1;i<N;i++)
        _MSUB_(k*N + l, k*(N*N)+l*N+i, i);

  /*k<j<i<l*/ /* opn_9 */
  for(k=0;k<j;k++)
    for(l=j+2;l<N;l++)
      for(i=j+1;i<l;i++)
        _MADD_(k*N + l, k*(N*N)+i*N+l, i);

  for(k=0; k<j; k++)
    for(l=j+1; l<N; l++)
      _MOD_RED_(k*(N*N)+j*N+l, k*N + l);

  /* i=j  */
  /*j<k<l*/
  for(k=j+1; k<N-1; k++)
    for(l=k+1; l<N; l++)
      _MUL_(k*N+l, j*(N*N)+k*N+l, j);

  /* i!=j*/
  /*i<j<k<l*/ /* opn_6 */
  for(k=j+1;k<N-1;k++)
    for(l=k+1;l<N;l++)
      for(i=0;i<j;i++)
        _MADD_(k*N+l, i*(N*N)+k*N+l, i);

  /*j<k<l<i*/ /* opn_10 */
  for(k=j+1;k<N-2;k++)
    for(l=k+1;l<N-1;l++)
      for(i=l+1;i<N;i++)
        _MADD_(k*N+l, k*(N*N)+l*N+i, i);

  /*j<k<i<l*/ /* opn_11 */
  for(k=j+1;k<N-2;k++)
    for(l=k+2;l<N;l++)
      for(i=k+1;i<l;i++)
        _MSUB_(k*N+l, k*(N*N)+i*N+l, i);

  /*j<i<k<l*/ /* opn_12 */
  for(k=j+2;k<N-1;k++)
    for(l=k+1;l<N;l++)
      for(i=j+1;i<k;i++)
        _MADD_(k*N+l, i*(N*N)+k*N+l, i);

  /* The end... Reduction */
  for(k=j+1; k<N-1; k++)
    for(l=k+1; l<N; l++)
      _MOD_RED_(j*(N*N)+k*N+l, k*N+l);

  /* i=j */
  /*k<l<j*/
  for(k=0; k<j-1; k++)
    for(l=k+1; l<j; l++)
      _MUL_(k*N+l, k*(N*N)+l*N+j, j);

  /* i!=j */
  /*k<l<i<j*/ /* opn_1 */
  for(k=0;k<j-2;k++)
    for(l=k+1;l<j-1;l++)
      for(i=l+1;i<j;i++)
        _MADD_(k*N+l, k*(N*N)+l*N+i, i);

  /*k<i<l<j*/ /* opn_2 */
  for(k=0;k<j-2;k++)
    for(l=k+2;l<j;l++)
      for(i=k+1;i<l;i++)
        _MSUB_(k*N+l, k*(N*N)+i*N+l, i);

  /*i<k<l<j*/ /* opn_4 */
  for(k=1;k<j-1;k++)
    for(l=k+1;l<j;l++)
      for(i=0;i<k;i++)
        _MADD_(k*N+l, i*(N*N)+k*N+l, i );

  /*k<l<j<i*/ /* opn_7 */
  for(k=0;k<j-1;k++)
    for(l=k+1;l<j;l++)
      for(i=j+1;i<N;i++)
        _MADD_(k*N+l, k*(N*N)+l*N+i, i);

  /*RED*/
  for(k=0; k<j-1; k++)
    for(l=k+1; l<j; l++)
      _MOD_RED_(k*(N*N)+l*N+j, k*N+l);

}

#undef _MUL_
#undef _MADD_
#undef _MSUB_
#undef _MOD_RED_


/* this one assume every entry is already vectorized: including atf in entry */
void
invertingOnATF(uint32_t *atf_out, const uint32_t *atf_in, const uint32_t *columns){
  int i,j,r;

  uint32_t atf[N*N*N*NB32_VEC_C];
  uint32_t diagonal[N*NB32_VEC_C];
  uint32_t column[N*NB32_VEC_C];

  uint64_t buf[N*N*NB32_VEC_C];

  /* make copies of the same ATF */
  decompressATFS_32_to_32_vecpacks(atf, atf_in, NB32_VEC_C);

  for(i=0;i<N;i++)
    for (r = 0; r < NB32_VEC_C; r++)
      diagonal[i*NB32_VEC_C + r]=columns[i*(N+1)*NB32_VEC_C + r];

  setInversionModuloP_vec_C(diagonal);

  for(j=N-1;j>=0;j--){
    for(i=0;i<N;i++)
      if (i!=j)
        for (r = 0; r < NB32_VEC_C; r++)
          column[i*NB32_VEC_C + r]=reductionModuloP(multiplicationModuloP((PRIME-diagonal[j*NB32_VEC_C + r]),columns[(j*N+i)*NB32_VEC_C + r]));
    for (r = 0; r < NB32_VEC_C; r++)
      column[j*NB32_VEC_C + r]=diagonal[j*NB32_VEC_C + r];
    actingOnATFwColumn_vec32(atf, column, j, NB32_VEC_C, buf);
  }

  /* compress many to many: output is NON-VECTORIZED (important because of challenge selection within pk) */
  compressATFS_32_to_32_vec(atf_out, atf, C, NB32_VEC_C);

  /* free */

}

/* same but assume every entry is ALREADY vectorized */
/* NOTE: vec_size should be the lowest number of elements such that it is divisible by vectors and contains n_atf */
__attribute__((section(".sdram"))) uint32_t atfs_actingOnATFS[N*N*N*NB32_VEC_ROUND];
__attribute__((section(".sdram"))) uint64_t buf_actingOnATFS[N*N*NB32_VEC_ROUND];
void
actingOnATFS(uint32_t *atf_out, const uint32_t *atf_in, const uint32_t *columns)
{

  /* full 32 bits */
  int j;

  // is this necessary?????????????????
  for(int i = 0 ; i < N*N*N*NB32_VEC_ROUND; i++) atfs_actingOnATFS[i] = 0;

  for(int i = 0 ; i < N*N*NB32_VEC_ROUND; i++) buf_actingOnATFS[i] = 0;


  /* the entry is vectorized already thus we use vec_size */
  decompressATFS_32_to_32_vecpacks(atfs_actingOnATFS, atf_in, NB32_VEC_ROUND);
  for(j=0;j<N;j++)
    actingOnATFwColumn_vec32(atfs_actingOnATFS, columns+j*N*NB32_VEC_ROUND, j, NB32_VEC_ROUND, buf_actingOnATFS);
  /* the output IS NOT vectorized: this is used by the hash function using ordered challenges ! */
  compressATFS_32_to_32_vec(atf_out, atfs_actingOnATFS, ROUND, NB32_VEC_ROUND);

}
