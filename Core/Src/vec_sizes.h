#ifndef _AVX2_ALTEQ_VEC_SIZES_H_
#define _AVX2_ALTEQ_VEC_SIZES_H_
#include "api.h"
#include <stdint.h>

/************************************************************************************************************
THE GOAL OF THIS FILE IS TO PREDETERMINE IN ADVANCE THE NUMBER OF VECTORS WE NEED TO CREATE FOR VECTORIZATION
************************************************************************************************************/

/* turns on/off the vec mode basically */
#ifndef VEC_MODE
#define VEC_MODE 1
#endif

/* number of 32-bits we can put into a vector of size VEC_SIZE */
/* AVX2 uses vectors of 256-bits */
#if VEC_MODE == 1
#define VEC_SIZE 256
#define VEC32_SIZE (VEC_SIZE/32)
#else
/* no vectorization */
#define VEC32_SIZE (1)
#endif

/************************************************************************************************************
GENERIC
************************************************************************************************************/

#define VEC32_LEN ((LEN + VEC32_SIZE - 1) / VEC32_SIZE)

#define VEC32_NN (((N*N) + VEC32_SIZE - 1) / VEC32_SIZE)

/************************************************************************************************************
SETUP
************************************************************************************************************/

/* the setup needs C consecutive operations */
#define VEC32_C ((C + VEC32_SIZE - 1) / VEC32_SIZE)

/************************************************************************************************************
SIGNATURE
************************************************************************************************************/

/* the signature needs ROUND expansion/generation/actionOnATFS */
#define VEC32_ROUND ((ROUND + VEC32_SIZE - 1) / VEC32_SIZE)

/* the signature needs K columns matrix to compute */
#define VEC32_K ((K + VEC32_SIZE - 1) / VEC32_SIZE)


/************************************************************************************************************
VERIFICATION
************************************************************************************************************/

/* the signature needs ROUND-K actionOnATFS */
#define VEC32_ROUNDmK (((ROUND - K) + VEC32_SIZE - 1) / VEC32_SIZE)

/************************************************************************************************************
VALUES TREATED CONSECUTIVELY (CONTAINS OVERHEAD)
************************************************************************************************************/

#define NB32_VEC_LEN (VEC32_LEN*VEC32_SIZE)
#define NB32_VEC_NN (VEC32_NN*VEC32_SIZE)
#define NB32_VEC_C (VEC32_C*VEC32_SIZE)
#define NB32_VEC_ROUND (VEC32_ROUND*VEC32_SIZE)
#define NB32_VEC_K (VEC32_K*VEC32_SIZE)
#define NB32_VEC_ROUNDmK (VEC32_ROUNDmK*VEC32_SIZE)

#endif /* VEC_MODE */