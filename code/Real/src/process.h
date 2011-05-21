#ifndef PROCESS_H
#define PROCESS_H 1

#include "types.h"
#include "resource_types.h"


enum PROCESS_STATE {PROCESS_ACTIVE, PROCESS_BLOCKED};


class ProcessManager;


struct CPUContext { 
  u64int   vector, 
           R15, R14, R13, R12, 
           R11, R10, R9, R8, 
           DI, SI, BP, Temp,
           BX, DX, CX, AX,
           error, IP, CS,
           FLAGS, SP, SS;
  } __attribute__((packed));


class Process {


private:

  // Atributai.
  
  u64int id;
  bool exists;
  PROCESS_STATE state;
  u64int screen_id;
  MemoryResource memory_resource;
  ProcessManager *process_manager;

  u64int instruction_pointer;           // TODO: Sutvarkyti.

public:

  // Metodai.
  
  Process() {

    this->id = 0;
    this->state = PROCESS_BLOCKED;
    this->screen_id = 0;
    this->process_manager = 0;
    this->exists = false;

    this->instruction_pointer = 0;      // TODO: Sutvarkyti.

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

    this->instruction_pointer = entry;

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

    // TODO: Perdaryti.
    jump(this->instruction_pointer);
      
    }

  };

#endif
