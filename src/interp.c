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

#if defined( macintosh )
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "merc.h"



bool	check_social	args( ( CHAR_DATA *ch, char *command,
			       char *argument ) );

/*
 * Command logging types.
 */
#define LOG_NORMAL              0
#define LOG_ALWAYS              1
#define LOG_NEVER               2

/*
 * Last command buffer, used for crashes
 */
char last_command[MAX_INPUT_LENGTH] = "\0";
int  last_command_room = 0;

/*
 * Log-all switch.
 */
bool				fLogAll		    = FALSE;



/*
 * Command table.
 *
 * Veygoth - Added three more variables at the end, snapvis, unhide, and breakmed,
 * which determine whether a person will lose invis, lose hide, or stop meditating
 * when using the command.
 */
const	struct	cmd_type	cmd_table	[ ] =
{
    /*
     * Common movement commands.
     */
    { "east",		do_east,	POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "north",          do_north,       POS_STANDING,    0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "south",		do_south,	POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "west",		do_west,	POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "up",		do_up,		POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "down",		do_down,	POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "northwest",	do_northwest,	POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "nw",		do_northwest,	POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "northeast",	do_northeast,	POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "ne",		do_northeast,	POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "southeast",	do_southeast,	POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "se",		do_southeast,	POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "southwest",	do_southwest,	POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "sw",		do_southwest,	POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "fly",            do_fly,         POS_STANDING,    0,  LOG_NORMAL, 1, 0, 1, 1 },

    /*
     * Common other commands.
     * Placed here so one and two letter abbreviations work.
     */
    { "at",		do_at,		POS_DEAD,    L_APP,  LOG_NORMAL, 1, 0, 0, 1 },
    { "buy",		do_buy,		POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "cast",		do_cast,	POS_FIGHTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "exits",		do_exits,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "get",		do_get,		POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "inventory",	do_inventory,	POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "kill",		do_kill,	POS_FIGHTING,	 0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "look",		do_look,	POS_RECLINING,	 0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "order",		do_order,	POS_RESTING,	 0,  LOG_ALWAYS, 1, 0, 1, 1 },
    { "quaff",		do_quaff,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "shoot",		do_shoot,	POS_STANDING,	 0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "search",		do_search,	POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "sing",           do_sing,        POS_STANDING,    0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "play",           do_play,        POS_STANDING,    0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "tell",		do_tell,	POS_RECLINING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "wield",		do_wield,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "will",           do_will,        POS_FIGHTING,    0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "wiz",      	do_immtalk,	POS_DEAD,    L_AVA,  LOG_NORMAL, 1, 0, 0, 1 },
    { "wizhelp",	do_wizhelp,	POS_DEAD,    L_HER,  LOG_NORMAL, 1, 0, 0, 1 },
    { "killproc",	do_killproc,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 1 },

    /*
     * Position commands.
     */
    { "kneel",          do_kneel,       POS_SLEEPING,    0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "rest",		do_rest,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "recline",        do_recline,     POS_SLEEPING,    0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "sit",		do_sit,		POS_SLEEPING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "sleep",		do_sleep,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "stand",		do_stand,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "wake",		do_wake,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },

    /*
     * Informational commands.
     *
     * Most informational commands should not break invis or hide.
     *
     * These are also about the only commands that should not break meditate.
     */
    { "areas",		do_areas,	POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "attributes",	do_attributes,	POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "bug",		do_bug,		POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "commands",	do_commands,	POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "compare",	do_compare,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "consider",	do_consider,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "consent",        do_consent,     POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "crashbug",	do_crashbug,	POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "credits",	do_credits,	POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "equipment",	do_equipment,	POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "examine",	do_examine,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "experience",     do_exp,         POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "fraglist",       do_fraglist,    POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "glance",         do_glance,      POS_RECLINING,   0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "help",		do_help,	POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "idea",		do_idea,	POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "ignore",         do_ignore,      POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "innate",         do_innate,      POS_DEAD,        0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "invoke",         do_invoke,      POS_RESTING,     0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "justice",        do_justice,     POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "news",           do_news,        POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "report",		do_report,	POS_RECLINING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "pagelength",     do_pagelen,     POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "read",		do_look,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "score",		do_score,	POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "society",        do_society,     POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "socials",	do_socials,	POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "summon",         do_summon_mount,POS_STANDING,    0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "scan",		do_scan,	POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "slist",          do_slist,       POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "skills",         do_slist,       POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "songs",          do_songs,       POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "spells",         do_spells,      POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "speak",          do_speak,       POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "time",		do_time,	POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "trophy",         do_trophy,      POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "typo",		do_typo,	POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "who",		do_who,		POS_RECLINING,	 0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "wizlist",        do_wizlist,     POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "zones",		do_areas,	POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 0 },
    /*
     * Combat commands.
     *
     * Most of these should break invis or hide
     */
    { "assist",         do_assist,      POS_FIGHTING,    0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "backstab",	do_backstab,	POS_STANDING,	 0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "bash",           do_bash,        POS_FIGHTING,    0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "bs",		do_backstab,	POS_STANDING,	 0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "berserk",	do_berserk,	POS_FIGHTING,	 0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "bodyslam",       do_bodyslam,    POS_STANDING,    0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "charge",         do_charge,      POS_STANDING,    0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "circle",		do_circle,	POS_FIGHTING,	 0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "disarm",		do_disarm,	POS_FIGHTING,	 0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "disengage",      do_disengage,   POS_FIGHTING,    0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "guard",          do_guard,       POS_FIGHTING,    0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "flee",		do_flee,	POS_RECLINING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "headbutt",       do_headbutt,    POS_FIGHTING,    0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "kick",		do_kick,	POS_FIGHTING,	 0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "dirt",		do_dirt,	POS_FIGHTING,	 0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "rescue",		do_rescue,	POS_FIGHTING,	 0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "springleap",     do_springleap,  POS_RESTING,     0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "stance",         do_stance,      POS_FIGHTING,    0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "throw",          do_throw,       POS_FIGHTING,    0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "trip",           do_trip,        POS_FIGHTING,    0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "whirlwind",	do_whirlwind,	POS_STANDING,	 0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "chill",		do_chill,	POS_FIGHTING, L_IMM, LOG_NORMAL, 1, 1, 1, 1 },

    /*
     * Configuration commands.
     *
     * Most of these should not break invis or hide, or meditate
     */
    { "auto",           do_auto,        POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "autoexit",       do_autoexit,    POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "blank",          do_blank,       POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "brief",          do_brief,       POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "cant",		do_cant,	POS_RECLINING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "channels",	do_channels,	POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "color",          do_color,       POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "combine",        do_combine,     POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "compact",        do_blank,       POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "config",		do_config,	POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "toggle",		do_config,	POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "description",	do_description,	POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "prompt",         do_prompt,      POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "display",        do_prompt,     	POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "title",		do_title,	POS_DEAD,    L_HER,  LOG_NORMAL, 1, 0, 0, 1 },
    { "wimpy",		do_wimpy,	POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 1 },
/* Making this IMM only until someone has time to verify that it's not buggy. - Lohrr */
    { "history",	do_history,	POS_DEAD,    L_IMM,  LOG_NORMAL, 1, 0, 0, 0 },

    /*
     * Communication commands.
     *
     * Most of these should beak hide but not invis
     */
    { "ask",            do_ask,         POS_SLEEPING,    0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "beep",		do_beep,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "emote",		do_emote,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { ":",		do_emote,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "gsay",		do_gtell,	POS_RECLINING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { ";",		do_gtell,	POS_RECLINING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "ptell",          do_ptell,       POS_DEAD,    L_AVA,  LOG_ALWAYS, 1, 0, 0, 1 },
    { "reply",		do_reply,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "say",		do_say,		POS_RECLINING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "random",		do_random,	POS_DEAD,    L_AVA,  LOG_NORMAL, 1, 0, 0, 1 },
    { "sign",		do_sign,	POS_RECLINING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "'",		do_say,		POS_RECLINING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "shout",		do_shout,	POS_RESTING,	 1,  LOG_NORMAL, 1, 0, 1, 1 },
    { "yell",		do_yell,	POS_RESTING,	 1,  LOG_NORMAL, 1, 0, 1, 1 },

    /*
     * Object manipulation commands.
     *
     * Most of these should break hide but not invis
     */
    { "auction",        do_auction,     POS_SITTING,     0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "brandish",	do_brandish,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "carve",          do_carve,       POS_STANDING,    0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "climb",		do_climb,	POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "close",		do_close,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "deposit",        do_deposit,     POS_RESTING,     0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "drag",           do_drag,        POS_STANDING,    0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "drink",		do_drink,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "drop",		do_drop,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "eat",		do_eat,		POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "fill",		do_fill,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "pour",		do_pour,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "give",		do_give,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "grab",		do_hold,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "hold",		do_hold,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "list",		do_list,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "lock",		do_lock,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "open",		do_open,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "pick",		do_pick,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "put",		do_put,		POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "quaff",		do_quaff,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "recite",		do_recite,	POS_RESTING,	 0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "remove",		do_remove,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "reload",		do_reload,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "sell",		do_sell,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "take",		do_get,		POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "unlock",		do_unlock,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "use",            do_use,         POS_RESTING,     0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "value",		do_value,	POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "wear",		do_wear,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "weather",	do_weather,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "withdraw",       do_withdraw,    POS_RESTING,     0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "zap",		do_zap,		POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "brew",		do_brew,	POS_STANDING, L_AVA, LOG_NORMAL, 1, 0, 1, 1 },
    { "scribe",		do_scribe,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },

    /*
     * Miscellaneous commands.
     */
    { "afk",            do_afk,         POS_SLEEPING,    0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "aggressive",     do_aggressive,  POS_SLEEPING,    0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "aware",          do_aware,       POS_STANDING,    0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "bandage",        do_bandage,     POS_STANDING,    0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "bet",            do_bet,         POS_STANDING,    0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "camp",		do_camp,	POS_SITTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "chameleon power",do_chameleon,   POS_STANDING,    0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "dice",           do_dice,        POS_RESTING,     0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "disembark",      do_disembark,   POS_STANDING,    0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "doorbash",       do_doorbash,    POS_STANDING,    0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "disband",        do_disband,     POS_RESTING,     0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "enter",		do_enter,	POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "firstaid",       do_firstaid,    POS_RESTING,     0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "follow",		do_follow,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "forage",         do_forage,      POS_STANDING,    0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "forget",         do_forget,      POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "group",		do_group,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "heighten senses",do_heighten,    POS_STANDING,    0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "hide",		do_hide,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "layhands",       do_layhands,    POS_FIGHTING,    0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "meditate",       do_meditate,    POS_RESTING,     0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "memorize",       do_memorize,    POS_RESTING,     0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "poison weapon",	do_poison_weapon,POS_SLEEPING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "pray",		do_pray,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 0 },
    { "practice",       do_practice,    POS_SLEEPING,    0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "qui",		do_qui,		POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "quit",		do_quit,	POS_DEAD,    L_AVA,  LOG_NORMAL, 1, 0, 0, 1 },
    { "rent",		do_rent,	POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "retir",		do_retir,	POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "save",		do_save,        POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "shadow form",    do_shadow,      POS_STANDING,    0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "shift",          do_shift,       POS_STANDING,    0,  LOG_NORMAL, 1, 0, 1, 0 },
    { "sleep",		do_sleep,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "capture",	do_capture,	POS_FIGHTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "sneak",		do_sneak,	POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "split",		do_split,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "steal",		do_steal,	POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "suicide",        do_suicide,     POS_DEAD,        0,  LOG_ALWAYS, 1, 0, 1, 1 },
    { "train",		do_train,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "unbind",		do_untangle,	POS_FIGHTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "untangle",	do_untangle,	POS_FIGHTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "ungroup",        do_ungroup,     POS_SLEEPING,    0,  LOG_NORMAL, 1, 0, 0, 1 },
    { "visible",	do_visible,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, 1, 1, 1 },
    { "wager",          do_bet,         POS_STANDING,    0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "wake",		do_wake,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "track",		do_track,	POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "mount",		do_mount,	POS_STANDING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },
    { "dismount",	do_dismount,	POS_RESTING,	 0,  LOG_NORMAL, 1, 0, 1, 1 },

    /*
     * Immortal commands.
     *
     * These should not cause you to go vis or lose hide
     */
    { "finger",         do_finger,      POS_DEAD,    L_AVA,  LOG_NORMAL, 1, 0, 0, 0 },
    { "fog",            do_fog,         POS_DEAD,    L_BUI,  LOG_NORMAL, 1, 0, 0, 0 },
    { "world",		do_mudconfig,	POS_DEAD,    L_DIR,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "advance",	do_advance,	POS_DEAD,    L_DIR,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "reset",          do_reset,       POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "trust",		do_trust,	POS_DEAD,    L_DIR,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "delet",		do_terminat,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "alist",          do_alist,       POS_DEAD,    L_AVA,  LOG_NORMAL, 1, 0, 0, 0 },
    { "allow",		do_allow,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "siteban",	do_ban,		POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "bitflag",	do_bitflag,	POS_DEAD,    L_AVA,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "crashmud",       do_crashmud,    POS_DEAD,    L_SEN,  LOG_NORMAL, 1, 0, 0, 0 },
    { "deny",		do_deny,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "disconnect",	do_disconnect,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "lookup",         do_find,        POS_DEAD,    L_APP,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "force",		do_force,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "freeze",		do_freeze,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "find",           do_find,        POS_DEAD,    L_APP,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "imtlset",	do_imtlset,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "language",	do_add_lang,	POS_FIGHTING, L_IMM, LOG_NORMAL, 1, 0, 0, 0 },
    { "load",           do_load,        POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "log",		do_log,		POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "loopmud",        do_loopmud,     POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "map",            do_map,         POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "noemote",	do_noemote,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "note",           do_note,        POS_DEAD,    L_AVA,  LOG_NORMAL, 1, 0, 0, 0 },
    { "numlock",	do_numlock,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "pardon",		do_pardon,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "purge",		do_purge,	POS_DEAD,    L_SEN,  LOG_NORMAL, 1, 0, 0, 0 },
    { "reboo",		do_reboo,	POS_DEAD,    L_SEN,  LOG_NORMAL, 1, 0, 0, 0 },
    { "reboot",		do_reboot,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "rename",		do_rename,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "namesucks",	do_namesucks,	POS_DEAD,    L_APP,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "restore",	do_restore,	POS_DEAD,    L_JUN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "setbit",         do_set,         POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "shutdow",	do_shutdow,	POS_DEAD,    L_SEN,  LOG_NORMAL, 1, 0, 0, 0 },
    { "shutdown",	do_shutdown,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "silence",	do_silence,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "sla",		do_sla,		POS_DEAD,    L_SEN,  LOG_NORMAL, 1, 0, 0, 0 },
    { "slay",		do_slay,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "sset",		do_sset,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "sstime",		do_sstime,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "stat",           do_stat,        POS_DEAD,    L_BUI,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "statdump",       do_statdump,    POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "terminate",	do_terminate,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "transfer",	do_transfer,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "users",		do_users,	POS_DEAD,    L_SEN,  LOG_NORMAL, 1, 0, 0, 0 },
    { "where",          do_where,       POS_DEAD,    L_AVA,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "wizify", 	do_wizify,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "wizlock",	do_wizlock,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "makeclan",	do_makeclan,	POS_DEAD,    L_SEN,  LOG_NORMAL, 1, 0, 0, 0 },
    { "killclan",       do_killclan,    POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "echo",		do_echo,	POS_DEAD,    L_APP,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "memory",		do_memory,	POS_DEAD,    L_JUN,  LOG_NORMAL, 1, 0, 0, 0 },
    { "peace",		do_peace,	POS_DEAD,    L_JUN,  LOG_NORMAL, 1, 0, 0, 0 },
    { "recho",		do_recho,	POS_DEAD,    L_JUN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "return",		do_return,	POS_DEAD,    L_JUN,  LOG_NORMAL, 1, 0, 0, 0 },
    { "sober",		do_sober,	POS_DEAD,    L_APP,  LOG_NORMAL, 1, 0, 0, 0 },
    { "snoop",		do_snoop,	POS_DEAD,    L_SEN,  LOG_NORMAL, 1, 0, 0, 0 },
    { "switch",		do_switch,	POS_DEAD,    L_JUN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "version",	do_version,	POS_DEAD,    L_AVA,  LOG_NORMAL, 1, 0, 0, 0 },
    { "wizinvis",	do_invis,	POS_DEAD,    L_JUN,  LOG_NORMAL, 1, 0, 0, 0 },
    { "bamfin",		do_bamfin,	POS_DEAD,    L_APP,  LOG_NORMAL, 1, 0, 0, 0 },
    { "bamfout",	do_bamfout,	POS_DEAD,    L_APP,  LOG_NORMAL, 1, 0, 0, 0 },
    { "goto",		do_goto,	POS_DEAD,    L_BUI,  LOG_NORMAL, 1, 0, 0, 0 },
    { "holylight",	do_holylight,	POS_DEAD,    L_AVA,  LOG_NORMAL, 1, 0, 0, 0 },
    { ":",		do_immtalk,	POS_DEAD,    L_AVA,  LOG_NORMAL, 1, 0, 0, 0 },
    { "wiznet",		do_wiznet,	POS_DEAD,    L_AVA,  LOG_NORMAL, 1, 0, 0, 0 },
    { "clone",		do_clone,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },
    { "zreset",		do_zreset,	POS_DEAD,    L_SEN,  LOG_ALWAYS, 1, 0, 0, 0 },

    /*
     * Clansman commands.
     */
    { "guilds",		do_clans,	POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "orders",		do_clans,	POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "clans",		do_clans,	POS_DEAD,        0,  LOG_NORMAL, 1, 0, 0, 0 },
    { "gcc",		do_wartalk,	POS_SLEEPING,	 0,  LOG_NORMAL, 1, 0, 0, 0 },
  
    /*
     * MOBprogram commands.
     */
    { "prgstat",        do_mpstat,      POS_DEAD,    L_JUN,  LOG_NORMAL, 1, 0, 0, 0 },
    { "prgasound",      do_prgasound,   POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0, 0 },
    { "prgjunk",        do_prgjunk,     POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0, 0 },
    { "prgecho",        do_prgecho,     POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0, 0 },
    { "prgechoat",      do_prgechoat,   POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0, 0 },
    { "prgechoaround",  do_prgechoaround,POS_DEAD,       0,  LOG_NORMAL, 0, 0, 0, 0 },
    { "prgkill",        do_prgkill     ,POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0, 0 },
    { "prgmload",       do_prgmload    ,POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0, 0 },
    { "prgoload",       do_prgoload    ,POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0, 0 },
    { "prgpurge",       do_prgpurge    ,POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0, 0 },
    { "prggoto",        do_prggoto     ,POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0, 0 },
    { "prgat",          do_prgat       ,POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0, 0 },
    { "prgtransfer",    do_prgtransfer ,POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0, 0 },
    { "prgforce",       do_prgforce    ,POS_DEAD,        0,  LOG_NORMAL, 0, 0, 0, 0 },

    /*
     * End of list.
     */
    { "",		0,		POS_DEAD,	 0,  LOG_NORMAL, 1, 0, 0, 0 }
};



/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void interpret( CHAR_DATA *ch, char *argument )
{
    char command [ MAX_INPUT_LENGTH ];
    OBJ_DATA *obj = NULL;
    ROOM_INDEX_DATA *room = NULL;
    char logline [ MAX_INPUT_LENGTH ];
    char buf[MAX_STRING_LENGTH];
    int  cmd;
    int  trust;
    bool found;

    last_fun( "interpret");
/*
    strcpy( last_command, argument );
    if (strlen( last_command ) + 5 + strlen( ch->name) < MAX_INPUT_LENGTH)
    {
        strcat( last_command, "' by " );
        strcat( last_command, ch->name );
    }
    if ( ch == NULL || ch->in_room == NULL )
        last_command_room = -1;
    else
        last_command_room = ch->in_room->vnum;
*/
    if ( ch )
        sprintf( last_command, "%s by %s in room %d", argument, ch->name, ch->in_room->vnum );
    else
        sprintf( last_command, "%s by (null)", argument );
    last_com( last_command );
    /*
     * Strip leading spaces.
     */
    while ( isspace( *argument ) )
	argument++;
    if ( argument[0] == '\0' )
	return;

    /*
     * Remove AFK
     */
    if ( !IS_NPC( ch ) )
        REMOVE_BIT( ch->act, PLR_AFK );

    /*
     * Implement freeze command.
     */
    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_FREEZE ) )
    {
	send_to_char( "You're totally frozen!\n\r", ch );
	return;
    }

    /*
     * Grab the command word.
     * Special parsing so ' can be a command,
     *   also no spaces needed after punctuation.
     */
    strcpy( logline, argument );
    if ( !isalpha( argument[0] ) && !isdigit( argument[0] ) )
    {
	command[0] = argument[0];
	command[1] = '\0';
	argument++;
	while ( isspace( *argument ) )
	    argument++;
    }
    else
    {
	argument = one_argument( argument, command );
    }

    /*
     * Look for an item with a teleport trigger in the room.
     * and check to see if the command is a teleport trigger
     * -- Veygoth
     */
    if( ch->in_room && ( obj = get_obj_here( ch, argument )))
    {
        if( obj->item_type == TYPE_TELEPORT )
        {
          if( check_command( command, obj->value[1] ) && obj->value[2] != 0)
          {
            if( obj->value[2] != -1 )
              obj->value[2]--;
            if( (room = get_room_index( obj->value[0] )))
            {
              act( "$n&n vanishes suddenly.", ch, NULL, NULL, TO_ROOM );
              sprintf( buf, "You %s $p&n.\n\r", command );
              act( buf, ch, obj, NULL, TO_CHAR );
              sprintf( buf, "%s activated keyword and was teleported by object.", ch->name);
              log_string( buf );
              char_from_room( ch );
              char_to_room( ch, room ); 
              do_look( ch, "auto" );
              act( "$n&n arrives suddenly.", ch, NULL, NULL, TO_ROOM );
            }
            else
            {
              send_to_char( "ERROR: The target room for this teleporter does not exist.\n\r", ch );
              bug( "Target room for object %d does not exist.", obj->pIndexData->vnum );
            }
            return;
          }
        }
        else if( obj->item_type == TYPE_SWITCH )
        {
          char cbuf[MAX_STRING_LENGTH];
          EXIT_DATA *pexit;
          sprintf( cbuf, "Checking %s against command no. %d for %s.", command, obj->value[0], obj->name);
          wiznet( 0, WIZ_LSPAM, 0, cbuf );
          if( check_command( command, obj->value[0] ))
          {
            send_to_char( "Click.\n\r", ch );
            if( !(room = get_room_index( obj->value[1] )))
            {
              bug( "Target room for switch object %d does not exist.", obj->pIndexData->vnum );
              return;
            }
            if ( !( pexit = room->exit[obj->value[2]] ) )
            {
              bug( "Target exit for switch object %d does not exist.", obj->pIndexData->vnum );
              return;
            }
            if( IS_SET( pexit->exit_info, EX_BLOCKED ))
              REMOVE_BIT( pexit->exit_info, EX_BLOCKED );
            return;
          }
        }
    }

    /*
     * Look for command in command table.
     */
    found = FALSE;
    trust = get_trust( ch );
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
	if ( command[0] == cmd_table[cmd].name[0]
	    && !str_prefix( command, cmd_table[cmd].name )
	    && ( cmd_table[cmd].level <= trust ) )

	{
    	    found = TRUE;
	    break;
	}
    }

    // Break meditate
    if( cmd_table[cmd].breakmed )
    {
      if( !IS_NPC( ch ) && IS_SET( ch->act, PLR_MEDITATING ))
      {
        REMOVE_BIT( ch->act, PLR_MEDITATING );
        send_to_char( "You stop meditating.\n\r", ch );
      }
    }

    // Break sneak, hide, and invis
    // Anything that will break hide OR invis will break concealment
    // This is DUMB!  Breaks invis w/backstab on a target that's not
    //   there: i.e. "backstab trolll"/"backstab humann" -> vis and no
    //   attack! - Lohrr  (Should be handled with make_vis function).
    if( cmd_table[cmd].snapvis )
    {
        if( is_affected( ch, AFF_INVISIBLE ))
        {
            remove_affects( ch, AFF_INVISIBLE );
            remove_affects( ch, AFF_HIDE );
            remove_affects( ch, AFF_MINOR_INVIS );
            REMOVE_AFF_BIT( ch, AFF_INVISIBLE );

            act( "$n&n snaps into visibility.", ch, NULL, NULL, TO_ROOM );
            send_to_char( "You snap into visibility.\n\r", ch);
        }
        else if( is_affected( ch, AFF_MINOR_INVIS ))
        {
            remove_affects( ch, AFF_INVISIBLE );
            remove_affects( ch, AFF_HIDE );
            remove_affects( ch, AFF_MINOR_INVIS );
            REMOVE_AFF_BIT( ch, AFF_INVISIBLE );

            send_to_char( "You appear.\n\r", ch );
        }
 
    }

    if( cmd_table[cmd].unhide )
    {
        if( is_affected( ch, AFF_MINOR_INVIS ))
        {
          send_to_char( "You appear.\n\r", ch );
        }
        // Is anything supposed to remove sneak?
//        affect_strip ( ch, gsn_sneak,  0, 0 );
        affect_strip ( ch, gsn_shadow, 0, 0 );
        affect_strip ( ch, gsn_hide, 0, 0 );
        affect_strip ( ch, 0, spl_concealment, 0 );
        REMOVE_AFF_BIT( ch, AFF_MINOR_INVIS );
//        REMOVE_AFF_BIT( ch, AFF_SNEAK    );
        REMOVE_AFF_BIT( ch, AFF_HIDE );
    }

    /*
     * Log and snoop.
     */
    if ( cmd_table[cmd].log == LOG_NEVER )
	strcpy( logline, "XXXXXXXX XXXXXXXX XXXXXXXX" );

    if ( ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_LOG ) )
	|| fLogAll
	|| cmd_table[cmd].log == LOG_ALWAYS )
    {
	sprintf( log_buf, "Log %s: %s", ch->name, logline );
	log_string( log_buf );
	wiznet( ch, WIZ_SECURE, get_trust( ch ), log_buf );
    }

    if ( ch->desc && ch->desc->snoop_by )
    {
	write_to_buffer( ch->desc->snoop_by, "% ",    2 );
	write_to_buffer( ch->desc->snoop_by, logline, 0 );
	write_to_buffer( ch->desc->snoop_by, "\n\r",  2 );
    }

    if ( !found )
    {
	/*
	 * Look for command in socials table.
	 */
	if ( !check_social( ch, command, argument ) )
	{
	    if ( !IS_NPC( ch ) && !str_prefix( command, "petition" ) )
	    {
		sprintf( log_buf, "Log %s: %s", ch->name, logline );
		log_string( log_buf );
		wiznet( ch, WIZ_SECURE, get_trust( ch ), log_buf );
		do_petition(ch, argument);
	    }
	    else
	    	send_to_char( "Huh?\n\r", ch );
	}
	return;
    }

    /*
     * Character not in position for command?
     */
    if ( ch->position < cmd_table[cmd].position )
    {
	switch( ch->position )
	{
	case POS_DEAD:
	    send_to_char( "Lie still; you are &+rDEAD&n!\n\r", ch );
	    break;

	case POS_MORTAL:
	case POS_INCAP:
	    send_to_char( "You are hurt far too bad for that.\n\r", ch );
	    break;

	case POS_STUNNED:
	    send_to_char( "You are too stunned to do that.\n\r",    ch );
	    break;

	case POS_SLEEPING:
	    send_to_char( "In your dreams, or what?\n\r",           ch );
	    break;

        case POS_RECLINING:
            send_to_char( "You can't do that while lying around.\n\r", ch );
            break;
 
        case POS_SITTING:
            send_to_char( "You can't do this sitting!\n\r", ch );
            break;

        case POS_KNEELING:
            send_to_char( "Get off your knees!\n\r", ch );
            break;

	case POS_RESTING:
	    send_to_char( "Nah... You feel too relaxed...\n\r",     ch );
	    break;

	case POS_FIGHTING:
	    send_to_char( "No way!  You are still fighting!\n\r",   ch );
	    break;

	}
        if( !IS_IMMORTAL( ch ) )
        {
            return;
        }
        else
            send_to_char( "You're not in the right position, but..\n\r", ch );
    }

    /*
     * Dispatch the command.
     */
    (*cmd_table[cmd].do_fun) ( ch, argument );

    tail_chain( );
    return;
}



/*
 * Cut & pasted from SMAUG (Zen)
 */
SOC_INDEX_DATA *find_social( char *command )
{
    SOC_INDEX_DATA *social;
    int             hash;

    last_fun( "find_social" );
    if ( command[0] < 'a' || command[0] > 'z' )
        hash = 0;
    else
        hash = (command[0] - 'a') + 1;

    for ( social = soc_index_hash[hash]; social; social = social->next )
        if ( !str_prefix( command, social->name ) )
            return social;

    return NULL;
}



bool check_social( CHAR_DATA *ch, char *command, char *argument )
{
    SOC_INDEX_DATA *soc;
    CHAR_DATA      *victim;
    char            arg [ MAX_INPUT_LENGTH ];

    last_fun( "check_social");
    if ( !( soc = find_social( command ) ) )
	return FALSE;

    if ( !IS_NPC( ch ) && IS_SET( ch->act, PLR_NO_EMOTE ) )
    {
	send_to_char( "You are anti-social!\n\r", ch );
	return TRUE;
    }

    // Added by Lohrr.
    if( !IS_NPC( ch ) && IS_SET( ch->act, PLR_MEDITATING ))
    {
        REMOVE_BIT( ch->act, PLR_MEDITATING );
        send_to_char( "You stop meditating.\n\r", ch );
    }

    // Remove hide and conceal -- Veygoth
    if( is_affected( ch, AFF_MINOR_INVIS ))
    {
       send_to_char( "You appear.\n\r", ch );
    }
    affect_strip ( ch, gsn_shadow, 0, 0 );
    affect_strip ( ch, gsn_hide, 0, 0 );
    affect_strip ( ch, 0, spl_concealment, 0 );
    REMOVE_AFF_BIT( ch, AFF_MINOR_INVIS );
    REMOVE_AFF_BIT( ch, AFF_HIDE );

    switch ( ch->position )
    {
    case POS_DEAD:
	send_to_char( "Lie still; you are DEAD.\n\r",             ch );
	return TRUE;

    case POS_INCAP:
    case POS_MORTAL:
	send_to_char( "You are hurt far too badly for that.\n\r", ch );
	return TRUE;

    case POS_STUNNED:
	send_to_char( "You are too stunned to do that.\n\r",      ch );
	return TRUE;

    case POS_SLEEPING:
	/*
	 * I just know this is the path to a 12" 'if' statement.  :(
	 * But two players asked for it already!  -- Furey
	 */
	if ( !str_cmp( soc->name, "snore" ) )
	    break;
	send_to_char( "In your dreams, or what?\n\r",             ch );
	return TRUE;

    }

    one_argument( argument, arg );
    victim = NULL;
    if ( arg[0] == '\0' )
    {
	act( soc->char_no_arg,   ch, NULL, victim, TO_CHAR    );
	act( soc->others_no_arg, ch, NULL, victim, TO_ROOM    );
    }
    else if ( !( victim = get_char_world( ch, arg ) )
              || ( race_table[ch->race].racewar_side != race_table[victim->race].racewar_side
		   && ch->in_room != victim->in_room ) )
    {
	send_to_char( "They aren't here.\n\r",                    ch );
    }
    else if ( victim == ch )
    {
	act( soc->char_auto,     ch, NULL, victim, TO_CHAR    );
	act( soc->others_auto,   ch, NULL, victim, TO_ROOM    );
    }
    else if ( !get_char_room( ch, arg ) && can_see( ch, victim )
		&& soc->char_found != NULL && soc->vict_found != NULL )
    {
        if ( !IS_IMMORTAL( ch ) ) {
            send_to_char("You don't see them here.\n\r", ch );
            return TRUE;
        }
	if ( !IS_NPC( victim ) )
        {
	    ROOM_INDEX_DATA *original;
	    char            *ldbase                      = "From far away, ";
	    char             ldmsg [ MAX_STRING_LENGTH ];

            if ( victim->pcdata->ignore == ch ) {
                send_to_char( "They are ignoring you.\n\r", ch);
                return FALSE;
            }
	    original = ch->in_room;
	    char_from_room( ch );
	    char_to_room( ch, victim->in_room );
            last_fun( "check_social");

	    strcpy( ldmsg, ldbase );
	    strcat( ldmsg, soc->char_found );
	    act( ldmsg,                       ch, NULL, victim, TO_CHAR    );

	    strcpy( ldmsg, ldbase );
	    strcat( ldmsg, soc->vict_found );
	    act( ldmsg,                       ch, NULL, victim, TO_VICT    );

	    char_from_room( ch );
	    char_to_room( ch, original );
            last_fun( "check_social");
	}
	else
	{
	    send_to_char( "They aren't here.\n\r",                ch );
	}
    }
    else
    {
	act( soc->char_found,    ch, NULL, victim, TO_CHAR    );
	act( soc->vict_found,    ch, NULL, victim, TO_VICT    );
	act( soc->others_found,  ch, NULL, victim, TO_NOTVICT );

        // If mobs are to respond to socials, it should be inserted here.
        // It has been removed -- Veygoth
    }

    return TRUE;
}



/*
 * Return true if an argument is completely numeric.
 */
bool is_number( char *arg )
{
    last_fun( "is_number");
    if ( *arg == '\0' )
	return FALSE;

    if ( *arg == '+' || *arg == '-' )
        arg++;

    for ( ; *arg != '\0'; arg++ )
    {
	if ( !isdigit( *arg ) )
	    return FALSE;
    }

    return TRUE;
}



/*
 * Given a string like 14.foo, return 14 and 'foo' =>
 * 14 = return value, argument unchanged, arg = 'foo'
 * default return int : 0.
 */
int number_argument( char *argument, char *arg )
{
    char *pdot;
    int   number;
    
    last_fun( "number_argument");
    for ( pdot = argument; *pdot != '\0'; pdot++ )
    {
	if ( *pdot == '.' )
	{
	    *pdot = '\0';
	    number = atoi( argument );
	    *pdot = '.';
	    strcpy( arg, pdot+1 );
	    return number;
	}
    }

    strcpy( arg, argument );
    return 1;
}



/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument( char *argument, char *arg_first )
{
    char cEnd;

    last_fun( "one_argument");
    while ( isspace( *argument ) )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
	cEnd = *argument++;

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
	*arg_first = LOWER( *argument );
	arg_first++;
	argument++;
    }
    *arg_first = '\0';

    while ( isspace( *argument ) )
	argument++;

    return argument;
}


/*
 * Zen's history code; see also read_from_buffer( ) in comm.c
 */
void do_history( CHAR_DATA *ch, char *argument )
{
    HISTORY_DATA *history;
    int           num;
    char          buf [ MAX_STRING_LENGTH ];

    last_fun( "do_history");
    if ( !ch->desc )
	return;

    for ( history = ch->desc->infirst, num = 0; history;
	 history = history->next, num-- )
    {
	sprintf( buf, " %4d: %s\n\r", abs(num), history->comm );
	send_to_char( buf, ch );
    }

    return;
}

/* 
 * strexg - copies a string, making some replacements
 * strexg( <to>, <from>, <what>, <for what> )
 */
void strexg( char *target, char *source, char c, char *t )
{
    int   i = 0;
    char *temp;

    last_fun( "stregx");
    while ( *source && i < MAX_INPUT_LENGTH )
    {
	if ( *source == c )
	{
	    if ( !t )
	    {
		source++;
		continue;
	    }
	    temp = t;
	    source++;
	    while ( *temp )
	    {
		i++;
		*target++ = *temp++;
	    }
	}
	else
	{
	    *target++ = *source++;
	    i++;
	}
    }
    *target = '\0';

    return;
}

// This function is used to compare an integer to the list
// of commands in DE to see if a teleport or switch
// trigger will work.
// -- Veygoth
bool check_command( char *argument, int number )
{
   switch( number )
   {
     default:
       bug( "Command %d is not a valid teleport/switch trigger command!\n\r", number );
     case 1:
       if( !str_cmp( argument, "north" )) return TRUE;
       return FALSE;
       break;
     case 2:
       if( !str_cmp( argument, "east" )) return TRUE;
       return FALSE;
       break;
     case 3:
       if( !str_cmp( argument, "south" )) return TRUE;
       return FALSE;
       break;
     case 4:
       if( !str_cmp( argument, "west" )) return TRUE;
       return FALSE;
       break;
     case 5:
       if( !str_cmp( argument, "up" )) return TRUE;
       return FALSE;
       break;
     case 6:
       if( !str_cmp( argument, "down" )) return TRUE;
       return FALSE;
       break;
     case 7:
       if( !str_cmp( argument, "enter" )) return TRUE;
       return FALSE;
       break;
     case 8:
       if( !str_cmp( argument, "turn" )) return TRUE;
       return FALSE;
       break;
     case 9:
       if( !str_cmp( argument, "kiss" )) return TRUE;
       return FALSE;
       break;
     case 10:
       if( !str_cmp( argument, "get" )) return TRUE;
       return FALSE;
       break;
     case 11:
       if( !str_cmp( argument, "drink" )) return TRUE;
       return FALSE;
       break;
     case 12:
       if( !str_cmp( argument, "eat" )) return TRUE;
       return FALSE;
       break;
     case 13:
       if( !str_cmp( argument, "wear" )) return TRUE;
       return FALSE;
       break;
     case 14:
       if( !str_cmp( argument, "wield" )) return TRUE;
       return FALSE;
       break;
     case 15:
       if( !str_cmp( argument, "look" )) return TRUE;
       return FALSE;
       break;
     case 17:
       if( !str_cmp( argument, "say" )) return TRUE;
       return FALSE;
       break;
     case 18:
       if( !str_cmp( argument, "shout" )) return TRUE;
       return FALSE;
       break;
     case 19:
       if( !str_cmp( argument, "tell" )) return TRUE;
       return FALSE;
       break;
     case 20:
       if( !str_cmp( argument, "climb" )) return TRUE;
       return FALSE;
       break;
     case 21:
       if( !str_cmp( argument, "swing" )) return TRUE;
       return FALSE;
       break;
     case 23:
       if( !str_cmp( argument, "smile" )) return TRUE;
       return FALSE;
       break;
     case 24:
       if( !str_cmp( argument, "dance" )) return TRUE;
       return FALSE;
       break;
     case 25:
       if( !str_cmp( argument, "kill" )) return TRUE;
       return FALSE;
       break;
     case 26:
       if( !str_cmp( argument, "cackle" )) return TRUE;
       return FALSE;
       break;
     case 27:
       if( !str_cmp( argument, "laugh" )) return TRUE;
       return FALSE;
       break;
     case 28:
       if( !str_cmp( argument, "giggle" )) return TRUE;
       return FALSE;
       break;
     case 29:
       if( !str_cmp( argument, "shake" )) return TRUE;
       return FALSE;
       break;
     case 30:
       if( !str_cmp( argument, "puke" )) return TRUE;
       return FALSE;
       break;
     case 31:
       if( !str_cmp( argument, "growl" )) return TRUE;
       return FALSE;
       break;
     case 32:
       if( !str_cmp( argument, "scream" )) return TRUE;
       return FALSE;
       break;
     case 33:
       if( !str_cmp( argument, "insult" )) return TRUE;
       return FALSE;
       break;
     case 34:
       if( !str_cmp( argument, "comfort" )) return TRUE;
       return FALSE;
       break;
     case 35:
       if( !str_cmp( argument, "nod" )) return TRUE;
       return FALSE;
       break;
     case 36:
       if( !str_cmp( argument, "sigh" )) return TRUE;
       return FALSE;
       break;
     case 37:
       if( !str_cmp( argument, "sulk" )) return TRUE;
       return FALSE;
       break;
     case 42:
       if( !str_cmp( argument, "stand" )) return TRUE;
       return FALSE;
       break;
     case 43:
       if( !str_cmp( argument, "sit" )) return TRUE;
       return FALSE;
       break;
     case 44:
       if( !str_cmp( argument, "rest" )) return TRUE;
       return FALSE;
       break;
     case 45:
       if( !str_cmp( argument, "sleep" )) return TRUE;
       return FALSE;
       break;
     case 46:
       if( !str_cmp( argument, "wake" )) return TRUE;
       return FALSE;
       break;
     case 49:
       if( !str_cmp( argument, "hug" )) return TRUE;
       return FALSE;
       break;
     case 50:
       if( !str_cmp( argument, "snuggle" )) return TRUE;
       return FALSE;
       break;
     case 51:
       if( !str_cmp( argument, "cuddle" )) return TRUE;
       return FALSE;
       break;
     case 52:
       if( !str_cmp( argument, "nuzzle" )) return TRUE;
       return FALSE;
       break;
     case 53:
       if( !str_cmp( argument, "cry" )) return TRUE;
       return FALSE;
       break;
     case 60:
       if( !str_cmp( argument, "drop" )) return TRUE;
       return FALSE;
       break;
     case 62:
       if( !str_cmp( argument, "weather" )) return TRUE;
       return FALSE;
       break;
     case 63:
       if( !str_cmp( argument, "read" )) return TRUE;
       return FALSE;
       break;
     case 64:
       if( !str_cmp( argument, "pour" )) return TRUE;
       return FALSE;
       break;
     case 65:
       if( !str_cmp( argument, "grab" )) return TRUE;
       return FALSE;
       break;
     case 67:
       if( !str_cmp( argument, "put" )) return TRUE;
       return FALSE;
       break;
     case 70:
       if( !str_cmp( argument, "hit" )) return TRUE;
       return FALSE;
       break;
     case 72:
       if( !str_cmp( argument, "give" )) return TRUE;
       return FALSE;
       break;
     case 83:
       if( !str_cmp( argument, "whisper" )) return TRUE;
       return FALSE;
       break;
     case 86:
       if( !str_cmp( argument, "ask" )) return TRUE;
       return FALSE;
       break;
     case 87:
       if( !str_cmp( argument, "order" )) return TRUE;
       return FALSE;
       break;
     case 88:
       if( !str_cmp( argument, "sip" )) return TRUE;
       return FALSE;
       break;
     case 89:
       if( !str_cmp( argument, "taste" )) return TRUE;
       return FALSE;
       break;
     case 91:
       if( !str_cmp( argument, "follow" )) return TRUE;
       return FALSE;
       break;
     case 93:
       if( !str_cmp( argument, "offer" )) return TRUE;
       return FALSE;
       break;
     case 94:
       if( !str_cmp( argument, "poke" )) return TRUE;
       return FALSE;
       break;
     case 96:
       if( !str_cmp( argument, "accuse" )) return TRUE;
       return FALSE;
       break;
     case 97:
       if( !str_cmp( argument, "grin" )) return TRUE;
       return FALSE;
       break;
     case 98:
       if( !str_cmp( argument, "bow" )) return TRUE;
       return FALSE;
       break;
     case 99:
       if( !str_cmp( argument, "open" )) return TRUE;
       return FALSE;
       break;
     case 100:
       if( !str_cmp( argument, "close" )) return TRUE;
       return FALSE;
       break;
     case 101:
       if( !str_cmp( argument, "lock" )) return TRUE;
       return FALSE;
       break;
     case 102:
       if( !str_cmp( argument, "unlock" )) return TRUE;
       return FALSE;
       break;
     case 103:
       if( !str_cmp( argument, "leave" )) return TRUE;
       return FALSE;
       break;
     case 104:
       if( !str_cmp( argument, "applaud" )) return TRUE;
       return FALSE;
       break;
     case 105:
       if( !str_cmp( argument, "blush" )) return TRUE;
       return FALSE;
       break;
     case 106:
       if( !str_cmp( argument, "burp" )) return TRUE;
       return FALSE;
       break;
     case 107:
       if( !str_cmp( argument, "chuckle" )) return TRUE;
       return FALSE;
       break;
     case 108:
       if( !str_cmp( argument, "clap" )) return TRUE;
       return FALSE;
       break;
     case 109:
       if( !str_cmp( argument, "cough" )) return TRUE;
       return FALSE;
       break;
     case 110:
       if( !str_cmp( argument, "curtsey" )) return TRUE;
       return FALSE;
       break;
     case 111:
       if( !str_cmp( argument, "fart" )) return TRUE;
       return FALSE;
       break;
     case 112:
       if( !str_cmp( argument, "flip" )) return TRUE;
       return FALSE;
       break;
     case 113:
       if( !str_cmp( argument, "fondle" )) return TRUE;
       return FALSE;
       break;
     case 114:
       if( !str_cmp( argument, "frown" )) return TRUE;
       return FALSE;
       break;
     case 115:
       if( !str_cmp( argument, "gasp" )) return TRUE;
       return FALSE;
       break;
     case 116:
       if( !str_cmp( argument, "glare" )) return TRUE;
       return FALSE;
       break;
     case 117:
       if( !str_cmp( argument, "groan" )) return TRUE;
       return FALSE;
       break;
     case 118:
       if( !str_cmp( argument, "grope" )) return TRUE;
       return FALSE;
       break;
     case 119:
       if( !str_cmp( argument, "hiccup" )) return TRUE;
       return FALSE;
       break;
     case 120:
       if( !str_cmp( argument, "lick" )) return TRUE;
       return FALSE;
       break;
     case 121:
       if( !str_cmp( argument, "love" )) return TRUE;
       return FALSE;
       break;
     case 122:
       if( !str_cmp( argument, "moan" )) return TRUE;
       return FALSE;
       break;
     case 123:
       if( !str_cmp( argument, "nibble" )) return TRUE;
       return FALSE;
       break;
     case 124:
       if( !str_cmp( argument, "pout" )) return TRUE;
       return FALSE;
       break;
     case 125:
       if( !str_cmp( argument, "purr" )) return TRUE;
       return FALSE;
       break;
     case 126:
       if( !str_cmp( argument, "ruffle" )) return TRUE;
       return FALSE;
       break;
     case 127:
       if( !str_cmp( argument, "shiver" )) return TRUE;
       return FALSE;
       break;
     case 128:
       if( !str_cmp( argument, "shrug" )) return TRUE;
       return FALSE;
       break;
     case 129:
       if( !str_cmp( argument, "sing" )) return TRUE;
       return FALSE;
       break;
     case 130:
       if( !str_cmp( argument, "slap" )) return TRUE;
       return FALSE;
       break;
     case 131:
       if( !str_cmp( argument, "smirk" )) return TRUE;
       return FALSE;
       break;
     case 132:
       if( !str_cmp( argument, "snap" )) return TRUE;
       return FALSE;
       break;
     case 133:
       if( !str_cmp( argument, "sneeze" )) return TRUE;
       return FALSE;
       break;
     case 134:
       if( !str_cmp( argument, "snicker" )) return TRUE;
       return FALSE;
       break;
     case 135:
       if( !str_cmp( argument, "sniff" )) return TRUE;
       return FALSE;
       break;
     case 136:
       if( !str_cmp( argument, "snore" )) return TRUE;
       return FALSE;
       break;
     case 137:
       if( !str_cmp( argument, "spit" )) return TRUE;
       return FALSE;
       break;
     case 138:
       if( !str_cmp( argument, "squeeze" )) return TRUE;
       return FALSE;
       break;
     case 139:
       if( !str_cmp( argument, "stare" )) return TRUE;
       return FALSE;
       break;
     case 140:
       if( !str_cmp( argument, "strut" )) return TRUE;
       return FALSE;
       break;
     case 141:
       if( !str_cmp( argument, "thank" )) return TRUE;
       return FALSE;
       break;
     case 142:
       if( !str_cmp( argument, "twiddle" )) return TRUE;
       return FALSE;
       break;
     case 143:
       if( !str_cmp( argument, "wave" )) return TRUE;
       return FALSE;
       break;
     case 144:
       if( !str_cmp( argument, "whistle" )) return TRUE;
       return FALSE;
       break;
     case 145:
       if( !str_cmp( argument, "wiggle" )) return TRUE;
       return FALSE;
       break;
     case 146:
       if( !str_cmp( argument, "wink" )) return TRUE;
       return FALSE;
       break;
     case 147:
       if( !str_cmp( argument, "yawn" )) return TRUE;
       return FALSE;
       break;
     case 150:
       if( !str_cmp( argument, "hold" )) return TRUE;
       return FALSE;
       break;
     case 155:
       if( !str_cmp( argument, "pick" )) return TRUE;
       return FALSE;
       break;
     case 156:
       if( !str_cmp( argument, "steal" )) return TRUE;
       return FALSE;
       break;
     case 160:
       if( !str_cmp( argument, "french" )) return TRUE;
       return FALSE;
       break;
     case 161:
       if( !str_cmp( argument, "comb" )) return TRUE;
       return FALSE;
       break;
     case 162:
       if( !str_cmp( argument, "massage" )) return TRUE;
       return FALSE;
       break;
     case 163:
       if( !str_cmp( argument, "tickle" )) return TRUE;
       return FALSE;
       break;
     case 165:
       if( !str_cmp( argument, "pat" )) return TRUE;
       return FALSE;
       break;
     case 166:
       if( !str_cmp( argument, "examine" )) return TRUE;
       return FALSE;
       break;
     case 167:
       if( !str_cmp( argument, "take" )) return TRUE;
       return FALSE;
       break;
     case 169:
       if( !str_cmp( argument, "spells" )) return TRUE;
       return FALSE;
       break;
     case 171:
       if( !str_cmp( argument, "curse" )) return TRUE;
       return FALSE;
       break;
     case 172:
       if( !str_cmp( argument, "use" )) return TRUE;
       return FALSE;
       break;
     case 178:
       if( !str_cmp( argument, "beg" )) return TRUE;
       return FALSE;
       break;
     case 179:
       if( !str_cmp( argument, "bleed" )) return TRUE;
       return FALSE;
       break;
     case 180:
       if( !str_cmp( argument, "cringe" )) return TRUE;
       return FALSE;
       break;
     case 181:
       if( !str_cmp( argument, "dream" )) return TRUE;
       return FALSE;
       break;
     case 182:
       if( !str_cmp( argument, "fume" )) return TRUE;
       return FALSE;
       break;
     case 183:
       if( !str_cmp( argument, "grovel" )) return TRUE;
       return FALSE;
       break;
     case 184:
       if( !str_cmp( argument, "hop" )) return TRUE;
       return FALSE;
       break;
     case 185:
       if( !str_cmp( argument, "nudge" )) return TRUE;
       return FALSE;
       break;
     case 186:
       if( !str_cmp( argument, "peer" )) return TRUE;
       return FALSE;
       break;
     case 187:
       if( !str_cmp( argument, "point" )) return TRUE;
       return FALSE;
       break;
     case 188:
       if( !str_cmp( argument, "ponder" )) return TRUE;
       return FALSE;
       break;
     case 189:
       if( !str_cmp( argument, "punch" )) return TRUE;
       return FALSE;
       break;
     case 190:
       if( !str_cmp( argument, "snarl" )) return TRUE;
       return FALSE;
       break;
     case 191:
       if( !str_cmp( argument, "spank" )) return TRUE;
       return FALSE;
       break;
     case 192:
       if( !str_cmp( argument, "steam" )) return TRUE;
       return FALSE;
       break;
     case 193:
       if( !str_cmp( argument, "tackle" )) return TRUE;
       return FALSE;
       break;
     case 194:
       if( !str_cmp( argument, "taunt" )) return TRUE;
       return FALSE;
       break;
     case 195:
       if( !str_cmp( argument, "think" )) return TRUE;
       return FALSE;
       break;
     case 196:
       if( !str_cmp( argument, "whine" )) return TRUE;
       return FALSE;
       break;
     case 197:
       if( !str_cmp( argument, "worship" )) return TRUE;
       return FALSE;
       break;
     case 198:
       if( !str_cmp( argument, "yodel" )) return TRUE;
       return FALSE;
       break;
     case 206:
       if( !str_cmp( argument, "quaff" )) return TRUE;
       return FALSE;
       break;
     case 207:
       if( !str_cmp( argument, "recite" )) return TRUE;
       return FALSE;
       break;
     case 240:
       if( !str_cmp( argument, "fill" )) return TRUE;
       return FALSE;
       break;
     case 248:
       if( !str_cmp( argument, "listen" )) return TRUE;
       return FALSE;
       break;
     case 256:
       if( !str_cmp( argument, "bribe" )) return TRUE;
       return FALSE;
       break;
     case 257:
       if( !str_cmp( argument, "bonk" )) return TRUE;
       return FALSE;
       break;
     case 258:
       if( !str_cmp( argument, "calm" )) return TRUE;
       return FALSE;
       break;
     case 259:
       if( !str_cmp( argument, "rub" )) return TRUE;
       return FALSE;
       break;
     case 260:
       if( !str_cmp( argument, "censor" )) return TRUE;
       return FALSE;
       break;
     case 261:
       if( !str_cmp( argument, "choke" )) return TRUE;
       return FALSE;
       break;
     case 262:
       if( !str_cmp( argument, "drool" )) return TRUE;
       return FALSE;
       break;
     case 263:
       if( !str_cmp( argument, "flex" )) return TRUE;
       return FALSE;
       break;
     case 264:
       if( !str_cmp( argument, "jump" )) return TRUE;
       return FALSE;
       break;
     case 265:
       if( !str_cmp( argument, "lean" )) return TRUE;
       return FALSE;
       break;
     case 266:
       if( !str_cmp( argument, "moon" )) return TRUE;
       return FALSE;
       break;
     case 267:
       if( !str_cmp( argument, "ogle" )) return TRUE;
       return FALSE;
       break;
     case 268:
       if( !str_cmp( argument, "pant" )) return TRUE;
       return FALSE;
       break;
     case 269:
       if( !str_cmp( argument, "pinch" )) return TRUE;
       return FALSE;
       break;
     case 270:
       if( !str_cmp( argument, "push" )) return TRUE;
       return FALSE;
       break;
     case 271:
       if( !str_cmp( argument, "scare" )) return TRUE;
       return FALSE;
       break;
     case 272:
       if( !str_cmp( argument, "scold" )) return TRUE;
       return FALSE;
       break;
     case 273:
       if( !str_cmp( argument, "seduce" )) return TRUE;
       return FALSE;
       break;
     case 274:
       if( !str_cmp( argument, "shove" )) return TRUE;
       return FALSE;
       break;
     case 275:
       if( !str_cmp( argument, "shudder" )) return TRUE;
       return FALSE;
       break;
     case 276:
       if( !str_cmp( argument, "shush" )) return TRUE;
       return FALSE;
       break;
     case 277:
       if( !str_cmp( argument, "slobber" )) return TRUE;
       return FALSE;
       break;
     case 278:
       if( !str_cmp( argument, "smell" )) return TRUE;
       return FALSE;
       break;
     case 279:
       if( !str_cmp( argument, "sneer" )) return TRUE;
       return FALSE;
       break;
     case 280:
       if( !str_cmp( argument, "spin" )) return TRUE;
       return FALSE;
       break;
     case 281:
       if( !str_cmp( argument, "squirm" )) return TRUE;
       return FALSE;
       break;
     case 282:
       if( !str_cmp( argument, "stomp" )) return TRUE;
       return FALSE;
       break;
     case 283:
       if( !str_cmp( argument, "strangle" )) return TRUE;
       return FALSE;
       break;
     case 284:
       if( !str_cmp( argument, "stretch" )) return TRUE;
       return FALSE;
       break;
     case 285:
       if( !str_cmp( argument, "tap" )) return TRUE;
       return FALSE;
       break;
     case 286:
       if( !str_cmp( argument, "tease" )) return TRUE;
       return FALSE;
       break;
     case 287:
       if( !str_cmp( argument, "tiptoe" )) return TRUE;
       return FALSE;
       break;
     case 288:
       if( !str_cmp( argument, "tweak" )) return TRUE;
       return FALSE;
       break;
     case 289:
       if( !str_cmp( argument, "twirl" )) return TRUE;
       return FALSE;
       break;
     case 290:
       if( !str_cmp( argument, "undress" )) return TRUE;
       return FALSE;
       break;
     case 291:
       if( !str_cmp( argument, "whimper" )) return TRUE;
       return FALSE;
       break;
     case 292:
       if( !str_cmp( argument, "exchange" )) return TRUE;
       return FALSE;
       break;
     case 293:
       if( !str_cmp( argument, "release" )) return TRUE;
       return FALSE;
       break;
     case 294:
       if( !str_cmp( argument, "search" )) return TRUE;
       return FALSE;
       break;
     case 306:
       if( !str_cmp( argument, "caress" )) return TRUE;
       return FALSE;
       break;
     case 314:
       if( !str_cmp( argument, "yell" )) return TRUE;
       return FALSE;
       break;
     case 320:
       if( !str_cmp( argument, "touch" )) return TRUE;
       return FALSE;
       break;
     case 321:
       if( !str_cmp( argument, "scratch" )) return TRUE;
       return FALSE;
       break;
     case 322:
       if( !str_cmp( argument, "wince" )) return TRUE;
       return FALSE;
       break;
     case 323:
       if( !str_cmp( argument, "toss" )) return TRUE;
       return FALSE;
       break;
     case 324:
       if( !str_cmp( argument, "flame" )) return TRUE;
       return FALSE;
       break;
     case 325:
       if( !str_cmp( argument, "arch" )) return TRUE;
       return FALSE;
       break;
     case 326:
       if( !str_cmp( argument, "amaze" )) return TRUE;
       return FALSE;
       break;
     case 327:
       if( !str_cmp( argument, "bathe" )) return TRUE;
       return FALSE;
       break;
     case 328:
       if( !str_cmp( argument, "embrace" )) return TRUE;
       return FALSE;
       break;
     case 329:
       if( !str_cmp( argument, "brb" )) return TRUE;
       return FALSE;
       break;
     case 330:
       if( !str_cmp( argument, "ack" )) return TRUE;
       return FALSE;
       break;
     case 331:
       if( !str_cmp( argument, "cheer" )) return TRUE;
       return FALSE;
       break;
     case 332:
       if( !str_cmp( argument, "snort" )) return TRUE;
       return FALSE;
       break;
     case 333:
       if( !str_cmp( argument, "eyebrow" )) return TRUE;
       return FALSE;
       break;
     case 334:
       if( !str_cmp( argument, "bang" )) return TRUE;
       return FALSE;
       break;
     case 335:
       if( !str_cmp( argument, "pillow" )) return TRUE;
       return FALSE;
       break;
     case 336:
       if( !str_cmp( argument, "nap" )) return TRUE;
       return FALSE;
       break;
     case 337:
       if( !str_cmp( argument, "nose" )) return TRUE;
       return FALSE;
       break;
     case 338:
       if( !str_cmp( argument, "raise" )) return TRUE;
       return FALSE;
       break;
     case 339:
       if( !str_cmp( argument, "hand" )) return TRUE;
       return FALSE;
       break;
     case 340:
       if( !str_cmp( argument, "pull" )) return TRUE;
       return FALSE;
       break;
     case 341:
       if( !str_cmp( argument, "tug" )) return TRUE;
       return FALSE;
       break;
     case 342:
       if( !str_cmp( argument, "wet" )) return TRUE;
       return FALSE;
       break;
     case 343:
       if( !str_cmp( argument, "mosh" )) return TRUE;
       return FALSE;
       break;
     case 344:
       if( !str_cmp( argument, "wait" )) return TRUE;
       return FALSE;
       break;
     case 345:
       if( !str_cmp( argument, "hi5" )) return TRUE;
       return FALSE;
       break;
     case 346:
       if( !str_cmp( argument, "envy" )) return TRUE;
       return FALSE;
       break;
     case 347:
       if( !str_cmp( argument, "flirt" )) return TRUE;
       return FALSE;
       break;
     case 348:
       if( !str_cmp( argument, "bark" )) return TRUE;
       return FALSE;
       break;
     case 349:
       if( !str_cmp( argument, "whap" )) return TRUE;
       return FALSE;
       break;
     case 350:
       if( !str_cmp( argument, "roll" )) return TRUE;
       return FALSE;
       break;
     case 351:
       if( !str_cmp( argument, "blink" )) return TRUE;
       return FALSE;
       break;
     case 352:
       if( !str_cmp( argument, "duh" )) return TRUE;
       return FALSE;
       break;
     case 353:
       if( !str_cmp( argument, "gag" )) return TRUE;
       return FALSE;
       break;
     case 354:
       if( !str_cmp( argument, "grumble" )) return TRUE;
       return FALSE;
       break;
     case 355:
       if( !str_cmp( argument, "dropkick" )) return TRUE;
       return FALSE;
       break;
     case 356:
       if( !str_cmp( argument, "whatever" )) return TRUE;
       return FALSE;
       break;
     case 357:
       if( !str_cmp( argument, "fool" )) return TRUE;
       return FALSE;
       break;
     case 358:
       if( !str_cmp( argument, "noogie" )) return TRUE;
       return FALSE;
       break;
     case 359:
       if( !str_cmp( argument, "melt" )) return TRUE;
       return FALSE;
       break;
     case 360:
       if( !str_cmp( argument, "smoke" )) return TRUE;
       return FALSE;
       break;
     case 361:
       if( !str_cmp( argument, "wheeze" )) return TRUE;
       return FALSE;
       break;
     case 362:
       if( !str_cmp( argument, "bird" )) return TRUE;
       return FALSE;
       break;
     case 363:
       if( !str_cmp( argument, "boggle" )) return TRUE;
       return FALSE;
       break;
     case 364:
       if( !str_cmp( argument, "hiss" )) return TRUE;
       return FALSE;
       break;
     case 365:
       if( !str_cmp( argument, "bite" )) return TRUE;
       return FALSE;
       break;
   }

   return FALSE;
}

