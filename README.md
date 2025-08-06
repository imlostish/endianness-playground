# Endian Playground

> A small project that aims to learn how endianness is used to order bytes and to understand the process and related concepts. This project includes a brief implementation and comments explaining the process so you don't get lost.

> Spanish: Un pequeño proyecto que busca aprender cómo se usa el orden de bytes y comprender el proceso y los conceptos relacionados. Este proyecto incluye una breve implementación y comentarios que explican el proceso para que no se pierda.

---

### English

```
Original:   0x12345678
Bytes:      12    34    56    78
            ^     ^     ^     ^
           B3    B2    B1    B0

After swap:
            78    56    34    12
            ^     ^     ^     ^
           B3    B2    B1    B0

Result:  0x78563412
```

Swap is a function used to reverse the byte order, if the value is reversed twice it returns the original value.

What happens if you swap a uint32 to a uint16_t?

You lose data because the uint16_t swap function only expects 2 bytes, and you give it a 4-byte value. It interprets the remaining bits as nonexistent, which causes incorrect behavior.

```c
uint32_t number = 0x12345678;

uint16_t unsafe_swap = swap_uint16(number);
uint16_t unsafe_swap2 = swap_uint16(unsafe_swap);
```

```bash
Original: 0x12345678
> Unsafe swap: 0x00007856
> Unsafe swap2: 0x00005678
```

---

Bitmask is a value that allows you to isolate, extract,
or clean certain bits from binary strings using a
bitwise operation (such as &, |, ^, ~).

And this is precisely what we need to obtain the MSB and
LSB, and move the intermediate bytes to the correct
positions to correctly form a BE and LE.

| Mask      | Binary                               | Goal                                    |
| ------------ | ------------------------------------- | ------------------------------------------- |
| `0x000000FF` | `00000000 00000000 00000000 11111111` | Extract LSByte (`0x78`) |
| `0x0000FF00` | `00000000 00000000 11111111 00000000` | Extract the second byte (`0x56`)             |
| `0x00FF0000` | `00000000 11111111 00000000 00000000` | Extract the third byte (`0x34`)              |
| `0xFF000000` | `11111111 00000000 00000000 00000000` | Extract MSByte (`0x12`)   |

How are they used?

```c
(val & 0x000000FF) << 24 → Moves byte 0 (LSByte) to byte location 3 (MSByte)
(val & 0x0000FF00) << 8 → Moves byte 1 to byte location 2
(val & 0x00FF0000) >> 8 → Moves byte 2 to byte location 1
(val & 0xFF000000) >> 24 → Moves byte 3 to byte location 0
```

This operation is repeated from 32 bits in ascending order.

How are these masks constructed?

```c
// 0xFF = 11111111 → 1byte
// << (8 * i) → Move 0xFF to the byte you are interested in
uint32_t mask_byte = 0xFF << (8 * <i>);
// mask_byte = 0x000000FF
// mask_byte = 0x0000FF00
// mask_byte = 0x00FF0000
// mask_byte = 0xFF000000
```

With this, I get the exact position of the byte I'm interested in; all we need to do is move it. To represent this, we have a good example in [endian.c → swap_uint32](./endian.c).

```c
uint32_t swap_uint32(uint32_t val) {
    return ((val & 0x000000FF) << 24) | // Byte 0 → Byte 3
           ((val & 0x0000FF00) << 8)  | // Byte 1 → Byte 2
           ((val & 0x00FF0000) >> 8)  | // Byte 2 → Byte 1
           ((val & 0xFF000000) >> 24);  // Byte 3 → Byte 0
}
```

The `(val & 0x000000FF)` operation isolates the desired byte. With `>> <i>` or `<< <i>` we move the byte
position, where `<i>` can be the size in bits of how
many bytes we want to move.

> Extra: You use & to extract, | to join, ^ to alternate, ~ to negate.

This pattern generalizes to any type (uint8_t, uint32_t, uint64_t, even raw structs).

#### Serialize and Deserialize, Alignment and padding of `struct` in C

---

### Spanish

```
Original:   0x12345678
Bytes:      12    34    56    78
            ^     ^     ^     ^
           B3    B2    B1    B0

After swap:
            78    56    34    12
            ^     ^     ^     ^
           B3    B2    B1    B0

Result:  0x78563412
```

Swap es una función que se utiliza para invertir el orden de bytes. Si el valor se invierte dos veces, devuelve el valor original.

¿Qué ocurre si se intercambia un uint32 por un uint16_t?

Se pierden datos porque la función de intercambio uint16_t solo espera 2 bytes y se le asigna un valor de 4 bytes. Interpreta los bits restantes como inexistentes, lo que provoca un comportamiento incorrecto.

```c
uint32_t number = 0x12345678;

uint16_t unsafe_swap = swap_uint16(number);
uint16_t unsafe_swap2 = swap_uint16(unsafe_swap);
```

```bash
Original: 0x12345678
> Unsafe swap: 0x00007856
> Unsafe swap2: 0x00005678
```

La máscara de bits es un valor que permite aislar, extraer o limpiar ciertos bits de cadenas binarias mediante una operación bit a bit (como &, |, ^, ~).

Y esto es precisamente lo que necesitamos para obtener el bit más significativo (MSB) y el bit menos significativo (LSB), y mover los bytes intermedios a las posiciones correctas para formar correctamente un bit inicial (BE) y un bit final (LE).

| Mask      | Binary                               | Goal                                    |
| ------------ | ------------------------------------- | ------------------------------------------- |
| `0x000000FF` | `00000000 00000000 00000000 11111111` | Extract LSByte (`0x78`) |
| `0x0000FF00` | `00000000 00000000 11111111 00000000` | Extract the second byte (`0x56`)             |
| `0x00FF0000` | `00000000 11111111 00000000 00000000` | Extract the third byte (`0x34`)              |
| `0xFF000000` | `11111111 00000000 00000000 00000000` | Extract MSByte (`0x12`)   |

Esta operación se repite desde los 32 bits en orden ascendente.

¿Cómo se construyen estas máscaras?

```c
(val & 0x000000FF) << 24 → Mueve el byte 0 (LSByte) a la posición de byte 3 (MSByte)
(val & 0x0000FF00) << 8 → Mueve el byte 1 a la posición de byte 2
(val & 0x00FF0000) >> 8 → Mueve el byte 2 a la posición de byte 1
(val & 0xFF000000) >> 24 → Mueve el byte 3 a la posición de byte 0
```

Esta operación se repite desde los 32 bits en orden ascendente.

¿Cómo se construyen estas máscaras?

```c
// 0xFF = 11111111 → 1byte
// << (8 * i) → Mueva 0xFF al byte que le interesa
uint32_t mask_byte = 0xFF << (8 * <i>);
// mask_byte = 0x000000FF
// mask_byte = 0x0000FF00
// mask_byte = 0x00FF0000
// mask_byte = 0xFF000000
```


Con esto, obtengo la posición exacta del byte que me interesa; solo tenemos que moverlo. Para representar esto, tenemos un buen ejemplo en [endian.c → swap_uint32](./endian.c).

```c
uint32_t swap_uint32(uint32_t val) {
    return ((val & 0x000000FF) << 24) | // Byte 0 → Byte 3
           ((val & 0x0000FF00) << 8)  | // Byte 1 → Byte 2
           ((val & 0x00FF0000) >> 8)  | // Byte 2 → Byte 1
           ((val & 0xFF000000) >> 24);  // Byte 3 → Byte 0
}
```

La operación `(val & 0x000000FF)` aísla el byte deseado. Con `>> <i>` o `<< <i>` movemos la posición del byte, donde `<i>` puede ser el tamaño en bits de los bytes que queremos mover.

> Extra: Se usa & para extraer, | para unir, ^ para alternar, ~ para negar.

Este patrón se generaliza a cualquier tipo (uint8_t, uint32_t, uint64_t, incluso estructuras sin formato).

---

#### Serializar y deserializar, alineación y relleno de `struct` en C

Ahora que comprendemos las bases del endianismo, ¿Como lo podemos
usar en estructuras complejas como `struct` en C? o ¿Como podemos
llevar a un nivel mas alto lo que aprendimos?

En esta seccion aprenderemos sobre los conceptos de serializacion,
deserializacion, alineación y relleno. Los conceptos se entre lanzaran
en el desarrollo de una implementación que explique practicamente
como manejarlos que se detallan en [sd.c](./src/sd.c).

Si quieres una referencia de como usar parte de los conceptos manejados
en esta seccion, consulta [reference](./src/reference.c) para una
documentacion en codigo.

Para empezar, aprendamos dos conceptos muy importantes, explicados por [nach131](https://github.com/nach131) en [Serialize and Deserialize](https://nach131.github.io/cpp42projects/guias/cpp_c5/module06/serializar).

> La serialización es el proceso de convertir un objeto o estructura de
> datos en una representación binaria o textual que puede ser almacenada en un
> archivo, enviada por red o guardada en una base de datos. La idea detrás de la
> serialización es convertir los datos del objeto en una forma más compacta y
> fácil de transportar.
>
> El proceso de deserializar se refiere a la conversión inversa, donde un archivo
> binario o textual se lee y se convierte de nuevo en un objeto o estructura
> de datos en memoria. Esto permite recuperar los datos del archivo original y
> volverlos accesibles para ser utilizados por el programa.
>
> Estos son conceptos fundamentales en programación, especialmente cuando trabajas
> con comunicación de red, almacenamiento persistente de datos o cualquier
> situación donde necesitas convertir estructuras de datos en formatos que puedan
> ser gestionados de manera más fácil.

**A diferencia del ejemplo de @nach131, no usaremos C++.**

📦 Serialización
  - Tome una estructura como entrada.
  - Cree un `uint8_t buffer[N]`.
  - Copie cada campo manualmente en orden usando memcpy, shift o casting seguro.
  - Aplique endianismo por campo.

📥 Deserialización
  - Reciba un `uint8_t*` y tamaño.
  - Lea byte por byte.
  - Reconstruya los campos y convierta desde endianismo.
  - Llene la estructura destino.

> No usaremos `raw pointers`, por que perderemos seguridad
> de tipos, no tiene un tamaño definido, no podemos desreferenciarlo
> y puede causar problemas a la hora de leer nuestro binario para
> el proceso de deserializacion.
> No exploraremos mucho de los `raw pointers` pero puedes consultar ["Raw pointers (C++)"][8] o [why do void* pointers even exist? by low level tv](https://www.youtube.com/watch?v=t7CUti_7d7c).

Usaremos offsets y sizes para ubicar y recorrer los datos con IDs
para identificar tipos o estructuras serializadas.

Aunque [`#pragma pack(1)` deshabilita el padding en las estructuras
y permite el conflicto de desalineamiento que genera dificultades
en la generacion de codigo ensamblador (para RISC)][4].

[1]: https://gcc.gnu.org/onlinedocs/gcc-4.1.0/gcc/Type-Attributes.html "5.32 Specifying Attributes of Types"
[2]: https://www.gnu.org/software/c-intro-and-ref/manual/html_node/Packed-Structures.html "15.6 Packed Structures"
[3]: https://learn.microsoft.com/en-us/cpp/preprocessor/pack?view=msvc-170 "#pragma pack docs"
[4]: https://devblogs.microsoft.com/oldnewthing/20200103-00/?p=103290 "Anybody who writes #pragma pack(1) may as well just wear a sign on their forehead that says “I hate RISC”"
[5]: https://stackoverflow.com/a/3318475 "#pragma pack effect"
[6]: https://stackoverflow.com/a/11772340 "What is the meaning of \"__attribute__((packed, aligned(4))) \""
[7]: https://stackoverflow.com/a/11669250 "Why does padding have to be a power of two?"
[8]: https://learn.microsoft.com/en-gb/cpp/cpp/raw-pointers?view=msvc-170 "Raw pointers (C++)"
[9]: https://www.youtube.com/watch?v=t7CUti_7d7c "why do void* pointers even exist? by low level tv"
