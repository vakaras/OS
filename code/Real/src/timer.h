#ifndef TIMER_H
#define TIMER_H 1

#include "types.h"
#include "monitor.h"
#include "primitives.h"

class Timer {
  
private:
  u64int tick;
  Monitor *monitor;
  
public:
  
  Timer(Monitor *monitor) {
    this->monitor = monitor;
    this->tick = 0;
  }
  
  void process_timer(CPUContext *cpu) {
    
    if (cpu->vector == 32) {
      this->tick++;
      if(this->tick > 5) {
        this->tick = 0;
        this->monitor->update_htop();
      }
    }
  }
  
};

#endif
