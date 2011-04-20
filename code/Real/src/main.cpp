void *__gxx_personality_v0;

// Spalvos.
#define COLOR_BLACK         0x0
#define COLOR_BLUE          0x1
#define COLOR_GREEN         0x2
#define COLOR_CYAN          0x3
#define COLOR_RED           0x4
#define COLOR_MAGENTA       0x5
#define COLOR_BROWN         0x6
#define COLOR_LIGHT_GREY    0x7
#define COLOR_DARK_GREY     0x8
#define COLOR_LIGHT_BLUE    0x9
#define COLOR_LIGHT_GREEN   0xA
#define COLOR_LIGHT_CYAN    0xB
#define COLOR_LIGHT_RED     0xC
#define COLOR_LIGHT_MAGENTA 0xD
#define COLOR_LIGHT_BROWN   0xE
#define COLOR_WHITE         0xF

// 64 bitų dydžiai.
typedef unsigned  long    u64int;
typedef           long    s64int;

// 32 bitų dydžiai.
typedef unsigned  int     u32int;
typedef           int     s32int;
typedef unsigned  short   u16int;
typedef           short   s16int;
typedef unsigned  char    u8int;
typedef           char    s8int;

struct ScreenCharacter {

// Atributai.
  char character;
  u8int
    foreground_color : 4,
    background_color : 4;

// Metodai.
  
  ScreenCharacter() { 
    this->character = 0x20;             // Tarpo simbolis.
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

//Kodėl nesikompiliuoja?
//void print(const char *str) {
  
  //}

int main() {

  ScreenCharacter *sc = (ScreenCharacter *) 0xB8000;
  //print("Labas Vyti!"); FIXME: Kodėl?
  *(sc + 0) = ScreenCharacter('L');
  *(sc + 1) = ScreenCharacter('a');
  *(sc + 2) = ScreenCharacter('b');
  *(sc + 3) = ScreenCharacter('a');
  *(sc + 4) = ScreenCharacter('s');
  *(sc + 5) = ScreenCharacter(' ');
  *(sc + 6) = ScreenCharacter('V');
  *(sc + 7) = ScreenCharacter('y');
  *(sc + 8) = ScreenCharacter('t');
  *(sc + 9) = ScreenCharacter('i');
  *(sc + 10) = ScreenCharacter('!');

  return 0xBABADEAD;
  }
