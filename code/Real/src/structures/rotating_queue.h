#ifndef ROTATING_QUEUE_H
#define ROTATING_QUEUE_H 1


#include "../types.h"
#include "../debug.h"

#define ROTATING_QUEUE_MAX_NODES 1024


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

    if(this->get_size() == ROTATING_QUEUE_MAX_NODES - 1) {
      PANIC("Rotating queue is full on push_back()");
    }

    u64int old_id = this->end;

    if (++(this->end) == ROTATING_QUEUE_MAX_NODES) {
      this->end = 0;
      }
    
    this->nodes[old_id] = element;

    }
  
  T pop_front() {

    if(this->is_empty()){
      PANIC("Rotating queue is emppty on pop_front()");
    }

    u64int old_id = this->begin;
    this->begin++;

    if (this->begin == ROTATING_QUEUE_MAX_NODES) {
      this->begin = 0;
      }

    return this->nodes[old_id];
    }

  /**
   * Išvalo eilę.
   */
  void clear() {
    this->begin = 0;
    this->end = 0;
    }

  /**
   * Visus įrašus turinčius reiškmę a pakeičia turinčiais reikšmę b
   */
  void replace(T a, T b) {

    if (this->end < this->begin) {

      for (u64int i = this->begin; i < ROTATING_QUEUE_MAX_NODES; i++) {
        if (this->nodes[i] == a) {
          this->nodes[i] = b;
          }
        }
      for (u64int i = 0; i < this->end; i++) {
        if (this->nodes[i] == a) {
          this->nodes[i] = b;
          }
        }

      }
    else {

      for (u64int i = this->begin; i < this->end; i++) {
        if (this->nodes[i] == a) {
          this->nodes[i] = b;
          }
        }

      }

    }

  };


#endif
