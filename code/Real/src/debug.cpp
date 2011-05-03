#include "debug.h"


// Nusiunčia numatytą į Bochs.
void debug_ping() {
  debug_string("Veikia.\n");
  }


// Nusiunčia baitą į Bochs.
void debug_char(char c) {
  send_byte(0xE9, c);
  }


// Nusiunčia simbolių seką, kurios pabaigos požymis yra 0, į Bochs.
void debug_string(const char *str) {
  for (const char *i = str; *i; i++)
    send_byte(0xE9, *i);
  }
