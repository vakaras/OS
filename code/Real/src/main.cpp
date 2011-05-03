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
#include "tests/test_idt.h"

#define PAGE_TABLE_PERM ((1<<0) | (1<<1))
#define PAGE_TABLE_PERM_NOCACHE (PAGE_TABLE_PERM | (1<<3) | (1<<4))
#define PAGE_TABLE_LARGE_FLAG (1<<7)
#define PTINV 0xFFFFFFFFFFFFF000

// Globalūs kintamieji.
Monitor monitor;
IDT idt;


extern "C" int main() {

  // Testai.
  test_debug();
  test_monitor(&monitor);
  test_idt();

  // Žaidimas su puslapiavimo mechanizmu.
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

  return 0xBABADEAD;
  }
