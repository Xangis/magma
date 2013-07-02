/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Envy Diku Mud improvements copyright (C) 1994 by Michael Quan, David   *
 *  Love, Guilherme 'Willie' Arnold, and Mitchell Tse.                     *
 *                                                                         *
 *  EnvyMud 2.0 improvements copyright (C) 1995 by Michael Quan and        *
 *  Mitchell Tse.                                                          *
 *                                                                         *
 *  EnvyMud 2.2 improvements copyright (C) 1996, 1997 by Michael Quan.     *
 *                                                                         *
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/*
 * Necro-pertinent tables/structs/utilities
 */
#define UNDEAD_SKELETON          0
#define UNDEAD_ZOMBIE            1
#define UNDEAD_SPECTRE           2
#define UNDEAD_WRAITH            3
#define UNDEAD_VAMPIRE           4
#define UNDEAD_LICH              5
#define UNDEAD_RED_DRACO         6
#define UNDEAD_BLUE_DRACO        7
#define UNDEAD_BLACK_DRACO       8
#define UNDEAD_GREEN_DRACO       9
#define UNDEAD_WHITE_DRACO      10
#define UNDEAD_MAX              11 /* max UNDEAD_ entries in table below */

struct undead_type  /* Define the table */
{
    char        *name;
    int         vnumber;
    int         min_level;
    int         max_level;
    int         slots;
};

const struct undead_type undead_table [UNDEAD_MAX] =
{
    { "skeleton",  MOB_VNUM_SKELETON,    5, 15,  1 },
    { "zombie",    MOB_VNUM_ZOMBIE,      5, 20,  1 },
    { "spectre",   MOB_VNUM_SPECTRE,     20, 41,  3 },
    { "wraith",    MOB_VNUM_WRAITH,      30, 40,  5 },
    { "vampire",   MOB_VNUM_VAMPIRE,     35, 41,  8 },
    { "lich",      MOB_VNUM_LICH,        41, 50, 15 },
    { "dracolich", MOB_VNUM_RED_DRACO,   41, 53, 26 },
    { "dracolich", MOB_VNUM_BLUE_DRACO,  41, 53, 26 },
    { "dracolich", MOB_VNUM_BLACK_DRACO, 41, 53, 26 },
    { "dracolich", MOB_VNUM_GREEN_DRACO, 41, 53, 26 },
    { "dracolich", MOB_VNUM_WHITE_DRACO, 41, 53, 26 }

};
