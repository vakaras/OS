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
    this->tick++;
    if (cpu->vector == 32){
      if(this->tick < 4){
        this->monitor->write_string("Tick: ");
        this->monitor->write_dec((u32int)this->tick);
        this->monitor->write_string("\n");
      }
    }
  }
  
};

#endif
