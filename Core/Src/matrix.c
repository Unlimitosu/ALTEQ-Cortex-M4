#include "matrix.h"
#include <string.h>

static void
columnMul_32_vec(uint32_t *mat, const uint32_t *column, const int j, const int s, const int vec_size)
{
  int i,k, r;

  for(i=0;i<N;i++)
    if (i!=j)
      for(k=s;k<N;k++)
        for(r=0; r<vec_size; r++)
	        mat[(i*N+k)*vec_size+r]=reductionModuloP(((uint64_t)mat[(i*N+k)*vec_size+r])+multiplicationModuloP(mat[(j*N+k)*vec_size+r],column[(i)*vec_size+r]));
  for(k=s;k<N;k++)
    for(r=0; r<vec_size; r++)
      mat[(j*N+k)*vec_size+r]=reductionModuloP(multiplicationModuloP(mat[(j*N+k)*vec_size+r],column[(j)*vec_size+r]));

}

static void
columnInv_32_vec(uint32_t *col, int j, const int vec_size)
{
  int i,r;
  for(r=0; r<vec_size; r++)
    col[(j)*vec_size+r]=inversionModuloP(col[(j)*vec_size+r]);
  for(i=0;i<N;i++)
    if (i!=j)
      for(r=0; r<vec_size; r++)
        col[(i)*vec_size+r]=reductionModuloP(multiplicationModuloP((PRIME-col[(j)*vec_size+r]),col[(i)*vec_size+r]));
}

void
columnsMatrix(uint32_t *mat, const uint32_t *colsA, const uint32_t *colsB, const int vec_size)
{
  int i,j,r;

  /* Transposition */
  for(i=0;i<N;i++)
    for(j=0;j<N;j++)
      for(r=0; r<vec_size; r++)
        mat[(i*N+j)*vec_size + r]=colsA[(j*N+i)*vec_size + r];

  /* operation */
  for(j=N-1;j>=0;j--)
    columnMul_32_vec(mat, &(colsA[j*N*vec_size]), j, j+1, vec_size);
  for(j=N-1;j>=0;j--)
    columnMul_32_vec(mat, &(colsB[j*N*vec_size]), j, 0  , vec_size);
}

int
columnsDecomposition(uint32_t *mat_to_transform, const int vec_size, const int nb_mats)
{
  uint32_t vec_mat0[N*N*vec_size];
  uint32_t vec_cols[N*vec_size];

  /* this will hold the return value */
  uint32_t vec_res[vec_size];

  int i,j, r;

  memset(vec_res, 1, sizeof(uint32_t) * vec_size);

  /* copy */
  memcpy(vec_mat0, mat_to_transform, sizeof(uint32_t) * N*N*vec_size);

  /* transposition/copy operation */
  for(i=0;i<N;i++)
    memcpy(mat_to_transform+i*vec_size, vec_mat0+i*N*vec_size, sizeof(uint32_t) * vec_size);
  /* pure copy operation */
  memcpy(vec_cols, mat_to_transform, sizeof(uint32_t) * N * vec_size);
  /* verification operation */
  for(r=0; r<K; r++)
    vec_res[r] &= !(vec_cols[r]==0||vec_cols[r]==PRIME);

  for(j=1;j<N;j++)
  {
    /* actual operations */
    columnInv_32_vec(vec_cols, j-1, vec_size);
    columnMul_32_vec(vec_mat0, vec_cols, j-1, j, vec_size);

    /* transposition/copy operation */
    for(i=0;i<N;i++)
      memcpy(mat_to_transform+(j*N+i)*vec_size, vec_mat0+(i*N+j)*vec_size, sizeof(uint32_t) * vec_size);
    /* pure copy operation */
    memcpy(vec_cols, mat_to_transform+j*N*vec_size, sizeof(uint32_t) * N * vec_size);
    /* verification operation */
    for(r=0; r<K; r++)
      vec_res[r] &= !(vec_cols[(j)*vec_size+r]==0||vec_cols[(j)*vec_size+r]==PRIME);
  }

  i = 1; /* this will hold the return value */
  for(r=0; r<nb_mats; r++)
    i &= vec_res[r];

  return i;

}
