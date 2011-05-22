#include "resources.h"
#include "processes.h"


void ResourceManager::block_process(u64int process_id) {

  this->process_manager->block_process(process_id);

  }

void ResourceManager::activate_process(u64int process_id) {

  this->process_manager->activate_process(process_id);

  }
