// special thx to http://blog.regehr.org/archives/1063
#include <stdint.h>   // for uint32_t
#include <limits.h>   // for CHAR_BIT

#ifdef __cplusplus
extern "C" {
#endif

uint32_t rotl32 (uint32_t n, unsigned int c)
{
  const unsigned int mask = (CHAR_BIT*sizeof(n)-1);
  c = c % 32;
  c &= mask;  // avoid undef behaviour with NDEBUG.  0 overhead for most types / compilers
  return (n<<c) | (n>>( (-c)&mask ));
}

uint32_t rotr32 (uint32_t n, unsigned int c)
{
  const unsigned int mask = (CHAR_BIT*sizeof(n)-1);
  c = c % 32;
  c &= mask;  // avoid undef behaviour with NDEBUG.  0 overhead for most types / compilers
  return (n>>c) | (n<<( (-c)&mask ));
}

uint64_t rotl64 (uint64_t n, unsigned int c)
{
  const unsigned int mask = (CHAR_BIT*sizeof(n)-1);
  c = c % 64;
  c &= mask;  // avoid undef behaviour with NDEBUG.  0 overhead for most types / compilers
  return (n<<c) | (n>>( (-c)&mask ));
}

uint64_t rotr64 (uint64_t n, unsigned int c)
{
  const unsigned int mask = (CHAR_BIT*sizeof(n)-1);
  c = c % 64;
  c &= mask;  // avoid undef behaviour with NDEBUG.  0 overhead for most types / compilers
  return (n>>c) | (n<<( (-c)&mask ));
}

#ifdef __cplusplus
} // extern "C"
#endif
