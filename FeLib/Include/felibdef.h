#ifndef __FELIBDEF_H__
#define __FELIBDEF_H__

/*
 * Global defines for the project FeLib.
 * This file is created to decrease the need of including headers in other headers just for the sake of some silly macros,
 * because it decreases compilation efficiency and may even cause cross-including
 *
 * List of macros that should be gathered here:
 * 1. all numeric defines used in multiple .cpp or .h files
 * 2. all inline functions used in multiple .cpp or .h files and independent enough (do not require other headers)
 * 3. class construction macros used in multiple .h files
 *
 * DO NOT INCLUDE ANY FILES IN THIS HEADER.
 */

#define FPI 3.1415926535897932384626433832795f

inline ushort GetRed16(ushort Color) { return Color >> 8 & 0xF8; }
inline ushort GetGreen16(ushort Color) { return Color >> 3 & 0xFC; }
inline ushort GetBlue16(ushort Color) { return Color << 3 & 0xF8; }
inline ushort MakeRGB16(ushort Red, ushort Green, ushort Blue) { return (Red << 8 & 0xF800) | (Green << 3 & 0x7E0) | (Blue >> 3 & 0x1F); }
inline ushort RightShift8AndMakeRGB16(ushort Red, ushort Green, ushort Blue) { return (Red & 0xF800) | (Green >> 5 & 0x7E0) | (Blue >> 11); }
inline ushort MakeShadeColor(ushort Color) { return MakeRGB16(GetRed16(Color) / 3, GetGreen16(Color) / 3, GetBlue16(Color) / 3); }

inline ulong GetRed24(ulong Color) { return Color >> 16 & 0xFF; }
inline ulong GetGreen24(ulong Color) { return Color >> 8 & 0xFF; }
inline ulong GetBlue24(ulong Color) { return Color & 0xFF; }
inline ulong MakeRGB24(ulong Red, ulong Green, ulong Blue) { return (Red << 16 & 0xFF0000) | (Green << 8 & 0xFF00) | (Blue & 0xFF); }

#define NONE 0
#define MIRROR 1
#define FLIP 2
#define ROTATE 4

#define TRANSPARENT_COLOR 0xF81F

#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F

#define YELLOW 0xFFE0
#define PINK 0xF01E

#define WHITE 0xFFFF
#define LIGHT_GRAY 0xB5B6
#define DARK_GRAY 0x528A
#define BLACK 0x0000

#define KEY_BACK_SPACE 0x08
#define KEY_ESC 0x1B
#define KEY_ENTER 0x0D
#define KEY_UP 0x148
#define KEY_DOWN 0x150
#define KEY_RIGHT 0x14D
#define KEY_LEFT 0x14B
#define KEY_HOME 0x147
#define KEY_END 0x14F
#define KEY_PAGE_DOWN 0x151
#define KEY_PAGE_UP 0x149
#define KEY_SPACE ' '
#define KEY_NUMPAD_5 2

#define NONE 0
#define MIRROR 1
#define FLIP 2
#define ROTATE_90 4

#define NOFLAME 0xFFFF

#define ERROR_VECTOR vector2d(-0x8000, -0x8000)

#ifdef GCC
#define NO_ALIGNMENT __attribute__ ((packed))
#else
#define NO_ALIGNMENT
#endif

#define SELECTABLE 1
#define INVERSE_MODE 2
#define BLIT_AFTERWARDS 4
#define DRAW_BACKGROUND_AFTERWARDS 8
#define FADE 16

#define FELIST_ERROR_BIT 0x8000
#define LIST_WAS_EMPTY 0xFFFF
#define ESCAPED 0xFFFE
#define NOTHING_SELECTED 0xFFFD

#define NO_LIMIT 0xFFFF

/* This multiplier was obtained from Knuth, D.E., "The Art of
   Computer Programming," Vol 2, Seminumerical Algorithms, Third
   Edition, Addison-Wesley, 1998, p. 106 (line 26) & p. 108 */

#ifdef GCC
#define RAND_MULTIPLIER 6364136223846793005ULL
#endif

#ifdef VC
#define RAND_MULTIPLIER 6364136223846793005
#endif

#define MAX_CONTROLS 0x10

#endif
