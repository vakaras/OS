#ifndef ELF64_H
#define ELF64_H 1

#include "types.h"
#include "monitor.h"
#include "debug.h"
#include "paging.h"

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


struct ElfHeader {

  u8int identification[16];
  u16int type;
  u16int machine;
  u32int version;
  u64int entry;
  u64int program_header_table_offset;
  u64int section_header_table_offset;
  u32int flags;
  u16int header_size;
  u16int program_header_entry_size;
  u16int number_program_header_entry;
  u16int section_header_entry_size;
  u16int number_section_header_entry;
  u16int section_name_table_index;

  };

struct ElfProgramHeader {

  u32int type;
  u32int flags;
  u64int offset;
  u64int virtual_address;
  u64int physical_address;
  u64int file_size;
  u64int memory_size;
  u64int alignment;

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
          (void *) MAKE_ADDRESS_VIRTUAL((u64int) *(p + (4 * i))));
      this->module_list.module[i].end = (
          (void *) MAKE_ADDRESS_VIRTUAL((u64int) *(p + (4 * i + 1))));
      this->module_list.module[i].name = (
          (const char *) MAKE_ADDRESS_VIRTUAL((u64int) *(p + (4 * i + 2))));
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

  /**
   * Pakrauna nurodytą programą į nurodytą atmintį.
   * @param id – kokią programą pakrauti;
   * @param pager – į kokią atmintį pakrauti.
   */
  u64int load(u64int id, ProgramPager pager) {

    if (id >= this->get_count()) {
      return 1;
      }

    pager.clear_lower();
    pager.create_lower();
    pager.activate();

    ElfHeader *header = (ElfHeader *) this->module_list.module[id].start;

    debug_string("header: ");
    debug_hex((u64int) header);

    if (header->identification[4] != 2) {
      // ELF klasė nėra 64 bitų.
      return 2;
      }
    if (header->identification[5] != 1) {
      // Edianess nėra little.
      return 3;
      }
    if (header->type != 2) {
      // Failas nėra vykdomasis.
      return 4;
      }
    if (header->number_program_header_entry != 1) {
      // Tokio ELF nemokame pakrauti.
      return 5;
      }

    // header table offset 0x40
    
    ElfProgramHeader *program_header = (ElfProgramHeader *)(
        ((u8int *) this->module_list.module[id].start) +
        header->program_header_table_offset);

    memcpy(
        (u8int *) program_header->virtual_address,
        ((u8int *) this->module_list.module[id].start) + 
          program_header->offset,
        program_header->file_size
        );

    debug_string("\nVirtuali atmintis:  ");
    debug_hex(program_header->virtual_address);
    u32int *p = (u32int *) 0x200000;
    debug_string("\nAtmintis:           ");
    debug_hex(*p);
    debug_string("\n:");
    
    //jump(header->entry); 
    // Persijungia į kitą procesą. Kadangi pertraukimų mechanizmas dar
    // nemoka įjungti buvusiojo puslapiavimo mechanizmo, tai OS nebegrįžta
    // iš pertraukimo ir sminga. Dėl šios priežasties persijungimas
    // užkomentuotas.

    return 0;
    }

  };


#endif
