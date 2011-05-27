#ifndef REUSABLE_RESOURCE_MANAGER_H
#define REUSABLE_RESOURCE_MANAGER_H 1

#include "types.h"
#include "debug.h"
#include "structures/rotating_queue.h"

#define MAX_REUSABLE_RESOURCES 10


class ResourceManager;


template <class T> class ReusableResourceManager {

private:

// Atributai.

  ResourceManager *resource_manager;

  RotatingQueue<u64int> process_queue;
  T resources[MAX_REUSABLE_RESOURCES];

// Metodai.

  void add_process_to_waiting_queue(u64int process_id) {

    this->process_queue.push_back(process_id);
    this->resource_manager->block_process(process_id);

    }

public:

  ReusableResourceManager(ResourceManager *resource_manager) {

    this->resource_manager = resource_manager;

    }

  void create_resource(T resource) {

    for (int i = 0; i < MAX_REUSABLE_RESOURCES; i++) {
      if (this->resources[i].is_empty_slot()) {
        this->resources[i] = resource;
        this->resources[i].set_id(i);
        return;
        }
      
      }

    PANIC("Nepavyko sukurti resurso");
    }

  void get_resource(u64int process_id) {

    this->add_process_to_waiting_queue(process_id);

    debug_string("Procesas pridėtas.\n");
    debug_value("\tDabar yra resursų: ", this->get_resources_number());
    debug_value("\tIš jų laisvi:      ", this->get_available_number());
    debug_value("\tLaukia procesų:    ", this->process_queue.get_size());

    this->plan();

    }

  void free_resource(T resource) {

    u64int id = resource.get_id();
    
    if(id >= MAX_REUSABLE_RESOURCES) {
      PANIC("Meginama atlaisvniti negzistuojanti resursa");
    }
    
    this->resources[id].set_free(true);

    this->plan();

    }

  T get_resource_info(u64int resource_id) {

    if(resource_id >= MAX_REUSABLE_RESOURCES) {
      PANIC("Meginama gauti negzistuojancio resurso info");
    }
    
    return this->resources[resource_id];
    }

  void plan() {

    debug_string("Vykdomas resursų planuotojas.\n");

    while (!this->process_queue.is_empty()) {

      bool found = false;

      for (u64int i = 0; i < MAX_REUSABLE_RESOURCES; i++) {

        if (this->resources[i].is_free() && 
            !this->resources[i].is_empty_slot()) {

          u64int process_id = this->process_queue.pop_front();
          if (process_id == 0) {
            debug_string("\tProcesas jau nebegyvas.\n");
            }
          else {
            this->resources[i].set_free(false);

            this->resource_manager->give_resource(
                process_id, this->resources[i]);
            this->resource_manager->activate_process(process_id);

            debug_value("\tResursas: ", this->resources[i].get_id());
            debug_value("\tAtiduotas procesui: ", process_id);
            }
          
          found = true;
          break;
          }

        }

      if (!found) {
        break;
        }
      
      }
    
    }

  u64int get_available_number() {

    u64int number = 0;

    for (u64int i = 0; i < MAX_REUSABLE_RESOURCES; i++) {
      if (this->resources[i].is_free() && 
          !this->resources[i].is_empty_slot()) {
        number++;
        }
      }

    return number;
    }

  u64int get_resources_number() {

    u64int number = 0;

    for (u64int i = 0; i < MAX_REUSABLE_RESOURCES; i++) {
      if (!this->resources[i].is_empty_slot()) {
        number++;
        }
      }

    return number;
    }

  void process_killed(u64int process_id) {

    this->process_queue.replace(process_id, 0);

    }

  };

#endif
