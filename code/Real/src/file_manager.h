#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H 1


#include "types.h"
#include "debug.h"
#include "structures/rotating_queue.h"

#define SCREEN_NUMBER 4


enum FILE_MODE {FILE_MODE_READ, FILE_MODE_WRITE};


class ProcessManager;


class FileManager {

private:

  // Atributai.

  ProcessManager *process_manager;
  RotatingQueue<u64int> screen_process_queue[SCREEN_NUMBER];
  RotatingQueue<char> screen_buffer_queue[SCREEN_NUMBER];
  
  // Metodai.

  void queue_process_for_byte(u64int process_id, u64int screen_id);

public:

  FileManager() {
    this->process_manager = 0;
    }
  
  void set_process_manager(ProcessManager *process_manager) {
    this->process_manager = process_manager;
    }

  void get_stdin_byte(u64int screen_id, u64int process_id) {
    this->queue_process_for_byte(process_id, screen_id);
    this->plan();
    }

  void plan();

  };

#endif
