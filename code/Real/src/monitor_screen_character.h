#ifndef MONITOR_SCREEN_CHARACTER_H
#define MONITOR_SCREEN_CHARACTER_H

#include "monitor_defines.h"


struct ScreenCharacter {

// Atributai.
  char character;
  u8int
    foreground_color : 4,
    background_color : 4;

// Metodai.
  
  ScreenCharacter() { 
    this->character = SYMBOL_SPACE;
    this->foreground_color = COLOR_WHITE;
    this->background_color = COLOR_BLACK;
    }

  ScreenCharacter(
      char character, 
      u8int foreground_color=COLOR_WHITE, 
      u8int background_color=COLOR_BLACK) {
    this->character = character;
    this->foreground_color = foreground_color;
    this->background_color = background_color;
    }
  
  };

#endif
