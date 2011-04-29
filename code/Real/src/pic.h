#ifndef PIC_H
#define PIC_H

typedef void (*isr_t)(struct context_s *);

// A struct describing an interrupt gate.
struct interrupt_gate_s
{
    u16int offset_0_15;       // The lower 16 bits of the address to jump
                              // to when this interrupt fires. (base_lo)
    u16int selector;          // Kernel segment selector.
    Byte  zero;               // This must always be zero.
    Byte  flags;              // More flags. See documentation.
    u16int offset_16_31;      // The upper 16 bits of the address 
                              // to jump to. (base_hi)

    u32int offset_32_63;       // Higher half offset.
    u32int reserved;           // System reserved bytes
} __attribute__((packed));
typedef struct interrupt_gate_s idt_entry_t;

// A struct describing a pointer to an array of interrupt handlers.
// This is in a format suitable for giving to 'lidt'.
struct idtr_s
{
    u16int    limit;
    u64int    base;            // The address of 
                              // the first element in idt_entry_t array.
} __attribute__((packed));
typedef struct idtr_s idtr_t;

#endif
