#ifndef TEST_MONITOR_H
#define TEST_MONITOR_H


#include "../monitor.h"


void test_monitor(Monitor *monitor) {

  // Tikrinama ar veikia ekrano išvalymas.
  monitor->set_background_color(COLOR_CYAN);
  monitor->clear();

  // Tikrinama ar veikia simbolio „padėjimas ant ekrano“.
  monitor->set_character(1, 1, 'A');
  monitor->set_character(2, 2, ScreenCharacter('A', COLOR_GREEN));
  monitor->set_foreground_color(COLOR_RED);
  monitor->set_character(3, 3, 'A');
  monitor->set_background_color(COLOR_BROWN);
  monitor->set_character(4, 4, 'A');

  // Tikrinama ar veikia regiono užpildymas.
  monitor->set_foreground_color(COLOR_WHITE);
  monitor->set_background_color(COLOR_WHITE);
  monitor->fill(5, 5, 5, 5, ' ');

  // Tikrinama ar veikia paprastas simbolių išvedimas.
  monitor->set_background_color(COLOR_BLACK);
  monitor->put_character('L');
  monitor->put_character('A');
  monitor->put_character('B');
  monitor->put_character('A');
  monitor->put_character('S');

  // Tikrinama ar veikia paprastas teksto išvedimas.
  monitor->write_string("\n\n");
  for (int i = 0; i < 10; i++) {
    monitor->put_character('0' + i / 10);
    monitor->put_character('0' + i % 10);
    monitor->put_character(' ');
    monitor->write_string("Sveikas pasauli! ");
    }

  // Tikrinama ar veikia specialiųjų simbolių apdorojimas.
  monitor->write_string("\n\n");
  monitor->write_string("1\t2\t3\t4\n");
  monitor->write_string("11\t22\t33\t44\rAAAA\n");

  // Tikrinama ar veikia skaičių išvedimas.
  monitor->write_hex(0x123DEAD);
  monitor->write_string("\n\n");
  monitor->write_dec(0x123DEAD);
  monitor->write_string("\n\n");
  monitor->write_dec(123456789u);
  monitor->write_string("\n\n");
  monitor->write_dec(0u);
  
  }

#endif
