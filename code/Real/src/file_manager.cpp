#include "file_manager.h"
#include "processes.h"


void FileManager::queue_process_for_byte(
    u64int process_id, u64int screen_id) {

  this->screen_process_queue[screen_id].push_back(process_id);
  this->process_manager->block_process(process_id);
  // FIXME: Reikėtų patikrinti ar geras screen id.
  }

void FileManager::plan() {

  // TODO: Realizuoti.

  debug_string("Vykdomas stdin planuotojas.");

  for (u64int i = 1; i <= SCREEN_NUMBER; i++) {

    while ((!this->screen_process_queue[i].is_empty()) &&
        (!this->screen_buffer_queue[i].is_empty())) {

      char symbol = this->screen_buffer_queue[i].pop_front();
      u64int process_id = this->screen_process_queue[i].pop_front();

      this->process_manager->give_byte(process_id, symbol);
      this->process_manager->activate_process(process_id);
      }
    
    }
  
  }
