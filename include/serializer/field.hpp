#ifndef SERIALIZER_SIMPLE_FIELD_HPP
#define SERIALIZER_SIMPLE_FIELD_HPP

#include "utility.hpp"        // for TypeList
#include "stream_utility.hpp" // for delimiter

namespace serializer {

template<typename T>
struct Field {

    using Data = T;

    template<class OutputStream>
    static void serialize(OutputStream& out, const T& t) {
        out << t;
    }
    template<class InputStream>
    static bool deserialize(InputStream& in, T& t) {
        in >> t;
        return !in.rdstate();
    }
};

using IntegerField = Field<int>;
using SizeField    = Field<uint64_t>;
using CharField    = Field<char>;
using FloatField   = Field<float>;
using DoubleField  = Field<double>;

} // namespace serializer

#endif // SERIALIZER_SIMPLE_FIELD_HPP
