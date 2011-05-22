#ifndef RESOURCES_H
#define RESOURCES_H 1

#include "types.h"
#include "debug.h"
#include "resource_types.h"
#include "message_resource_manager.h"
#include "reusable_resource_manager.h"

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
  ReusableResourceManager<MemoryResource> memory_manager;

  ProcessManager *process_manager;

public:

  // Metodai.

  ResourceManager(): 
    message_waiter_a_manager(this), 
    message_waiter_b_manager(this),
    memory_manager(this)
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
        this->message_waiter_a_manager.create_resource(resource);
        break;
        }
      case RESOURCE_MWB: {
        MessageWaiterBResource resource;
        this->message_waiter_b_manager.create_resource(resource);
        break;
        }
      // TODO: Realizuoti ir kitus.
      default:
        return RETURN_CODE_UKNOWN_RESOURCE_TYPE;
      }

    return RETURN_CODE_OK;
    }

  void add_memory_resource(MemoryResource memory_resource) {

    this->memory_manager.create_resource(memory_resource);

    }

  void free_memory_resource(MemoryResource memory_resource) {

    this->memory_manager.free_resource(memory_resource);

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

  void give_resource(u64int process_id, MemoryResource resource);
  void block_process(u64int process_id);
  void activate_process(u64int process_id);

  MemoryResource get_memory_resource_info(u64int memory_resource_id) {

    return this->memory_manager.get_resource_info(memory_resource_id);

    }

  };

#endif
