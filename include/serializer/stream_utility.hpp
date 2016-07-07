#ifndef SERIALIZER_STREAM_UTILITY_HPP
#define SERIALIZER_STREAM_UTILITY_HPP

#include <ostream> // for std::basic_ostream
#include "bitstream.hpp"

namespace serializer {

template< class CharT, class Traits >
std::basic_ostream<CharT, Traits>& delimiter( std::basic_ostream<CharT, Traits>& os ) {
    return os << CharT(' ');
}

template< class CharT, class Traits >
std::basic_istream<CharT, Traits>& delimiter( std::basic_istream<CharT, Traits>& is ) {
    return is;
}

BitStream& delimiter(BitStream& bs); // implemented in bitstream.cpp

class align {
    size_t bits;
public:
    align(size_t bits): bits(bits) { }
};

template<class CharT, class Traits>
std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os, const align& aligner) {
    return os;
}

template<class CharT, class Traits>
std::basic_istream<CharT, Traits>& operator>>(std::basic_istream<CharT, Traits>& is, const align& aligner) {
    return is;
}

BitStream& operator<<(BitStream& bs, const align& aligner); // implemented in bitstream.cpp

} // namespace serializer

#endif // SERIALIZER_STREAM_UTILITY_HPP
