#ifndef api_h
#define api_h

#include <stdint.h>

#define LAMBDA  128
#define N  13
#define LOG_Q  32
#define PRIME  4294967291lu
#define C  7
#define K  22
#define ROUND 84
/*///////////////////////////////////////////////////////////////////////
//////////////////////////// OTHER PARAMETER ////////////////////////////
///////////////////////////////////////////////////////////////////////*/

#define LEN   (N*(N-1)*(N-2)/6)
#define ALT_SIZE (LEN*LOG_Q/8)
#define MAT_SIZE (N*N*LOG_Q/8)

/* switch to 0 or 1 whether you want to use SALT + INDEX */
#define USE_SALT 1
/* #define USE_SALT 0 */

/* IMPORTANT NOTE: WE ASSUME r < 256 for SALT + INDEX */
#if (USE_SALT == 1) && (ROUND > 256)
#error "ROUND > 256: needs more than 8-bits for the index, implementation not ready :D"
#elif USE_SALT == 1
/* The MEDS way */
/* #define SALT_SIZE (32) */
/* The LESS way */
#define SALT_SIZE (LAMBDA/4)
#else
/* set SALT_SIZE to 0 if no SALT */
#define SALT_SIZE (0)
#endif

/* Following the Beullens' attack, we set flexible SEED_SIZE */
/* keep those values as legacy, number of chars to get lambda-bits and 2lambda-bits */
/* #define SEED_SIZE (LAMBDA/8) */
#define MSG_HASH_SIZE (LAMBDA/4)
/* might as well use this opportunity to make any challenge 256-bits? */
/* #define CHLG_SIZE (32) */
#define CHLG_SIZE (LAMBDA/4)
/* Currently just keep everything as 256-bits so we can just keep using AES-256 */
/* might need to switch to SHAKE for security level 5 (need more than 256-bits?) */
/*
#define PK_SEED_SIZE (32)
#define SK_SEED_SIZE (32)
#define MAT_SK_SEED_SIZE (32)
#define SIG_SEED_SIZE (32)
*/
#define PK_SEED_SIZE (LAMBDA/4)
#define SK_SEED_SIZE (LAMBDA/4)
#define MAT_SK_SEED_SIZE (LAMBDA/4)

#if USE_SALT == 0
#define SIG_SEED_SIZE (LAMBDA/4)
#define EXPCOL_SIG_SEED_SIZE (SIG_SEED_SIZE)
#else
#define SIG_SEED_SIZE (LAMBDA/8)
/* adds the SALT_SIZE and one octet for 256 positions */
#define EXPCOL_SIG_SEED_SIZE (SIG_SEED_SIZE + SALT_SIZE + 1)
#endif

/* AES-256 is kinda limited, so switch to Keccak if needed */
/* Note that this only affects: expandSeeds, expandColumns, expandATF_vec_copy */
/* expandChallenge should not go beyond 256-bits, as it is not seed-size dependent */
#if (PK_SEED_SIZE > 32) || (SK_SEED_SIZE > 32) || (MAT_SK_SEED_SIZE > 32) || (EXPCOL_SIG_SEED_SIZE > 32) || (USE_SALT == 1)
#define SHAKE_FOR_EXPANDERS 1
#else
#define SHAKE_FOR_EXPANDERS 0
#endif

/*///////////////////////////////////////////////////////////////////////
//////////////////////////// NIST PARAMETER //////////////////////////////
///////////////////////////////////////////////////////////////////////*/

int crypto_sign_keypair(uint8_t *pk, uint8_t *sk);
int crypto_sign(uint8_t *sm, uint64_t *smlen, const uint8_t *m, uint64_t mlen, const uint8_t *sk);
int crypto_sign_open(uint8_t *m, uint64_t *mlen, const uint8_t *sm, uint64_t smlen, const uint8_t *pk);

/* Following Beullens' attack we change the values here */
#define CRYPTO_ALGNAME "ALTEQ"
/* Legacy sizes*/
/*
#define CRYPTO_SECRETKEYBYTES (SEED_SIZE)
#define CRYPTO_PUBLICKEYBYTES (C*ALT_SIZE+SEED_SIZE)
#define CRYPTO_BYTES ((ROUND-K+2)*SEED_SIZE+K*MAT_SIZE)
*/
#define CRYPTO_SECRETKEYBYTES SK_SEED_SIZE
#define CRYPTO_PUBLICKEYBYTES (C*ALT_SIZE + PK_SEED_SIZE)
#define CRYPTO_BYTES (CHLG_SIZE + SALT_SIZE + ((ROUND-K)*SIG_SEED_SIZE) + K*MAT_SIZE)

#endif

