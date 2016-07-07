#ifndef SERIALIZER_BETTER_STATIC_SCHEMA_HPP
#define SERIALIZER_BETTER_STATIC_SCHEMA_HPP

#include <type_traits> // for std::enable_if, std::is_same
#include <utility>     // for std::declval
#include "../rotate.h" // for rotl64
#include "field.hpp"   // for SizeField

namespace serializer {

template<typename... Fields>
class Schema;

class HashField {}; // dummy field class

template<typename F, typename L> class can_serialize;
template<typename F, typename... Ts>
class can_serialize<F, TypeList<Ts...>>
{
    template <typename U> static char func(typename std::enable_if<std::is_same<decltype(U::serialize(std::declval<Ts>()...)), void>::value>::type*);
    template <typename U> static long func(...);
  public:
    using type = can_serialize;
    static const int value = ( sizeof(func<F>(0)) == sizeof(char) );
};

template<typename F, typename L> class can_deserialize;
template<typename F, typename... Ts>
class can_deserialize<F, TypeList<Ts...>>
{
    template <typename U> static char func(typename std::enable_if<std::is_same<decltype(U::deserialize(std::declval<Ts>()...)), bool>::value>::type*);
    template <typename U> static long func(...);
  public:
    using type = can_deserialize;
    static const int value = ( sizeof(func<F>(0)) == sizeof(char) );
};

/* serializer */

template<typename F, typename NextSerializer, typename OS, bool has_serialize, typename TL>
struct SchemaSerializer;

template<typename F, typename NextSerializer, typename OS>
struct SchemaSerializer<F, NextSerializer, OS, false, EmptyList> {
    // stop recursion! nothing to do!
    template<typename... TailTypes> static void serialize(void); // to clarify `using` directive below
};

template<typename F, typename NextSerializer, typename OS>
struct SchemaSerializer<F, NextSerializer, OS, true, EmptyList> {
    // stop recursion!
    template<typename... TailTypes>
    static auto serialize(OS& out, TailTypes&&... targs)
        -> decltype(NextSerializer::serialize(out, std::forward<TailTypes>(targs)...))
    {
        F ::serialize(out);
        out << delimiter;
        NextSerializer::serialize(out, std::forward<TailTypes>(targs)...);
    }
};

template<typename F, typename NextSerializer, typename OS, typename... Types>
struct SchemaSerializer<F, NextSerializer, OS, false, TypeList<Types...>>:
    public SchemaSerializer<
        F,
        NextSerializer,
        OS,
        can_serialize<F, typename Head<TypeList<OS, Types...>>::Result>::value,
        typename Head<TypeList<Types...>>::Result
    > {
    // nothing to do!
};

template<typename F, typename NextSerializer, typename OS, typename... Types>
struct SchemaSerializerImplementation {
    template<typename... TailTypes>
    static auto serialize(OS& out, Types... args, TailTypes&&... targs)
        -> decltype(NextSerializer::serialize(out, std::forward<TailTypes>(targs)...))
    {
        F::serialize(out, std::forward<Types>( args)...); // TODO do I really need std::forward here?
        out << delimiter;
        NextSerializer::serialize(out, std::forward<TailTypes>(targs)...);
    }
};

template<typename F, typename NextSerializer, typename OS, typename... Types>
struct SchemaSerializer<F, NextSerializer, OS, true, TypeList<Types...>>:
    public SchemaSerializer<
        F,
        NextSerializer,
        OS,
        can_serialize<F, typename Head<TypeList<OS, Types...>>::Result>::value,
        typename Head<TypeList<Types...>>::Result
    >, public SchemaSerializerImplementation<F, NextSerializer, OS, Types...> {
    using SchemaSerializer<
        F,
        NextSerializer,
        OS,
        can_serialize<F, typename Head<TypeList<OS, Types...>>::Result>::value,
        typename Head<TypeList<Types...>>::Result
    >::serialize; // TODO */
    using SchemaSerializerImplementation<F, NextSerializer, OS, Types...>::serialize;
};

/* deserializer */

template<typename F, typename NextDeserializer, typename IS, bool has_deserialize, typename TL>
struct SchemaDeserializer;

template<typename F, typename NextDeserializer, typename IS>
struct SchemaDeserializer<F, NextDeserializer, IS, false, EmptyList> {
    // stop recursion! nothing to do!
    template<typename... TailTypes> static bool deserialize(void); // to clarify `using` directive below
};

template<typename F, typename NextDeserializer, typename IS>
struct SchemaDeserializer<F, NextDeserializer, IS, true, EmptyList> {
    // stop recursion!
    template<typename... TailTypes>
    static auto deserialize(IS& in, TailTypes&&... targs) ->
        typename std::enable_if<sizeof...(TailTypes) == -1, decltype(NextDeserializer::deserialize(in, std::forward<TailTypes>(targs)...) )>::type
    {
        if(!F::deserialize(in)) return false;
        in >> delimiter;
        return !in.rdstate()
        && NextDeserializer::deserialize(in, std::forward<TailTypes>(targs)...);
    }
};

template<typename F, typename NextDeserializer, typename IS, typename... Types>
struct SchemaDeserializer<F, NextDeserializer, IS, false, TypeList<Types...>>:
    public SchemaDeserializer<
        F,
        NextDeserializer,
        IS,
        can_deserialize<F, typename Head<TypeList<IS, Types...>>::Result>::value,
        typename Head<TypeList<Types...>>::Result
    > {
    // nothing to do!
};

template<typename F, typename NextDeserializer, typename IS, typename... Types>
struct SchemaDeserializerImplementation {
    template<typename... TailTypes>
    static auto deserialize(IS& in, Types... args, TailTypes&&... targs) -> decltype(NextDeserializer::deserialize(in, std::forward<TailTypes>(targs)...))
    {
        if(!F::deserialize(in, std::forward<Types>( args)...)) return false; // TODO do I really need std::forward here?
        in >> delimiter;
        return !in.rdstate()
        && NextDeserializer::deserialize(in, std::forward<TailTypes>(targs)...);
    }
};

template<typename F, typename NextDeserializer, typename IS, typename... Types>
struct SchemaDeserializer<F, NextDeserializer, IS, true, TypeList<Types...>>:
    public SchemaDeserializer<
        F,
        NextDeserializer,
        IS,
        can_deserialize<F, typename Head<TypeList<IS, Types...>>::Result>::value,
        typename Head<TypeList<Types...>>::Result
    >, public SchemaDeserializerImplementation<F, NextDeserializer, IS, Types...> {
    using SchemaDeserializer<
        F,
        NextDeserializer,
        IS,
        can_deserialize<F, typename Head<TypeList<IS, Types...>>::Result>::value,
        typename Head<TypeList<Types...>>::Result
    >::deserialize; // TODO */
    using SchemaDeserializerImplementation<F, NextDeserializer, IS, Types...>::deserialize;
};

/* schema */

template<>
struct Schema<> {
    template<typename OutputStream>
    static void serialize(OutputStream&) {
        // nothing to do!
    }
    template<typename InputStream>
    static bool deserialize(InputStream&) {
        return true; // nothing to do!
    }
};

// this specialization is mainly needed to avoid adding delimiter after all fields.
template<typename F>
struct Schema<F> {
    template<typename OutputStream, typename... Types>
    static auto serialize(OutputStream& out, Types&&... args) -> decltype(F::serialize(out, std::forward<Types>(args)...)) {
        F::serialize(out, std::forward<Types>(args)...);
    }
    template<typename InputStream, typename... Types>
    static auto deserialize(InputStream& in, Types&&... args) -> decltype(F::deserialize(in, std::forward<Types>(args)...)) {
        return F::deserialize(in, std::forward<Types>(args)...);
    }
};

template<typename... Fields>
struct Schema<HashField, Fields...> {
    template<typename OutputStream, typename... Types>
    static auto serialize(OutputStream& out, Types&&... args) -> decltype(Schema<Fields...>::serialize(out, std::forward<Types>(args)...))
    {
        SizeField::serialize(out, std::hash<Schema<Fields...>>()(Schema<Fields...>()));
        out << delimiter;
        Schema<Fields...>::serialize(out, std::forward<Types>(args)...);
    }
    template<typename InputStream, typename... Types>
    static auto deserialize(InputStream& in, Types&&... args) -> decltype(Schema<Fields...>::deserialize(in, std::forward<Types>(args)...))
    {
        uint64_t hash;
        return SizeField::deserialize(in, hash)
        && hash == std::hash<Schema<Fields...>>()(Schema<Fields...>())
        && Schema<Fields...>::deserialize(in, std::forward<Types>(args)...);
    }
};

template<typename F, typename... Fields>
class Schema<F, Fields...> {
    template<class OutputStream, typename... Types>
    using Serializer = SchemaSerializer<
        F,                       // current field to serialize
        Schema<Fields...>, // recursion tail schema
        OutputStream&,           // output stream type
        can_serialize<F, TypeList<OutputStream&, Types...>>::value, // does F possess necessary implementation
        TypeList<Types...>       // arguments' types
    >;
    template<class InputStream, typename... Types>
    using Deserializer = SchemaDeserializer<
        F,                       // current field to serialize
        Schema<Fields...>, // recursion tail schema
        InputStream&,            // output stream type
        can_deserialize<F, TypeList<InputStream&, Types...>>::value, // does F possess necessary implementation
        TypeList<Types...>       // arguments' types
    >;

public:
    template<typename OutputStream, typename... Types>
    static auto serialize(OutputStream& out, Types&&... args) -> decltype(Serializer<OutputStream, Types...>::serialize(out, std::forward<Types>(args)...) )
    {
        Serializer<OutputStream, Types...>::serialize(out, std::forward<Types>(args)...);
    }
    template<typename InputStream, typename... Types>
    static auto deserialize(InputStream& in, Types&&... args) -> decltype(Deserializer<InputStream, Types...>::deserialize(in, std::forward<Types>(args)...) )
    {
        return Deserializer<InputStream, Types...>::deserialize(in, std::forward<Types>(args)...);
    }

};

} // namespace serializer

namespace std {

template<>
struct hash<serializer::Schema<> > {
    uint64_t operator() (const serializer::Schema<>&) const noexcept {
        return 0;
    }
};

template<typename T, typename... Types>
struct hash<serializer::Schema<T, Types...> > {
    uint64_t operator() (const serializer::Schema<T, Types...>&) const noexcept {
        using TailSchema  =  serializer::Schema<Types...>;
        // primitive hashing: sum of hash codes of types modulo 2 with bit rotations
        return typeid(T).hash_code() ^ rotl64(hash<TailSchema>()(TailSchema()), 1);
    }
};

} // namespace std (instantiations of std::hash<T>)

#endif // SERIALIZER_BETTER_STATIC_SCHEMA_HPP
