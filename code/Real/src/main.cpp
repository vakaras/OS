#include "cxx.h"
#include "icxxabi.h"
#include "types.h"
#include "cpu.h"
#include "primitives.h"
#include "structures/point.h"
#include "structures/array2dpointer.h"
#include "structures/rotating_queue.h"
#include "structures/priority_queue.h"
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
FileManager file_manager;
ProcessManager process_manager(
    &resource_manager, &program_manager, &file_manager);
Keyboard kbd(
    &monitor,&process_manager, &program_manager, &resource_manager);



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
    PANIC("Planuotojo klaida: Nera vykdymui skirto proceso!");
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

void set_screen_type(u64int screen_id, bool type) {
  monitor.set_screen_type((u32int) screen_id, type);
  }

extern "C" void load_gdt();

extern "C" void activate_kernel_pager() {
  kernel_pager.activate();
  }

extern "C" int main() {

  //monitor.set_proc_m(&process_manager);
  
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

  file_manager.set_process_manager(&process_manager);

  debug_string("\nFileManager inicializuotas.\n");

  // Pakraunami servisai.
  process_manager.load_process(3, 5, 0, 15);
  debug_string("\nProcesas loader pakrautas.\n");
  process_manager.load_process(1, 5, 1, 5);
  debug_string("\nProcesas waitera pakrautas.\n");
  process_manager.load_process(2, 5, 2, 5);
  debug_string("\nProcesas waiterb pakrautas.\n");

  // Testai.
  //test_debug();
  //test_monitor(&monitor);
  //test_idt();

  MessageLoadProgramResource resource(5, 1);
  resource_manager.add_resource(resource);
  monitor.activate_screen(1);

  multiprogramming_enabled = true;
  
  // Persijungiam į kitą procesą.
  process_manager.plan();

  return 0xBABADEAD;
  }
  
void update_processes_info(){
  monitor.clear();
  monitor.write_string("Procesu sarasas:\n");
  for (int i = 0; i < 8; i++) {
    if (process_manager.processes[i].is_existing()) {
      if (process_manager.processes[i].is_blocked()) {
        monitor.write_string("\n-\tblokuotas:     ----");
        monitor.write_hex(i);
      }
      else {
        monitor.write_string("\n+\taktyvus:       ++++");
        monitor.write_hex(i);
      }
    }
    else {
      monitor.write_string("\n \tneegzistuojantis:  ");
      monitor.write_hex(i);
    }
  }
  monitor.write_string("\n  Is viso eileje yra: ");
  monitor.write_hex(process_manager.active_process_queue.get_size());
  
  monitor.write_string("\n\nProcesoriaus busena:\n");
  monitor.write_value("rax: ",cpu.AX);
  monitor.write_value(" rbx: ",cpu.BX);
  monitor.write_value(" rcx: ",cpu.CX);
  monitor.write_value("\nrdx: ",cpu.DX);
  monitor.write_value(" rsp: ",cpu.SP);
  monitor.write_value(" rbp: ",cpu.BP);
  monitor.write_value("\nrsi: ",cpu.SI);
  monitor.write_value(" rdi: ",cpu.DI);
  monitor.write_value(" rip: ",cpu.IP);
  monitor.write_value("\nr8 : ",cpu.R8);
  monitor.write_value(" r9 : ",cpu.R9);
  monitor.write_value(" r10: ",cpu.R10);
  monitor.write_value("\nr11: ",cpu.R11);
  monitor.write_value(" r12: ",cpu.R12);
  monitor.write_value(" r13: ",cpu.R13);
  monitor.write_value("\nr14: ",cpu.R14);
  monitor.write_value(" r15: ",cpu.R15);
//     monitor.write_string(" r10: ",cpu.R10);
  }
