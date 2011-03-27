#include "monitor.h"
#include "descriptor_tables.h"
#include "isr.h"

void *__gxx_personality_v0;

int main() {

  Monitor monitor;
  monitor.set_background_color(COLOR_CYAN);
  monitor.clear();
  monitor.set_character(1, 1, 'A');
  monitor.set_character(2, 2, ScreenCharacter('A', COLOR_GREEN));
  monitor.set_foreground_color(COLOR_RED);
  monitor.set_character(3, 3, 'A');
  monitor.set_background_color(COLOR_BROWN);
  monitor.set_character(4, 4, 'A');

  monitor.set_foreground_color(COLOR_WHITE);
  monitor.set_background_color(COLOR_WHITE);
  monitor.fill(5, 5, 5, 5, ' ');
  monitor.set_background_color(COLOR_BLACK);

  monitor.put_character('L');
  monitor.put_character('A');
  monitor.put_character('B');
  monitor.put_character('A');
  monitor.put_character('S');

  for (int i = 0; i < 100; i++) {
    monitor.put_character('0' + i / 10);
    monitor.put_character('0' + i % 10);
    monitor.write_string(". Sveikas pasauli! ");
    }

  monitor.write_string("\n\n");
  monitor.write_string("1\t2\t3\t4\n");
  monitor.write_string("11\t22\t33\t44\rAAAA\n");

  monitor.write_hex(0x123DEAD);
  monitor.write_string("\n\n");
  monitor.write_dec(0x123DEAD);
  monitor.write_string("\n\n");
  monitor.write_dec(123456789u);
  monitor.write_string("\n\n");
  monitor.write_dec(0u);

  asm volatile ("int $0x3");
  asm volatile ("int $0x4");

  return 0xBABADEAD;
  }
