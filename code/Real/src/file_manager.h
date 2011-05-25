#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H 1


#include "types.h"
#include "debug.h"
#include "structures/rotating_queue.h"

#define SCREEN_NUMBER 4


enum FILE_MODE {FILE_MODE_READ, FILE_MODE_WRITE};


class ProcessManager;


void print_program_message(int screen_id, char text);
void print_program_message(int screen_id, const char * text);
void print_program_message(int screen_id, u64int number, u8int size);
void print_program_message(int screen_id, u64int number);


class FileManager {

private:

  // Atributai.

  ProcessManager *process_manager;
  RotatingQueue<u64int> screen_process_queue[SCREEN_NUMBER+1];
  RotatingQueue<char> screen_buffer_queue[SCREEN_NUMBER+1];
  
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

  void reset_stdin(u64int screen_id) {
    this->screen_buffer_queue[screen_id].clear();
    this->screen_process_queue[screen_id].clear();
    }

  void give_stdin_byte(u64int screen_id, char symbol) {
    this->screen_buffer_queue[screen_id].push_back(symbol);
    this->plan();
    }
  
  void write_stdout_byte(u64int screen_id, char symbol) {

    print_program_message(screen_id, symbol);

    }

  void plan();

  };

#endif
