#ifndef ELF64_H
#define ELF64_H 1

#include "types.h"
#include "monitor.h"

#define MAX_PROGRAM 10                  // Kiek daugiausiai skirtingų
                                        // programų gali būti.

struct Module {

  void *start;
  void *end;
  const char *name;

  };


struct ModuleList {

  u64int count;
  Module module[MAX_PROGRAM];
  
  };


class ProgramManager {

private:

  // Atributai.
  
  ModuleList module_list;
  
public:

  // Metodai.

  /**
   * Konstruktorius.
   * @param module_list – adresas, kuriuo saugoma informacija apie 
   * GRUB pakrautus modulius.
   */
  ProgramManager(u64int module_list) {

    u32int *p = (u32int *) module_list;

    this->module_list.count = *p;

    p = (u32int *)((u64int) *(p + 1));

    for (u64int i = 0; i < this->module_list.count; i++) {
      this->module_list.module[i].start = (
          (void *)((u64int) *(p + (4 * i))));
      this->module_list.module[i].end = (
          (void *)((u64int) *(p + (4 * i + 1))));
      this->module_list.module[i].name = (
          (const char *)((u64int) *(p + (4 * i + 2))));
      }


    }

  /**
   * Grąžina kiek iš viso turi programų.
   */
  u64int get_count() {
    return this->module_list.count;
    }

  /**
   * Gražina nuorodą į programos pavadinimą.
   */
  const char *get_name(u8int id) {
    return this->module_list.module[id].name;
    }

  void debug(Monitor *monitor) {

    monitor->write_string("\nModuliai: ");
    monitor->write_hex(this->module_list.count);
    monitor->write_string("\nAdresas:  ");
    monitor->write_hex((u64int) this->module_list.module);

    for (u64int i = 0; i < this->get_count(); i++) {
      monitor->write_string("\n\tPavadinimas: ");
      monitor->write_string(this->module_list.module[i].name);
      }

    }

  };


#endif
