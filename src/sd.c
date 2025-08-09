#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

typedef struct {
    uint32_t magic;
    uint8_t version;
    uint16_t payload_len;
    uint64_t payload;
} BinaryExampleOne;

typedef struct {
    uint32_t id;
    float temperature;
    char name[10];
} BinaryExampleTwo;

size_t serialize_two(uint8_t *buffer, const BinaryExampleTwo *src) {
    size_t offset = 0;

    write_u32(buffer + offset, src->id);
    offset += 4;

    // Copiar float como 4 bytes crudos
    uint32_t temp_bits;
    memcpy(&temp_bits, &src->temperature, sizeof(float));
    write_u32(buffer + offset, temp_bits);
    offset += 4;

    memcpy(buffer + offset, src->name, 10);
    offset += 10;

    return offset;
}

void deserialize_two(const uint8_t *buffer, BinaryExampleTwo *dst) {
    size_t offset = 0;

    dst->id = read_u32(buffer + offset);
    offset += 4;

    uint32_t temp_bits = read_u32(buffer + offset);
    memcpy(&dst->temperature, &temp_bits, sizeof(float));
    offset += 4;

    memcpy(dst->name, buffer + offset, 10);
}

typedef struct {
    uint64_t timestamp;
    uint16_t count;
    uint32_t *values;
} BinaryExampleThree;

size_t serialize_three(uint8_t *buffer, const BinaryExampleThree *src) {
    size_t offset = 0;

    write_u64(buffer + offset, src->timestamp);
    offset += 8;

    write_u16(buffer + offset, src->count);
    offset += 2;

    for (int i = 0; i < src->count; i++) {
        write_u32(buffer + offset, src->values[i]);
        offset += 4;
    }

    return offset;
}

void deserialize_three(const uint8_t *buffer, BinaryExampleThree *dst) {
    size_t offset = 0;

    dst->timestamp = read_u64(buffer + offset);
    offset += 8;

    dst->count = read_u16(buffer + offset);
    offset += 2;

    dst->values = malloc(dst->count * sizeof(uint32_t));
    for (int i = 0; i < dst->count; i++) {
        dst->values[i] = read_u32(buffer + offset);
        offset += 4;
    }
}

void console_sd(void) {
    puts("--- Console S/D ---");
    uint8_t buffer[16]; // A 16-byte raw buffer is declared.

    write_u16(buffer, 0xABCD); // Save 2 bytes (CD AB)
    write_u32(buffer + 2, 0x12345678); // Save 4 bytes (78 56 34 12)
    write_u64(buffer + 6, 0xCAFEBABEDEADBEEF); // Save 8 bytes from position (EF BE AD DE BE BA FE CA)

    hex_dump(buffer, 14);

    uint16_t a = read_u16(buffer); // Read the first 2 bytes → 0xABCD
    uint32_t b = read_u32(buffer + 2); // Read bytes 2–5 → 0x12345678
    uint64_t c = read_u64(buffer + 6); // Read bytes 6–13 → 0xCAFEBABEDEADBEEF

    printf("Read values: %04X, %08X, %016lX\n", a, b, c);
    puts("--- End Console S/D ---");

}

void binary_sd(void) {
    printf("--- Binary S/D ---\n");

    printf("> Example One\n");
    BinaryExampleOne beo = {
        .magic = 0x62636473,
        .version = 0x31,
        .payload_len = 0x00,
        .payload = 0x00,
    };

    FILE *fl_w = fopen("file-one.bin", "wb");
    int fw_eor = fwrite(&beo, sizeof(beo), 1, fl_w);

    if (fw_eor == -1) {
        printf("Has been a problem with fwrite in \"binary_sd\"\n");
        return;
    }

    fclose(fl_w);
    printf("It has been created \"file-one.bin\"\n");
    printf("> End of Example One\n");

    // --- Example Two: Serialización manual ---
    printf("> Example Two\n");

    BinaryExampleTwo bet = {
           .id = 12345,
           .temperature = 36.5f,
           .name = "Sensor_01"
    };

    uint8_t buffer_et[64];
    size_t size_et = serialize_two(buffer_et, &bet);

    FILE *f2 = fopen("file-two.bin", "wb");
    int fw_etr = fwrite(buffer_et, 1, size_et, f2);
    if (fw_etr == -1) {
        printf("Has been a problem with fwrite in \"binary_sd\"\n");
        return;
    }

    fclose(f2);
    printf("It has been created \"file-two.bin\"\n");

    uint8_t readbuff_et[64];
    f2 = fopen("file-two.bin", "rb");
    fread(readbuff_et, 1, size_et, f2);

    BinaryExampleTwo bet_copy;
    deserialize_two(readbuff_et, &bet_copy);

    printf("Result of deserialize:\n");
    printf("Two -> ID: %u, Temp: %.2f, Name: %s\n", bet_copy.id, bet_copy.temperature, bet_copy.name);
    printf("> End of Example Two\n");

    // --- Example Three: Lista dinámica ---
    printf("> Example Three\n");

    BinaryExampleThree bet3;
    bet3.timestamp = 0x1122334455667788;
    bet3.count = 3;
    uint32_t vals[] = {100, 200, 300};
    bet3.values = vals;

    uint8_t buffer3[128];
    size_t size3 = serialize_three(buffer3, &bet3);

    FILE *f3 = fopen("file-three.bin", "wb");
    int fw_ett = fwrite(buffer3, 1, size3, f3);
    if (fw_etr == -1) {
        printf("Has been a problem with fwrite in \"binary_sd\"\n");
        return;
    }
    fclose(f3);
    printf("It has been created \"file-three.bin\"\n");

    uint8_t readbuf3[128];
    f3 = fopen("file-three.bin", "rb");
    fread(readbuf3, 1, size3, f3);
    fclose(f3);

    BinaryExampleThree bet3_copy;
    deserialize_three(readbuf3, &bet3_copy);
    printf("Result of deserialize:\n");
    printf("Three -> Timestamp: %016llX, Count: %u\n", (unsigned long long)bet3_copy.timestamp, bet3_copy.count);
    for (int i = 0; i < bet3_copy.count; i++)
           printf("Value[%d] = %u\n", i, bet3_copy.values[i]);
    free(bet3_copy.values);
    printf("> End of Example Three\n");
    printf("--- End Binary S/D ---\n");
}

void send_in_net_sd(void) {
    printf("--- Send in Network S/D ---\n");

    printf("--- End Send in Network S/D ---\n");
}

int main() {

    // first step
    console_sd();

    // second step
    binary_sd();

    // third step
    send_in_net_sd();
    return 0;
}
