#ifndef _COMMON_H
#define _COMMON_H

#include <stdint.h>

template <uint8_t A, uint8_t B, uint8_t... C>
struct areAllEqual {
    static constexpr bool value = A == B && areAllEqual<A, C...>::value;
};

template <uint8_t A, uint8_t B>
struct areAllEqual<A, B> {
    static constexpr bool value = A == B;
};

#define ASSERT_REGISTER_FLAGS_MATCH(X, Y, ...)                                 \
    static_assert(areAllEqual<X, Y, ##__VA_ARGS__>::value,                     \
                  "Registers " #X ", " #Y __VA_OPT__(", ") #__VA_ARGS__        \
                  " must match")

// Fake is_same implementation, based on
// https://github.com/modm-io/avr-libstdcpp/blob/master/include/type_traits
template <typename T, typename U>
struct isType {
    static constexpr bool value = false;
};

template <typename T>
struct isType<T, T> {
    static constexpr bool value = true;
};

#endif /* _COMMON_H */