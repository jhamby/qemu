/*
 * Amiga chipset registers (AGA custom chips + CIAs).
 *
 * Copyright (c) 2021 Jake Hamby
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef AMIGA_CHIPSET_REGS_H
#define AMIGA_CHIPSET_REGS_H

/* Generic helper macros */

#define LOW_WORD(x)             ((x) & 0x0000ffff)
#define HIGH_WORD(x)            ((x) & 0xffff0000)
#define HIGH_WORD_RSHIFT(x)     ((x) >> 16)
#define HIGH_WORD_LSHIFT(x)     ((uint32_t)(x) << 16)
#define WORD_ADDRESS_MASK(x)    ((x) & 0xfffe)      /* mask odd addresses */
#define CHIP_RAM_HIGH_MASK(x)   ((x) & 0x001f)      /* 2 MiB chip RAM limit */

/* Mask and replace the high or low 16-bit word in a register. */
#define REPLACE_HIGH(x, val)    { x = (LOW_WORD(x) | HIGH_WORD_LSHIFT(val)); }
#define REPLACE_LOW(x, val)     { x = (HIGH_WORD(x) | LOW_WORD(val)); }

/* Pre-shifted ID bits for Alice 8374 rev 3 and 4 (NTSC and PAL variants). */
#define NTSC_ALICE_VERSION_BITS     (0x33 << 8)
#define PAL_ALICE_VERSION_BITS      (0x23 << 8)

#define LISA_VERSION    0x00f8      /* Lisa chip ID (AGA) */

#define NUM_AUDIO_CHANNELS      4
#define NUM_MOUSE_PORTS         2

#define MAX_NUM_BITPLANES       8
#define MAX_NUM_COLORS          256
#define MAX_NUM_DRIVES          4
#define MAX_NUM_PLAYFIELDS      2
#define MAX_NUM_SPRITES         8

#define KEYBOARD_BUFFER_LEN     16  /* slightly larger than real keyboards */

/* Custom chip registers in address order, as 16-bit word offsets */

/* read-only registers */
#define BLTDDAT     (0x0000 >> 1)   /* blitter dest. early read (dummy) */
#define DMACONR     (0x0002 >> 1)   /* DMA control and blitter status */
#define VPOSR       (0x0004 >> 1)   /* highest bits of vpos, frame flop */
#define VHPOSR      (0x0006 >> 1)   /* vert. and horiz. beam position */
#define DSKDATR     (0x0008 >> 1)   /* disk data early read (dummy) */
#define JOY0DAT     (0x000a >> 1)   /* joystick/mouse 0 position */
#define JOY1DAT     (0x000c >> 1)   /* joystick/mouse 1 position */
#define CLXDAT      (0x000e >> 1)   /* collision data (read / clear) */
#define ADKCONR     (0x0010 >> 1)   /* audio and disk control */
#define POT0DAT     (0x0012 >> 1)   /* pot counter pair 0 data */
#define POT1DAT     (0x0014 >> 1)   /* pot counter pair 1 data */
#define POTGOR      (0x0016 >> 1)   /* pot port data read */
#define SERDATR     (0x0018 >> 1)   /* serial port data and status */
#define DSKBYTR     (0x001a >> 1)   /* disk data byte and status */
#define INTENAR     (0x001c >> 1)   /* interrupt enable bits */
#define INTREQR     (0x001e >> 1)   /* interrupt request bits */
#define DENISEID    (0x007c >> 1)   /* chip revision for Denise / Lisa */
#define HHPOSR      (0x01da >> 1)   /* dual mode hires h-beam counter read */

/* write-only and strobe registers */
#define DSKPTH      (0x0020 >> 1)   /* disk pointer (high word) */
#define DSKPTL      (0x0022 >> 1)   /* disk pointer (low word) */
#define DSKLEN      (0x0024 >> 1)   /* disk length */
#define DSKDAT      (0x0026 >> 1)   /* disk DMA data write */
#define REFPTR      (0x0028 >> 1)   /* DRAM refresh pointer */
#define VPOSW       (0x002a >> 1)   /* highest bits of vpos, frame flop */
#define VHPOSW      (0x002c >> 1)   /* vert. and horiz. beam position */
#define COPCON      (0x002e >> 1)   /* copper control register */
#define SERDAT      (0x0030 >> 1)   /* serial port data and stop bits */
#define SERPER      (0x0032 >> 1)   /* serial port period and control */
#define POTGO       (0x0034 >> 1)   /* pot port data write and start */
#define JOYTEST     (0x0036 >> 1)   /* write to all joystick-mouse counters */
#define STREQU      (0x0038 >> 1)   /* strobe for hsync with vblank and EQU */
#define STRVBL      (0x003a >> 1)   /* strobe for hsync with vblank */
#define STRHOR      (0x003c >> 1)   /* strobe for hsync */
#define STRLONG     (0x003e >> 1)   /* strobe for long horizontal line */
#define BLTCON0     (0x0040 >> 1)   /* blitter control register 0 */
#define BLTCON1     (0x0042 >> 1)   /* blitter control register 1 */
#define BLTAFWM     (0x0044 >> 1)   /* blitter first word mask for source A */
#define BLTALWM     (0x0046 >> 1)   /* blitter last word mask for source A */
#define BLTCPTH     (0x0048 >> 1)   /* blitter ptr. to source C (high word) */
#define BLTCPTL     (0x004a >> 1)   /* blitter ptr. to source C (low word) */
#define BLTBPTH     (0x004c >> 1)   /* blitter ptr. to source B (high word) */
#define BLTBPTL     (0x004e >> 1)   /* blitter ptr. to source B (low word) */
#define BLTAPTH     (0x0050 >> 1)   /* blitter ptr. to source A (high word) */
#define BLTAPTL     (0x0052 >> 1)   /* blitter ptr. to source A (low word) */
#define BLTDPTH     (0x0054 >> 1)   /* blitter ptr. to dest. D (high word) */
#define BLTDPTL     (0x0056 >> 1)   /* blitter ptr. to dest. D (low word) */
#define BLTSIZE     (0x0058 >> 1)   /* blitter start, size (width, height) */
#define BLTCON0L    (0x005a >> 1)   /* blitter ctrl. 0, low byte (minterms) */
#define BLTSIZV     (0x005c >> 1)   /* blitter vsize (for 15-bit vsize) */
#define BLTSIZH     (0x005e >> 1)   /* blitter hsize, start (11-bit hsize) */
#define BLTCMOD     (0x0060 >> 1)   /* blitter modulo for source C */
#define BLTBMOD     (0x0062 >> 1)   /* blitter modulo for source B */
#define BLTAMOD     (0x0064 >> 1)   /* blitter modulo for source A */
#define BLTDMOD     (0x0066 >> 1)   /* blitter modulo for destination D */
#define BLTCDAT     (0x0070 >> 1)   /* blitter source C data register */
#define BLTBDAT     (0x0072 >> 1)   /* blitter source B data register */
#define BLTADAT     (0x0074 >> 1)   /* blitter source A data register */
#define SPRHDAT     (0x0078 >> 1)   /* ext. logic UHRES sprite ptr & id */
#define BPLHDAT     (0x007a >> 1)   /* ext. logic UHRES bit plane id */
#define DSKSYNC     (0x007e >> 1)   /* disk sync pattern for disk read */
#define COP1LCH     (0x0080 >> 1)   /* copper first location (high word) */
#define COP1LCL     (0x0082 >> 1)   /* copper first location (low word) */
#define COP2LCH     (0x0084 >> 1)   /* copper second location (high word) */
#define COP2LCL     (0x0086 >> 1)   /* copper second location (low word) */
#define COPJMP1     (0x0088 >> 1)   /* copper restart at first location */
#define COPJMP2     (0x008a >> 1)   /* copper restart at second location */
#define COPINS      (0x008c >> 1)   /* copper instruction fetch identify */
#define DIWSTRT     (0x008e >> 1)   /* display window start (upper-left) */
#define DIWSTOP     (0x0090 >> 1)   /* display window stop (lower-right) */
#define DDFSTRT     (0x0092 >> 1)   /* bitplane data fetch start (horiz.) */
#define DDFSTOP     (0x0094 >> 1)   /* bitplane data fetch stop (horiz.) */
#define DMACON      (0x0096 >> 1)   /* DMA control write (clear / set) */
#define CLXCON      (0x0098 >> 1)   /* collision control bits */
#define INTENA      (0x009a >> 1)   /* interrupt enable bits (clear / set) */
#define INTREQ      (0x009c >> 1)   /* interrupt request bits (clear / set) */
#define ADKCON      (0x009e >> 1)   /* audio, disk, UART control */
#define AUD0LCH     (0x00a0 >> 1)   /* audio channel 0 location (high word) */
#define AUD0LCL     (0x00a2 >> 1)   /* audio channel 0 location (low word) */
#define AUD0LEN     (0x00a4 >> 1)   /* audio channel 0 length */
#define AUD0PER     (0x00a6 >> 1)   /* audio channel 0 period */
#define AUD0VOL     (0x00a8 >> 1)   /* audio channel 0 volume */
#define AUD0DAT     (0x00aa >> 1)   /* audio channel 0 data */
#define AUD1LCH     (0x00b0 >> 1)   /* audio channel 1 location (high word) */
#define AUD1LCL     (0x00b2 >> 1)   /* audio channel 1 location (low word) */
#define AUD1LEN     (0x00b4 >> 1)   /* audio channel 1 length */
#define AUD1PER     (0x00b6 >> 1)   /* audio channel 1 period */
#define AUD1VOL     (0x00b8 >> 1)   /* audio channel 1 volume */
#define AUD1DAT     (0x00ba >> 1)   /* audio channel 1 data */
#define AUD2LCH     (0x00c0 >> 1)   /* audio channel 2 location (high word) */
#define AUD2LCL     (0x00c2 >> 1)   /* audio channel 2 location (low word) */
#define AUD2LEN     (0x00c4 >> 1)   /* audio channel 2 length */
#define AUD2PER     (0x00c6 >> 1)   /* audio channel 2 period */
#define AUD2VOL     (0x00c8 >> 1)   /* audio channel 2 volume */
#define AUD2DAT     (0x00ca >> 1)   /* audio channel 2 data */
#define AUD3LCH     (0x00d0 >> 1)   /* audio channel 3 location (high word) */
#define AUD3LCL     (0x00d2 >> 1)   /* audio channel 3 location (low word) */
#define AUD3LEN     (0x00d4 >> 1)   /* audio channel 3 length */
#define AUD3PER     (0x00d6 >> 1)   /* audio channel 3 period */
#define AUD3VOL     (0x00d8 >> 1)   /* audio channel 3 volume */
#define AUD3DAT     (0x00da >> 1)   /* audio channel 3 data */
#define BPL1PTH     (0x00e0 >> 1)   /* bitplane 1 pointer (high word) */
#define BPL1PTL     (0x00e2 >> 1)   /* bitplane 1 pointer (low word) */
#define BPL2PTH     (0x00e4 >> 1)   /* bitplane 2 pointer (high word) */
#define BPL2PTL     (0x00e6 >> 1)   /* bitplane 2 pointer (low word) */
#define BPL3PTH     (0x00e8 >> 1)   /* bitplane 3 pointer (high word) */
#define BPL3PTL     (0x00ea >> 1)   /* bitplane 3 pointer (low word) */
#define BPL4PTH     (0x00ec >> 1)   /* bitplane 4 pointer (high word) */
#define BPL4PTL     (0x00ee >> 1)   /* bitplane 4 pointer (low word) */
#define BPL5PTH     (0x00f0 >> 1)   /* bitplane 5 pointer (high word) */
#define BPL5PTL     (0x00f2 >> 1)   /* bitplane 5 pointer (low word) */
#define BPL6PTH     (0x00f4 >> 1)   /* bitplane 6 pointer (high word) */
#define BPL6PTL     (0x00f6 >> 1)   /* bitplane 6 pointer (low word) */
#define BPL7PTH     (0x00f8 >> 1)   /* bitplane 7 pointer (high word) */
#define BPL7PTL     (0x00fa >> 1)   /* bitplane 7 pointer (low word) */
#define BPL8PTH     (0x00fc >> 1)   /* bitplane 8 pointer (high word) */
#define BPL8PTL     (0x00fe >> 1)   /* bitplane 8 pointer (low word) */
#define BPLCON0     (0x0100 >> 1)   /* bitplane control (misc. bits) */
#define BPLCON1     (0x0102 >> 1)   /* bitplane control (scroll PF1, PF2) */
#define BPLCON2     (0x0104 >> 1)   /* bitplane control (priority control) */
#define BPLCON3     (0x0106 >> 1)   /* bitplane control (enhanced features) */
#define BPL1MOD     (0x0108 >> 1)   /* bitplane modulo (odd planes) */
#define BPL2MOD     (0x010a >> 1)   /* bitplane module (even planes) */
#define BPLCON4     (0x010c >> 1)   /* bitplane control 4 (AGA) */
#define CLXCON2     (0x010e >> 1)   /* collision control bits 2 (AGA) */
#define BPL1DAT     (0x0110 >> 1)   /* bitplane 1 data (parallel-to-serial) */
#define BPL2DAT     (0x0112 >> 1)   /* bitplane 2 data (parallel-to-serial) */
#define BPL3DAT     (0x0114 >> 1)   /* bitplane 3 data (parallel-to-serial) */
#define BPL4DAT     (0x0116 >> 1)   /* bitplane 4 data (parallel-to-serial) */
#define BPL5DAT     (0x0118 >> 1)   /* bitplane 5 data (parallel-to-serial) */
#define BPL6DAT     (0x011a >> 1)   /* bitplane 6 data (parallel-to-serial) */
#define BPL7DAT     (0x011c >> 1)   /* bitplane 7 data (parallel-to-serial) */
#define BPL8DAT     (0x011e >> 1)   /* bitplane 8 data (parallel-to-serial) */
#define SPR0PTH     (0x0120 >> 1)   /* sprite 0 pointer (high word) */
#define SPR0PTL     (0x0122 >> 1)   /* sprite 0 pointer (low word) */
#define SPR1PTH     (0x0124 >> 1)   /* sprite 1 pointer (high word) */
#define SPR1PTL     (0x0126 >> 1)   /* sprite 1 pointer (low word) */
#define SPR2PTH     (0x0128 >> 1)   /* sprite 2 pointer (high word) */
#define SPR2PTL     (0x012a >> 1)   /* sprite 2 pointer (low word) */
#define SPR3PTH     (0x012c >> 1)   /* sprite 3 pointer (high word) */
#define SPR3PTL     (0x012e >> 1)   /* sprite 3 pointer (low word) */
#define SPR4PTH     (0x0130 >> 1)   /* sprite 4 pointer (high word) */
#define SPR4PTL     (0x0132 >> 1)   /* sprite 4 pointer (low word) */
#define SPR5PTH     (0x0134 >> 1)   /* sprite 5 pointer (high word) */
#define SPR5PTL     (0x0136 >> 1)   /* sprite 5 pointer (low word) */
#define SPR6PTH     (0x0138 >> 1)   /* sprite 6 pointer (high word) */
#define SPR6PTL     (0x013a >> 1)   /* sprite 6 pointer (low word) */
#define SPR7PTH     (0x013c >> 1)   /* sprite 7 pointer (high word) */
#define SPR7PTL     (0x013e >> 1)   /* sprite 7 pointer (low word) */
#define SPR0POS     (0x0140 >> 1)   /* sprite 0 vert-horiz start position */
#define SPR0CTL     (0x0142 >> 1)   /* sprite 0 vert stop and control data */
#define SPR0DATA    (0x0144 >> 1)   /* sprite 0 image data register A */
#define SPR0DATB    (0x0146 >> 1)   /* sprite 0 image data register B */
#define SPR1POS     (0x0148 >> 1)   /* sprite 1 vert-horiz start position */
#define SPR1CTL     (0x014a >> 1)   /* sprite 1 vert stop and control data */
#define SPR1DATA    (0x014c >> 1)   /* sprite 1 image data register A */
#define SPR1DATB    (0x014e >> 1)   /* sprite 1 image data register B */
#define SPR2POS     (0x0150 >> 1)   /* sprite 2 vert-horiz start position */
#define SPR2CTL     (0x0152 >> 1)   /* sprite 2 vert stop and control data */
#define SPR2DATA    (0x0154 >> 1)   /* sprite 2 image data register A */
#define SPR2DATB    (0x0156 >> 1)   /* sprite 2 image data register B */
#define SPR3POS     (0x0158 >> 1)   /* sprite 3 vert-horiz start position */
#define SPR3CTL     (0x015a >> 1)   /* sprite 3 vert stop and control data */
#define SPR3DATA    (0x015c >> 1)   /* sprite 3 image data register A */
#define SPR3DATB    (0x015e >> 1)   /* sprite 3 image data register B */
#define SPR4POS     (0x0160 >> 1)   /* sprite 4 vert-horiz start position */
#define SPR4CTL     (0x0162 >> 1)   /* sprite 4 vert stop and control data */
#define SPR4DATA    (0x0164 >> 1)   /* sprite 4 image data register A */
#define SPR4DATB    (0x0166 >> 1)   /* sprite 4 image data register B */
#define SPR5POS     (0x0168 >> 1)   /* sprite 5 vert-horiz start position */
#define SPR5CTL     (0x016a >> 1)   /* sprite 5 vert stop and control data */
#define SPR5DATA    (0x016c >> 1)   /* sprite 5 image data register A */
#define SPR5DATB    (0x016e >> 1)   /* sprite 5 image data register B */
#define SPR6POS     (0x0170 >> 1)   /* sprite 6 vert-horiz start position */
#define SPR6CTL     (0x0172 >> 1)   /* sprite 6 vert stop and control data */
#define SPR6DATA    (0x0174 >> 1)   /* sprite 6 image data register A */
#define SPR6DATB    (0x0176 >> 1)   /* sprite 6 image data register B */
#define SPR7POS     (0x0178 >> 1)   /* sprite 7 vert-horiz start position */
#define SPR7CTL     (0x017a >> 1)   /* sprite 7 vert stop and control data */
#define SPR7DATA    (0x017c >> 1)   /* sprite 7 image data register A */
#define SPR7DATB    (0x017e >> 1)   /* sprite 7 image data register B */
#define COLOR00     (0x0180 >> 1)   /* color table 00 (high / low 12 bits) */
#define COLOR01     (0x0182 >> 1)   /* color table 01 (high / low 12 bits) */
#define COLOR02     (0x0184 >> 1)   /* color table 02 (high / low 12 bits) */
#define COLOR03     (0x0186 >> 1)   /* color table 03 (high / low 12 bits) */
#define COLOR04     (0x0188 >> 1)   /* color table 04 (high / low 12 bits) */
#define COLOR05     (0x018a >> 1)   /* color table 05 (high / low 12 bits) */
#define COLOR06     (0x018c >> 1)   /* color table 06 (high / low 12 bits) */
#define COLOR07     (0x018e >> 1)   /* color table 07 (high / low 12 bits) */
#define COLOR08     (0x0190 >> 1)   /* color table 08 (high / low 12 bits) */
#define COLOR09     (0x0192 >> 1)   /* color table 09 (high / low 12 bits) */
#define COLOR10     (0x0194 >> 1)   /* color table 10 (high / low 12 bits) */
#define COLOR11     (0x0196 >> 1)   /* color table 11 (high / low 12 bits) */
#define COLOR12     (0x0198 >> 1)   /* color table 12 (high / low 12 bits) */
#define COLOR13     (0x019a >> 1)   /* color table 13 (high / low 12 bits) */
#define COLOR14     (0x019c >> 1)   /* color table 14 (high / low 12 bits) */
#define COLOR15     (0x019e >> 1)   /* color table 15 (high / low 12 bits) */
#define COLOR16     (0x01a0 >> 1)   /* color table 16 (high / low 12 bits) */
#define COLOR17     (0x01a2 >> 1)   /* color table 17 (high / low 12 bits) */
#define COLOR18     (0x01a4 >> 1)   /* color table 18 (high / low 12 bits) */
#define COLOR19     (0x01a6 >> 1)   /* color table 19 (high / low 12 bits) */
#define COLOR20     (0x01a8 >> 1)   /* color table 20 (high / low 12 bits) */
#define COLOR21     (0x01aa >> 1)   /* color table 21 (high / low 12 bits) */
#define COLOR22     (0x01ac >> 1)   /* color table 22 (high / low 12 bits) */
#define COLOR23     (0x01ae >> 1)   /* color table 23 (high / low 12 bits) */
#define COLOR24     (0x01b0 >> 1)   /* color table 24 (high / low 12 bits) */
#define COLOR25     (0x01b2 >> 1)   /* color table 25 (high / low 12 bits) */
#define COLOR26     (0x01b4 >> 1)   /* color table 26 (high / low 12 bits) */
#define COLOR27     (0x01b6 >> 1)   /* color table 27 (high / low 12 bits) */
#define COLOR28     (0x01b8 >> 1)   /* color table 28 (high / low 12 bits) */
#define COLOR29     (0x01ba >> 1)   /* color table 29 (high / low 12 bits) */
#define COLOR30     (0x01bc >> 1)   /* color table 30 (high / low 12 bits) */
#define COLOR31     (0x01be >> 1)   /* color table 31 (high / low 12 bits) */
#define HTOTAL      (0x01c0 >> 1)   /* highest horiz. count (VARBEAMEN=1) */
#define HSSTOP      (0x01c2 >> 1)   /* horiz. position for hsync stop */
#define HBSTRT      (0x01c4 >> 1)   /* horiz. position for hblank start */
#define HBSTOP      (0x01c6 >> 1)   /* horiz. position for hblank stop */
#define VTOTAL      (0x01c8 >> 1)   /* highest vertical line (VARBEAMEN=1) */
#define VSSTOP      (0x01ca >> 1)   /* vert. position for vsync stop */
#define VBSTRT      (0x01cc >> 1)   /* vert. position for vblank start */
#define VBSTOP      (0x01ce >> 1)   /* vert. position for vblank stop */
#define SPRHSTRT    (0x01d0 >> 1)   /* UHRES sprite vertical start */
#define SPRHSTOP    (0x01d2 >> 1)   /* UHRES sprite vertical stop */
#define BPLHSTRT    (0x01d4 >> 1)   /* UHRES bitplane vertical start */
#define BPLHSTOP    (0x01d6 >> 1)   /* UHRES bitplane vertical stop */
#define HHPOSW      (0x01d8 >> 1)   /* dual mode hires h-beam counter write */
#define BEAMCON0    (0x01dc >> 1)   /* beam counter control (SHRES, PAL) */
#define HSSTRT      (0x01de >> 1)   /* horiz. sync start (VARHSY) */
#define VSSTRT      (0x01e0 >> 1)   /* vert. sync start (VARVSY) */
#define HCENTER     (0x01e2 >> 1)   /* horiz. pos. for vsync on interlace */
#define DIWHIGH     (0x01e4 >> 1)   /* disp. window (high bits start, stop) */
#define BPLHMOD     (0x01e6 >> 1)   /* UHRES bitplane modulo */
#define SPRHPTH     (0x01e8 >> 1)   /* UHRES sprite pointer (high word) */
#define SPRHPTL     (0x01ea >> 1)   /* UHRES sprite pointer (low word) */
#define BPLHPTH     (0x01ec >> 1)   /* UHRES bitplane pointer (high word) */
#define BPLHPTL     (0x01ee >> 1)   /* UHRES bitplane pointer (low word) */
#define FMODE       (0x01fc >> 1)   /* DMA fetch mode (AGA) */
#define NOP         (0x01fe >> 1)   /* no-op */

#define AUD_CH_BASE (0x00a0 >> 1)   /* base for audio channel 0 registers */
#define AUD_CH_SHIFT    (4 - 1)     /* bits to shift for channel number */
#define BPLP_OFFSET (0x0004 >> 1)   /* register offset for bitplanes 2 to 8 */
#define SPRP_OFFSET (0x0004 >> 1)   /* register offset for sprites 1 to 7 */
#define SPRD_OFFSET (0x0008 >> 1)   /* register offset for sprites 1 to 7 */
#define SPRD_MASK   (0x0038 >> 1)   /* sprite data sprite number mask */

#define SET_CLEAR_BIT           BIT(15)     /* used by multiple registers */

#define ADKCON_PRECOMP_MASK     0x6000      /* precomp (none to 560 ns) */
#define ADKCON_PRECOMP_SHIFT    13          /* precomp (none to 560 ns) */
#define ADKCON_MFMPREC          BIT(12)     /* precomp: 1 = MFM, 0 = GCR */
#define ADKCON_UARTBRK          BIT(11)     /* send UART break (clears TXD) */
#define ADKCON_WORDSYNC         BIT(10)     /* enable floppy sync on word */
#define ADKCON_MSBSYNC          BIT(9)      /* enable sync on MSB (for GCR) */
#define ADKCON_FAST             BIT(8)      /* disk clock: 1 = 2us, 0 = 4us */
#define ADKCON_USE3PN           BIT(7)      /* mutes audio channel 3 */
#define ADKCON_USE2P3           BIT(6)      /* audio ch. 2 mod. period of 3 */
#define ADKCON_USE1P2           BIT(5)      /* audio ch. 1 mod. period of 2 */
#define ADKCON_USE0P1           BIT(4)      /* audio ch. 0 mod. period of 1 */
#define ADKCON_USE3VN           BIT(3)      /* mutes audio channel 3 */
#define ADKCON_USE2V3           BIT(2)      /* audio ch. 2 mod. volume of 3 */
#define ADKCON_USE1V2           BIT(1)      /* audio ch. 1 mod. volume of 2 */
#define ADKCON_USE0V1           BIT(0)      /* audio ch. 0 mod. volume of 1 */
#define ADKCON_WRITE_MASK       0x7fff      /* all bits except set/clear */

#define AUDIO_MAX_VOLUME        BIT(6)      /* force volume to max (64) */
#define AUDIO_VOLUME_MASK       (0x003f)    /* mask for volumes 0 - 63 */

#define BPLCON0_HIRES_MODE      BIT(15)     /* hi-res modes (640x256/512) */
#define BPLCON0_BP_MASK         0x7000      /* mask for BPU0-BPU2 */
#define BPLCON0_BP_SHIFT        12          /* shift for bitplanes used */
#define BPLCON0_ENABLE_HAM_MODE BIT(11)     /* enable HAM6 or HAM8 mode */
#define BPLCON0_DUAL_PLAYFIELD  BIT(10)     /* enable dual playfields */
#define BPLCON0_GENLOCK_COLOR   BIT(9)      /* composite video (genlock) */
#define BPLCON0_GENLOCK_AUDIO   BIT(8)      /* audio passthrough (genlock) */
#define BPLCON0_ULTRA_HIRES     BIT(7)      /* UHRES mode (unsupported) */
#define BPLCON0_SUPER_HIRES     BIT(6)      /* super hi-res mode (35ns) */
#define BPLCON0_BYPASS_PALETTE  BIT(5)      /* bypass palette (grayscale) */
#define BPLCON0_BP3_BIT         BIT(4)      /* BPU3 for 8 bitplane modes */
#define BPLCON0_BP3_SHIFT       (4 - 3)     /* BPU3 value bit shift */
#define BPLCON0_INTERLACE_MODE  BIT(2)      /* enable interlace mode */
#define BPLCON0_EXTERNAL_SYNC   BIT(1)      /* use external sync (genlock) */
#define BPLCON0_ENABLE_BPLCON3  BIT(0)      /* enable BPLCON3 (ECS/AGA) */

#define BPLCON1_PF2_HS_HI_MASK  0xc000      /* PF2 hscroll value high bits */
#define BPLCON1_PF2_HS_HI_SHIFT (14 - 6)    /* PF2 hscroll high bit shift */
#define BPLCON1_PF2_HS_LO_MASK  0x3000      /* PF2 hscroll value low bits */
#define BPLCON1_PF2_HS_LO_SHIFT (12 - 0)    /* PF2 hscroll low bit shift */
#define BPLCON1_PF1_HS_HI_MASK  0x0c00      /* PF1 hscroll value high bits */
#define BPLCON1_PF1_HS_HI_SHIFT (10 - 6)    /* PF1 hscroll high bit shift */
#define BPLCON1_PF1_HS_LO_MASK  0x0300      /* PF1 hscroll value low bits */
#define BPLCON1_PF1_HS_LO_SHIFT (8 - 0)     /* PF1 hscroll low bit shift */
#define BPLCON1_PF2_HS_MASK     0x00f0      /* PF2 hscroll value bits */
#define BPLCON1_PF2_HS_SHIFT    (4 - 2)     /* PF2 hscroll bit shift */
#define BPLCON1_PF1_HS_MASK     0x000f      /* PF1 hscroll value bits */
#define BPLCON1_PF1_HS_LSHIFT   (2 - 0)     /* PF1 hscroll bit shift left */

#define BPLCON2_KILL_EHB        BIT(9)      /* kill EHB (AGA 6 bitplane) */
#define BPLCON2_READ_RAM        BIT(8)      /* color register read mode */
#define BPLCON2_PF2_PRIORITY    BIT(6)      /* playfield 2 has priority */
#define BPLCON2_PF2_SPR_MASK    0x38        /* priority vs. sprites (0-4) */
#define BPLCON2_PF2_SPR_SHIFT   3           /* playfield 2 vs. sprite bits */
#define BPLCON2_PF1_SPR_MASK    0x07        /* priority vs. sprites (0-4) */

#define BPLCON3_AGA_BANK_MASK   0xe000      /* color register bank (0-7) */
#define BPLCON3_AGA_BANK_SHIFT  (13 - 5)    /* shift to convert to offset */
#define BPLCON3_PF2_OFF_MASK    0x1c00      /* playfield 2 offset (0-7) */
#define BPLCON3_PF2_OFF_SHIFT   10          /* shift to convert to (0-7) */
#define BPLCON3_AGA_LOW_NIBBLE  BIT(9)      /* set/get palette low nibbles */
#define BPLCON3_SPR_RES_MASK    0x0600      /* mask for sprite resolution */
#define BPLCON3_SPR_RES_SHIFT   6           /* shift sprite resolution */

#define AMIGA_RED_MASK          0x0f00      /* big-endian 12-bit RGB */
#define AMIGA_GREEN_MASK        0x00f0
#define AMIGA_BLUE_MASK         0x000f

#define RGB_HIGH_NIBBLE_MASK    0x00f0f0f0  /* keep high nibbles of color */

#define RED_HIGH_NIBBLE_MASK    0x00f00000  /* high nibbles of 24-bit color */
#define RED_LOW_NIBBLE_MASK     0x000f0000  /* low nibbles of 24-bit color */
#define GREEN_HIGH_NIBBLE_MASK  0x0000f000
#define GREEN_LOW_NIBBLE_MASK   0x00000f00
#define BLUE_HIGH_NIBBLE_MASK   0x000000f0
#define BLUE_LOW_NIBBLE_MASK    0x0000000f

/* shifts for high/low nibbles of 24-bit colors as 12-bit values */
#define RED_HIGH_NIBBLE_SHIFT       12
#define GREEN_HIGH_NIBBLE_SHIFT     8
#define BLUE_HIGH_NIBBLE_SHIFT      4

#define RED_LOW_NIBBLE_SHIFT        8
#define GREEN_LOW_NIBBLE_SHIFT      4
#define BLUE_LOW_NIBBLE_SHIFT       0

#define COPCON_ENABLE_DANGER    BIT(1)      /* copper can write below 0x40 */

#define DMACON_BLITTER_BUSY_SHIFT   14      /* blitter busy (read-only) */
#define DMACON_BLITTER_ZERO_SHIFT   13      /* blitter zero status (ro) */
#define DMACON_BLITTER_NASTY    BIT(10)     /* blitter priority over CPU */
#define DMACON_DMA_ENABLE       BIT(9)      /* main DMA enable bit */
#define DMACON_DMA_BITPLANE     BIT(8)      /* bitplane DMA enable */
#define DMACON_DMA_COPPER       BIT(7)      /* copper DMA enable */
#define DMACON_DMA_BLITTER      BIT(6)      /* blitter DMA enable */
#define DMACON_DMA_SPRITES      BIT(5)      /* sprite DMA enable */
#define DMACON_DMA_DISK         BIT(4)      /* disk DMA enable */
#define DMACON_DMA_AUDIO_MASK   0x0003      /* audio DMA channels 3-0 */
#define DMACON_WRITE_MASK       0x03ff      /* mask for writable bits */

#define VPOS_LONG_FRAME_SHIFT   15          /* long frame */
#define VPOS_LONG_LINE_SHIFT    7           /* long line (NTSC modes) */
#define VPOS_VERT_RIGHT_SHIFT   8       /* top 3 bits in VPOS/VPOSW */
#define VPOS_VERT_LEFT_SHIFT    8       /* low byte in high byte of VHPOS */

#define VPOS_LONG_FRAME         BIT(VPOS_LONG_FRAME_SHIFT)
#define VPOS_LONG_LINE          BIT(VPOS_LONG_LINE_SHIFT)

#define FMODE_SPR_SCAN_DOUBLE   BIT(15)     /* AGA sprite scan-doubling */
#define FMODE_BPL_SCAN_DOUBLE   BIT(14)     /* AGA bitplane scan-doubling */
#define FMODE_SPR_DBL_CAS       BIT(3)      /* sprite double CAS mode */
#define FMODE_SPR_32_BITS       BIT(2)      /* sprite 32-bit fetch mode */
#define FMODE_BPL_DBL_CAS       BIT(1)      /* bitplane double CAS mode */
#define FMODE_BPL_32_BITS       BIT(0)      /* bitplane 32-bit fetch mode */

/* Amiga interrupt levels (mapped to m68k INT1-INT6). */
#define INTEN_MAIN_ENABLE       BIT(14)
#define INT_CIAB_AND_LEVEL_6    BIT(13)
#define INT_DISK_SYNC           BIT(12)
#define INT_SERIAL_RECEIVE_FULL BIT(11)
#define INT_AUDIO_CHAN_3_DONE   BIT(10)
#define INT_AUDIO_CHAN_2_DONE   BIT(9)
#define INT_AUDIO_CHAN_1_DONE   BIT(8)
#define INT_AUDIO_CHAN_0_DONE   BIT(7)
#define INT_BLITTER_DONE        BIT(6)
#define INT_START_OF_VBLANK     BIT(5)
#define INT_COPPER              BIT(4)
#define INT_CIAA_AND_LEVEL_2    BIT(3)
#define INT_SOFTWARE            BIT(2)
#define INT_DISK_BLOCK_DONE     BIT(1)
#define INT_SERIAL_SEND_EMPTY   BIT(0)

#define INTEN_WRITE_MASK        (0x7fff)    /* mask for writable bits */
#define INTREQ_WRITE_MASK       (0x3fff)

#endif /* AMIGA_CHIPSET_REGS_H */
