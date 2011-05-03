/**
 * Įvairūs primityvai.
 */
#ifndef PRIMITIVES_H
#define PRIMITIVES_H 1

#include "types.h"
 
// Pasiųsti baitą į nurodytą portą.
void send_byte(u16int port, u8int value);

// Nuskaityti baitą iš nurodyto porto.
u8int get_byte(u16int port);

// Nuskaityti du baitus iš nurodyto porto.
u16int get_bytes2(u16int port);

// Nukopijuoja nurodytus len baitų iš src į dest.
void memcpy(u8int *dest, const u8int *src, u32int length);

// Užpildo nurodytą atmintį reikšme value.
void memset(u8int *dest, u8int value, u32int length);

// Nusiunčia žinutę į Bochs. (Funkcija skirta derinimui.)
extern "C" void debug_ping();

// Nusiunčia simbolį į Bochs. (Funkcija skirta derinimui.)
extern "C" void debug_char(char c);

// Nusiunčia simbolių seką, kurios pabaigos požymis yra 0, į Bochs.
// (Funkcija skirta derinimui.)
extern "C" void debug_string(const char *str);

#endif
