#ifndef FIELD_H_
#define FIELD_H_

#include "api.h"

extern uint64_t multiplicationModuloP(const uint32_t a, const uint32_t b);

extern uint32_t reductionModuloP(const uint64_t a);

extern uint32_t reductionStrict(const uint64_t a);

extern uint32_t inversionModuloP(const uint32_t a);

/* Modular inversion of a set of C fields element */
void setInversionModuloP_vec_C(uint32_t *set);

#endif
