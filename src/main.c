#include <stdio.h>
#include <stdint.h>
#include "endian.h"

int main() {
    printf("🧠 Endian Playground\n\n");

    if (is_little_endian()) {
        printf("✅ System: Little Endian\n");
    } else {
        printf("✅ System: Big Endian\n");
    }

    printf("\n🔍 Example with uint32_t:\n");

    uint32_t number = 0x12345678;
    /*
     *  |  uint32_t number   |
     *  | Byte | Value (hex) |
     *  | ---- | ----------- |
     *  | B0   | `12`        |
     *  | B1   | `34`        |
     *  | B2   | `56`        |
     *  | B3   | `78`        |
    */

    uint32_t swapped = swap_uint32(number);
    uint32_t back = swap_uint32(swapped);

    uint16_t unsafe_swap = swap_uint16(number);
    uint16_t unsafe_swap2 = swap_uint16(unsafe_swap);

    printf("Original: 0x%08X\n", number);
    printf("> Unsafe swap: 0x%08X\n", unsafe_swap);
    printf("> Unsafe swap2: 0x%08X\n", unsafe_swap2);
    printf("Bytes in memory: ");
    print_bytes(&number, sizeof(number));

    printf("Swapped : 0x%08X\n", swapped);
    printf("Bytes swapped: ");
    print_bytes(&swapped, sizeof(swapped));

    printf("Swapped 2: 0x%08X\n", back);
    printf("Bytes swapped: ");
    print_bytes(&back, sizeof(back));

    printf("\n🎯 Example with float:\n");

    float pi = 3.1415927f;
    printf("Float: %f\n", pi);
    printf("Bytes in memory: ");
    print_bytes(&pi, sizeof(pi));

    return 0;
}
