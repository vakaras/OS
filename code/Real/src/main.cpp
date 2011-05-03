#include "cxx.h"
#include "icxxabi.h"
#include "types.h"
#include "primitives.h"
#include "structures/point.h"
#include "structures/array2dpointer.h"
#include "monitor_defines.h"
#include "monitor_screen_character.h"
#include "monitor.h"
#include "gdt.h"
#include "idt.h"
// #include "pic.h"
#include "tests/test_monitor.h"


// Globalūs kintamieji.
Monitor monitor;
GDT gdt;
IDT idt;

extern "C" int main() {
//   InitInterrupts();
  //test_monitor(&monitor);
  gdt.print_debug_info(&monitor);
  idt.print_debug_info(&monitor);
  u64int a, b;
  a = 23;
  b = 0;
  b = a + b + 1;
  return 0xBABADEAD;
  }
