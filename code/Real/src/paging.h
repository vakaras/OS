#ifndef PAGING_H 
#define PAGING_H 1

#include "types.h"

typedef struct page
{
   u64int present    : 1;   // Page present in memory
   u64int rw         : 1;   // Read-only if clear, readwrite if set
   u64int user       : 1;   // Supervisor level only if clear
   u64int accessed   : 1;   // Has the page been accessed since last refresh?
   u64int dirty      : 1;   // Has the page been written to since last refresh?
   u64int unused     : 7;   // Amalgamation of unused and reserved bits
   u64int frame      : 20;  // Frame address (shifted right 12 bits)
} page_t;

typedef struct page_table
{
   page_t pages[1024];
} page_table_t;

typedef struct page_directory
{
   /**
      Array of pointers to pagetables.
   **/
   page_table_t *tables[1024];
   /**
      Array of pointers to the pagetables above, but gives their *physical*
      location, for loading into the CR3 register.
   **/
   u64int tablesPhysical[1024];
   /**
      The physical address of tablesPhysical. This comes into play
      when we get our kernel heap allocated and the directory
      may be in a different location in virtual memory.
   **/
   u64int physicalAddr;
} page_directory_t;

void initialise_paging();

/**
 C auses the specified page directory to be loaded into the       *
 CR3 register.
 **/
void switch_page_directory(page_directory_t *);

/**
 R etrieves a pointer to the page required.                       *
 If make == 1, if the page-table in which this page should
 reside isn't created, create it!
 **/
page_t *get_page(u64int address, int make, page_directory_t *dir);

#endif
