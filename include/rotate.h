#include <stdint.h>   // for uint32_t

#ifdef __cplusplus
extern "C" {
#endif

uint32_t rotl32 (uint32_t n, unsigned int c);
uint32_t rotr32 (uint32_t n, unsigned int c);
uint64_t rotl64 (uint64_t n, unsigned int c);
uint64_t rotr64 (uint64_t n, unsigned int c);

#ifdef __cplusplus
} // extern "C"
#endif
