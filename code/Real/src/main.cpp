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

#define PAGE_TABLE_PERM ((1<<0) | (1<<1))
#define PAGE_TABLE_PERM_NOCACHE (PAGE_TABLE_PERM | (1<<3) | (1<<4))
#define PAGE_TABLE_LARGE_FLAG (1<<7)
#define PTINV 0xFFFFFFFFFFFFF000

// Globalūs kintamieji.
Monitor monitor;
GDT gdt;
IDT idt;

extern "C" void interrupt_handler(int *stack) {

  monitor.write_hex((u64int) stack);

  }

extern "C" void load_pager(u64int);

extern "C" int main() {

  u64int *pml = (u64int *) 0x0000000000103000;
  u64int *pdp1 = (u64int *) (pml[0] & PTINV);
  u64int *pd1 = (u64int *) (pdp1[0] & PTINV);

  monitor.clear();

  monitor.write_string("PML[");
  monitor.write_dec(0);
  monitor.write_string("] \t= ");
  monitor.write_hex(pml[0]);
  monitor.write_string("\n");

  monitor.write_string("PDP[");
  monitor.write_dec(0);
  monitor.write_string("] \t= ");
  monitor.write_hex(pdp1[0]);
  monitor.write_string("\n");

  for (int i = 0; i < 10; i++) {
    monitor.write_string("PD[");
    monitor.write_dec(i);
    monitor.write_string("] \t= ");
    monitor.write_hex(pd1[i]);
    monitor.write_string("\n");
    }
    
  // Atspausdina PML reikšmes.
  //for (int i = 0; i < 10; i++) {
    //monitor.write_string("PML4[");
    //monitor.write_dec(i);
    //monitor.write_string("] \t= ");
    //monitor.write_hex(pml4[i]);
    //monitor.write_string("\n");
    //}
  //for (int i = 250; i < 260; i++) {
    //monitor.write_string("PML4[");
    //monitor.write_dec(i);
    //monitor.write_string("] \t= ");
    //monitor.write_hex(pml4[i]);
    //monitor.write_string("\n");
    //}

  //monitor.write_hex((u64int) pml4);
  //monitor.write_string(" = ");
  //monitor.write_hex(*pml4);

  //load_pager(0xFFFFFFFFFF103000);
  //load_pager(0x0000000000103000);

  //test_monitor(&monitor);
  //gdt.print_debug_info(&monitor);

  //asm volatile ("int $0x3");


  return 0xBABADEAD;
  }
