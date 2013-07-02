#ifndef _MOBBITS_H_

#include "../types.h"

// mob "action bit" defines

#define NUMB_MOBACT_FLAGS        32

// bits 1, 21, 22, 23, and 24 are unused.
// bits 21 and 22 should never be used.
#define ACT_SENTINEL           BIT_2    /* this mobile not to be moved         */
#define ACT_SCAVENGER          BIT_3    /* pick up stuff lying around          */
#define ACT_ISNPC              BIT_4    /* This bit is set for use with IS_NPC */
#define ACT_NICE_THIEF         BIT_5    /* Set if a thief should NOT be killed */
#define ACT_AGGRESSIVE         BIT_6    /* Set if automatic attack on PC's     */
#define ACT_STAY_ZONE          BIT_7    /* MOB Must stay inside its own zone   */
#define ACT_WIMPY              BIT_8    /* MOB Will flee when injured          */
#define ACT_AGGRESSIVE_EVIL    BIT_9
#define ACT_AGGRESSIVE_GOOD    BIT_10
#define ACT_AGGRESSIVE_NEUTRAL BIT_11
#define ACT_MEMORY             BIT_12
#define ACT_IMMUNE_TO_PARA     BIT_13
#define ACT_NO_SUMMON          BIT_14   /* mob can't be summoned */

#define ACT_TEACHER            BIT_16   /* guildmaster */
#define NPC_OUTLAW             BIT_17   /* temp kludge */
#define ACT_CANFLY             BIT_18
#define ACT_CANSWIM            BIT_19
#define ACT_NOTK               BIT_20

#define ACT_WITNESS            BIT_25
#define ACT_BREAK_CHARM        BIT_26
#define ACT_PROTECTOR          BIT_27
#define ACT_MOUNT              BIT_28   /* MOB can be mounted by player -DCL */
#define ACT_AGG_RACEEVIL       BIT_29   /* Aggressive towards evil races. */
#define ACT_AGG_RACEGOOD       BIT_30   /* Aggressive to good races. --MIAX */
#define ACT_HUNTER             BIT_31   /* killeristic hunt mode, in which
                                         mob hunts regardless of sentinel */
#define ACT_AGG_OUTCAST        BIT_32   /* Mob aggroes on outcasts */


// mob "action bits" bit flag structure and union

typedef struct _mobActionBitFlags
{
  unsigned spec        : 1;
  unsigned sentinel    : 1;
  unsigned scavenger   : 1;
  unsigned isnpc       : 1;
  unsigned niceThief   : 1;
  unsigned aggressive  : 1;
  unsigned stayZone    : 1;
  unsigned wimpy       : 1;
  unsigned aggresEvil  : 1;
  unsigned aggresGood  : 1;
  unsigned aggresNeut  : 1;
  unsigned memory      : 1;
  unsigned noPara      : 1;
  unsigned unused2     : 1;
  unsigned unused3     : 1;
  unsigned teacher     : 1;
  unsigned unused4     : 1;
  unsigned canFly      : 1;
  unsigned canSwim     : 1;
  unsigned unused5     : 1;
  unsigned unused6     : 1;
  unsigned unused7     : 1;
  unsigned unused8     : 1;
  unsigned specDie     : 1;
  unsigned citizen     : 1;
  unsigned breakCharm  : 1;
  unsigned protector   : 1;
  unsigned mount       : 1;
  unsigned aggrEvilR   : 1;
  unsigned aggrGoodR   : 1;
  unsigned hunter      : 1;
  unsigned aggrOutcast : 1;
} mobActionBitFlags;

typedef union _mobActionFlagRec
{
  ulong longIntFlags;

  mobActionBitFlags mobActionBits;
} mobActionFlagRec;


// mob "affect1 bit" defines

#define NUMB_AFF1_FLAGS           32

#define AFF_BLIND             BIT_1
#define AFF_INVISIBLE         BIT_2
#define AFF_FARSEE            BIT_3
#define AFF_DETECT_INVISIBLE  BIT_4
#define AFF_HASTE             BIT_5
#define AFF_SENSE_LIFE        BIT_6
#define AFF_MINOR_GLOBE       BIT_7
#define AFF_STONE_SKIN        BIT_8
#define AFF_UD_VISION         BIT_9
#define AFF_SHADOW            BIT_10
#define AFF_WRAITHFORM        BIT_11
#define AFF_WATERBREATH       BIT_12
#define AFF_KNOCKED_OUT       BIT_13
#define AFF_PROTECT_EVIL      BIT_14
#define AFF_BOUND             BIT_15
#define AFF_SLOW_POISON       BIT_16
#define AFF_PROTECT_GOOD      BIT_17
#define AFF_SLEEP             BIT_18
#define AFF_SKILL_AWARE       BIT_19    /* for awareness skill --TAM 7-9-94 */
#define AFF_SNEAK             BIT_20
#define AFF_HIDE              BIT_21
#define AFF_FEAR              BIT_22
#define AFF_CHARM             BIT_23
#define AFF_MEDITATE          BIT_24
#define AFF_BARKSKIN          BIT_25
#define AFF_INFRAVISION       BIT_26
#define AFF_LEVITATE          BIT_27
#define AFF_FLY               BIT_28
#define AFF_AWARE             BIT_29
#define AFF_PROT_FIRE         BIT_30
#define AFF_CAMPING           BIT_31
#define AFF_BIOFEEDBACK       BIT_32


// mob "affect1" bit flag structure and union

typedef struct _affect1BitFlags
{
  unsigned blind       : 1;
  unsigned invisible   : 1;
  unsigned farsee      : 1;
  unsigned detectInv   : 1;
  unsigned haste       : 1;
  unsigned senseLife   : 1;
  unsigned minorGlobe  : 1;
  unsigned stoneSkin   : 1;
  unsigned ud_vision   : 1;
  unsigned shadow      : 1;
  unsigned wraithForm  : 1;
  unsigned waterBreath : 1;
  unsigned knockedOut  : 1;
  unsigned protectEvil : 1;
  unsigned bound       : 1;
  unsigned slowPoison  : 1;
  unsigned protectGood : 1;
  unsigned sleep       : 1;
  unsigned skillAware  : 1;
  unsigned sneak       : 1;
  unsigned hide        : 1;
  unsigned fear        : 1;
  unsigned charm       : 1;
  unsigned meditate    : 1;
  unsigned barkskin    : 1;
  unsigned infravision : 1;
  unsigned levitate    : 1;
  unsigned fly         : 1;
  unsigned aware       : 1;
  unsigned protFire    : 1;
  unsigned camping     : 1;
  unsigned biofeedback : 1;
} affect1BitFlags;

typedef union _affect1FlagRec
{
  ulong longIntFlags;

  affect1BitFlags affect1Bits;
} affect1FlagRec;


// mob "affect2 bit" defines

#define NUMB_AFF2_FLAGS              32

#define AFF2_FIRESHIELD       BIT_1
#define AFF2_ULTRAVISION      BIT_2
#define AFF2_DETECT_EVIL      BIT_3
#define AFF2_DETECT_GOOD      BIT_4
#define AFF2_DETECT_MAGIC     BIT_5
#define AFF2_MAJOR_PHYSICAL   BIT_6
#define AFF2_PROT_COLD        BIT_7
#define AFF2_PROT_LIGHTNING   BIT_8
#define AFF2_MINOR_PARALYSIS  BIT_9
#define AFF2_MAJOR_PARALYSIS  BIT_10
#define AFF2_SLOW             BIT_11
#define AFF2_GLOBE            BIT_12
#define AFF2_PROT_GAS         BIT_13
#define AFF2_PROT_ACID        BIT_14
#define AFF2_POISONED         BIT_15
#define AFF2_SOULSHIELD       BIT_16
#define AFF2_SILENCED         BIT_17
#define AFF2_MINOR_INVIS      BIT_18
#define AFF2_VAMPIRIC_TOUCH   BIT_19
#define AFF2_STUNNED          BIT_20
#define AFF2_DROPPED_PRIM     BIT_21
#define AFF2_DROPPED_SECOND   BIT_22
#define AFF2_FUMBLING_PRIM    BIT_23
#define AFF2_FUMBLING_SECOND  BIT_24
#define AFF2_HOLDING_BREATH   BIT_25    /* Underwater */
#define AFF2_MEMORIZING       BIT_26
#define AFF2_IS_DROWNING      BIT_27    /* Underwater */
#define AFF2_PASSDOOR         BIT_28
#define AFF2_DRAINING         BIT_29
#define AFF2_CASTING          BIT_30
#define AFF2_SCRIBING         BIT_31
#define AFF2_HUNTER           BIT_32


// mob "affect2" bit flag structure and union

typedef struct _affect2BitFlags
{
  unsigned fireshield  : 1;
  unsigned ultravision : 1;
  unsigned detectEvil  : 1;
  unsigned detectGood  : 1;
  unsigned detectMagic : 1;
  unsigned majorPhys   : 1;
  unsigned protCold    : 1;
  unsigned protLight   : 1;
  unsigned minorPara   : 1;
  unsigned majorPara   : 1;
  unsigned slow        : 1;
  unsigned majorGlobe  : 1;
  unsigned protGas     : 1;
  unsigned protAcid    : 1;
  unsigned poisoned    : 1;
  unsigned soulshield  : 1;
  unsigned silenced    : 1;
  unsigned minor_invis : 1;
  unsigned vamp_touch  : 1;
  unsigned stunned     : 1;
  unsigned droppedPrim : 1;
  unsigned droppedScnd : 1;
  unsigned fumblePrim  : 1;
  unsigned fumbleScnd  : 1;
  unsigned hold_breath : 1;
  unsigned memorizing  : 1;
  unsigned drowning    : 1;
  unsigned passdoor    : 1;
  unsigned draining    : 1;
  unsigned casting     : 1;
  unsigned scribing    : 1;
  unsigned hunter      : 1;
} affect2BitFlags;

typedef union _affect2FlagRec
{
  ulong longIntFlags;

  affect2BitFlags affect2Bits;
} affect2FlagRec;

/* affected_by 3 */

#define NUMB_AFF3_FLAGS         32

#define AFF3_TENSORS_DISC       BIT_1
#define AFF3_TRACKING           BIT_2
#define AFF3_SINGING            BIT_3
#define AFF3_ECTOPLASMIC_FORM   BIT_4
#define AFF3_ABSORBING          BIT_5
#define AFF3_DRAINEE            BIT_6
#define AFF3_SPIRIT_WARD        BIT_7
#define AFF3_GR_SPIRIT_WARD     BIT_8
#define AFF3_NON_DETECTION      BIT_9
#define AFF3_SILVER             BIT_10    /* Char needs silver to dam  */
#define AFF3_PLUSONE            BIT_11    /* Char needs +1 to damage   */
#define AFF3_PLUSTWO            BIT_12    /* Char needs +2 to damage   */
#define AFF3_PLUSTHREE          BIT_13    /* Char needs +3 to damage   */
#define AFF3_PLUSFOUR           BIT_14    /* Char needs +4 to damage   */
#define AFF3_PLUSFIVE           BIT_15    /* Char needs +5 to damage   */
#define AFF3_ENLARGE            BIT_16
#define AFF3_REDUCE             BIT_17
#define AFF3_COVER              BIT_18
#define AFF3_FOUR_ARMS          BIT_19
#define AFF3_INERTIAL_BARRIER   BIT_20
#define AFF3_INTELLECT_FORT     BIT_21
#define AFF3_COLDSHIELD         BIT_22
#define AFF3_CANIBALIZE         BIT_23
#define AFF3_SWIMMING           BIT_24
#define AFF3_TOWER_IRON_WILL    BIT_25
#define AFF3_UNDERWATER         BIT_26
#define AFF3_BLUR		BIT_27
#define AFF3_NECKBITING         BIT_28   /* gith mana regain action */
#define AFF3_ELEMENTAL_FORM     BIT_29
#define AFF3_PASS_WITHOUT_TRACE BIT_30
#define AFF3_PALADIN_AURA       BIT_31
#define AFF3_FAMINE		BIT_32

// mob "affect3" bit flag structure and union

typedef struct _affect3BitFlags
{
  unsigned tensors     : 1;
  unsigned tracking    : 1;
  unsigned singing     : 1;
  unsigned ectoplasmic : 1;
  unsigned absorbing   : 1;
  unsigned drainee     : 1;
  unsigned spirit_ward : 1;
  unsigned greater_sw  : 1;
  unsigned non_detect  : 1;
  unsigned silver      : 1;
  unsigned plusone     : 1;
  unsigned plustwo     : 1;
  unsigned plusthree   : 1;
  unsigned plusfour    : 1;
  unsigned plusfive    : 1;
  unsigned enlarged    : 1;
  unsigned reduced     : 1;
  unsigned cover       : 1;
  unsigned four_arms   : 1;
  unsigned inertial    : 1;
  unsigned int_fort    : 1;
  unsigned coldshield  : 1;
  unsigned canibalize  : 1;
  unsigned swimming    : 1;
  unsigned tower_will  : 1;
  unsigned underwater  : 1;
  unsigned blur        : 1;
  unsigned neckbiting  : 1;
  unsigned ele_form    : 1;
  unsigned no_trace    : 1;
  unsigned pal_aura    : 1;
  unsigned famine      : 1;
} affect3BitFlags;

typedef union _affect3FlagRec
{
  ulong longIntFlags;

  affect3BitFlags affect3Bits;
} affect3FlagRec;

/* affected_by 4 */

#define NUMB_AFF4_FLAGS         20

#define AFF4_LOOTER             BIT_1 /* Just looted someone, prevent rent */
#define AFF4_CARRY_DISEASE      BIT_2
#define AFF4_SACKING            BIT_3 /* sacking a guildhall */
#define AFF4_SENSE_FOLLOWER     BIT_4
#define AFF4_STORNOGS_SPHERES   BIT_5
#define AFF4_STORNOGS_GREATER_SPHERES BIT_6
#define AFF4_VAMPIRE_FORM       BIT_7
#define AFF4_NO_UNMORPH         BIT_8 /* can't return .. */
#define AFF4_HOLY_SACRIFICE     BIT_9
#define AFF4_BATTLE_ECSTASY     BIT_10
#define AFF4_DAZZLER            BIT_11
#define AFF4_DAZZLEE            BIT_12
#define AFF4_THROAT_CRUSH       BIT_13
#define AFF4_REGENERATION       BIT_14
#define AFF4_BEARHUG            BIT_15
#define AFF4_GRAPPLE            BIT_16
#define AFF4_GRAPPLED           BIT_17
#define AFF4_MAGE_FLAME         BIT_18 /* magic torch floating above char */
#define AFF4_NO_IMMOLATE        BIT_19
#define AFF4_MULTICLASS         BIT_20 /* for multiclassed npcs */

// mob "affect4" bit flag structure and union

typedef struct _affect4BitFlags
{
  unsigned looter      : 1;
  unsigned diseased    : 1;
  unsigned sacking     : 1;
  unsigned sense_fol   : 1;
  unsigned spheres     : 1;
  unsigned gr_spheres  : 1;
  unsigned vamp_form   : 1;
  unsigned no_unmorph  : 1;
  unsigned holy_sac    : 1;
  unsigned battle_xtc  : 1;
  unsigned dazzler     : 1;
  unsigned dazzlee     : 1;
  unsigned throatcrush : 1;
  unsigned regen       : 1;
  unsigned bearhug     : 1;
  unsigned grapple     : 1;
  unsigned grappled    : 1;
  unsigned mage_flame  : 1;
  unsigned no_immolate : 1;
  unsigned multiclass  : 1;
  unsigned no_tk       : 1;
  unsigned unused2     : 1;
  unsigned unused3     : 1;
  unsigned unused4     : 1;
  unsigned unused5     : 1;
  unsigned unused6     : 1;
  unsigned unused7     : 1;
  unsigned unused8     : 1;
  unsigned unused9     : 1;
  unsigned unused10    : 1;
  unsigned unused11    : 1;
  unsigned unused12    : 1;
} affect4BitFlags;

typedef union _affect4FlagRec
{
  ulong longIntFlags;

  affect4BitFlags affect4Bits;
} affect4FlagRec;

#define _MOBBITS_H_
#endif
