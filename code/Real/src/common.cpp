#include "common.h"

void send_byte(u16int port, u8int value) {
  asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
  }


u8int get_byte(u16int port) {
  u8int result;
  asm volatile("inb %1, %0" : "=a" (result) : "dN" (port));
  return result;
  }

u16int get_bytes2(u16int port) {
  u16int result;
  asm volatile("inw %1, %0" : "=a" (result) : "dN" (port));
  return result;
  }
