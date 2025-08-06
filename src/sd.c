#include <stdint.h>
#include <stdio.h>

void write_u8(uint8_t* buffer, uint8_t value) {
    buffer[0] = value;
}

void write_u16(uint8_t* buffer, uint16_t value) {
    buffer[0] = value & 0xFF;        // Byte 0
    buffer[1] = (value >> 8) & 0xFF; // Byte 1
}

void write_u32(uint8_t* buffer, uint32_t value) {
    buffer[0] = value & 0xFF;           // Byte 0
    buffer[1] = (value >> 8) & 0xFF;    // Byte 1
    buffer[2] = (value >> 16) & 0xFF;   // Byte 2
    buffer[3] = (value >> 24) & 0xFF;   // Byte 3
}

void write_u64(uint8_t* buffer, uint64_t value) {
    for (int i = 0; i < 8; i++)
        buffer[i] = (value >> (i * 8)) & 0xFF;
}

uint8_t read_u8(const uint8_t* buffer) {
    return buffer[0];
}

uint16_t read_u16(const uint8_t* buffer) {
    return buffer[0] | (buffer[1] << 8);
}

uint32_t read_u32(const uint8_t* buffer) {
    return buffer[0] | (buffer[1] << 8) | (buffer[2] << 16) | (buffer[3] << 24);
}

uint64_t read_u64(const uint8_t* buffer) {
    uint64_t value = 0;
    for (int i = 0; i < 8; i++)
        value |= ((uint64_t)buffer[i]) << (i * 8);
    return value;
}

void hex_dump(const uint8_t* buffer, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", buffer[i]);
        if ((i + 1) % 8 == 0) printf("| ");
        if ((i + 1) % 16 == 0) printf("\n");
    }
    if (len % 16 != 0) printf("\n");
}

int main() {
    uint8_t buffer[16]; // A 16-byte raw buffer is declared.

    write_u16(buffer, 0xABCD); // Save 2 bytes (CD AB)
    write_u32(buffer + 2, 0x12345678); // Save 4 bytes (78 56 34 12)
    write_u64(buffer + 6, 0xCAFEBABEDEADBEEF); // Save 8 bytes from position (EF BE AD DE BE BA FE CA)

    hex_dump(buffer, 14);

    uint16_t a = read_u16(buffer); // Read the first 2 bytes → 0xABCD
    uint32_t b = read_u32(buffer + 2); // Read bytes 2–5 → 0x12345678
    uint64_t c = read_u64(buffer + 6); // Read bytes 6–13 → 0xCAFEBABEDEADBEEF

    printf("Read values: %04X, %08X, %016lX\n", a, b, c);
    return 0;
}
