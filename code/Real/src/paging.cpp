
#include "paging.h"
#include "memlib.h"

// The kernel's page directory
page_directory_t *kernel_directory=0;

// The current page directory;
page_directory_t *current_directory=0;

// A bitset of frames - used or free.
u64int *frames;
u64int nframes;

u64int placement_address = 16777215;


// Macros used in the bitset algorithms.
#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

// Static function to set a bit in the frames bitset
static void set_frame(u64int frame_addr)
{
  u64int frame = frame_addr/0x1000;
  u64int idx = INDEX_FROM_BIT(frame);
  u64int off = OFFSET_FROM_BIT(frame);
  frames[idx] |= (0x1 << off);
}

// Static function to clear a bit in the frames bitset
static void clear_frame(u64int frame_addr)
{
  u64int frame = frame_addr/0x1000;
  u64int idx = INDEX_FROM_BIT(frame);
  u64int off = OFFSET_FROM_BIT(frame);
  frames[idx] &= ~(0x1 << off);
}

// Static function to test if a bit is set.
static u64int test_frame(u64int frame_addr)
{
  u64int frame = frame_addr/0x1000;
  u64int idx = INDEX_FROM_BIT(frame);
  u64int off = OFFSET_FROM_BIT(frame);
  return (frames[idx] & (0x1 << off));
}

// Static function to find the first free frame.
static u64int first_frame()
{
  u64int i, j;
  for (i = 0; i < INDEX_FROM_BIT(nframes); i++)
  {
    if (frames[i] != 0xFFFFFFFFFFFFFFFF) // nothing free, exit early.
       {
         // at least one bit is free here.
         for (j = 0; j < 64; j++)
         {
           u64int toTest = 0x1 << j;
           if ( !(frames[i]&toTest) )
           {
             return i*4*8*2+j;
           }
         }
       }
  }
  return 0;
}

// Function to allocate a frame.
void alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
  if (page->frame != 0)
  {
    return; // Frame was already allocated, return straight away.
  }
  else
  {
    u64int idx = first_frame(); // idx is now the index of the first free frame.
    if (idx == (u64int)-1)
    {
//       PANIC("No free frames!");
    }
    set_frame(idx*0x1000); // this frame is now ours!
    page->present = 1; // Mark it as present.
    page->rw = (is_writeable)?1:0; // Should the page be writeable?
    page->user = (is_kernel)?0:1; // Should the page be user-mode?
    page->frame = idx;
  }
}

// Function to deallocate a frame.
void free_frame(page_t *page)
{
  u64int frame;
  if (!(frame=page->frame))
  {
    return; // The given page didn't actually have an allocated frame!
  }
  else
  {
    clear_frame(frame); // Frame is now free again.
    page->frame = 0x0; // Page now doesn't have a frame.
  }
}

void initialise_paging()
{
  // The size of physical memory. For the moment we
  // assume it is 16MB big.
  u64int mem_end_page = 0x1000000;
  
  nframes = mem_end_page / 0x1000;
  frames = (u64int*)kmalloc(INDEX_FROM_BIT(nframes));
  memset((u8int*)frames, 0, INDEX_FROM_BIT(nframes));
  
  // Let's make a page directory.
  kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
  memset((u8int*)kernel_directory, 0, sizeof(page_directory_t));
  current_directory = kernel_directory;
  
  // We need to identity map (phys addr = virt addr) from
  // 0x0 to the end of used memory, so we can access this
  // transparently, as if paging wasn't enabled.
  // NOTE that we use a while loop here deliberately.
  // inside the loop body we actually change placement_address
  // by calling kmalloc(). A while loop causes this to be
  // computed on-the-fly rather than once at the start.
  u64int i = 0;
  while (i < placement_address)
  {
    // Kernel code is readable but not writeable from userspace.
    alloc_frame( get_page(i, 1, kernel_directory), 0, 0);
    i += 0x1000;
  }
  // Before we enable paging, we must register our page fault handler.
  //register_interrupt_handler(14, page_fault);
  
  // Now, enable paging!
  switch_page_directory(kernel_directory);
}

void switch_page_directory(page_directory_t *dir)
{
  current_directory = dir;
  asm volatile("mov %0, %%cr3":: "r"(&dir->tablesPhysical));
  u64int cr0;
  asm volatile("mov %%cr0, %0": "=r"(cr0));
  cr0 |= 0x80000000; // Enable paging!
  asm volatile("mov %0, %%cr0":: "r"(cr0));
}

page_t *get_page(u64int address, int make, page_directory_t *dir)
{
  // Turn the address into an index.
  address /= 0x1000;
  // Find the page table containing this address.
  u64int table_idx = address / 1024;
  if (dir->tables[table_idx]) // If this table is already assigned
   {
     return &dir->tables[table_idx]->pages[address%1024];
   }
   else if(make)
   {
     u64int tmp;
     dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
     memset((u8int*)dir->tables[table_idx], 0, 0x1000);
     dir->tablesPhysical[table_idx] = tmp | 0x7; // PRESENT, RW, US.
     return &dir->tables[table_idx]->pages[address%1024];
   }
   else
   {
     return 0;
   }
}