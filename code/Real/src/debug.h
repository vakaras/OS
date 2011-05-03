/**
 * Įvairios funkcijos skirtos derinimui.
 */
#ifndef DEBUG_H
#define DEBUG_H 1

#include "primitives.h"

// NASM funkcijos.

extern "C" void pause();
extern "C" void pause0();
extern "C" void pause1(u64int);
extern "C" void pause2(u64int, u64int);

// C++ funkcijos.

// Nusiunčia numatytą žinutę į Bochs.
extern "C" void debug_ping();

// Nusiunčia simbolį į Bochs.
extern "C" void debug_char(char c);

// Nusiunčia simbolių seką, kurios pabaigos požymis yra 0, į Bochs.
extern "C" void debug_string(const char *str);

#endif
