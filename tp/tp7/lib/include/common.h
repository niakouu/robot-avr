#ifndef _ASSERT_H
#define _ASSERT_H

#include <stdint.h>

template <uint8_t First, uint8_t... Rest>
constexpr bool areAllEqual = ((First == Rest) && ...);

#define ASSERT_REGISTER_FLAGS_MATCH(X, Y, ...)                                 \
    static_assert(areAllEqual<X, Y, ##__VA_ARGS__>,                            \
                  "Registers " #X ", " #Y __VA_OPT__(", ") #__VA_ARGS__        \
                  " must match")


// Fake is_same implementation, based on
// https://github.com/modm-io/avr-libstdcpp/blob/master/include/type_traits
template <typename T, typename U>
struct is_type {
    static constexpr bool value = false;
};

template <typename T>
struct is_type<T, T> {
    static constexpr bool value = true;
};

#endif /* _ASSERT_H */