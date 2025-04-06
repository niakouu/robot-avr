#ifndef _COMMON_H
#define _COMMON_H

#include <stddef.h>
#include <stdint.h>

namespace {
    constexpr uint8_t UINT8_WIDTH = 8U;
    constexpr uint16_t UINT16_WIDTH = 16U;
} // namespace

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

template <typename T>
constexpr T&& move(T&& value) {
    return static_cast<T&&>(value);
}

/// Taken from https://en.cppreference.com/w/cpp/algorithm/clamp
template <class T>
constexpr const T clamp(const T value, const T low, const T high) {
    return value < low ? low : value > high ? high : value;
}

// Placement new and delete. Taken from
// https://github.com/modm-io/avr-libstdcpp/blob/master/include/new
inline void* operator new(size_t, void* p) noexcept {
    return p;
}

inline void* operator new[](size_t, void* p) noexcept {
    return p;
}

inline void operator delete(void*, void*) noexcept {}

inline void operator delete[](void*, void*) noexcept {}

template <typename T>
inline T cappingSubtract(T source, T diff) {
    return diff < source ? source - diff : 0;
}

#endif /* _COMMON_H */