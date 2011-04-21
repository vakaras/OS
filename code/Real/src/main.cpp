#include "cxx.h"
#include "icxxabi.h"
#include "types.h"
#include "primitives.h"
#include "structures/point.h"
#include "structures/array2dpointer.h"
#include "monitor_defines.h"
#include "monitor_screen_character.h"
#include "monitor.h"
#include "tests/test_monitor.h"


// Globalūs kintamieji.
Monitor monitor;

extern "C" int main() {

  test_monitor(&monitor);

  return 0xBABADEAD;
  }
