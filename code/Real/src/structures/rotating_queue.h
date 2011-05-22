#ifndef ROTATING_QUEUE_H
#define ROTATING_QUEUE_H 1


#include "../types.h"

//#define ROTATING_QUEUE_MAX_NODES 1024
#define ROTATING_QUEUE_MAX_NODES 8


template <class T> class RotatingQueue {

private:

// Atributai.

  T nodes[ROTATING_QUEUE_MAX_NODES];

  u64int begin;                         // Pirmojo egzistuojančio elemento 
                                        // indeksas.
  u64int end;                           // Pirmojo neegzistuojančio elemento
                                        // indeksas.
                                        // Jei begin == end, tai eilė 
                                        // tuščia.

public: 

// Metodai.

  RotatingQueue () {
    this->begin = 0;
    this->end = 0;
    }

  u64int get_size() const {
    if (this->begin <= this->end) {
      return this->end - this->begin;
      }
    else {
      return (this->end + ROTATING_QUEUE_MAX_NODES) - this->begin;
      }
    }

  bool is_empty() const {
    return this->begin == this->end;
    }

  void push_back(T element) {

    // FIXME: Pridėti patikrinimą, ar nebuvo perpildyta eilė.

    if (this->end == ROTATING_QUEUE_MAX_NODES) {
      this->end = 0;
      }
    
    this->nodes[this->end++] = element;

    }
  
  T pop_front() {

    // FIXME: Pridėti patikrinimą, ar yra ką gražinti.

    u64int old_id = this->begin;
    this->begin++;

    if (this->begin == ROTATING_QUEUE_MAX_NODES) {
      this->begin = 0;
      }

    return this->nodes[old_id];
    }

  };


#endif
