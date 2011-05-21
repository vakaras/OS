#ifndef PROCESSES_H
#define PROCESSES_H 1

#include "types.h"
#include "debug.h"
#include "structures/rotating_queue.h"
#include "resources.h"
#include "process.h"
#include "elf64.h"

#define MAX_PROCESSES 0x10


class ProcessManager {

private:

  // Atributai.

  Process processes[MAX_PROCESSES];
  ResourceManager *resource_manager;
  ProgramManager *program_manager;

  u64int running_process_id;            // Kuris procesas yra šiuo metu 
                                        // vykdomas?
  RotatingQueue<u64int> active_process_queue;
                                        // Tikėtina, jog pasiruošusių 
                                        // vykdymui, procesų eilė.

public:

  // Metodai.

  ProcessManager(
      ResourceManager *resource_manager, ProgramManager *program_manager) {

    this->resource_manager = resource_manager;
    this->program_manager = program_manager;
    
    }

  void manage_interrupt() {
    // TODO: Realizuoti.
    // TODO: Jei procesas egzistuoja ir nėra užblokuotas, tai pridėti jį
    // į aktivių procesų sąrašą prieš iškviečiant planuotoją.
    }
  
  void plan() {

    while (!this->active_process_queue.is_empty()) {

      u64int process_id = this->active_process_queue.pop_front();
      if (this->processes[process_id].is_existing() && 
          !this->processes[process_id].is_blocked()) {

        this->processes[process_id].run();
        
        }

      }

    // Jei čia pasiekėme turėtų kilti panika.
    }
  
  void block_process(u64int process_id) {

    // FIXME: Patikrinti ar procesas su tokiu process_id egzistuoja ir nėra
    // užblokuotas.

    this->processes[process_id].block();

    }
  
  void activate_process(u64int process_id) {

    // FIXME: Patikrinti ar procesas su tokiu process_id egzistuoja ir yra
    // užblokuotas.

    this->processes[process_id].unblock();

    }

  void load_process(
      u64int program_id, u64int screen_id, u64int memory_resource_id) {

    debug_string("\nPakraudinėjamas procesas.\n");

    MemoryResource memory_resource = \
      this->resource_manager->get_memory_resource_info(memory_resource_id);

    ProgramPager *pager = memory_resource.get_pager();

    debug_string("Puslapiavimo adresas (load_process): ");
    debug_hex((u64int) pager->entry);
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
        this, process_id, screen_id, memory_resource, entry);

    this->active_process_queue.push_back(process_id);

    debug_string("Sukurtas procesas.\n");

    }

  u64int load_program(u64int program_id, ProgramPager *pager) {

    u64int entry;

    this->program_manager->load(program_id, *pager, &entry);
    
    return entry;
    }

  void save_process_state(CPUContext *context) {

    debug_string("\nCPU būsena.\n");
    debug_value("vector:", context->vector);
    debug_value("ip:", context->IP);
    debug_value("ax:", context->AX);

    }

  };

#endif
