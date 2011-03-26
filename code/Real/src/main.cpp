#include "monitor.h"

void *__gxx_personality_v0;

int main() {


  Monitor2 monitor2;
  //monitor.clear();
  //monitor.set_foreground_color(COLOR_GREEN);
  //monitor.write("Sveikas pasauli!\n");
  //monitor.write("Kaip laikaisi?\n");
  
  // The VGA framebuffer starts at 0xB8000.
  u16int *video_memory = (u16int *)0xB8000;

  *(video_memory + 0) = '0' | 0x0F00;
  *(video_memory + 1) = '0' | 0x0200;
  *(video_memory + 2) = '0' | 0x0200;
  *(video_memory + 3) = '0' | 0x0F00;

  //Array2dPointer<ScreenCharacter> video(
      //(ScreenCharacter *)0xB8000, SCREEN_WIDTH, SCREEN_HEIGHT);

  //video.set(1, 1, ScreenCharacter('A'));
  //video.set(2, 2, ScreenCharacter('A', COLOR_GREEN));
  //video.set(3, 3, ScreenCharacter('A', COLOR_BLACK, COLOR_WHITE));
  //video.set(4, 4, ScreenCharacter('A'));
  
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

  return 0xBABADEAD;
  }
