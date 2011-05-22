#include "resources.h"
#include "processes.h"


void ResourceManager::give_resource(
    u64int process_id, MemoryResource resource) {

  this->process_manager->give_loader_memory(process_id, resource);

  }

void ResourceManager::block_process(u64int process_id) {

  this->process_manager->block_process(process_id);

  }

void ResourceManager::activate_process(u64int process_id) {

  this->process_manager->activate_process(process_id);

  }
