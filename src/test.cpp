#include <iostream>
#include <utility>
#include "../include/serializer.hpp"

using namespace serializer;
using MySchema = Schema<HashField, IntegerField, IntegerField, CharField >;
using ComplexSchema = Schema<HashField, MySchema, IntegerField, MySchema>;

template<class T>
T test_template(T&& k) {
    return 2 * k;
}

int test(int&& k) {
    return 2 * k;
}

int main() {

    static_assert( can_serialize<IntegerField, TypeList<BitStream&, int>>::value, "something went wrong!");
    static_assert( can_serialize<IntegerField, TypeList<BitStream&, char&>>::value, "something went wrong!");
    static_assert(!can_serialize<IntegerField, TypeList<BitStream&>>::value, "something went wrong!");
    static_assert(!can_serialize<IntegerField, TypeList<BitStream&, MySchema>>::value, "something went wrong!");

    static_assert(!can_deserialize<IntegerField, TypeList<BitStream&, int >>::value, "something went wrong!");
    static_assert( can_deserialize<IntegerField, TypeList<BitStream&, int&>>::value, "something went wrong!");
    static_assert(!can_deserialize<IntegerField, TypeList<BitStream&, const int&>>::value, "something went wrong!");
    static_assert(!can_deserialize<IntegerField, TypeList<BitStream&, MySchema>>::value, "something went wrong!");

    static_assert(!can_serialize<Schema<IntegerField>, TypeList<BitStream&, int, int, char>>::value, "something went wrong");
    static_assert(!can_serialize<MySchema, TypeList<BitStream&, int, int, char, int, int, int, char>>::value, "something went wrong!");

    int k = 777;
    //test_template(k);
    //test(k);

    //std::cout << Schema<IntegerField, IntegerField, SimpleField<char> >::hash_code() << std::endl;
    //std::cout << Schema<IntegerField, SimpleField<char>, IntegerField >::hash_code() << std::endl;

    MySchema::serialize(std::cout, 777, 6666, 'a');
    std::cout << std::endl;

    BitStream bs;
    MySchema::serialize(bs, 777, 6666, 'a');

    //ComplexSchema::OutputArguments::hey();
    ComplexSchema::serialize(std::cout, 11, 22, 'a', 33, 44, 55, 'b');
    std::cout << std::endl;

    int a, b;
    char c;
    std::cout << "Enter hash, number, number and character: " << std::flush;
    bool result = MySchema::deserialize(std::cin, a, b, c);
    if(result) {
        std::cout << "Your (correct) input: ";
        MySchema::serialize(std::cout, a, b, c);
        std::cout << std::endl;
    } else {
        std::cout << "Incorrect input." << std::endl;
    }

    std::vector<int> v;
    std::cout << "Enter length and sequence of numbers: " << std::flush;
    result = Schema<SimpleArrayField<int>>::deserialize(std::cin, v);
    if(result) {
        std::cout << "Your (correct) input: ";
        Schema<SimpleArrayField<int>>::serialize(std::cout, v);
        std::cout << std::endl;
    } else {
        std::cout << "Incorrect input." << std::endl;
    }

    std::cout << "Enter length and sequence of numbers: " << std::flush;
    uint64_t n;
    int* data;
    result = Schema<SimpleArrayField<int>>::deserialize(std::cin, n, data);
    if(result) {
        std::cout << "Your (correct) input: ";
        Schema<SimpleArrayField<int>>::serialize(std::cout, n, data);
        std::cout << std::endl;
        delete[] data;
    } else {
        std::cout << "Incorrect input." << std::endl;
    }

}
