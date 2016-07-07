#ifndef SERIALIZER_ARRAY_FIELD_HPP
#define SERIALIZER_ARRAY_FIELD_HPP

#include <iterator>    // for std::distance
#include <vector>      // for std::vector
#include "utility.hpp" // for TypeList
#include "stream_utility.hpp" // for align
#include "field.hpp"   // for SizeField

namespace serializer {

template<typename Field>
class ArrayField {
    using T = typename Field::Data;
public:
    using Data = std::vector<T>;
    template<class OutputStream>
    static void serialize(OutputStream& out, uint64_t n, const T* data) {
        out << align(8 * sizeof(T));
        SizeField::serialize(out, n);
        out << delimiter;
        for(uint64_t i = 0; i < n; ++i) {
            Field::serialize(out, data[i]);
            if(i < n - 1) out << delimiter;
        }
    }
    template<class OutputStream>
    static void serialize(OutputStream& out, const std::vector<T>& vector) {
        serialize(out, vector.size(), vector.data());
    }
    template<class OutputStream, class Iterator>
    static auto serialize(OutputStream& out, Iterator begin, Iterator end)
        -> decltype((std::distance(begin, end), Field::serialize(out, *begin++), begin != end, void())) {
        out << align(8 * sizeof(T));
        uint64_t i = 0, n = std::distance(begin, end);
        SizeField::serialize(out, n);
        out << delimiter;
        while(begin != end) {
            Field::serialize(out, *begin++);
            if(i < n - 1) out << delimiter;
        }
    }
    template<class InputStream>
    static bool deserialize(InputStream& in, uint64_t& n, T*& data) {
        in >> align(8 * sizeof(T));
        if(in.rdstate() || !SizeField::deserialize(in, n)) return false;
        bool result = true;
        data = new T[n];
        for(uint64_t i = 0; i < n && result; ++i) {
            result = result && Field::deserialize(in, data[i]);
            if(i < n - 1) {
                in >> delimiter;
                result = result && !in.rdstate();
            }
        }
        if(!result) {
            delete[] data;
            data = nullptr;
        }
        return result;
    }
    template<class InputStream>
    static bool deserialize(InputStream& in, std::vector<T>& vector) {
        in >> align(8 * sizeof(T));
        uint64_t n;
        if(in.rdstate() || !SizeField::deserialize(in, n)) return false;
        vector.clear();
        vector.reserve(n);
        bool result = true;
        T tmp;
        for(size_t i = 0; i < n && result; ++i) {
            result = result && Field::deserialize(in, tmp);
            if(i < n - 1) {
                in >> delimiter;
                result = result && !in.rdstate();
            }
            if(result) vector.push_back(tmp);
        }
        return result;
    }
};

template<typename T> using SimpleArrayField = ArrayField<Field<T>>;
/*template<typename T>
class SimpleArrayField {
    using Field = Field<T>;
public:
    using Data = std::vector<T>;
    template<class OutputStream>
    static void serialize(OutputStream& out, uint64_t n, const T* data) {
        out << align(8 * sizeof(T));
        SizeField::serialize(out, n);
        out << delimiter;
        for(uint64_t i = 0; i < n; ++i) {
            Field::serialize(out, data[i]);
            if(i < n - 1) out << delimiter;
        }
    }
    template<class OutputStream>
    static void serialize(OutputStream& out, const std::vector<T>& vector) {
        serialize(out, vector.size(), vector.data());
    }
    template<class InputStream>
    static bool deserialize(InputStream& in, uint64_t& n, T*& data) {
        in >> align(8 * sizeof(T));
        if(in.rdstate() || !SizeField::deserialize(in, n)) return false;
        bool result = true;
        data = new T[n];
        for(uint64_t i = 0; i < n && result; ++i) {
            result = result && Field::deserialize(in, data[i]);
            if(i < n - 1) {
                in >> delimiter;
                result = result && !in.rdstate();
            }
        }
        if(!result) {
            delete[] data;
            data = nullptr;
        }
        return result;
    }
    template<class InputStream>
    static bool deserialize(InputStream& in, std::vector<T>& vector) {
        in >> align(8 * sizeof(T));
        uint64_t n;
        if(in.rdstate() || !SizeField::deserialize(in, n)) return false;
        vector.clear();
        vector.reserve(n);
        bool result = true;
        T tmp;
        for(size_t i = 0; i < n && result; ++i) {
            result = result && Field::deserialize(in, tmp);
            if(i < n - 1) {
                in >> delimiter;
                result = result && !in.rdstate();
            }
            if(result) vector.push_back(tmp);
        }
        return result;
    }
};*/

} // namespace serializer

#endif // SERIALIZER_ARRAY_FIELD_HPP
