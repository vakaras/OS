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


// Nusiunčia šešioliktainį skaičių į Bochs.
void debug_hex(u64int number) {

  for (int i = 60; i >= 0; i -= 4) {
    u8int digit = (number >> i) & 0xF;
    if (digit >= 10) {
      debug_char(digit - 10 + 'A');
      }
    else {
      debug_char(digit + '0');
      }
    }

  }


// Nusiunčia šešioliktainį skaičių su paaiškinimu į Bochs.
void debug_value(const char *str, u64int number) {

  debug_string(str);
  debug_hex(number);
  debug_char('\n');
  }

// Pauzė. 
void pausep(u64int *pointer) {
  pause1((u64int) pointer);
  }
