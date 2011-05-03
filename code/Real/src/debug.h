/**
 * Įvairios funkcijos skirtos derinimui.
 */
#ifndef DEBUG_H
#define DEBUG_H 1

#include "primitives.h"

// NASM funkcijos.

// Pauzė. 
extern "C" void pause();
extern "C" void pause0();
extern "C" void pause1(u64int);
extern "C" void pause2(u64int, u64int);

// C++ funkcijos.

// Pauzė. 
void pausep(u64int *);

// Nusiunčia numatytą žinutę į Bochs.
extern "C" void debug_ping();

// Nusiunčia simbolį į Bochs.
extern "C" void debug_char(char c);

// Nusiunčia simbolių seką, kurios pabaigos požymis yra 0, į Bochs.
extern "C" void debug_string(const char *str);

// Nusiunčia šešioliktainį skaičių į Bochs.
extern "C" void debug_hex(u64int number);

#endif
