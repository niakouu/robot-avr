#include "MemoryTest.h"

#include <stdlib.h>
#include <string.h>

#include "Board.h"
#include "Memory24.h"

const char* MemoryTest::NAME = "Memory";

const char* MemoryTest::getName() const {
    return MemoryTest::NAME;
}

uint8_t MemoryTest::runTests(void (*log)(const char* format, ...)) const {
    uint8_t fails = 0;
    Memory24& memory = Board::get().getMemory();
    constexpr uint8_t BUF_SIZE = 127;
    constexpr uint8_t SINGLE_BYTE_TEST_ADDRESSES[] = {0, 63, 127};
    constexpr uint8_t SINGLE_BYTE_TESTS[] = {0x00, 0x35, 0xa2, 0xff};
    constexpr const char* STRING_TESTS[] = {"This is a test to write",
                                            "$$$$$$$$$$$$$$$$$$$$$$$",
                                            "Shorter string", "tiny"};

    char buf[BUF_SIZE];

    for (const uint8_t address : SINGLE_BYTE_TEST_ADDRESSES) {
        for (const uint8_t singleByteTest : SINGLE_BYTE_TESTS) {
            memory.write(address, singleByteTest);
            const uint8_t readData = memory.read(address);
            if (readData != singleByteTest) {
                log("r/w of 0x%02x failed at address 0x%02x: got "
                    "0x%02x\n",
                    singleByteTest, address, readData);

                ++fails;
            }
        }
    }

    for (const char* i : STRING_TESTS) {
        const uint8_t length = strlen(i);
        // We must go through reinterpret_cast because of pointer cast.
        // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
        memory.write(0, reinterpret_cast<const uint8_t*>(i), length + 1);

        memory.read(0, reinterpret_cast<uint8_t*>(buf), sizeof(buf));
        // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

        if (strncmp(i, buf, sizeof(buf) < length ? sizeof(buf) : length) != 0) {
            log("read/write of string \"%s\" failed\n", i);

            ++fails;
        }
    }

    return fails;
}