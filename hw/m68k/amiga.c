/*
 * Amiga 4000 System Driver
 *
 * Copyright (c) 2021 Jake Hamby
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 */

#include "qemu/osdep.h"
#include "qemu/units.h"
#include "exec/hwaddr.h"
#include "sysemu/reset.h"
#include "sysemu/sysemu.h"
#include "sysemu/qtest.h"
#include "hw/irq.h"
#include "hw/boards.h"
#include "hw/loader.h"
#include "hw/sysbus.h"
#include "qom/object.h"
#include "hw/qdev-properties.h"
#include "qapi/error.h"
#include "ui/console.h"
#include "target/m68k/cpu.h"
#include "migration/vmstate.h"
#include "hw/misc/amiga.h"

/* 512 KB Kickstart ROM */
#define KICKSTART_ROM_FILE          "kicka4000.rom"
#define KICKSTART_ROM_FILE_SCSI     "kicka4000t.rom"
#define KICKSTART_ROM_BASE          0x00f80000
#define KICKSTART_ROM_SIZE          0x00080000

/* Up to 112 MiB fast RAM @ 0x01000000 autoconfigured.
 * A real A4000 can have up to 16 MiB @ 0x07000000.
 * Fill downward from the end (0x07ffffff).
 */
#define FAST_RAM_A4000_BASE         0x01000000
#define FAST_RAM_A4000_END          0x08000000
#define MAX_A4000_FAST_RAM      (FAST_RAM_A4000_END - FAST_RAM_A4000_BASE)

#define TYPE_AMIGA_MACHINE MACHINE_TYPE_NAME("amiga")
OBJECT_DECLARE_SIMPLE_TYPE(AmigaState, AMIGA_MACHINE)

/* We currently have one machine option, NTSC, passed to the chipset. */
struct AmigaState {
    MachineState parent;

    bool ntsc;      /* Note: I think QEMU saves properties for me? */
    /* TODO: add Gary interrupt state? */
    /* TODO: add Ramsey config */
};

/* Custom handling of m68k reset to avoid emulating the ROM overlay bit. */
static void main_cpu_reset(void *opaque)
{
    M68kCPU *cpu = opaque;
    CPUState *cs = CPU(cpu);

    cpu_reset(cs);

    /* Jump directly into Kickstart instead of emulating overlay at 0x0 */
    cpu->env.aregs[7] = ldl_phys(cs->as, KICKSTART_ROM_BASE);
    cpu->env.pc = ldl_phys(cs->as, KICKSTART_ROM_BASE + 4);
    DPRINTF("CPU RESET: jumping to Kickstart at 0x%"PRIx32"\n", cpu->env.pc);
}

static bool amiga_get_ntsc(Object *obj, Error **errp)
{
    AmigaState *state = AMIGA_MACHINE(obj);
    return state->ntsc;
}

static void amiga_set_ntsc(Object *obj, bool value, Error **errp)
{
    AmigaState *state = AMIGA_MACHINE(obj);
    state->ntsc = value;
}

static void amiga_instance_init(Object *obj)
{
    object_property_add_bool(obj, "ntsc", amiga_get_ntsc, amiga_set_ntsc);
    object_property_set_description(obj, "ntsc", "Use NTSC instead of PAL.");
}

static void amiga_machine_init(MachineState *machine)
{
    M68kCPU *cpu;
    MemoryRegion *rom = g_new(MemoryRegion, 1);
    MemoryRegion *sysmem = get_system_memory();
    const char *kickstart_name = machine->firmware ?: KICKSTART_ROM_FILE;
    AmigaState *state = AMIGA_MACHINE(machine);

    /* The A4000 recognizes up to 112 MiB fast RAM ending at 0x08000000. */
    if (machine->ram_size > MAX_A4000_FAST_RAM) {
        error_report("Too much memory for this machine: %"PRId64" MiB, "
                     "maximum %"PRId64" MiB", machine->ram_size / MiB,
                     (MAX_A4000_FAST_RAM / MiB));
        exit(1);
    }

    /* Initialize the cpu core */
    cpu = M68K_CPU(cpu_create(machine->cpu_type));
    qemu_register_reset(main_cpu_reset, cpu);

    /* Fill downwards from the end of the A4000 motherboard RAM region. */
    const hwaddr fast_ram_base = FAST_RAM_A4000_END - machine->ram_size;
    memory_region_add_subregion(sysmem, fast_ram_base, machine->ram);

    /* Custom chip emulation (will map 2 MiB chip RAM and chip registers). */
    DeviceState *dev = qdev_new(TYPE_AMIGA_CHIPSET);
    object_property_set_link(OBJECT(dev), "cpu", OBJECT(cpu), &error_abort);
    object_property_set_bool(OBJECT(dev), "ntsc", state->ntsc, &error_abort);
    DPRINTF("Creating %s Amiga chipset\n", state->ntsc ? "NTSC" : "PAL");
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, 0);

    /* TODO: Gary, Ramsey */

    /* Amiga 3000/4000 RTC. */
    dev = qdev_new(TYPE_AMIGA_RTC);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, RTC_BASE);

    /* Amiga 4000 IDE controller with up to two drives. */
    dev = qdev_new(TYPE_AMIGA_IDE);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);
    sysbus_mmio_map(SYS_BUS_DEVICE(dev), 0, AMIGA_4000_IDE_BASE);
    DriveInfo *hd[2] = {
        drive_get(IF_IDE, 0, 0),
        drive_get(IF_IDE, 0, 1)
    };
    amiga_ide_init_drives(dev, hd);

    /* Load Kickstart ROM. The overlay at 0x0 will be immediately disabled
       with a CIA write, so we probably don't need to emulate it.  */
    memory_region_init_rom(rom, NULL, kickstart_name, KICKSTART_ROM_SIZE,
                           &error_fatal);
    memory_region_add_subregion(sysmem, KICKSTART_ROM_BASE, rom);
    if (load_image_targphys(kickstart_name, KICKSTART_ROM_BASE,
                            KICKSTART_ROM_SIZE) < 8) {
        if (!qtest_enabled()) {
            error_report("Failed to load firmware '%s'.", kickstart_name);
        }
    }
}

static void amiga_machine_class_init(ObjectClass *oc, void *data)
{
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "Amiga 4000";
    mc->init = amiga_machine_init;
    mc->block_default_type = IF_IDE;
    mc->default_ram_size = MAX_A4000_FAST_RAM;
    mc->default_ram_id = "amiga.ram";
    mc->default_cpu_type = M68K_CPU_TYPE_NAME("m68040");
}

static const TypeInfo amiga_machine_typeinfo = {
    .name       = TYPE_AMIGA_MACHINE,
    .parent     = TYPE_MACHINE,
    .class_init = amiga_machine_class_init,
    .instance_size = sizeof(AmigaState),
    .instance_init = amiga_instance_init,
};

static void amiga_machine_register_types(void)
{
    type_register_static(&amiga_machine_typeinfo);
}

type_init(amiga_machine_register_types)
