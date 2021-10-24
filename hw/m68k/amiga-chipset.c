/*
 * Amiga chipset implementation (AGA custom chips + CIAs).
 *
 * Copyright (c) 2021 Jake Hamby
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 */

#include "qemu/osdep.h"
#include "qemu/log.h"
#include "qemu/timer.h"
#include "exec/memory.h"
#include "hw/sysbus.h"
#include "hw/qdev-properties.h"
#include "ui/input.h"
#include "ui/console.h"
#include "target/m68k/cpu.h"
#include "migration/vmstate.h"
#include "hw/misc/amiga.h"
#include "hw/m68k/amiga-chipset.h"
#include "hw/m68k/amiga-keys.h"

/* Register names for debugging (all names are 8 bytes, including '\0'). */
#ifdef DEBUG_AMIGA
static const char *chipset_register_names =
    "BLTDDAT\0DMACONR\0VPOSR  \0VHPOSR \0DSKDATR\0JOY0DAT\0JOY1DAT\0CLXDAT \0"
    "ADKCONR\0POT0DAT\0POT1DAT\0POTGOR \0SERDATR\0DSKBYTR\0INTENAR\0INTREQR\0"
    "DSKPTH \0DSKPTL \0DSKLEN \0DSKDAT \0REFPTR \0VPOSW  \0VHPOSW \0COPCON \0"
    "SERDAT \0SERPER \0POTGO  \0JOYTEST\0STREQU \0STRVBL \0STRHOR \0STRLONG\0"
    "BLTCON0\0BLTCON1\0BLTAFWM\0BLTALWM\0BLTCPTH\0BLTCPTL\0BLTBPTH\0BLTBPTL\0"
    "BLTAPTH\0BLTAPTL\0BLTDPTH\0BLTDPTL\0BLTSIZE\0BLCON0L\0BLTSIZV\0BLTSIZH\0"
    "BLTCMOD\0BLTBMOD\0BLTAMOD\0BLTDMOD\0REG0068\0REG006A\0REG006C\0REG006E\0"
    "BLTCDAT\0BLTBDAT\0BLTADAT\0REG0076\0SPRHDAT\0BPLHDAT\0LISAID \0DSKSYNC\0"
    "COP1LCH\0COP1LCL\0COP2LCH\0COP2LCL\0COPJMP1\0COPJMP2\0COPINS \0DIWSTRT\0"
    "DIWSTOP\0DDFSTRT\0DDFSTOP\0DMACON \0CLXCON \0INTENA \0INTREQ \0ADKCON \0"
    "AUD0LCH\0AUD0LCL\0AUD0LEN\0AUD0PER\0AUD0VOL\0AUD0DAT\0REG00AC\0REG00AE\0"
    "AUD1LCH\0AUD1LCL\0AUD1LEN\0AUD1PER\0AUD1VOL\0AUD1DAT\0REG01AC\0REG01AE\0"
    "AUD2LCH\0AUD2LCL\0AUD2LEN\0AUD2PER\0AUD2VOL\0AUD2DAT\0REG02AC\0REG02AE\0"
    "AUD3LCH\0AUD3LCL\0AUD3LEN\0AUD3PER\0AUD3VOL\0AUD3DAT\0REG03AC\0REG03AE\0"
    "BPL1PTH\0BPL1PTL\0BPL2PTH\0BPL2PTL\0BPL3PTH\0BPL3PTL\0BPL4PTH\0BPL4PTL\0"
    "BPL5PTH\0BPL5PTL\0BPL6PTH\0BPL6PTL\0BPL7PTH\0BPL7PTL\0BPL8PTH\0BPL8PTL\0"
    "BPLCON0\0BPLCON1\0BPLCON2\0BPLCON3\0BPL1MOD\0BPL2MOD\0BPLCON4\0CLXCON2\0"
    "BPL1DAT\0BPL2DAT\0BPL3DAT\0BPL4DAT\0BPL5DAT\0BPL6DAT\0BPL7DAT\0BPL8DAT\0"
    "SPR0PTH\0SPR0PTL\0SPR1PTH\0SPR1PTL\0SPR2PTH\0SPR2PTL\0SPR3PTH\0SPR3PTL\0"
    "SPR4PTH\0SPR4PTL\0SPR5PTH\0SPR5PTL\0SPR6PTH\0SPR6PTL\0SPR7PTH\0SPR7PTL\0"
    "SPR0POS\0SPR0CTL\0SPR0DTA\0SPR0DTB\0SPR1POS\0SPR1CTL\0SPR1DTA\0SPR1DTB\0"
    "SPR2POS\0SPR2CTL\0SPR2DTA\0SPR2DTB\0SPR3POS\0SPR3CTL\0SPR3DTA\0SPR3DTB\0"
    "SPR4POS\0SPR4CTL\0SPR4DTA\0SPR4DTB\0SPR5POS\0SPR5CTL\0SPR5DTA\0SPR5DTB\0"
    "SPR6POS\0SPR6CTL\0SPR6DTA\0SPR6DTB\0SPR7POS\0SPR7CTL\0SPR7DTA\0SPR7DTB\0"
    "COLOR00\0COLOR01\0COLOR02\0COLOR03\0COLOR04\0COLOR05\0COLOR06\0COLOR07\0"
    "COLOR08\0COLOR09\0COLOR10\0COLOR11\0COLOR12\0COLOR13\0COLOR14\0COLOR15\0"
    "COLOR16\0COLOR17\0COLOR18\0COLOR19\0COLOR20\0COLOR21\0COLOR22\0COLOR23\0"
    "COLOR24\0COLOR25\0COLOR26\0COLOR27\0COLOR28\0COLOR29\0COLOR30\0COLOR31\0"
    "HTOTAL \0HSSTOP \0HBSTRT \0HBSTOP \0VTOTAL \0VSSTOP \0VBSTRT \0VBSTOP \0"
    "SPRHSTR\0SPRHSTP\0BPLHSTR\0BPLHSTP\0HHPOSW \0HHPOSR \0BEMCON0\0HSSTRT \0"
    "VSSTRT \0HCENTER\0DIWHIGH\0BPLHMOD\0SPRHPTH\0SPRHPTL\0BPLHPTH\0BPLHPTL\0"
    "REG01F0\0REG01F2\0REG01F4\0REG01F6\0REG01F8\0REG01FA\0FMODE  \0NOP    ";
#endif

/* The clock speed is fixed and independent of NTSC/PAL screen modes. */
#define PAL_CLK_PER_SEC     (3546895.0)     /* (28.375160 MHz / 8) */
#define NTSC_CLK_PER_SEC    (3579545.375)   /* (28.636363 MHz / 8) */

/* Every day, update the clock offsets, for more accurate conversions. */
#define CYCLES_PER_DAY(x)   (int64_t)(x * 3600 * 24)
#define NS_PER_DAY          (NANOSECONDS_PER_SECOND * 3600 * 24)

/* Convert relative ticks (within a day) to ns for timer callbacks. */
static inline uint64_t ticks_to_ns(uint64_t value, bool ntsc)
{
    double d_val = (double)value;
    return ntsc ? d_val * (NANOSECONDS_PER_SECOND / NTSC_CLK_PER_SEC)
                : d_val * (NANOSECONDS_PER_SECOND / PAL_CLK_PER_SEC);
}

/* Convert relative ns (within a day) to DMA cycles. */
static inline uint64_t ns_to_ticks(uint64_t value, bool ntsc)
{
    double d_val = (double)value;
    return ntsc ? d_val * (NTSC_CLK_PER_SEC / NANOSECONDS_PER_SECOND)
                : d_val * (PAL_CLK_PER_SEC / NANOSECONDS_PER_SECOND);
}

/* Return the contents of a chipset register, by word offset. */
static inline uint16_t chipset_readw(AmigaChipset *cs, uint8_t word_offset)
{
    uint16_t result = 0;

    switch (word_offset) {
    case DMACONR:
        result = cs->state.dma.control_bits |
                (cs->state.blitter.busy << DMACON_BLITTER_BUSY_SHIFT) |
                (cs->state.blitter.zero_status << DMACON_BLITTER_ZERO_SHIFT);
        break;  /* debug printf */

    case VPOSR:
        result = (cs->state.display.alice_y_counter >> VPOS_VERT_RIGHT_SHIFT) |
               (cs->state.display.long_line << VPOS_LONG_LINE_SHIFT) |
               (cs->ntsc ? NTSC_ALICE_VERSION_BITS : PAL_ALICE_VERSION_BITS) |
               (cs->state.display.long_frame << VPOS_LONG_FRAME_SHIFT);
        break;  /* debug printf */

    case VHPOSR:
        result = (cs->state.display.alice_y_counter << VPOS_VERT_LEFT_SHIFT) |
                cs->state.display.alice_x_counter;
        break;  /* debug printf */

    case JOY0DAT:
        /* FIXME: return mouse 0 position */
        break;  /* debug printf */

    case JOY1DAT:
        /* no mouse in port 2 until QEMU supports multiple mice */
        break;  /* debug printf */

    /* collision data (read and clear) */
    case CLXDAT:
        // TODO: clear before returning
        result = cs->state.collision.bits;
        break;  /* debug printf */

    case ADKCONR:
        // FIXME: return cs->state.audio_disk_uart_bits;
        break;  /* debug printf */

    case POT0DAT:
        break;  /* debug printf */

    case POT1DAT:
        break;  /* debug printf */

    case POTGOR:
        break;  /* debug printf */

    case SERDATR:
        break;  /* debug printf */

    case DSKBYTR:
        break;  /* debug printf */

    case INTENAR:
        result = cs->state.irq.interrupt_enable_bits;
        break;  /* debug printf */

    case INTREQR:
        result = cs->state.irq.interrupt_request_bits;
        break;  /* debug printf */

    case DENISEID:
        result = LISA_VERSION;
        break;  /* debug printf */

    case COLOR00 ... COLOR31: {
        int index = (word_offset - COLOR00) +
                    cs->state.palette.register_bank_offset;

        if (unlikely(!(cs->state.palette.read_color_table))) {
            DPRINTF("warning: read from color register %d in write mode\n",
                    index);
        }

        uint32_t rgb_color = cs->state.palette.color[index];

        if (cs->state.palette.update_low_nibble) {
            /* return the low nibbles of this color palette entry */
            result = ((rgb_color >> RED_LOW_NIBBLE_SHIFT) & AMIGA_RED_MASK) |
                   ((rgb_color >> GREEN_LOW_NIBBLE_SHIFT) & AMIGA_GREEN_MASK) |
                   ((rgb_color >> BLUE_LOW_NIBBLE_SHIFT) & AMIGA_BLUE_MASK);
        } else {
            /* return the high nibbles of this color palette entry */
            result = ((rgb_color >> RED_HIGH_NIBBLE_SHIFT) & AMIGA_RED_MASK) |
                ((rgb_color >> GREEN_HIGH_NIBBLE_SHIFT) & AMIGA_GREEN_MASK) |
                ((rgb_color >> BLUE_HIGH_NIBBLE_SHIFT) & AMIGA_BLUE_MASK);
        }
        break;  /* debug printf */
    }

    default:
        /* TODO: return random noise here instead of 0 if useful for demos */
        break;
    }

    /* unhandled registers and registers we want to trace reads from */
    DPRINTF("chipset read returning 0x%"PRIx16" from register %s\n",
            result, (chipset_register_names + (word_offset * 8)));

    return result;
}

/* Write a 16-bit value to a chipset register, by word offset. */
static inline void chipset_writew(AmigaChipset *cs, uint8_t word_offset,
                                  uint16_t value)
{
    switch (word_offset) {
    /* disk DMA registers */
    case DSKPTH:
        REPLACE_HIGH(cs->state.fdc.dma_address, value);
        break;  /* debug printf */

    case DSKPTL:
        REPLACE_LOW(cs->state.fdc.dma_address, value);
        break;  /* debug printf */

    case DSKLEN:
        cs->state.fdc.dma_length = value;
        break;  /* debug printf */

    case DSKDAT:
        cs->state.fdc.dma_data_buffer = value;
        break;  /* debug printf */

    case VPOSW:
        // FIXME: add defines for the hex values
        cs->state.display.alice_y_counter =
                (cs->state.display.alice_y_counter & 0xff) |
                ((value & 0x07) << 8);
        cs->state.display.long_frame = (value & VPOS_LONG_FRAME);
        cs->state.display.long_line = (value & VPOS_LONG_LINE);
        DPRINTF("VPOSW set to %"PRIu16" LOF=%d LOL=%d\n",
                cs->state.display.alice_y_counter,
                cs->state.display.long_frame,
                cs->state.display.long_line);
        return;

    case VHPOSW:
        // FIXME: add defines for the hex values
        cs->state.display.alice_y_counter = (value >> 8) |
                (cs->state.display.alice_y_counter & 0x0700);
        cs->state.display.alice_x_counter = (value & 0x00ff);
        DPRINTF("VHPOSW set to (%"PRIu16", %"PRIu16")\n",
                cs->state.display.alice_x_counter,
                cs->state.display.alice_y_counter);
        return;

    /* copper control */
    case COPCON:
        cs->state.copper.write_all = (value & COPCON_ENABLE_DANGER);
        break;  /* debug printf */

    case SERDAT:
        break;  /* debug printf */

    case SERPER:
        break;  /* debug printf */

    case POTGO:
        break;  /* debug printf */

    /* hblank and vblank strobe registers */
    case STREQU:
        break;  /* debug printf */

    case STRVBL:
        break;  /* debug printf */

    case STRHOR:
        break;  /* debug printf */

    case STRLONG:
        break;  /* debug printf */

    /* blitter registers */
    case BLTCON0:
        break;  /* debug printf */

    case BLTCON1:
        break;  /* debug printf */

    case BLTAFWM:
        break;  /* debug printf */

    case BLTALWM:
        break;  /* debug printf */

    case BLTCPTH:
        break;  /* debug printf */

    case BLTCPTL:
        break;  /* debug printf */

    case BLTBPTH:
        break;  /* debug printf */

    case BLTBPTL:
        break;  /* debug printf */

    case BLTAPTH:
        break;  /* debug printf */

    case BLTAPTL:
        break;  /* debug printf */

    case BLTDPTH:
        break;  /* debug printf */

    case BLTDPTL:
        break;  /* debug printf */

    case BLTSIZE:
        break;  /* debug printf */

    case BLTCON0L:
        break;  /* debug printf */

    case BLTSIZV:
        break;  /* debug printf */

    case BLTSIZH:
        break;  /* debug printf */

    case BLTCMOD:
        break;  /* debug printf */
    case BLTBMOD:
        break;  /* debug printf */
    case BLTAMOD:
        break;  /* debug printf */
    case BLTDMOD:
        break;  /* debug printf */

    case BLTCDAT:
        break;  /* debug printf */
    case BLTBDAT:
        break;  /* debug printf */
    case BLTADAT:
        break;  /* debug printf */

    case DSKSYNC:
        break;  /* debug printf */

    /* copper registers */
    case COP1LCH:
        REPLACE_HIGH(cs->state.copper.location[0], value);
        break;  /* debug printf */

    case COP1LCL:
        REPLACE_LOW(cs->state.copper.location[0], value);
        break;  /* debug printf */

    case COP2LCH:
        REPLACE_HIGH(cs->state.copper.location[1], value);
        break;  /* debug printf */

    case COP2LCL:
        REPLACE_LOW(cs->state.copper.location[1], value);
        break;  /* debug printf */

    case COPJMP1:
        /* Do we want to emulate the blitter DMA bug when the CPU strobes? */
        cs->state.copper.current_address = cs->state.copper.location[0];
        cs->state.copper.state = COPPER_LOADING_WORD_1;
        break;  /* debug printf */

    case COPJMP2:
        /* Do we want to emulate the blitter DMA bug when the CPU strobes? */
        cs->state.copper.current_address = cs->state.copper.location[1];
        cs->state.copper.state = COPPER_LOADING_WORD_1;
        break;  /* debug printf */

    /* display start/stop registers */
    case DIWSTRT:
        break;  /* debug printf */

    case DIWSTOP:
        break;  /* debug printf */

    case DDFSTRT:
        break;  /* debug printf */

    case DDFSTOP:
        break;  /* debug printf */

    /* DMA and interrupt control */
    case DMACON:
        if (value & SET_CLEAR_BIT) {
            cs->state.dma.control_bits |= (value & DMACON_WRITE_MASK);
        } else {
            cs->state.dma.control_bits &= ~(value);
        }
        break;  /* debug printf */

    /* collision control */
    case CLXCON:
        /* FIXME: implement */
        break;  /* debug printf */

    case INTENA:
        if (value & SET_CLEAR_BIT) {
            cs->state.irq.interrupt_enable_bits |= (value & INTEN_WRITE_MASK);
        } else {
            cs->state.irq.interrupt_enable_bits &= ~(value);
        }
        break;  /* debug printf */

    case INTREQ:
        if (value & SET_CLEAR_BIT) {
            cs->state.irq.interrupt_request_bits |= (value & INTREQ_WRITE_MASK);
        } else {
            cs->state.irq.interrupt_request_bits &= ~(value);
        }
        break;  /* debug printf */

    case ADKCON:
    /* FIXME:
        if (value & SET_CLEAR_BIT) {
            cs->paula.audio_disk_uart_bits |= (value & ADKCON_WRITE_MASK);
        } else {
            cs->paula.audio_disk_uart_bits &= ~(value);
        } */
        break;  /* debug printf */

    /* audio registers */
    case AUD0LCH ... AUD3DAT: {
        int channel = (word_offset - AUD_CH_BASE) >> AUD_CH_SHIFT;
        switch (word_offset & 0x07) {
            case AUD0LCH - AUD_CH_BASE:
                REPLACE_HIGH(cs->state.audio[channel].data_address,
                             value);
                break;  /* debug printf */

            case AUD0LCL - AUD_CH_BASE:
                REPLACE_LOW(cs->state.audio[channel].data_address,
                            value);
                break;  /* debug printf */

            case AUD0LEN - AUD_CH_BASE:
                cs->state.audio[channel].data_length_words = value;
                break;  /* debug printf */

            case AUD0PER - AUD_CH_BASE:
                cs->state.audio[channel].period = value;
                break;  /* debug printf */

            case AUD0VOL - AUD_CH_BASE:
                cs->state.audio[channel].volume =
                    (value & AUDIO_MAX_VOLUME) ? AUDIO_MAX_VOLUME :
                    (value & AUDIO_VOLUME_MASK);
                /* TODO: update prescaled 16-bit channel value */
                break;  /* debug printf */

            case AUD0DAT - AUD_CH_BASE:
                cs->state.audio[channel].dac_data = value;
                /* TODO: update prescaled 16-bit channel value */
                break;  /* debug printf */

            default:
                break;      /* fall through to error log */
        }
        break;              /* fall through again */
    }

    case BPL1PTH ... BPL8PTL:
        /* FIXME: implement */
        break;  /* debug printf */

    case BPLCON0:
        /* FIXME: add write mask for unpreserved bits */
        cs->state.display.bplcon0_bits = value;
        cs->state.display.bitplanes_used = ((value & BPLCON0_BP_MASK) >> BPLCON0_BP_SHIFT) |
                             ((value & BPLCON0_BP3_BIT) >> BPLCON0_BP3_SHIFT);

        if (unlikely(cs->state.display.bitplanes_used > 8)) {
            /* let's assume BPL3 was set on accident (non-AGA-aware code?) */
            /* NOTE: would it be better to assume 8 bitplanes instead? */
            int compat_bitplanes_used = cs->state.display.bitplanes_used & 0x07;
            DPRINTF("bad bitplanes used value %d, setting to %d\n",
                     cs->state.display.bitplanes_used, compat_bitplanes_used);
            cs->state.display.bitplanes_used = compat_bitplanes_used;
        }

        DPRINTF("BPLCON0: bitplanes=%d hires=%d ham=%d dual_pf=%d shres=%d "
                "bypass=%d interlace=%d genlock=%d bplcon3_enable=%d\n",
                cs->state.display.bitplanes_used,
                !!(cs->state.display.bplcon0_bits & BPLCON0_HIRES_MODE),
                !!(cs->state.display.bplcon0_bits & BPLCON0_ENABLE_HAM_MODE),
                !!(cs->state.display.bplcon0_bits & BPLCON0_DUAL_PLAYFIELD),
                !!(cs->state.display.bplcon0_bits & BPLCON0_SUPER_HIRES),
                !!(cs->state.display.bplcon0_bits & BPLCON0_BYPASS_PALETTE),
                !!(cs->state.display.bplcon0_bits & BPLCON0_INTERLACE_MODE),
                !!(cs->state.display.bplcon0_bits & BPLCON0_EXTERNAL_SYNC),
                !!(cs->state.display.bplcon0_bits & BPLCON0_ENABLE_BPLCON3));
        return;

    case BPLCON1:
        cs->state.playfield[0].hscroll_delay =
            ((value & BPLCON1_PF1_HS_HI_MASK) >> BPLCON1_PF1_HS_HI_SHIFT) |
            ((value & BPLCON1_PF1_HS_MASK) << BPLCON1_PF1_HS_LSHIFT) |
            ((value & BPLCON1_PF1_HS_LO_MASK) >> BPLCON1_PF1_HS_LO_SHIFT);
        cs->state.playfield[1].hscroll_delay =
            ((value & BPLCON1_PF2_HS_HI_MASK) >> BPLCON1_PF2_HS_HI_SHIFT) |
            ((value & BPLCON1_PF2_HS_MASK) >> BPLCON1_PF2_HS_SHIFT) |
            ((value & BPLCON1_PF2_HS_LO_MASK) >> BPLCON1_PF2_HS_LO_SHIFT);
        /* Note: this register may change too rapidly for DPRINTF logging */
        break;  /* debug printf */

    case BPLCON2:
        cs->state.display.kill_ehb_mode = (value & BPLCON2_KILL_EHB);
        cs->state.palette.read_color_table = (value & BPLCON2_READ_RAM);
        cs->state.display.playfield_2_priority = (value & BPLCON2_PF2_PRIORITY);
        cs->state.playfield[1].priority_vs_sprites =
                (value & BPLCON2_PF2_SPR_MASK) >> BPLCON2_PF2_SPR_SHIFT;
        cs->state.playfield[0].priority_vs_sprites = (value & BPLCON2_PF1_SPR_MASK);

        DPRINTF("BPLCON2: kill_ehb=%d read_colors=%d pf2_pri=%d "
                "pf2_pri_vs_sprites=%d pf1_pri_vs_sprites=%d\n",
                cs->state.display.kill_ehb_mode,
                cs->state.palette.read_color_table,
                cs->state.display.playfield_2_priority,
                cs->state.playfield[1].priority_vs_sprites,
                cs->state.playfield[0].priority_vs_sprites);
        return;

    case BPLCON3:
        /* convert bank into offset (0, 32, 64...) */
        cs->state.palette.register_bank_offset =
            (value & BPLCON3_AGA_BANK_MASK) >> BPLCON3_AGA_BANK_SHIFT;
        /* convert power of 2 into offset, masking (1 << 0) into 0 offset */
        cs->state.playfield[1].color_offset = 0xfe &
            BIT((value & BPLCON3_PF2_OFF_MASK) >> BPLCON3_PF2_OFF_SHIFT);
        cs->state.palette.update_low_nibble = (value & BPLCON3_AGA_LOW_NIBBLE);
        cs->state.display.sprite_resolution =
            (value & BPLCON3_SPR_RES_MASK) >> BPLCON3_SPR_RES_SHIFT;

        DPRINTF("BPLCON3: color_bank_offset=%d pf2_color_offset=%d "
                "color_low_nibble=%d sprite_resolution=%d\n",
                cs->state.palette.register_bank_offset,
                cs->state.playfield[1].color_offset,
                cs->state.palette.update_low_nibble,
                cs->state.display.sprite_resolution);
        return;

    /* modulos for odd and even bitplanes */
    case BPL1MOD:
        cs->state.display.bitplane_modulo[0] = value;
        break;  /* debug printf */

    case BPL2MOD:
        cs->state.display.bitplane_modulo[1] = value;
        break;  /* debug printf */

    case BPLCON4:
        /* XXX: implement this */
        break;  /* debug printf */

    /* collision control 2 */
    case CLXCON2:
        /* XXX: implement this */
        break;  /* debug printf */

    case BPL1DAT ... BPL8DAT:
        /* XXX: implement this */
        break;  /* debug printf */

    case SPR0PTH ... SPR7PTL: {
        /* XXX: implement this */
        break;  /* debug printf */
    }

    case SPR0POS ... SPR7DATB: {
        /* XXX: implement this */
        break;  /* debug printf */
    }

    case COLOR00 ... COLOR31: {
        int index = (word_offset - COLOR00) +
                    cs->state.palette.register_bank_offset;

        if (unlikely(cs->state.palette.read_color_table)) {
            DPRINTF("ignoring write to color register %d in read mode\n",
                    index);
            return;
        }

        uint32_t rgb_low_nibble = ((value & AMIGA_RED_MASK) << 8) |
                              ((value & AMIGA_GREEN_MASK) << 4) |
                              (value & AMIGA_BLUE_MASK);

        if (cs->state.palette.update_low_nibble) {
            /* replace the low nibbles of this color palette entry */
            cs->state.palette.color[index] = rgb_low_nibble |
                (cs->state.palette.color[index] & RGB_HIGH_NIBBLE_MASK);
        } else {
            /* replace both high and low nibbles (ECS compatibility) */
            cs->state.palette.color[index] =
                rgb_low_nibble | (rgb_low_nibble << 4);
        }
        return;
    }

    case HTOTAL:
        cs->state.display.x_total = value;
        break;  /* debug printf */

    case HSSTOP:
        cs->state.display.x_pos_hsync_stop = value;
        break;  /* debug printf */

    case HBSTRT:
        cs->state.display.x_pos_hblank_start = value;
        break;  /* debug printf */

    case HBSTOP:
        cs->state.display.x_pos_hblank_stop = value;
        break;  /* debug printf */

    case VTOTAL:
        cs->state.display.y_total = value;
        break;  /* debug printf */

    case VSSTOP:
        cs->state.display.y_pos_vsync_stop = value;
        break;  /* debug printf */

    case VBSTRT:
        cs->state.display.y_pos_vblank_start = value;
        break;  /* debug printf */

    case VBSTOP:
        cs->state.display.y_pos_vblank_stop = value;
        break;  /* debug printf */

    case BEAMCON0:
        /* XXX: implement this */
        break;  /* debug printf */

    case HSSTRT:
        cs->state.display.x_pos_hsync_start = value;
        break;  /* debug printf */

    case VSSTRT:
        cs->state.display.y_pos_vsync_start = value;
        break;  /* debug printf */

    case HCENTER:
        cs->state.display.x_pos_vsync_interlace = value;
        break;  /* debug printf */

    case DIWHIGH:
        /* XXX: implement this */
        break;  /* debug printf */

    /* DMA fetch mode and scan doubling (AGA) */
    case FMODE:
        cs->state.display.bitplane_scan_double =
                                (value & FMODE_BPL_SCAN_DOUBLE);
        cs->state.display.sprite_scan_double =
                                (value & FMODE_SPR_SCAN_DOUBLE);

        /* DMA fetch size: 0 = 16-bit, 1 = 32-bit, 2 = 64-bit */
        cs->state.display.bitplane_fetch_size =
            (!!(value & FMODE_BPL_DBL_CAS)) + (!!(value & FMODE_BPL_32_BITS));
        cs->state.display.sprite_fetch_size =
            (!!(value & FMODE_SPR_DBL_CAS)) + (!!(value & FMODE_SPR_32_BITS));

        DPRINTF("FMODE: bitplane_double=%d sprite_double=%d "
                "bitplane_fetch_words=%d sprite_fetch_words=%d\n",
                cs->state.display.bitplane_scan_double,
                cs->state.display.sprite_scan_double,
                cs->state.display.bitplane_fetch_size,
                cs->state.display.sprite_fetch_size);
        return;

    case NOP:
        return;

    default:
        break;      /* fall through to error log */
    }

    /* unhandled registers and registers we want to trace writes to */
    DPRINTF("chipset write 0x%"PRIx16" to register %s\n",
            value, (chipset_register_names + (word_offset * 8)));
}

#if 0
typedef uint8_t packed_pixel_16 __attribute__ ((vector_size (16)));

/* pack one bitplane word into pixels using SIMD if possible */
static inline void pack_one_bitplane(uint16_t bitplane_word,
                                     uint8_t bitplane_bit,
                                     packed_pixel_16 *dest)
{
    packed_pixel_16 new_plane = {
        ((bitplane_word & (1 << 15)) ? bitplane_bit : 0),
        ((bitplane_word & (1 << 14)) ? bitplane_bit : 0),
        ((bitplane_word & (1 << 13)) ? bitplane_bit : 0),
        ((bitplane_word & (1 << 12)) ? bitplane_bit : 0),
        ((bitplane_word & (1 << 11)) ? bitplane_bit : 0),
        ((bitplane_word & (1 << 10)) ? bitplane_bit : 0),
        ((bitplane_word & (1 << 9)) ? bitplane_bit : 0),
        ((bitplane_word & (1 << 8)) ? bitplane_bit : 0),
        ((bitplane_word & (1 << 7)) ? bitplane_bit : 0),
        ((bitplane_word & (1 << 6)) ? bitplane_bit : 0),
        ((bitplane_word & (1 << 5)) ? bitplane_bit : 0),
        ((bitplane_word & (1 << 4)) ? bitplane_bit : 0),
        ((bitplane_word & (1 << 3)) ? bitplane_bit : 0),
        ((bitplane_word & (1 << 2)) ? bitplane_bit : 0),
        ((bitplane_word & (1 << 1)) ? bitplane_bit : 0),
        ((bitplane_word & (1 << 0)) ? bitplane_bit : 0)
    };

    *dest |= new_plane;
}

/* pack bitplanes into pixels using SIMD and no loops */
static packed_pixel_16 pack_next_16_pixels(uint16_t *bitplane_words,
                                                  int num_bitplanes)
{
    packed_pixel_16 pp16 = { };

    switch (num_bitplanes) {
    case 8:
        pack_one_bitplane(bitplane_words[7], 0x80, &pp16);
        /* fallthrough */
    case 7:
        pack_one_bitplane(bitplane_words[6], 0x40, &pp16);
        /* fallthrough */
    case 6:
        pack_one_bitplane(bitplane_words[5], 0x20, &pp16);
        /* fallthrough */
    case 5:
        pack_one_bitplane(bitplane_words[4], 0x10, &pp16);
        /* fallthrough */
    case 4:
        pack_one_bitplane(bitplane_words[3], 0x08, &pp16);
        /* fallthrough */
    case 3:
        pack_one_bitplane(bitplane_words[2], 0x04, &pp16);
        /* fallthrough */
    case 2:
        pack_one_bitplane(bitplane_words[1], 0x02, &pp16);
        /* fallthrough */
    case 1:
        pack_one_bitplane(bitplane_words[0], 0x01, &pp16);
    }

    return pp16;
}

/* pack bitplanes into pixels (0-4 planes) */
static packed_pixel_16 pack_next_16_pixels_max_4(uint16_t *bitplane_words,
                                                 int num_bitplanes)
{
    packed_pixel_16 pp16 = { };

    switch (num_bitplanes) {
    case 4:
        pack_one_bitplane(bitplane_words[3], 0x08, &pp16);
        /* fallthrough */
    case 3:
        pack_one_bitplane(bitplane_words[2], 0x04, &pp16);
        /* fallthrough */
    case 2:
        pack_one_bitplane(bitplane_words[1], 0x02, &pp16);
        /* fallthrough */
    case 1:
        pack_one_bitplane(bitplane_words[0], 0x01, &pp16);
    }

    return pp16;
}
#endif

/*
 * The main worker function, completing one loop per 3.55 MHz DMA cycle,
 * until caught up to the current clock time, or any pending CPU request
 * is completed. In both scenarios, the callback timer is modified to
 * call the loop again on the next DMA cycle, unless the CPU requests
 * to access a chipset register first.
 */
static void amiga_chipset_update_state(void *arg)
{
    AmigaChipset *cs = AMIGA_CHIPSET(arg);

    /* local copies of state, saved to struct on exit */
    int64_t last_cycle_emulated = cs->last_cycle_emulated;
    uint32_t uptime_days = cs->uptime_days;

    const bool ntsc = cs->ntsc;         /* system NTSC/PAL never changes */

    bool exit_event_loop = false;

    const uint64_t dma_cycles_per_day =
            CYCLES_PER_DAY(ntsc ? NTSC_CLK_PER_SEC : PAL_CLK_PER_SEC);

    /* loop until the main thread requests exit, and it's safe to do so */
    while (likely(!exit_event_loop)) {

        /* reset the cycle counters daily for clock conversion accuracy */
        if (unlikely(last_cycle_emulated > dma_cycles_per_day)) {
            uptime_days++;
            DPRINTF("uptime: %d day%s\n", uptime_days,
                    (uptime_days == 1) ? ", hooray!" : "s");

            last_cycle_emulated -= dma_cycles_per_day;
        }

        /* DMA cycle priority calculation */
        uint16_t horiz_counter = cs->state.display.alice_x_counter;
        if (horiz_counter == 0 || horiz_counter == 1 ||
            horiz_counter == 3 || horiz_counter == 5) {
            /* DRAM refresh */
        } else {

        }

        /* perform DMA cycle based on priorities and reserved slots */

        /* copper and blitter */

        /* update display with new pixels, in priority order */
        /* collision detection and sprites */
        /* color palette lookup */
        /* horizontal widening for low and hires pixels */

        /* hblank and vblank handling */
        /* on hblank, duplicate the just-completed scanline vertically */
        /* on vblank, send the just-completed frame to QEMU console */
        /* reset the copper on vblank, and other special handling */

        /* CIAs and timers */
        /* keyboard, mouse wheel */
        /* serial port */
        /* floppy drive control */

        /* send new interrupt level to CPU */

        /* update exit_event_loop if it's safe to exit */
    }

    /* save local state to struct */
    cs->last_cycle_emulated = last_cycle_emulated;
    cs->uptime_days = uptime_days;

    /* TODO: reset timer for next cycle */
}

static void amiga_chipset_realize(DeviceState *dev, Error **errp)
{
    AmigaChipset *cs = AMIGA_CHIPSET(dev);
    // SysBusDevice *sbd = SYS_BUS_DEVICE(dev);

    /* set up console framebuffer */
    /* set up keyboard / mouse events */
    /* set up audio output */
    /* FIXME: initialize and start worker thread */
    amiga_chipset_update_state(cs);
    /* FIXME: remove these bogus calls to suppress unused function errors */
    chipset_readw(cs, 123);
    chipset_writew(cs, 234, 0x4321);
    ticks_to_ns(1, true);
    ns_to_ticks(321, false);
}

#if 0

/* TODO: change this function to post to worker thread */
static uint64_t chipset_register_readfn(void *opaque, hwaddr addr,
                                        unsigned size)
{
    AmigaChipset *s = AMIGA_CHIPSET(opaque);

    switch (size) {
    // case 1:
        // return mmio_readb(s, addr);

    case 2:
        return chipset_readw(s, addr);

    default:
        g_assert_not_reached();
    }
}

/* TODO: change this function to post to worker thread */
static void chipset_register_writefn(void *opaque, hwaddr addr,
                                     uint64_t value, unsigned size)
{
    AmigaChipset *s = AMIGA_CHIPSET(opaque);

    switch (size) {
    case 1:
        if (addr & 0x01) {

        } else {

        }
        break;

    case 2:
        chipset_writew(s, addr, value);
        break;

    default:
        g_assert_not_reached();
    }
}

/* TODO: region ops for Gary */
static const MemoryRegionOps chipset_register_ops = {
    .read = chipset_register_readfn,
    .write = chipset_register_writefn,
    .impl = {
        .min_access_size = 2,   /* make QEMU pad accesses to word size */
        .max_access_size = 2,
    },
    .endianness = DEVICE_BIG_ENDIAN,
};

static uint64_t cia_register_readfn(void *opaque, hwaddr addr,
                                    unsigned size)
{
    // AmigaChipset *s = AMIGA_CHIPSET(opaque);

    switch (size) {
    // case 1:
        // return mmio_readb(s, addr);
    // case 2:
        // return mmio_readw(s, addr);
    default:
        g_assert_not_reached();
    }
}

static void cia_register_writefn(void *opaque, hwaddr addr,
                                 uint64_t value, unsigned size)
{
    // AmigaChipset *s = AMIGA_CHIPSET(opaque);

    switch (size) {
    // case 1:
        // mmio_writeb(s, addr, value);
        // break;
    // case 2:
        // mmio_writew(s, addr, value);
        // break;
    default:
        g_assert_not_reached();
    }
}

static const MemoryRegionOps cia_register_ops = {
    .read = cia_register_readfn,
    .write = cia_register_writefn,
    .impl = {
        .min_access_size = 1,
        .max_access_size = 2,
    },
    .endianness = DEVICE_BIG_ENDIAN,
};

#endif

/*
 * The m68k CPU has its own m68k_set_irq_level() function with an extra
 * argument to select the vector, so we can't use QEMU's generic GPIO to
 * interrupt mapping. It's unfortunate, because the rest of this code is
 * entirely CPU-independent.
 */
static Property amiga_chipset_properties[] = {
    DEFINE_PROP_LINK("cpu", AmigaChipset, cpu, TYPE_M68K_CPU, M68kCPU *),
    DEFINE_PROP_BOOL("ntsc", AmigaChipset, ntsc, false),
    DEFINE_PROP_END_OF_LIST(),
};

/*
 * Reset enter state: clear our local state.
 */
static void amiga_chipset_reset_enter(Object *obj, ResetType type)
{
    // AmigaChipset *cs = AMIGA_CHIPSET(obj);
}

/*
 * Reset hold state: reset the m68k interrupt level.
 */
static void amiga_chipset_reset_hold(Object *obj)
{
    AmigaChipset *cs = AMIGA_CHIPSET(obj);

    m68k_set_irq_level(cs->cpu, 0, 0);
}

/*
 * Reset exit state: start the DMA cycle timer.
 */
static void amiga_chipset_reset_exit(Object *obj)
{
    // start the DMA cycle timer
}

static void amiga_chipset_class_init(ObjectClass *oc, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(oc);
    ResettableClass *rc = RESETTABLE_CLASS(oc);

    dc->desc = "Amiga AGA Chipset";
    dc->realize = amiga_chipset_realize;
    rc->phases.enter = amiga_chipset_reset_enter;
    rc->phases.hold = amiga_chipset_reset_hold;
    rc->phases.exit = amiga_chipset_reset_exit;
    device_class_set_props(dc, amiga_chipset_properties);
    dc->vmsd = &amiga_chipset_vmstate;
}

static const TypeInfo amiga_chipset_info = {
    .name = TYPE_AMIGA_CHIPSET,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(AmigaChipset),
    .class_init = amiga_chipset_class_init,
};

static void amiga_chipset_register_types(void)
{
    type_register_static(&amiga_chipset_info);
}

type_init(amiga_chipset_register_types)
