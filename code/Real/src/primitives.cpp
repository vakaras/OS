#include "primitives.h"


// Išsiunčia baitą į nurodytą portą.
void send_byte(u16int port, u8int value) {
  asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
  }


// Nuskaito baitą iš nurodyto porto.
u8int get_byte(u16int port) {
  u8int result;
  asm volatile("inb %1, %0" : "=a" (result) : "dN" (port));
  return result;
  }


// Nuskaito baitų porą iš nurodyto porto.
u16int get_bytes2(u16int port) {
  u16int result;
  asm volatile("inw %1, %0" : "=a" (result) : "dN" (port));
  return result;
  }


// Nukopijuoja nurodytus len baitų iš src į dest.
void memcpy(u8int *dest, const u8int *src, u32int length) {

  for (u32int i = 0; i < length; i++) {
    dest[i] = src[i];
    }
  
  }


// Užpildo nurodytą atmintį reikšme value.
void memset(u8int *dest, u8int value, u32int length) {

  for (u32int i = 0; i < length; i++) {
    dest[i] = value;
    }

  }
