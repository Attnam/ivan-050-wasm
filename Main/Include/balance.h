/*
 *
 *  Iter Vehemens ad Necem (IVAN)
 *  Copyright (C) Timo Kiviluoto
 *  Released under the GNU General
 *  Public License
 *
 *  See LICENSING which should included
 *  with this file for more details
 *
 */

#ifndef __BALANCE_H__
#define __BALANCE_H__

/*
 * Global balance defines for the project IVAN.
 * This file is created to hold macros that affect the game balance
 */

#define GLOBAL_WEAK_BODYPART_HIT_MODIFIER 10.

#define EDIT_ATTRIBUTE_DAY_MIN 10 // last day there is no monster attribute plus
#define EDIT_ATTRIBUTE_DAY_SHIFT 1 // attribute plus = floor((day - min) / 2^shift)

#define DANGER_PLUS_DAY_MIN 5 // last day monster danger plus is zero
#define DANGER_PLUS_MULTIPLIER .001 // increases danger plus

#define KAMIKAZE_INVISIBILITY_DAY_MIN 10 // last day no dwarf can be generated invisible
#define KAMIKAZE_INVISIBILITY_DAY_MAX 30 // after this, all dwarves are invisible

#endif
