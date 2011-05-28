#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H 1


#include "types.h"
#include "debug.h"
#include "structures/rotating_queue.h"
#include "resource_types.h"

#define SCREEN_NUMBER 4
#define FILE_SIZE 0x1000000             // 16 MB
#define MAX_FILES 16
#define BASE_ADDRESS 0x0000000010000000


enum FILE_MODE {FILE_MODE_READ, FILE_MODE_WRITE};


class ProcessManager;
class FileManager;


void print_program_message(int screen_id, char text);
void print_program_message(int screen_id, const char * text);
void print_program_message(int screen_id, u64int number, u8int size);
void print_program_message(int screen_id, u64int number);


struct ProcessContainer {

// Atributai.

  u64int process_id;
  FILE_MODE mode;

// Metodai.

  ProcessContainer() {
    this->process_id = 0;
    }
  
  ProcessContainer(u64int process_id, FILE_MODE mode) {
    this->process_id = process_id;
    this->mode = mode;
    }

  bool operator == (const ProcessContainer &a) const {
    return this->process_id == a.process_id;
    }
  
  };


class File: public ReusableResource {

private:

  // Atributai.

  u64int file_name;
  u64int address;
  u64int offset;

  RotatingQueue<ProcessContainer> process_queue;
  FileManager *file_manager;

  // Metodai.

protected:

  void set_byte(u64int offset, u8int byte) {

    if (this->is_empty_slot()) {
      PANIC("Rašoma į neegzistuojantį failą.");
      }

    if (offset >= FILE_SIZE) {
      PANIC("Peržengti failo rėžiai.");
      }
    
    *((u8int *) (this->address + offset)) = byte;
    *((u8int *) (this->address + offset + 1)) = 0x0;

    debug_value("Rašoma į failą: ", this->file_name);
    debug_value("  Adresas: ", this->address + offset);
    debug_value("  Reikšmė: ", byte);

    }

  u8int get_byte(u64int offset) {

    if (this->is_empty_slot()) {
      PANIC("Skaitoma iš neegzistuojančio failo.");
      }

    if (offset >= FILE_SIZE) {
      PANIC("Peržengti failo rėžiai.");
      }

    return *((u8int *) (this->address + offset));
    }

public:

  File(): ReusableResource() {
    }

  File(
      FileManager *file_manager, u64int address, u64int file_name, 
      u64int id, bool free=true) {

    this->id = id;
    this->exists = true;
    this->free = free;

    this->file_name = file_name;
    this->address = address;
    this->offset = 0;

    this->file_manager = file_manager;

    }

  u64int get_file_name() {
    return this->file_name;
    }

  void add_process_to_waiting_queue(u64int process_id, FILE_MODE mode);

  void plan();

  void process_killed(u64int process_id) {

    this->process_queue.replace(
        ProcessContainer(process_id, FILE_MODE_WRITE),
        ProcessContainer(0, FILE_MODE_WRITE));

    // TODO: Užbaigti. (Failo resurso atlaisvinimas.)

    }

  void reset() {
    this->offset = 0;
    }

  void write_byte(u8int byte) {

    this->set_byte(this->offset++, byte);

    }

  u8int read_byte() {

    return this->get_byte(this->offset++);
    }

  };


class FileManager {

private:

  // Atributai.

  ProcessManager *process_manager;
  RotatingQueue<u64int> screen_process_queue[SCREEN_NUMBER+1];
  RotatingQueue<char> screen_buffer_queue[SCREEN_NUMBER+1];
  File files[MAX_FILES];
  
  // Metodai.

  void queue_process_for_byte(u64int process_id, u64int screen_id);

public:

  FileManager() {
    this->process_manager = 0;
    }

  bool exists(u64int file_name) {

    for (u64int i = 0; i < MAX_FILES; i++) {
      if (!this->files[i].is_empty_slot()) {
        if (this->files[i].get_file_name() == file_name) {
          return true;
          }
        }
      }
    
    return false;
    }

  u64int create_file_write(u64int file_name) {

    for (u64int i = 0; i < MAX_FILES; i++) {
      if (this->files[i].is_empty_slot()) {
        this->files[i] = File(
            this, BASE_ADDRESS + FILE_SIZE * i, file_name, i, false);
        return i;
        }
      }
    
    PANIC("Užpildytas failų sąrašas.");
    return 0;
    }

  void get_file_write(u64int file_name, u64int process_id) {

    for (u64int i = 0; i < MAX_FILES; i++) {
      if (!this->files[i].is_empty_slot() &&
          this->files[i].get_file_name() == file_name) {

        this->files[i].add_process_to_waiting_queue(
            process_id, FILE_MODE_WRITE);
        this->files[i].plan();

        return;
        }
      }

    PANIC("Failų sistemos klaida?");
    }

  void get_file_read(u64int file_name, u64int process_id) {

    for (u64int i = 0; i < MAX_FILES; i++) {
      if (!this->files[i].is_empty_slot() &&
          this->files[i].get_file_name() == file_name) {

        this->files[i].add_process_to_waiting_queue(
            process_id, FILE_MODE_READ);
        this->files[i].plan();

        return;
        }
      }
    
    }

  void free_file(u64int file_id) {

    debug_value("Atlaisvinamas failas: ", file_id);
    this->files[file_id].set_free(true);
    this->files[file_id].plan();

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

  void write_file_byte(u64int file_id, char symbol) {

    if (file_id >= MAX_FILES) {
      PANIC("Rašoma į neegzistuojantį failą.");
      }
    
    this->files[file_id].write_byte(symbol);

    }

  char get_file_byte(u64int file_id) {

    if (file_id >= MAX_FILES) {
      PANIC("skaitoma iš neegzistuojančio failo.");
      }
    
    return this->files[file_id].read_byte();
    }

  void plan();
  void block_process(u64int process_id);
  void activate_process(u64int process_id);
  void give_file(u64int process_id, u64int file_id, FILE_MODE mode);

  void process_killed(u64int process_id) {

    for (u64int i = 0; i < MAX_FILES; i++) {
      if (!this->files[i].is_empty_slot()) {
        this->files[i].process_killed(process_id);
        }
      }

    }

  };

#endif
