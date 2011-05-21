#ifndef MESSAGE_RESOURCE_MANAGER
#define MESSAGE_RESOURCE_MANAGER 1


#include "types.h"
#include "structures/rotating_queue.h"


class ResourceManager;


template <class T> class MessageResourceManager {

private:

// Atributai.

  ResourceManager *resource_manager;

  RotatingQueue<T> resource_queue;
  RotatingQueue<u64int> process_queue;

// Metodai.

  /** 
   * Prideda procesą į resurso laukiančių procesų eilę ir
   * užblokuoja.
   */
  void add_process_to_waiting_queue(u64int process_id) {

    this->process_queue.push_back(process_id);
    this->resource_manager->block_process(process_id);

    }

public:

  MessageResourceManager(ResourceManager *resource_manager) {
    this->resource_manager = resource_manager;
    }

  void create_resource(T resource) {
    
    this->resource_queue.push_back(resource);
    this->plan();

    }

  void get_resource(u64int process_id) {

    this->add_process_to_waiting_queue(process_id);
    this->plan();
    
    }
  
  /**
   * Resursų planuotojas – resursus procesams skirstanti funkcija.
   */
  void plan() {

    while ((!this->resource_queue.is_empty()) 
        && (!this->process_queue.is_empty())) {

      T resource = this->resource_queue.pop_front();
      u64int process_id = this->process_queue.pop_front();

      this->resource_manager->give_resource(process_id, resource);
      this->resource_manager->activate_process(process_id);

      }
    

    }
  
  
  };

#endif
