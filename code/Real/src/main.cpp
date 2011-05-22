#include "cxx.h"
#include "icxxabi.h"
#include "types.h"
#include "cpu.h"
#include "primitives.h"
#include "structures/point.h"
#include "structures/array2dpointer.h"
#include "structures/rotating_queue.h"
#include "monitor_defines.h"
#include "monitor_screen_character.h"
#include "monitor.h"
#include "idt.h"
#include "pit.h"
#include "timer.h"
#include "keyboard.h"
#include "paging.h"
#include "debug.h"
#include "elf64.h"
#include "processes.h"
#include "message_resource_manager.h"
#include "resources.h"
#include "tests/test_monitor.h"
#include "tests/test_debug.h"
#include "tests/test_idt.h"
#include "tests/test_pit.h"

#define PAGERS 7


// Globalūs kintamieji.
Monitor monitor;
IDT idt(&monitor); 
PIT pit(50);
Timer timer(&monitor);
Keyboard kbd(&monitor);

KernelPager kernel_pager(0x00000000011fa000, 0x103000);
ProgramPager pager[PAGERS];

ProgramManager program_manager(0x400000, &kernel_pager);
ResourceManager resource_manager;
ProcessManager process_manager(&resource_manager, &program_manager);


CPUContext cpu;
u64int kernel_stack;

extern "C" void default_interrupt_handler(CPUContext *cpu_pointer){

  cpu = *cpu_pointer;

  debug_value("\nPertraukimas:", cpu.vector);

  // Išsaugoma proceso informacija.
  process_manager.save_state(cpu_pointer);
  // Atstatomas branduolio dėklas.
  asm volatile("mov %%rsp, %0": "=r"(kernel_stack));
  debug_value("\nBranduolio dėklas: ", kernel_stack);
  kernel_stack = process_manager.get_kernel_stack(kernel_stack);
  debug_value("\nAtnaujintas branduolio dėklas: ", kernel_stack);
  asm volatile("mov %0, %%rsp" : : "r"(kernel_stack));
  debug_ping();
  // Įjungiamas branduolio puslapiavimas.
  kernel_pager.activate();

  debug_string("veikia2\n");
  if (cpu.vector == 32) {
    timer.process_timer(&cpu);
    }
  else if (cpu.vector == 33) {
    kbd.process_keyboard(&cpu);
    }
  else if (cpu.vector == 0x3f) {
    process_manager.manage_interrupt(&cpu);
    }
  else {
    idt.process_interrupt(&cpu);
    }
  debug_string("veikia3\n");

  process_manager.plan();
  monitor.write_string("\n\nPlanuotojo klaida!\n");
}

extern "C" void load_gdt();

extern "C" int main() {

  load_gdt();

  // Aktyvuojam savo puslapiavimą.
  kernel_pager.activate();

  // Įjungiam laikroduką.
  enable_PIT(&pit);

  // Inicijuojam kitus puslapiavimo mechanizmus.
  for (int i = 0; i < PAGERS; i++) {
    pager[i].init(
        0x00000000012fa000 + i * 0x100000,
        0x0000000002000000 + i * 0x1000000,
        kernel_pager.get_entry(256));
    //pager[i].clear_lower();
    //pager[i].create_lower();
    }

  resource_manager.set_process_manager(&process_manager);
  resource_manager.init_memory_resource_manager(pager, PAGERS);
  debug_string("\nResourceManager inicializuotas.\n");

  // Pakraunamas procesas waitera.
  process_manager.load_process(1, 5, 1);
  debug_string("\nProcesas waitera pakrautas.\n");
  process_manager.load_process(2, 5, 2);
  debug_string("\nProcesas waiterb pakrautas.\n");

  // Testai.
  //test_debug();
  //test_monitor(&monitor);
  //test_idt();

  monitor.write_string("Penktas ekranas -- derinimo.\n");
  
  // Persijungiam į kitą procesą.
  process_manager.plan();

  //pager[3].activate();
  //debug_string("\nSveikas pasauli iš kito puslapiavimo!\n");
  //kernel_pager.activate();

  //program_manager.debug(&monitor);

  //u64int rez = program_manager.load(1, pager[1]);
  //kernel_pager.activate();
  //monitor.write_string("\nLoaded 1 into pager[1]: ");
  //monitor.write_hex(rez);

  return 0xBABADEAD;
  }
