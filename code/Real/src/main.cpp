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

s64int active_pager;                    // Kuris puslapiavimo mechanizmas
                                        // yra dabar aktyvus.
                                        // (-1 – branduolio.)
KernelPager kernel_pager(0x00000000011fa000, 0x103000, &active_pager);
ProgramPager pager[PAGERS];

ProgramManager program_manager(0x400000, &kernel_pager);
ResourceManager resource_manager;
ProcessManager process_manager(&resource_manager, &program_manager);
Keyboard kbd(&monitor,&process_manager, &program_manager, &resource_manager);



bool multiprogramming_enabled;          // Ar jau OS pilnai startavo ir
                                        // galima persijunginėti į kitus 
                                        // procesus.

CPUContext cpu;
u64int kernel_stack;

extern "C" void default_interrupt_handler(CPUContext *cpu_pointer){

  cpu = *cpu_pointer;

  debug_value("\nPertraukimas:", cpu.vector);

  if (cpu.vector == 0xe) {
    debug_value("active_pager=", active_pager);
    }

  if (!IS_HIGHER_HALF(cpu.IP)) {
    // Išsaugoma proceso informacija.
    process_manager.save_state(cpu_pointer);
    // Atstatomas branduolio dėklas.
    asm volatile("mov %%rsp, %0": "=r"(kernel_stack));
    debug_value("\nBranduolio dėklas: ", kernel_stack);
    kernel_stack = process_manager.get_kernel_stack(kernel_stack);
    debug_value("\nAtnaujintas branduolio dėklas: ", kernel_stack);
    asm volatile("mov %0, %%rsp" : : "r"(kernel_stack));
    debug_ping();
    }

  s64int old_pager = active_pager;
  if (active_pager != -1) {
    // Įjungiamas branduolio puslapiavimas.
    kernel_pager.activate();
    debug_value("Įjungtas branduolio puslapiavimas. Senas: ", old_pager);
    }

  debug_string("veikia2\n");
  if (cpu.vector == 32) {
    timer.process_timer(&cpu);
    }
  else if (cpu.vector == 33) {
    kbd.process_keyboard(&cpu);
    }
  else if (cpu.vector == 0x3e) {
    monitor.write_string("\nKLAVA!\n");
    }
  else if (cpu.vector == 0x3f) {
    process_manager.manage_interrupt(&cpu);
    }
  else {
    idt.process_interrupt(&cpu);
    }
  debug_string("veikia3\n");

  if (!IS_HIGHER_HALF(cpu.IP)) {
    process_manager.plan();
    monitor.write_string("\n\nPlanuotojo klaida!\n");
    }

  if (old_pager != -1) {
    // Įjungiamas senas puslapiavimas.
    pager[old_pager].activate();
    debug_value("Įjungtas puslapiavimas: ", active_pager);
    }

  }
  
  void print_service_message(int screen_id, char text) {
    monitor.print_char_message(screen_id, text, COLOR_WHITE);
  }
  
  void print_program_message(int screen_id, char text) {
    monitor.print_char_message(screen_id, text, COLOR_GREEN);
  }
  
  void print_service_message(int screen_id, const char * text) {
    monitor.print_string_message(screen_id, text, COLOR_WHITE);
  }
  
  void print_program_message(int screen_id, const char * text) {
    monitor.print_string_message(screen_id, text, COLOR_GREEN);
  }
  
  void print_service_message(int screen_id, u64int number, u8int size) {
    monitor.print_hex_message(screen_id, number, size, COLOR_WHITE);
  }
  
  void print_program_message(int screen_id, u64int number, u8int size) {
    monitor.print_hex_message(screen_id, number, size, COLOR_GREEN);
  }
  
  void print_service_message(int screen_id, u64int number) {
    monitor.print_dec_message(screen_id, number, COLOR_WHITE);
  }
  
  void print_program_message(int screen_id, u64int number) {
    monitor.print_dec_message(screen_id, number, COLOR_GREEN);
  }

extern "C" void load_gdt();

extern "C" int main() {

  multiprogramming_enabled = false;

  load_gdt();

  // Aktyvuojam savo puslapiavimą.
  kernel_pager.activate();

  // Įjungiam laikroduką.
  pit.init_pit();

  // Inicijuojam kitus puslapiavimo mechanizmus.
  for (int i = 0; i < PAGERS; i++) {
    pager[i].init(
        0x00000000012fa000 + i * 0x100000,
        0x0000000002000000 + i * 0x1000000,
        kernel_pager.get_entry(256),
        &active_pager,
        i);
    //pager[i].clear_lower();
    //pager[i].create_lower();
    }

  resource_manager.set_process_manager(&process_manager);
  resource_manager.add_resource(MemoryResource(0, &pager[0], false));
                                        // loader
  resource_manager.add_resource(MemoryResource(1, &pager[1], false));
                                        // waitera
  resource_manager.add_resource(MemoryResource(2, &pager[2], false));
                                        // waiterb
  resource_manager.add_resource(MemoryResource(3, &pager[3], false));
                                        // hello
  resource_manager.add_resource(MemoryResource(4, &pager[4]));
  resource_manager.add_resource(MemoryResource(5, &pager[5]));
  resource_manager.add_resource(MemoryResource(6, &pager[6]));

  debug_string("\nResourceManager inicializuotas.\n");

  // Pakraunami servisai.
  process_manager.load_process(3, 5, 0);
  debug_string("\nProcesas loader pakrautas.\n");
  process_manager.load_process(1, 5, 1);
  debug_string("\nProcesas waitera pakrautas.\n");
  process_manager.load_process(2, 5, 2);
  debug_string("\nProcesas waiterb pakrautas.\n");

  pause();

  // Pakraunami bandyminiai procesai.
  process_manager.load_process(4, 1, 3);
  debug_string("\nPrograma hello pakrauta.\n");
  pause();

  // Testai.
  //test_debug();
  //test_monitor(&monitor);
  //test_idt();

  MessageLoadProgramResource resource(4, 2);
  resource_manager.add_resource(resource);

  monitor.write_string("Penktas ekranas -- derinimo.\n");

  multiprogramming_enabled = true;
  print_service_message(2, "labas pasauli nr2.\n");
  print_program_message(2, "as irgi cia\n\n");
  print_service_message(5, "labas pasauli nr5.\n");
  print_program_message(5, "as irgi cia5\n\n");
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
