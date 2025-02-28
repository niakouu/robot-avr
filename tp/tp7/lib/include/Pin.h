#ifndef _PIN_H
#define _PIN_H

#include <stdint.h>

class Pin {
public:
    enum class Region : uint8_t { A, B, C, D };

    enum class Direction : uint8_t { IN, OUT };

    enum class Id : uint8_t { P0, P1, P2, P3, P4, P5, P6, P7 };

    struct Mappings {
        uint8_t directionBit;
        uint8_t pinBit;
        uint8_t portBit;
    };

    Pin(Region region, Direction direction, Id id);
    ~Pin();

    bool read() const;

    void write(bool set) const;
    void set() const;
    void unset() const;

private:
    static const Mappings* getMappings(const Region& region, const Id& id);
    const Mappings* mappings_;
    volatile uint8_t* pinPort_;

    void updateDirection(volatile uint8_t* dataDirectionRegister,
                         const Direction& direction);
};

#endif /* _PIN_H */