#ifndef PROCESS_H
#define PROCESS_H 1

#include "types.h"
#include "resource_types.h"
#include "cpu.h"
#include "memlib.h"


enum PROCESS_STATE {PROCESS_ACTIVE, PROCESS_BLOCKED};


class ProcessManager;


extern "C" void switch_process(u64int stack_address);


class Process {


private:

  // Atributai.
  
  u64int id;
  bool exists;
  PROCESS_STATE state;
  u64int screen_id;
  MemoryResource memory_resource;
  ProcessManager *process_manager;

  CPUContext cpu;
  u64int stack;                         // Dėklo viršūnės adresas.

public:

  // Metodai.
  
  Process() {

    this->id = 0;
    this->state = PROCESS_BLOCKED;
    this->screen_id = 0;
    this->process_manager = 0;
    this->exists = false;

    }

  Process(
      ProcessManager *process_manager, 
      u64int process_id,
      u64int screen_id, 
      MemoryResource memory_resource,
      u64int entry) {

    this->id = process_id;
    this->state = PROCESS_ACTIVE;
    this->screen_id = screen_id;
    this->memory_resource = memory_resource;
    this->process_manager = process_manager;
    this->exists = true;

    this->stack = entry + 0x100000;     // Pradinė dėklo pozicija.

    this->cpu.IP = entry;
    this->cpu.CS = 0x8;
    this->cpu.FLAGS = 0x210292;
    this->cpu.SP = this->stack;
    this->cpu.SS = 0x10;

    }

  u64int get_id() {
    return this->id;
    }

  bool is_existing() {
    return this->exists;
    }
  
  //void set_value(u64int value) {
    //// TODO: Realizuoti.
    //}
  
  //void set_value(u64int value1, u64int value2) {
    //// TODO: Realizuoti.
    //}
  
  PROCESS_STATE get_state() {
    return this->state;
    }
  
  u64int get_screen_id() {
    return this->screen_id;
    }
  
  MemoryResource get_memory_resource() {
    return this->memory_resource;
    }
  
  void block() {
    // TODO: Patikrinti ar būsena yra aktyvus.
    
    this->state = PROCESS_BLOCKED;

    }
  
  void unblock() {
    // TODO: Patikrinti ar būsena yra blokuotas.

    this->state = PROCESS_ACTIVE;

    }

  bool is_blocked() {

    return this->state == PROCESS_BLOCKED;

    }

  void run() {

    ProgramPager *pager = this->memory_resource.get_pager();
    pager->activate();

    CPUContext *cpu = (CPUContext *) this->stack;
    *cpu = this->cpu;

    switch_process(this->stack);
    }

  void save(CPUContext *context) {

    this->cpu = *context;
    this->stack = (u64int) context;

    }

  };

#endif
