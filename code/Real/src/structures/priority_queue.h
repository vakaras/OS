#ifndef PRIO_QUEUE_H
#define PRIO_QUEUE_H 1


#include "../types.h"
#include "../debug.h"

#define PRIORITY_QUEUE_MAX_NODES 1024

template <class P, class T> struct Node {

// Atributai.

  T value;
  P priority;

// Metodai.

  Node() {}
  
  Node(P priority, T value) {
    this->value = value;
    this->priority = priority;
    }

  };

template <class P, class T> class PriorityQueue {

private:

// Atributai.

  Node<P, T> nodes[PRIORITY_QUEUE_MAX_NODES];
  u64int end;                           // Elementas po paskutiniojo.

  /**
   * Pastumia nodes[pos:] į nodes[pos+1:]
   */
  void shift_right(u64int pos) {

    for(u64int i = this->end; i > pos; i--) {
      this->nodes[i] = this->nodes[i-1];
      }

    }

  /**
   * Pastumia nodes[pos:] į nodes[pos-1:]
   */
  void shift_left(u64int pos) {
    for(u64int i = pos; i < this->end; i++){
      this->nodes[i-1] = this->nodes[i];
      }
    }

public: 

// Metodai.

  PriorityQueue () {
    this->end = 0;
    }

  u64int get_size() const {
    return this->end;
    }

  bool is_empty() const {
    return !this->end;
    }
    
  bool is_full() const {
    return this->end >= PRIORITY_QUEUE_MAX_NODES;
    }

  void add(P priority, T value) {

    if (this->is_full()) {
      PANIC("QUEUE IS FULL!");
      }

    u64int pos = 0;
    for (; 
        (pos < this->end) && (priority <= this->nodes[pos].priority); 
        pos++);

    shift_right(pos);
    this->nodes[pos] = Node<P, T>(priority, value);
    this->end++;

    }
  
  T pop_front(){

    if (this->is_empty()) {
      PANIC("Queue is empty!");
      }

    T value = nodes[0].value;
    shift_left(0);
    this->end--;

    return value;
    }

  /**
   * Išvalo eilę.
   */
  void clear() {
    this->end = 0;
    }

  };

#endif
