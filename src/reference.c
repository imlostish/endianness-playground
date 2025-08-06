#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

/*
 * ? __attribute__((<attr>))
 * A compiler extension (not a macro) that is applied
 * directly to a struct or union. It tells the compiler
 * to pack the structure without padding.
 *
 * - ⚠️ Not compatible with MSVC.
 * - ⚠️ Specific to GCC/Clang-type compilers.
 *
 * - __attribute__((aligned(n))): Forces the structure or variable to be aligned to n bytes.
 * - __attribute__((packed)): Prevents padding between structure members.
 * - __attribute__((transparent_union)): Allows a function to receive any type from the union without explicit casting.
 * - __attribute__((unused)): Prevents warnings about unused variables.
 * - __attribute__((deprecated("msg"))): Marks a function, variable, or type as deprecated; may include an optional message.
 * - __attribute__((may_alias)): Allows aliasing between types, useful for reinterpret_casts.
 *
 *  typedef struct {
 *      uint8_t test;
 *  } __attribute__((packed)) Example;
 *
 *  typedef struct __attribute__((packed)) {
 *      uint8_t test;
 *  } Example;
 *
 * ? #pragma pack
 *
 * A preprocessor directive that tells the compiler
 * how to align the fields of a structure.
 *
 * Portable between compilers that support #pragma
 * (such as GCC, MSVC, Clang).
 *
 * Can be applied temporarily (with push/pop).
 *
 * - ⚠️ Not all compilers guarantee the same behavior.
 * - ⚠️ It is not part of the ISO C standard (but is widely supported).
 *
 * - #pragma pack( show )
 * - #pragma pack( push [ , identifier ] [ , n ] )
 * - #pragma pack( pop [ , { identifier | n } ] )
 * - #pragma pack( [ n ] )
 *
 *
*/

// __attribute__((aligned (<n>))):
typedef struct {
    short   l[3];
    int     o[3];
    char    s[3];
    double  t[3];
} __attribute__((aligned (1))) attr_aligned_exp;

// __attribute__((packed)):
typedef struct {
    char cl;
    int  co;
} __attribute__((__packed__)) attr_packed_exp;

// __attribute__((transparent_union)):
typedef union {
    int         *__l;
    short       *__o;
    char        __s;
    double      __t;
} __attribute__((transparent_union)) attr_tu_exp;

// void process(attr_tu_exp arg) {
//    printf("TransparentUnion called with int: %n or short: %p or char: %c or double: %f\n", arg.__l, arg.__o, arg.__s, arg.__t);
// }

// __attribute__((unused)):
typedef struct {
    char block_function_name;
    char check_function_name;
} __attribute__((unused)) attr_unused_exp;

// void blockf(const char* msg, size_t size_msg, attr_aligned_exp* attr_a_exp);
// int checkproccesf(const char* process_name);

// __attribute__((deprecated("msg" | "replacement"))):

typedef int TL1 __attribute__((deprecated("This alias is being replaced in the latest version.")));
typedef int TL4;

// __attribute__((may_alias)):

typedef short __attribute__((__may_alias__)) TL2;

// #pragma pack
// → #pragma pack() // n defaults to 8; equivalent to /Zp8
// → #pragma pack(4) → ⚠️ It doesn't restore the original settings, it can be problematic. Use push and pop as an alternative.
// → #pragma pack(show)
// → #pragma pack(push, r1, 16)
// → #pragma pack(pop, r1, 2)

typedef struct {
    int     l; // 4 bytes
    short   o; // 2 bytes
    char    s; // 1 byte
    double  t; // 8 bytes
} WITHOUT_PRAGMAPACK_EXP;

// ? #pragma pack(2) → !#pragma pack(pop)
// Save the previous configuration and force 1-byte alignment
#pragma pack(push, 2)
typedef struct {
    int     r; // 4 bytes
    short   u; // 2 bytes
    char    k; // 1 byte
    double  a; // 8 bytes
} WITH_PRAGMAPACK_EXP;
// Restores previous alignment
#pragma pack(pop)

// #pragma pack + __attribute__

#pragma pack(push, 1)
typedef struct {
    uint8_t type;
    uint16_t length;
} __attribute__((packed)) PRAGMAPACK_WITH_ATTR;
#pragma pack(pop)

int main(void) {
    puts("% Welcome to reference\n");

    // https://gcc.gnu.org/onlinedocs/gcc-4.1.0/gcc/Type-Attributes.html

    puts("% __attribute__ examples:");

    printf("\nSize of attr_packed_exp: %zu\n", sizeof(attr_packed_exp));
    printf("Size of attr_aligned_exp: %zu\n", sizeof(attr_aligned_exp));

    // https://learn.microsoft.com/en-us/cpp/preprocessor/pack?view=msvc-170

    puts("\n% Pragma Pack examples:");

    printf("WITHOUT_PRAGMAPACK_EXP offsets:\n");
    printf("  l: %zu\n", offsetof(WITHOUT_PRAGMAPACK_EXP,  l));
    printf("  o: %zu\n", offsetof(WITHOUT_PRAGMAPACK_EXP,  o));
    printf("  s: %zu\n", offsetof(WITHOUT_PRAGMAPACK_EXP,  s));
    printf("  t: %zu\n", offsetof(WITHOUT_PRAGMAPACK_EXP,  t));
    printf("  Total Size: %zu\n", sizeof(WITHOUT_PRAGMAPACK_EXP));

    printf("WITH_PRAGMAPACK_EXP offsets:\n");
    printf("  r: %zu\n", offsetof(WITH_PRAGMAPACK_EXP,  r));
    printf("  u: %zu\n", offsetof(WITH_PRAGMAPACK_EXP,  u));
    printf("  k: %zu\n", offsetof(WITH_PRAGMAPACK_EXP,  k));
    printf("  a: %zu\n", offsetof(WITH_PRAGMAPACK_EXP,  a));
    printf("  Total Size: %zu\n", sizeof(WITH_PRAGMAPACK_EXP));

    // - `offsetof(struct, member)` can help you inspect actual field offsets.

    puts("\n% Pragma Pack with __attribute__:");
    printf("Size of PRAGMAPACK_WITH_ATTR: %zu\n", sizeof(PRAGMAPACK_WITH_ATTR));

    return 0;
}
