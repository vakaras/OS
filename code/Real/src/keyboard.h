#ifndef KBD_H
#define KBD_H 1

#include "types.h"
#include "monitor.h"
#include "primitives.h"
#include "elf64.h"
#include "processes.h"
#include "resources.h"



//==========================================================================
// Klaviatūros išdėstymai:
// - lowercase: paprasti pirminiai simboliai
// - uppercase: simboliai, gaunami laikant paspaustą LShift arba RShift
//
// Šaltiniai:
// - http://www.win.tue.nl/~aeb/linux/kbd/scancodes-10.html
// - Mano klaviatūros išdėstymas o.O
//
// TODO:
// - Sutvarkyti escaped chars
// - Išanalizuoti Enter veikimą (dabar nustatyta Enter = 13, 
//                               LShift/RShift+Enter = "\n")
// - Padaryti Caps Lock
// - Padaryti LEDs veikimą
// - Sutvarkyti Backspace veikimą
//
// P.S. Sorry už ilgas eilutes
//==========================================================================



char lowercase[] =
{
  0, 0 /*esc*/, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 8 /*backspace*/,
  '\t' /*tab*/, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',  10 /*enter*/,
  0 /*ctrl*/, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0 /*lshift*/, '\\',
  'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
  0 /*rshift*/, 0 /*sysreq*/,  0 /*alt*/, ' ',
  0 /*capslock*/,  0/*F1*/,  0 /*F2*/, 0 /*F3*/,
  0 /*F4*/, 0 /*F5*/, 0 /*F6*/,  0 /*F7*/, 0 /*F8*/,
  0 /*F9*/,  0 /*F10*/, 0 /*numlock*/, 0 /*scrolllock*/, 
  0 /*home*/, 0 /*uarrow*/, 0 /*pup*/, 0 /*numminus*/,
  0 /*larrow*/, 0 /*num5*/, 0 /*rarrow*/, 0 /*numplus*/, 0 /*end*/,
  0 /*darrow*/, 0 /*pdown*/, 0 /*ins*/, 0 /*del*/, 0, 0, '\\', 
  0 /*F11*/, 0 /*F12*/
};

char uppercase[] =
{
  0, 0 /*esc*/, '!',  '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 8 /*backspace*/,
  '\t' /*tab*/, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 13,
  0 /*ctrl*/, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0 /*lshift*/, '|',
  'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
  0 /*rshift*/, 0 /*sysreq*/, 0 /*alt*/, ' ', 
  0 /*capslock*/, 0 /*F1*/, 0 /*F2*/, 0 /*F3*/,
  0 /*F4*/, 0 /*F5*/, 0 /*F6*/, 0 /*F7*/, 0 /*F8*/,
  0 /*F9*/, 0 /*F10*/, 0 /*numlock*/, 0 /*scrolllock*/, 
  0 /*home*/, 0 /*uarrow*/, 0 /*pup*/, '-' /*numminus*/,
  0 /*larrow*/, '5' /*num5*/, 0 /*rarrow*/, '+' /*numplus*/, 0 /*end*/,
  0 /*darrow*/, 0 /*pdown*/, 0 /*ins*/, 0 /*del*/, 0, 0, '|', 
  0 /*F11*/,0 /*F12*/
};

class Keyboard {
private:
  u8int shift_state;
  u8int escaped;
  
  Monitor *monitor;
  ProcessManager *proc_m;
  ProgramManager *prog_m;
  ResourceManager *reso_m;
  
  char komanda[8];
  int einamas;
  
public:
  
  Keyboard(Monitor *monitor, ProcessManager *proc_m, ProgramManager *prog_m,
           ResourceManager *reso_m) {
    this->monitor = monitor;
    this->proc_m = proc_m;
    this->prog_m = prog_m;
    this->reso_m = reso_m;
    this->shift_state = 0;
    this->escaped = 0;
    this->einamas = 0;
    reset_komanda();
  }
  
  void reset_komanda(){
    for(int i = 0; i<8;i++){
      this->komanda[i] = 0;
    }
  }
  
  void test_komanda(){/*
    if((this->komanda[0]=='') && (this->komanda[1]=='') 
        && (this->komanda[2]=='') && (this->komanda[3]==' ')){
      
    };*/
    this->monitor->write_string("\nYou entered: \"");
    this->monitor->write_string((const char*)this->komanda);
    this->monitor->write_string("\"\n");
    
    // tasks - show modules
    if((this->komanda[0]=='t') && (this->komanda[1]=='a') 
      && (this->komanda[2]=='s') && (this->komanda[3]=='k')
      && (this->komanda[4]=='s')){
        this->prog_m->debug(this->monitor);
    };
    
    // run X - run process id X
    if((this->komanda[0]=='r') && (this->komanda[1]=='u') 
      && (this->komanda[2]=='n') && (this->komanda[3]==' ')){
        u32int len;
        for (len = 0; len < 4; len++) {
          if (!('0' <= this->komanda[len + 4] 
                && this->komanda[len + 4] <= '9')) {
            break;
            }
          }
        u32int command = 0;
        for (u64int i = 0; i < len; i++) {
          command *= 10;
          command += this->komanda[i + 4] - '0';
          }
        if (!(1 <= command && command <= this->prog_m->get_count())) {
          this->monitor->write_string("\nUnknown program id #");
          this->monitor->write_dec(command);
          }
        else if (this->monitor->active_screen_id < 5 && 0 < command ){
          MessageLoadProgramResource resource(command,
                                        this->monitor->active_screen_id);
          this->monitor->write_string("\nStarting process #");
          this->monitor->write_dec(command);
          this->reso_m->add_resource(resource);
        }
      };
    
    // kill X - kill process id X
    if((this->komanda[0]=='k') && (this->komanda[1]=='i') 
      && (this->komanda[2]=='l') && (this->komanda[3]=='l')
      && (this->komanda[4]==' ')){
        this->proc_m->kill_process(this->komanda[5]-'0');
        this->monitor->write_string("\nKilling process #");
        this->monitor->write_dec(this->komanda[5]-'0');
      };
    
    this->monitor->put_character('\n');
    
  }
  
  void process_keyboard(CPUContext *cpu) {
    if (cpu->vector == 33) {
      Byte new_scan_code = get_byte(0x60);
      if (escaped) {   
        escaped = 0;
        }
      switch(new_scan_code) {
        case 0x2a: this->shift_state = 1; break; //+LShift (2A)
        case 0x36: this->shift_state = 1; break; //+RShift (36)
        case 0xaa: this->shift_state = 0; break; //-LShift (AA)
        case 0xb6: this->shift_state = 0; break; //-RShift (B6)
        case 0x3b: this->monitor->activate_screen(1); break;
        case 0x3c: this->monitor->activate_screen(2); break;
        case 0x3d: this->monitor->activate_screen(3); break;
        case 0x3e: this->monitor->activate_screen(4); break;
        case 0x3f: this->monitor->activate_screen(5); break;
        case 0x40: this->monitor->activate_screen(6); break;
        case 0x1d:  //+LCtrl (1D)
          if(this->monitor->active_screen_id < 5){
            this->proc_m->kill_process_by_screen_id(
                this->monitor->active_screen_id);
            this->monitor->write_string("\nKilling this process!");
          }
          break;
        default:
          if (new_scan_code & 0x80) {
            /* Ignore the break code */
          } else {
            char new_char = \
              (shift_state ? uppercase:lowercase)[new_scan_code];
            if((this->monitor->active_screen_id < 5) 
                && (this->monitor->get_active_screen_type())
                && new_char != 0) {
              this->proc_m->char_entered(this->monitor->active_screen_id, 
                  new_char);
              }
            else {
              if (new_char == 13) {
                this->einamas = 0;
                reset_komanda();
                } 
              else if (new_char == 10) {
                this->einamas = 0;
                test_komanda();
                reset_komanda();
                } 
              else if (this->einamas < 8) {
                this->komanda[this->einamas++] = new_char;
                };
              }
            this->monitor->put_character(new_char);
          }
          break;
      }
      send_byte(0x20,0x20);
    }
  }
  
};
#endif
