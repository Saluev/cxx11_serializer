#ifndef SERIALIZER_UTILITY_HPP
#define SERIALIZER_UTILITY_HPP

namespace serializer {

template<typename... Types>
struct TypeList {
    static const size_t length = sizeof...(Types);
};
using EmptyList = TypeList<>;
template<typename... Ts> struct Concatenate;
template<>
struct Concatenate<> {
    using Result = EmptyList;
};
template<typename... A>
struct Concatenate<TypeList<A...>> {
    using Result = TypeList<A...>;
};
template<typename... A, typename... B>
struct Concatenate<TypeList<A...>, TypeList<B...>> {
    using Result = TypeList<A..., B...>;
};
template<typename... A, typename... Ts>
struct Concatenate<TypeList<A...>, Ts...> {
    using Result = typename Concatenate<
        TypeList<A...>,
        typename Concatenate<Ts...>::Result
    >::Result;
};
template<typename T> struct Head;
template<typename T, typename... Ts>
struct Head<TypeList<T, Ts...>> {
    using Result = typename Concatenate<TypeList<T>, typename Head<TypeList<Ts...>>::Result>::Result;
};
template<typename T, typename Q>
struct Head<TypeList<T, Q>> {
    using Result = TypeList<T>;
};
template<typename T>
struct Head<TypeList<T>> {
    using Result = EmptyList;
};
template<>
struct Head<EmptyList> {
    using Result = EmptyList;
};

// CartesianProduct takes two lists of lists and produces list of lists
template<typename A, typename B> struct CartesianProduct;
template<typename A, typename... As, typename B, typename... Bs>
struct CartesianProduct<TypeList<A, As...>, TypeList<B, Bs...>> {
    using Result = typename Concatenate<
        TypeList<typename Concatenate<A, B>::Result>,
        typename CartesianProduct<TypeList<A    >, TypeList<Bs   >>::Result...,
        typename CartesianProduct<TypeList<As   >, TypeList<B    >>::Result...,
        typename CartesianProduct<TypeList<As...>, TypeList<Bs...>>::Result
    >::Result;
};
template<>
struct CartesianProduct<EmptyList, EmptyList> {
    using Result = EmptyList;
};
template<typename A>
struct CartesianProduct<A, EmptyList> {
    using Result = A;
};
template<typename B>
struct CartesianProduct<EmptyList, B> {
    using Result = B;
};

template<typename T>
struct Envelope {
    using Result = TypeList<T>; // making a list of lists
};
template<typename... As, typename... Bs>
struct Envelope<TypeList<TypeList<As...>, Bs...>> {
    using Result = TypeList<TypeList<As...>, Bs...>; // no changes
};

template <typename T>
T swap_endian(T u)
{
    static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");

    union
    {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}

} // namespace serializer

#endif // SERIALIZER_UTILITY_HPP
