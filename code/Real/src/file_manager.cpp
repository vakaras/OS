#include "file_manager.h"
#include "processes.h"


void File::add_process_to_waiting_queue(u64int process_id, FILE_MODE mode) {

  this->process_queue.push_back(ProcessContainer(process_id, mode));
  this->file_manager->block_process(process_id);

  }

void File::plan() {

  debug_value("Vykdomas failo planuotojas: ", this->id);

  if (this->is_free()) {
    debug_string("\tFailas užimtas.\n");
    return;
    }

  while (!this->process_queue.is_empty()) {

    ProcessContainer container = process_queue.pop_front();

    if (container.process_id == 0) {
      debug_string("\tProcesas jau nebegyvas.\n");
      }
    else {
      this->set_free(false);
      this->file_manager->give_file(
          container.process_id, this->id, container.mode);
      this->file_manager->activate_process(container.process_id);

      debug_value("\tAtiduotas procesui: ", container.process_id);
      break;
      }

    }

  }


void FileManager::block_process(u64int process_id) {

  this->process_manager->block_process(process_id);
  
  }

void FileManager::activate_process(u64int process_id) {

  this->process_manager->activate_process(process_id);

  }

void FileManager::give_file(
    u64int process_id, u64int file_id, FILE_MODE mode) {

  this->process_manager->give_file(process_id, file_id, mode);

  }

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
