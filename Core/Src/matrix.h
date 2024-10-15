#ifndef MATRIX_H_
#define MATRIX_H_

#include "api.h"
#include "field.h"
#include <stdlib.h>

extern void columnsMatrix(uint32_t *mat, const uint32_t *colsA, const uint32_t *colsB, const int vec_size);
/* nb_mats <= vec_size, this is to avoid checking zeroes on useless positions when vec_size > nb_mats */
extern int columnsDecomposition(uint32_t *mat_to_transform, const int vec_size, const int nb_mats);

#endif
