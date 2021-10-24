/*
 * Definitions for Commodore Amiga.
 *
 * Copyright (c) 2021 Jake Hamby
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef AMIGA_H
#define AMIGA_H

#define DEBUG_AMIGA

#ifdef DEBUG_AMIGA
#define DPRINTF(fmt, ...) \
    do { printf("Amiga: " fmt , ## __VA_ARGS__); } while (0)
#else
#define DPRINTF(fmt, ...) do { } while (0)
#endif

#define TYPE_AMIGA_CHIPSET  "amiga-chipset"
#define TYPE_AMIGA_IDE      "amiga-ide"
#define TYPE_AMIGA_RTC      "amiga-rtc"

/* Called by Amiga machine init function to attach up to two drives. */
void amiga_ide_init_drives(DeviceState *dev, DriveInfo **hd);

/* 2 MB chip RAM @ 0x0 */
#define CHIP_RAM_BASE           0x00000000
#define CHIP_RAM_SIZE           0x00200000

/* 64 KB region for odd and even CIA registers */
#define CIA_BASE                0x00bf0000
#define CIA_SIZE                0x00010000

/* 4 KB region for Alice, Lisa, and Paula */
#define CUSTOM_CHIP_BASE        0x00dff000
#define CUSTOM_CHIP_SIZE        0x00001000

/* 64 KB region for RTC */
#define RTC_BASE                0x00dc0000
#define RTC_SIZE                0x00010000

/* 8 KB region for A4000 IDE controller */
#define AMIGA_4000_IDE_BASE     0x00dd2000
#define AMIGA_4000_IDE_SIZE     0x00002000

#endif /* AMIGA_H */
