#ifndef _PIN_H
#define _PIN_H

#include <stdint.h>

class Pin {
public:
    static constexpr uint8_t NUMBER_OF_PINS = 8;

    enum class Region : uint8_t { A, B, C, D };

    enum class Direction : uint8_t { IN, OUT };

    enum class Id : uint8_t { P0, P1, P2, P3, P4, P5, P6, P7 };

    struct Mappings {
        uint8_t directionBit;
        uint8_t pinBit;
        uint8_t portBit;
    };

    struct Registers {
        volatile uint8_t *pin, *port, *dataDirection;
    };

    Pin(Region region, Id id) noexcept;
    Pin(Region region, Id id, Direction direction);
    ~Pin();

    void updateDirection(Direction direction) const;

    bool read() const;
    void write(bool set) const;
    void set() const;
    void unset() const;

private:
    static const constexpr Mappings& getMappings(Region region, Id id);
    static constexpr Registers getRegisters(Region region);

    const Mappings& mappings_;
    Registers registers_;
};

#endif /* _PIN_H */