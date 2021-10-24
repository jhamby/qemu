/*
 * QEMU IDE Emulation: Amiga 4000/4000T 16-bit IDE interface
 *
 * Copyright (c) 2003 Fabrice Bellard
 * Copyright (c) 2006 Openedhand Ltd.
 * Copyright (c) 2021 Jake Hamby
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "qemu/osdep.h"
#include "hw/sysbus.h"
#include "migration/vmstate.h"
#include "qemu/module.h"
#include "sysemu/blockdev.h"
#include "sysemu/dma.h"

#include "hw/ide/internal.h"
#include "hw/misc/amiga.h"
#include "hw/qdev-properties.h"
#include "qom/object.h"

OBJECT_DECLARE_SIMPLE_TYPE(AmigaIDEState, AMIGA_IDE)

struct AmigaIDEState {
    SysBusDevice parent_obj;

    IDEBus bus;

    bool saved_irq;
    MemoryRegion iomem;
};

static uint64_t amiga_ide_read(void *opaque, hwaddr addr,
                               unsigned size)
{
    AmigaIDEState *s = AMIGA_IDE(opaque);

    uint32_t port_addr = (addr >> 2) & 0x07;
    uint32_t byte_shift = (size == 2) ? 8 : 0;

    switch (addr & 0x1002) {
    case 0x0000:
        /* 16-bit data read (0x1f0) */
        if (unlikely(port_addr != 0)) {
            /* return the 8-bit status shifted to big-endian word. */
            DPRINTF("Error: 16-bit IDE control register read.\n");
            return ide_ioport_read(&s->bus, port_addr) << byte_shift;
        } else {
#ifdef DEBUG_AMIGA
            if (unlikely(size == 1)) {
                DPRINTF("Error: 8-bit IDE data read at 16-bit address!\n");
            }
#endif
            return ide_data_readw(&s->bus, port_addr);
        }

    case 0x0002:
        /* 8-bit port read (0x1f1 - 1x1f7) */
#ifdef DEBUG_AMIGA
        if (unlikely(port_addr == 0)) {
            DPRINTF("Warning: 8-bit IDE data read may not work!\n");
        }
#endif
        return ide_ioport_read(&s->bus, port_addr) << byte_shift;

    case 0x1000:
        /* highest bit of 8/16-bit IDE interrupt register */
        return s->saved_irq ? (0x80 << byte_shift) : 0;

    case 0x1002:
        /* 8-bit IDE status register (0x3f6 - 0x3f7) */
        return ide_status_read(&s->bus, port_addr) << byte_shift;

    default:
        g_assert_not_reached();
        return 0;
    }
}

static void amiga_ide_write(void *opaque, hwaddr addr,
                            uint64_t val, unsigned size)
{
    AmigaIDEState *s = AMIGA_IDE(opaque);

    uint32_t port_addr = (addr >> 2) & 0x07;
    uint32_t byte_shift = (size == 2) ? 8 : 0;

    switch (addr & 0x1002) {
    case 0x0000:
        /* 16-bit data write (0x1f0) */
        if (unlikely(port_addr != 0)) {
            /* write the 8-bit status shifted from big-endian word. */
            DPRINTF("Error: 16-bit IDE control register write.\n");
            ide_ioport_write(&s->bus, port_addr, (val >> byte_shift));
            break;
        }
#ifdef DEBUG_AMIGA
        if (unlikely(size == 1)) {
            DPRINTF("Error: 8-bit IDE data write at 16-bit address!\n");
        }
#endif
        ide_data_writew(&s->bus, port_addr, val);
        break;

    case 0x0002:
        /* 8-bit port write (0x1f1 - 1x1f7) */
#ifdef DEBUG_AMIGA
        if (unlikely(size == 2)) {
            DPRINTF("Error: 16-bit IDE port write at 8-bit address!\n");
        }
#endif
        ide_ioport_write(&s->bus, port_addr, (val >> byte_shift));
        break;

    default:
        DPRINTF("Error: unhandled IDE write value 0x%"PRIx64" size %u to "
                "addr 0x%"PRIx64"\n", val, size, addr);
    }
}

/* Note: the 16-bit data accesses are big-endian (I hope). */
static const MemoryRegionOps amiga_ide_ops = {
    .read = amiga_ide_read,
    .write = amiga_ide_write,
    .impl = {
        .min_access_size = 1,
        .max_access_size = 2,
    },
    .endianness = DEVICE_BIG_ENDIAN,    /* FIX THIS if incorrect */
};

static const VMStateDescription vmstate_amiga_ide = {
    .name = TYPE_AMIGA_IDE,
    .version_id = 0,
    .minimum_version_id = 0,
    .fields = (VMStateField[]) {
        VMSTATE_IDE_BUS(bus, AmigaIDEState),
        VMSTATE_IDE_DRIVES(bus.ifs, AmigaIDEState),
        VMSTATE_BOOL(saved_irq, AmigaIDEState),
        VMSTATE_END_OF_LIST()
    }
};

static void amiga_ide_set_irq(void *opaque, int irq, int level)
{
    AmigaIDEState *s = AMIGA_IDE(opaque);
    s->saved_irq = level;
}

static void amiga_ide_realizefn(DeviceState *dev, Error **errp)
{
    SysBusDevice *d = SYS_BUS_DEVICE(dev);
    AmigaIDEState *s = AMIGA_IDE(dev);

    /* handle the interrupt ourselves */
    ide_init2(&s->bus, qdev_get_gpio_in(DEVICE(s), 0));

    memory_region_init_io(&s->iomem, OBJECT(s), &amiga_ide_ops, s,
                          TYPE_AMIGA_IDE, AMIGA_4000_IDE_SIZE);
    sysbus_init_mmio(d, &s->iomem);
}

/* Initialize the IDE bus and make a GPIO in for its interrupts. */
static void amiga_ide_initfn(Object *obj)
{
    AmigaIDEState *s = AMIGA_IDE(obj);

    ide_bus_init(&s->bus, sizeof(s->bus), DEVICE(obj), 0, 2);

    qdev_init_gpio_in(DEVICE(s), amiga_ide_set_irq, 1);
}

/* Reset our local state during enter phase. */
static void amiga_ide_reset_enter(Object *obj, ResetType type)
{
    AmigaIDEState *s = AMIGA_IDE(obj);

    DPRINTF("IDE entering reset phase.\n");
    s->saved_irq = false;
}

/* Call the IDE reset function during hold phase. */
static void amiga_ide_reset_hold(Object *obj)
{
    AmigaIDEState *s = AMIGA_IDE(obj);

    DPRINTF("IDE resetting bus.\n");
    ide_bus_reset(&s->bus);
}

static void amiga_ide_class_init(ObjectClass *oc, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(oc);
    ResettableClass *rc = RESETTABLE_CLASS(oc);

    dc->realize = amiga_ide_realizefn;
    dc->vmsd = &vmstate_amiga_ide;
    rc->phases.enter = amiga_ide_reset_enter;
    rc->phases.hold = amiga_ide_reset_hold;
    set_bit(DEVICE_CATEGORY_STORAGE, dc->categories);
}

static const TypeInfo amiga_ide_type_info = {
    .name = TYPE_AMIGA_IDE,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(AmigaIDEState),
    .instance_init = amiga_ide_initfn,
    .class_init = amiga_ide_class_init,
};

static void amiga_ide_register_types(void)
{
    type_register_static(&amiga_ide_type_info);
}

/* Called by Amiga machine init function to attach up to two drives. */
void amiga_ide_init_drives(DeviceState *dev, DriveInfo **hd)
{
    AmigaIDEState *s = AMIGA_IDE(dev);

    for (int i = 0; i < 2; ++i) {
        if (hd[i] != NULL) {
            DPRINTF("Attaching IDE %s drive as unit %d",
                    (hd[i]->media_cd ? "CD-ROM" : "hard"), i);

            ide_create_drive(&s->bus, i, hd[i]);
        }
    }
}

type_init(amiga_ide_register_types)
