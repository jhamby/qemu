/*
 * Amiga 3000/4000 RTC chip (Ricoh RP5C01A)
 *
 * Copyright (c) 2021 Jake Hamby
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 */

#include "qemu/osdep.h"
#include "hw/sysbus.h"
#include "qapi/error.h"
#include "qemu/module.h"
#include "qemu/timer.h"
#include "qom/object.h"
#include "migration/vmstate.h"
#include "hw/misc/amiga.h"

/* RP5C01A registers (mode 00) */
#define RTC_1_SECOND_COUNTER        0x00
#define RTC_10_SECOND_COUNTER       0x01
#define RTC_1_MINUTE_COUNTER        0x02
#define RTC_10_MINUTE_COUNTER       0x03
#define RTC_1_HOUR_COUNTER          0x04
#define RTC_10_HOUR_COUNTER         0x05
#define RTC_DAY_OF_WEEK_COUNTER     0x06
#define RTC_1_DAY_COUNTER           0x07
#define RTC_10_DAY_COUNTER          0x08
#define RTC_1_MONTH_COUNTER         0x09
#define RTC_10_MONTH_COUNTER        0x0a
#define RTC_1_YEAR_COUNTER          0x0b
#define RTC_10_YEAR_COUNTER         0x0c
#define RTC_MODE_REGISTER           0x0d
#define RTC_TEST_REGISTER           0x0e
#define RTC_RESET_REGISTER          0x0f

#define RTC_REGISTER_MASK           0x0f

/* RP5C01A registers (mode 01) */
#define RTC_12_OR_24_HOUR_SELECT    0x0a
#define RTC_LEAP_YEAR_COUNTER       0x0b

#define RTC_MODE_MASK               0x03    /* low 2 bits of mode register */
#define RTC_ALARM_ENABLE            0x04
#define RTC_TIMER_ENABLE            0x08

/* register names for debugging. */
#ifdef DEBUG_AMIGA
static const char *rtc_register_name[] = {
    [RTC_1_SECOND_COUNTER]      = "1-second",
    [RTC_10_SECOND_COUNTER]     = "10-second",
    [RTC_1_MINUTE_COUNTER]      = "1-minute",
    [RTC_10_MINUTE_COUNTER]     = "10-minute",
    [RTC_1_HOUR_COUNTER]        = "1-hour",
    [RTC_10_HOUR_COUNTER]       = "10-hour",
    [RTC_DAY_OF_WEEK_COUNTER]   = "day-of-week",
    [RTC_1_DAY_COUNTER]         = "1-day",
    [RTC_10_DAY_COUNTER]        = "10-day",
    [RTC_1_MONTH_COUNTER]       = "1-month",
    [RTC_10_MONTH_COUNTER]      = "10-month",
    [RTC_1_YEAR_COUNTER]        = "1-year",
    [RTC_10_YEAR_COUNTER]       = "10-year",
    [RTC_MODE_REGISTER]         = "mode",
    [RTC_TEST_REGISTER]         = "test",
    [RTC_RESET_REGISTER]        = "reset",
};
#endif

OBJECT_DECLARE_SIMPLE_TYPE(AmigaRtcState, AMIGA_RTC)

struct AmigaRtcState {
    SysBusDevice parent_obj;

    MemoryRegion iomem;
    uint8_t mode;
    bool use_24_hour_mode;
    uint8_t ram[13];        /* bank 0 in low nibble, bank 1 in high nibble */
};

/*
 * Return clock time from QEMU host, or the two mode registers.
 * The RAM in modes 2 and 3 is preserved, but not the
 * contents of the unsupported alarm registers and enable bits.
 */
static uint64_t amiga_rtc_read(void *opaque, hwaddr addr, unsigned size)
{
    AmigaRtcState *s = AMIGA_RTC(opaque);
    uint32_t chip_addr = ((addr >> 2) & RTC_REGISTER_MASK);
    uint32_t result;

    /* RTC is mapped into the low nibble of 0x03, 0x07, 0x0b, 0x0f, ... */
    if (unlikely((size == 1 && ((addr & 0x03) != 0x03)) ||
        (size == 2 && ((addr & 0x03) != 0x02)))) {
        DPRINTF("RTC returning 0 for read of size %u from %s register\n",
                size, rtc_register_name[chip_addr]);
        return 0;
    }

    if (chip_addr == RTC_MODE_REGISTER) {
        /* This device always runs as if the timer is enabled. */
        result = s->mode | RTC_TIMER_ENABLE;
    } else if (chip_addr >= RTC_TEST_REGISTER) {
        result = 0;
    } else switch (s->mode) {
    case 0: {
        /* mode 00: return local time */
        time_t time_h = time(NULL);
        struct tm info;
        localtime_r(&time_h, &info);

        switch (chip_addr) {
        case RTC_1_SECOND_COUNTER:
            result = info.tm_sec % 10;
            break;

        case RTC_10_SECOND_COUNTER:
            result = info.tm_sec / 10;
            break;

        case RTC_1_MINUTE_COUNTER:
            result = info.tm_min % 10;
            break;

        case RTC_10_MINUTE_COUNTER:
            result = info.tm_min / 10;
            break;

        case RTC_1_HOUR_COUNTER:
            if (s->use_24_hour_mode) {
                result = info.tm_hour % 10;
            } else {
                int twelve_hour = info.tm_hour;
                if (twelve_hour == 0) {
                    twelve_hour = 12;
                } else if (twelve_hour > 12) {
                    twelve_hour -= 12;
                }
                result = twelve_hour % 10;
            }
            break;

        case RTC_10_HOUR_COUNTER:
            if (s->use_24_hour_mode) {
                result = info.tm_hour / 10;
            } else {
                /* bit 1 = 1 for p.m., 0 for a.m. */
                switch (info.tm_hour) {
                case 0:
                    result = 0x01;      /* 12:00 a.m. */
                    break;
                case 1 ... 9:
                    result = 0x00;
                    break;
                case 10 ... 11:
                    result = 0x01;
                    break;
                case 12:
                    result = 0x03;      /* 12:00 p.m. */
                    break;
                case 13 ... 21:
                    result = 0x02;
                    break;
                case 22 ... 23:
                    result = 0x03;
                    break;
                default:
                    g_assert_not_reached();
                    return 0;
                }
            }
            break;

        case RTC_DAY_OF_WEEK_COUNTER:
            result = info.tm_wday;    /* is Sunday == 0 correct? */
            break;

        case RTC_1_DAY_COUNTER:
            result = info.tm_mday % 10;
            break;

        case RTC_10_DAY_COUNTER:
            result = info.tm_mday / 10;
            break;

        case RTC_1_MONTH_COUNTER:
            result = (info.tm_mon + 1) % 10;
            break;

        case RTC_10_MONTH_COUNTER:
            result = (info.tm_mon + 1) / 10;
            break;

        case RTC_1_YEAR_COUNTER:
            result = info.tm_year % 10;
            break;

        case RTC_10_YEAR_COUNTER:
            /* Note: this will work until the year 2077 */
            result = (info.tm_year / 10) % 10;
            break;

        default:
            /* the other registers were handled above */
            g_assert_not_reached();
            return 0;
        }
        break;
    }

    case 1:
        /* mode 01: only 12/24 hour register is implemented */
        if (chip_addr == RTC_12_OR_24_HOUR_SELECT) {
            result = s->use_24_hour_mode;
        } else {
            result = 0;
        }
        break;

    case 2:
        result = s->ram[chip_addr] & 0x0f;
        break;

    case 3:
        result = (s->ram[chip_addr] & 0xf0) >> 4;
        break;
    }

    DPRINTF("RTC read from %s returns %"PRIu32")\n",
            rtc_register_name[chip_addr], result);
    return result;
}

/*
 * Handle writes to 12/24 select and mode registers (mode 1).
 * Ignore writes to RTC time and unsupported alarm registers.
 * Save values to RAM when in modes 2 and 3.
 */
static void amiga_rtc_write(void *opaque, hwaddr addr,
                            uint64_t val, unsigned size)
{
    AmigaRtcState *s = AMIGA_RTC(opaque);
    uint32_t chip_addr = ((addr >> 2) & RTC_REGISTER_MASK);

    /* RTC is mapped into the low nibble of 0x03, 0x07, 0x0b, 0x0f, ... */
    if (unlikely((size == 1 && ((addr & 0x03) != 0x03)) ||
        (size == 2 && ((addr & 0x03) != 0x02)))) {
        DPRINTF("RTC ignoring unmapped write of %"PRIu64" size %u to %s\n",
                val, size, rtc_register_name[chip_addr]);
        return;
    }

    switch (chip_addr) {
    case RTC_MODE_REGISTER:
        s->mode = (val & RTC_MODE_MASK);
        DPRINTF("RTC setting mode to %"PRIu8", alarm=%d, timer=%d\n",
                s->mode,
                !!(val & RTC_ALARM_ENABLE),
                !!(val & RTC_TIMER_ENABLE));
        return;

    case RTC_TEST_REGISTER:
    case RTC_RESET_REGISTER:
        /* unsupported */
        break;

    default: {
        switch (s->mode) {
        case 0:
            /* ignore and log writes to RTC time */
            break;

        case 1:
            if (chip_addr == RTC_12_OR_24_HOUR_SELECT) {
                s->use_24_hour_mode = (val & 0x01);
                return;
            }
            /* ignore and log everything else */
            break;

        case 2:
            s->ram[chip_addr] = (s->ram[chip_addr] & 0xf0) | (val & 0x0f);
            return;

        case 3:
            s->ram[chip_addr] = (s->ram[chip_addr] & 0x0f) |
                                ((val & 0x0f) << 4);
            return;
        }
    }
    }

    /* Anything that isn't handled falls through to here. */
    DPRINTF("RTC ignoring write of %"PRIu64" to register %s\n",
            val, rtc_register_name[chip_addr]);
}

/* We can handle 1, 2, and 4 byte reads/writes. */
static const MemoryRegionOps amiga_rtc_ops = {
    .read = amiga_rtc_read,
    .write = amiga_rtc_write,
    .endianness = DEVICE_BIG_ENDIAN,
};

static const VMStateDescription amiga_rtc_vmstate = {
    .name = TYPE_AMIGA_RTC,
    .version_id = 0,
    .minimum_version_id = 0,
    .fields = (VMStateField[]) {
        VMSTATE_UINT8(mode, AmigaRtcState),
        VMSTATE_BOOL(use_24_hour_mode, AmigaRtcState),
        VMSTATE_BUFFER(ram, AmigaRtcState),
        VMSTATE_END_OF_LIST()
    },
};

static void amiga_rtc_realize(DeviceState *dev, Error **errp)
{
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);
    AmigaRtcState *s = AMIGA_RTC(dev);

    memory_region_init_io(&s->iomem, OBJECT(s), &amiga_rtc_ops, s,
                          TYPE_AMIGA_RTC, RTC_SIZE);
    sysbus_init_mmio(sbd, &s->iomem);

    /* initialize and zero the registers and RAM */
    s->mode = 0;
    s->use_24_hour_mode = true;
    memset(s->ram, 0, sizeof(s->ram));
}

static void amiga_rtc_class_init(ObjectClass *oc, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = amiga_rtc_realize;
    dc->vmsd = &amiga_rtc_vmstate;
}

static const TypeInfo amiga_rtc_info = {
    .name          = TYPE_AMIGA_RTC,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(AmigaRtcState),
    .class_init    = amiga_rtc_class_init,
};

static void amiga_rtc_register_types(void)
{
    type_register_static(&amiga_rtc_info);
}

type_init(amiga_rtc_register_types)
