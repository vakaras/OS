#ifndef RESOURCE_TYPES_H
#define RESOURCE_TYPES_H 1

#include "types.h"
#include "paging.h"


class Resource {
  };


class MessageResource: Resource {
  };


class MessageWaiterAResource: MessageResource {
  };


class MessageWaiterBResource: MessageResource {
  };


class MessageLoadProgramResource: MessageResource {

protected:

  // Atributai.
  
  u64int program_id;
  u64int screen_id;

public:

  // Metodai.
  MessageLoadProgramResource() {
    this->program_id = INFINITY;
    this->screen_id = INFINITY;
    }

  MessageLoadProgramResource(u64int program_id, u64int screen_id) {

    this->program_id = program_id;
    this->screen_id = screen_id;

    }
  
  u64int get_program_id() {
    return this->program_id;
    }
  
  u64int get_screen_id() {
    return this->screen_id;
    }

  };


class ReusableResource: Resource {

protected:

  // Atributai.
  
  s64int id;                            // Jei -1 – tai reiškia, kad 
                                        // resursas dar neperduotas 
                                        // tvarkyklei.
  bool exists;
  bool free;

public:
  
  // Metodai.

  ReusableResource() {

    this->id = -1;
    this->exists = false;
    this->free = false;

    }

  u64int get_id() {
    return this->id;
    }

  void set_free(bool free) {
    this->free = free;
    }

  void set_id(u64int id) {
    this->id = id;
    }
  
  bool is_empty_slot() {
    return !this->exists;
    }
  
  bool is_free() {
    return this->free;
    }
  
  };


class MemoryResource: public ReusableResource {

private:

  // Atributai.

  ProgramPager *pager;

public:

  // Metodai.

  MemoryResource(): ReusableResource() {
    }

  MemoryResource(u64int id, ProgramPager *pager, bool free=true) {

    this->id = id;
    this->pager = pager;
    this->exists = true;
    this->free = free;

    }
  
  ProgramPager *get_pager() {
    return this->pager;
    }

  };

#endif
