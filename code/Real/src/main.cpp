#include "cxx.h"
#include "icxxabi.h"
#include "types.h"
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


extern "C" void default_interrupt_handler(struct context_s *s){
  if (s->vector != 32) {
    debug_string("\nPertraukimas. ");
    debug_hex(s->vector);
    }

  kernel_pager.activate();              // FIXME: Padaryti normaliai.
  //process_manager.save_process_state(s);

  if(s->vector == 32){
    timer.process_timer(s);
  } else if(s->vector == 33){
    kbd.process_keyboard(s);
  } else {
    idt.process_interrupt(s);
  }
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

  // Testai.
  //test_debug();
  test_monitor(&monitor);
  //test_idt();
  
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
