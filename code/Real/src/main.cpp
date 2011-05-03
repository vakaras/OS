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
#include "tests/test_monitor.h"


// Globalūs kintamieji.
Monitor monitor;
GDT gdt;
IDT idt;

extern "C" void interrupt_handler(int *stack) {

  monitor.write_hex((u64int) stack);

  }

extern "C" int main() {

  //test_monitor(&monitor);
  gdt.print_debug_info(&monitor);

  return 0xBABADEAD;
  }
