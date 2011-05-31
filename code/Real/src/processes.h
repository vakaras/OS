#ifndef PROCESSES_H
#define PROCESSES_H 1

#include "types.h"
#include "debug.h"
#include "structures/rotating_queue.h"
#include "structures/priority_queue.h"
#include "resources.h"
#include "process.h"
#include "file_manager.h"
#include "elf64.h"

#define MAX_PROCESSES 0x10

#define CREATE_RESOURCE_MWA 20
#define CREATE_RESOURCE_MWB 30

#define GET_RESOURCE_MWA 31
#define GET_RESOURCE_MWB 21
#define GET_RESOURCE_LP 10
#define GET_RESOURCE_MEM 11

#define LOAD_PROCESS 12

#define GET_BYTE 5
#define PUT_BYTE 6

#define OPEN_FILE_READ 1
#define OPEN_FILE_WRITE 2
#define CLOSE_FILE 3


void print_service_message(int screen_id, char text);
void print_service_message(int screen_id, const char * text);
void print_service_message(int screen_id, u64int number, u8int size);
void print_service_message(int screen_id, u64int number);
void set_screen_type(u64int screen_id, bool type);


class ProcessManager {

private:

  // Atributai.

  ResourceManager *resource_manager;
  ProgramManager *program_manager;
  FileManager *file_manager;

  u64int running_process_id;            // Kuris procesas yra šiuo metu 
                                        // vykdomas?
                                        // Tikėtina, jog pasiruošusių 
                                        // vykdymui, procesų eilė.
  u64int kernel_stack;                  // Branduolio dėklo viršūnės 
                                        // adresas.

public:
  Process processes[MAX_PROCESSES];
  PriorityQueue<u64int> active_process_queue;
  

  // Metodai.

  ProcessManager(
      ResourceManager *resource_manager, ProgramManager *program_manager,
      FileManager *file_manager) {

    this->resource_manager = resource_manager;
    this->program_manager = program_manager;
    this->file_manager = file_manager;
    this->running_process_id = 0;       // Požymis, kad dar nei vienas iš 
                                        // procesų nebuvo vykdomas.
    
    }

  void manage_interrupt(CPUContext *cpu) {

    switch (cpu->AX) {
      case CREATE_RESOURCE_MWA:
        debug_value(
            "create_resource_a, process_id:", 
            this->running_process_id);
        this->resource_manager->create_resource(RESOURCE_MWA);
        break;
      case CREATE_RESOURCE_MWB:
        debug_value(
            "create_resource_b, process_id:", 
            this->running_process_id);
        this->resource_manager->create_resource(RESOURCE_MWB);
        break;
      case GET_RESOURCE_MWA:
        debug_value(
            "get_resource_a, process_id:", 
            this->running_process_id);
        this->resource_manager->get_resource(
            RESOURCE_MWA, this->running_process_id);
        break;
      case GET_RESOURCE_MWB:
        debug_value(
            "get_resource_b, process_id:", 
            this->running_process_id);
        this->resource_manager->get_resource(
            RESOURCE_MWB, this->running_process_id);
        break;
      case GET_RESOURCE_LP:
        debug_value(
            "get_resource_lp, process_id:", 
            this->running_process_id);
        this->resource_manager->get_resource(
            RESOURCE_LP, this->running_process_id);
        break;
      case GET_RESOURCE_MEM:
        debug_value(
            "get_resource_mem, process_id:", 
            this->running_process_id);
        this->resource_manager->get_resource(
            RESOURCE_MEM, this->running_process_id);
        pause();
        break;
      case LOAD_PROCESS:
        debug_value(
            "loading_process, process_id:", 
            this->running_process_id);
        this->load_process(cpu->DI, cpu->SI, cpu->DX);
        break;
      case GET_BYTE: {
        debug_value("get_byte, process_id:", this->running_process_id);
        u64int file_descriptor = cpu->BX;
        if (this->processes[this->running_process_id].opened(
              file_descriptor, FILE_MODE_READ)) {
          this->processes[this->running_process_id].read_byte(
              file_descriptor);
          }
        else {
          debug_value("Žudomas procesas: ", this->running_process_id);
          this->kill_process(this->running_process_id);
          }
        }
        break;
      case PUT_BYTE: {
        debug_value("put_byte, process_id:", this->running_process_id);
        u64int file_descriptor = cpu->BX;
        if (this->processes[this->running_process_id].opened(
              file_descriptor, FILE_MODE_WRITE)) {
          this->processes[this->running_process_id].write_byte(
              file_descriptor, (char) cpu->DI);
          }
        else {
          this->processes[this->running_process_id].dump_files();
          debug_value("file_descriptor: ", file_descriptor);
          debug_value("Žudomas procesas: ", this->running_process_id);
          this->kill_process(this->running_process_id);
          pause();
          }
        }
        break;
      case OPEN_FILE_WRITE: 
        this->open_file_write(this->running_process_id, cpu->BX);
        break;
      case OPEN_FILE_READ:
        this->open_file_read(this->running_process_id, cpu->BX);
        break;
      case CLOSE_FILE:
        this->close_file(this->running_process_id, cpu->BX);
        break;
      default:
        debug_value("Žudomas procesas: ", this->running_process_id);
        this->kill_process(this->running_process_id);
        pause();
        break;
      }

    }
  
  void kill_process_by_screen_id(u8int screen_id){
    
    for (u64int i = 0; i < MAX_PROCESSES; i++) {

      if (this->processes[i].is_existing() &&
          this->processes[i].get_screen_id() == screen_id) {

        this->kill_process(i);

        }

      }
    
    }

  void save_state(CPUContext *context) {

    this->processes[this->running_process_id].save(context);

    }

  void plan() {

    debug_string("\nVykdomas planuotojas. ");

    // Prieš persijungian į kitą procesą yra išsaugomas branduolio dėklo 
    // viršūnės adresas.
    asm volatile("mov %%rsp, %0": "=r"(this->kernel_stack));
    this->kernel_stack += 0x20;         // Kadangi planuotojo kvietimas 
                                        // neturi būti įsimintas… 
                                        // (Reikšmė nustatyta bandymų būdu.)

    this->_plan();

    }
  
  void _plan() {

    // Jei vykdomas procesas neužsiblokavo, tai jis pridedamas į 
    // aktyvių procesų sąrašą.
    if (this->processes[this->running_process_id].is_existing() && 
        !this->processes[this->running_process_id].is_blocked()) {
      this->active_process_queue.add(
          this->processes[this->running_process_id].get_priority(), 
          this->running_process_id);
      }

    debug_string("Procesų sąrašas:\n");
    for (int i = 0; i < 8; i++) {
      if (this->processes[i].is_existing()) {
        if (this->processes[i].is_blocked()) {
          debug_value("-\tblokuotas:     ----", i);
          }
        else {
          debug_value("+\taktyvus:       ++++", i);
          }
        }
      else {
        debug_value(" \tneegzistuojantis:  ", i);
        }
      }
    debug_value(
        "  Iš viso eilėje yra: ", 
        this->active_process_queue.get_size());

    // Surandamas neužblokuotas procesas ir paleidžiamas.
    while (!this->active_process_queue.is_empty()) {

      u64int process_id = this->active_process_queue.pop_front();
      if (this->processes[process_id].is_existing() && 
          !this->processes[process_id].is_blocked()) {

        debug_value("Planuotojas perjungia į procesą:", process_id);

        this->running_process_id = process_id;
        this->processes[process_id].run();
        
        }

      }

    debug_string("Procesų sąrašas tuščias – OS dar nestartavo?\n");

    // Jei čia pasiekėme turėtų kilti panika.
    }

  u64int get_kernel_stack(u64int stack) {

    if (this->running_process_id == 0) {
      return stack;
      }
    else {
      return this->kernel_stack;
      }
    
    }
  
  void block_process(u64int process_id) {

    if((process_id > 0)&&(process_id < MAX_PROCESSES)){
      if(!(this->processes[process_id].is_existing())){
        PANIC("Procesas neegzistuoja");
      } else if(this->processes[process_id].is_blocked()) {
        PANIC("Procesas jau yra blokuotas");
      }
    } else {
      debug_dec(process_id);
      debug_string("\n");
      PANIC("Blogas proceso ID.");
    }
    this->processes[process_id].block();

    }
  
  void activate_process(u64int process_id) {

    if ((process_id > 0) && (process_id < MAX_PROCESSES)) {
      if (!this->processes[process_id].is_existing()) {
        PANIC("Procesas neegzistuoja");
        } 
      else if (!this->processes[process_id].is_blocked()) {
        PANIC("Procesas nebuvo blokuotas");
        }
      } 
    else {
      debug_dec(process_id);
      debug_string("\n");
      PANIC("Blogas proceso ID.");
      }

    this->processes[process_id].unblock();
    this->active_process_queue.add(
        this->processes[process_id].get_priority(),
        process_id);

    }

  void kill_process(u64int process_id) {

    if((process_id > 0)&&(process_id < MAX_PROCESSES)){
      if(!(this->processes[process_id].is_existing())){
        PANIC("Procesas neegzistuoja");
      };
    } else {
      debug_string("proceso id: ");
      debug_dec(process_id);
      debug_string("\n");
      PANIC("Blogas proceso ID.");
    }

    MemoryResource memory_resource = \
      this->processes[process_id].get_memory_resource();
    u64int screen_id = this->processes[process_id].get_screen_id();

    for (u64int i = 2; i < MAX_FILES; i++) {
      if (this->processes[process_id].opened(i)) {
        this->close_file(process_id, i);
        }
      }
    
    this->processes[process_id] = Process();

    this->resource_manager->free_memory_resource(memory_resource);
    this->resource_manager->process_killed(process_id);
    
    if (screen_id < 5) {
      this->file_manager->reset_stdin(screen_id);
      set_screen_type(screen_id, false);
      print_service_message(screen_id, "Process ended.\n");
      }

    }

  void load_process(
      u64int program_id, u64int screen_id, u64int memory_resource_id, 
      u64int priority=10) {

    debug_string("\nPakraudinėjamas procesas.\n");

    MemoryResource memory_resource = \
      this->resource_manager->get_memory_resource_info(memory_resource_id);

    ProgramPager *pager = memory_resource.get_pager();

    debug_string("Puslapiavimo adresas (load_process): ");
    debug_hex(pager->get_entry_address());
    debug_string("\n");

    debug_string("Puslapiavimas gautas.\n");

    debug_string("Kraunama programa: ");
    debug_hex(program_id);
    debug_string("\n");

    u64int entry = this->load_program(program_id, pager);

    debug_string("Programa atmintyje.\n");

    // Susirandam laisvą įrašą.
    u64int process_id = 1;
    for (; (process_id < MAX_PROCESSES) && 
        this->processes[process_id].is_existing(); process_id++);

    this->processes[process_id] = Process(
        this, process_id, screen_id, memory_resource, entry, 
        this->file_manager, priority);

    this->active_process_queue.add(
        this->processes[process_id].get_priority(),
        process_id);

    if (screen_id < 5) {
      set_screen_type(screen_id, true);
      print_service_message(screen_id, "Process started.\n");
      }
    debug_string("Sukurtas procesas.\n");

    }

  u64int load_program(u64int program_id, ProgramPager *pager) {

    u64int entry;

    this->program_manager->load(program_id, *pager, &entry);
    
    return entry;
    }

  void print_process_state(CPUContext *context) {

    debug_string("\nCPU būsena.\n");
    debug_value("vector:", context->vector);
    debug_value("di:", context->DI);
    debug_value("si:", context->SI);
    debug_value("bp:", context->BP);
    debug_value("bx:", context->BX);
    debug_value("dx:", context->DX);
    debug_value("cx:", context->CX);
    debug_value("ax:", context->AX);
    debug_value("ip:", context->IP);
    debug_value("cs:", context->CS);
    debug_value("flags:", context->FLAGS);
    debug_value("sp:", context->SP);
    debug_value("ss:", context->SS);

    }

  void give_file(u64int process_id, u64int file_id, FILE_MODE mode) {

    if (!this->processes[process_id].is_existing()) {
      PANIC("Procesas jau negyvas!");
      }

    u64int file_descriptor = this->processes[process_id].add_file(
        file_id, mode);
    this->processes[process_id].set_value(file_descriptor);
    
    }

  void give_byte(u64int process_id, char symbol) {

    if (this->processes[process_id].is_existing()) {
      this->processes[process_id].set_value((u64int) symbol);
      }
    
    }

  void give_loader_task(
      u64int process_id, MessageLoadProgramResource resource) {

    this->processes[process_id].set_value(
        resource.get_program_id(), resource.get_screen_id());
    
    }

  void give_loader_memory(u64int process_id, MemoryResource resource) {

    this->processes[process_id].set_value(resource.get_id());
    
    }

  void char_entered(u64int screen_id, char symbol) {

    this->file_manager->give_stdin_byte(screen_id, symbol);

    }

  void open_file_write(u64int process_id, u64int file_name) {

    if (this->file_manager->exists(file_name)) {
      this->file_manager->get_file_write(file_name, process_id);
      }
    else {
      u64int file_id = this->file_manager->create_file_write(file_name);
      u64int file_descriptor = this->processes[process_id].add_file(
          file_id, FILE_MODE_WRITE);
      this->processes[process_id].set_value(file_descriptor);
      }

    }

  void open_file_read(u64int process_id, u64int file_name) {

    if (this->file_manager->exists(file_name)) {
      this->file_manager->get_file_read(file_name, process_id);
      }
    else {
      this->kill_process(process_id);
      }
    }

  void close_file(u64int process_id, u64int file_descriptor) {

    if (file_descriptor < 2 ||
        !this->processes[process_id].opened(file_descriptor)) {
      this->kill_process(process_id);
      }
    else {
      u64int file_id = this->processes[process_id].get_file_id(
          file_descriptor);
      this->processes[process_id].remove_file(file_descriptor);
      this->file_manager->free_file(file_id);
      }

    }

  };

#endif
