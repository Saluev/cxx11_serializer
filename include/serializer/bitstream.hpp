#ifndef SERIALIZER_BITSTREAM_HPP
#define SERIALIZER_BITSTREAM_HPP

#include <climits> // for CHAR_BIT
#include <cstdint> // for uint64_t
#include <type_traits> // for std::enable_if, std::is_integral
#include "utility.hpp" // for swap_endian

namespace serializer {

using byte = uint8_t;

class BitStream {
    byte* bdata;
    uint64_t position, length, allocated;
    int mode;  // 0 = read, other = write
    int state; // 0 = OK
    void reallocate(uint64_t);
public:
    static const int MODE_READ  = 0;
    static const int MODE_WRITE = 1;
    inline int get_mode(void) const noexcept { return mode; }
    BitStream(void);
    BitStream(void*, uint64_t);
    ~BitStream(void);
    int rdstate(void) const;
    BitStream& seekp(uint64_t p);
    void write_bits(uint8_t how_much, uint64_t bits);
    uint64_t read_bits(uint8_t how_much);
    void* data(void);
    BitStream& operator<<(BitStream&(*func)(BitStream&));
    BitStream& operator>>(BitStream&(*func)(BitStream&));
};

template<typename Integer>
typename std::enable_if<std::is_integral<Integer>::value, BitStream&>::type
operator<<(BitStream& out, const Integer& arg) {
    const size_t num_bits = sizeof(Integer) * 8;
    #ifdef BIG_ENDIAN
    uint64_t long_arg = static_cast<uint64_t>(swap_endian<Integer>(arg));
    #else  // ifdef BIG_ENDIAN
    uint64_t long_arg = static_cast<uint64_t>(                     arg );
    #endif // ifdef BIG_ENDIAN
    out.write_bits(num_bits, long_arg);
    return out;
}

template<typename Integer>
typename std::enable_if<std::is_integral<Integer>::value, BitStream&>::type
operator>>(BitStream& in, Integer& arg) {
    const size_t num_bits = sizeof(Integer) * 8;
    uint64_t long_arg = in.read_bits(num_bits);
    #ifdef BIG_ENDIAN
    arg = swap_endian<Integer>(static_cast<Integer>(long_arg));
    #else  // ifdef BIG_ENDIAN
    arg =                      static_cast<Integer>(long_arg) ;
    #endif // ifdef BIG_ENDIAN
    return in;
}

} // namespace serializer

#endif // SERIALIZER_BITSTREAM_HPP
