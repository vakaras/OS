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


class ReusableResource: Resource {

protected:

  // Atributai.
  
  u64int id;
  bool exists;
  bool free;

public:
  
  // Metodai.

  ReusableResource() {

    this->id = 0;
    this->exists = false;
    this->free = false;

    }

  u64int get_id() {
    return this->id;
    }
  
  bool is_empty_slot() {
    return !this->exists;
    }
  
  bool is_free() {
    return this->free;
    }
  
  };


class MemoryResource: ReusableResource {

private:

  // Atributai.

  ProgramPager *pager;

public:

  // Metodai.

  MemoryResource(): ReusableResource() {
    }

  MemoryResource(u64int id, ProgramPager *pager) {

    this->id = id;
    this->pager = pager;
    this->exists = true;
    this->free = true;

    }
  
  ProgramPager *get_pager() {
    return this->pager;
    }

  };

#endif
