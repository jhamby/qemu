=============================
QEMU Amiga/m68k Release Notes
=============================

This GitHub repo contains Jake Hamby's implementation of a Commodore
Amiga 4000/040, including the AGA chipset. My intention is to
contribute this code to the official QEMU repo as soon as it's
sufficiently reliable and fast.

The code doesn't work yet but it should at least compile.
I hope to complete the project over the next several weeks.
The following are the release notes for the planed 1.0 release.

Amiga 4000 IDE support is implemented, but no floppy emulation (yet).
The default system type is PAL, with an "ntsc" machine option boolean.
It has 2 MiB chip RAM, keyboard, mouse on port 1, and no joystick.
The default, and maximum autoconfigurable, fast RAM size is 112 MiB.

The chipset emulation is incomplete and it doesn't run yet.

The left-Amiga key is mapped to left-Meta (aka left-Windows key).
The right-Amiga key is mapped to right-Ctrl, right-Meta, and Menu,
to accommodate keyboards which may only have one of those keys.
The End key (and Help, if present) are mapped to Help, and the
Home and PgUp keys map to numeric keypad left and right parens.
PgDn is mapped to the international key left of Return, and both
Insert and the angle brackets key, if present, are mapped to the
angle brackets key to the right of left Shift. The optional A3000
10-second warm reset warning for Ctrl-Amiga-Amiga is implemented.
Mouse wheel events are sent through keyboard scancodes as well.

Planned features:

* floppy disk images, including .adf and MS-DOS 720K/1.44M formats
* optimized screen refresh mode for low-bandwidth remote connections
* SCSI support (emulating Amiga 4000T built-in controller)
* RS-232 serial (Paula) and parallel printer (CIAs) ports
* Zorro II/III bus support (including INT2 and INT6 GPIO lines)
* A2065 Ethernet support (use existing QEMU Lance Ethernet)
* Zorro expansion RAM (up to 2 GB, vs. 112 MB motherboard RAM)
* Picasso II/IV or another suitable RTG-compatible graphics card
* USB gamepad (and second mouse support if possible)
* Configurable stereo separation and Amiga low-pass audio filter
* interface to real Amiga devices via Raspberry Pi GPIO lines
* PCI bus interface and virtio devices (with AmigaOS support)

Three aspects of the A4000 have been intentionally left unimplemented.
The power-on overlay of the Kickstart ROM at address 0x0 in place of
chip RAM is not implemented because the CPU reset has been configured
to jump into the Kickstart ROM address directly. Kickstart immediately
turns off the OVL bit in order to access chip RAM, and it would make
no sense to re-enable that bit and lose access to chip RAM.

The copper / blitter DMA interaction bug hasn't been implemented,
although if it would apply, a DPRINTF() warning is printed, in order
to see if it would make sense to add an option for bug-compatibility.

The last "feature" which was intentionally omitted is the A4000-only
chip register shadow in the 1MB region at 0x00c00000, where 24-bit
addressing Amigas sometimes map fast or pseudo-fast RAM. The CBM
chip specification for Gary (4393) says this shadow mapping is for
compatibility, but it makes no sense to map chip registers where
some older program may be expecting to see RAM, if anything. So the
address space from 0xc00000 to 0xcfffff is left unmapped instead.

Original README below.

===========
QEMU README
===========

QEMU is a generic and open source machine & userspace emulator and
virtualizer.

QEMU is capable of emulating a complete machine in software without any
need for hardware virtualization support. By using dynamic translation,
it achieves very good performance. QEMU can also integrate with the Xen
and KVM hypervisors to provide emulated hardware while allowing the
hypervisor to manage the CPU. With hypervisor support, QEMU can achieve
near native performance for CPUs. When QEMU emulates CPUs directly it is
capable of running operating systems made for one machine (e.g. an ARMv7
board) on a different machine (e.g. an x86_64 PC board).

QEMU is also capable of providing userspace API virtualization for Linux
and BSD kernel interfaces. This allows binaries compiled against one
architecture ABI (e.g. the Linux PPC64 ABI) to be run on a host using a
different architecture ABI (e.g. the Linux x86_64 ABI). This does not
involve any hardware emulation, simply CPU and syscall emulation.

QEMU aims to fit into a variety of use cases. It can be invoked directly
by users wishing to have full control over its behaviour and settings.
It also aims to facilitate integration into higher level management
layers, by providing a stable command line interface and monitor API.
It is commonly invoked indirectly via the libvirt library when using
open source applications such as oVirt, OpenStack and virt-manager.

QEMU as a whole is released under the GNU General Public License,
version 2. For full licensing details, consult the LICENSE file.


Documentation
=============

Documentation can be found hosted online at
`<https://www.qemu.org/documentation/>`_. The documentation for the
current development version that is available at
`<https://www.qemu.org/docs/master/>`_ is generated from the ``docs/``
folder in the source tree, and is built by `Sphinx
<https://www.sphinx-doc.org/en/master/>_`.


Building
========

QEMU is multi-platform software intended to be buildable on all modern
Linux platforms, OS-X, Win32 (via the Mingw64 toolchain) and a variety
of other UNIX targets. The simple steps to build QEMU are:


.. code-block:: shell

  mkdir build
  cd build
  ../configure
  make

Additional information can also be found online via the QEMU website:

* `<https://wiki.qemu.org/Hosts/Linux>`_
* `<https://wiki.qemu.org/Hosts/Mac>`_
* `<https://wiki.qemu.org/Hosts/W32>`_


Submitting patches
==================

The QEMU source code is maintained under the GIT version control system.

.. code-block:: shell

   git clone https://gitlab.com/qemu-project/qemu.git

When submitting patches, one common approach is to use 'git
format-patch' and/or 'git send-email' to format & send the mail to the
qemu-devel@nongnu.org mailing list. All patches submitted must contain
a 'Signed-off-by' line from the author. Patches should follow the
guidelines set out in the `style section
<https://www.qemu.org/docs/master/devel/style.html>` of
the Developers Guide.

Additional information on submitting patches can be found online via
the QEMU website

* `<https://wiki.qemu.org/Contribute/SubmitAPatch>`_
* `<https://wiki.qemu.org/Contribute/TrivialPatches>`_

The QEMU website is also maintained under source control.

.. code-block:: shell

  git clone https://gitlab.com/qemu-project/qemu-web.git

* `<https://www.qemu.org/2017/02/04/the-new-qemu-website-is-up/>`_

A 'git-publish' utility was created to make above process less
cumbersome, and is highly recommended for making regular contributions,
or even just for sending consecutive patch series revisions. It also
requires a working 'git send-email' setup, and by default doesn't
automate everything, so you may want to go through the above steps
manually for once.

For installation instructions, please go to

*  `<https://github.com/stefanha/git-publish>`_

The workflow with 'git-publish' is:

.. code-block:: shell

  $ git checkout master -b my-feature
  $ # work on new commits, add your 'Signed-off-by' lines to each
  $ git publish

Your patch series will be sent and tagged as my-feature-v1 if you need to refer
back to it in the future.

Sending v2:

.. code-block:: shell

  $ git checkout my-feature # same topic branch
  $ # making changes to the commits (using 'git rebase', for example)
  $ git publish

Your patch series will be sent with 'v2' tag in the subject and the git tip
will be tagged as my-feature-v2.

Bug reporting
=============

The QEMU project uses GitLab issues to track bugs. Bugs
found when running code built from QEMU git or upstream released sources
should be reported via:

* `<https://gitlab.com/qemu-project/qemu/-/issues>`_

If using QEMU via an operating system vendor pre-built binary package, it
is preferable to report bugs to the vendor's own bug tracker first. If
the bug is also known to affect latest upstream code, it can also be
reported via GitLab.

For additional information on bug reporting consult:

* `<https://wiki.qemu.org/Contribute/ReportABug>`_


ChangeLog
=========

For version history and release notes, please visit
`<https://wiki.qemu.org/ChangeLog/>`_ or look at the git history for
more detailed information.


Contact
=======

The QEMU community can be contacted in a number of ways, with the two
main methods being email and IRC

* `<mailto:qemu-devel@nongnu.org>`_
* `<https://lists.nongnu.org/mailman/listinfo/qemu-devel>`_
* #qemu on irc.oftc.net

Information on additional methods of contacting the community can be
found online via the QEMU website:

* `<https://wiki.qemu.org/Contribute/StartHere>`_
