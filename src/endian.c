#include "endian.h"
#include <stdint.h>
#include <stdio.h>


bool is_little_endian() {
    uint16_t x = 0x1;
    return *((uint8_t*)&x) == 0x1;
}

bool is_big_endian() {
    return !is_little_endian();
}

void print_bytes(const void* data, size_t size) {
    const uint8_t* bytes = (const uint8_t*)data;
    for (size_t i = 0; i < size; ++i) {
        printf("%02X ", bytes[i]);
    }
    printf("\n");
}

uint16_t swap_uint16(uint16_t val) {
  return (val << 8) | (val >> 8);
};

uint32_t swap_uint32(uint32_t val) {
    return ((val & 0x000000FF) << 24) | // Byte 0 → Byte 3
           ((val & 0x0000FF00) << 8)  | // Byte 1 → Byte 2
           ((val & 0x00FF0000) >> 8)  | // Byte 2 → Byte 1
           ((val & 0xFF000000) >> 24);  // Byte 3 → Byte 0
}

uint64_t swap_uint64(uint64_t val) {
    return ((val & 0x00000000000000FFULL) << 56) | // Byte 0 → Byte 7
           ((val & 0x000000000000FF00ULL) << 40) | // Byte 1 → Byte 6
           ((val & 0x0000000000FF0000ULL) << 24) | // Byte 2 → Byte 5
           ((val & 0x00000000FF000000ULL) << 8)  | // Byte 3 → Byte 4
           ((val & 0x000000FF00000000ULL) >> 8)  | // Byte 4 → Byte 3
           ((val & 0x0000FF0000000000ULL) >> 24) | // Byte 5 → Byte 2
           ((val & 0x00FF000000000000ULL) >> 40) | // Byte 6 → Byte 1
           ((val & 0xFF00000000000000ULL) >> 56);  // Byte 7 → Byte 0
} // ⚠️ Note that I use 0xFFULL to indicate that we are working with uint64_t.
