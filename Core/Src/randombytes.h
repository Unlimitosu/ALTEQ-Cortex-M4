#ifndef PQCLEAN_RANDOMBYTES_H
#define PQCLEAN_RANDOMBYTES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifdef _WIN32
/* Load size_t on windows */
#include <crtdefs.h>
#else
#include <unistd.h>
#endif /* _WIN32 */

/*
 * Write `n` bytes of high quality random bytes to `buf`
 */
#define randombytes     PQCLEAN_randombytes
void randombytes(uint8_t* buf, size_t xlen);

#ifdef __cplusplus
}
#endif

#endif /* PQCLEAN_RANDOMBYTES_H */
