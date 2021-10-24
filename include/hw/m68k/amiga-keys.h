/*
 * Amiga keycode mapping.
 *
 * Copyright (c) 2021 Jake Hamby
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#ifndef AMIGA_KEYS_H
#define AMIGA_KEYS_H

/*
 * Enums for all Amiga keycodes, shifted left one bit to leave
 * room for the key-up bit, which is always sent last.
 */
enum {
    AMIGA_KEY_GRAVE_ACCENT = (0x00 << 1),
    AMIGA_KEY_1 = (0x01 << 1),
    AMIGA_KEY_2 = (0x02 << 1),
    AMIGA_KEY_3 = (0x03 << 1),
    AMIGA_KEY_4 = (0x04 << 1),
    AMIGA_KEY_5 = (0x05 << 1),
    AMIGA_KEY_6 = (0x06 << 1),
    AMIGA_KEY_7 = (0x07 << 1),
    AMIGA_KEY_8 = (0x08 << 1),
    AMIGA_KEY_9 = (0x09 << 1),
    AMIGA_KEY_0 = (0x0a << 1),
    AMIGA_KEY_MINUS = (0x0b << 1),
    AMIGA_KEY_EQUAL = (0x0c << 1),
    AMIGA_KEY_BACKSLASH = (0x0d << 1),

    AMIGA_KEY_KP_0 = (0x0f << 1),

    AMIGA_KEY_Q = (0x10 << 1),
    AMIGA_KEY_W = (0x11 << 1),
    AMIGA_KEY_E = (0x12 << 1),
    AMIGA_KEY_R = (0x13 << 1),
    AMIGA_KEY_T = (0x14 << 1),
    AMIGA_KEY_Y = (0x15 << 1),
    AMIGA_KEY_U = (0x16 << 1),
    AMIGA_KEY_I = (0x17 << 1),
    AMIGA_KEY_O = (0x18 << 1),
    AMIGA_KEY_P = (0x19 << 1),
    AMIGA_KEY_LEFT_BRACKET = (0x1a << 1),
    AMIGA_KEY_RIGHT_BRACKET = (0x1b << 1),

    AMIGA_KEY_KP_1 = (0x1d << 1),
    AMIGA_KEY_KP_2 = (0x1e << 1),
    AMIGA_KEY_KP_3 = (0x1f << 1),

    AMIGA_KEY_A = (0x20 << 1),
    AMIGA_KEY_S = (0x21 << 1),
    AMIGA_KEY_D = (0x22 << 1),
    AMIGA_KEY_F = (0x23 << 1),
    AMIGA_KEY_G = (0x24 << 1),
    AMIGA_KEY_H = (0x25 << 1),
    AMIGA_KEY_J = (0x26 << 1),
    AMIGA_KEY_K = (0x27 << 1),
    AMIGA_KEY_L = (0x28 << 1),
    AMIGA_KEY_SEMICOLON = (0x29 << 1),
    AMIGA_KEY_APOSTROPHE = (0x2a << 1),
    AMIGA_KEY_2B = (0x2b << 1),         // key between ' and return (mapped to Insert)

    AMIGA_KEY_KP_4 = (0x2d << 1),
    AMIGA_KEY_KP_5 = (0x2e << 1),
    AMIGA_KEY_KP_6 = (0x2f << 1),

    AMIGA_KEY_LESS = (0x30 << 1),       // key between left-shift and z (mapped to 102nd key)
    AMIGA_KEY_Z = (0x31 << 1),
    AMIGA_KEY_X = (0x32 << 1),
    AMIGA_KEY_C = (0x33 << 1),
    AMIGA_KEY_V = (0x34 << 1),
    AMIGA_KEY_B = (0x35 << 1),
    AMIGA_KEY_N = (0x36 << 1),
    AMIGA_KEY_M = (0x37 << 1),
    AMIGA_KEY_COMMA = (0x38 << 1),
    AMIGA_KEY_PERIOD = (0x39 << 1),
    AMIGA_KEY_FORWARD_SLASH = (0x3a << 1),

    AMIGA_KEY_KP_PERIOD = (0x3c << 1),
    AMIGA_KEY_KP_7 = (0x3d << 1),
    AMIGA_KEY_KP_8 = (0x3e << 1),
    AMIGA_KEY_KP_9 = (0x3f << 1),

    AMIGA_KEY_SPACEBAR = (0x40 << 1),
    AMIGA_KEY_BACKSPACE = (0x41 << 1),
    AMIGA_KEY_TAB = (0x42 << 1),
    AMIGA_KEY_KP_ENTER = (0x43 << 1),
    AMIGA_KEY_RETURN = (0x44 << 1),
    AMIGA_KEY_ESC = (0x45 << 1),
    AMIGA_KEY_DELETE = (0x46 << 1),

    AMIGA_KEY_KP_SUBTRACT = (0x4a << 1),

    AMIGA_KEY_UP = (0x4c << 1),
    AMIGA_KEY_DOWN = (0x4d << 1),
    AMIGA_KEY_RIGHT = (0x4e << 1),
    AMIGA_KEY_LEFT = (0x4f << 1),

    AMIGA_KEY_F1 = (0x50 << 1),
    AMIGA_KEY_F2 = (0x51 << 1),
    AMIGA_KEY_F3 = (0x52 << 1),
    AMIGA_KEY_F4 = (0x53 << 1),
    AMIGA_KEY_F5 = (0x54 << 1),
    AMIGA_KEY_F6 = (0x55 << 1),
    AMIGA_KEY_F7 = (0x56 << 1),
    AMIGA_KEY_F8 = (0x57 << 1),
    AMIGA_KEY_F9 = (0x58 << 1),
    AMIGA_KEY_F10 = (0x59 << 1),

    AMIGA_KEY_KP_LEFT_PAREN = (0x5a << 1),      // mapped to Home
    AMIGA_KEY_KP_RIGHT_PAREN = (0x5b << 1),     // mapped to Page Up
    AMIGA_KEY_KP_DIVIDE = (0x5c << 1),
    AMIGA_KEY_KP_MULTIPLY = (0x5d << 1),
    AMIGA_KEY_KP_PLUS = (0x5e << 1),

    AMIGA_KEY_HELP = (0x5f << 1),               // mapped to End (usually right of Delete)

    AMIGA_KEY_LEFT_SHIFT = (0x60 << 1),
    AMIGA_KEY_RIGHT_SHIFT = (0x61 << 1),
    AMIGA_KEY_CAPS_LOCK = (0x62 << 1),
    AMIGA_KEY_CONTROL = (0x63 << 1),
    AMIGA_KEY_LEFT_ALT = (0x64 << 1),
    AMIGA_KEY_RIGHT_ALT = (0x65 << 1),
    AMIGA_KEY_LEFT_AMIGA = (0x66 << 1),         // mapped to Left Meta
    AMIGA_KEY_RIGHT_AMIGA = (0x67 << 1),        // mapped to Right Meta, Right Ctrl, or Menu
};

/* The Amiga keyboard doesn't have every key imaginable */
#define NO_KEY 0xff

/* XXX: remove when I reference this from my code. */
#if 0

static int qcode_to_amiga_keycode[] = {
    /* Make sure future additions are automatically set to NO_KEY */
    [0 ... 0xff]               = NO_KEY,

    [Q_KEY_CODE_SHIFT]         = AMIGA_KEY_LEFT_SHIFT,
    [Q_KEY_CODE_SHIFT_R]       = AMIGA_KEY_RIGHT_SHIFT,
    [Q_KEY_CODE_ALT]           = AMIGA_KEY_LEFT_ALT,
    [Q_KEY_CODE_ALT_R]         = AMIGA_KEY_RIGHT_ALT,
    [Q_KEY_CODE_CTRL]          = AMIGA_KEY_CONTROL,
    [Q_KEY_CODE_CTRL_R]        = AMIGA_KEY_RIGHT_AMIGA,
    [Q_KEY_CODE_META_L]        = AMIGA_KEY_LEFT_AMIGA,
    [Q_KEY_CODE_META_R]        = AMIGA_KEY_RIGHT_AMIGA,
    [Q_KEY_CODE_MENU]          = AMIGA_KEY_RIGHT_AMIGA,
    [Q_KEY_CODE_SPC]           = AMIGA_KEY_SPACEBAR,

    [Q_KEY_CODE_ESC]           = AMIGA_KEY_ESC,
    [Q_KEY_CODE_1]             = AMIGA_KEY_1,
    [Q_KEY_CODE_2]             = AMIGA_KEY_2,
    [Q_KEY_CODE_3]             = AMIGA_KEY_3,
    [Q_KEY_CODE_4]             = AMIGA_KEY_4,
    [Q_KEY_CODE_5]             = AMIGA_KEY_5,
    [Q_KEY_CODE_6]             = AMIGA_KEY_6,
    [Q_KEY_CODE_7]             = AMIGA_KEY_7,
    [Q_KEY_CODE_8]             = AMIGA_KEY_8,
    [Q_KEY_CODE_9]             = AMIGA_KEY_9,
    [Q_KEY_CODE_0]             = AMIGA_KEY_0,
    [Q_KEY_CODE_MINUS]         = AMIGA_KEY_MINUS,
    [Q_KEY_CODE_EQUAL]         = AMIGA_KEY_EQUAL,
    [Q_KEY_CODE_BACKSPACE]     = AMIGA_KEY_DELETE,
    [Q_KEY_CODE_TAB]           = AMIGA_KEY_TAB,
    [Q_KEY_CODE_Q]             = AMIGA_KEY_Q,
    [Q_KEY_CODE_W]             = AMIGA_KEY_W,
    [Q_KEY_CODE_E]             = AMIGA_KEY_E,
    [Q_KEY_CODE_R]             = AMIGA_KEY_R,
    [Q_KEY_CODE_T]             = AMIGA_KEY_T,
    [Q_KEY_CODE_Y]             = AMIGA_KEY_Y,
    [Q_KEY_CODE_U]             = AMIGA_KEY_U,
    [Q_KEY_CODE_I]             = AMIGA_KEY_I,
    [Q_KEY_CODE_O]             = AMIGA_KEY_O,
    [Q_KEY_CODE_P]             = AMIGA_KEY_P,
    [Q_KEY_CODE_BRACKET_LEFT]  = AMIGA_KEY_LEFT_BRACKET,
    [Q_KEY_CODE_BRACKET_RIGHT] = AMIGA_KEY_RIGHT_BRACKET,
    [Q_KEY_CODE_RET]           = AMIGA_KEY_RETURN,
    [Q_KEY_CODE_A]             = AMIGA_KEY_A,
    [Q_KEY_CODE_S]             = AMIGA_KEY_S,
    [Q_KEY_CODE_D]             = AMIGA_KEY_D,
    [Q_KEY_CODE_F]             = AMIGA_KEY_F,
    [Q_KEY_CODE_G]             = AMIGA_KEY_G,
    [Q_KEY_CODE_H]             = AMIGA_KEY_H,
    [Q_KEY_CODE_J]             = AMIGA_KEY_J,
    [Q_KEY_CODE_K]             = AMIGA_KEY_K,
    [Q_KEY_CODE_L]             = AMIGA_KEY_L,
    [Q_KEY_CODE_SEMICOLON]     = AMIGA_KEY_SEMICOLON,
    [Q_KEY_CODE_APOSTROPHE]    = AMIGA_KEY_APOSTROPHE,
    [Q_KEY_CODE_GRAVE_ACCENT]  = AMIGA_KEY_GRAVE_ACCENT,
    [Q_KEY_CODE_BACKSLASH]     = AMIGA_KEY_BACKSLASH,
    [Q_KEY_CODE_Z]             = AMIGA_KEY_Z,
    [Q_KEY_CODE_X]             = AMIGA_KEY_X,
    [Q_KEY_CODE_C]             = AMIGA_KEY_C,
    [Q_KEY_CODE_V]             = AMIGA_KEY_V,
    [Q_KEY_CODE_B]             = AMIGA_KEY_B,
    [Q_KEY_CODE_N]             = AMIGA_KEY_N,
    [Q_KEY_CODE_M]             = AMIGA_KEY_M,
    [Q_KEY_CODE_COMMA]         = AMIGA_KEY_COMMA,
    [Q_KEY_CODE_DOT]           = AMIGA_KEY_PERIOD,
    [Q_KEY_CODE_SLASH]         = AMIGA_KEY_FORWARD_SLASH,
    [Q_KEY_CODE_ASTERISK]      = AMIGA_KEY_KP_MULTIPLY,
    [Q_KEY_CODE_CAPS_LOCK]     = AMIGA_KEY_CAPS_LOCK,

    [Q_KEY_CODE_F1]            = AMIGA_KEY_F1,
    [Q_KEY_CODE_F2]            = AMIGA_KEY_F2,
    [Q_KEY_CODE_F3]            = AMIGA_KEY_F3,
    [Q_KEY_CODE_F4]            = AMIGA_KEY_F4,
    [Q_KEY_CODE_F5]            = AMIGA_KEY_F5,
    [Q_KEY_CODE_F6]            = AMIGA_KEY_F6,
    [Q_KEY_CODE_F7]            = AMIGA_KEY_F7,
    [Q_KEY_CODE_F8]            = AMIGA_KEY_F8,
    [Q_KEY_CODE_F9]            = AMIGA_KEY_F9,
    [Q_KEY_CODE_F10]           = AMIGA_KEY_F10,

    [Q_KEY_CODE_KP_DIVIDE]     = AMIGA_KEY_KP_DIVIDE,
    [Q_KEY_CODE_KP_MULTIPLY]   = AMIGA_KEY_KP_MULTIPLY,
    [Q_KEY_CODE_KP_SUBTRACT]   = AMIGA_KEY_KP_SUBTRACT,
    [Q_KEY_CODE_KP_ADD]        = AMIGA_KEY_KP_PLUS,
    [Q_KEY_CODE_KP_ENTER]      = AMIGA_KEY_KP_ENTER,
    [Q_KEY_CODE_KP_DECIMAL]    = AMIGA_KEY_KP_PERIOD,
    [Q_KEY_CODE_KP_0]          = AMIGA_KEY_KP_0,
    [Q_KEY_CODE_KP_1]          = AMIGA_KEY_KP_1,
    [Q_KEY_CODE_KP_2]          = AMIGA_KEY_KP_2,
    [Q_KEY_CODE_KP_3]          = AMIGA_KEY_KP_3,
    [Q_KEY_CODE_KP_4]          = AMIGA_KEY_KP_4,
    [Q_KEY_CODE_KP_5]          = AMIGA_KEY_KP_5,
    [Q_KEY_CODE_KP_6]          = AMIGA_KEY_KP_6,
    [Q_KEY_CODE_KP_7]          = AMIGA_KEY_KP_7,
    [Q_KEY_CODE_KP_8]          = AMIGA_KEY_KP_8,
    [Q_KEY_CODE_KP_9]          = AMIGA_KEY_KP_9,

    [Q_KEY_CODE_UP]            = AMIGA_KEY_UP,
    [Q_KEY_CODE_DOWN]          = AMIGA_KEY_DOWN,
    [Q_KEY_CODE_LEFT]          = AMIGA_KEY_LEFT,
    [Q_KEY_CODE_RIGHT]         = AMIGA_KEY_RIGHT,

    [Q_KEY_CODE_HELP]          = AMIGA_KEY_HELP,
    [Q_KEY_CODE_END]           = AMIGA_KEY_HELP,            // End is usually right of Delete

    [Q_KEY_CODE_DELETE]        = AMIGA_KEY_DELETE,

    [Q_KEY_CODE_HOME]          = AMIGA_KEY_KP_LEFT_PAREN,
    [Q_KEY_CODE_PGUP]          = AMIGA_KEY_KP_RIGHT_PAREN,

    [Q_KEY_CODE_PGDN]          = AMIGA_KEY_2B,              // non-US Amiga key left of Return
    [Q_KEY_CODE_INSERT]        = AMIGA_KEY_LESS,            // non-US Amiga key between Left Shift and z
    [Q_KEY_CODE_LESS]          = AMIGA_KEY_LESS,            // also mapped to 102nd key on non-US keyboards
};
#endif

#endif /* AMIGA_KEYS_H */
