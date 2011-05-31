#ifndef PRIO_QUEUE_H
#define PRIO_QUEUE_H 1


#include "../types.h"
#include "../debug.h"

#define PRIORITY_QUEUE_MAX_NODES 1024


template <class T> class PriorityQueue {

private:

// Atributai.

  struct Node {

    T elem;
    u64int prio;

    Node() {
      prio = 0;
      }
    
    Node(u64int key, T val) {
      this->elem = val;
      this->prio = key;
      }

  } nodes[PRIORITY_QUEUE_MAX_NODES];

  u64int end;

public: 

// Metodai.

  PriorityQueue () {
    this->end = 0;
    }

  u64int get_size() const {
    return this->end;
    }

  bool is_empty() const {
    return this->end == 0;
    }
    
  bool is_full() const {
    return this->end + 1 >= PRIORITY_QUEUE_MAX_NODES;
    }

  void shift_right(u64int i) {
    u64int j = 0;
    for(j = get_size(); j>i; j--){
      nodes[j+1] = nodes[j];
      }
    }

  void add(u64int prio, T element) {
    debug_string("Pridedamas elementas: \n");
    debug_value("\tprioritetas: ", prio);
    debug_value("\telementas: ", element);
    u64int i = 0;
    if(this->is_full()) {
      PANIC("QUEUE IS FULL!");
      };
    while((i < this->end) && (prio >= nodes[i].prio)){
      i++;
      }
    shift_right(i);
    nodes[i] = Node(prio, element);
    debug_value("\tvieta: ", i);
    this->end++;
    }
  
  void shift_left(u64int i) {
    u64int j = 0;
    for(j = i; j < get_size(); j++){
      nodes[j] = nodes[j+1];
      }
    }
  
  void remove(T element) {
    u64int i;
    for (; (i < this->end) && (element != nodes[i].elem); i++);
    shift_left(i);
    this->end--;
    }
  
  T pop_front(){
    T elem = nodes[0].elem;
    shift_left(0);
    this->end--;
    return elem;
    }

  T get_front(){
    return nodes[0].elem;
    }
  
  /**
   * Išvalo eilę.
   */
  void clear() {
    this->begin = 0;
    }

  };

#endif
