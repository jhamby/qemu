/*
 * Amiga chipset state (AGA custom chips + CIAs).
 *
 * Copyright (c) 2021 Jake Hamby
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef AMIGA_CHIPSET_H
#define AMIGA_CHIPSET_H

#include "hw/m68k/amiga-chipset-regs.h"

/* Declare typedefs for all of the chipset components. */

typedef struct AudioChannelState AudioChannelState;
typedef struct AudioMixState AudioMixState;
typedef struct BitplaneState BitplaneState;
typedef struct BlitterState BlitterState;
typedef struct CIACounterState CIACounterState;
typedef struct CIATimerState CIATimerState;
typedef struct CollisionState CollisionState;
typedef struct CopperState CopperState;
typedef struct DisplayState DisplayState;
typedef struct DMAChannelState DMAChannelState;
typedef struct FloppyControlState FloppyControlState;
typedef struct FloppyDriveState FloppyDriveState;
typedef struct InterruptState InterruptState;   /* including Gary control */
typedef struct KeyboardState KeyboardState;
typedef struct MousePortState MousePortState;
typedef struct PaletteState PaletteState;
typedef struct PlayfieldState PlayfieldState;
typedef struct PrinterPortState PrinterPortState;
typedef struct RamControlState RamControlState;     /* Ramsey register */
typedef struct SerialPortState SerialPortState;
typedef struct SpriteState SpriteState;

/* State for a single audio channel. */
struct AudioChannelState {
    uint32_t data_address;
    uint32_t current_dma_address;
    uint16_t data_length_words;
    uint16_t period;
    uint16_t dac_data;              /* two samples or 1 volume/period */
    uint8_t volume;                 /* volumes are 0 to 64 (not 63) */
};

/* Audio volume and modulation state. */
struct AudioMixState {

};

/* State for one bitplane. */
struct BitplaneState {
    uint64_t dma_data;      /* shift register value for this bitplane */
};

/* Bit blitter state. */
struct BlitterState {
    bool busy;
    bool priority;
    bool zero_status;
    bool line_mode;
    bool reverse_mode;
};

/* State for one CIA "TOD" counter. */
struct CIACounterState {

};

/* State for one CIA timer. */
struct CIATimerState {

};

/* Display collision detection state. */
struct CollisionState {
    uint16_t bits;          /* Collision data (same bits as CLXDAT) */
};

/* Copper state. */
struct CopperState {
    uint32_t location[2];   /* first and second location register */
    uint32_t current_address;

    enum {
        COPPER_IDLE,
        COPPER_LOADING_WORD_1,
        COPPER_LOADING_WORD_2,
        COPPER_WAITING_FOR_BEAM,
        COPPER_WAKING_UP
    } state;

    bool skip_next_move;    /* last instruction was a successful SKIP */
    bool write_all;         /* enable writes to registers below 0x40 */
};

/* Display control and CRT beam state. */
struct DisplayState {
    uint16_t beam_x_pos;            /* 11 bits */
    uint16_t beam_y_pos;            /* 9 bits */

    uint16_t alice_x_counter;       /* 11 bits */
    uint16_t alice_y_counter;       /* 9 bits */

    uint16_t x_total;
    uint16_t y_total;

    uint16_t bitplane_modulo[2];    /* odd and even bitplanes */

    uint16_t x_pos_hsync_start;
    uint16_t x_pos_hsync_stop;
    uint16_t x_pos_vsync_interlace;     /* HCENTER */
    uint16_t y_pos_vsync_start;
    uint16_t y_pos_vsync_stop;

    uint16_t x_pos_hblank_start;
    uint16_t x_pos_hblank_stop;
    uint16_t y_pos_vblank_start;
    uint16_t y_pos_vblank_stop;

    /* flags from VPOSW and Alice timing */
    bool long_frame;
    bool long_line;

    /* BPLCONx bits */
    bool bitplane_scan_double;
    bool sprite_scan_double;
    bool kill_ehb_mode;
    bool playfield_2_priority;

    uint16_t bplcon0_bits;                  /* BPLCON0 register */
    uint8_t bitplanes_used;                 /* bitplanes used (0-8) */
    uint8_t sprite_resolution;              /* ECS, LORES, HIRES, SHRES */

    /* AGA DMA fetch modes */
    uint8_t bitplane_fetch_size;    /* 0 = 1 word, 1 = 2 words, 2 = 4 words */
    uint8_t sprite_fetch_size;
};

/* DMA channel enable and request state. */
struct DMAChannelState {
    uint16_t control_bits;                  /* DMA / feature enable */
};

/* Floppy drive controller state. */
struct FloppyControlState {
    uint32_t dma_address;
    uint16_t dma_length;        /* number of words of DMA data */
    uint16_t dma_data_buffer;   /* the current word being read/written */
    bool dma_enable;            /* bit 15 of DSKLEN */
    bool disk_write;            /* bit 14 of DSKLEN */
    bool disk_really_write;     /* write bit has been written twice */
};

/* Floppy per-drive state. */
struct FloppyDriveState {
    bool motor_state;
    bool disk_inserted;
    uint8_t current_track;
    uint8_t current_head;
    /* TODO: define disk types and mapping to host .adf / .img files. */
};

/* Interrupt controller state. */
struct InterruptState {
    uint16_t interrupt_enable_bits;
    uint16_t interrupt_request_bits;
};

/* Keyboard controller / scancode state. */
struct KeyboardState {
    uint8_t buffer_length;
    uint8_t z_pos_delivered;    /* mouse wheel position delivered to guest */
    uint8_t buffer[KEYBOARD_BUFFER_LEN];
};

/* Mouse port state. */
struct MousePortState {
    uint8_t x_pos;
    uint8_t y_pos;
    uint8_t z_pos;          /* mouse wheel events turn into keyboard events */
    uint8_t button_state;
};

/* Color palette state. */
struct PaletteState {
    bool read_color_table;
    bool update_low_nibble;
    uint8_t register_bank_offset;   /* bank offset: 0, 32 ... 224 */

    /* 24-bit color palette registers (in host byte order) */
    uint32_t color[MAX_NUM_COLORS];
};

/* Playfield state. */
struct PlayfieldState {
    uint8_t hscroll_delay;
    uint8_t color_offset;           /* PF2 offset: 0, 2, 4 ... 128 */
    uint8_t priority_vs_sprites;    /* priority 0 (front) to 4 */
};

/* Printer port state. */
struct PrinterPortState {

};

/* RAM control state (Ramsey register). */
struct RamControlState {

};

/* Serial port state. */
struct SerialPortState {

};

/* Per-sprite state (global sprite state is in DisplayState). */
struct SpriteState {
    uint64_t dma_data;      /* shift register value for this sprite */
};

/*
 * Main Amiga chipset type, containing subcomponents and QEMU objects.
 */
typedef struct AmigaChipset {
    SysBusDevice parent_obj;

    /* system configuration (only one property for now) */
    bool ntsc;                          /* default is PAL (false) */

    /* link to CPU for raising/lowering the m68k interrupt level */
    M68kCPU *cpu;

    /* 2 MiB of shared chip RAM */
    MemoryRegion chip_ram;

    /* Cycle counter in days and DMA cycles (for conversion accuracy). */
    uint64_t last_cycle_emulated;
    uint32_t uptime_days;

    /* QEMU framebuffer */
    QemuConsole *con;

    /* This inner struct can be memset to zero to reset the machine state. */
    struct {
        AudioChannelState audio[NUM_AUDIO_CHANNELS];
        AudioMixState audio_mix;
        BitplaneState bitplane[MAX_NUM_BITPLANES];
        BlitterState blitter;
        CIACounterState counter[2]; /* TODO: define this. */
        CIATimerState timer[4]; /* TODO: define this. */
        CollisionState collision;
        CopperState copper;
        DisplayState display;
        DMAChannelState dma;
        FloppyControlState fdc;
        FloppyDriveState drive[MAX_NUM_DRIVES];
        InterruptState irq;
        KeyboardState keyboard;
        MousePortState mouse_port[NUM_MOUSE_PORTS];
        PlayfieldState playfield[MAX_NUM_PLAYFIELDS];
        PrinterPortState parallel_port;
        RamControlState ram_control;
        SerialPortState serial_port;
        SpriteState sprite[MAX_NUM_SPRITES];
        /* Note: insert any additional states above this line. */

        PaletteState palette;   /* end with the large color palette */
    } state;

} AmigaChipset;

/*
 * Amiga chipset VM state definition.
 * Note: "ntsc" is a property, so it should hopefully already be saved.
 */
static const VMStateDescription amiga_chipset_vmstate = {
    .name = TYPE_AMIGA_CHIPSET,
    .version_id = 0,
    .minimum_version_id = 0,
    .fields = (VMStateField[]) {
        VMSTATE_UINT64(last_cycle_emulated, AmigaChipset),
        VMSTATE_UINT16(state.display.beam_x_pos, AmigaChipset),
        VMSTATE_UINT16(state.display.beam_y_pos, AmigaChipset),
        VMSTATE_UINT16(state.irq.interrupt_enable_bits, AmigaChipset),
        VMSTATE_UINT16(state.irq.interrupt_request_bits, AmigaChipset),
        /* TODO: fill in this list after I've finalized the fields */
        VMSTATE_END_OF_LIST()
    },
};

OBJECT_DECLARE_SIMPLE_TYPE(AmigaChipset, AMIGA_CHIPSET)


/* Old definitions below... */

#if 0
/* Current CPU request type, shifted and OR'd with the address. */
enum AmigaChipsetRequestType {
    NO_COMMAND,
    REGISTER_READ,
    REGISTER_WRITE,
    CIA_READ,
    CIA_WRITE,
    GARY_READ,
    GARY_WRITE,
};


/* Number of bits to shift the request type before OR'ing */
#define CHIPSET_REQUEST_TYPE_SHIFT      24

/* Number of bits to shift the word offset before OR'ing */
#define CHIPSET_WORD_OFFSET_SHIFT       16

/* Registers and state for Alice (AGA version of Agnus) */
typedef struct {
    uint16_t dma_control_bits;              /* DMA / feature enable */

    /* Display DMA registers and counters */
    struct display_state {
        uint16_t vertical_counter;          /* 11 bits */
        uint16_t horizontal_counter;        /* 9 bits */

        uint16_t vertical_total;
        uint16_t horizontal_total;

        uint16_t bitplane_modulo[2];    /* odd and even bitplanes */

        uint16_t vert_pos_vsync_start;
        uint16_t vert_pos_vsync_stop;
        uint16_t horiz_pos_hsync_start;
        uint16_t horiz_pos_hsync_stop;
        uint16_t horiz_pos_vsync_interlace;     /* HCENTER */

        uint16_t horiz_pos_hblank_start;
        uint16_t horiz_pos_hblank_stop;
        uint16_t vert_pos_vblank_start;
        uint16_t vert_pos_vblank_stop;
    } display;

    /* Copper registers and state */
    struct copper_state {
        uint32_t location[2];   /* first and second location register */
        uint32_t current_address;
        uint8_t state;
        bool skip_next_move;    /* last instruction was a successful SKIP */
        bool write_all;         /* enable writes to registers below 0x40 */
    } copper;

    /* Blitter registers and state */
    struct blitter_state {
        bool busy;
        bool priority;
        bool zero_status;
        bool line_mode;
        bool reverse_mode;
    } blitter;

} AliceState;

/* Registers and state for Lisa (AGA version of Denise) */
typedef struct {
    /* flags from VPOSW and Alice timing */
    bool long_frame;
    bool long_line;

    /* BPLCONx bits */
    bool color_palette_low_nibble;
    bool read_color_table;
    bool bitplane_scan_double;
    bool sprite_scan_double;
    bool kill_ehb_mode;
    bool playfield_2_priority;

    uint16_t bplcon0_bits;                  /* BPLCON0 register */
    uint8_t bitplanes_used;                 /* bitplanes used (0-8) */
    uint8_t playfield_hscroll_delay[2];     /* hscroll delay for PF1/PF2 */
    uint8_t color_register_bank_offset;     /* bank offset: 0, 32 ... 224 */
    uint8_t playfield_2_color_offset;       /* PF2 offset: 0, 2, 4 ... 128 */
    uint8_t sprite_resolution;              /* ECS, LORES, HIRES, SHRES */

    /* Note: the PF2 priority bits are used for single playfields */
    /* FIXME: this should be an array of length 2, once I know which order */
    uint8_t playfield_2_pri_vs_sprites;     /* priority 0 (front) to 4 */
    uint8_t playfield_1_pri_vs_sprites;     /* priority 0 (front) to 4 */

    uint16_t collision_data;        /* Collision data (same bits as CLXDAT) */

    /* AGA DMA fetch modes */
    uint8_t bitplane_fetch_size;    /* 0 = 1 word, 1 = 2 words, 2 = 4 words */
    uint8_t sprite_fetch_size;

    /* bitplane and sprite DMA shift registers */
    uint64_t bitplane_dma_data[MAX_NUM_BITPLANES];
    uint64_t sprite_dma_data[MAX_NUM_SPRITES];

    /* 24-bit color palette registers (in host byte order) */
    uint32_t color_palette[MAX_NUM_COLORS];

} LisaState;

/* Registers and state for Paula */
typedef struct {
    uint16_t audio_disk_uart_bits;      /* ADKCON */
    uint16_t interrupt_enable_bits;
    uint16_t interrupt_request_bits;

    /* Audio state */
    struct audio_state {
        uint32_t data_address;
        uint32_t current_dma_address;
        uint16_t data_length_words;
        uint16_t period;
        uint16_t dac_data;              /* two samples or 1 volume/period */
        uint8_t volume;                 /* volumes are 0 to 64 (not 63) */
    } audio[NUM_AUDIO_CHANNELS];

    /* Floppy drive controller */
    struct floppy_state {
        uint32_t dma_address;
        uint16_t dma_length;        /* number of words of DMA data */
        uint16_t dma_data_buffer;   /* the current word being read/written */
        bool dma_enable;            /* bit 15 of DSKLEN */
        bool disk_write;            /* bit 14 of DSKLEN */
        bool disk_really_write;     /* write bit has been written twice */

        /* TODO: finish per-drive state */
        struct drive_state {
            bool motor_state;
            bool disk_inserted;
            uint8_t current_track;
            uint8_t current_head;
        } drive[MAX_NUM_DRIVES];
    } floppy;

} PaulaState;


struct OldAmigaChipset {
    SysBusDevice parent_obj;

    /* thread-safe CPU request that blocks for a response */
    QemuSemaphore cpu_request_sem;

    /* atomically set by main thread */
    uint32_t cpu_request_type_offset_value;
    bool exit_event_loop;
    bool reset_event_loop;

    /* system configuration (only one property for now) */
    bool ntsc;                          /* default is PAL (false) */

    /* position of the emulated CRT beam */
    uint16_t beam_vert_position;        /* 11 bits */
    uint16_t beam_horiz_position;        /* 9 bits */

    /* Mouse state (only on port 1, until QEMU supports multiple mice) */
    uint8_t mouse_0_x_pos;
    uint8_t mouse_0_y_pos;
    uint8_t mouse_0_z_pos;      /* mouse wheel turns into keyboard events */
    uint8_t buttons_state;

    /* register state for each custom chip */
    AliceState alice;
    PaulaState paula;
    CIAState cia[2];
    GaryState gary;
    LisaState lisa;     /* put Lisa last, due to the big color palette */

    /* TODO: move the state below into the appropriate chip structs */

    /* link to CPU for raising/lowering the m68k interrupt level */
    M68kCPU *cpu;

    /* 2 MiB of shared chip RAM */
    MemoryRegion chip_mem;

    /* timing and video beam state (copied to local vars) */
    int64_t last_cycle_emulated;
    uint32_t uptime_days;

    /* callback to set the m68k interrupt level */
    QEMUBH *set_int_level_bh;

    /* callback to swap display buffers */
    QEMUBH *swap_console_buffers_bh;

    /* QEMU framebuffer */
    QemuConsole *con;

    /* Note: do I need to keep a copy here? */
    QemuThread chipset_worker_thread;

    /* TODO: QEMU audio stuff */
    /* TODO: QEMU input event stuff */
};
#endif

#endif /* AMIGA_CHIPSET_H */
