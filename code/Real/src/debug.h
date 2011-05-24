/**
 * Įvairios funkcijos skirtos derinimui.
 */
#ifndef DEBUG_H
#define DEBUG_H 1

#include "primitives.h"
#include "memlib.h"
#include "monitor_screen_character.h"
#include "monitor_defines.h"


#define PANIC(text) \
              panic(__FILE__, __LINE__, __func__, text);
// NASM funkcijos.

// Pauzė. 
extern "C" void pause();
extern "C" void pause0();
extern "C" void pause1(u64int);
extern "C" void pause2(u64int, u64int);

// Valdymo perdavimas.
extern "C" void jump(u64int);

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

extern "C" void debug_dec(u64int number);
  
extern "C" void activate_kernel_pager();
  
void panic(const char * file, int line, const char * func, const char * t);

// Nusiunčia šešioliktainį skaičių su paaiškinimu į Bochs.
extern "C" void debug_value(const char *str, u64int number);

#endif
