#include <stdint.h>
#include <stdio.h>

int main(void) {
    for (int i = 0; i < 4; i++) {
        printf("Byte: %i\n", i);
        printf("Value: 0x%08X\n", 0xFF << (8 * i));
    }

    return 0;
}
