/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright5 (C) 1992, 1993 by Michael         *
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
#include <math.h>
#if defined( macintosh )
#include <types.h>
#elseif !defined( WIN32 )
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include "merc.h"

#if !defined( macintosh )
extern  int     _filbuf	        args( (FILE *) );
#endif

#if !defined( ultrix ) && !defined( apollo )
#include <memory.h>
#endif


char * unique_exists( OBJ_INDEX_DATA *pObjIndex );
void fill_arti_holders( void );
bool is_artifact( int vnum );
/* 
 * Globals. 
 */ 
SYSTEM_DATA		sysdata;		/* for mud configuration */

HELP_DATA *		help_first;
HELP_DATA *		help_last;

SHOP_DATA *		shop_first;
SHOP_DATA *		shop_last;

GAME_DATA *		game_first;
GAME_DATA *		game_last;

CHAR_DATA *		char_free;

CAST_DATA *           cast_list = NULL;  // To keep track of everyone casting spells
PERSON_DATA *           wait_list = NULL;  // To keep track of everyone lagged.

EXTRA_DESCR_DATA *	extra_descr_free;
AFFECT_DATA *		affect_free;
NOTE_DATA *		note_free;
OBJ_DATA *		obj_free;
VEHICLE_DATA *		vehicle_free;
PC_DATA *		pcdata_free;

char                    bug_buf                 [ MAX_INPUT_LENGTH*2 ];
CHAR_DATA *		char_list;
CHAR_DATA *             not_char_list;
char *			help_greeting;
char *			help_greeting_color;
char	                log_buf                 [ MAX_INPUT_LENGTH*2 ];
KILL_DATA	        kill_table              [ MAX_LEVEL          ];
NOTE_DATA *		note_list;
OBJ_DATA *		object_list;
VEHICLE_DATA *		vehicle_list;
CRIME_DATA *            crime_list;
TIME_INFO_DATA		time_info;
WEATHER_DATA		weather_info;
time_t                  down_time;
time_t                  warning1;
time_t                  warning2;
bool                    Reboot;
QUEST_INDEX_DATA *      quest_first = NULL;
ROOM_INDEX_DATA *       room_index_hash[MAX_KEY_HASH];

// WARNING: If you get more than 16kb of illegal names, this will have to
// be increased or the mud will barf.  also, is_name will also have to
// change.  - Veygoth
char                    strName[MAX_STRING_LENGTH * 4];

int                     gsn_alcohol_tolerance;
int                     gsn_appraise;
int                     gsn_aware;
int			gsn_backstab;
int                     gsn_bandage;
int                     gsn_bash;
int                     gsn_berserk;		/* by Thelonius */
int			gsn_bladeflash;
int                     gsn_blindfighting;
int                     gsn_bodyslam;
int                     gsn_capture;		/* by Binky / Thelonius */
int                     gsn_charge;
int                     gsn_circle;		/* by Thelonius */
int			gsn_disarm;
int			gsn_dodge;
int                     gsn_drums;
int                     gsn_dulcimer;
int                     gsn_fiddle;
int                     gsn_flute;
int                     gsn_guard;
int                     gsn_harp;
int                     gsn_headbutt;
int			gsn_hide;
int                     gsn_horn;
int                     gsn_instant_kill;
int                     gsn_listen;
int                     gsn_lyre;
int                     gsn_mandolin;
int                     gsn_mounted_combat;
int			gsn_peek;
int			gsn_pick_lock;
int			gsn_pickpocket;
int                     gsn_pipes;
int                     gsn_poison;
int                     gsn_poison_bite;
int                     gsn_poison_weapon;      /* by Thelonius */
int                     gsn_quick_chant;
int                     gsn_scrolls;		/* by Binky / Thelonius */
int			gsn_sneak;
int                     gsn_springleap;
int                     gsn_staves;		/* by Binky / Thelonius */
int			gsn_steal;
int			gsn_summon_mount;
int                     gsn_switch;
int                     gsn_untangle;		/* by Thelonius */
int                     gsn_wands;		/* by Binky / Thelonius */
int                     gsn_doorbash;
int                     gsn_dual;		/* by Thelonius */
int			gsn_enhanced_damage;
int			gsn_kick;
int			gsn_parry;
int			gsn_rescue;
int                     gsn_riposte;
int			gsn_second_attack;
int                     gsn_singing;
int			gsn_third_attack;
int			gsn_throat_crush;
int                     gsn_throw;              /* Lithos */
/*
 * Psionicist gsn's (by Thelonius).
 */
int                     gsn_chameleon;
int                     gsn_heighten;
int                     gsn_shadow;
/*
 * New gsn's (by Zen & other ppl).
 */
int                     gsn_shield_block;
int                     gsn_climb;              // Veygoth
int                     gsn_fast_healing;
int                     gsn_fourth_attack;
int			gsn_brew;
int			gsn_scribe;
int                     gsn_dirt;
int                     gsn_meditate;		/* by Zen */
int                     gsn_safe_fall;          // Veygoth
int			gsn_swim;		/* by Zen */
int                     gsn_trip;
int			gsn_track;
int                     gsn_tumble;             // Veygoth
int                     gsn_unarmed;            /* Veygoth */
int			gsn_whirlwind;
int			gsn_hit;
int			gsn_slash;
int			gsn_pierce;
int			gsn_whip;
int			gsn_explode;
int			gsn_pound;
int			gsn_suction;
int			gsn_shot;
int			gsn_mount;
/*
 * Race gsn's (by Kahn).
 */
int                     gsn_vampiric_bite;
// Misc gsn stuff by Veygoth
int                     gsn_carve;
int                     gsn_layhands_timer;
int                     gsn_summon_mount_timer;

/*
 * Spell GSN's
 *
 * These should be in alphabetical order for easy reference.
 */
int                     spl_acid_arrow;
int                     spl_acid_blast;
int                     spl_agitation;
int                     spl_agony;
int                     spl_aid;
int                     spl_airy_starshell;
int                     spl_airy_water;
int			spl_animate_dead; 	/* Govno */
int                     spl_armor;
int                     spl_ball_lightning;
int                     spl_barkskin;
int                     spl_battle_ecstasy;
int                     spl_bear;
int                     spl_blending;
int			spl_bless;
int			spl_blur;
int                     spl_burning_hands;
int			spl_blindness;
int                     spl_bombard;
int                     spl_bonethunder;
int     		spl_cause_critical;
int     		spl_cause_light;
int	     		spl_cause_serious;
int                     spl_change_self;
int			spl_charm_person;
int                     spl_chill_touch;
int			spl_cloak_of_fear;	/* Govno */
int                     spl_cold_ward;
int                     spl_concealment;
int                     spl_conflaguration;
int                     spl_cone_of_cold;
int                     spl_cowardice;
int			spl_create_dracolich;	/* Govno */
int                     spl_creeping_doom;
int                     spl_cure_blind;
int                     spl_cure_critical;
int                     spl_cure_light;
int                     spl_cure_serious;
int			spl_curse;
int                     spl_cure_blind;
int                     spl_cure_critical;
int                     spl_cure_light;
int                     spl_cure_serious;
int			spl_curse;
int			spl_cutting_breeze;
int                     spl_dazzle;
int                     spl_destroy_undead;
int                     spl_dexterity;
int                     spl_detect_evil;
int                     spl_detect_good;
int                     spl_detect_invis;
int                     spl_detect_magic;
int                     spl_detect_undead;
int                     spl_dirt_cloud;
int                     spl_disintegrate;
int                     spl_domination;
int                     spl_dust_blast;
int                     spl_earthen_grasp;
int                     spl_earthen_starshell;
int                     spl_earthen_tomb;
int                     spl_earthquake;
int                     spl_elemental_sight;
int                     spl_elemental_form;
int                     spl_elephant;
int                     spl_energy_drain;
int                     spl_enfeeblement;
int                     spl_enlarge;
int                     spl_enthrall;
int                     spl_fear;
int                     spl_feeblemind;
int                     spl_fiery_starshell;
int                     spl_fireball;
int                     spl_fire_bolt;
int                     spl_firestorm;
int                     spl_fire_ward;
int                     spl_flameburst;
int			spl_flame_shield;
int                     spl_flamestrike;
int                     spl_fly;
int			spl_frost_shield;
int                     spl_fullheal;
int                     spl_full_harm;
int                     spl_giant_strength;
int                     spl_gr_earthen_grasp;
int                     spl_gr_mending;
int                     spl_remove_poison;
int                     spl_harm;
int                     spl_haste;
int                     spl_hawk;
int                     spl_heal;
int			spl_heal_undead;	/* Govno */
int                     spl_herbal_remedy;
int                     spl_hex;
int                     spl_holy_sacrifice;
int                     spl_holy_word;
int                     spl_hypnotic_pattern;
int                     spl_iceball;
int                     spl_ice_bolt;
int                     spl_ice_missile;
int                     spl_ice_storm;
int                     spl_identify;
int                     spl_immolate;
int                     spl_incendiary_cloud;
int                     spl_incompetence;
int                     spl_infravision;
int			spl_invis;
int                     spl_levitation;
int                     spl_lightanddark;
int                     spl_lightning_bolt;
int                     spl_lightning_curtain;
int                     spl_lion;
int                     spl_ls_herbal_remedy;
int                     spl_ls_mending;
int			spl_magic_missile;      /* by the Mighty Mighty Ski */
int                     spl_magnetism;
int                     spl_major_globe;
int                     spl_malison;
int                     spl_mass_heal;
int			spl_mass_invis;
int			spl_mass_vortex_lift;	/* by Zen */
int                     spl_mending;
int                     spl_mermaids_kiss;
int                     spl_meteor;
int                     spl_minor_para;
int                     spl_m_blending;
int                     spl_mirage_arcana;
int                     spl_misdirection;
int                     spl_mole;
int                     spl_molten;
int                     spl_mouse;
int                     spl_nightmares;
int                     spl_panther;
int                     spl_pebble;
int                     spl_phantasmal_killer;
int			spl_plague;
int			spl_poison;
int                     spl_polymorph;
int                     spl_prismatic_spray;
int			spl_protect_acid;
int			spl_protect_cold;
int			spl_protect_fire;
int			spl_protect_gas;
int			spl_protect_lightning;
int			spl_protect_undead;	/* Govno */
int                     spl_prot_evil;
int                     spl_prowess;
int                     spl_pythonsting;
int			spl_raise_skeleton; 	/* Govno */
int                     spl_raise_zombie; 	/* Govno */
int                     spl_raise_spectre; 	/* Govno */
int                     spl_raise_wraith; 	/* Govno */
int                     spl_raise_vampire; 	/* Govno */
int                     spl_raise_lich; 	/* Govno */
int                     spl_ravenflight;
int                     spl_recall;
int                     spl_reduce;
int                     spl_rejuvenate;
int                     spl_remove_poison;
int                     spl_resurrect;
int                     spl_scalding;
int                     spl_scathing_wind;
int                     spl_scorching;
int                     spl_shield;
int                     spl_shocking_grasp;
int			spl_shock_shield;
int                     spl_shrew;
int			spl_silence;		/* by Thelonius */
int                     spl_soulshield;
int			spl_sleep;
int                     spl_slowness;
int                     spl_snail;
int                     spl_soothe;
int                     spl_soul_disturbance;
int                     spl_spark;
int                     spl_spirit_anguish;
int                     spl_spirit_armor;
int			spl_summon_elemental;
int                     spl_stoneskin;
int                     spl_stonestorm;
int                     spl_stornogs_spheres;
int                     spl_sunray;
int                     spl_sustenance;
int                     spl_telekinesis;
int                     spl_teleport;
int			spl_turn_undead;
int			spl_ultra_death_ray;
int                     spl_unholy_word;
int                     spl_vacancy;
int                     spl_vig_critical;
int                     spl_vig_light;
int                     spl_vig_serious;
int                     spl_vitality;
int                     spl_wall_of_fire;
int                     spl_wall_of_ice;
int                     spl_wall_of_mist;
int                     spl_wall_of_sparks;
int                     spl_water_breathing;	/* by Thelonius */
int                     spl_water_blast;
int                     spl_water_bolt;
int                     spl_watery_starshell;
int                     spl_wierd;
int                     spl_wither;
int                     spl_wolf;
int spl_bird_prey;
int spl_darkness;
int spl_miners_intuition;
int spl_wall_iron;
int spl_wall_stone;
int spl_wall_force;
int spl_wall_illusion;
int spl_mirror_image;
int spl_shadow_monsters;
int spl_demi_shad_monster;
int spl_shades;
int spl_cloak_fear;
int spl_clone;
int spl_power_word_blind;
int spl_spook;
int spl_sunburst;
int spl_call_lightning;
int spl_gr_agony;
int spl_m_agony;
int spl_ravenous_vines;
int spl_change_sex;
int spl_color_spray;
int spl_prismatic_orb;
int spl_shadow_magic;
int spl_demi_shad_magic;
int spl_cone_silence;
int spl_continual_light;
int spl_twilight;
int spl_control_weather;
int spl_create_food;
int spl_conjure_windsaber;
int spl_celestial_sword;
int spl_create_skin;
int spl_create_spring;
int spl_create_watern;
int spl_purify_spirit;
int spl_purify;
int spl_cure_disease;
int spl_nourishment;
int spl_ls_nourishment;
int spl_gr_sustenance;
int spl_rain_maker;
int spl_destroy_cursed;
int spl_spirit_sight;
int spl_analyze_balance;
int spl_sense_life;
int spl_sense_spirit;
int spl_gr_spirit_sight;
int spl_true_seeing;
int spl_comp_lang;
int spl_detect_poison;
int spl_dispel_evil;
int spl_gleam_dawn;
int spl_gleam_dusk;
int spl_dispel_magic;
int spl_enchant_weapon;
int spl_exorcise;
int spl_faerie_fire;
int spl_faerie_fog;
int spl_reveal_true_form;
int spl_reveal_true_name;
int spl_dispel_invisible;
int spl_negate_veil;
int spl_negate_luster;
int spl_illumination;
int spl_bigby_fist;
int spl_akiaurn_word;
int spl_windsaber;
int spl_flight;
int spl_gr_ravenflight;
int spl_gate;
int spl_farsee;
int spl_know_alignment;
int spl_reveal_spirit_essence;
int spl_touch_nightshade;
int spl_locate_object;
int spl_minor_creation;
int spl_elemental_creation;
int spl_tidal_wave;
int spl_cyclone;
int spl_whirlwind;
int spl_inferno;
int spl_earthen_rain;
int spl_gr_soul_dist;
int spl_group_heal;
int spl_wellness;
int spl_pass_door;
int spl_gr_pythonsting;
int spl_call_wild;
int spl_minor_globe;
int spl_group_globe;
int spl_spirit_ward;
int spl_gr_spirit_ward;
int spl_deny_air;
int spl_deny_earth;
int spl_deny_fire;
int spl_deny_water;
int spl_protection_good;
int spl_harbor_balance;
int spl_adaptation;
int spl_recharge_item;
int spl_stamina;
int spl_gr_stamina;
int spl_remove_alignment;
int spl_preserve;
int spl_remove_curse;
int spl_negate_hex;
int spl_remove_silence;
int spl_sanctuary;
int spl_shadow_shield;
int spl_entangle;
int spl_group_stone;
int spl_hurricane;
int spl_flashfire;
int spl_mass_dispel_magic;
int spl_stonestrength;
int spl_summon;
int spl_plane_shift;
int spl_flickering_flame;
int spl_ripple;
int spl_ventriloquate;
int spl_group_recall;
int spl_acid_breath;
int spl_fire_breath;
int spl_water_breath;
int spl_frost_breath;
int spl_gas_breath;
int spl_lightning_breath;
int spl_general_purpose;
int spl_high_explosive;
int spl_adrenaline_control;
int spl_aura_sight;
int spl_awe;
int spl_ballistic_attack;
int spl_biofeedback;
int spl_cell_adjustment;
int spl_combat_mind;
int spl_complete_healing;
int spl_control_flames;
int spl_create_sound;
int spl_deathfield;
int spl_detonate;
int spl_neurofrag;
int spl_displacement;
int spl_ectoplasmic_form;
int spl_ego_whip;
int spl_energy_containment;
int spl_enhance_armor;
int spl_enhanced_strength;
int spl_flesh_armor;
int spl_inertial_barrier;
int spl_inflict_pain;
int spl_intellect_fortress;
int spl_lend_health;
int spl_transfer_wellness;
int spl_mental_barrier;
int spl_mind_thrust;
int spl_project_force;
int spl_psionic_blast;
int spl_psychic_crush;
int spl_psychic_drain;
int spl_psychic_healing;
int spl_share_strength;
int spl_thought_shield;
int spl_ultrablast;
int spl_chain_lightning;
int spl_wizard_eye;
int spl_vortex_lift;
int spl_shadow_walk;
int spl_dimdoor;
int spl_migration;
int spl_shadow_door;
int spl_shadow_veil;
int spl_spirit_jump;
int spl_dispel_good;
int spl_judgement;
int spl_apocalypse;
int spl_power_word_lag;
int spl_power_word_stun;
int spl_etherportal;
int spl_moonwell;
int spl_wormhole;
int spl_create_buffet;
int spl_fireskin;
int spl_ethereal_shield;
int spl_slow_poison;
int spl_acid_spray;
int spl_aura;
int spl_earthen_smith;
int spl_fiery_smith;
int spl_airy_smith;
int spl_aquatic_smith;
int spl_hover;
int spl_azure_flame;
int spl_gust_of_frost;
int spl_tide_seas;
int spl_dismissal;
int spl_summon_serpent;
int spl_summon_nerneid;
int spl_summon_pech;
int spl_summon_slyph;
int spl_summon_gr_elem;
int spl_thirst;
int spl_tridenttides;

/* 
 * race GSNs
 */
int    race_human;
int    race_barbarian;
int    race_grey_elf;
int    race_rakshasa;
int    race_half_elf;
int    race_flind;
int    race_drow_elf;
int    race_dwarf;
int    race_duergar;
int    race_halfling;
int    race_troll;
int    race_ogre;
int    race_orc;
int    race_gnome;
int    race_centaur;
int    race_githyanki;
int    race_goblin;
int    race_minotaur;
int    race_githzerai;
int    race_illithid;
int    race_thri_kreen;
int    race_aquatic_elf;
int    race_sahaugin;
int    race_half_orc;
int    race_half_dwarf;
int    race_half_kobold;
int    race_giant;
int    race_neogi;
int    race_kobold;
int    race_hobgoblin;
int    race_hobgoblin;
int    race_humanoid;
int    race_demon;
int    race_god;
int    race_devil;
int    race_angel;
int    race_faerie;
int    race_vampire;
int    race_werewolf;
int    RACE_DRAGON;
int    race_beholder;
int    race_derro;
int    race_golem;
int    race_dracolich;
int    race_dragonkin;
int    race_insect;
int    race_animal;
int    race_herbivore;
int    race_carnivore;
int    race_bird;
int    race_horse;
int    race_primate;
int    race_undead;
int    race_ghost;
int    race_harpy;
int    race_bear;
int    race_rust_monster;
int    race_fire_elemental;
int    race_air_elemental;
int    race_water_elemental;
int    race_earth_elemental;
int    race_bat;
int    race_plant;
int    race_tree;
int    race_rat;
int    race_parasite;
int    race_lycanthrope;
int    race_arachnid;
int    race_object;
int    race_mist;
int    race_snake;
int    race_worm;
int    race_fish;
int    race_ixitxachitl;
int    race_aboleth;
int    race_hydra;
int    race_reptile;
int    race_boar;



// Traps
int     		trp_acid;
int     		trp_bash;
int     		trp_cold;
int                     trp_disease;
int     		trp_dispel;
int     		trp_energy;
int     		trp_fire;
int    	 		trp_gate;
int     		trp_harm;
int     		trp_para;
int     		trp_pierce;
int     		trp_poison;
int     		trp_slash;
int     		trp_sleep;
int     		trp_stun;
int     		trp_summon;
int     		trp_teleport;
int     		trp_wither;

int RACE_ELEM_FIRE;
int RACE_ELEM_AIR;
int RACE_ELEM_WATER;
int RACE_ELEM_EARTH;
int RACE_UNDEAD;
int RACE_GHOST;
int RACE_VAMPIRE;
int RACE_GOD;
int RACE_DEVIL;
int RACE_DEMON;
int RACE_DRACOLICH;

/*
 * Locals.
 */
MOB_INDEX_DATA *	mob_index_hash	        [ MAX_KEY_HASH       ];
OBJ_INDEX_DATA *	obj_index_hash	        [ MAX_KEY_HASH       ];

AREA_DATA *		area_first;
AREA_DATA *		area_last;

extern char		str_empty	        [ 1                  ];

int			top_affect;
int			top_area;
int			top_ed;
int			top_exit;
int			top_help;
int			top_mob_index;
int			top_obj_index;
int			top_reset;
int			top_room;
int			top_shop;
int			top_game;
int                     top_vnum_room;
int                     top_vnum_mob;
int                     top_vnum_obj;
int                     top_quest;
int                     top_qmsg;
int                     top_qdata;
int                     top_qitem;
int                     top_event;
int                     top_memdata;
int                     top_note;
int                     top_ban;
int                     top_desc;
int                     top_social;
int                     top_vehicle;
int                     top_obj;
int                     top_char;
int                     top_pcdata;
int                     top_trap;
int                     top_crime;
int                     top_mvscript;

/*
 * Memory management.
 * Increase MAX_STRING from 1500000 if you have too.
 * Tune the others only if you understand what you're doing.
 * MAX_STRING is now in ssm.c
 */
extern int		MAX_STRING;

#if defined( macintosh )
#define			MAX_PERM_BLOCK  131072
#define			MAX_MEM_LIST    11

void *			rgFreeList              [ MAX_MEM_LIST       ];
const int		rgSizeList              [ MAX_MEM_LIST       ]  =
{
    16, 32, 64, 128, 256, 1024, 2048, 4096, 8192, 16384, 32768-64
};
#else
#define			MAX_PERM_BLOCK  131072
#define			MAX_MEM_LIST    14

void *			rgFreeList              [ MAX_MEM_LIST       ];
const int		rgSizeList              [ MAX_MEM_LIST       ]  =
{
    4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768-64
};
#endif

extern int		nAllocString;
extern int		sAllocString;
extern int		nOverFlowString;
extern int		sOverFlowString;
extern bool		Full;
int			nAllocPerm;
int			sAllocPerm;


/*
 * Semi-locals.
 */
bool			fBootDb;
FILE *			fpArea;
char			strArea			[ MAX_INPUT_LENGTH   ];

void    init_string_space ( void );
void    boot_done( );
char *  daPrompt;

/*
 * Local booting procedures.
 */
void	init_mm		  args( ( void ) );

void	load_area_file    args( ( void ) );
void	load_area         args( ( FILE *fp ) );
void	load_helps        args( ( FILE *fp ) );
void    load_recall       args( ( FILE *fp ) );
void	load_resets       args( ( FILE *fp ) );
void	load_specials     args( ( FILE *fp ) );
void	load_games	  args( ( FILE *fp ) );
void	load_notes	  args( ( void ) );
void	load_ban          args( ( void ) );
void    load_names        args( ( void ) );
void    load_sysdata      args( ( void ) );
void    save_sysdata      args( ( void ) );
void    load_down_time    args( ( void ) );
void	fix_exits         args( ( void ) );
int     convert_envy_item_type args( ( int type ) );
int     convert_envy_act_flags args( ( int flags ) );
int     convert_envy_room_flags args( ( int flags ) );
int     convert_envy_exit_flags args( ( int flags ) );
void    convert_envy_affect_flags args ( ( int* flags ) );
void    load_quests       args( ( FILE *fp ) );
void	reset_area        args( ( AREA_DATA *pArea ) );
void    make_arti_owner   args( ( char * const owner, const int vnum ) );
int     envy_race_convert args( ( char * name ) );

/*
 * MOBprogram locals
 */
int 		prog_name_to_type	args ( ( char* name ) );
MPROG_DATA *	mprog_file_read 		args ( ( const char* f, MPROG_DATA* mprg, 
						MOB_INDEX_DATA *pMobIndex ) );
MPROG_DATA *	oprog_file_read 	args ( ( const char* f, MPROG_DATA* oprg, 
						OBJ_INDEX_DATA *pObjIndex ) );
void		load_mobprogs           args ( ( FILE* fp ) );
void            load_objprogs           args ( ( FILE* fp ) );

/*
 * Non-Envy Loading procedures.
 * Put any new loading function in this section.
 */
void    new_load_area		args( ( FILE *fp ) );	/* OLC */
void    load_magma_rooms         args( ( FILE *fp ) );  /* Magma rooms*/
void    load_envy_rooms          args( ( FILE *fp ) );  /* Emvy rooms */
void    load_magma_objects       args( ( FILE *fp ) );  /* Magma objs*/
void    load_envy_objects        args( ( FILE *fp ) );  /* Envy objs */
void    load_magma_mobiles       args( ( FILE *fp ) );  /* Magma mobs*/
void    load_envy_mobiles        args( ( FILE *fp ) );  /* Envy mobs */
void    load_magma_resets        args( ( FILE *fp ) );  /* Magma loads*/
void    load_magma_shops         args( ( FILE *fp ) );  /* Magma shops*/
void    load_envy_shops          args( ( FILE *fp ) );  /* Envy shops */
int     magma_spell_lookup       args( ( int skill ) );  /* Magma Magic items*/

/*
 * Big mama top level function.
 */
void boot_db( void )
{
#ifdef HARDCORE_DEBUG
    ROOM_INDEX_DATA *pRoomIndex;
    char buf[MAX_STRING_LENGTH];
#endif
    int i;

//  can't call this, haven't initialised buffers yet
//    last_fun( "boot_db");
    /*
     * Init some data space stuff.
     */
    {
        init_string_space( );
	fBootDb		= TRUE;
    }

    fprintf( stderr, "[*****] BOOT: ---------------------------[ Boot Log ]---------------------------\n" );

    log_string( "Initializing last_funs buffers" );
    for( i = 0; i < MAX_LAST_FUNS; i++ ) {
        //allocate memory
        last_funs[i] = (char *) calloc( 100, sizeof( char ) );
    }
    last_funs_init = TRUE;
    for( i = 0; i < MAX_LAST_COMS; i++) {
        last_coms[i] = (char *) calloc( 100, sizeof( char ) );
    }
    last_coms_init = TRUE;

    /*
     * Init random number generator.
     */
    log_string( "Initializing random number generator" );
    {
	init_mm( );
    }

    /*
     * Set time and weather.
     */
    log_string( "Setting time and weather" );
    {
	long lhour, lday, lmonth;

	lhour		= (int)( current_time - 650336715 )
			   / ( PULSE_WEATHER / PULSE_PER_SECOND );
	time_info.hour  = lhour  % 24;
	lday		= lhour  / 24;
	time_info.day	= lday   % 35;
	lmonth		= lday   / 35;
	time_info.month	= lmonth % 17;
	time_info.year	= lmonth / 17;

	     if ( time_info.hour <  5 ) weather_info.sunlight = MOON_SET;
	else if ( time_info.hour <  6 ) weather_info.sunlight = SUN_RISE;
	else if ( time_info.hour < 19 ) weather_info.sunlight = SUN_LIGHT;
	else if ( time_info.hour < 20 ) weather_info.sunlight = SUN_SET;
	else if ( time_info.hour < 24 ) weather_info.sunlight = SUN_DARK;
	else                            weather_info.sunlight = MOON_RISE;

	weather_info.change	= 0;
	weather_info.mmhg	= 960;
	if ( time_info.month >= 7 && time_info.month <=12 )
	    weather_info.mmhg += number_range( 1, 50 );
	else
	    weather_info.mmhg += number_range( 1, 80 );

	     if ( weather_info.mmhg <=  980 ) weather_info.sky = SKY_LIGHTNING;
	else if ( weather_info.mmhg <= 1000 ) weather_info.sky = SKY_RAINING;
	else if ( weather_info.mmhg <= 1020 ) weather_info.sky = SKY_CLOUDY;
	else                                  weather_info.sky = SKY_CLOUDLESS;

        weather_info.moonphase = MOON_NEW;
        weather_info.moonday   = 0;
    }

    /*
     * Assign gsn's for skills which have them.
     * Init skills_table class levels.
     */
    // also initialize the spells_table - Veygoth
    //
    // Assign's values for races too.
    //
    log_string( "Assigning gsn's" );
    {
	int sn;
	int class;

	for ( sn = 0; sn < MAX_SKILL; sn++ )
	{
	    if ( skills_table[sn].pgsn )
		*skills_table[sn].pgsn = sn;

	    for ( class = 0; class < MAX_CLASS; class++ )
		skills_table[sn].skill_level[class] = L_APP;
	}

	for ( sn = 0; sn < MAX_SPELL; sn++ )
	{
	    if ( spells_table[sn].pgsn )
		*spells_table[sn].pgsn = sn;

	    for ( class = 0; class < MAX_CLASS; class++ )
		spells_table[sn].spell_circle[class] = MAX_CIRCLE+3;//Govno
	}
        //Initialize race gsns  - Lithos
        for ( sn = 0; sn < MAX_RACE; sn++) {
            if ( race_table[sn].race_gsn )
                *race_table[sn].race_gsn = sn;
        }


	RACE_ELEM_FIRE = race_lookup( "Fire Elemental");
	RACE_ELEM_AIR = race_lookup( "Air Elemental" );
	RACE_ELEM_WATER = race_lookup( "Water Elemental" );
	RACE_ELEM_EARTH = race_lookup( "Earth Elemental" );
	RACE_UNDEAD = race_lookup( "Undead" );
	RACE_GHOST = race_lookup( "Ghost" );
	RACE_VAMPIRE = race_lookup( "Vampire" );
        RACE_GOD = race_lookup( "God" );
        RACE_DEMON = race_lookup( "Demon" );
        RACE_DEVIL = race_lookup( "Devil" );
        RACE_DRACOLICH = race_lookup( "Dracolich" );
    }

    load_sysdata( );

    log_string( "Loading socials" );
    load_socials( );

    log_string( "Loading classes" );
    load_classes( );

    /*
     * Read in all the area files.
     */
    log_string( "Reading in area files..." );
    {
	FILE      *fpList;
	char       strsave [ MAX_INPUT_LENGTH ];
	sprintf( strsave, "%s%s%s", executable_directory, AREA_DIR, AREA_LIST );

	if ( !( fpList = fopen( strsave, "r" ) ) )
	{
	    perror( AREA_LIST );
	    exit( 1 );
	}

	for ( ; ; )
	{
	    int stat;

	    strcpy( strArea, fread_word( fpList, &stat ) );
	    if ( strArea[0] == '$' )
		break;

	    load_area_file( );

	}
	fclose( fpList );
    }

    /*
     * Fix up exits.
     * Declare db booting over.
     * Reset all areas once.
     * Load up the notes file.
     */
    {
	log_string( "Fixing exits" );
	fix_exits( );
	fBootDb  = FALSE;
	daPrompt = str_dup( 
         "&+g<%hhp %mm %vmv>\n\r<&n%T %E&+g>&n " );
	boot_done( );
	log_string( "Resetting areas" );
	area_update( );
        log_string( "Loading notes" );
	load_notes( );
        log_string( "Loading bans" );
	load_ban( );
        log_string( "Loading illegal names" );
        load_names( );
	load_clans( );
        log_string( "Loading down time" );
	load_down_time( );
        load_corpses( );
        //load_crimes( );
        //reinitialize_fraglist();
        load_fraglist( );
	MOBtrigger = TRUE;
    }

    log_string( "Creating events." );
    create_event( EVENT_SAVE_CORPSES, PULSE_SAVE_CORPSES, NULL, NULL, 0 );
    create_event( EVENT_SAVE_SYSDATA, PULSE_SAVE_SYSDATA, NULL, NULL, 0 );
    create_event( EVENT_VIOLENCE_UPDATE, PULSE_VIOLENCE_UP, NULL, NULL, 0 );
    create_event( EVENT_AREA_UPDATE, PULSE_AREA, NULL, NULL, 0 );
    create_event( EVENT_ROOM_UPDATE, PULSE_ROOM, NULL, NULL, 0 );
    create_event( EVENT_OBJECT_SPECIAL, PULSE_OBJECT, NULL, NULL, 0 );
    create_event( EVENT_MOBILE_UPDATE, PULSE_MOBILE, NULL, NULL, 0 );
    create_event( EVENT_WEATHER_UPDATE, PULSE_WEATHER, NULL, NULL, 0 );
    create_event( EVENT_CHAR_UPDATE, PULSE_CHAR_UPDATE, NULL, NULL, 0 );
    create_event( EVENT_OBJECT_UPDATE, PULSE_OBJ_UPDATE, NULL, NULL, 0 );
    create_event( EVENT_LIST_UPDATE, PULSE_LIST, NULL, NULL, 0 );
    create_event( EVENT_AGGR_UPDATE, PULSE_AGGRESS, NULL, NULL, 0 );
    create_event( EVENT_MEMORIZE_UPDATE, PULSE_MEMORIZE, NULL, NULL, 0 );
    create_event( EVENT_HIT_GAIN, PULSE_HITGAIN, NULL, NULL, 0 );
    create_event( EVENT_MANA_GAIN, PULSE_MANAGAIN, NULL, NULL, 0 );
    create_event( EVENT_MOVE_GAIN, PULSE_MOVEGAIN, NULL, NULL, 0 );
    create_event( EVENT_HEARTBEAT, PULSE_WEATHER, NULL, NULL, 0 );

    return;
}



/*
 * Snarf an 'area' header line.
 */
void load_area_file( )
{
    int		stat;
    char	buf [ MAX_STRING_LENGTH ];

    last_fun( "load_area_file");
    if ( !fBootDb )
    {
        bug( "Load_area: can't load area if not booting!", 0 );
        return;
    }

    if ( strArea[0] == '-' )
    {
	fpArea = stdin;
	strcpy ( buf, &strArea[1] );
	strcpy ( strArea, buf );
    }
    else
    {
	char strsave [ MAX_INPUT_LENGTH ]; 

	sprintf( strsave, "%s%s%s", executable_directory, AREA_DIR, strArea );

	if ( !( fpArea = fopen( strsave, "r" ) ) )
	{
	    perror( strArea );
	    exit( 1 );
	}
    }

    for ( ; ; )
    {
	char *word;

	if ( fread_letter( fpArea ) != '#' )
	{
	    bug( "Load_area: # not found.", 0 );
	    exit( 1 );
	}

	word = fread_word( fpArea, &stat );


	if ( word[0] == '$'               )
	    break;
	else if ( !str_cmp( word, "AREA"     ) )
	    load_area    ( fpArea );
	else if ( !str_cmp( word, "HELPS"    ) ) 
	    load_helps   ( fpArea );
	else if ( !str_cmp( word, "RECALL"   ) )
	    load_recall  ( fpArea );
	else if ( !str_cmp( word, "MAGMAMOBS"  ) )
	    load_magma_mobiles ( fpArea );
    else if ( !str_cmp( word, "MOBILES" ))
        load_envy_mobiles( fpArea );
	else if ( !str_cmp( word, "MOBPROGS" ) ) 
	    load_mobprogs( fpArea );
        else if ( !str_cmp( word, "OBJPROGS" ) )
            load_objprogs( fpArea );
	else if ( !str_cmp( word, "RESETS"   ) )
	    load_resets  ( fpArea );
	else if ( !str_cmp( word, "MAGMARESETS"   ) )
	    load_magma_resets  ( fpArea );
	else if ( !str_cmp( word, "MAGMASHOPS"    ) )
	    load_magma_shops   ( fpArea );
    else if( !str_cmp( word, "SHOPS" ))
        load_envy_shops( fpArea );
	else if ( !str_cmp( word, "SPECIALS" ) )
	    load_specials( fpArea );
	else if ( !str_cmp( word, "GAMES"    ) )
	    load_games( fpArea );
	else if ( !str_cmp( word, "AREADATA" ) )
	    new_load_area( fpArea );
        else if( !str_cmp( word, "MAGMAROOMS" ) )
            load_magma_rooms( fpArea );
        else if( !str_cmp( word, "ROOMS" ))
            load_envy_rooms( fpArea );
        else if ( !str_cmp( word, "MAGMAOBJECTS" ) )
            load_magma_objects( fpArea );
        else if ( !str_cmp( word, "OBJECTS" ))
            load_envy_objects( fpArea );
        else if ( !str_cmp( word, "QUESTS" ) )
            load_quests( fpArea );
        else if( !str_cmp( word, "SHIPS" ) )
            load_vehicles( fpArea );
	else
	{
        char name[2048];
        sprintf( name, "Load_area: bad section name '%s'.", word );
	    bug( name, 0 );
	    exit( 1 );
	}
    }

    if ( fpArea != stdin )
	fclose( fpArea );
    fpArea = NULL;

    if ( area_last )
    {

	fprintf( stderr, "%-14s: Rooms: %5d - %-5d Objs: %5d - %-5d Mobs: %5d - %d\n",
		area_last->filename,
		area_last->low_r_vnum, area_last->hi_r_vnum,
		area_last->low_o_vnum, area_last->hi_o_vnum,
		area_last->low_m_vnum, area_last->hi_m_vnum );
    }
    else
    {
	fprintf( stderr, "(%s)\n", strArea );
    }

    return;
}



/*
 * Snarf an 'area' header line.
 */
void load_area( FILE *fp )
{
    AREA_DATA *pArea;
    char       author [ MAX_STRING_LENGTH ];
    char       range  [ MAX_STRING_LENGTH ];
    char       name   [ MAX_STRING_LENGTH ];
    char       arg1   [ MAX_STRING_LENGTH ];
    char       arg2   [ MAX_STRING_LENGTH ];
    char       buf    [ MAX_STRING_LENGTH ];
    char      *ptr;

    last_fun( "load_area");
    pArea               = alloc_perm( sizeof( *pArea ) );       /*
    pArea->reset_first  = NULL;                                  * OLC-Removed
    pArea->reset_last   = NULL;                                  */

    /*
     * Ugly hack to convert well behaved areas to new format.  -Zen
     */
    temp_fread_string( fp, buf );

    while ( ( ptr = strchr( buf, '{' ) ) )
	*ptr = '"';

    while ( ( ptr = strchr( buf, '}' ) ) )
	*ptr = '"';

    ptr = &buf[0];

    ptr = one_argument( ptr, range	);
    ptr = one_argument( ptr, author	);

    strcpy( name, ptr );

    ptr = &range[0];

    ptr = one_argument( ptr, arg1	);
    ptr = one_argument( ptr, arg2	);

    if ( is_number( arg1 ) && is_number( arg2 ) )
    {
	pArea->llv		= atoi( arg1 );
	pArea->ulv		= atoi( arg2 );
    }
    else
    if ( !str_cmp( arg1, "All" ) )
    {
	pArea->llv		= 0;
	pArea->ulv		= MAX_LEVEL;
    }
    else
    {
	pArea->llv		= 0;
	pArea->ulv		= 0;
    }

    pArea->name		= str_dup( name );
    pArea->author	= str_dup( capitalize( author ) );
    pArea->resetmsg	= str_dup( "You hear the patter of little feet." );

    pArea->recall       = ROOM_VNUM_TEMPLE;
    pArea->area_flags   = AREA_LOADING;
    pArea->security     = 1;
    pArea->builders     = str_dup( "None" );
    pArea->lvnum        = 0;
    pArea->uvnum        = 0;
    pArea->vnum         = top_area;
    pArea->filename     = str_dup( strArea );
    pArea->age		= 15;
    pArea->nplayer	= 0;
    pArea->justice      = 0;
    pArea->low_r_vnum	= 0;
    pArea->hi_r_vnum	= 0;
    pArea->low_o_vnum	= 0;
    pArea->hi_o_vnum	= 0;
    pArea->low_m_vnum	= 0;
    pArea->hi_m_vnum	= 0;
    
    if ( !area_first )
	area_first = pArea;
    if (  area_last  )
    {
	area_last->next = pArea;
        REMOVE_BIT( area_last->area_flags, AREA_LOADING );
    }
    area_last	= pArea;
    pArea->next	= NULL;

    top_area++;
    return;
}



/* 
 * OLC
 * Snarf an 'area' header line.   Check this format.  MUCH better.  Add fields
 * too.
 *
 * #AREAFILE
 * Name   -Newbie School~
 * Author Locke~
 * Levels 1 54
 * Recall 3001
 * Reset You hear hitting a pencil on paper.~
 * End
 *
 * The first character of the name ( '-' or '*' ) signs, that the area
 * is taken from the original (-) or has been added by wizards (*).
 * 
 * -Sektor
 */
void fread_area( FILE *fp, AREA_DATA *pArea )
{
    char        *word;
    char        buf [ MAX_STRING_LENGTH ];
    int         i;
    int         j;
    int         error_count = 0;
    int         status;
    char        *p;
    int         tmpi;
    int         num_keys;
    int         last_key = 0;

    char	def_author [] = "Envy";
    char	def_reset  [] = "You hear the patter of little feet.";

    struct key_data key_tab [] = { 
      { "Name",     TRUE,  MAND,		{ &pArea->name,       NULL } },
      { "Author",   TRUE,  (unlong) &def_author,{ &pArea->author,     NULL } },
      { "Levels",   FALSE, 9999,		{ &pArea->llv,
						  &pArea->ulv,        NULL } },
      { "Security", FALSE, MAND,		{ &pArea->security,   NULL } },
      { "VNUMs",    FALSE, MAND,		{ &pArea->lvnum,
						  &pArea->uvnum,      NULL } },
      { "Builders", TRUE,  MAND,		{ &pArea->builders,   NULL } },
      { "Justice",  FALSE, 0,                   { &pArea->justice,    NULL } },
      { "Judge",    FALSE, 0,                   { &pArea->judge,      NULL } },
      { "Jail",     FALSE, 0,                   { &pArea->jail,       NULL } },
      { "Squads",   FALSE, 0,                   { &pArea->squads,     NULL } },
      { "Barracks", FALSE, 0,                   { &pArea->barracks,   NULL } },
      { "Defender", FALSE, 0,                   { &pArea->defender, &pArea->defender_num,   NULL } },
      { "Recall",   FALSE, ROOM_VNUM_TEMPLE,	{ &pArea->recall,     NULL } },
      { "Reset",    TRUE,  (unlong) &def_reset,	{ &pArea->resetmsg,   NULL } },
      { "\0",       FALSE, 0                                               } };

    last_fun( "fread_area");
    pArea->squads = 5;

    for ( num_keys = 0; *key_tab [num_keys].key; )
        num_keys++;

    for ( ; !feof (fp) ; )
    {

        word = fread_word( fp, &status );

        if ( !word )
	{
            bug( "fread_area:  Error reading key.  EOF?", 0 );
            fread_to_eol( fp );
            break;
	}

                /* This little diddy searches for the keyword
                   from the last keyword found */

        for ( i = last_key;
              i < last_key + num_keys &&
                str_cmp (key_tab [i % num_keys].key, word); )
            i++;

        i = i % num_keys;

        if ( !str_cmp (key_tab [i].key, word) )
            last_key = i;
        else
            i = num_keys;

        if ( *key_tab [i].key )         /* Key entry found in key_tab */
	{
            if ( key_tab [i].string == SPECIFIED )
                bug ("Key already specified.", 0);

                        /* Entry is a string */

            else
	      if ( key_tab [i].string )
	      {
                  if ( ( p = fread_string( fp, &status ) ) && !status )
		  {
		      free_string ( *(char **)key_tab [i].ptrs [0] );
		      *(char **)key_tab [i].ptrs [0] = p;
		  }
	      }

                        /* Entry is an integer */
            else
                for ( j = 0; key_tab [i].ptrs [j]; j++ )
		{
                    tmpi = fread_number ( fp, &status );
                    if ( !status )
                        *(int *)key_tab [i].ptrs [j] = tmpi;
		}

            if ( status )
	    {
                fread_to_eol( fp );
                continue;
	    }
	    else
                key_tab [i].string = SPECIFIED;
	}

        else if ( !str_cmp( word, "End" ) )
            break;

        else
	{
	    sprintf( buf, "fread_area: Unknown key '%s' in file.", word );
	    bug( buf, 0 );
	    fread_to_eol( fp );
	}
	
    }

                /* Require all manditory fields, set defaults */

    for ( i = 0; *key_tab [i].key; i++ )
    {

        if ( key_tab [i].string == SPECIFIED ||
             key_tab [i].deflt == DEFLT )
            continue;

        if ( key_tab [i].deflt == MAND )
	{
            sprintf( buf, "Manditory field '%s' missing from file.",
                          key_tab [i].key );
            bug( buf, 0 );
            error_count++;
            continue;
	}

               /* This if/else sets default strings and numbers */

        if ( key_tab [i].string && key_tab [i].deflt )
	{
	    free_string( *(char **)key_tab [i].ptrs [0] );
            *(char **)key_tab [i].ptrs [0] =
	      str_dup( (char *)key_tab [i].deflt );
	}
        else
            for ( j = 0; key_tab [i].ptrs [j]; j++ )
	        *(int *)key_tab [i].ptrs [j] = key_tab [i].deflt;
    }

    if ( pArea->llv == 9999 )
	pArea->llv = 0;

    if ( pArea->ulv == 9999 )
	pArea->ulv = MAX_LEVEL;

    pArea->times_reset = 0;
    pArea->llifespan = 15;
    pArea->ulifespan = 15;

    return;
}

void new_load_area( FILE *fp )
{
    AREA_DATA *pArea;

    last_fun( "new_load_area");
    pArea		= alloc_perm( sizeof( *pArea ) );
    pArea->recall       = ROOM_VNUM_TEMPLE;
    pArea->area_flags   = AREA_LOADING;
    pArea->security     = 1;
    pArea->builders     = str_dup( "None" );
    pArea->lvnum        = 0;
    pArea->uvnum        = 0;
    pArea->vnum         = top_area;
    pArea->filename     = str_dup( strArea );
    pArea->age		= 15;
    pArea->nplayer	= 0;
    pArea->justice      = 0;
    pArea->low_r_vnum	= 0;
    pArea->hi_r_vnum	= 0;
    pArea->low_o_vnum	= 0;
    pArea->hi_o_vnum	= 0;
    pArea->low_m_vnum	= 0;
    pArea->hi_m_vnum	= 0;

    fread_area( fp, pArea );

    if ( !area_first )
	area_first = pArea;
    if (  area_last  )
    {
	area_last->next = pArea;
        REMOVE_BIT( area_last->area_flags, AREA_LOADING );
    }
    area_last	= pArea;
    pArea->next	= NULL;

    top_area++;
    return;
}


/*
 * Sets vnum range for area using OLC protection features.
 */
void assign_area_vnum( int vnum )
{
    last_fun( "assign_area_vnum");
    if ( area_last->lvnum == 0 || area_last->uvnum == 0 )
        area_last->lvnum = area_last->uvnum = vnum;
    if ( vnum != URANGE( area_last->lvnum, vnum, area_last->uvnum ) )
    {
        if ( vnum < area_last->lvnum )
            area_last->lvnum = vnum;
        else
            area_last->uvnum = vnum;
    }
    return;
}



/*
 * Snarf a help section.
 */
void load_helps( FILE *fp )
{
    HELP_DATA *pHelp;
    char      *keyword;
    int        level;
    int        stat;

    last_fun( "load_helps");
    for ( ; ; )
    {
        level           = fread_number( fp, &stat );
	keyword         = fread_string( fp, &stat );
	if ( keyword[0] == '$' )
	    break;

	pHelp		= alloc_perm( sizeof( *pHelp ) );

	pHelp->level	= level;
	pHelp->keyword	= keyword;
	pHelp->text	= fread_string( fp, &stat );
        pHelp->area     = area_last ? area_last : NULL;

	if ( !str_cmp( pHelp->keyword, "_greeting_color" ) )
	    help_greeting_color = pHelp->text;
	else if ( !str_cmp( pHelp->keyword, "_greeting_ascii" ) )
	    help_greeting = pHelp->text;
        else if( !str_cmp( pHelp->keyword, "_races_color" ))
            help_races_color = pHelp->text;
        else if( !str_cmp( pHelp->keyword, "_races_nocolor" ))
            help_races_nocolor = pHelp->text;

	if ( !help_first )
	    help_first = pHelp;
	if (  help_last  )
	    help_last->next = pHelp;


	help_last	= pHelp;
	pHelp->next	= NULL;
	top_help++;
    }

    return;
}



/*
 * Snarf a recall point.
 */
void load_recall( FILE *fp )
{
    AREA_DATA *pArea;
    char       buf [ MAX_STRING_LENGTH ];
    int        stat;

    last_fun( "load_recall");
    pArea         = area_last;
    pArea->recall = fread_number( fp, &stat );

    if ( pArea->recall < 1 )
    {
        sprintf( buf, "Load_recall:  %s invalid recall point", pArea->name );
	bug( buf, 0 );
	pArea->recall = ROOM_VNUM_TEMPLE;
    }

    return;

}

MOB_FUN *get_breath_type( char *name )
{
    if( !str_infix( "br_f", name ) )
        return spec_mob_lookup( "spec_breath_fire" );
    if( !str_infix( "br_a", name ) )
        return spec_mob_lookup( "spec_breath_acid" );
    if( !str_infix( "br_c", name ) )
        return spec_mob_lookup( "spec_breath_frost" );
    if( !str_infix( "br_g", name ) )
        return spec_mob_lookup( "spec_breath_gas" );
    if( !str_infix( "br_l", name ) )
        return spec_mob_lookup( "spec_breath_lightning" );
    if ( !str_infix ( "br_w", name ) )
        return spec_mob_lookup( "spec_breath_water" );
    /* THis needs to be fixed when shadow breath is addeD! */
    if ( !str_infix ( "br_s", name ) )
        return spec_mob_lookup( "spec_breath_fire" );

    return spec_mob_lookup( "spec_breath_any" );
}

/*
 * Load a Magma format mob section - Veygoth.
 */
void load_magma_mobiles( FILE *fp )
{
    MOB_INDEX_DATA *pMobIndex;
    char buf[MAX_STRING_LENGTH]; // better error messages...
    int count;

    last_fun( "load_magma_mobiles");
    if ( !area_last )				/* OLC */
    {
        bug( "Load_mobiles: no #AREA seen yet.", 0 );
        exit( 1 );
    }

    for ( ; ; )
    {
	char *race;
	char  letter;
	int   vnum;
	int   iHash;
	int   stat;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_mobiles: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp, &stat );
	if ( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if ( get_mob_index( vnum ) )
	{
	    bug( "Load_mobiles: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pMobIndex			= alloc_perm( sizeof( *pMobIndex ) );
	pMobIndex->vnum			= vnum;

	if ( !area_last->low_m_vnum )
	    area_last->low_m_vnum       = vnum;
	if ( vnum > area_last->hi_m_vnum )
	    area_last->hi_m_vnum        = vnum;

        pMobIndex->area                 = area_last;
	pMobIndex->player_name		= fread_string( fp, &stat );
	pMobIndex->short_descr		= fread_string( fp, &stat );
	pMobIndex->long_descr		= fread_string( fp, &stat );
	pMobIndex->description		= fread_string( fp, &stat );

	pMobIndex->long_descr[0]	= UPPER( pMobIndex->long_descr[0]  );
	pMobIndex->description[0]	= UPPER( pMobIndex->description[0] );

	pMobIndex->act			= fread_number( fp, &stat ) | ACT_IS_NPC;
	pMobIndex->affected_by[0]	= fread_number( fp, &stat );
    pMobIndex->affected_by[1]       = fread_number( fp, &stat );
    pMobIndex->affected_by[2]       = fread_number( fp, &stat );
    pMobIndex->affected_by[3]       = fread_number( fp, &stat );
    // Set all other vectors to zero since this file format only has
    // two vectors - Veygoth
    if( NUM_AFFECT_VECTORS > 2 )
    {
       for( count = 2; count < NUM_AFFECT_VECTORS; count++ )
       {
            pMobIndex->affected_by[count] = 0;
       }
    }
	pMobIndex->pShop		= NULL;
	pMobIndex->alignment		= fread_number( fp, &stat );
	letter				= fread_letter( fp );

	if ( letter != 'S' )
	{
	    bug( "Load_mobiles: vnum %d non-S.", vnum );
	    exit( 1 );
	}

        race                            = fread_word( fp, &stat );

	pMobIndex->race = race_key_lookup( race );
	if ( pMobIndex->race < 0 )
	{
	    sprintf( buf, "Load_mobiles: vnum %d bad race key %s", vnum,
                     race );
            bug( buf, 0 );
	    pMobIndex->race = 0;
	}

        /* hometown not used */           fread_number( fp, &stat );

        pMobIndex->class                = fread_number( fp, &stat );
        // This class_convert will not be necessary if we add
        // classes in the same order as they are in DE.
        // It'll make us load a lil faster - Veygoth
        pMobIndex->class = class_convert( pMobIndex->class );

        switch( pMobIndex->class )
        {
            default:
                break;
            case CLASS_SHAMAN:
                pMobIndex->spec_fun = spec_mob_lookup( "spec_cast_shaman" );
                break;
            case CLASS_ELEMENT_AIR:
                pMobIndex->spec_fun = spec_mob_lookup( "spec_cast_air_ele" );
                break;
            case CLASS_ELEMENT_EARTH:
                pMobIndex->spec_fun = spec_mob_lookup( "spec_cast_earth_ele" );
                break;
            case CLASS_ELEMENT_FIRE:
                pMobIndex->spec_fun = spec_mob_lookup( "spec_cast_fire_ele" );
                break;
            case CLASS_ELEMENT_WATER:
                pMobIndex->spec_fun = spec_mob_lookup( "spec_cast_water_ele" );
                break;
            case CLASS_DRUID:
                pMobIndex->spec_fun = spec_mob_lookup( "spec_cast_druid" );
                break;
            case CLASS_RANGER:
                pMobIndex->spec_fun = spec_mob_lookup( "spec_cast_ranger" );
                break;
            case CLASS_HUNTER:
                pMobIndex->spec_fun = spec_mob_lookup( "spec_cast_hunter" );
                break;
            case CLASS_WARRIOR:
                pMobIndex->spec_fun = spec_mob_lookup( "spec_warrior" );
                break;
            case CLASS_BARD:
                pMobIndex->spec_fun = spec_mob_lookup( "spec_bard" );
                break;
            case CLASS_ILLUSIONIST:
                pMobIndex->spec_fun = spec_mob_lookup( "spec_cast_illusionist" );
                break;
            case CLASS_PALADIN:
                pMobIndex->spec_fun = spec_mob_lookup( "spec_cast_paladin" );
                break;
            case CLASS_ANTIPALADIN:
                pMobIndex->spec_fun = spec_mob_lookup( "spec_cast_antipaladin" );
                break;
            case CLASS_THIEF:
                pMobIndex->spec_fun = spec_mob_lookup( "spec_thief" );
                break;
            case CLASS_MERCENARY:
                pMobIndex->spec_fun = spec_mob_lookup( "spec_mercenary" );
                break;
            case CLASS_ASSASSIN:
                pMobIndex->spec_fun = spec_mob_lookup( "spec_assassin" );
                break;
            case CLASS_CLERIC:
                pMobIndex->spec_fun = spec_mob_lookup( "spec_cast_cleric" );
                break;
            case CLASS_SORCERER:
                pMobIndex->spec_fun = spec_mob_lookup( "spec_cast_sorcerer" );
                break;
            case CLASS_PSIONICIST:
                pMobIndex->spec_fun = spec_mob_lookup( "spec_cast_psionicist" );
                break;
        };

        // Assign race-based special functions.  Race functions take precedence over class
        // functions -- Veygoth
        if( pMobIndex->race == RACE_DRAGON )
        {
            if( !pMobIndex->spec_fun )
            {
                pMobIndex->spec_fun = get_breath_type( pMobIndex->player_name );
            }
            else
            {
                pMobIndex->spec_fun2 = pMobIndex->spec_fun;
                pMobIndex->spec_fun = get_breath_type( pMobIndex->player_name );
            }      
        }
        else if( pMobIndex->race == RACE_VAMPIRE )
        {
                if( !pMobIndex->spec_fun )
                {
                  pMobIndex->spec_fun = spec_mob_lookup( "spec_cast_vampire" );
                }
                else
                {
                  pMobIndex->spec_fun2 = pMobIndex->spec_fun;
                  pMobIndex->spec_fun = spec_mob_lookup( "spec_cast_vampire" );
                }      
        }

	pMobIndex->level		= fread_number( fp, &stat );
        /* Took number_fuzzy out of above line to store mob levels
           at a constant due to OLC resaving areas  --Stoked */

	fread_number( fp, &stat );	/* Unused */
	fread_number( fp, &stat );	/* Unused */
	fread_word( fp, &stat );	/* 1d1+1 Unused */
	fread_word( fp, &stat );	/* 1d1+1 Unused */
	fread_word( fp, &stat );	/* 0.0.0.0 Unused */
	fread_number( fp, &stat );	/* Unused */
	pMobIndex->position 		= fread_number( fp, &stat );
        fread_number( fp, &stat );
	pMobIndex->sex			= fread_number( fp, &stat );

	// Convert positions from DE
        switch( pMobIndex->position )
        {
            default:
            	pMobIndex->position = POS_STANDING;
		break;
            case 4:
                pMobIndex->position = POS_SLEEPING;
                break;
            case 0:
            case 1:
            case 2:
            case 3:
            case 5:
            	pMobIndex->position = POS_RESTING;
		break;
            case 6:
            	pMobIndex->position = POS_SITTING;
		break;
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            	pMobIndex->position = POS_STANDING;
		break;
            case 13:
            	pMobIndex->position = POS_RECLINING;
		break;
	}

        letter = fread_letter( fp );
        if ( letter == '>' )
        {
          bug( "Don't use the > for mobprograms - we want them in a separate file in the mobprog directory.", 0 );
          ungetc( letter, fp );
        }
        else ungetc( letter,fp );

	iHash			= vnum % MAX_KEY_HASH;
	pMobIndex->next		= mob_index_hash[iHash];
	mob_index_hash[iHash]	= pMobIndex;
	top_mob_index++;
        top_vnum_mob = top_vnum_mob < vnum ? vnum : top_vnum_mob;
        assign_area_vnum( vnum );
	kill_table[URANGE( 0, pMobIndex->level, MAX_LEVEL-1 )].number++;
    }

    return;
}

/*
 * Load a Magma format mob section - Veygoth.
 */
void load_envy_mobiles( FILE *fp )
{
    MOB_INDEX_DATA *pMobIndex;
    char buf[MAX_STRING_LENGTH]; // better error messages...
    int count;

    last_fun( "load_envy_mobiles");
    if ( !area_last )				/* OLC */
    {
        bug( "load_envy_mobiles: no #AREA seen yet.", 0 );
        exit( 1 );
    }

    for ( ; ; )
    {
	char *race;
	char  letter;
	int   vnum;
	int   iHash;
	int   stat;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "load_envy_mobiles: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp, &stat );
	if ( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if ( get_mob_index( vnum ) )
	{
	    bug( "load_envy_mobiles: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pMobIndex			= alloc_perm( sizeof( *pMobIndex ) );
	pMobIndex->vnum			= vnum;

	if ( !area_last->low_m_vnum )
	    area_last->low_m_vnum       = vnum;
	if ( vnum > area_last->hi_m_vnum )
	    area_last->hi_m_vnum        = vnum;

    pMobIndex->area             = area_last;
	pMobIndex->player_name		= fread_string( fp, &stat );
	pMobIndex->short_descr		= fread_string( fp, &stat );
	pMobIndex->long_descr		= fread_string( fp, &stat );
	pMobIndex->description		= fread_string( fp, &stat );

	pMobIndex->long_descr[0]	= UPPER( pMobIndex->long_descr[0]  );
	pMobIndex->description[0]	= UPPER( pMobIndex->description[0] );

	pMobIndex->act			    = fread_number( fp, &stat ) | ACT_IS_NPC;
    pMobIndex->act              = convert_envy_act_flags( pMobIndex->act );
    // This may require some translation to work with Magma affect bits.
	pMobIndex->affected_by[0]	= fread_number( fp, &stat );
    // Set all other vectors to zero since this file format only has
    // two vectors - Veygoth
    if( NUM_AFFECT_VECTORS > 1 )
    {
       for( count = 1; count < NUM_AFFECT_VECTORS; count++ )
       {
            pMobIndex->affected_by[count] = 0;
       }
    }
    // Convert flags to work with Magma.
    convert_envy_affect_flags( pMobIndex->affected_by );

	pMobIndex->pShop      = NULL;
	pMobIndex->alignment  = fread_number( fp, &stat );
	letter				  = fread_letter( fp );

	if ( letter != 'S' )
	{
	    bug( "load_envy_mobiles: vnum %d non-S.", vnum );
	    exit( 1 );
	}

        /* hometown not used            fread_number( fp, &stat );*/

	pMobIndex->level		= fread_number( fp, &stat );
        /* Took number_fuzzy out of above line to store mob levels
           at a constant due to OLC resaving areas  --Stoked */
 
	fread_number( fp, &stat );	/* Unused */
	fread_number( fp, &stat );	/* Unused */
    // Damage, coins, and experience points are calculated by the MUD.
	fread_to_eol( fp );	        /* 1d1+1 1d1+1 Unused */
	fread_word( fp, &stat );	/* Unused */
	fread_number( fp, &stat );	/* Unused */
	pMobIndex->position 		= fread_number( fp, &stat );


    race                   = fread_string( fp, &stat );

	pMobIndex->race = race_lookup( race );
	if ( pMobIndex->race < 0 )
	{
        // Second chance conversion of envy races.
        pMobIndex->race = envy_race_convert( race );
        if( pMobIndex->race < 0 )
        {
	        sprintf( buf, "load_envy_mobiles: vnum %d bad race key %s", vnum, race );
            bug( buf, 0 );
	        pMobIndex->race = 0;
        }
	}

	pMobIndex->sex			= fread_number( fp, &stat );

	    // Convert positions from Envy
        switch( pMobIndex->position )
        {
            default:
            	pMobIndex->position = POS_STANDING;
		        break;
            case 4:
                pMobIndex->position = POS_SLEEPING;
                break;
            case 0:
            case 1:
            case 2:
            case 3:
            case 5:
            	pMobIndex->position = POS_RESTING;
		        break;
            case 6:
            	pMobIndex->position = POS_STANDING;
		        break;
	}

        letter = fread_letter( fp );
        if ( letter == '>' )
        {
          bug( "Don't use the > for mobprograms - we want them in a separate file in the mobprog directory.", 0 );
          ungetc( letter, fp );
        }
        else ungetc( letter,fp );

	iHash			= vnum % MAX_KEY_HASH;
	pMobIndex->next		= mob_index_hash[iHash];
	mob_index_hash[iHash]	= pMobIndex;
	top_mob_index++;
        top_vnum_mob = top_vnum_mob < vnum ? vnum : top_vnum_mob;
        assign_area_vnum( vnum );
	kill_table[URANGE( 0, pMobIndex->level, MAX_LEVEL-1 )].number++;
    }

    return;
}

// This function is used to load objects stored in the Magma format.
// - Veygoth
void load_magma_objects( FILE *fp )
{
    int count;
    OBJ_INDEX_DATA *pObjIndex;
    char buf[MAX_STRING_LENGTH]; // better error messages...

    last_fun( "load_magma_objects");
    if ( !area_last )
    {
        bug( "Load_objects: no #AREA seen yet.", 0 );
        exit( 1 );
    }

    for ( ; ; )
    {
	char  letter;
	int   vnum;
	int   iHash;
	int   stat;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "Load_objects: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp, &stat );
	if ( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if ( get_obj_index( vnum ) )
	{
	    bug( "Load_objects: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pObjIndex			= alloc_perm( sizeof( *pObjIndex ) );
	pObjIndex->vnum			= vnum;

        for( count = 0; count < NUM_AFFECT_VECTORS; count++ )
        {
            pObjIndex->affected_by[count] = 0;
        }

	if ( !area_last->low_o_vnum )
	    area_last->low_o_vnum       = vnum;
	if ( vnum > area_last->hi_o_vnum )
	    area_last->hi_o_vnum        = vnum;

        pObjIndex->area                 = area_last;		/* OLC */
	pObjIndex->name			= fread_string( fp, &stat );
	pObjIndex->short_descr		= fread_string( fp, &stat );
	pObjIndex->description		= fread_string( fp, &stat );
	/* Action description */	  fread_string( fp, &stat );

	pObjIndex->short_descr[0]	= LOWER( pObjIndex->short_descr[0] );
	pObjIndex->description[0]	= UPPER( pObjIndex->description[0] );

	pObjIndex->item_type		= fread_number( fp, &stat );
        pObjIndex->material             = fread_number( fp, &stat );
        if( pObjIndex->material >= MAX_MATERIAL )
        {
            sprintf( buf, "Object %d has material type beyond maximum allowable value.", vnum );
            log_string( buf );
        }
        pObjIndex->size                 = fread_number( fp, &stat );
        pObjIndex->volume               = fread_number( fp, &stat );
        pObjIndex->craftsmanship        = fread_number( fp, &stat );
        // Damage resistance bonus not used, may not be - Veygoth
        /* damage resistance bonus */     fread_number( fp, &stat );
	pObjIndex->extra_flags[0]	= fread_number( fp, &stat );
	pObjIndex->wear_flags		= fread_number( fp, &stat );
	pObjIndex->extra_flags[1]	= fread_number( fp, &stat );
        pObjIndex->anti_flags           = fread_number( fp, &stat );
        pObjIndex->anti_flags2          = fread_number( fp, &stat );
	pObjIndex->value[0]             = fread_number( fp, &stat );
	pObjIndex->value[1]             = fread_number( fp, &stat );
	pObjIndex->value[2]             = fread_number( fp, &stat );
	pObjIndex->value[3]             = fread_number( fp, &stat );
	pObjIndex->value[4]             = fread_number( fp, &stat );
	pObjIndex->value[5]             = fread_number( fp, &stat );
	pObjIndex->value[6]             = fread_number( fp, &stat );
	pObjIndex->value[7]             = fread_number( fp, &stat );
	pObjIndex->weight		= fread_number( fp, &stat );
	pObjIndex->cost			= fread_number( fp, &stat );
        pObjIndex->trap                 = NULL;
        if( pObjIndex->cost <= 1 )
            set_cost( pObjIndex );
        pObjIndex->condition            = fread_number( fp, &stat );

        // convert area to Magma format - Veygoth
             if( pObjIndex->size == 3)
                 pObjIndex->size = 4;
        else if( pObjIndex->size == 4)
                 pObjIndex->size = 6;
        else if( pObjIndex->size == 5)
                 pObjIndex->size = 8;
        else if( pObjIndex->size == 6)
                 pObjIndex->size = 7;
        else if( pObjIndex->size == 7)
                 pObjIndex->size = 10;
        else if( pObjIndex->size == 8)
                 pObjIndex->size = 3;
        else if( pObjIndex->size == 9)
                 pObjIndex->size = 5;
        else if( pObjIndex->size == 10)
                 pObjIndex->size = 0;
        else if( pObjIndex->size == 0)
                 pObjIndex->size = 11;

	/*
	 * Check here for the redundancy of invisible light sources - Kahn
         *
         * Not bothering to check - Veygoth
	 */
        if( pObjIndex->item_type == TYPE_WEAPON && pObjIndex->value[3] == 0 )
            bug( "Object vnum %d: object type weapon with no damage type specified", vnum );
	    
	for ( ; ; )
	{
	    char letter;

	    letter = fread_letter( fp );

	    if ( letter == 'A' )
	    {
		AFFECT_DATA *paf;

		paf			= alloc_perm( sizeof( *paf ) );
		paf->skill		= -1;
		paf->spell		= -1;
		paf->song		= -1;
		paf->duration		= -1;
		paf->location		= fread_number( fp, &stat );
		paf->modifier		= fread_number( fp, &stat );
		set_bitvector( paf, AFF_NONE);
		paf->next		= pObjIndex->affected;
		pObjIndex->affected	= paf;
		top_affect++;
	    }

	    else if ( letter == 'E' )
	    {
		EXTRA_DESCR_DATA *ed;

		ed			= alloc_perm( sizeof( *ed ) );
		ed->keyword		= fread_string( fp, &stat );
		ed->description		= fread_string( fp, &stat );
		ed->next		= pObjIndex->extra_descr;
		pObjIndex->extra_descr	= ed;
		top_ed++;
	    }

            else if( letter == 'T' )
            {
                pObjIndex->trap = alloc_perm( sizeof( TRAP_DATA ));
                top_trap++;
                pObjIndex->trap->trigger = fread_number( fp, &stat );
                pObjIndex->trap->damage = fread_number( fp, &stat );
                if( pObjIndex->trap->damage >= MAX_TRAP )
                  bug( "Trap with damage type %d greater than max.", pObjIndex->trap->damage );
                pObjIndex->trap->charges = fread_number( fp, &stat );
                pObjIndex->trap->level = fread_number( fp, &stat );
                if( pObjIndex->trap->level > 100 )
                  bug( "Trap with level %d greater than 100.", pObjIndex->trap->level );
                letter = getc( fp );
		ungetc( letter, fp );
                if( is_number( &letter ))
                {
                  pObjIndex->trap->percent = fread_number( fp, &stat );
                }
                else
                  pObjIndex->trap->percent = 100;
            }

	    else if( letter == '#' )
	    {
		ungetc( letter, fp );
		break;
	    }
	    else
	    {
                // Currently ignoring affect bits on objects - Veygoth
		ungetc( letter, fp );
                pObjIndex->affected_by[0] = fread_number( fp, &stat );
                pObjIndex->affected_by[1] = fread_number( fp, &stat );
                pObjIndex->affected_by[2] = fread_number( fp, &stat );
                pObjIndex->affected_by[3] = fread_number( fp, &stat );
	    }
	}

	/*
	 * Translate character strings *value[] into integers:  sn's for
	 * items with spells, or straight conversion for other items.
	 * - Thelonius
	 */
	switch ( pObjIndex->item_type )
	{
	default:
	    break;

	case TYPE_PILL:
	case TYPE_POTION:
	case TYPE_SCROLL:
	  pObjIndex->value[1] = magma_spell_lookup( pObjIndex->value[1] );
	  pObjIndex->value[2] = magma_spell_lookup( pObjIndex->value[2] );
	  pObjIndex->value[3] = magma_spell_lookup( pObjIndex->value[3] );
	  pObjIndex->value[4] = magma_spell_lookup( pObjIndex->value[4] );
	    break;

	case TYPE_STAFF:
	case TYPE_WAND:
          pObjIndex->value[3] = magma_spell_lookup( pObjIndex->value[3] );
	    break;
	}

	iHash			= vnum % MAX_KEY_HASH;
	pObjIndex->next		= obj_index_hash[iHash];
	obj_index_hash[iHash]	= pObjIndex;

	top_obj_index++;
        top_vnum_obj = top_vnum_obj < vnum ? vnum : top_vnum_obj;
        assign_area_vnum( vnum );
    }

    return;
}

// This function is used to load objects stored in the Envy format.
// - Veygoth
void load_envy_objects( FILE *fp )
{
    int count;
    OBJ_INDEX_DATA *pObjIndex;

    last_fun( "load_envy_objects");
    if ( !area_last )
    {
        bug( "load_envy_objects: no #AREA seen yet.", 0 );
        exit( 1 );
    }

    for ( ; ; )
    {
	char  letter;
	int   vnum;
	int   iHash;
	int   stat;
    char * word;

	letter				= fread_letter( fp );
	if ( letter != '#' )
	{
	    bug( "load_envy_objects: # not found.", 0 );
	    exit( 1 );
	}

	vnum				= fread_number( fp, &stat );
	if ( vnum == 0 )
	    break;

	fBootDb = FALSE;
	if ( get_obj_index( vnum ) )
	{
	    bug( "load_envy_objects: vnum %d duplicated.", vnum );
	    exit( 1 );
	}
	fBootDb = TRUE;

	pObjIndex			= alloc_perm( sizeof( *pObjIndex ) );
	pObjIndex->vnum			= vnum;

        for( count = 0; count < NUM_AFFECT_VECTORS; count++ )
        {
            pObjIndex->affected_by[count] = 0;
        }

	if ( !area_last->low_o_vnum )
	    area_last->low_o_vnum       = vnum;
	if ( vnum > area_last->hi_o_vnum )
	    area_last->hi_o_vnum        = vnum;

        pObjIndex->area                 = area_last;		/* OLC */
	pObjIndex->name			= fread_string( fp, &stat );
	pObjIndex->short_descr		= fread_string( fp, &stat );
	pObjIndex->description		= fread_string( fp, &stat );
	/* Action description */	  fread_string( fp, &stat );

	pObjIndex->short_descr[0]	= LOWER( pObjIndex->short_descr[0] );
	pObjIndex->description[0]	= UPPER( pObjIndex->description[0] );

	pObjIndex->item_type		= convert_envy_item_type( fread_number( fp, &stat ) );
    pObjIndex->material             = 0;
    pObjIndex->size                 = 0;
    pObjIndex->volume               = 0;
    pObjIndex->craftsmanship        = 7;
	pObjIndex->extra_flags[0]	= fread_number( fp, &stat );
	pObjIndex->wear_flags		= fread_number( fp, &stat );
	pObjIndex->extra_flags[1]	= 0;
    pObjIndex->anti_flags           = 0;
    pObjIndex->anti_flags2          = 0;
    word = fread_string( fp, &stat );
    if( is_number( word ))
    {
        pObjIndex->value[0] = atoi(word);
    }
    else
    {
        pObjIndex->value[0] = spell_lookup(word);
    }
    word = fread_string( fp, &stat );
    if( is_number( word ))
    {
        pObjIndex->value[1] = atoi(word);
    }
    else
    {
        pObjIndex->value[1] = spell_lookup(word);
    }
    word = fread_string( fp, &stat );
    if( is_number( word ))
    {
        pObjIndex->value[2] = atoi(word);
    }
    else
    {
        pObjIndex->value[2] = spell_lookup(word);
    }
    word = fread_string( fp, &stat );
    if( is_number( word ))
    {
        pObjIndex->value[3] = atoi(word);
    }
    else
    {
        pObjIndex->value[3] = spell_lookup(word);
    }
	pObjIndex->value[4]             = 0;
	pObjIndex->value[5]             = 0;
	pObjIndex->value[6]             = 0;
	pObjIndex->value[7]             = 0;
	pObjIndex->weight		= fread_number( fp, &stat );
    // Ignore cost and set it ourselves.
    fread_number(fp,&stat);
    fread_number(fp,&stat);
    set_cost( pObjIndex );
    pObjIndex->trap                 = NULL;

	/*
	 * Check here for the redundancy of invisible light sources - Kahn
         *
         * Not bothering to check - Veygoth
	 */
        if( pObjIndex->item_type == TYPE_WEAPON && pObjIndex->value[3] == 0 )
            warning( "Object vnum %d: object type weapon with no damage type specified", vnum );
	    
	for ( ; ; )
	{
	    char letter;

	    letter = fread_letter( fp );

	    if ( letter == 'A' )
	    {
		AFFECT_DATA *paf;

		paf			= alloc_perm( sizeof( *paf ) );
		paf->skill		= -1;
		paf->spell		= -1;
		paf->song		= -1;
		paf->duration		= -1;
		paf->location		= fread_number( fp, &stat );
        // APPLY_RACE is 55 here, not 25.
        if( paf->location == 25 )
            paf->location = 55;
		paf->modifier		= fread_number( fp, &stat );
		set_bitvector( paf, AFF_NONE);
		paf->next		= pObjIndex->affected;
		pObjIndex->affected	= paf;
		top_affect++;
	    }

	    else if ( letter == 'E' )
	    {
		EXTRA_DESCR_DATA *ed;

		ed			= alloc_perm( sizeof( *ed ) );
		ed->keyword		= fread_string( fp, &stat );
		ed->description		= fread_string( fp, &stat );
		ed->next		= pObjIndex->extra_descr;
		pObjIndex->extra_descr	= ed;
		top_ed++;
	    }

            else if( letter == 'T' )
            {
                pObjIndex->trap = alloc_perm( sizeof( TRAP_DATA ));
                top_trap++;
                pObjIndex->trap->trigger = fread_number( fp, &stat );
                pObjIndex->trap->damage = fread_number( fp, &stat );
                if( pObjIndex->trap->damage >= MAX_TRAP )
                  bug( "Trap with damage type %d greater than max.", pObjIndex->trap->damage );
                pObjIndex->trap->charges = fread_number( fp, &stat );
                pObjIndex->trap->level = fread_number( fp, &stat );
                if( pObjIndex->trap->level > 100 )
                  bug( "Trap with level %d greater than 100.", pObjIndex->trap->level );
                letter = getc( fp );
		ungetc( letter, fp );
                if( is_number( &letter ))
                {
                  pObjIndex->trap->percent = fread_number( fp, &stat );
                }
                else
                  pObjIndex->trap->percent = 100;
            }

	    else if( letter == '#' )
	    {
		ungetc( letter, fp );
		break;
	    }
	    else
	    {
                // Currently ignoring affect bits on objects - Veygoth
		ungetc( letter, fp );
                pObjIndex->affected_by[0] = fread_number( fp, &stat );
                pObjIndex->affected_by[1] = fread_number( fp, &stat );
                pObjIndex->affected_by[2] = fread_number( fp, &stat );
                pObjIndex->affected_by[3] = fread_number( fp, &stat );
	    }
	}

    // No spell lookups needed -- we did a string-based lookup earlier.

	iHash			= vnum % MAX_KEY_HASH;
	pObjIndex->next		= obj_index_hash[iHash];
	obj_index_hash[iHash]	= pObjIndex;

	top_obj_index++;
        top_vnum_obj = top_vnum_obj < vnum ? vnum : top_vnum_obj;
        assign_area_vnum( vnum );
    }

    return;
}

// Veygoth - this function is supposed to reference the spell number
// used in DE to the spell used in Magma.  Currently it does not do
// anything but place an annoying message in the log file.
int magma_spell_lookup( int skill )
{
    last_fun( "magma_spell_lookup");
   switch ( skill )
   {
      default:
         bug( "Magic item with unrecognized spell %d disabled.", skill);
         return 0;
         break;
      case -1:
      case 0:
         return 0;
         break;
      case 1:
         return spl_armor;
         break;
      case 2:
         return spl_teleport;
         break;
      case 3:
         return spl_bless;
         break;
      case 4:
         return spl_blindness;
         break;
      case 5:
         return spl_burning_hands;
         break;
      case 7:
         return spl_charm_person;
         break;
      case 8:
         return spl_chill_touch;
         break;
      case 9:
         return spl_fullheal;
         break;
      case 10:
         return spl_cone_of_cold;
         break;
      case 14:
         return spl_cure_blind;
         break;
      case 15:
         return spl_cure_critical;
         break;
      case 16:
         return spl_cure_light;
         break;
      case 17:
         return spl_curse;
         break;
      case 18:
         return spl_continual_light;
         break;
      case 19:
         return spl_detect_invis;
         break;
      case 20:
         return spl_minor_creation;
         break;
      case 21:
         return spl_flamestrike;
         break;
      case 22:
         return spl_dispel_evil;
         break;
      case 23:
         return spl_earthquake;
         break;
      case 25:
         return spl_energy_drain;
         break;
      case 26:
         return spl_fireball;
         break;
      case 27:
         return spl_harm;
         break;
      case 28:
         return spl_heal;
         break;
      case 29:
         return spl_invis;
         break;
      case 30:
         return spl_lightning_bolt;
         break;
      case 31:
         return spl_locate_object;
         break;
      case 32:
         return spl_magic_missile;
         break;
      case 33:
         return spl_poison;
         break;
      case 34:
         return spl_prot_evil;
         break;
      case 35:
         return spl_remove_curse;
         break;
      case 36:
         return spl_stoneskin;
         break;
      case 37:
         return spl_shocking_grasp;
         break;
      case 38:
         return spl_sleep;
         break;
      case 39:
         return spl_giant_strength;
         break;
      case 41:
         return spl_haste;
         break;
      case 43:
         return spl_remove_poison;
         break;
      case 44:
         return spl_sense_life;
         break;
      case 45:
         return spl_identify;
         break;
      case 47:
         return spl_firestorm;
         break;
      case 54:
         return spl_fear;
         break;
      case 56:
         return spl_vitality;
         break;
      case 57:
         return spl_cure_serious;
         break;
      case 61:
         return spl_minor_globe;
         break;
      case 64:
         return spl_vig_light;
         break;
      case 65:
         return spl_vig_serious;
         break;
      case 66:
         return spl_vig_critical;
         break;
      case 71:
         return spl_enfeeblement;
         break;
      case 72:
         return spl_dispel_good;
         break;
      case 73:
         return spl_dexterity;
         break;
      case 76:
         return spl_cyclone;
         break;
      case 81:
         return spl_protection_good;
         break;
      case 83:
         return spl_levitation;
         break;
      case 84:
         return spl_fly;
         break;
      case 85:
         return spl_water_breathing;
         break;
      case 90:
         return spl_detect_evil;
         break;
      case 91:
         return spl_detect_good;
         break;
      case 92:
         return spl_detect_magic;
         break;
      case 93:
         return spl_dispel_magic;
         break;
      case 95:
         return spl_mass_invis;
         break;
      case 96:
         return spl_protect_fire;
         break;
      case 97:
         return spl_protect_cold;
         break;
      case 98:
         return spl_protect_lightning;
         break;
      case 99:
         return spl_darkness;
         break;
      case 100:
         return spl_minor_para;
         break;
      case 102:
         return spl_slowness;
         break;
      case 103:
         return spl_wither;
         break;
      case 104:
         return spl_protect_gas;
         break;
      case 105:
         return spl_protect_acid;
         break;
      case 106:
         return spl_infravision;
         break;
      case 107:
         return spl_prismatic_spray;
         break;
      case 108:
         return spl_flame_shield;
         break;
      case 109:
         return spl_color_spray;
         break;
      case 110:
         return spl_incendiary_cloud;
         break;
      case 111:
         return spl_ice_storm;
         break;
      case 112:
         return spl_disintegrate;
         break;
      case 113:
         return spl_cause_light;
         break;
      case 114:
         return spl_cause_serious;
         break;
      case 115:
         return spl_cause_critical;
         break;
      case 116:
         return spl_acid_blast;
         break;
      case 124:
         return spl_sunray;
         break;
      case 126:
         return spl_feeblemind;
         break;
      case 127:
         return spl_silence;
         break;
      case 128:
         return spl_turn_undead;
         break;
      case 131:
         return spl_frost_shield;
         break;
      case 141:
         return spl_barkskin;
         break;
      case 144:
         return spl_major_globe;
         break;
      case 156:
         return spl_agitation;
         break;
      case 157:
         return spl_adrenaline_control;
         break;
      case 160:
         return spl_ballistic_attack;
         break;
      case 163:
         return spl_combat_mind;
         break;
      case 170:
         return spl_domination;
         break;
      case 172:
         return spl_ego_whip;
         break;
      case 179:
         return spl_flesh_armor;
         break;
      case 180:
         return spl_inertial_barrier;
         break;
      case 181:
         return spl_inflict_pain;
         break;
      case 186:
         return spl_plague;
         break;
      case 188:
         return spl_soulshield;
         break;
      case 190:
         return spl_mass_heal;
         break;
      case 192:
         return spl_spirit_armor;
         break;
      case 197:
         return spl_pythonsting;
         break;
      case 201:
         return spl_panther;
         break;
      case 204:
         return spl_hawk;
         break;
      case 206:
         return spl_mending;
         break;
      case 212:
         return spl_malison;
         break;
      case 214:
         return spl_gr_mending;
         break;
      case 215:
         return spl_lion;
         break;
      case 221:
         return spl_elephant;
         break;
      case 223:
         return spl_scathing_wind;
         break;
      case 228:
         return spl_sustenance;
         break;
      case 234:
         return spl_ls_mending;
         break;
      case 236:
         return spl_true_seeing;
         break;
      case 238:
         return spl_ravenflight;
         break;
      case 248:
         return spl_enlarge;
         break;
      case 249:
         return spl_reduce;
         break;
      case 254:
         return spl_iceball;
         break;
      case 263:
         return spl_blur;
         break;
      case 275:
         return spl_cure_disease;
         break;
      
   }
}

/*
 * Adds a reset to a room.  OLC
 * Similar to add_reset in olc.c
 */
void new_reset( ROOM_INDEX_DATA *pR, RESET_DATA *pReset )
{
    RESET_DATA *pr;

    last_fun( "new_reset");
    if ( !pR )
       return;

    pr = pR->reset_last;

    if ( !pr )
    {
        pR->reset_first = pReset;
        pR->reset_last  = pReset;
    }
    else
    {
        pR->reset_last->next = pReset;
        pR->reset_last       = pReset;
        pR->reset_last->next = NULL;
    }

    top_reset++;
    return;
}


/*
 * Snarf a reset section.
 */
void load_resets( FILE *fp )
{
    RESET_DATA *pReset;
    int         iLastRoom = 0;
    int         iLastObj  = 0;
    int		 stat;

    last_fun( "load_resets");
    if ( !area_last )
    {
	bug( "Load_resets: no #AREA seen yet.", 0 );
	exit( 1 );
    }

    for ( ; ; )
    {
	EXIT_DATA       *pexit;
	ROOM_INDEX_DATA *pRoomIndex;
	char             letter;

	if ( ( letter = fread_letter( fp ) ) == 'S' )
	    break;

	if ( letter == '*' )
	{
	    fread_to_eol( fp );
	    continue;
	}

	pReset		= alloc_perm( sizeof( *pReset ) );
	pReset->command	= letter;
	/* if_flag */	  fread_number( fp, &stat );
	pReset->arg1	= fread_number( fp, &stat );
	pReset->arg2	= fread_number( fp, &stat );
	pReset->arg3	= ( letter == 'G' || letter == 'R' )
			    ? 0 : fread_number( fp, &stat );
			  fread_to_eol( fp );
	pReset->arg4	= 0;
	pReset->arg5	= 0;
	pReset->arg6	= 0;
	pReset->arg7	= 0;

	/*
	 * Validate parameters.
	 * We're calling the index functions for the side effect.
	 */
	switch ( letter )
	{
	default:
	    bug( "Load_resets: bad command '%c'.", letter );
	    exit( 1 );
	    break;

        case 'M':
            get_mob_index  ( pReset->arg1 );
            if ( ( pRoomIndex = get_room_index ( pReset->arg3 ) ) )
            {
                new_reset( pRoomIndex, pReset );
                iLastRoom = pReset->arg3;
            }
            break;

        case 'O':
            get_obj_index  ( pReset->arg1 );
            if ( ( pRoomIndex = get_room_index ( pReset->arg3 ) ) )
            {
                new_reset( pRoomIndex, pReset );
                iLastObj = pReset->arg3;
            }
            break;

        case 'P':
            get_obj_index  ( pReset->arg1 );
            if ( ( pRoomIndex = get_room_index ( iLastObj ) ) )
            {
                new_reset( pRoomIndex, pReset );
            }
            break;

        case 'G':
        case 'E':
            get_obj_index  ( pReset->arg1 );
            // WEAR_HOLD is now WEAR_HAND
            if( pReset->arg3 == 17 )
                pReset->arg3 = 16;
            if ( ( pRoomIndex = get_room_index ( iLastRoom ) ) )
            {
                new_reset( pRoomIndex, pReset );
                iLastObj = iLastRoom;
            }
            break;

        case 'D':
            pRoomIndex = get_room_index( pReset->arg1 );

            if( !pRoomIndex )
            {
                bug( "Load_resets: 'D': destination room vnum %d does not exist.", pReset->arg1 );
                break;
            }

            pexit = pRoomIndex->exit[pReset->arg2];

            if( !pexit )
            {
                bug( "Load_resets: 'D': exit_data does not exist for direction %d.", pReset->arg2 );
                break;
            }
            // We ignore this error for Envy zones.
            //if( !IS_SET( pexit->exit_info, EX_ISDOOR ))
            //{
            //    bug( "Load_resets: 'D': EX_ISDOOR flag is not set on exit_data->exit_info for destination %d", pReset->arg1 );
            //    break;
            //}

            if ( pReset->arg2 < 0 || pReset->arg2 >= MAX_DIR )
            {
                bug( "Load_resets: 'D': exit %d not door.", pReset->arg2 );
            }

            switch ( pReset->arg3 )
            {
                default:
                    bug( "Load_resets: 'D': bad 'locks': %d." , pReset->arg3);
                case 0:
                    break;
                case 1: SET_BIT( pexit->exit_info, EX_CLOSED );
                    break;
                case 2: SET_BIT( pexit->exit_info, EX_CLOSED | EX_LOCKED );
                    break;
                case 4: SET_BIT( pexit->exit_info, EX_SECRET );
                    break;
                case 5: SET_BIT( pexit->exit_info, EX_SECRET | EX_CLOSED );
                    break;
                case 6: SET_BIT( pexit->exit_info, EX_SECRET | EX_CLOSED | EX_LOCKED );
                    break;
                case 8: SET_BIT( pexit->exit_info, EX_BLOCKED );
                    break;
                case 9: SET_BIT( pexit->exit_info, EX_BLOCKED | EX_CLOSED );
                    break;
                case 10: SET_BIT( pexit->exit_info, EX_BLOCKED | EX_CLOSED | EX_LOCKED );
                    break;
                case 12: SET_BIT( pexit->exit_info, EX_BLOCKED | EX_SECRET );
                    break;
                case 13: SET_BIT( pexit->exit_info, EX_BLOCKED | EX_SECRET | EX_CLOSED );
                    break;
                case 14: SET_BIT( pexit->exit_info, EX_BLOCKED | EX_SECRET | EX_CLOSED | EX_LOCKED );
                    break;
            }
            break;

        case 'R':
            if ( pReset->arg2 < 0 || pReset->arg2 > MAX_DIR ) /* Last Door. */
            {
                bug( "Load_resets: 'R': bad exit %d.", pReset->arg2 );
                exit( 1 );
            }

            if ( ( pRoomIndex = get_room_index( pReset->arg1 ) ) )
                new_reset( pRoomIndex, pReset );

            break;
	}
    }

    return;
}

/*
 * Load a Magma style reset section - Veygoth.
 */
void load_magma_resets( FILE *fp )
{
    RESET_DATA *pReset;
    int         iLastRoom = 0;
    int         iLastObj  = 0;
    int		 stat;

    last_fun( "load_magma_resets");
    if ( !area_last )
    {
	bug( "Load_resets: no #AREA seen yet.", 0 );
	exit( 1 );
    }

    // Ignore all Magma area header stuff...
    fread_word( fp, &stat );
    fread_string( fp, &stat);
    fread_number( fp, &stat);    
    area_last->resetmode = fread_number( fp, &stat); // this is the area's reset mode    
    fread_number( fp, &stat);    
    area_last->llifespan = fread_number( fp, &stat) / 5; // lower range of reset time
    area_last->ulifespan = fread_number( fp, &stat) / 5;
    fread_number( fp, &stat);    

    for ( ; ; )
    {
	EXIT_DATA       *pexit;
	ROOM_INDEX_DATA *pRoomIndex;
	char             letter;

	if ( ( letter = fread_letter( fp ) ) == 'S' )
	    break;

	if ( letter == '*' )
	{
	    fread_to_eol( fp );
	    continue;
	}

	pReset		= alloc_perm( sizeof( *pReset ) );
	pReset->command	= letter;
	/* if_flag 	  fread_number( fp, &stat ); */
	pReset->arg0	= fread_number( fp, &stat );
	pReset->arg1	= fread_number( fp, &stat );
	pReset->arg2	= fread_number( fp, &stat );
	pReset->arg3	= fread_number( fp, &stat );
	pReset->arg4    = fread_number( fp, &stat );
	pReset->arg5	= fread_number( fp, &stat );
	pReset->arg6	= fread_number( fp, &stat );
	pReset->arg7	= fread_number( fp, &stat );
			  fread_to_eol( fp );

	/*
	 * Validate parameters.
	 * We're calling the index functions for the side effect.
	 */
	switch ( letter )
	{
	default:
	    bug( "Load_resets: bad command '%c'.", letter );
	    exit( 1 );
	    break;

        case 'M':
            get_mob_index  ( pReset->arg1 );
            if ( ( pRoomIndex = get_room_index ( pReset->arg3 ) ) )
            {
                new_reset( pRoomIndex, pReset );
                iLastRoom = pReset->arg3;
            }
            break;

        case 'F':
            get_mob_index  ( pReset->arg1 );
            if ( ( pRoomIndex = get_room_index ( pReset->arg3 ) ) )
            {
                new_reset( pRoomIndex, pReset );
                iLastRoom = pReset->arg3;
            }
            break;

        case 'O':
            get_obj_index  ( pReset->arg1 );
            if ( ( pRoomIndex = get_room_index ( pReset->arg3 ) ) )
            {
                new_reset( pRoomIndex, pReset );
                iLastObj = pReset->arg3;
            }
            break;

        case 'P':
            get_obj_index  ( pReset->arg1 );
            if ( ( pRoomIndex = get_room_index ( iLastObj ) ) )
            {
                new_reset( pRoomIndex, pReset );
            }
            break;

        case 'G':
        case 'E':
            get_obj_index  ( pReset->arg1 );
            // WEAR_HOLD is now WEAR_HAND
            if( pReset->arg3 == 17 )
                pReset->arg3 = 16;
            if ( ( pRoomIndex = get_room_index ( iLastRoom ) ) )
            {
                new_reset( pRoomIndex, pReset );
                iLastObj = iLastRoom;
            }
            break;

        case 'D':
            pRoomIndex = get_room_index( pReset->arg1 );

            if( !pRoomIndex )
            {
                bug( "load_magma_resets: 'D': destination room vnum %d does not exist.", pReset->arg1 );
            }
            else if( !(pexit = pRoomIndex->exit[pReset->arg2] ))
            {
                bug( "load_magma_resets: 'D': exit_data does not exist for direction %d.", pReset->arg2 );
            }
            else if (   pReset->arg2 < 0 || pReset->arg2 >= MAX_DIR
                || !IS_SET( pexit->exit_info, EX_ISDOOR ) )
            {
                bug( "load_magma_resets: 'D': exit %d not door.", pReset->arg2 );
            }
            break;

        case 'R':
	    if (pReset->arg0 == 1)
	    { // If this is a ridden mob ..
                get_mob_index  ( pReset->arg1 );
                if ( ( pRoomIndex = get_room_index ( pReset->arg3 ) ) )
                {
		    pReset->command = 'M';
		    pReset->arg0    = -1;
                    new_reset( pRoomIndex, pReset );
                    iLastRoom = pReset->arg3;
//		    bug (" Ridden mob %d should be loaded.", pReset->arg1);
                }
		else
		    bug ("Ridden mob not loaded: No room %d.", pReset->arg3);
	    }
	    else
	    { // Otherwise ..
                if ( pReset->arg2 < 0 || pReset->arg2 > MAX_DIR ) /* Last Door. */
            	{
                    bug( "Load_resets: 'R' bad exit %d.", pReset->arg2 );
                    exit( 1 );
            	}
                if ( ( pRoomIndex = get_room_index( pReset->arg1 ) ) )
                    new_reset( pRoomIndex, pReset );
	    }
            break;
	}
    }

    return;
}

// Loads Magma style room sections.  Only difference from 
// new_load_rooms is that new_load_rooms automatically converts
// the sector type from Envy zones to Magma zones - Veygoth
void load_magma_rooms( FILE *fp )
{
    ROOM_INDEX_DATA *pRoomIndex;
#ifdef HARDCORE_DEBUG
    ROOM_INDEX_DATA *room;
    int count;
#endif
    int iHash;
    last_fun( "load_magma_rooms");

    if ( !area_last )
    {
        bug( "Load_rooms: no #AREA seen yet.", 0 );
        exit( 1 );
    }

    for ( ; ; )
    {
        char letter;
        int  vnum;
        int  door;
        int  stat;

        letter                          = fread_letter( fp );
        if ( letter != '#' )
        {
            bug( "Load_rooms: # not found.", 0 );
            exit( 1 );
        }

        vnum                            = fread_number( fp, &stat );
        if ( vnum == 0 )
            break;

        fBootDb = FALSE;
        if ( get_room_index( vnum ) )
        {
            bug( "Load_rooms: vnum %d duplicated.", vnum );
            exit( 1 );
        }
        fBootDb = TRUE;

        pRoomIndex                      = alloc_perm( sizeof( *pRoomIndex ) );
        pRoomIndex->people              = NULL;
        pRoomIndex->contents            = NULL;
        pRoomIndex->extra_descr         = NULL;
        pRoomIndex->area                = area_last;
        pRoomIndex->vnum                = vnum;
#ifdef HARDCORE_DEBUG
        bug( "Found room with vnum %d.", pRoomIndex->vnum );
#endif
	if ( !area_last->low_r_vnum )
	    area_last->low_r_vnum       = vnum;
	if ( vnum > area_last->hi_r_vnum )
	    area_last->hi_r_vnum        = vnum;

        pRoomIndex->name                = fread_string( fp, &stat );
        pRoomIndex->description         = fread_string( fp, &stat );
        /* This value was originally area number, but is now used for terrain
           description generation with maps -- Veygoth  */
        pRoomIndex->extra_sector_info   = fread_number( fp, &stat );
        pRoomIndex->room_flags[0]       = fread_number( fp, &stat );
        pRoomIndex->room_flags[1]       = 0;
	pRoomIndex->orig_room_flags	= pRoomIndex->room_flags[0];      /* OLC */
        pRoomIndex->sector_type         = fread_number( fp, &stat );
        pRoomIndex->light               = 0;
        for ( door = 0; door < MAX_DIR; door++ )
            pRoomIndex->exit[door] = NULL;

        for ( ; ; )
        {
            letter = fread_letter( fp );

            if ( letter == 'S' || letter == 's' )
            {
                if ( letter == 's' )
                    bug( "Load_rooms: vnum %d has lowercase 's'", vnum );
                break;
            }

            if ( letter == 'D' )
            {
                EXIT_DATA *pexit;
                int        locks;

                door = fread_number( fp, &stat );
                if ( door < 0 || door >= MAX_DIR )
                {
                    bug( "Fread_rooms: vnum %d has bad door number.", vnum );
                    exit( 1 );
                }

                pexit                   = alloc_perm( sizeof( *pexit ) );
                pexit->description      = fread_string( fp, &stat );
                pexit->keyword          = fread_string( fp, &stat );
                locks                   = fread_number( fp, &stat );
                // convert exits - Veygoth
                switch( locks )
                {
                  case 0:
                    pexit->exit_info = 0;
                    break;
                  case 1:
                    pexit->exit_info = EX_ISDOOR | EX_CLOSED;
                    break;
                  case 2:
                    pexit->exit_info = EX_ISDOOR | EX_CLOSED | EX_LOCKED;
                    break;
                  case 3:
                    pexit->exit_info = EX_ISDOOR | EX_CLOSED | EX_LOCKED |
                                       EX_BASHPROOF | EX_PICKPROOF;
                    break;
                  case 4:
                    pexit->exit_info = EX_ISDOOR | EX_SECRET;
                    break;
                  case 5:
                    pexit->exit_info = EX_SECRET | EX_ISDOOR | EX_CLOSED;
                    break;
                  case 6:
                    pexit->exit_info = EX_ISDOOR | EX_CLOSED | EX_LOCKED |
                                       EX_SECRET;
                    break;
                  case 7:
                    pexit->exit_info = EX_ISDOOR | EX_CLOSED | EX_LOCKED |
                                       EX_BASHPROOF | EX_PICKPROOF |
                                       EX_SECRET;
                    break;
                  case 8:
                    pexit->exit_info = EX_BLOCKED;
                    break;
                  case 9:
                    pexit->exit_info = EX_BLOCKED | EX_ISDOOR | EX_CLOSED;
                    break;
                  case 10:
                    pexit->exit_info = EX_ISDOOR | EX_CLOSED | EX_LOCKED |
                                       EX_BLOCKED;
                    break;
                  case 11:
                    pexit->exit_info = EX_ISDOOR | EX_CLOSED | EX_LOCKED |
                                       EX_BASHPROOF | EX_PICKPROOF |
                                       EX_BLOCKED;
                    break;
                  case 12:
                    pexit->exit_info = EX_ISDOOR | EX_BLOCKED | EX_SECRET;
                    break;
                  case 13:
                    pexit->exit_info = EX_SECRET | EX_ISDOOR | EX_CLOSED |
                                       EX_BLOCKED;
                    break;
                  case 14:
                    pexit->exit_info = EX_ISDOOR | EX_CLOSED | EX_LOCKED |
                                       EX_SECRET | EX_BLOCKED;
                    break;
                  case 15:
                    pexit->exit_info = EX_ISDOOR | EX_CLOSED | EX_LOCKED |
                                       EX_BASHPROOF | EX_PICKPROOF |
                                       EX_SECRET | EX_BLOCKED;
                    break;
                  default:
                    bug( "The MUD does not understand an exits value of %d.  Defaulting to generic closed door.", locks );
                    pexit->exit_info = EX_ISDOOR;
                    break;
                };

                pexit->key              = fread_number( fp, &stat );
                pexit->vnum             = fread_number( fp, &stat );


                pRoomIndex->exit[door]          = pexit;
                top_exit++;
            }
            else if ( letter == 'E' )
            {
                EXTRA_DESCR_DATA *ed;

                ed                      = alloc_perm( sizeof( *ed ) );
                ed->keyword             = fread_string( fp, &stat );
                ed->description         = fread_string( fp, &stat );
                ed->next                = pRoomIndex->extra_descr;
                pRoomIndex->extra_descr = ed;
                top_ed++;
            }
            // Fall chance - Veygoth
            else if ( letter == 'F' )
            {
                pRoomIndex->fall_chance  = fread_number( fp, &stat );
            }
            // Current not yet used, values discarded - Veygoth
            else if ( letter == 'C' )
            {
                pRoomIndex->current = fread_number( fp, &stat );
                pRoomIndex->current_dir = fread_number( fp, &stat );
            }
            // Mana not yet used, values discarded - Veygoth
            else if ( letter == 'M' )
            {
                fread_number( fp, &stat );
                fread_number( fp, &stat );
            }
            else
            {
                bug( "Load_rooms: vnum %d has flag not 'DES'.", vnum );
                exit( 1 );
            }
        }

        iHash                   = vnum % MAX_KEY_HASH;
        pRoomIndex->next        = room_index_hash[iHash];
        room_index_hash[iHash]  = pRoomIndex;
        top_room++;
        top_vnum_room = top_vnum_room < vnum ? vnum : top_vnum_room;
        assign_area_vnum( vnum );
    }

    return;
}

void load_envy_rooms( FILE *fp )
{
    ROOM_INDEX_DATA *pRoomIndex;
#ifdef HARDCORE_DEBUG
    ROOM_INDEX_DATA *room;
    int count;
#endif
    int iHash;
    last_fun( "load_envy_rooms");

    if ( !area_last )
    {
        bug( "load_envy_rooms: no #AREA seen yet.", 0 );
        exit( 1 );
    }

    for ( ; ; )
    {
        char letter;
        int  vnum;
        int  door;
        int  stat;

        letter                          = fread_letter( fp );
        if ( letter != '#' )
        {
            bug( "load_envy_rooms: # not found.", 0 );
            exit( 1 );
        }

        vnum                            = fread_number( fp, &stat );
        if ( vnum == 0 )
            break;

        fBootDb = FALSE;
        if ( get_room_index( vnum ) )
        {
            bug( "load_envy_rooms: vnum %d duplicated.", vnum );
            exit( 1 );
        }
        fBootDb = TRUE;

        pRoomIndex                      = alloc_perm( sizeof( *pRoomIndex ) );
        pRoomIndex->people              = NULL;
        pRoomIndex->contents            = NULL;
        pRoomIndex->extra_descr         = NULL;
        pRoomIndex->area                = area_last;
        pRoomIndex->vnum                = vnum;
#ifdef HARDCORE_DEBUG
        bug( "Found room with vnum %d.", pRoomIndex->vnum );
#endif
	if ( !area_last->low_r_vnum )
	    area_last->low_r_vnum       = vnum;
	if ( vnum > area_last->hi_r_vnum )
	    area_last->hi_r_vnum        = vnum;

        pRoomIndex->name                = fread_string( fp, &stat );
        pRoomIndex->description         = fread_string( fp, &stat );
        /* This value was originally area number, but is now used for terrain
           description generation with maps -- Veygoth  */
        pRoomIndex->extra_sector_info   = fread_number( fp, &stat );
        pRoomIndex->room_flags[0]       = fread_number( fp, &stat );
        pRoomIndex->room_flags[0]       = convert_envy_room_flags( pRoomIndex->room_flags[0] );
        pRoomIndex->room_flags[1]       = 0;
	pRoomIndex->orig_room_flags	= pRoomIndex->room_flags[0];      /* OLC */
        pRoomIndex->sector_type         = fread_number( fp, &stat );
        // Convert sector type
        if( pRoomIndex->sector_type == 8 )
            pRoomIndex->sector_type = 9;
        else if( pRoomIndex->sector_type == 9 )
            pRoomIndex->sector_type = 8;
        else if( pRoomIndex->sector_type == 10 )
            pRoomIndex->sector_type = 24;

        pRoomIndex->light               = 0;
        for ( door = 0; door < MAX_DIR; door++ )
            pRoomIndex->exit[door] = NULL;

        for ( ; ; )
        {
            letter = fread_letter( fp );

            if ( letter == 'S' || letter == 's' )
            {
                if ( letter == 's' )
                    bug( "Load_rooms: vnum %d has lowercase 's'", vnum );
                break;
            }

            if ( letter == 'D' )
            {
                EXIT_DATA *pexit;
                int        locks;

                door = fread_number( fp, &stat );
                if ( door < 0 || door >= MAX_DIR )
                {
                    bug( "Fread_rooms: vnum %d has bad door number.", vnum );
                    exit( 1 );
                }

                pexit                   = alloc_perm( sizeof( *pexit ) );
                pexit->description      = fread_string( fp, &stat );
                pexit->keyword          = fread_string( fp, &stat );
                locks                   = fread_number( fp, &stat );
                // convert exits - Veygoth
                pexit->exit_info        = convert_envy_exit_flags( locks );
                pexit->key              = fread_number( fp, &stat );
                pexit->vnum             = fread_number( fp, &stat );


                pRoomIndex->exit[door]          = pexit;
                top_exit++;
            }
            else if ( letter == 'E' )
            {
                EXTRA_DESCR_DATA *ed;

                ed                      = alloc_perm( sizeof( *ed ) );
                ed->keyword             = fread_string( fp, &stat );
                ed->description         = fread_string( fp, &stat );
                ed->next                = pRoomIndex->extra_descr;
                pRoomIndex->extra_descr = ed;
                top_ed++;
            }
            // Fall chance - Veygoth
            else if ( letter == 'F' )
            {
                pRoomIndex->fall_chance  = fread_number( fp, &stat );
            }
            // Current not yet used, values discarded - Veygoth
            else if ( letter == 'C' )
            {
                pRoomIndex->current = fread_number( fp, &stat );
                pRoomIndex->current_dir = fread_number( fp, &stat );
            }
            // Mana not yet used, values discarded - Veygoth
            else if ( letter == 'M' )
            {
                fread_number( fp, &stat );
                fread_number( fp, &stat );
            }
            else
            {
                bug( "Load_rooms: vnum %d has flag not 'DES'.", vnum );
                exit( 1 );
            }
        }

        iHash                   = vnum % MAX_KEY_HASH;
        pRoomIndex->next        = room_index_hash[iHash];
        room_index_hash[iHash]  = pRoomIndex;
        top_room++;
        top_vnum_room = top_vnum_room < vnum ? vnum : top_vnum_room;
        assign_area_vnum( vnum );
    }

    return;
}

/*
 * Load a Magma format shop section - Veygoth.
 */
void load_magma_shops( FILE *fp )
{
    SHOP_DATA *pShop;
    int        keeper = 0;

    last_fun( "load_magma_shops");
    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	int             iTrade;
	int             stat;
        char            letter;
        int             count;

        letter = fread_letter( fp );
        if( letter != '#' )
        {
            bug( "Load shops: # not found before vnum", 0 );
        }
        keeper = fread_number( fp, &stat );
        letter = fread_letter( fp );
        if( letter != '~' )
        {
            bug( "Load shops: ~ not found after vnum", 0 );
        }
        letter                  = fread_letter( fp );
        if( letter == 'S' )
           return;
        else if( letter != 'N' )
        {
           bug( "Shop section: Letter 'N' not found after shop vnum", 0 );
        }
	if ( keeper == 0 )
	    break;
	pShop			= alloc_perm( sizeof( *pShop ) );
	pShop->keeper		= keeper;
        for( count = 0; count < MAX_SELL; count++ )
        {
           pShop->sell_item[count] = fread_number( fp, &stat );
           if( pShop->sell_item[count] == 0)
               break;
        }

	pShop->profit_sell	= fread_number( fp, &stat ) * 100;
        letter = fread_letter( fp );
        if( letter != '.' )
           bug( "Expecting decimal point in shop profit_sell value", 0);
        pShop->profit_sell       += fread_number( fp, &stat );

	pShop->profit_buy	= fread_number( fp, &stat ) * 100;
        letter = fread_letter( fp );
        if( letter != '.' )
           bug( "Expecting decimal point in shop profit_buy value", 0);
        pShop->profit_buy       += fread_number( fp, &stat );

	for ( iTrade = 0; iTrade < MAX_BUY; iTrade++ )
        {
	    pShop->buy_type[iTrade] = fread_number( fp, &stat );
            if( pShop->buy_type[iTrade] == 0 )
               break;
        }
        // discard seven message strings
        fread_string( fp, &stat );
        fread_string( fp, &stat );
        fread_string( fp, &stat );
        fread_string( fp, &stat );
        fread_string( fp, &stat );
        fread_string( fp, &stat );
        fread_string( fp, &stat );
        // discard two numbers
        fread_number( fp, &stat );
        fread_number( fp, &stat );
	/*discard*/               fread_number( fp, &stat );
        fread_number( fp, &stat );
        // room vnum of shop discarded
        fread_number( fp, &stat );
	pShop->open_hour	= fread_number( fp, &stat );
	pShop->close_hour	= fread_number( fp, &stat );
        // discard second open and close times
        fread_number( fp, &stat);
        fread_number( fp, &stat);
        // discard racist, roam, and killable flags
        fread_letter( fp );
        fread_letter( fp );
        fread_letter( fp );
        // discard open and close messages
        fread_string( fp, &stat );
        fread_string( fp, &stat );
        // discard the rest of the junk
        fread_number( fp, &stat );
        fread_number( fp, &stat );
        fread_letter( fp );
        fread_number( fp, &stat );
        fread_string( fp, &stat );
        // end of the shop
        letter = fread_letter( fp );
        if( letter != 'X')
        {
           bug( "Letter 'X' not found at end of shop section", 0 );
        }
	fread_to_eol( fp );
	pMobIndex		= get_mob_index( pShop->keeper );
	pMobIndex->pShop	= pShop;

	if ( !shop_first )
	    shop_first = pShop;
	if (  shop_last  )
	    shop_last->next = pShop;

	shop_last	= pShop;
	pShop->next	= NULL;
	top_shop++;
    }

    return;
}

/*
 * Load an Envy format shop section - Veygoth.
 */
void load_envy_shops( FILE *fp )
{
    SHOP_DATA *pShop;
    int        keeper = 0;

    last_fun( "load_envy_shops");
    for ( ; ; )
    {
	    MOB_INDEX_DATA *pMobIndex;
	    int             iTrade;
	    int             stat;

        keeper = fread_number( fp, &stat );

	    if ( keeper == 0 )
	        break;
	    pShop			= alloc_perm( sizeof( *pShop ) );
	    pShop->keeper	= keeper;
	    for ( iTrade = 0; iTrade < 5; iTrade++ )
        {
	        pShop->buy_type[iTrade] = fread_number( fp, &stat );
        }

	    pShop->profit_buy	= fread_number( fp, &stat );
	    pShop->profit_sell	= fread_number( fp, &stat );

	    pShop->open_hour	= fread_number( fp, &stat );
	    pShop->close_hour	= fread_number( fp, &stat );
        // Gobble up comment.
	    fread_to_eol( fp );
	    pMobIndex = get_mob_index( pShop->keeper );
	    pMobIndex->pShop	= pShop;

	    if ( !shop_first )
	        shop_first = pShop;
	    if (  shop_last  )
	        shop_last->next = pShop;

	    shop_last	= pShop;
	    pShop->next	= NULL;
	    top_shop++;
    }

    return;
}

// Loads a #QUESTS section.  Quest data must remain in a certain order
// (that which is created by DikuEdit) in order to load properly
// -- Veygoth
void load_quests( FILE *fp )
{
    QUEST_INDEX_DATA *quest;
    QUEST_DATA *qdata;
    QUEST_ITEM *qitem;
    TALK_DATA *message;
    bool log = IS_SET( sysdata.act, MUD_VERBOSE_QUESTLOAD );

    last_fun( "load_quests");
    if ( !area_last )
    {
      bug( "Load_quests: no #AREA seen yet.", 0 );
      exit( 1 );
    }

    for ( ; ; )
    {
      char letter;
      int  vnum;
      int  stat;

      letter                          = fread_letter( fp );
      if ( letter != '#' )
      {
          bug( "Load_quests: # not found.", 0 );
          exit( 1 );
      }

      vnum                            = fread_number( fp, &stat );
      if ( vnum == 0 )
          break;

      if( log ) log_string( "Valid quest vnum found." );

      quest = create_quest( );

      quest->vnum = vnum;

      for( ; ; )
      {
        letter                          = fread_letter( fp );
        
        if( letter == 'M' )
        {
           if( log ) log_string( "Message found" );
           if( !quest )
           {
              bug( "Error in load_quests:  message data seen before quest vnum.", 0 );
              exit( 1 );
           }
           message = create_quest_message( );
           message->keywords = fread_string( fp, &stat );
           message->message = fread_string( fp, &stat );
           message->next = quest->messages;
           quest->messages = message;
        }
        else if( letter == 'Q' )
        {
           if( log ) log_string( "Quest data found" );
           if( !quest )
           {
              bug( "Error in load_quests:  quest data seen before quest vnum.", 0 );
              exit( 1 );
           }
           qdata = create_quest_data( );
           qdata->complete = fread_string( fp, &stat );
           qdata->next = quest->quests;
           quest->quests = qdata;
        }
        else if( letter == 'R' )
        {
           if( log ) log_string( "Receive item found" );
           if( !quest )
           {
              bug( "Error in load_quests:  receive item data seen before quest vnum.", 0 );
              exit( 1 );
           }
           if( !quest->quests )
           {
              bug( "Error in load_quests:  receive item data seen before quest data.", 0 );
              exit( 1 );
           }
           qitem = create_quest_item( );
           letter = fread_letter( fp );
           switch( letter )
           {
             case 'I':
               qitem->type = 0;
               break;
             case 'C':
               qitem->type = 1;
               break;
             case 'E':
               qitem->type = 2;
               break;
             case 'S':
               qitem->type = 3;
               break;
             default:
               bug( "Quest Item data:  Invalid Give type!", 0 );
               break;
           }
           qitem->value = fread_number( fp, &stat );
           qitem->next = quest->quests->give;
           quest->quests->give = qitem;
        }
        else if( letter == 'G' )
        {
           if( log ) log_string( "Give item found" );
           if( !quest )
           {
              bug( "Error in load_quests:  give item data seen before quest vnum.", 0 );
              exit( 1 );
           }
           if( !quest->quests )
           {
              bug( "Error in load_quests:  give item data seen before quest data.", 0 );
              exit( 1 );
           }
           qitem = create_quest_item( );
           letter = fread_letter( fp );
           switch( letter )
           {
             case 'I':
               qitem->type = 0;
               break;
             case 'C':
               qitem->type = 1;
               break;
             case 'T':
               qitem->type = 2;
               break;
             default:
               bug( "Quest Item data:  Invalid Give type!", 0 );
               break;
           }
           qitem->value = fread_number( fp, &stat );
           qitem->next = quest->quests->receive;
           quest->quests->receive = qitem;
        }
        else if( letter == 'S' )
        {
           if( log ) log_string( "End of quest found" );
           // reset everything so we get proper error messages:
           quest = NULL;
           qdata = NULL;
           qitem = NULL;
           message = NULL;
           break;
        }
        else if( letter == 'D' )
        {
           if( log ) log_string( "Disappearance Message found" );
           if( !quest )
           {
              bug( "Error in load_quests:  disappear message seen before quest vnum.", 0 );
              exit( 1 );
           }
           if( !quest->quests )
           {
              bug( "Error in load_quests:  disappear message seen before quest data.", 0 );
              exit( 1 );
           }
           quest->quests->disappear = fread_string( fp, &stat );
        }
      }
    }

    return;
}


/*
 * Snarf spec proc declarations.
 */
void load_specials( FILE *fp )
{
    last_fun( "load_specials");
    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	OBJ_INDEX_DATA *pObjIndex;
	char            letter;
	int             stat;
    char*           word;

	switch ( letter = fread_letter( fp ) )
	{
	default:
	    bug( "Load_specials: letter '%c' not *OMS.", letter );
	    exit( 1 );

	case 'S':
	    return;

	case '*':
	    break;

	case 'M':
	    pMobIndex           = get_mob_index  ( fread_number ( fp, &stat ) );
        word = fread_word( fp, &stat );
        if( pMobIndex->spec_fun )
        {
           if( pMobIndex->spec_fun2 )
           {
               bug( "Attempt to assign a third special function to a mobile ignored.\n\r", 0 );
               break;
           }
        }
	    pMobIndex->spec_fun = spec_mob_lookup( word );
	    if ( pMobIndex->spec_fun == 0 )
	    {
            // Second chance lookup -- maybe we can translate it to one of our own specials.
            pMobIndex->spec_fun = envy_spec_mob_convert( word );
            if( pMobIndex->spec_fun == 0 )
            {
                char buf[1024];
                sprintf( buf, "load_specials: 'M': vnum %d has unrecognized special %s.",
                pMobIndex->vnum, word );
	            bug( buf,0 );
	            exit( 1 );
            }
	    }
	    break;

	case 'D':
	    pMobIndex           = get_mob_index  ( fread_number ( fp, &stat ) );
            if( pMobIndex->death_fun )
               bug( "Mob being assigned a third special function. Class function negated.\n\r", 0 );
	    pMobIndex->death_fun = spec_mob_lookup( fread_word   ( fp, &stat ) );
	    if ( pMobIndex->death_fun == 0 )
	    {
		bug( "Load_specials: 'D': vnum %d.", pMobIndex->vnum );
		exit( 1 );
	    }
	    break;

	case 'O':
	    pObjIndex           = get_obj_index  ( fread_number ( fp, &stat ) );
	    pObjIndex->spec_fun = spec_obj_lookup( fread_word   ( fp, &stat ) );
	    if ( pObjIndex->spec_fun == 0 )
	    {
		bug( "Load_specials: 'O': vnum %d.", pObjIndex->vnum );
		exit( 1 );
	    }
	    break;
	}

	fread_to_eol( fp );
    }
}



/*
 * Snarf games proc declarations.
 */
void load_games( FILE *fp )
{
    int        croupier = 0;

    last_fun( "load_games");
    for ( ; ; )
    {
	MOB_INDEX_DATA *pMobIndex;
	GAME_DATA      *pGame;
        char            letter;
	int             stat;

	pGame = alloc_perm( sizeof( GAME_DATA ) );

        switch ( letter = fread_letter( fp ) )
        {
        default:
            bug( "Load_games: letter '%c' not *MS.", letter );
            exit( 1 );

        case 'S':
            return;

        case '*':
            break;

        case 'M':
	    croupier			= fread_number( fp, &stat );
	    if ( croupier == 0 )
	    {
		free_mem( pGame, sizeof( GAME_DATA ) );
		return;
	    }
	    pMobIndex			= get_mob_index( croupier );
	    pMobIndex->pGame		= pGame;
	    pGame->croupier		= croupier;
	    pGame->game_fun		= game_lookup( fread_word ( fp, &stat ) );
	    pGame->bankroll		= fread_number( fp, &stat );
	    pGame->max_wait		= fread_number( fp, &stat );
	    pGame->cheat		= fread_number( fp, &stat );

	    if ( pGame->game_fun == 0 )
            {
                bug( "Load_games: 'M': vnum %d.", pMobIndex->vnum );
                exit( 1 );
            }
					  fread_to_eol( fp );
            break;
        }

	if ( !game_first )
	    game_first = pGame;
	if (  game_last  )
	    game_last->next = pGame;

	game_last	= pGame;
	pGame->next	= NULL;
	top_game++;
    }

    return;
}



/*
 * Snarf notes file.
 */
void load_notes( void )
{
    FILE      *fp;
    NOTE_DATA *pnotelast;
    char       strsave [ MAX_INPUT_LENGTH ];

    last_fun( "load_notes");
    sprintf( strsave, "%s%s%s", executable_directory, SYSTEM_DIR, NOTE_FILE );

    if ( !( fp = fopen( strsave, "r" ) ) )
	return;

    pnotelast = NULL;
    for ( ; ; )
    {
	NOTE_DATA *pnote;
	int       letter;
	int        stat;

	do
	{
	    letter = getc( fp );
	    if ( feof( fp ) )
	    {
		fclose( fp );
		return;
	    }
	}
	while ( isspace( letter ) );
	ungetc( letter, fp );

	pnote		  = alloc_mem( sizeof( *pnote ) );

	if ( str_cmp( fread_word( fp, &stat ), "sender" ) )
	    break;
	pnote->sender     = fread_string( fp, &stat );

	if ( str_cmp( fread_word( fp, &stat ), "date" ) )
	    break;
	pnote->date       = fread_string( fp, &stat );

	if ( str_cmp( fread_word( fp, &stat ), "stamp" ) )
	    break;
	pnote->date_stamp = fread_number( fp, &stat );

	if ( str_cmp( fread_word( fp, &stat ), "to" ) )
	    break;
	pnote->to_list    = fread_string( fp, &stat );

	if ( str_cmp( fread_word( fp, &stat ), "subject" ) )
	    break;
	pnote->subject    = fread_string( fp, &stat );

	if ( str_cmp( fread_word( fp, &stat ), "text" ) )
	    break;
	pnote->text       = fread_string( fp, &stat );

	if ( !note_list )
	    note_list           = pnote;
	else
	    pnotelast->next     = pnote;

	pnotelast               = pnote;
    }

    strcpy( strArea, NOTE_FILE );
    fpArea = fp;
    bug( "Load_notes: bad key word.", 0 );
    exit( 1 );
    return;
}



void load_sysdata( void )
{
    char        *word;
    char        buf [ MAX_STRING_LENGTH ];
    int         i;
    int         j;
    int         error_count = 0;
    int         status;
    char        *p;
    int         tmpi;
    int         num_keys;
    int         last_key = 0;
    FILE       *fp;
    char        strsave [ MAX_INPUT_LENGTH ];

    char	def_time [] = "";

    struct key_data key_tab [] = { 
      { "AllTimeMax", FALSE,  MAND,		{ &sysdata.all_time_max,
                                                                      NULL } },
      { "TimeOfMax",  TRUE,  (unlong) &def_time,{ &sysdata.time_of_max,
                                                                      NULL } },
      { "Act",        FALSE, 0,			{ &sysdata.act,       NULL } },
      { "Hour",       FALSE, 0,                 { &time_info.hour, NULL } },
      { "Day",        FALSE, 0,                 { &time_info.day,     NULL } },
      { "Month",      FALSE, 0,                 { &time_info.month, NULL } },
      { "Year",       FALSE, 0,                 { &time_info.year, NULL } },
      { "Moonphase",  FALSE, 0,                 { &weather_info.moonphase, NULL } },
      { "Moonday",    FALSE, 0,                 { &weather_info.moonday, NULL } },
      { "\0",         FALSE, 0                                             } };


    last_fun( "load_sysdata");
    sprintf( strsave, "%s%s%s", executable_directory, SYSTEM_DIR, SYSDATA_FILE );

    if ( !( fp = fopen( strsave, "r" ) ) )
	return;


    for ( num_keys = 0; *key_tab [num_keys].key; )
        num_keys++;

    for ( ; !feof (fp) ; )
    {

        word = fread_word( fp, &status );

        if ( !word )
	{
            bug( "fread_area:  Error reading key.  EOF?", 0 );
            fread_to_eol( fp );
            break;
	}

                /* This little diddy searches for the keyword
                   from the last keyword found */

        for ( i = last_key;
              i < last_key + num_keys &&
                str_cmp (key_tab [i % num_keys].key, word); )
            i++;

        i = i % num_keys;

        if ( !str_cmp (key_tab [i].key, word) )
            last_key = i;
        else
            i = num_keys;

        if ( *key_tab [i].key )         /* Key entry found in key_tab */
	{
            if ( key_tab [i].string == SPECIFIED )
                bug ("Key already specified.", 0);

                        /* Entry is a string */

            else
	      if ( key_tab [i].string )
	      {
                  if ( ( p = fread_string( fp, &status ) ) && !status )
		  {
		      free_string ( *(char **)key_tab [i].ptrs [0] );
		      *(char **)key_tab [i].ptrs [0] = p;
		  }
	      }

                        /* Entry is an integer */
            else
                for ( j = 0; key_tab [i].ptrs [j]; j++ )
		{
                    tmpi = fread_number ( fp, &status );
                    if ( !status )
                        *(int *)key_tab [i].ptrs [j] = tmpi;
		}

            if ( status )
	    {
                fread_to_eol( fp );
                continue;
	    }
	    else
                key_tab [i].string = SPECIFIED;
	}

        else if ( !str_cmp( word, "End" ) )
            break;

        else
	{
	    sprintf( buf, "load_sysdata: Unknown key '%s' in file.", word );
	    bug( buf, 0 );
	    fread_to_eol( fp );
	}
	
    }

                /* Require all manditory fields, set defaults */

    for ( i = 0; *key_tab [i].key; i++ )
    {

        if ( key_tab [i].string == SPECIFIED ||
             key_tab [i].deflt == DEFLT )
            continue;

        if ( key_tab [i].deflt == MAND )
	{
            sprintf( buf, "Manditory field '%s' missing from sysfile.",
                          key_tab [i].key );
            bug( buf, 0 );
            error_count++;
            continue;
	}

               /* This if/else sets default strings and numbers */

        if ( key_tab [i].string && key_tab [i].deflt )
	{
	    free_string( *(char **)key_tab [i].ptrs [0] );
            *(char **)key_tab [i].ptrs [0] =
	      str_dup( (char *)key_tab [i].deflt );
	}
        else
            for ( j = 0; key_tab [i].ptrs [j]; j++ )
	        *(int *)key_tab [i].ptrs [j] = key_tab [i].deflt;
    }

    fclose( fp );

    sysdata.max_players			= 0;

    return;
}


void save_sysdata( void )
{
    FILE *fp;
    char  strsave [ MAX_INPUT_LENGTH ];

    last_fun( "save_sysdata");
    fclose( fpReserve );

    sprintf( strsave, "%s%s%s", executable_directory, SYSTEM_DIR, SYSDATA_FILE );

    if ( !( fp = fopen( strsave, "w" ) ) )
	return;

    fprintf( fp, "AllTimeMax  %d\n",	sysdata.all_time_max	);
    fprintf( fp, "TimeOfMax   %s~\n",	sysdata.time_of_max	);
    fprintf( fp, "Act         %d\n",	sysdata.act		);
    fprintf( fp, "Hour        %d\n",    time_info.hour          );
    fprintf( fp, "Day         %d\n",    time_info.day           );
    fprintf( fp, "Month       %d\n",    time_info.month         );
    fprintf( fp, "Year        %d\n",    time_info.year          );
    fprintf( fp, "Moonphase   %d\n",    weather_info.moonphase  );
    fprintf( fp, "Moonday     %d\n",    weather_info.moonday    );
    fprintf( fp, "End\n" );

    fclose( fp );

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


void load_down_time( void )
{
    FILE *fp;
    int   number = 0;
    char  strsave [ MAX_INPUT_LENGTH ];

    last_fun( "load_down_time");
    down_time = 0;
    warning1  = 0;
    warning2  = 0;
    Reboot    = FALSE;

    sprintf( strsave, "%s%s%s", executable_directory, SYSTEM_DIR, DOWN_TIME_FILE );

    if ( !( fp = fopen( strsave, "r" ) ) )
	return;

    for ( ; ; )
    {
        char *word;
	int   letter;
	int   stat;

	do
	{
	    letter = getc( fp );
	    if ( feof( fp ) )
	    {
		fclose( fp );
		if ( down_time > 0 && down_time < 301 )
		{
		    down_time  = current_time + 300;
		    warning2   = down_time - 150;
		    warning1   = warning2  - 75;
		}
		else
		    if ( down_time > 0 )
		    {
			down_time += current_time;
			warning2   = down_time - 150;
			warning1   = warning2  - 150;
		    }
		return;
	    }
	}
	while ( isspace( letter ) );
	ungetc( letter, fp );
	
	word = fread_word( fp, &stat );

	if ( !str_cmp( word, "HOUR" ) )
	{
	    number = fread_number( fp, &stat );
	    if ( number > 0 )
	        down_time += (time_t) ( number * 3600 );
	}
	if ( !str_cmp( word, "MINUTE" ) )
	{
	    number = fread_number( fp, &stat );
	    if ( number > 0 )
	        down_time += (time_t) ( number * 60 );
	}
	if ( !str_cmp( word, "REBOOT" ) )
	{
	    Reboot = TRUE;
	}
    }
}

/*
 * Load up the ban file
 */
void load_ban( void )
{
    BAN_DATA  *pban;
    FILE      *fp;
    char       strsave [ MAX_INPUT_LENGTH ];

    last_fun( "load_ban");
    sprintf( strsave, "%s%s%s", executable_directory, SYSTEM_DIR, BAN_FILE );

    if ( !( fp = fopen( strsave, "r" ) ) )
	return;

    for ( ; ; )
    {
        int    letter;
	int    stat;

	do
	{
	    letter = getc( fp );
	    if ( feof( fp ) )
	    {
		fclose( fp );
		return;
	    }
	}
	while ( isspace( letter ) );
	ungetc( letter, fp );

	if ( !ban_free )
	{
	    pban     = alloc_perm( sizeof( *pban ) );
            top_ban++;
	}
	else
	{
	    pban     = ban_free;
	    ban_free = ban_free->next;
	}

	pban->name   = fread_string( fp, &stat );

	pban->next   = ban_list;
	ban_list     = pban;

    }

}

// Load_names, takes from Vasco Costa's (a.k.a. Zen) GreedMUD code
//
// Loads up a list of illegal names
void load_names( void )
{
	FILE *fp;
        char strsave[MAX_INPUT_LENGTH];

        last_fun( "load_names");
        sprintf( strsave, "%s%s%s", executable_directory, SYSTEM_DIR, NAME_LIST );

        if( ! ( fp = fopen( strsave, "r" ) ) )
           return;

        strName[0] = '\0';

        for( ; ; )
        {
		char *word;
                int   stat;

                word = fread_word( fp, &stat );

                if( word[0] == '$')
                  break;

                strcat( strName, word );
                strcat( strName, " " );

                if( (strlen( strName )) > (sizeof( strName ) - 16))
                  log_string( "Banned name list too long, mud about to crash." );
	}

	fclose( fp );
	return;
} 

void save_names( void )
{
    FILE *fp;
    char strsave[MAX_INPUT_LENGTH];

    last_fun( "save_names");
    sprintf( strsave, "%s%s%s", executable_directory, SYSTEM_DIR, NAME_LIST );

    if( ! ( fp = fopen( strsave, "w" ) ) )
      return;

    fprintf( fp, "%s \n$\n", strName );

    fclose( fp );
}



/*
 * Translate all room exits from virtual to real.
 * Has to be done after all rooms are read in.
 * Check for bad or suspicious reverse exits.
 */
void fix_exits( void )
{
		 EXIT_DATA       *pexit;
		 EXIT_DATA       *pexit_rev;
		 ROOM_INDEX_DATA *pRoomIndex;
		 ROOM_INDEX_DATA *to_room;
//                 char             buf     [ MAX_STRING_LENGTH ];
    extern const int              rev_dir [ ];
		 int              door;
                 int              iHash;

        last_fun( "fix_exits");

        for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
        {
	for ( pRoomIndex  = room_index_hash[iHash];
	      pRoomIndex;
	      pRoomIndex  = pRoomIndex->next )
	{
	    for ( door = 0; door < MAX_DIR; door++ )
	    {
		if ( ( pexit = pRoomIndex->exit[door] ) )
		{
		    if ( pexit->vnum <= 0 )
			pexit->to_room = NULL;
		    else
			pexit->to_room = get_room_index( pexit->vnum );
		}
	    }

	}
        }

        for( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
        {
	for ( pRoomIndex  = room_index_hash[iHash];
	      pRoomIndex;
	      pRoomIndex  = pRoomIndex->next )
	{
	    for ( door = 0; door < MAX_DIR; door++ )
	    {
		if (   ( pexit     = pRoomIndex->exit[door]       )
		    && ( to_room   = pexit->to_room               )
		    && ( pexit_rev = to_room->exit[rev_dir[door]] )
		    &&   pexit_rev->to_room != pRoomIndex )
		{
/*		    sprintf( buf, "Fix_exits: %d:%d -> %d:%d -> %d.",
			    pRoomIndex->vnum, door,
			    to_room->vnum,    rev_dir[door],
			    ( !pexit_rev->to_room ) ? 0
			    :  pexit_rev->to_room->vnum );
		    bug( buf, 0 );
*/
		}
	    }
       }
       }

    return;
}


void reset_room( ROOM_INDEX_DATA *pRoom , int not_initial)
{
    RESET_DATA  *pReset;
    EXIT_DATA   *pexit;
    CHAR_DATA   *pMob;
    CHAR_DATA   *LastMob = NULL;
    int          level = 0;
    bool         last;
// Lohrr - Added for debugging.
    char       buf [ MAX_STRING_LENGTH ];
//    char        *owner;

    last_fun( "reset_room");
    if ( !pRoom )
        return;

    pMob        = NULL;
    last        = TRUE;
    
    for ( pReset = pRoom->reset_first; pReset; pReset = pReset->next )
    {
        OBJ_DATA        *pObj;
        OBJ_DATA        *LastObj;
        MOB_INDEX_DATA  *pMobIndex;
        OBJ_INDEX_DATA  *pObjIndex;
        OBJ_INDEX_DATA  *pObjToIndex;
        ROOM_INDEX_DATA *pRoomIndex;

        switch ( pReset->command )
        {
        default:
                bug( "Reset_room: bad command %c.", pReset->command );
                break;
        case 'M':
            if ( !( pMobIndex = get_mob_index( pReset->arg1 ) ) )
            {
                bug( "Reset_room: 'M': bad vnum %d.", pReset->arg1 );
                continue;
            }

            if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
            {
                bug( "Reset_area: 'R': bad vnum %d.", pReset->arg3 );
                continue;
            }

            if ( ( pMobIndex->spec_fun == spec_mob_lookup( "spec_cast_ghost" )
                  && ( weather_info.sunlight > SUN_DARK )
                  && ( weather_info.sunlight < MOON_RISE ) ) )
            {
                last = FALSE;
                continue;
            }

            level = URANGE( 0, pMobIndex->level - 2, LEVEL_HERO );

            if ( pMobIndex->count >= pReset->arg2 )
            {
                last = FALSE;
                break;
            }
            pMob = create_mobile( pMobIndex );

            /*
             * Check for pet shop.
             */
            {
                ROOM_INDEX_DATA *pRoomIndexPrev;

                pRoomIndexPrev = get_room_index( pRoomIndex->vnum - 1 );
                if ( pRoomIndexPrev
                    && IS_ROOM( pRoomIndexPrev, ROOM_PET_SHOP ) )
                    SET_BIT( pMob->act, ACT_PET );
            }

            if ( room_is_dark( pRoomIndex ) )
                SET_AFF_BIT( pMob, AFF_INFRARED );

            char_to_room( pMob, pRoomIndex );
            pMob->load_room_vnum = pRoomIndex->vnum;

// Lohrr - This code makes mounts make their mounters mount them.
	    if (pReset->arg0 == -1 && LastMob ) 
	    {  // If set to be mounted. 
		one_argument(pMob->name, buf);
		do_mount (LastMob, buf); 
		sprintf(buf, "Mob %s trying to mount %s", 
			LastMob->name, pMob->name);
	    }  
 
            LastMob = pMob;

            level = URANGE( 0, pMob->level - 2, LEVEL_HERO );
            last  = TRUE;
            break;

        case 'F':
            if ( !( pMobIndex = get_mob_index( pReset->arg1 ) ) )
            {
                bug( "Reset_room: 'M': bad vnum %d.", pReset->arg1 );
                continue;
            }

            if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
            {
                bug( "Reset_area: 'R': bad vnum %d.", pReset->arg3 );
                continue;
            }

            if ( ( pMobIndex->spec_fun == spec_mob_lookup( "spec_cast_ghost" )
                  && ( weather_info.sunlight > SUN_DARK )
                  && ( weather_info.sunlight < MOON_RISE ) ) )
            {
                last = FALSE;
                continue;
            }

            level = URANGE( 0, pMobIndex->level - 2, LEVEL_HERO );

                if ( pMobIndex->count >= pReset->arg2 )

                {
                    last = FALSE;
                    break;
                }
            pMob = create_mobile( pMobIndex );

            /*
             * Check for pet shop.
             */
            {
                ROOM_INDEX_DATA *pRoomIndexPrev;

                pRoomIndexPrev = get_room_index( pRoomIndex->vnum - 1 );
                if ( pRoomIndexPrev
                    && IS_ROOM( pRoomIndexPrev, ROOM_PET_SHOP ) )
                    SET_BIT( pMob->act, ACT_PET );
            }

            if ( room_is_dark( pRoomIndex ) )
                SET_AFF_BIT( pMob, AFF_INFRARED );

            // set following bit.  can't have a message sent because
            // there is no valid room number (causes a segmentation fault)
            // - Veygoth
            add_follower_nomsg( pMob, LastMob);

            LastMob = pMob;
            char_to_room( pMob, pRoomIndex );
            pMob->load_room_vnum = pRoomIndex->vnum;
            level = URANGE( 0, pMob->level - 2, LEVEL_HERO );
            last  = TRUE;
            break;

        case 'O':
            if (not_initial)
            {
              last = FALSE;
              continue;
            }
            if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
            {
                bug( "Reset_room: 'O': bad vnum %d.", pReset->arg1 );
                continue;
            }

            if ( !( pRoomIndex = get_room_index( pReset->arg3 ) ) )
            {
                bug( "Reset_room: 'O': bad vnum %d.", pReset->arg3 );
                continue;
            }

            if ( pRoom->area->nplayer > 0
              || count_obj_list( pObjIndex, pRoomIndex->contents ) > 0 )
            {
                last = FALSE;
                break;
            }
            // check if is unique/arti
            if( is_artifact( pObjIndex->vnum ) )
            {
                sprintf( buf, "Artifact found: %s (%d)", pObjIndex->name, pObjIndex->vnum);
                log_string( buf );
            } //end if artifact

            pObj = create_object( pObjIndex, number_fuzzy( level ) ); 
            pObj->cost = 0;
            obj_to_room( pObj, pRoom );
            last = TRUE;
            break;

        case 'P':
            if (not_initial) {
              last = FALSE;
              continue;
            }
            if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
            {
                bug( "Reset_room: 'P': bad vnum %d.", pReset->arg1 );
                continue;
            }

            if ( !( pObjToIndex = get_obj_index( pReset->arg3 ) ) )
            {
                bug( "Reset_room: 'P': bad vnum %d.", pReset->arg3 );
                continue;
            }

            if ( pRoom->area->nplayer > 0
              || !( LastObj = get_obj_type( pObjToIndex ) )
              || count_obj_list( pObjIndex, LastObj->contains ) > 0 )
            {
                last = FALSE;
                break;
            }
            // check if is unique/arti
            if( is_artifact( pObjIndex->vnum ) )
            {
                sprintf( buf, "Artifact found: %s (%d)", pObjIndex->name, pObjIndex->vnum);
                log_string( buf );
            } //end if artifact

            pObj = create_object( pObjIndex, number_fuzzy( LastObj->level ) );
            obj_to_obj( pObj, LastObj );
            last = TRUE;

            /*
             * Ensure that the container gets reset.    OLC 1.1b
             * Added drink containers to this reset business.  Govno   
             */
            if ( ( LastObj->item_type == TYPE_CONTAINER ) || \
                 ( LastObj->item_type == TYPE_DRINK_CON ) )
            {
                LastObj->value[1] = LastObj->pIndexData->value[1];
            }
            else
            {
                    /* THIS SPACE INTENTIONALLY LEFT BLANK */
            }
            break;

        case 'G':
        case 'E':
            if (not_initial) {
              last = FALSE;
              continue;
            }
            if ( !( pObjIndex = get_obj_index( pReset->arg1 ) ) )
            {
                bug( "Reset_room: 'E' or 'G': bad vnum %d.", pReset->arg1 );
                continue;
            }

            if ( !last )
                break;

            if ( !LastMob )
            {
                bug( "Reset_room: 'E' or 'G': null mob for vnum %d.",
                    pReset->arg1 );
                last = FALSE;
                break;
            }
            // check if is unique/arti
            if( is_artifact( pObjIndex->vnum ) )
            {
                sprintf( buf, "Artifact found: %s (%d)", pObjIndex->name, pObjIndex->vnum);
                log_string( buf );
            } //end if artifact

            if ( LastMob->pIndexData->pShop )   /* Shop-keeper? */
            {
                int olevel;

                switch ( pObjIndex->item_type )
                {
                default:		olevel = 0;                      break;
                case TYPE_PILL:		olevel = number_range(  0, 10 ); break;
                case TYPE_POTION:	olevel = number_range(  0, 10 ); break;
                case TYPE_SCROLL:	olevel = number_range(  5, 15 ); break;
                case TYPE_WAND:		olevel = number_range( 10, 20 ); break;
                case TYPE_STAFF:	olevel = number_range( 15, 25 ); break;
                case TYPE_ARMOR:	olevel = number_range(  5, 15 ); break;
                case TYPE_OTHER:	olevel = number_range(  5, 15 ); break;
                case TYPE_CLOTHING:	olevel = number_range(  5, 15 ); break;
                case TYPE_WEAPON:       if ( pReset->command == 'G' )
                                            olevel = number_range( 5, 15 );
                                        else
                                            olevel = number_fuzzy( level );
                  break;
                }

                pObj = create_object( pObjIndex, olevel );
                if ( pReset->command == 'G' )
                    SET_OBJ_STAT( pObj, ITEM_INVENTORY );
            }
            else
            {
                pObj = create_object( pObjIndex, number_fuzzy( level ) );
            }
            obj_to_char( pObj, LastMob );
            if ( pReset->command == 'E' )
                equip_char( LastMob, pObj, pReset->arg3 );
            last = TRUE;
            break;

        case 'D':
            if (   pReset->arg2 < 0
                || pReset->arg2 >= MAX_DIR
                || !( pexit = pRoom->exit[pReset->arg2] )
                || !IS_SET( pexit->exit_info, EX_ISDOOR ) )
            {
                bug( "Reset_Room: 'D': exit %d not door.", pReset->arg2 );
            }

            switch ( pReset->arg3 )
            {
                default:
                    bug( "Resets_Room: 'D': bad 'locks': %d." , pReset->arg3);
                case 0:
                    break;
                case 1: SET_BIT( pexit->exit_info, EX_CLOSED );
                    break;
                case 2: SET_BIT( pexit->exit_info, EX_CLOSED | EX_LOCKED );
                    break;
                case 4: SET_BIT( pexit->exit_info, EX_SECRET );
                    break;
                case 5: SET_BIT( pexit->exit_info, EX_SECRET | EX_CLOSED );
                    break;
                case 6: SET_BIT( pexit->exit_info, EX_SECRET | EX_CLOSED | EX_LOCKED );
                    break;
                case 8: SET_BIT( pexit->exit_info, EX_BLOCKED );
                    break;
                case 9: SET_BIT( pexit->exit_info, EX_BLOCKED | EX_CLOSED );
                    break;
                case 10: SET_BIT( pexit->exit_info, EX_BLOCKED | EX_CLOSED | EX_LOCKED );
                    break;
                case 12: SET_BIT( pexit->exit_info, EX_BLOCKED | EX_SECRET );
                    break;
                case 13: SET_BIT( pexit->exit_info, EX_BLOCKED | EX_SECRET | EX_CLOSED );
                    break;
                case 14: SET_BIT( pexit->exit_info, EX_BLOCKED | EX_SECRET | EX_CLOSED | EX_LOCKED );
                    break;
            }
            break;

        case 'R':
            break;
        }
    }

    return;
}


/*
 * Repopulate areas periodically.
 *
 * Also gradually replenishes a town's guard store
 * if guards have been killed
 *
 * Called every five minutes.  pAre->age is in 5 minute increments
 */
void area_update( void )
{
    AREA_DATA *pArea;
    char       buf [ MAX_STRING_LENGTH ];
//    int reset_age;

    last_fun( "area_update");
    for ( pArea = area_first; pArea; pArea = pArea->next )
    {
	// CHAR_DATA *pch;

        if( pArea->dispatched > 0 )
            pArea->dispatched--;

        pArea->age++;

        // added check for resetmode and lifespan
        // - Lithos
        if ( pArea->resetmode == 0  && pArea->times_reset )
            continue;
        if ( pArea->resetmode == 1 && pArea->nplayer > 0 )
            continue;
        if ( pArea->age < number_range( pArea->llifespan, pArea->ulifespan ) 
            && pArea->times_reset )
            continue;

	/*
	 * Check for PC's.
	 */
        /* Reset message disabled by Veygoth.
	if ( pArea->nplayer > 0 && pArea->age == 15 - 1 )
	{
	    for ( pch = char_list; pch; pch = pch->next )
	    {
		if ( !IS_NPC( pch )
		    && IS_AWAKE( pch )
		    && pch->in_room
		    && pch->in_room->area == pArea )
		{
		    sprintf( buf, "%s\n\r", pArea->resetmsg );
		    send_to_char( buf,
			pch );
		}
	    }
	}
        End of disabled reset message code. */

	/*
	 * Check age and reset.
	 */
	    sprintf( buf, "%s has just been reset.", pArea->filename );
	    wiznet( NULL, WIZ_RESETS, L_DIR, buf );
	    reset_area( pArea );
    }

    return;
}



/*
 * OLC
 * Reset one area.
 */
void reset_area( AREA_DATA *pArea )
{
    ROOM_INDEX_DATA *pRoom;
    int  vnum;

    last_fun( "reset_area");
    pArea->squads = 0;
    pArea->dispatched = 0;
    for ( vnum = pArea->lvnum; vnum <= pArea->uvnum; vnum++ )
    {
        if ( ( pRoom = get_room_index( vnum ) ) )
        {
            reset_room( pRoom , pArea->times_reset);
        }
    }
    pArea->age = number_range(0, 2);
    pArea->times_reset++;
    return;
}


/*
 * Take a character data from the free list and clean it out.
 */
CHAR_DATA *new_character( bool player )
{
    static PC_DATA    pcdata_zero;
           CHAR_DATA *ch;

    last_fun( "new_character");
    if ( !char_free )
    {
	ch		= alloc_perm( sizeof( CHAR_DATA ) );
        top_char++;
    }
    else
    {
	ch		= char_free;
	char_free	= char_free->next;
    }

    clear_char( ch );

    if ( player )
    {
	if ( !pcdata_free )
	{
	    ch->pcdata  = alloc_perm( sizeof( *ch->pcdata ) );
            top_pcdata++;
	}
	else
	{
	    ch->pcdata  = pcdata_free;
	    pcdata_free = pcdata_free->next;
	}

	*ch->pcdata = pcdata_zero;
    }

    ch->deleted = FALSE;

    return ch;
}

AFFECT_DATA *new_affect( void )
{
    AFFECT_DATA *paf;

    last_fun( "new_affect");
    if ( !affect_free )
        return alloc_perm( sizeof( AFFECT_DATA ) );

    paf		= affect_free;
    affect_free = affect_free->next;

    return paf;
}

OBJ_DATA *new_object( void )
{
    OBJ_DATA *obj;

    last_fun( "new_object");
    if ( !obj_free )
    {
        obj	 = alloc_perm( sizeof( *obj ) );
        top_obj++;
    }
    else
    {
	obj	 = obj_free;
	obj_free = obj_free->next;
    }

    memset( obj, 0, sizeof( OBJ_DATA ) );

    obj->deleted = FALSE;

    return obj;
}

EXTRA_DESCR_DATA *new_extra_descr( void )
{
    EXTRA_DESCR_DATA *ed;

    last_fun( "new_extra_descr");
    if ( !extra_descr_free )
        return alloc_perm( sizeof( EXTRA_DESCR_DATA ) );
    
    ed		     = extra_descr_free;
    extra_descr_free = extra_descr_free->next;

    return ed;

}
	
/*
 * Create an instance of a mobile.
 */
CHAR_DATA *create_mobile( MOB_INDEX_DATA *pMobIndex )
{
    CHAR_DATA *mob;
    int count;

    last_fun( "create_mobile");
    if ( !pMobIndex )
    {
	bug( "Create_mobile: NULL pMobIndex.", 0 );
	exit( 1 );
    }

    mob			= new_character( FALSE );

    mob->pIndexData     = pMobIndex;
    mob->followers      = NULL;
    mob->name		= str_dup( pMobIndex->player_name );
    mob->short_descr	= str_dup( pMobIndex->short_descr );
    mob->long_descr	= str_dup( pMobIndex->long_descr  );
    mob->description	= str_dup( pMobIndex->description );
    mob->spec_fun	= pMobIndex->spec_fun;
    mob->spec_fun2      = pMobIndex->spec_fun2;
    mob->class          = pMobIndex->class;
    mob->level		= number_fuzzy( pMobIndex->level );
    mob->act		= pMobIndex->act;
    mob->position       = pMobIndex->position;
    for( count = 0; count < NUM_AFFECT_VECTORS; count++ )
    {
    	mob->affected_by[count] = pMobIndex->affected_by[count];
    }
    mob->alignment	= pMobIndex->alignment;
    mob->sex		= pMobIndex->sex;
    mob->race           = pMobIndex->race;
    mob->size           = race_table[mob->race].size;
    if( IS_SET( mob->act, ACT_SIZEMINUS ))
      mob->size--;
    if( IS_SET( mob->act, ACT_SIZEPLUS ))
      mob->size++;

    // Added for Magma style non-default sizes.
    /* This code is commented out because the allowing size within
     * the zone as an extra keyword was sloppy as all shit and I
     * will not allow it.  Not here.  No way. -- Veygoth
    if ( !str_infix ( "_size_none_", mob->name ) )
 	mob->size = SIZE_NONE;
    else if ( !str_infix ( "_size_tiny_", mob->name ) )
 	mob->size = SIZE_TINY;
    else if ( !str_infix ( "_size_small_", mob->name ) )
 	mob->size = SIZE_SMALL;
    else if ( !str_infix ( "_size_petite_", mob->name ) )
 	mob->size = SIZE_PETITE;
    else if ( !str_infix ( "_size_average_", mob->name ) )
 	mob->size = SIZE_AVERAGE;
    else if ( !str_infix ( "_size_medium_", mob->name ) )
 	mob->size = SIZE_MEDIUM;
    else if ( !str_infix ( "_size_large_", mob->name ) )
 	mob->size = SIZE_LARGE;
    else if ( !str_infix ( "_size_huge_", mob->name ) )
 	mob->size = SIZE_HUGE;
    else if ( !str_infix ( "_size_giant_", mob->name ) )
 	mob->size = SIZE_GIANT;
    else if ( !str_infix ( "_size_titanic_", mob->name ) )
 	mob->size = SIZE_TITANIC;
    else if ( !str_suffix ( "_size_gargantuan_", mob->name ) )
 	mob->size = SIZE_GARGANTUAN;
    */
    mob->cast_spl       = 0;
    mob->cast_time      = 0;
    mob->perm_str       = dice( 2, 46 ) + 8;
    mob->perm_int       = dice( 2, 46 ) + 8;
    mob->perm_wis       = dice( 2, 46 ) + 8;
    mob->perm_dex       = dice( 2, 46 ) + 8;
    mob->perm_con       = dice( 2, 46 ) + 7;
    mob->perm_agi       = dice( 2, 46 ) + 8;
    mob->perm_cha       = dice( 2, 46 ) + 8;
    mob->perm_pow       = dice( 2, 46 ) + 8;
    mob->perm_luk       = dice( 2, 46 ) + 8;
    mob->mod_str        = 0;
    mob->mod_int        = 0;
    mob->mod_wis        = 0;
    mob->mod_dex        = 0;
    mob->mod_con        = 0;
    mob->mod_agi        = 0;
    mob->mod_cha        = 0;
    mob->mod_pow        = 0;
    mob->mod_luk        = 0;
    mob->resistant	= pMobIndex->resistant;
    mob->immune		= pMobIndex->immune;
    mob->susceptible	= pMobIndex->susceptible;
    mob->max_mana       = mob->level * 10;
    if( mob->race < MAX_COIN_RACE )
    {
      // Veygoth - reworked the math on mob money to better reflect the mob's level.
      // previously, 1st level mobs could have gold or platinum.

      int level = pMobIndex->level;

      mob->money.copper = dice( 10, level ) / 25;
      mob->money.silver = dice( 7, level ) / 25;
      mob->money.gold = dice( 4, level ) / 25;

      if( level >= 10)
        level -= 10;

      mob->money.platinum = dice( 2, level ) / 25;
    }
    else
    {
      mob->money.copper = 0;
      mob->money.silver = 0;
      mob->money.gold = 0;
      mob->money.platinum = 0;
    }
    mob->armor		= interpolate( mob->level, 100, -100 );

    // * MOB HITPOINTS *
    //
    // Was level d 8, upped it to level d 13
    // considering mobs *still* won't have as many hitpoints as some players until
    // at least level 10, this shouldn't be too big an upgrade.
    //
    // Mob hitpoints are not based on constitution *unless* they have a
    // constitution modifier from an item, spell, or other affect
    //
    // -- Veygoth

  // In light of recent player dissatisfaction with the
  // mob hitpoints, I'm implementing a log curve, using
  //  hp = exp( 2.15135 + level*0.151231)
  // This will will result in the following hp matrix:
  //     Level    Hitpoints
  //      20        175
  //      30        803
  //      40        3643
  //      50        16528
  //      55        35207
  //      60        75000
  // - Lithos
    mob->max_hit	= dice(mob->level, 13 ) + 1;
    // Mob hps are non-linear above level 10.
    if( mob->level > 20 )
    {
        int lvl = mob->level - 5;
        int lower = 4 * (lvl*lvl);
        int upper = (28 * (lvl*lvl))/5;
/*
        lvl -= 5;
        lvl = (lvl*lvl*lvl*lvl) / 150;
// If people keep complaining about mid-level mobs having too many hps,
//   this line will make lvl 55 mobs have about 45k, and lvl 60 mobs ->
//   60k hps. - Lohrr
//        lvl = (lvl*lvl*lvl*lvl*lvl) / 7000;
        // totals 4((lvl-5)^2) + ((lvl-10)^4)/150
        lower += lvl;
        // totals 6((lvl-5)^2) + ((lvl-10)^4)/150
        upper += lvl;
*/

        upper = (int)exp( 2.2 + mob->level * 0.151231);
        lower = (int)exp( 2.1 + mob->level * 0.151231);
        mob->max_hit +=	number_range( lower, upper );
    }
    else if ( mob->level > 10 )
        mob->max_hit += number_range( mob->level * 2,
                            ((mob->level-8)^2 * mob->level )/2);

    // Demons/devils gain an extra 30 hitpoints per level (+1500 at lvl 50).
    // -- Veygoth
    if( mob->race == RACE_DEMON || mob->race == RACE_DEVIL || mob->race == RACE_DRAGON )
      mob->max_hit += (mob->level * 30);

    mob->hit		= get_max_hit( mob );
    
    /* 
     * Horses get more moves, necessary for mounts.
     */
    if( !str_cmp( race_table[mob->race].name, "Horse" ) )
    {
        mob->max_move = 290 + dice(4, 5);
        mob->move = mob->max_move;
    }
    mob->load_room_vnum = 0;

    /*
     * Insert in list.
     */
    mob->next		= char_list;
    char_list		= mob;
    pMobIndex->count++;
    return mob;
}

/* Govno
 * duplicate a mobile minus inventory
 */
void clone_mobile( CHAR_DATA *parent, CHAR_DATA *clone )
{
    int i;
    AFFECT_DATA *paf;

    last_fun( "clone_mobile");
    if ( parent == NULL || clone == NULL || !IS_NPC( parent ) )
        return;

    /* fix values */
    clone->name         = str_dup(parent->name);
    clone->short_descr  = str_dup(parent->short_descr);
    clone->long_descr   = str_dup(parent->long_descr);
    clone->description  = str_dup(parent->description);
    clone->sex          = parent->sex;
    clone->class        = parent->class;
    clone->race         = parent->race;
    clone->level        = parent->level;
    clone->trust        = 0;
    clone->spec_fun	= parent->spec_fun;
    clone->spec_fun2	= parent->spec_fun2;
    clone->timer        = parent->timer;
    clone->wait         = parent->wait;
    clone->hit          = parent->hit;
    clone->max_hit      = parent->max_hit;
    clone->mana         = parent->mana;
    clone->max_mana     = parent->max_mana;
    clone->move         = parent->move;
    clone->max_move     = parent->max_move;
    clone->money	= parent->money;
    clone->exp          = parent->exp;
    clone->act          = parent->act;
    clone->affected     = parent->affected;
//    clone->affected_by  = parent->affected_by;
    clone->position     = parent->position;
//    clone->saving_throw = parent->saving_throw;
    clone->alignment    = parent->alignment;
    clone->hitroll      = parent->hitroll;
    clone->damroll      = parent->damroll;
    clone->wimpy        = parent->wimpy;
    clone->deaf		= parent->deaf;
    clone->hunting	= parent->hunting;
    clone->hating	= parent->hating;
    clone->fearing	= parent->fearing;
    clone->resistant	= parent->resistant;
    clone->immune	= parent->immune;
    clone->susceptible	= parent->susceptible;
    clone->size         = parent->size;
    clone->spec_fun     = parent->spec_fun;
    clone->perm_str	= parent->perm_str;
    clone->perm_int	= parent->perm_int;
    clone->perm_wis	= parent->perm_wis;
    clone->perm_dex	= parent->perm_dex;
    clone->perm_con	= parent->perm_con;
    clone->perm_agi	= parent->perm_agi;
    clone->perm_cha	= parent->perm_cha;
    clone->perm_pow	= parent->perm_pow;
    clone->perm_luk	= parent->perm_luk;
    clone->mod_str	= parent->mod_str;
    clone->mod_int	= parent->mod_int;
    clone->mod_wis	= parent->mod_wis;
    clone->mod_dex	= parent->mod_dex;
    clone->mod_con	= parent->mod_con;
    clone->mod_agi	= parent->mod_agi;
    clone->mod_cha	= parent->mod_cha;
    clone->mod_pow	= parent->mod_pow;
    clone->mod_luk	= parent->mod_luk;
    clone->armor	= parent->armor;
    clone->mpact	= parent->mpact;
    clone->mpactnum	= parent->mpactnum;

    for ( i = 0; i < 6; i++ )
        clone->saving_throw[i]	= parent->saving_throw[i];

    /* now add the affects */
    for ( paf = parent->affected; paf != NULL; paf = paf->next )
        affect_to_char( clone, paf );

}

/*
 * Create an instance of an object.
 */
OBJ_DATA *create_object( OBJ_INDEX_DATA *pObjIndex, int level )
{
    int count;
    OBJ_DATA *obj;
    char buf[MAX_STRING_LENGTH]; // better error messages...

    last_fun( "create_object");
    if( level < 1 )
        level = 1;

    if ( !pObjIndex )
    {
	bug( "Create_object: NULL pObjIndex.", 0 );
	exit( 1 );
    }

    obj			= new_object();

    obj->pIndexData	= pObjIndex;
    obj->in_room	= NULL;
    obj->level		= level;
    obj->wear_loc	= -1;
    obj->fly_level      = 0;

    obj->name		= str_dup( pObjIndex->name        );
    obj->short_descr	= str_dup( pObjIndex->short_descr );
    obj->description	= str_dup( pObjIndex->description );
    obj->spec_fun	= pObjIndex->spec_fun;
    obj->item_type	= pObjIndex->item_type;
    obj->extra_flags[0]	= pObjIndex->extra_flags[0];
    obj->extra_flags[1] = pObjIndex->extra_flags[1];
    for( count = 0; count < NUM_AFFECT_VECTORS; count++ )
       obj->affected_by[count] = pObjIndex->affected_by[count];
    obj->wear_flags	= pObjIndex->wear_flags;
    obj->anti_flags     = pObjIndex->anti_flags;
    obj->anti_flags2    = pObjIndex->anti_flags2;
    obj->material       = pObjIndex->material;
    obj->size           = pObjIndex->size;
    obj->volume         = pObjIndex->volume;
    obj->craftsmanship  = pObjIndex->craftsmanship;
    obj->value[0]	= pObjIndex->value[0];
    obj->value[1]	= pObjIndex->value[1];
    obj->value[2]	= pObjIndex->value[2];
    obj->value[3]	= pObjIndex->value[3];
    obj->value[4]	= pObjIndex->value[4];
    obj->value[5]	= pObjIndex->value[5];
    obj->value[6]	= pObjIndex->value[6];
    obj->value[7]	= pObjIndex->value[7];
    obj->weight		= pObjIndex->weight;
    obj->cost		= pObjIndex->cost;
    obj->condition      = pObjIndex->condition;
    obj->deleted        = FALSE;
    obj->trap           = pObjIndex->trap;

    // Create vehicle data for vehicles that are created.  The
    // bulk of the data is stored in the object - Veygoth
    if( obj->item_type == TYPE_VEHICLE || obj->item_type == TYPE_SHIP )
    {
      VEHICLE_DATA * vehicle;
      vehicle = new_vehicle();
      if( obj->item_type == TYPE_SHIP )
        vehicle->type = VEH_SHIP_ANY_WATER;
      else
        vehicle->type = VEH_FLATLAND;
      // need to create virtual rooms for the rest of the data
      vehicle->hull = obj->value[5];
      vehicle->fly_level = 0;
      vehicle->direction = 0;
      vehicle->speed = 0;
      vehicle->occupants = 0;
      vehicle->movement_timer = 0;
      vehicle->movement_delay = 0;
      vehicle->movement_pointer = 0;
      vehicle->movement_script[0] = '\0';
      vehicle->ship = obj;
      vehicle->owner = NULL;
      vehicle->entry_point = obj->value[1];
      vehicle->control_panel = obj->value[2];
    }

    /*
     * Mess with object properties.
     */
    switch ( obj->item_type )
    {
    default:
        sprintf( buf, "Read_object: vnum %d bad type %d", pObjIndex->vnum,
                       obj->item_type );
	bug( buf, 0 );
	break;

    case TYPE_SWITCH:
    case TYPE_TELEPORT:
    case TYPE_LIGHT:
    case TYPE_TREASURE:
    case TYPE_OTHER:
    case TYPE_PEN:
    case TYPE_TRASH:
    case TYPE_CONTAINER:
    case TYPE_DRINK_CON:
    case TYPE_KEY:
    case TYPE_FOOD:
    case TYPE_BOAT:
    case TYPE_CORPSE_NPC:
    case TYPE_CORPSE_PC:
    case TYPE_PORTAL:
    case TYPE_QUIVER:
    case TYPE_MISSILEWEAP:
    case TYPE_TOTEM:
    case TYPE_SHIELD:
    case TYPE_SPELLBOOK:
    case TYPE_LOCKPICK:
    case TYPE_ARMOR:
    case TYPE_CLOTHING:
    case TYPE_WEAPON:
    case TYPE_RANGED_WEAPON:
    case TYPE_WALL:
	break;

    case TYPE_SCROLL:
	obj->value[0]   = number_fuzzy( obj->value[0] );
	break;

    case TYPE_WAND:
    case TYPE_STAFF:
	obj->value[0]   = number_fuzzy( obj->value[0] );
	obj->value[1]	= number_fuzzy( obj->value[1] );
	obj->value[2]	= obj->value[1];
	break;

    case TYPE_POTION:
    case TYPE_PILL:
	obj->value[0]   = number_fuzzy( number_fuzzy( obj->value[0] ) );
	break;

    case TYPE_MONEY:
	obj->value[0]   = obj->cost;
	break;
    }

    obj->next		= object_list;
    object_list		= obj;
    pObjIndex->count++;

    return obj;
}

/* Govno
 * Duplicate an object exactly minus contents
 */
void clone_object( OBJ_DATA *parent, OBJ_DATA *clone )
{
    int i;
    EXTRA_DESCR_DATA *ed,*ed_new;

    last_fun( "clone_object");
    if ( parent == NULL || clone == NULL )
        return;

    /* start fixing the object */
    clone->name         = str_dup(parent->name);
    clone->short_descr  = str_dup(parent->short_descr);
    clone->description  = str_dup(parent->description);
    clone->spec_fun	= parent->spec_fun;
    clone->affected	= parent->affected;
    clone->item_type    = parent->item_type;
    clone->wear_flags   = parent->wear_flags;
    clone->anti_flags	= parent->anti_flags;
    clone->anti_flags2	= parent->anti_flags2;
    clone->material	= parent->material;
    clone->size		= parent->size;
    clone->volume	= parent->volume;
    clone->craftsmanship = parent->craftsmanship;
    clone->weight       = parent->weight;
    clone->cost         = parent->cost;
    clone->level        = parent->level;
    clone->condition    = parent->condition;
    clone->timer        = parent->timer;
    clone->condition    = parent->condition;
    clone->trap         = parent->trap;

    for ( i = 0; i < NUM_ITEM_EXTRA_VECTORS; i++ )
       clone->extra_flags[i] = parent->extra_flags[i];

    for ( i = 0; i < NUM_AFFECT_VECTORS; i++ )
       clone->affected_by[i] = parent->affected_by[i];

    for ( i = 0; i < 8; i++ )
        clone->value[i] = parent->value[i];

    /* extended desc */
    for ( ed = parent->extra_descr; ed != NULL; ed = ed->next )
    {
        ed_new                  = new_extra_descr();
        ed_new->keyword         = str_dup( ed->keyword);
        ed_new->description     = str_dup( ed->description );
        ed_new->next            = clone->extra_descr;
        clone->extra_descr      = ed_new;
    }

}


/*
 * Create some quest index data - Veygoth
 */
QUEST_INDEX_DATA *create_quest( )
{
    QUEST_INDEX_DATA *quest;

    last_fun( "create_quest");
    quest = alloc_perm( sizeof( *quest ));

    memset( quest, 0, sizeof( QUEST_INDEX_DATA ));

    quest->vnum = 0;
    quest->quests = NULL;
    quest->messages = NULL;
    quest->next = quest_first;
    quest_first = quest;
    top_quest++;

    return quest;
}

/*
 * Create some quest data - Veygoth
 */
QUEST_DATA *create_quest_data( )
{
    QUEST_DATA *qdata;

    last_fun( "create_quest_data");
    qdata = alloc_perm( sizeof( *qdata ));

    memset( qdata, 0, sizeof( QUEST_DATA ));

    qdata->next = NULL;
    qdata->receive = NULL;
    qdata->give = NULL;
    qdata->disappear = NULL;
    qdata->complete = NULL;
    top_qdata++;

    return qdata;
}

/*
 * Create some quest message data - Veygoth
 */
TALK_DATA *create_quest_message( )
{
    TALK_DATA *message;

    last_fun( "create_quest_message");
    message = alloc_perm( sizeof( *message ));

    memset( message, 0, sizeof( TALK_DATA ));

    message->next = NULL;
    message->keywords = NULL;
    message->message = NULL;
    top_qmsg++;

    return message;
}

/*
 * Create some quest data - Veygoth
 */
QUEST_ITEM *create_quest_item( )
{
    QUEST_ITEM *qitem;

    last_fun( "create_quest_item");
    qitem = alloc_perm( sizeof( *qitem ));

    memset( qitem, 0, sizeof( QUEST_ITEM ));

    qitem->complete = FALSE;
    qitem->type = 0;
    qitem->value = 0;
    top_qitem++;

    return qitem;
}

/*
 * Clear a new character.
 */
void clear_char( CHAR_DATA *ch )
{
    static CHAR_DATA ch_zero;

    last_fun( "clear_char");
    *ch				= ch_zero;
    ch->name			= &str_empty[0];
    ch->short_descr		= &str_empty[0];
    ch->long_descr		= &str_empty[0];
    ch->description		= &str_empty[0];
    ch->last_note               = 0;
    ch->logon			= current_time;
    ch->armor			= 100;
    ch->position		= POS_STANDING;
    ch->level                   = 0;
    ch->race                    = 0;
    ch->hit			= 20;
    ch->max_hit			= 20;
    ch->mana			= 100;
    ch->max_mana		= 50;
    ch->move			= 150;
    ch->max_move		= 150;
    ch->followers               = NULL;
    ch->group_leader            = NULL;
    ch->master                  = NULL;
    ch->deleted                 = FALSE;
    ch->hunting			= NULL;
    ch->fearing			= NULL;
    ch->hating			= NULL;
    ch->riding			= NULL;
    ch->rider			= NULL;
    ch->resistant		= 0;
    ch->immune			= 0;
    ch->susceptible		= 0;
    ch->fly_level               = 0;
    return;
}



/*
 * Free a character.
 */
void free_char( CHAR_DATA *ch )
{
    OBJ_DATA    *obj;
    OBJ_DATA    *obj_next;
    AFFECT_DATA *paf;

    last_fun( "free_char");
    /*
     * This places the character in a place we know exists and allows for
     * more controled removal of mud effects and such.  Suggested by Erwin.
     */

    ch->in_room = get_room_index( ROOM_VNUM_LIMBO );

    for ( obj = ch->carrying; obj; obj = obj_next )
    {
        obj_next = obj->next_content;
        if ( obj->deleted )
	    continue;
	extract_obj( obj );
    }

    for ( paf = ch->affected; paf; paf = paf->next )
    {
        if ( paf->deleted )
	    continue;
	affect_remove( ch, paf );
    }

    free_string( ch->name               );
    free_string( ch->short_descr	);
    free_string( ch->long_descr		);
    free_string( ch->description	);

    if ( ch->pcdata )
    {
	free_string( ch->pcdata->pwd		);
	free_string( ch->pcdata->bamfin		);
	free_string( ch->pcdata->bamfout	);
	free_string( ch->pcdata->immskll	);
	free_string( ch->pcdata->title		);
	free_string( ch->pcdata->prompt         );
	ch->pcdata->next = pcdata_free;
	pcdata_free      = ch->pcdata;
    }

    ch->next	     = char_free;
    char_free	     = ch;
    return;
}



/*
 * Get an extra description from a list.
 */
char *get_extra_descr( const char *name, EXTRA_DESCR_DATA *ed )
{
    last_fun( "get_extra_descr");
    for ( ; ed; ed = ed->next )
    {
	if ( is_name( name, ed->keyword ) )
	    return ed->description;
    }
    return NULL;
}



/*
 * Translates mob virtual number to its mob index struct.
 * Hash table lookup.
 */
MOB_INDEX_DATA *get_mob_index( int vnum )
{
    MOB_INDEX_DATA *pMobIndex;

    last_fun( "get_mob_index");
    /*
     * There is a possibility of vnum passed is negative.
     * Trapping suggested by Erwin Andreasen  -Kahn
     */

    if ( vnum < 0 )
        return NULL;

    for ( pMobIndex  = mob_index_hash[vnum % MAX_KEY_HASH];
	  pMobIndex;
	  pMobIndex  = pMobIndex->next )
    {
	if ( pMobIndex->vnum == vnum )
	    return pMobIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_mob_index: bad vnum %d.", vnum );
	exit( 1 );
    }

    return NULL;
}



/*
 * Translates mob virtual number to its obj index struct.
 * Hash table lookup.
 */
OBJ_INDEX_DATA *get_obj_index( int vnum )
{
    OBJ_INDEX_DATA *pObjIndex;

    last_fun( "get_obj_index");
    /*
     * There is a possibility of vnum passed is negative.
     * Trapping suggested by Erwin Andreasen  -Kahn
     */

    if ( vnum < 0 )
        return NULL;

    for ( pObjIndex  = obj_index_hash[vnum % MAX_KEY_HASH];
	  pObjIndex;
	  pObjIndex  = pObjIndex->next )
    {
	if ( pObjIndex->vnum == vnum )
	    return pObjIndex;
    }

    if ( fBootDb )
    {
    	bug( "get_obj_index: bad vnum %d.", vnum );
        bug( "The mud would previously crash at this point.  This has been disabled, but you should fix this to avoid null reference crashes later!", 0 );
    }
    else
    {
        // Uncomment this to be warned every time a bad object vnum is found.
        //warning( "get_obj_index: bad vnum %d", vnum );
    }

    return NULL;
}



/*
 * Translates mob virtual number to its room index struct.
 * Hash table lookup.
 */
ROOM_INDEX_DATA *get_room_index( int vnum )
{
    ROOM_INDEX_DATA *pRoomIndex;

    last_fun( "get_room_index");
    /*
     * There is a possibility of vnum passed is negative.
     * Trapping suggested by Erwin Andreasen  -Kahn
     */

    if ( vnum < 0 )
        return NULL;

    for ( pRoomIndex = room_index_hash[vnum % MAX_KEY_HASH];
          pRoomIndex;
          pRoomIndex = pRoomIndex->next )
    {
	if ( pRoomIndex->vnum == vnum )
	    return pRoomIndex;
    }

    if ( fBootDb )
    {
	bug( "Get_room_index: bad vnum %d.", vnum );
    }

    return NULL;
}



/*
 * Read a letter from a file.
 */
char fread_letter( FILE *fp )
{
    char c;
    last_fun( "fread_letter");

    do
    {
	c = getc( fp );
    }
    while ( isspace( c ) );

    return c;
}



/*
 * Read a number from a file.
 */
int fread_number( FILE *fp, int *status )
{
    int  c;
    bool sign;
    int  number;
    int  stat;

    last_fun( "fread_number");

    do
    {
	c = getc( fp );
    }
    while ( isspace( c ) );

    number = 0;

    sign   = FALSE;
    if ( c == '+' )
    {
	c = getc( fp );
    }
    else if ( c == '-' )
    {
	sign = TRUE;
	c = getc( fp );
    }

    if ( !isdigit( c ) )
    {
        char errbuf[1024];
        *status = 1;
        sprintf( errbuf, "Fread_number: bad format.  '%c' not expected.", c );
	    bug( errbuf,0 );
	    bug( "   If bad object or mobile, check for missing '~' in value[] fields.", 0 );
	    return 0;
    }

    while ( isdigit(c) )
    {
	number = number * 10 + c - '0';
	c      = getc( fp );
    }

    if ( sign )
	number = 0 - number;

    if ( c == '|' )
	number += fread_number( fp, &stat );
    else if ( c != ' ' )
	ungetc( c, fp );

    return number;
}



/*
 * Read to end of line (for comments).
 */
void fread_to_eol( FILE *fp )
{
    int c;

    last_fun( "fread_to_eol");
    do
    {
	c = getc( fp );
    }
    while ( c != '\n' && c != '\r' && c != EOF );

    do
    {
	c = getc( fp );
    }
    while ( c == '\n' || c == '\r' );

    ungetc( c, fp );
    return;
}



/*
 * Read one word (into static buffer).
 */
char *fread_word( FILE *fp, int *status )
{
    static char  word [ MAX_INPUT_LENGTH ];
           char *pword;
           int   cEnd;

    *status = 0;

    last_fun( "fread_word");
    do
    {
	cEnd = getc( fp );
    }
    while ( isspace( cEnd ) );

    if ( cEnd == '\'' || cEnd == '"' )
    {
	pword   = word;
    }
    else
    {
	word[0] = cEnd;
	pword   = word+1;
	cEnd    = ' ';
    }

    for ( ; pword < word + MAX_INPUT_LENGTH; pword++ )
    {
	*pword = getc( fp );
	if ( cEnd == ' ' ? isspace( *pword ) : *pword == cEnd )
	{
	    if ( cEnd == ' ' )
		ungetc( *pword, fp );
	    *pword = '\0';
	    return word;
	}
    }

    pword[-1] = '\0';

//    bug( "Fread_word: word too long.", 0 );
//    log_string( word );
//    log_string( "End of word!" );
    *status = 1;
    return NULL;
}



/*
 * Allocate some ordinary memory,
 *   with the expectation of freeing it someday.
 */
void *alloc_mem( int sMem )
{
    void *pMem;
    int iList;

    last_fun( "alloc_mem");
    for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
    {
	if ( sMem <= rgSizeList[iList] )
	    break;
    }

    if ( iList == MAX_MEM_LIST )
    {
	bug( "Alloc_mem: size %d too large.", sMem );
	exit( 1 );
    }

    if ( !rgFreeList[iList] )
    {
	pMem		  = alloc_perm( rgSizeList[iList] );
    }
    else
    {
	pMem              = rgFreeList[iList];
	rgFreeList[iList] = * ( (void **) rgFreeList[iList] );
    }

    return pMem;
}



/*
 * Free some memory.
 * Recycle it back onto the free list for blocks of that size.
 */
void free_mem( void *pMem, int sMem )
{
    int iList;

    last_fun( "free_mem");
    for ( iList = 0; iList < MAX_MEM_LIST; iList++ )
    {
	if ( sMem <= rgSizeList[iList] )
	    break;
    }

    if ( iList == MAX_MEM_LIST )
    {
	bug( "Free_mem: size %d too large.", sMem );
	exit( 1 );
    }

    * ( (void **) pMem ) = rgFreeList[iList];
    rgFreeList[iList]  = pMem;

    return;
}

/*
 * Allocate some permanent memory.
 * Permanent memory is never freed,
 *   pointers into it may be copied safely.
 */
void *alloc_perm( int sMem )
{
           void *pMem;
    static char *pMemPerm;
    static int   iMemPerm;

    last_fun( "alloc_perm");
    while ( sMem % sizeof( long ) != 0 )
	sMem++;
    if ( sMem > MAX_PERM_BLOCK )
    {
	bug( "Alloc_perm: %d too large.", sMem );
	exit( 1 );
    }

    if ( !pMemPerm || iMemPerm + sMem > MAX_PERM_BLOCK )
    {
	iMemPerm = 0;
	if ( !( pMemPerm = calloc( 1, MAX_PERM_BLOCK ) ) )
	{
	    perror( "Alloc_perm" );
	    exit( 1 );
	}
    }

    pMem        = pMemPerm + iMemPerm;
    iMemPerm   += sMem;
    nAllocPerm += 1;
    sAllocPerm += sMem;
    return pMem;
}

/*
 * Translate area range values to Envy style ranges.  -Zen
 */
char *range_string( AREA_DATA *pArea )
{
    static char buf [ 512 ];

    last_fun( "range_string");
    if ( !pArea )
	return "none";

    if ( pArea->llv == 0 && pArea->ulv == MAX_LEVEL )
	sprintf( buf, " All " );
    else
	if ( pArea->llv == 0 && pArea->ulv == 0 )
	    sprintf( buf, "None " );
    else
	sprintf( buf, "%2d %2d", pArea->llv, pArea->ulv );

    return buf;
}

void do_areas( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea1;
    AREA_DATA *pArea2;
    char       buf   [ MAX_STRING_LENGTH   ];
    char       buf1  [ MAX_STRING_LENGTH*4 ];
    int        iArea;
    int        iAreaHalf;

    last_fun( "do_areas");
    buf1[0] = '\0';

    iAreaHalf = ( top_area + 1 ) / 2;
    pArea1 = area_first;
    pArea2 = area_first;

    for ( iArea = 0; iArea < iAreaHalf; iArea++ )
	pArea2 = pArea2->next;

    for ( iArea = 0; iArea < iAreaHalf; iArea++ )
    {
	sprintf( buf, "{%5.5s} %-15.15s %-52.52s \n\r",
		range_string( pArea1 ), pArea1->author, pArea1->name );
	strcat( buf1, buf );

	if ( pArea2 )
	{
	    sprintf( buf, "{%5.5s} %-15.15s %-52.52s\n\r",
		range_string( pArea2 ), pArea2->author, pArea2->name );
	}
	else
	    sprintf( buf, "\n\r" );

	strcat( buf1, buf );


	pArea1 = pArea1->next;
	if ( pArea2 )
	    pArea2 = pArea2->next;
    }

    send_to_char( buf1, ch );
    return;
}



void do_memory( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *rch;
    char       buf [ MAX_STRING_LENGTH ];

    last_fun( "do_memory");
    rch = get_char( ch );
    
    if ( !authorized( rch, "memory" ) )
        return;

    sprintf( buf, "Affects %5d - %4zu bytes each (%zu total)\n\r",
	 top_affect,
         sizeof( AFFECT_DATA ),
         (top_affect * sizeof( AFFECT_DATA )) );
	 send_to_char( buf, ch );
    sprintf( buf, "Areas   %5d - %4zu bytes each (%zu total)\n\r",
	 top_area,
         sizeof( AREA_DATA ),
         (top_area * sizeof( AREA_DATA )) );
	 send_to_char( buf, ch );
    sprintf( buf, "ExDes   %5d - %4zu bytes each (%zu total)\n\r",
	 top_ed,
         sizeof( EXTRA_DESCR_DATA ),
         (top_ed * sizeof( EXTRA_DESCR_DATA )) );
	 send_to_char( buf, ch );
    sprintf( buf, "Exits   %5d - %4zu bytes each (%zu total)\n\r",
	 top_exit,
         sizeof( EXIT_DATA ),
         (top_exit * sizeof( EXIT_DATA )) );
	 send_to_char( buf, ch );
    sprintf( buf, "Helps   %5d - %4zu bytes each (%zu total)\n\r",
	 top_help,
         sizeof( HELP_DATA ),
         (top_help * sizeof( HELP_DATA )) );
	 send_to_char( buf, ch );
    sprintf( buf, "Socials %5d - %4zu bytes each (%zu total)\n\r",
	 top_social,
         sizeof( SOC_INDEX_DATA ),
         (top_social * sizeof( SOC_INDEX_DATA )) );
	 send_to_char( buf, ch );
    sprintf( buf, "Mobindx %5d - %4zu bytes each (%zu total)\n\r",
	 top_mob_index,
         sizeof( MOB_INDEX_DATA ),
         (top_mob_index * sizeof( MOB_INDEX_DATA )) );
	 send_to_char( buf, ch );
    sprintf( buf, "Chars   %5d - %4zu bytes each (%zu total)\n\r",
	 top_char,
         sizeof( CHAR_DATA ),
         (top_char * sizeof( CHAR_DATA )) );
	 send_to_char( buf, ch );
    sprintf( buf, "PCdata  %5d - %4zu bytes each (%zu total)\n\r",
	 top_pcdata,
         sizeof( PC_DATA ),
         (top_pcdata * sizeof( PC_DATA )) );
	 send_to_char( buf, ch );
    sprintf( buf, "Objindx %5d - %4zu bytes each (%zu total)\n\r",
	 top_obj_index,
         sizeof( OBJ_INDEX_DATA ),
         (top_obj_index * sizeof( OBJ_INDEX_DATA )) );
	 send_to_char( buf, ch );
    sprintf( buf, "Objs    %5d - %4zu bytes each (%zu total)\n\r",
	 top_obj,
         sizeof( OBJ_DATA ),
         (top_obj * sizeof( OBJ_DATA )) );
	 send_to_char( buf, ch );
    sprintf( buf, "Resets  %5d - %4zu bytes each (%zu total)\n\r",
	 top_reset,
         sizeof( RESET_DATA ),
         (top_reset * sizeof( RESET_DATA )) );
	 send_to_char( buf, ch );
    sprintf( buf, "Rooms   %5d - %4zu bytes each (%zu total)\n\r",
	 top_room,
         sizeof( ROOM_INDEX_DATA ),
         (top_room * sizeof( ROOM_INDEX_DATA )) );
	 send_to_char( buf, ch );
    sprintf( buf, "Shops   %5d - %4zu bytes each (%zu total)\n\r",
	 top_shop,
         sizeof( SHOP_DATA ),
         (top_shop * sizeof( SHOP_DATA )) );
	 send_to_char( buf, ch );
    sprintf( buf, "MScript %5d - %4zu bytes each (%zu total)\n\r",
	 top_mvscript,
         sizeof( MOVE_SCRIPT ),
         (top_mvscript * sizeof( MOVE_SCRIPT )) );
	 send_to_char( buf, ch );
    sprintf( buf, "Quests  %5d - %4zu bytes each (%zu total)\n\r",
	 top_quest,
         sizeof( QUEST_INDEX_DATA ),
         (top_quest * sizeof( QUEST_INDEX_DATA )) );
	 send_to_char( buf, ch );
    sprintf( buf, "- Msgs  %5d - %4zu bytes each (%zu total)\n\r",
	 top_qmsg,
         sizeof( TALK_DATA ),
         (top_qmsg * sizeof( TALK_DATA )) );
	 send_to_char( buf, ch );
    sprintf( buf, "- Qdata %5d - %4zu bytes each (%zu total)\n\r",
	 top_qdata,
         sizeof( QUEST_DATA ),
         (top_qdata * sizeof( QUEST_DATA )) );
	 send_to_char( buf, ch );
    sprintf( buf, "- Qitem %5d - %4zu bytes each (%zu total)\n\r",
	 top_qitem,
         sizeof( QUEST_ITEM ),
         (top_qitem * sizeof( QUEST_ITEM )) );
	 send_to_char( buf, ch );
    sprintf( buf, "Events  %5d - %4zu bytes each (%zu total)\n\r",
	 top_event,
         sizeof( EVENT_DATA ),
         (top_event * sizeof( EVENT_DATA )) );
	 send_to_char( buf, ch );
    sprintf( buf, "Traps   %5d - %4zu bytes each (%zu total)\n\r",
	 top_trap,
         sizeof( TRAP_DATA ),
         (top_trap * sizeof( TRAP_DATA )) );
	 send_to_char( buf, ch );
    sprintf( buf, "Memdata %5d - %4zu bytes each (%zu total)\n\r",
         top_memdata,
         sizeof( MEM_DATA ),
         (top_memdata * sizeof( MEM_DATA )) );
         send_to_char( buf, ch );
    sprintf( buf, "Siteban %5d - %4zu bytes each (%zu total)\n\r",
         top_ban,
         sizeof( BAN_DATA ),
         (top_ban * sizeof( BAN_DATA )) );
         send_to_char( buf, ch );
    sprintf( buf, "Desc    %5d - %4zu bytes each (%zu total)\n\r",
         top_desc,
         sizeof( DESCRIPTOR_DATA ),
         (top_desc * sizeof( DESCRIPTOR_DATA )) );
         send_to_char( buf, ch );
    sprintf( buf, "Vehicle %5d - %4zu bytes each (%zu total)\n\r",
         top_vehicle,
         sizeof( VEHICLE_DATA ),
         (top_vehicle * sizeof( VEHICLE_DATA )) );
         send_to_char( buf, ch );
    sprintf( buf, "Crime   %5d - %4zu bytes each (%zu total)\n\r",
         top_crime,
         sizeof( CRIME_DATA ),
         (top_crime * sizeof( CRIME_DATA )) );
         send_to_char( buf, ch );
    sprintf( buf, "Notes   %5d - Various sizes\n\r",
         top_note );
         send_to_char( buf, ch );

    sprintf( buf, "Perms   %5d blocks  of %7d bytes.\n\r\n\r",
	    nAllocPerm, sAllocPerm );
    send_to_char( buf, ch );

    sprintf( buf, "Shared Strings   %5d strings of %7d bytes (max %d).\n\r",
	    nAllocString, sAllocString, MAX_STRING );
    send_to_char( buf, ch );
    sprintf( buf, "Overflow Strings %5d strings of %7d bytes.\n\r",
	    nOverFlowString, sOverFlowString );
    send_to_char( buf, ch );
    if ( Full )
    {
	sprintf( buf, "Shared String Heap is full, increase MAX_STRING.\n\r" );
	send_to_char( buf, ch );
    }

    return;
}



/*
 * Stick a little fuzz on a number.
 */
int number_fuzzy( int number )
{
    last_fun( "number_fuzzy");
    switch ( number_bits( 2 ) )
    {
    case 0:  number -= 1; break;
    case 3:  number += 1; break;
    }

    return UMAX( 1, number );
}



/*
 * Generate a random number.
 */
int number_range( int from, int to )
{
    int  power;
    int  number;
    char lfbuf[200];

    sprintf( lfbuf, "number_range: %d, %d", from, to );
    last_fun( lfbuf );
    if ( ( to = to - from + 1 ) <= 1 )
	return from;

    for ( power = 2; power < to; power <<= 1 )
	;

    while ( ( number = number_mm( ) & ( power - 1 ) ) >= to )
	;

    return from + number;
}



/*
 * Generate a percentile roll.
 */
int number_percent( void )
{
    int percent;

    last_fun( "number_percent");
    while ( ( percent = number_mm( ) & ( 128-1 ) ) > 99 )
	;

    return 1 + percent;
}



/*
 * Generate a random door.
 */
int number_door( void )
{
    int door;

    last_fun( "number_door");

    // Was using MAX_DIR which caused it to crash occasionally
    // because we needed to do MAX_DIR -1 - Veygoth
    door = number_range( 0, (MAX_DIR-1) );

    return door;
}



int number_bits( int width )
{
    last_fun( "number_bits");
    return number_mm( ) & ( ( 1 << width ) - 1 );
}



/*
 * I've gotten too many bad reports on OS-supplied random number generators.
 * This is the Mitchell-Moore algorithm from Knuth Volume II.
 * Best to leave the constants alone unless you've read Knuth.
 * -- Furey
 */
static	int	rgiState[2+55];

void init_mm( )
{
    int *piState;
    int  iState;

    last_fun( "init_mm");
    piState	= &rgiState[2];

    piState[-2]	= 55 - 55;
    piState[-1]	= 55 - 24;

    piState[0]	= ( (int) current_time ) & ( ( 1 << 30 ) - 1 );
    piState[1]	= 1;
    for ( iState = 2; iState < 55; iState++ )
    {
	piState[iState] = ( piState[iState-1] + piState[iState-2] )
			& ( ( 1 << 30 ) - 1 );
    }
    return;
}



int number_mm( void )
{
    int *piState;
    int  iState1;
    int  iState2;
    int  iRand;

//    last_fun( "number_mm");
    piState		= &rgiState[2];
    iState1	 	= piState[-2];
    iState2	 	= piState[-1];
    iRand	 	= ( piState[iState1] + piState[iState2] )
			& ( ( 1 << 30 ) - 1 );
    piState[iState1]	= iRand;
    if ( ++iState1 >= 55 )
	iState1 = 0;
    if ( ++iState2 >= 55 )
	iState2 = 0;
    piState[-2]		= iState1;
    piState[-1]		= iState2;
    return iRand >> 6;
}



/*
 * Roll some dice.
 */
int dice( int number, int size )
{
    int idice;
    int sum;

    last_fun( "dice");
    switch ( size )
    {
    case 0: return 0;
    case 1: return number;
    }

    for ( idice = 0, sum = 0; idice < number; idice++ )
	sum += number_range( 1, size );

    return sum;
}



/*
 * Simple linear interpolation.
 */
int interpolate( int level, int value_00, int value_47 )
{
    last_fun( "interpolate");
    return value_00 + level * ( value_47 - value_00 ) / 47;
}



/*
 * Removes the tildes from a string.
 * Used for player-entered strings that go into disk files.
 */
void smash_tilde( char *str )
{
    last_fun( "smash_tilde");
    for ( ; *str != '\0'; str++ )
    {
	if ( *str == '~' )
	    *str = '-';
    }

    return;
}



/*
 * Compare strings, case insensitive.
 * Return TRUE if different
 *   (compatibility with historical functions).
 */
bool str_cmp( const char *astr, const char *bstr )
{
    if ( !astr )
    {
	bug( "Str_cmp: null astr.", 0 );
	return TRUE;
    }

    if ( !bstr )
    {
	bug( "Str_cmp: null bstr.", 0 );
	return TRUE;
    }

    for ( ; *astr || *bstr; astr++, bstr++ )
    {
	if ( LOWER( *astr ) != LOWER( *bstr ) )
	    return TRUE;
    }

    return FALSE;
}



/*
 * Compare strings, case insensitive, for prefix matching.
 * Return TRUE if astr not a prefix of bstr
 *   (compatibility with historical functions).
 */
bool str_prefix( const char *astr, const char *bstr )
{
    if ( !astr )
    {
	bug( "Str_prefix: null astr.", 0 );
	return TRUE;
    }

    if ( !bstr )
    {
	bug( "Str_prefix: null bstr.", 0 );
	return TRUE;
    }

    for ( ; *astr; astr++, bstr++ )
    {
	if ( LOWER( *astr ) != LOWER( *bstr ) )
	    return TRUE;
    }

    return FALSE;
}



/*
 * Compare strings, case insensitive, for match anywhere.
 * Returns TRUE is astr not part of bstr.
 *   (compatibility with historical functions).
 */
bool str_infix( const char *astr, const char *bstr )
{
    char c0;
    int  sstr1;
    int  sstr2;
    int  ichar;

    if ( ( c0 = LOWER( astr[0] ) ) == '\0' )
	return FALSE;

    sstr1 = strlen( astr );
    sstr2 = strlen( bstr );

    for ( ichar = 0; ichar <= sstr2 - sstr1; ichar++ )
    {
	if ( c0 == LOWER( bstr[ichar] ) && !str_prefix( astr, bstr + ichar ) )
	    return FALSE;
    }

    return TRUE;
}



/*
 * Compare strings, case insensitive, for suffix matching.
 * Return TRUE if astr not a suffix of bstr
 *   (compatibility with historical functions).
 */
bool str_suffix( const char *astr, const char *bstr )
{
    int sstr1;
    int sstr2;

    sstr1 = strlen( astr );
    sstr2 = strlen( bstr );
    if ( sstr1 <= sstr2 && !str_cmp( astr, bstr + sstr2 - sstr1 ) )
	return FALSE;
    else
	return TRUE;
}

/*
 * Returns an initial-capped string.
 */
/* This function does not work properly and prevents
 * people from being able to log in, among other things
 * - Veygoth 12-29-99
 */
char *capitalize( const char *str )
{
    static char strcap [ MAX_STRING_LENGTH ];
    int i;
    int j;

    /* Set all non colour codes to lower case. */
    for ( j = 0; str[j] != '\0'; j++ )
    {
        if ( ( str[j] == '&' ) && ( str[j+1] == '+' || str[j+1] == '-' ) )
        {   /*  Copy the &, +/-, and the next char. */
            strcap[j] = str[j];
            strcap[j+1] = str[j+1];
            j += 2;
            if ( str[j] != '\0' )
                strcap[j] = str[j];
            else
                j--;
	}
        else
            strcap[j] = LOWER( str[j] );
    }

    strcap[j] = '\0';

    /* Set first non-colour code to upper case. */
    for ( i = 0; str[i] != '\0'; i++)
    {
        if ( str[i] == '&' && str[i+1] != '\0' ) 
	{
            if ( str[i+1] == 'n' || str[i+1] == 'N' )
                i++;
            else
            if ( str[i+1] == '+' || str[i+1] == '-' )
                i += 2;
            else
            {
                if ( str[i] != '\0' )
                    i++;
                break;
            }
        }
        else break;
    }

    strcap[i] = UPPER( strcap[i] );

    return strcap;
}

/*
 * Returns an all-caps string.
 */
char *all_capitalize( const char *str )
{
    static char strcap [ MAX_STRING_LENGTH ];
           int  i;

    for ( i = 0; str[i] != '\0'; i++ )
	strcap[i] = UPPER( str[i] );
    strcap[i] = '\0';
    return strcap;
}



/*
 * Append a string to a file.
 */
void append_file( CHAR_DATA *ch, char *file, char *str )
{
    FILE *fp;

    last_fun( "append_file" );

    if ( IS_NPC( ch ) || str[0] == '\0' )
	return;

    fclose( fpReserve );
    if ( !( fp = fopen( file, "a" ) ) )
    {
	perror( file );
	send_to_char( "Could not open the file!\n\r", ch );
    }
    else
    {
	fprintf( fp, "[%5d] %s: %s\n",
		ch->in_room ? ch->in_room->vnum : 0, ch->name, str );
	fclose( fp );
    }

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}



/*
 * Reports a bug.
 */
void bug( const char *str, int param )
{
    FILE *fp;
    char  buf     [ MAX_STRING_LENGTH ];

    last_fun( "bug" );

    if ( fpArea )
    {
	int iLine;
	int iChar;
        char strsave[MAX_STRING_LENGTH];

	if ( fpArea == stdin )
	{
	    iLine = 0;
	}
	else
	{
	    iChar = ftell( fpArea );
	    fseek( fpArea, 0, 0 );
	    for ( iLine = 0; ftell( fpArea ) < iChar; iLine++ )
	    {
		while ( getc( fpArea ) != '\n' );
	    }
	    fseek( fpArea, iChar, 0 );
	}

	sprintf( buf, "[*****] FILE: %s LINE: %d", strArea, iLine );
	log_string( buf );

        sprintf( strsave, "%s%s%s", executable_directory, SYSTEM_DIR, SHUTDOWN_FILE );
	if ( ( fp = fopen( strsave, "a" ) ) )
	{
	    fprintf( fp, "[*****] %s\n", buf );
	    fclose( fp );
	}
    }

    strcpy( buf, "[*****] ERROR: " );
    sprintf( buf + strlen( buf ), str, param );
    log_string( buf );

    /* Don't show the "[*****] ERROR:" part of the bug. */
    wiznet( NULL, WIZ_DEBUG, L_DIR, &(buf[14]) );

    fclose( fpReserve );

    /* Removed the logging to BUGS.TXT because the log files are enough info and we want BUGS.TXT readable */

    fpReserve = fopen( NULL_FILE, "r" );

    return;
}

/*
 * Reports a warning to the log.
 */
void warning( const char *str, int param )
{
    char  buf     [ MAX_STRING_LENGTH ];

    last_fun( "warning" );

    strcpy( buf, "[*****] WARNING: " );
    sprintf( buf + strlen( buf ), str, param );
    log_string( buf );

    return;
}


/*
 * Reports a bug.
 * Got it from Erwin S.A. source - Zen
 */
void bugf( char * fmt, ... )
{
    char        buf [ MAX_STRING_LENGTH ];
    va_list     args;

    last_fun( "bugf" );

    va_start( args, fmt );
    vsprintf( buf, fmt, args );
    va_end( args );

    bug ( buf, 0 );
    return;
}



/*
 * Clan warnings, info, etc.
 */
void log_clan( const char *str )
{
    FILE *fp;
    char  strsave [ MAX_INPUT_LENGTH ];

    last_fun( "log_clan");
    fclose( fpReserve );

    sprintf( strsave, "%s%s%s", executable_directory, SYSTEM_DIR, CLAN_FILE );

    if ( ( fp = fopen( strsave, "a" ) ) )
    {
	fprintf( fp, "%s\n", str );
	fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );

    return;
}

/*
 * Writes a string to the log.
 * Do not add last_fun here, or last_fun will cease to function properly.
 */
void log_string( const char *str )
{
    char *strtime;

    strtime                    = ctime( &current_time );
    strtime[strlen( strtime )-1] = '\0';
    fprintf( stderr, "%s :: %s\n", strtime, str );
    return;
}

/*
 * Writes a string to the log.
 * Got it from Erwin S.A. source - Zen
 */
void logff( char * fmt, ... )
{
    char        buf [ 2 * MAX_STRING_LENGTH ];
    va_list     args;

    va_start( args, fmt );
    vsprintf( buf, fmt, args );
    va_end( args );

    log_string( buf );
    return;
}



/*
 * This function is here to aid in debugging.
 * If the last expression in a function is another function call,
 *   gcc likes to generate a JMP instead of a CALL.
 * This is called "tail chaining."
 * It hoses the debugger call stack for that call.
 * So I make this the last call in certain critical functions,
 *   where I really need the call stack to be right for debugging!
 *
 * If you don't understand this, then LEAVE IT ALONE.
 * Don't remove any calls to tail_chain anywhere.
 *
 * -- Furey
 */
void tail_chain( void )
{
    return;
}



/* 
 * MOBProg functions:
 * This routine transfers between alpha and numeric forms of the
 *    mob_prog bitvector types. This allows the use of the words in the
 *    mob/script files.
 */
int prog_name_to_type ( char *name )
{
   last_fun( "prog_name_to_type");
   if ( !str_cmp( name, "in_file_prog"   ) )	return IN_FILE_PROG;
   if ( !str_cmp( name, "act_prog"       ) )    return ACT_PROG;
   if ( !str_cmp( name, "speech_prog"    ) )	return SPEECH_PROG;
   if ( !str_cmp( name, "rand_prog"      ) ) 	return RAND_PROG;
   if ( !str_cmp( name, "fight_prog"     ) )	return FIGHT_PROG;
   if ( !str_cmp( name, "hitprcnt_prog"  ) )	return HITPRCNT_PROG;
   if ( !str_cmp( name, "death_prog"     ) )	return DEATH_PROG;
   if ( !str_cmp( name, "entry_prog"     ) )	return ENTRY_PROG;
   if ( !str_cmp( name, "greet_prog"     ) )	return GREET_PROG;
   if ( !str_cmp( name, "all_greet_prog" ) )	return ALL_GREET_PROG;
   if ( !str_cmp( name, "give_prog"      ) ) 	return GIVE_PROG;
   if ( !str_cmp( name, "bribe_prog"     ) )	return BRIBE_PROG;
   if ( !str_cmp( name, "proc_prog"      ) )    return PROC_PROG;
   if ( !str_cmp( name, "move_prog"      ) )    return MOVE_PROG;

   return( ERROR_PROG );
}

/*
 * This routine reads in scripts of MOBprograms from a file.
 */
MPROG_DATA* mprog_file_read( const char *f, MPROG_DATA *mprg,
			    MOB_INDEX_DATA *pMobIndex )
{
  MOVE_SCRIPT *script;
  char        MOBProgfile[ MAX_INPUT_LENGTH ];
  MPROG_DATA *mprg2;
  FILE       *progfile;
  char        letter;
  bool        done = FALSE;
  int         stat;

  last_fun( "mprog_file_read");

  sprintf( MOBProgfile, "%s%s%s", executable_directory, MOB_DIR, f );

  progfile = fopen( MOBProgfile, "r" );
  if ( !progfile )
  {
     bug( "Mob: %d couldnt open mobprog file", pMobIndex->vnum );
     exit( 1 );
  }

  mprg2 = mprg;
  switch ( letter = fread_letter( progfile ) )
  {
    case '>':
     break;
    case '|':
       bug( "empty mobprog file.", 0 );
       exit( 1 );
     break;
    default:
       bug( "in mobprog file syntax error - no letter.", 0 );
       exit( 1 );
     break;
  }

  while ( !done )
  {
    mprg2->type = prog_name_to_type( fread_word( progfile, &stat ) );
    switch ( mprg2->type )
    {
     case ERROR_PROG:
        bug( "mobprog file type error", 0 );
        exit( 1 );
      break;
     case IN_FILE_PROG:
        bug( "prog file contains a call to file.", 0 );
        exit( 1 );
      break;
     case MOVE_PROG:
        script = create_move_script( );
        pMobIndex->move_script = script;
        pMobIndex->progtypes = pMobIndex->progtypes | mprg2->type;
        script->ship = FALSE;
        script->vnum = pMobIndex->vnum;
        script->start_room = fread_number( progfile, &stat );
        if ( (fread_letter( progfile )) != '~' )
        {
            bug( "Missing tilde after start room of move_prog data in mobprog file.", 0);
            exit( 1 );
        }
        script->script = fread_string( progfile, &stat );
        switch ( letter = fread_letter( progfile ) )
        {
          case '>':
             mprg2->next = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
             mprg2       = mprg2->next;
             mprg2->next = NULL;
           break;
          case '|':
             done = TRUE;
           break;
          default:
             bug( "in mobprog file syntax error - bad move_prog data.", 0 );
             exit( 1 );
           break;
        }
        break;
     default:
        pMobIndex->progtypes = pMobIndex->progtypes | mprg2->type;
        mprg2->arglist       = fread_string( progfile, &stat );
        mprg2->comlist       = fread_string( progfile, &stat );
        switch ( letter = fread_letter( progfile ) )
        {
          case '>':
             mprg2->next = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
             mprg2       = mprg2->next;
             mprg2->next = NULL;
           break;
          case '|':
             done = TRUE;
           break;
          default:
             bug( "in mobprog file syntax error - bad prog data.", 0 );
             exit( 1 );
           break;
        }
      break;
    }
  }
  fclose( progfile );
  return mprg2;
}

/*
 * This routine reads in scripts of MOBprograms from a file.
 */
MPROG_DATA* oprog_file_read( const char *f, MPROG_DATA *oprg,
			    OBJ_INDEX_DATA *pObjIndex )
{
  char        OBJProgfile[ MAX_INPUT_LENGTH ];
  MPROG_DATA *oprg2;
  FILE       *progfile;
  char        letter;
  bool        done = FALSE;
  int         stat;

  last_fun( "oprog_file_read");
  sprintf( OBJProgfile, "%s%s%s", executable_directory, OBJ_DIR, f );

  progfile = fopen( OBJProgfile, "r" );
  if ( !progfile )
  {
     bug( "Obj: %d couldnt open objprog file", pObjIndex->vnum );
     exit( 1 );
  }

  oprg2 = oprg;
  switch ( letter = fread_letter( progfile ) )
  {
    case '>':
     break;
    case '|':
       bug( "empty objprog file.", 0 );
       exit( 1 );
     break;
    default:
       bug( "in objprog file syntax error - no letter.", 0 );
       exit( 1 );
     break;
  }

  while ( !done )
  {
    oprg2->type = prog_name_to_type( fread_word( progfile, &stat ) );
    switch ( oprg2->type )
    {
     case ERROR_PROG:
        bug( "objprog file type error", 0 );
        exit( 1 );
      break;
     case IN_FILE_PROG:
        bug( "prog file contains a call to file.", 0 );
        exit( 1 );
      break;
     default:
        pObjIndex->progtypes = pObjIndex->progtypes | oprg2->type;
        oprg2->arglist       = fread_string( progfile, &stat );
        oprg2->comlist       = fread_string( progfile, &stat );
        switch ( letter = fread_letter( progfile ) )
        {
          case '>':
             oprg2->next = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
             oprg2       = oprg2->next;
             oprg2->next = NULL;
           break;
          case '|':
             done = TRUE;
           break;
          default:
             bug( "in objprog file syntax error - bad data.", 0 );
             exit( 1 );
           break;
        }
      break;
    }
  }
  fclose( progfile );
  return oprg2;
}

void load_mobprogs( FILE *fp )
{
  MOB_INDEX_DATA *iMob;
  MPROG_DATA     *original;
  MPROG_DATA     *working;
  char            letter;
  int             value;
  int             stat;

  last_fun( "load_mobprogs");

  for ( ; ; )
    switch ( letter = fread_letter( fp ) )
    {
    default:
      bug( "Load_mobprogs: bad command '%c'.",letter);
      exit(1);
      break;
    case 'S':
    case 's':
      fread_to_eol( fp ); 
      return;
    case '*':
      fread_to_eol( fp ); 
      break;
    case 'M':
    case 'm':
      value = fread_number( fp, &stat );
      if ( !( iMob = get_mob_index( value ) ) )
      {
	bug( "Load_mobprogs: vnum %d doesnt exist", value );
	exit( 1 );
      }
    
      /* Go to the end of the prog command list if other commands exist */

      if ( ( original = iMob->mobprogs ) )
	for ( ; original->next; original = original->next );

      working = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
      if ( original )
	original->next = working;
      else
	iMob->mobprogs = working;
      working       = mprog_file_read( fread_word( fp, &stat ), working, iMob );
      working->next = NULL;
      fread_to_eol( fp );
      break;
    }

  return;

} 

void load_objprogs( FILE *fp )
{
  OBJ_INDEX_DATA *iObj;
  MPROG_DATA     *original;
  MPROG_DATA     *working;
  char            letter;
  int             value;
  int             stat;

  last_fun( "load_objprogs");

  for ( ; ; )
    switch ( letter = fread_letter( fp ) )
    {
    default:
      bug( "Load_objprogs: bad command '%c'.",letter);
      exit(1);
      break;
    case 'S':
    case 's':
      fread_to_eol( fp ); 
      return;
    case '*':
      fread_to_eol( fp ); 
      break;
    case 'M':
    case 'm':
      value = fread_number( fp, &stat );
      if ( !( iObj = get_obj_index( value ) ) )
      {
	bug( "Load_objprogs: vnum %d doesnt exist", value );
	exit( 1 );
      }
    
      /* Go to the end of the prog command list if other commands exist */

      if ( ( original = iObj->objprogs ) )
	for ( ; original->next; original = original->next );

      working = (MPROG_DATA *)alloc_perm( sizeof( MPROG_DATA ) );
      if ( original )
	original->next = working;
      else
	iObj->objprogs = working;
      working       = oprog_file_read( fread_word( fp, &stat ), working, iObj );
      working->next = NULL;
      fread_to_eol( fp );
      break;
    }

  return;

} 

// Converts the classes used in the editor to the classes used on the MUD.
//
// Though having a need for this function is kind of wasteful, it does
// allow us to translate a class in the editor to one int the game if
// we were to do something like remove rangers or assassins yet still
// wanted the mobs to have the warrior and thief abilities without
// having to rewrite the whole zone - Veygoth
int class_convert( int class )
{
	switch( class )
        {
           default:
              bug( "Invalid class %d!", class );
              return CLASS_NONE;
              break;
           case 0:
              return CLASS_NONE;
              break;
           case 3:
              return CLASS_PSIONICIST;
              break;
           case 2:
              return CLASS_RANGER;
              break;
           case 1:
           case 7: // Monk
              return CLASS_WARRIOR;
              break;
           case 4:
              return CLASS_PALADIN;
              break;
           case 5:
              return CLASS_ANTIPALADIN;
              break;
           case 6:
              return CLASS_CLERIC;
              break;
           case 8:
              return CLASS_DRUID;
              break;
           case 9:
              return CLASS_SHAMAN;
              break;
           case 10:
           case 11: // Necromancer
              return CLASS_SORCERER;
              break;
           case 12: // Conjurer = random elementalist
              switch( number_range( 1, 4 ))
              {
                default: return CLASS_NONE; break;
                case 1: return CLASS_ELEMENT_AIR; break;
                case 2: return CLASS_ELEMENT_EARTH; break;
                case 3: return CLASS_ELEMENT_FIRE; break;
                case 4: return CLASS_ELEMENT_WATER; break;
              }
              break;
           case 13:
              return CLASS_THIEF;
              break;
           case 14:
              return CLASS_ASSASSIN;
              break;
           case 15:
              return CLASS_MERCENARY;
              break;
           case 16:
              return CLASS_BARD;
              break;
           case 17:
              return CLASS_ILLUSIONIST;
              break;
           case 18:
              return CLASS_ELEMENT_FIRE;
              break;
           case 19:
              return CLASS_ELEMENT_EARTH;
              break;
           case 20:
              return CLASS_ELEMENT_AIR;
              break;
           case 21:
              return CLASS_ELEMENT_WATER;
              break;
           case 22:
              return CLASS_HUNTER;
              break;
        };


}

// Checks whether a particular vnum is flagged artifact.
bool is_artifact( int vnum )
{
    OBJ_INDEX_DATA * pObj;

    last_fun( "is_artifact" );

    pObj = get_obj_index( vnum );
    if( pObj != NULL
        && IS_SET( pObj->extra_flags[ITEM_ARTIFACT.group], ITEM_ARTIFACT.vector ) )
        return TRUE;

    return FALSE;
}

// Converts Envy races whose names don't match Magma races.
int envy_race_convert( char* name )
{
    if( !str_cmp( name, "elf" ))
    {
        return race_lookup( "Grey Elf" );
    }
    else if( !str_cmp( name, "hobbit" ))
    {
        return race_lookup( "Halfling" );
    }
    else if( !str_cmp( name, "halfelf" ))
    {
        return race_lookup( "Half-Elf" );
    }
    else if( !str_cmp( name, "mindflayer" ))
    {
        return race_lookup( "Illithid" );
    }
    else if( !str_cmp( name, "halfdwarf" ))
    {
        return race_lookup( "Half-Dwarf" );
    }
    else if( !str_cmp( name, "lizard" ))
    {
        return race_lookup( "Reptile" );
    }
    else if( !str_cmp( name, "elemental" ))
    {
        // Since it doesn't specify a type we just pick a random one.
        switch( number_range(1,4))
        {
        case 1:
            return race_lookup( "Air Elemental" );
            break;
        case 2:
            return race_lookup( "Earth Elemental" );
            break;
        case 3:
            return race_lookup( "Fire Elemental" );
            break;
        case 4:
            return race_lookup( "Water Elemental" );
            break;
        }

    }
    return -1;
}

// Converts Envy item type to a Magma item type.
int convert_envy_item_type( int type )
{
    switch( type )
    {
    case 23:
        return 38;
    case 24:
        return 39;
    case 25:
        return 17;
    case 26:
        return 41;
    default:
        return type;
    }
}

int convert_envy_act_flags( int flags )
{
    if( IS_SET( flags, BV00 ))
    {
        REMOVE_BIT(flags,BV00);
        SET_BIT(flags,BV03);
    }
    if( IS_SET( flags, BV08 ))
    {
        REMOVE_BIT(flags,BV08);
        SET_BIT(flags,BV21);
    }
    if( IS_SET( flags, BV09 ))
    {
        REMOVE_BIT(flags,BV09);
        SET_BIT(flags,BV15);
    }
    if( IS_SET( flags, BV10 ))
    {
        REMOVE_BIT(flags,BV10);
        SET_BIT(flags,BV15);
    }
    if( IS_SET( flags, BV17 ))
    {
        REMOVE_BIT(flags,BV17);
        SET_BIT(flags,BV20);
    }
    // ACT_GAMBLE does not exist here.
    if( IS_SET( flags, BV12 ))
    {
        REMOVE_BIT(flags,BV12);
    }
    return flags;
}

void convert_envy_affect_flags( int* flags )
{
    if( IS_SET( flags[0], BV08 ))
    {
        REMOVE_BIT(flags[0],BV08);
        SET_BIT(flags[4],BV05);
    }
    if( IS_SET( flags[0], BV10 ))
    {
        REMOVE_BIT(flags[0],BV10);
        SET_BIT(flags[4],BV08);
    }
    if( IS_SET( flags[0], BV11 ))
    {
        REMOVE_BIT(flags[0],BV11);
        SET_BIT(flags[4],BV07);
    }
    if( IS_SET( flags[0], BV14 ))
    {
        REMOVE_BIT(flags[0],BV14);
        SET_BIT(flags[4],BV10);
    }
    if( IS_SET( flags[0], BV25 ))
    {
        REMOVE_BIT(flags[0],BV25);
        SET_BIT(flags[2],BV05);
    }
    if( IS_SET( flags[0], BV23 ))
    {
        REMOVE_BIT(flags[0],BV15);
        SET_BIT(flags[4],BV04);
    }
    if( IS_SET( flags[0], BV02 ))
    {
        REMOVE_BIT(flags[0],BV02);
        SET_BIT(flags[1],BV02);
    }
    if( IS_SET( flags[0], BV04 ))
    {
        REMOVE_BIT(flags[0],BV04);
        SET_BIT(flags[1],BV04);
    }
    if( IS_SET( flags[0], BV06 ))
    {
        REMOVE_BIT(flags[0],BV06);
        SET_BIT(flags[1],BV09);
    }
    if( IS_SET( flags[0], BV07 ))
    {
        REMOVE_BIT(flags[0],BV07);
        SET_BIT(flags[1],BV15);
    }
    if( IS_SET( flags[0], BV12 ))
    {
        REMOVE_BIT(flags[0],BV12);
        SET_BIT(flags[1],BV14);
    }
    if( IS_SET( flags[0], BV20 ))
    {
        REMOVE_BIT(flags[0],BV20);
        SET_BIT(flags[1],BV27);
    }
    if( IS_SET( flags[0], BV27 ))
    {
        REMOVE_BIT(flags[0],BV27);
        SET_BIT(flags[1],BV00);
    }
    // We have changed that affect flags[0] last to prevent cascading changes
    // (overwriting a bit that will cause another change to trigger)
    if( IS_SET( flags[0], BV09 ))
    {
        REMOVE_BIT(flags[0],BV09);
        SET_BIT(flags[0],BV25);
    }
    // This one *MUST* go before the flag change that sets bit #19 or things will break.
    if( IS_SET( flags[0], BV19 ))
    {
        REMOVE_BIT(flags[0],BV19);
        SET_BIT(flags[0],BV27);
    }
    if( IS_SET( flags[0], BV15 ))
    {
        REMOVE_BIT(flags[0],BV15);
        SET_BIT(flags[0],BV19);
    }
    if( IS_SET( flags[0], BV16 ))
    {
        REMOVE_BIT(flags[0],BV16);
        SET_BIT(flags[0],BV20);
    }
    if( IS_SET( flags[0], BV18 ))
    {
        REMOVE_BIT(flags[0],BV18);
        SET_BIT(flags[0],BV22);
    }
    if( IS_SET( flags[0], BV21 ))
    {
        REMOVE_BIT(flags[0],BV21);
        SET_BIT(flags[0],BV11);
    }
    if( IS_SET( flags[0], BV24 ))
    {
        REMOVE_BIT(flags[0],BV24);
        SET_BIT(flags[0],BV11);
    }
    if( IS_SET( flags[0], BV26 ))
    {
        REMOVE_BIT(flags[0],BV26);
        SET_BIT(flags[0],BV10);
    }
}

int convert_envy_room_flags( int flags )
{
    if( IS_SET( flags, BV04 ))
    {
        REMOVE_BIT(flags,BV04);
        SET_BIT(flags,BV08);
    }
    if( IS_SET( flags, BV10 ))
    {
        REMOVE_BIT(flags,BV10);
        SET_BIT(flags,BV11);
    }
    if( IS_SET( flags, BV13 ))
    {
        REMOVE_BIT(flags,BV13);
        SET_BIT(flags,BV06);
    }
    if( IS_SET( flags, BV14 ))
    {
        REMOVE_BIT(flags,BV14);
        SET_BIT(flags,BV04);
    }
    return flags;
}

int convert_envy_exit_flags( int flags )
{
    if( IS_SET( flags, BV03 ))
    {
        REMOVE_BIT(flags,BV03);
        SET_BIT(flags,BV12);
    }
    if( IS_SET( flags, BV04 ))
    {
        REMOVE_BIT(flags,BV04);
        SET_BIT(flags,BV13);
    }
    if( IS_SET( flags, BV05 ))
    {
        REMOVE_BIT(flags,BV05);
        SET_BIT(flags,BV08);
    }
    if( IS_SET( flags, BV06 ))
    {
        REMOVE_BIT(flags,BV06);
        SET_BIT(flags,BV14);
    }
    return flags;
}

