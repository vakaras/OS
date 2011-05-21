#ifndef RESOURCES_H
#define RESOURCES_H 1

#include "types.h"
#include "resource_types.h"
#include "message_resource_manager.h"

#define RESOURCE_MWA 1
#define RESOURCE_MWB 2

#define RETURN_CODE_OK 0
#define RETURN_CODE_UKNOWN_RESOURCE_TYPE 1


class ProcessManager;


class ResourceManager {

private:

  // Atributai.

  MessageResourceManager<MessageWaiterAResource> message_waiter_a_manager;
  MessageResourceManager<MessageWaiterBResource> message_waiter_b_manager;
  // TODO: Atkomentuoti.
  //MessageResourceManager<MessageLoadProgramResource> 
    //message_program_load_manager;
  //ReusableResourceManager<MemoryResource> memory_manager;

  ProcessManager *process_manager;

  // TODO: Iškelti ir sutvarkyti.
  MemoryResource memory_resource[10];

public:

  // Metodai.

  ResourceManager(): 
    message_waiter_a_manager(this), 
    message_waiter_b_manager(this) 
    // TODO: Reikia inicializuoti ir kitus.
    {

    this->process_manager = 0;

    }

  void set_process_manager(ProcessManager *process_manager) {
    this->process_manager = process_manager;
    }

  u64int get_resource(u64int resource_type, u64int process_id) {

    // FIXME: Šlapias kodas.
    switch (resource_type) {
      case RESOURCE_MWA:
        message_waiter_a_manager.get_resource(process_id);
        break;
      case RESOURCE_MWB:
        message_waiter_b_manager.get_resource(process_id);
        break;
      // TODO: Realizuoti ir kitus.
      default:
        return RETURN_CODE_UKNOWN_RESOURCE_TYPE;
      }

    return RETURN_CODE_OK;
    }

  u64int create_resource(u64int resource_type) {

    // FIXME: Šlapias kodas.
    switch (resource_type) {
      case RESOURCE_MWA: {
        MessageWaiterAResource resource;
        message_waiter_a_manager.create_resource(resource);
        break;
        }
      case RESOURCE_MWB: {
        MessageWaiterBResource resource;
        message_waiter_b_manager.create_resource(resource);
        break;
        }
      // TODO: Realizuoti ir kitus.
      default:
        return RETURN_CODE_UKNOWN_RESOURCE_TYPE;
      }

    return RETURN_CODE_OK;
    }

#pragma GCC diagnostic ignored "-Wunused-parameter"
  void give_resource(u64int process_id, MessageWaiterAResource resource) {
    // Ši funkcija nieko nedaro, kadangi ši žinutė procesui nieko 
    // neperduoda.
    }
#pragma GCC diagnostic error "-Wunused-parameter"

#pragma GCC diagnostic ignored "-Wunused-parameter"
  void give_resource(u64int process_id, MessageWaiterBResource resource) {
    // Ši funkcija nieko nedaro, kadangi ši žinutė procesui nieko 
    // neperduoda.
    }
#pragma GCC diagnostic error "-Wunused-parameter"

  void block_process(u64int process_id);
  void activate_process(u64int process_id);

  void init_memory_resource_manager(ProgramPager pager[], u64int number) {
    // TODO: Iškelti šitą funkciją į MemoryManager.

    for (u64int i = 0; i < number; i++) {

      debug_string("Puslapiavimo adresas (init_memory_resource_manager): ");
      debug_hex((u64int) pager[i].entry);
      debug_string("\n");

      this->memory_resource[i] = MemoryResource(i, &pager[i]);
      }
    
    }

  MemoryResource get_memory_resource_info(u64int memory_resource_id) {
    // TODO: Perdaryti normaliai.

    return this->memory_resource[memory_resource_id];

    }

  };

#endif
