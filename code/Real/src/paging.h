#ifndef PAGING_H 
#define PAGING_H 1

#include "types.h"
#include "memlib.h"


#define PAGE_TABLE_PERM ((1<<0) | (1<<1))
#define PAGE_TABLE_PERM_NOCACHE (PAGE_TABLE_PERM | (1<<3) | (1<<4))
#define PAGE_TABLE_LARGE_FLAG (1<<7)
#define PAGE_TABLE_ADDRESS 0xfffffffffffff000

#define PHYSICAL_ADDRESS_START 0x00000000001fa000
#define VIRTUAL_ADDRESS_START 0xffff800000000000
#define FIX_ADDRESS(a) \
  ((a) - VIRTUAL_ADDRESS_START + PHYSICAL_ADDRESS_START)
#define MAKE_ADDRESS_VIRTUAL(a) \
  ((a) - PHYSICAL_ADDRESS_START + VIRTUAL_ADDRESS_START)


/** 
 * Užtikrina, kad adresas būtų išlygiuotas 4KB.
 */
void *align(void *address);
u64int align(u64int address);


class PageDirectory {

private:

  // Atributai.

  u64int *entry;                        // Pirmojo lentelės įrašo adresas.
  u64int physical_address;              // Kokius fizinius adresus duoti 
                                        // naujiems įrašams.
  u64int tables;                        // Kiek puslapių lentelių buvo
                                        // sukurta šiame kataloge.

public:

  // Metodai.

  /**
   * Konstruktorius.
   * @param address – kokiu adresu „padėti“ lentelę.
   */
  PageDirectory(u64int table_address, u64int physical_address) {
    this->entry = (u64int *) align(table_address);
    this->physical_address = align(physical_address);
    this->clear();
    }
  
  /**
   * Išvalo lentelę.
   */
  void clear() {

    this->tables = 0;

    for (int i = 0; i < 512; i++) {
      this->entry[i] = 0;
      }

    }

  /**
   * Prideda 2 MB puslapį.
   */
  void add_page(u64int number) {
    // FIXME: Pridėti patikrinimą, kad 0 <= number < 512.
    this->entry[number] = (
        PAGE_TABLE_PERM_NOCACHE | 
        PAGE_TABLE_LARGE_FLAG | 
        (this->physical_address + 0x200000 * number)
        );
    }

  /**
   * Nukopijuoja egzistuojančią puslapių lentelę iš karto po savo įrašų.
   */
  void copy_table(u64int number, u64int *table) {

    u64int *new_table = (u64int *) (
        this->physical_address + 0x1000 * (this->tables + 1));

    for (int i = 0; i < 512; i++) {
      new_table[i] = table[i];
      }

    this->entry[number] = PAGE_TABLE_PERM_NOCACHE | ((u64int) new_table);

    }
  
  };


/**
 * Pačio branduolio puslapiavimo mechanizmas.
 */
class KernelPager {

private:

  // Atributai.

  u64int *entry;                        // Pirmojo lentelės įrašo adresas.
  u64int physical_address;              // Virtualiosios adresų erdvės 
                                        // viršutinės pusės realusis 
                                        // adresas.

public:

  // Metodai.

  /**
   * Konstruktorius.
   * @param address – kokiu adresu „padėti“ lentelę.
   * @param pml4_address – iš kur nukopijuoti struktūrą.
   */
  KernelPager(u64int table_address, u64int pml4_address) {

    this->entry = (u64int *) align(table_address);
    memset((u8int *)this->entry, 0, 0x1000);
                                        // Išvalom paskirties vietą.

    u64int *pml4 = (u64int *)(pml4_address & PAGE_TABLE_ADDRESS);

    // Perkuriame žemesniąją dalį.
    //  Pasižymime naujosios struktūros vietas.
    u64int *pdp_l_new = (u64int *)(align(table_address + 0x1000));
    u64int *pd_l_new = (u64int *)(align(table_address + 0x2000));

    //  Paruošiame atmintį.
    for (int i = 0; i < 512; i++) {     // Pirmas GB rodo į save.
      pd_l_new[i] = (
          PAGE_TABLE_PERM_NOCACHE | PAGE_TABLE_LARGE_FLAG | (0x200000 * i));
      }
    memset((u8int *) pdp_l_new, 0, 0x1000);

    //  Susaistome.
    pdp_l_new[0] = PAGE_TABLE_PERM_NOCACHE | (u64int) pd_l_new;
    this->entry[0] = ((u64int) pdp_l_new) | PAGE_TABLE_PERM_NOCACHE;

    // Perkuriame aukštesniąją dalį.
    //  Originali struktūra.
    u64int *pdp_h = (u64int *)(pml4[256] & PAGE_TABLE_ADDRESS);
    u64int *pd_h = (u64int *)(pdp_h[0] & PAGE_TABLE_ADDRESS);
    u64int *pt_h = (u64int *)(pd_h[0] & PAGE_TABLE_ADDRESS);

    this->physical_address = pt_h[0] & PAGE_TABLE_ADDRESS;

    //  Pasižymime naujosios struktūros vietas.
    u64int *pdp_h_new = (u64int *)(align(table_address + 0x3000));
    u64int *pd_h_new = (u64int *)(align(table_address + 0x4000));
    u64int *pt_h_new[8];                // 16 MB

    for (int i = 0; i < 8; i++) {
      pt_h_new[i] = (u64int *)(align(table_address + 0x5000 + i * 0x1000));
      }
    
    //  Paruošiame atmintį.
    memset((u8int *) pdp_h_new, 0, 0x1000);
    memset((u8int *) pd_h_new, 0, 0x1000);

    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 512; j++) {
        pt_h_new[i][j] = (
          PAGE_TABLE_PERM_NOCACHE | 
          (this->physical_address + i * 0x200000 + j * 0x1000));
        }
      }
    
    //  Susaistome.
    for (int i = 0; i < 8; i++) {
      pd_h_new[i] = PAGE_TABLE_PERM_NOCACHE | ((u64int) pt_h_new[i]); 
      }
    pdp_h_new[0] = PAGE_TABLE_PERM_NOCACHE | (u64int) pd_h_new;
    this->entry[256] = PAGE_TABLE_PERM_NOCACHE | (u64int) pdp_h_new;
    }

  /**
   * Nukopijuoja visą struktūrą.
   */
  void copy(u64int pml4_address) {

    u64int table_address = (u64int) this->entry;

    u64int *pml4 = (u64int *)(pml4_address & PAGE_TABLE_ADDRESS);

    // TODO: Vietoje kopijavimo perkurti.

    // Kopijuojame žemesniąją dalį.
    //  Originali struktūra.
    u64int *pdp_l = (u64int *)(pml4[0] & PAGE_TABLE_ADDRESS);
    u64int *pd_l = (u64int *)(pdp_l[0] & PAGE_TABLE_ADDRESS);

    //  Pasižymime naujosios struktūros vietas.
    u64int *pdp_l_new = (u64int *)(align(table_address + 0x1000));
    u64int *pd_l_new = (u64int *)(align(table_address + 0x2000));

    //  Paruošiame atmintį.
    memcpy((u8int *) pd_l_new, (u8int *)pd_l, 0x1000);
    memset((u8int *) pdp_l_new, 0, 0x1000);

    //  Susaistome.
    pdp_l_new[0] = PAGE_TABLE_PERM_NOCACHE | (u64int) pd_l_new;
    this->entry[0] = ((u64int) pdp_l_new) | PAGE_TABLE_PERM_NOCACHE;

    // Kopijuojame aukštesniąją dalį.
    //  Originali struktūra.
    u64int *pdp_h = (u64int *)(pml4[256] & PAGE_TABLE_ADDRESS);
    u64int *pd_h = (u64int *)(pdp_h[0] & PAGE_TABLE_ADDRESS);
    u64int *pt_h = (u64int *)(pd_h[0] & PAGE_TABLE_ADDRESS);

    //  Pasižymime naujosios struktūros vietas.
    u64int *pdp_h_new = (u64int *)(align(table_address + 0x3000));
    u64int *pd_h_new = (u64int *)(align(table_address + 0x4000));
    u64int *pt_h_new_1 = (u64int *)(align(table_address + 0x5000));

    //  Paruošiame atmintį.
    memset((u8int *) pdp_h_new, 0, 0x1000);
    memset((u8int *) pd_h_new, 0, 0x1000);
    memcpy((u8int *) pt_h_new_1, (u8int *)pt_h, 0x1000);

    //  Susaistome.
    pd_h_new[0] = PAGE_TABLE_PERM_NOCACHE | (u64int) pt_h_new_1;
    pdp_h_new[0] = PAGE_TABLE_PERM_NOCACHE | (u64int) pd_h_new;
    this->entry[256] = PAGE_TABLE_PERM_NOCACHE | (u64int) pdp_h_new;

    }

  /**
   * Aktyvuoja šį puslapiavimo mechanizmą.
   */
  void activate() {

    asm volatile("mov %0, %%cr3" : : "r"((u64int) this->entry));

    }

  /**
   * Grąžina nurodyto PML4 įrašą.
   */
  u64int get_entry(u64int index) {
    return this->entry[index];
    }
  
  };


/**
 * Programos puslapiavimo mechanizmas.
 */
class ProgramPager {

private:

  // Atributai.

public:
  u64int *entry;
  u64int physical_address;              // Virtualiosios adresų erdvės
                                        // apatinės pusės realusis
                                        // adresas.

  ProgramPager(): entry(0), physical_address(0) {
    }

  /**
   * Inicijuoja puslapiavimo mechanizmą.
   * @param table_address – kokiu adresu „padėti“ lentelę.
   * @param physical_address – virtualiosios adresų erdvės apatinės pusės
   * realusis adresas.
   * @param higher_half_pdp_address – kokiu adresu „padėtas“ PDP atsakingas
   * už branduolio virtualią atmintį (+ nustatymai).
   */
  void init(
      u64int table_address, u64int physical_address, 
      u64int higher_half_pdp_address) {

    this->physical_address = align(physical_address);
    this->entry = (u64int *) align(table_address);
    memset((u8int *) this->entry, 0, 0x1000);

    // Susiejame su branduolio PDP.
    this->entry[256] = higher_half_pdp_address;

    }

  /**
   * Sukuria apatiniąją atminties pusę. (Išskiria 16 MB.)
   */
  void create_lower() {

    u64int table_address = (u64int) this->entry;

    //  Pasižymime naujosios struktūros vietas.
    u64int *pdp_l_new = (u64int *)(align(table_address + 0x1000));
    u64int *pd_l_new = (u64int *)(align(table_address + 0x2000));

    //  Paruošiame atmintį.
    memset((u8int *) pdp_l_new, 0, 0x1000);
    memset((u8int *) pd_l_new, 0, 0x1000);

    for (int i = 1; i < 9; i++) {       // Išskiriame 16 MB. (Pirmuosius 
                                        // 2 MB praleidžiame, nes vaizdas
                                        // yra nustatomas pagal virtualų
                                        // adresą, tai jei leisime 
                                        // programoms rašyti į pirmąjį MB,
                                        // vaizdas mirgės.)
      pd_l_new[i] = (
          PAGE_TABLE_PERM_NOCACHE | 
          PAGE_TABLE_LARGE_FLAG | 
          (this->physical_address + 0x200000 * i)
          );
      }

    // Susaistome.
    pdp_l_new[0] = PAGE_TABLE_PERM_NOCACHE | ((u64int) pd_l_new); 
    this->entry[0] = PAGE_TABLE_PERM_NOCACHE | ((u64int) pdp_l_new);
    }
  
  /**
   * Išvalo apatiniąją atminties pusę.
   */
  void clear_lower() {
    memset((u8int *) this->entry, 0, 0x0800);
    }

  /**
   * Aktyvuoja šį puslapiavimo mechanizmą.
   */
  void activate() {

    asm volatile("mov %0, %%cr3" : : "r"((u64int) this->entry));

    }
  
  };

#endif
