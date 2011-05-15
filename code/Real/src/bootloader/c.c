/*
 * This file content was copied from JohnnyTheDon post at 
 * http://f.osdev.org/viewtopic.php?f=1&t=17548
 */
#pragma pack(1)

//Declare dbgwrite function from assembly
void _dbgwrite(char* str);

//Define a macro for dbgwrite
#define dbgwrite(str) _dbgwrite(str)

typedef struct _Module
{
   void* Start;
   void* End;
   char* Name;
   unsigned int Reserved;
} Module;
typedef struct _MemoryMap
{
   unsigned int Size;
   unsigned long long Address;
   unsigned long long Length;
   unsigned int Type;
} MemoryMap;
typedef struct _Drive
{
   unsigned int Size;
   unsigned char Number;
   unsigned char Mode;
   unsigned char Cylinders;
   unsigned char Heads;
   unsigned char Sectors;
   unsigned short Ports[];
} Drive;
typedef struct _VBEControlInfo
{
   unsigned char Signature[4];
   unsigned short Version;
   unsigned short OemString[2];
   unsigned char Capabilities[4];
   unsigned short VideoModes[2];
   unsigned short TotalMemory;
} VBEControlInfo;
typedef struct _VBEModeInfo
{
   unsigned short Attributes;
   unsigned char WinA;
   unsigned char WinB;
   unsigned short Granularity;
   unsigned short WinSize;
   unsigned short SegmentA;
   unsigned short SegmentB;
   unsigned short VBEFar;
   unsigned short Pitch;
   unsigned short XRes;
   unsigned short YRes;
   unsigned char WChar, YChar, Planes, BPP, Banks;
   unsigned char MemoryModel, BackSize, ImagePages;
   unsigned char Reserved;
   unsigned char RedMask, RedPosition, GreenMask, GreenPosition, BlueMask, BluePosition, RSVMask, RSVPosition, DirectcolorAttributes;
   unsigned int Physbase;
   unsigned int Reserved1;
   unsigned short Reserved2;
} VBEModeInfo;


typedef struct _Memory
{
   unsigned int Lower;
   unsigned int Upper;
} Memory;
typedef struct _BootDevice
{
   unsigned char Drive;
   unsigned char Partition1;
   unsigned char Partition2;
   unsigned char Partition3;
} BootDevice;
typedef struct _Modules
{
   unsigned int Count;
   Module* Address;
} Modules;
typedef struct _Symbols
{
   unsigned int Useless[4];
} Symbols;
typedef struct _MemoryMaps
{
   unsigned int Length;
   void* Address;
} MemoryMaps;
typedef struct _Drives
{
   unsigned int Length;
   Drive* Address;
} Drives;
typedef struct _ApmTable
{
   unsigned short Version;
   unsigned short CodeSegment;
   unsigned int Offset;
   unsigned short CodeSegment16;
   unsigned short DataSegment;
   unsigned short Flags;
   unsigned short CodeSegmentLength;
   unsigned short CodeSegment16Length;
   unsigned short DataSegmentLength;
} ApmTable;
typedef struct _VBE
{
   VBEControlInfo* ControlInfo;
   VBEModeInfo* ModeInfo;
   unsigned short Mode;
   unsigned short Segment;
   unsigned short Offset;
   unsigned short Length;
} VBE;

typedef struct _MultibootInfo
{
   unsigned int Flags;
   Memory Memory;
   BootDevice BootDevice;
   char* CommandLine;
   Modules Modules;
   Symbols Symbols;
   MemoryMaps MemoryMaps;
   Drives Drives;
   void* ConfigTable;
   char* BootloaderName;
   ApmTable* ApmTable;
   VBE VBE;
} MultibootInfo;

#define MEMORY_PRESENT (1<<0)
#define BOOTDEVICE_PRESET (1<<1)
#define COMMANDLINE_PRESENT (1<<2)
#define MODULES_PRESENT (1<<3)
#define SYMBOLS1_PRESENT (1<<4)
#define SYMBOLS2_PRESENT (1<<5)
#define MEMORYMAP_PRESENT (1<<6)
#define DRIVES_PRESENT (1<<7)
#define CONFIGTABLE_PRESENT (1<<8)
#define BOOTLOADERNAME_PRESENT (1<<9)
#define APMTABLE_PRESENT (1<<10)
#define VBEINFO_PRESENT (1<<11)

#define REQ_FLAGS (MEMORY_PRESENT | MEMORYMAP_PRESENT | MODULES_PRESENT | COMMANDLINE_PRESENT)



//Define some basic string functions. These aren't as efficient as the assembly versions, but they'll do.
int strlen(const char* str)
{
   for(int i = 0; ; i++)
   {
      if(str[i] == 0)
         return i;
   }
}
void strcpy(char* dst, const char* src)
{
   int len = strlen(src)+1;
   for(int i = 0; i < len; i++)
   {
      dst[i] = src[i];
   }
}
void strncpy(char* dst, const char* src, int len)
{
   for(int i = 0; i < len; i++)
   {
      dst[i] = src[i];
   }
}
int strcmp(const char* str1, const char* str2)
{
   int len = strlen(str1);
   if(len != strlen(str2))
      return 0;
   for(int i = 0; i < len; i++)
   {
      if(str1[i] != str2[i])
         return 0;
   }
   return 1;
}

void memcpy(void* dst, const void* src, int size, int count)
{
   for(int i = 0; i < (size*count); i++)
   {
      ((unsigned char*)dst)[i] = ((unsigned char*)src)[i];
   }
}

int MoveMultiboot(MultibootInfo* multiboot_addr, MultibootInfo* buffer_addr)
{
   *buffer_addr = *multiboot_addr; //Move the main multiboot structure (duh)
   if((multiboot_addr->Flags & REQ_FLAGS) != REQ_FLAGS) //Check if the required entries are present
      return 1;
   void* currentBufferAddr = buffer_addr+sizeof(MultibootInfo); //Gets the first address in the multiboot buffer avaliable for writing
   if(multiboot_addr->Flags & COMMANDLINE_PRESENT)
   {
      strcpy(currentBufferAddr, multiboot_addr->CommandLine); //Copy the command line to the buffer
      buffer_addr->CommandLine = currentBufferAddr; //Change the address of the command line in the new multiboot structure
      currentBufferAddr += strlen(multiboot_addr->CommandLine)+1; //Increment the current buffer address
   }
   if(multiboot_addr->Flags & MODULES_PRESENT)
   {
      if(multiboot_addr->Modules.Count == 0) //Makes sure module count != 0, this should never happen
         return 2;
      
      memcpy(currentBufferAddr, multiboot_addr->Modules.Address, sizeof(Module), multiboot_addr->Modules.Count); //Copy the module structures
      buffer_addr->Modules.Address = currentBufferAddr; //Change the address to module information in the new multiboot structure
      currentBufferAddr += sizeof(Module)*(multiboot_addr->Modules.Count); //Increment the current buffer address
      for(int i = 0; i < multiboot_addr->Modules.Count; i++)
      {
         strcpy(currentBufferAddr, multiboot_addr->Modules.Address[i].Name);
         buffer_addr->Modules.Address[i].Name = currentBufferAddr;
         currentBufferAddr += strlen(multiboot_addr->Modules.Address[i].Name)+1;
      }
   }
   if(multiboot_addr->Flags & MEMORYMAP_PRESENT)
   {
      memcpy(currentBufferAddr, multiboot_addr->MemoryMaps.Address, 1, multiboot_addr->MemoryMaps.Length); //Copy all the memory maps
      buffer_addr->MemoryMaps.Address = currentBufferAddr; //Change the address to the copies
      currentBufferAddr += multiboot_addr->MemoryMaps.Length; //Increment the current buffer address
   }
   if(multiboot_addr->Flags & DRIVES_PRESENT)
   {
      memcpy(currentBufferAddr, multiboot_addr->Drives.Address, 1, multiboot_addr->Drives.Length); //Copy the drives
      buffer_addr->Drives.Address = currentBufferAddr; //Change the address to the copies
      currentBufferAddr += multiboot_addr->Drives.Length; //Increment the current buffer address
   }
   if(multiboot_addr->Flags & CONFIGTABLE_PRESENT)
      multiboot_addr->Flags &= ~CONFIGTABLE_PRESENT; //clear the flag and act like we didn't get the config table
   if(multiboot_addr->Flags & BOOTLOADERNAME_PRESENT)
   {
      strcpy(currentBufferAddr, multiboot_addr->BootloaderName);
      buffer_addr->BootloaderName = currentBufferAddr;
      currentBufferAddr += strlen(multiboot_addr->BootloaderName)+1;
   }
   if(multiboot_addr->Flags & APMTABLE_PRESENT)
   {
      memcpy(currentBufferAddr, multiboot_addr->ApmTable, sizeof(ApmTable), 1);
      buffer_addr->ApmTable = currentBufferAddr;
      currentBufferAddr += sizeof(ApmTable);
   }
   if(multiboot_addr->Flags & VBEINFO_PRESENT)
   {
      memcpy(currentBufferAddr, multiboot_addr->VBE.ControlInfo, sizeof(VBEControlInfo), 1);
      buffer_addr->VBE.ControlInfo = currentBufferAddr;
      currentBufferAddr += sizeof(VBEControlInfo);
      memcpy(currentBufferAddr, multiboot_addr->VBE.ModeInfo, sizeof(VBEModeInfo), 1);
      buffer_addr->VBE.ModeInfo = currentBufferAddr;
      currentBufferAddr += sizeof(VBEModeInfo);
   }
}

#define PAGE_TABLE_PERM ((1<<0) | (1<<1))
#define PAGE_TABLE_PERM_NOCACHE (PAGE_TABLE_PERM | (1<<3) | (1<<4))
#define PAGE_TABLE_LARGE_FLAG (1<<7)

typedef unsigned long long PML4;
typedef unsigned long long PDP;
typedef unsigned long long PD;
typedef unsigned long long PT;

typedef struct _ElfHeader
{
   unsigned char Identification[16];
   unsigned short Type;
   unsigned short Machine;
   unsigned long Version;
   unsigned long long Entry;
   unsigned long long ProgramHeaderTableOffset;
   unsigned long long SectionHeaderTableOffset;
   unsigned long Flags;
   unsigned short HeaderSize;
   unsigned short ProgramHeaderEntrySize;
   unsigned short NumberProgramHeaderEntry;
   unsigned short SectionHeaderEntrySize;
   unsigned short NumberSectionHeaderEntry;
   unsigned short SectionNameTableIndex;
} ElfHeader;

typedef struct _ElfProgramHeader
{
   unsigned long Type;
   unsigned long Flags;
   unsigned long long Offset;
   unsigned long long VAddr;
   unsigned long long PAddr;
   unsigned long long FileSize;
   unsigned long long MemSize;
   unsigned long long Alignment;
} ElfProgramHeader;

void* align(void* addr)
{
   long iaddr = (long)addr;
   if(iaddr % 4096 == 0)
      return (void*)iaddr;
   iaddr += (4096-(iaddr % 4096));
}

int BuildPageTables(MultibootInfo* multiboot, unsigned long long* entrypoint, void* kernel_buffer, PML4* pml4, PDP* pdp1, PDP* pdp2, PD* pd1, PD* pd2, PT* pt1)
{
   pml4[0] = PAGE_TABLE_PERM_NOCACHE | (long)pdp1; //Builds the page table structure
   pml4[256] = PAGE_TABLE_PERM_NOCACHE | (long)pdp2;
   pdp1[0] = PAGE_TABLE_PERM_NOCACHE | (long)pd1;
   pdp2[0] = PAGE_TABLE_PERM_NOCACHE | (long)pd2;
   pd2[0] = PAGE_TABLE_PERM_NOCACHE | (long)pt1;
   for(int i = 0; i < 512; i++) //Maps first GB of physical addresses to themselves in 2MB pages
   {
      pd1[i] = PAGE_TABLE_PERM_NOCACHE | PAGE_TABLE_LARGE_FLAG | (0x200000*i);
   }
   Module* kernel_mod = multiboot->Modules.Address; //Kernel is the first module loaded
   ElfHeader* kernel = kernel_mod->Start;
   /*if(!((kernel->Identification[1]) == 'E' && (kernel->Identification[2] == 'L') && (kernel->Identification[3] == 'F'))); //Check ELF signature
      return 1;*/
   if(kernel->Identification[4] != 2) //Check Elf Class (make sure its 64-bit)
      return 2;
   if(kernel->Identification[5] != 1) //Check Edianess (make sure its little)
      return 3;
   if(kernel->Type != 2) //Check File Type (make sure its executable)
      return 4;
   *entrypoint = kernel->Entry;
   void* currentKernelBuffer = kernel_buffer;
   for (unsigned long long i = 0; i < 512; i++) {
     pt1[i] = PAGE_TABLE_PERM_NOCACHE | (((unsigned long long)currentKernelBuffer) + i * 0x1000);
     }
   for(int i = 0; i < kernel->NumberProgramHeaderEntry; i++) //Loop through the program headers and do what they say
   {
      ElfProgramHeader* program = (void*)kernel+((long)kernel->ProgramHeaderTableOffset)+(i*((long)kernel->ProgramHeaderEntrySize)); //get the program header
      memcpy(currentKernelBuffer, (void*)kernel+program->Offset, 1, (int)program->FileSize); //Copy the data
      for(unsigned long long j = 0; j <= program->MemSize; j += 4096) //Write page table entries for all the pages
      {
         pt1[(program->VAddr+j)/4096] = PAGE_TABLE_PERM_NOCACHE | (((unsigned long long)currentKernelBuffer)+j); //Write necessary page table entries
      }
      currentKernelBuffer += program->MemSize; //Increment buffer address
      currentKernelBuffer = align(currentKernelBuffer);
   }
   return 0;
}
