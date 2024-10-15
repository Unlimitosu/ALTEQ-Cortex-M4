#ifndef ATF_H_
#define ATF_H_

#include "api.h"
#include "field.h"
#include <stdlib.h>

extern void invertingOnATF(uint32_t *atf_out, const uint32_t *atf_in, const uint32_t *columns);
extern void actingOnATFS(uint32_t *atf_out, const uint32_t *atf_in, const uint32_t *columns);

#endif

