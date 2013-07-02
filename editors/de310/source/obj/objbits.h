// OBJBITS.H - constants and structures related to object flags

#ifndef _OBJBITS_H_

#include "../misc/bitdef.h"

// item "extra bit" defines

#define NUMB_OBJEBIT_FLAGS        32    // pretty self-explanatory

#define ITEM_GLOW                  1  // The item is glowing ** ONLY on GOD Items **
#define ITEM_NOSHOW                2  // it doesn't show up, blah
#define ITEM_BURIED                4  // object is buried
#define ITEM_LOCK                  8  // DO NOT USE
#define ITEM_EVIL                 16  // DO NOT USE
#define ITEM_INVISIBLE            32  // Item is invisible.
#define ITEM_MAGIC                64  // Item will show an aura when detect magic is used
#define ITEM_NODROP              128  // Item can not be dropped (cursed for example)
#define ITEM_BLESS               256  // Item is blessed.
#define ITEM_ANTI_GOOD           512  // not usable by good people
#define ITEM_ANTI_EVIL          1024  // not usable by evil people
#define ITEM_ANTI_NEUTRAL       2048  // not usable by neutral people
#define ITEM_SECRET             4096  // The item is only detectable by "search"
#define ITEM_FLOAT              8192  // Will float on water
#define ITEM_NOBURN            16384  // Item cant burn from dragon fire.
#define ITEM_NOLOCATE          32768  // Item cant be located with locate obj spell
#define ITEM_NOIDENTIFY        65536  // Item cannot be identified.
#define ITEM_NOSUMMON         131072  // Item will not allow user to be summoned.
#define ITEM_LIT              262144  // Item is lit by spell
#define ITEM_TRANSIENT        524288  // Item is transient (newbie items)
#define ITEM_NOSLEEP         1048576  // When item is worn pc cant be slept.
#define ITEM_NOCHARM         2097152  // When item is worn pc cant be charmed.
#define ITEM_TWOHANDS        4194304  // Item requires two hands to wield.
#define ITEM_NORENT          8388608  // Item cant be rented.
#define ITEM_GOOD           16777216  // Item used by good only.
#define ITEM_ANTI_WA        33554432  // Item cant be used by warriors.
#define ITEM_ANTI_CL        67108864  // Item cant be used by cleric.
#define ITEM_ANTI_TH       134217728  // Item cant be used by a thief.
#define ITEM_ANTI_MU       268435456  // Item cant be used by a mage.
#define ITEM_WHOLE_BODY    536870912  // Item goes around entire body when worn.
#define ITEM_WHOLE_HEAD   1073741824  // Item goes around entire head.
#define ITEM_WAS_DISARMED 2147483648  // DO NOT USE

// object "extra bits" bit flag structure and union

typedef struct _objExtraBitFlags
{
  unsigned glow        : 1;
  unsigned noshow      : 1;
  unsigned buried      : 1;
  unsigned lock        : 1;
  unsigned evil        : 1;
  unsigned invisible   : 1;
  unsigned magic       : 1;
  unsigned noDrop      : 1;
  unsigned bless       : 1;
  unsigned antiGood    : 1;
  unsigned antiEvil    : 1;
  unsigned antiNeutral : 1;
  unsigned secret      : 1;
  unsigned itemFloat   : 1;
  unsigned noBurn      : 1;
  unsigned noLocate    : 1;
  unsigned noIdentify  : 1;
  unsigned noSummon    : 1;
  unsigned lit         : 1;
  unsigned transient   : 1;
  unsigned noSleep     : 1;
  unsigned noCharm     : 1;
  unsigned twoHands    : 1;
  unsigned noRent      : 1;
  unsigned good        : 1;
  unsigned hum         : 1;
  unsigned levitates   : 1;
  unsigned armorThin   : 1;
  unsigned armorThick  : 1;
  unsigned wholeBody   : 1;
  unsigned wholeHead   : 1;
  unsigned wasDisarmed : 1;
} objExtraBitFlags;

typedef union _objExtraFlagRec
{
  ulong longIntFlags;

  objExtraBitFlags objExtraBits;
} objExtraFlagRec;


// item "extra2 flag" defines

#define NUMB_OBJE2BIT_FLAGS     24

#define ITEM2_SILVER         BIT_1    /* Item harm AFF_SILVER         */
#define ITEM2_PLUSONE        BIT_2    /* Item harm upto AFF_PLUSONE   */
#define ITEM2_PLUSTWO        BIT_3    /* Item harm upto AFF_PLUSTWO   */
#define ITEM2_PLUSTHREE      BIT_4    /* Item harm upto AFF_PLUSTHREE */
#define ITEM2_PLUSFOUR       BIT_5    /* Item harm upto AFF_PLUSFOUR  */
#define ITEM2_PLUSFIVE       BIT_6    /* Item harm upto AFF_PLUSFIVE  */
#define ITEM2_RETURNING      BIT_7    /* Boomerang item, returns      */
#define ITEM2_CAN_THROW1     BIT_8    /* Item can be thrown 1 room    */
#define ITEM2_CAN_THROW2     BIT_9    /* Item can be thrown 2 rooms   */
#define ITEM2_ARTIFACT       BIT_10   /* item is unique in game */
#define ITEM2_ENLARGE        BIT_17   /* item been enlarge */
#define ITEM2_REDUCE         BIT_18   /* item been reduce  */
#define ITEM2_POISONED       BIT_19
#define ITEM2_FLY            BIT_20
#define ITEM2_SAVE           BIT_21
#define ITEM2_ALTERED        BIT_22
#define ITEM2_NOFLEE         BIT_23   /* Prevents wielder from fleeing */
#define ITEM2_NOREPAIR       BIT_24   /* Cannot be repaired. */
#if 0

// old new combat bits

#define ITEM2_ENCHANT_FIRE   BIT_10   /* Item enchanted against fire  */
#define ITEM2_ENCHANT_ACID   BIT_11   /* Item enchanted against acid  */
#define ITEM2_ENCHANT_COLD   BIT_12   /* Item enchanted against cold  */
#define ITEM2_ENCHANT_LIGHTN BIT_13   /* Item enchanted against lightn*/
#define ITEM2_ENCHANT_GAS    BIT_14   /* Item enchanted against gas   */
#define ITEM2_ENCHANT_GOOD   BIT_15   /* Item enchanted against good  */
#define ITEM2_ENCHANT_EVIL   BIT_16   /* Item enchanted against evil  */
#define ITEM2_NOMORE_ENCHANT BIT_17   /* Item can have no more ench.  */

#endif

// object "extra2 bits" bit flag structure and union

typedef struct _objExtra2BitFlags
{
  unsigned silver      : 1;
  unsigned plusone     : 1;
  unsigned plustwo     : 1;
  unsigned plusthree   : 1;
  unsigned plusfour    : 1;
  unsigned plusfive    : 1;
  unsigned returning   : 1;
  unsigned canthrow1   : 1;
  unsigned canthrow2   : 1;
  unsigned artifact    : 1;
  unsigned unused11    : 1;
  unsigned unused12    : 1;
  unsigned unused13    : 1;
  unsigned unused14    : 1;
  unsigned unused15    : 1;
  unsigned unused16    : 1;
  unsigned enlarged    : 1;
  unsigned reduced     : 1;
  unsigned poisoned    : 1;
  unsigned fly         : 1;
  unsigned save        : 1;
  unsigned altered     : 1;
  unsigned noflee      : 1;
  unsigned norepair    : 1;

/*

 // old new combat bits

  unsigned enchantFire : 1;
  unsigned enchantAcid : 1;
  unsigned enchantCold : 1;
  unsigned enchantLigh : 1;
  unsigned enchantGas  : 1;
  unsigned enchantGood : 1;
  unsigned enchantEvil : 1;
  unsigned nomoreEnch  : 1;
*/

  unsigned slack       : 14;
} objExtra2BitFlags;

typedef union _objExtra2FlagRec
{
  ulong longIntFlags;

  objExtra2BitFlags objExtra2Bits;
} objExtra2FlagRec;


// item "wear flag" defines

#define NUMB_OBJWBIT_FLAGS      28 // pretty self-explanatory

#define ITEM_TAKE                1 // Item is takeable.
#define ITEM_WEAR_FINGER         2 // Can be worn on a finger (rings usually)
#define ITEM_WEAR_NECK           4 // Can be worn around neck.
#define ITEM_WEAR_BODY           8 // Can be worn on body.
#define ITEM_WEAR_HEAD          16 // Can be worn on head.
#define ITEM_WEAR_LEGS          32 // Can be worn on legs.
#define ITEM_WEAR_FEET          64 // Can be worn on feet
#define ITEM_WEAR_HANDS        128 // Can be worn on hands (gauntlets, etc)
#define ITEM_WEAR_ARMS         256 // Can be worn on arms.
#define ITEM_WEAR_SHIELD       512 // Can be used as a shield.
#define ITEM_WEAR_ABOUT       1024 // Can be worn about body.
#define ITEM_WEAR_WAIST       2048 // Can be worn around the waist (belt)
#define ITEM_WEAR_WRIST       4096 // Can be worn on wrist (bracelets)
#define ITEM_WIELD            8192 // Can be wielded and used as a weapon
#define ITEM_HOLD            16384 // Item can be held in a hand.
#define ITEM_THROW           32768 // Item can be thrown. (Not implemented.)
#define ITEM_LIGHT_SOURCE    65536 // Item can be held as a light.
#define ITEM_WEAR_EYES      131072 // Item can be worn on eyes. (glasses, eyepatch, etc)
#define ITEM_WEAR_FACE      262144 // Item is worn on face. (mask, veil, etc)
#define ITEM_WEAR_EARRING   524288 // Item is worn in ear.
#define ITEM_WEAR_QUIVER   1048576 // Item is a quiver (Not implemented.)
#define ITEM_WEAR_INSIGNIA 2097152 // as reported by Diatem
#define ITEM_WEAR_BACK      BIT_23
#define ITEM_ATTACH_BELT    BIT_24
#define ITEM_HORSE_BODY     BIT_25
#define ITEM_WEAR_TAIL      BIT_26
#define ITEM_WEAR_NOSE      BIT_27
#define ITEM_WEAR_HORNS     BIT_28

// object "wear bits" bit flag structure and union

typedef struct _objWearBitFlags
{
  unsigned take        : 1;
  unsigned wearFinger  : 1;
  unsigned wearNeck    : 1;
  unsigned wearBody    : 1;
  unsigned wearHead    : 1;
  unsigned wearLegs    : 1;
  unsigned wearFeet    : 1;
  unsigned wearHands   : 1;
  unsigned wearArms    : 1;
  unsigned wearShield  : 1;
  unsigned wearAbout   : 1;
  unsigned wearWaist   : 1;
  unsigned wearWrist   : 1;
  unsigned wield       : 1;
  unsigned hold        : 1;
  unsigned throwable   : 1;
  unsigned lightSource : 1;
  unsigned wearEyes    : 1;
  unsigned wearFace    : 1;
  unsigned wearEarring : 1;
  unsigned wearQuiver  : 1;
  unsigned wearBadge   : 1;
  unsigned wearBack    : 1;
  unsigned attachBelt  : 1;
  unsigned horseBody   : 1;
  unsigned wearTail    : 1;
  unsigned wearNose    : 1;
  unsigned wearHorns   : 1;

  unsigned slack       : 4;
} objWearBitFlags;

typedef union _objWearFlagRec
{
  ulong longIntFlags;

  objWearBitFlags objWearBits;
} objWearFlagRec;


// object anti-bit defines

#define NUMB_OBJANTI_FLAGS        32

#define ITEM_ALLOW_ALL         BIT_1
#define ITEM_ALLOW_WARRIOR     BIT_2
#define ITEM_ALLOW_RANGER      BIT_3
#define ITEM_ALLOW_PALADIN     BIT_4
#define ITEM_ALLOW_ANTIPALADIN BIT_5
#define ITEM_ALLOW_CLERIC      BIT_6
#define ITEM_ALLOW_MONK        BIT_7
#define ITEM_ALLOW_DRUID       BIT_8
#define ITEM_ALLOW_SHAMAN      BIT_9
#define ITEM_ALLOW_SORCERER    BIT_10
#define ITEM_ALLOW_NECROMANCER BIT_11
#define ITEM_ALLOW_CONJURER    BIT_12
#define ITEM_ALLOW_PSIONICIST  BIT_13
#define ITEM_ALLOW_THIEF       BIT_14
#define ITEM_ALLOW_ASSASSIN    BIT_15
#define ITEM_ALLOW_MERCENARY   BIT_16
#define ITEM_ALLOW_BARD        BIT_17
#define ITEM_ANTI_HUMAN        BIT_18
#define ITEM_ANTI_GREYELF      BIT_19
#define ITEM_ANTI_HALFELF      BIT_20
#define ITEM_ANTI_DWARF        BIT_21
#define ITEM_ANTI_HALFLING     BIT_22
#define ITEM_ANTI_GNOME        BIT_23
#define ITEM_ANTI_BARBARIAN    BIT_24
#define ITEM_ANTI_DUERGAR      BIT_25
#define ITEM_ANTI_DROWELF      BIT_26
#define ITEM_ANTI_TROLL        BIT_27
#define ITEM_ANTI_OGRE         BIT_28
#define ITEM_ANTI_ILLITHID     BIT_29
#define ITEM_ANTI_ORC          BIT_30
#define ITEM_ANTI_EVILRACE     BIT_31
#define ITEM_ANTI_GOODRACE     BIT_32

// object "anti bits" bit flag structure and union

typedef struct _objAntiBitFlags
{
  unsigned allowAll    : 1;

  unsigned allowWar    : 1;
  unsigned allowRan    : 1;
  unsigned allowPal    : 1;
  unsigned allowAnP    : 1;
  unsigned allowCle    : 1;
  unsigned allowMon    : 1;
  unsigned allowDru    : 1;
  unsigned allowSha    : 1;
  unsigned allowSor    : 1;
  unsigned allowNec    : 1;
  unsigned allowCon    : 1;
  unsigned allowPsi    : 1;
  unsigned allowThi    : 1;
  unsigned allowAss    : 1;
  unsigned allowMer    : 1;
  unsigned allowBar    : 1;

  unsigned antiHuman   : 1;
  unsigned antiGrey    : 1;
  unsigned antiHalfE   : 1;
  unsigned antiDwarf   : 1;
  unsigned antiHalfl   : 1;
  unsigned antiGnome   : 1;
  unsigned antiBarb    : 1;
  unsigned antiDuerg   : 1;
  unsigned antiDrow    : 1;
  unsigned antiTroll   : 1;
  unsigned antiOgre    : 1;
  unsigned antiIllit   : 1;
  unsigned antiOrc     : 1;

  unsigned antiEvilR   : 1;
  unsigned antiGoodR   : 1;
} objAntiBitFlags;

typedef union _objAntiFlagRec
{
  ulong longIntFlags;

  objAntiBitFlags objAntiBits;
} objAntiFlagRec;


// object anti2-bit defines

#define NUMB_OBJANTI2_FLAGS        14

#define ITEM_ANTI2_THRIKREEN    BIT_1
#define ITEM_ANTI2_CENTAUR      BIT_2
#define ITEM_ANTI2_GITHYANKI    BIT_3
#define ITEM_ANTI2_MINOTAUR     BIT_4
#define ITEM_ANTI2_MALE         BIT_5
#define ITEM_ANTI2_FEMALE       BIT_6
#define ITEM_ANTI2_NEUTER       BIT_7
#define ITEM_ANTI2_AQUAELF      BIT_8
#define ITEM_ANTI2_SAHAUGIN     BIT_9
#define ITEM_ANTI2_GOBLIN       BIT_10
#define ITEM_ANTI2_RAKSASHA     BIT_11
#define ITEM_ANTI2_FLIND        BIT_12
#define ITEM_ANTI2_GITHZERAI    BIT_13
#define ITEM_ANTI2_DUALWIELD    BIT_14

// object "anti2 bits" bit flag structure and union

typedef struct _objAnti2BitFlags
{
  unsigned antiThri     : 1;
  unsigned antiCentaur  : 1;
  unsigned antiGith     : 1;
  unsigned antiMinotaur : 1;

  unsigned antiMale     : 1;
  unsigned antiFemale   : 1;
  unsigned antiNeuter   : 1;

  unsigned antiAquaelf  : 1;
  unsigned antiSahaugin : 1;
  unsigned antiGoblin   : 1;
  unsigned antiRaksasha : 1;
  unsigned antiFlind    : 1;

  unsigned antiGithzerai : 1;
  unsigned antiDual      : 1;

  unsigned slack : 20;
} objAnti2BitFlags;

typedef union _objAnti2FlagRec
{
  ulong longIntFlags;

  objAnti2BitFlags objAnti2Bits;
} objAnti2FlagRec;


#define _OBJBITS_H_
#endif
