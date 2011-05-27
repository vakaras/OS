#ifndef PROCESS_H
#define PROCESS_H 1

#include "types.h"
#include "resource_types.h"
#include "cpu.h"
#include "memlib.h"
#include "file_manager.h"


enum PROCESS_STATE {PROCESS_ACTIVE, PROCESS_BLOCKED};


class ProcessManager;


extern "C" void switch_process(u64int stack_address);


struct OpenedFile {

  // Atributai.

  bool opened;
  u64int file_id;
  FILE_MODE mode;
  
  // Metodai.

  OpenedFile() {
    this->opened = false;
    this->file_id = 0;
    }
  
  OpenedFile(u64int file_id, FILE_MODE mode) {
    this->opened = true;
    this->file_id = file_id;
    this->mode = mode;
    }
  
  };


class Process {

private:

  // Atributai.
  
  u64int id;
  bool exists;
  PROCESS_STATE state;
  u64int screen_id;
  MemoryResource memory_resource;
  ProcessManager *process_manager;
  FileManager *file_manager;

  CPUContext cpu;
  u64int stack;                         // Dėklo viršūnės adresas.

  OpenedFile files[MAX_FILES];

public:

  // Metodai.
  
  Process() {

    this->id = 0;
    this->state = PROCESS_BLOCKED;
    this->screen_id = 0;
    this->process_manager = 0;
    this->exists = false;
    this->file_manager = 0;

    }

  Process(
      ProcessManager *process_manager, 
      u64int process_id,
      u64int screen_id, 
      MemoryResource memory_resource,
      u64int entry,
      FileManager *file_manager) {

    this->id = process_id;
    this->state = PROCESS_ACTIVE;
    this->screen_id = screen_id;
    this->memory_resource = memory_resource;
    this->process_manager = process_manager;
    this->file_manager = file_manager;
    this->file_manager->reset_stdin(this->screen_id);
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
  
  void set_value(u64int value) {
    this->cpu.AX = value;
    }
  
  void set_value(u64int value1, u64int value2) {
    this->cpu.AX = value1;
    this->cpu.BX = value2;
    }
  
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

  bool opened(u64int file_descriptor, FILE_MODE mode) {

    if (file_descriptor >= MAX_FILES) {
      return false;
      }
    else if (file_descriptor == 0 && mode == FILE_MODE_READ) {
      return true;
      }
    else if ((file_descriptor == 1 && mode == FILE_MODE_WRITE)) {
      return true;
      }
    else {

      if (this->files[file_descriptor].mode == mode) {
        return true;
        }
      else {
        return false;
        }
        
      }

    // TODO: Realizuoti.
    }

  void read_byte(u64int file_descriptor) {

    if (file_descriptor == 0) {
      this->file_manager->get_stdin_byte(this->screen_id, this->id);
      }
    else {
      // TODO: Realizuoti.
      }
    
    }

  void write_byte(u64int file_descriptor, char symbol) {

    if (file_descriptor == 1) {
      this->file_manager->write_stdout_byte(this->screen_id, symbol);
      }
    else {
      this->file_manager->write_file_byte(
          this->files[file_descriptor].file_id, symbol);
      }
    
    }

  u64int add_file(u64int file_id, FILE_MODE mode) {

    for (u64int i = 2; i < MAX_FILES; i++) {
      if (!this->files[i].opened) {
        this->files[i] = OpenedFile(file_id, mode);
        return i;
        }
      }

    PANIC("Nepavyko atidaryti failo: nebėra vietos.");
    return 0;
    }

  void dump_files() {

    debug_string("Failų sąrašas:\n");
    for (u64int i = 0; i < MAX_FILES; i++) {
      debug_string("deskriptorius: ");
      debug_hex(i);
      if (this->files[i].opened) {
        if (this->files[i].mode == FILE_MODE_READ) {
          debug_string("\tskaitymui id:\t");
          debug_hex(this->files[i].file_id);
          }
        else {
          debug_string("\trašymui   id:\t");
          debug_hex(this->files[i].file_id);
          }
        }
      else {
        debug_string("neatidarytas");
        }
      debug_string("\n");
      }
    
    }
  

  };

#endif
