#ifndef ENDIAN_H
#define ENDIAN_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

bool is_little_endian();
bool is_big_endian();

void print_bytes(const void* data, size_t size);
uint16_t swap_uint16(uint16_t val);
uint32_t swap_uint32(uint32_t val);
uint64_t swap_uint64(uint64_t val);

#endif
