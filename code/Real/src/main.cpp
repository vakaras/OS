#include "cxx.h"
#include "icxxabi.h"
#include "types.h"
#include "primitives.h"
#include "structures/point.h"
#include "structures/array2dpointer.h"
#include "monitor_defines.h"
#include "monitor_screen_character.h"
#include "monitor.h"
#include "idt.h"
#include "paging.h"
#include "debug.h"
#include "tests/test_monitor.h"
#include "tests/test_debug.h"

#define PAGE_TABLE_PERM ((1<<0) | (1<<1))
#define PAGE_TABLE_PERM_NOCACHE (PAGE_TABLE_PERM | (1<<3) | (1<<4))
#define PAGE_TABLE_LARGE_FLAG (1<<7)
#define PTINV 0xFFFFFFFFFFFFF000

// Globalūs kintamieji.
Monitor monitor;
IDT idt;

extern "C" void interrupt_handler(int *stack) {

  monitor.write_hex((u64int) stack);

  }

extern "C" void load_pager(u64int);

extern "C" int main() {

  test_debug();

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
  //initialise_paging();

  //u64int *pml = (u64int *) 0x0000000000103000;
  //u64int *pdp1 = (u64int *) (pml[0] & PTINV);
  //u64int *pd1 = (u64int *) (pdp1[0] & PTINV);

  //monitor.clear();

  //monitor.write_string("PML[");
  //monitor.write_dec(0);
  //monitor.write_string("] \t= ");
  //monitor.write_hex(pml[0]);
  //monitor.write_string("\n");

  //monitor.write_string("PDP[");
  //monitor.write_dec(0);
  //monitor.write_string("] \t= ");
  //monitor.write_hex(pdp1[0]);
  //monitor.write_string("\n");

  //for (int i = 0; i < 10; i++) {
    //monitor.write_string("PD[");
    //monitor.write_dec(i);
    //monitor.write_string("] \t= ");
    //monitor.write_hex(pd1[i]);
    //monitor.write_string("\n");
    //}

    
  //u64int *p2 = (u64int *) 0xFFFF800000000000;
  //for (int i = 0; i < 100; i++) {
    //p2[i] = 0x0;
    //}

  //u8int *p3 = (u8int *) 0xFFFF800000000000;
  //p3[0] = 0xAA;

  //u64int *p = (u64int *) &idt;
  //for (int i = 0; i < 10; i++) {
    //p[i] = 0xDEADBABA + (i * 0x100000000);
    //}
  //pause1((u64int) &idt);
  
  // Atspausdina PML reikšmes.
//   for (int i = 0; i < 10; i++) {
//     monitor.write_string("PML4[");
//     monitor.write_dec(i);
//     monitor.write_string("] \t= ");
//     monitor.write_hex(pml4[i]);
//     monitor.write_string("\n");
//     }
//   for (int i = 250; i < 260; i++) {
//     monitor.write_string("PML4[");
//     monitor.write_dec(i);
//     monitor.write_string("] \t= ");
//     monitor.write_hex(pml4[i]);
//     monitor.write_string("\n");
//     }
// 
//   monitor.write_hex((u64int) pml4);
//   monitor.write_string(" = ");
//   monitor.write_hex(*pml4);
// 
//   load_pager(0xFFFFFFFFFF103000);
//   load_pager(0x0000000000103000);

  //test_monitor(&monitor);
  //gdt.print_debug_info(&monitor);

  //asm volatile ("int $0x3");
  
  monitor.write_string("IDT address:         \t");
  monitor.write_hex((u64int) &idt);
  monitor.write_string("\nFIXED IDT address: \t");
  monitor.write_hex(FIX_ADDRESS((u64int) &idt));

  //gdt.print_debug_info(&monitor);
  //idt.print_debug_info(&monitor);
  u64int a, b;
  a = 23;
  b = 0;
  b = a + b + 1;
  idt.print_debug_info(&monitor);
  //u64int a, b;
  //a = 23;
  //b = 0;
  //b = a + b + 1;

  pause();
  asm volatile("int $0x4;");

  return 0xBABADEAD;
  }
