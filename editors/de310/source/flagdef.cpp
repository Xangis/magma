//
//  File: flagdef.cpp    originally part of dikuEdit
//
//  Usage: defines tables used to give names and positions of
//         flags in bitvectors
//
//  Copyright 1995-98 (C) Michael Glosenger
//

#include "flagdef.h"
#include "const.h"

#include "room/roombits.h"
#include "obj/objbits.h"
#include "mob/mobbits.h"
#include "obj/object.h"
#include "obj/armor.h"
#include "obj/shields.h"
#include "obj/traps.h"
#include "zone/zone.h"

//
// room flags
//

_CONST flagDef roomFlagDef[NUMB_ROOMBIT_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_ROOMBIT_FLAGS, 0 },
  { "DARK", "Dark", 1, 0 },
  { "LIMIT_SCAN", "Limit scan", 1, 0 },
  { "NO_MOB", "No mob", 1, 0 },
  { "INDOORS", "Indoors", 1, 0 },
  { "SILENT", "Silent", 1, 0 },
  { "UNDERWATER", "Underwater", 1, 0 },
  { "NO_RECALL", "No recall", 1, 0 },
  { "NO_MAGIC", "No magic", 1, 0 },
  { "TUNNEL", "Tunnel", 1, 0 },
  { "PRIVATE", "Private", 0, 0 },
  { "NO_CAMP ", "No Camp", 1, 0 },
  { "SAFE", "Safe", 0, 0 },
  { "NO_PRECIP", "No precipitation", 1, 0 },
  { "SINGLE_FILE", "Single file", 1, 0 },
  { "JAIL", "Jail", 1, 0 },
  { "NO_TELEPORT", "No teleport", 1, 0 },
  { "PRIV_ZONE", "Solitary", 0, 0 },
  { "HEAL", "Heal", 1, 0 },
  { "NO_HEAL", "No heal", 1, 0 },
  { "INN", "Inn", 1, 0 },
  { "DOCKABLE", "Dockable", 1, 0 },
  { "MAGIC_DARK", "Magic darkness", 1, 0 },
  { "MAGIC_LIGHT", "Magic light", 1, 0 },
  { "NO_SUMMON", "No summon", 1, 0 },
  { "GUILD_ROOM", "Guild room", 1, 0 },
  { "TWILIGHT", "Twilight", 1, 0 },
  { "NO_PSI", "No psionics", 1, 0 },
  { "NO_GATE", "No gate/planeshift", 1, 0 },
  { "BANK", "Bank", 1, 0 },
  { "PET_SHOP", "Pet Shop", 1, 0 },
  { "BLOCKS_SIGHT", "Blocks farsee/scan", 1, 0 },
  { "UNUSED_R6", "Unused", 0, 0 }
};


//
// obj extra flags
//

_CONST flagDef objExtraFlagDef[NUMB_OBJEBIT_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_OBJEBIT_FLAGS, 0 },
  { "GLOW", "Glow", 1, 0 },
  { "NOSHOW", "No show", 1, 0 },
  { "BURIED", "Buried", 0, 0 },
  { "NOSELL", "No sell", 1, 0 },
  { "EVIL", "Evil", 1, 0 },
  { "INVISIBLE", "Invisible", 1, 0 },
  { "MAGIC", "Magic", 1, 0 },
  { "NODROP", "No drop (cursed)", 1, 0 },
  { "BLESS", "Blessed", 1, 0 },
  { "ANTI_GOOD", "Anti-good", 1, 0 },
  { "ANTI_EVIL", "Anti-evil", 1, 0 },
  { "ANTI_NEUT", "Anti-neutral", 1, 0 },
  { "SECRET", "Secret", 1, 0 },
  { "FLOAT", "Floats", 1, 0 },
  { "NOBURN", "No burn", 1, 0 },
  { "NOLOCATE", "No locate", 1, 0 },
  { "NOIDENTIFY", "No identify", 1, 0 },
  { "NOSUMMON", "No summon", 1, 0 },
  { "LIT", "Lit", 1, 0 },
  { "TRANSIENT", "Transient", 1, 0 },
  { "NOSLEEP", "No sleep", 1, 0 },
  { "NOCHARM", "No charm", 1, 0 },
  { "TWOHANDS", "Two-handed", 1, 0 },
  { "NORENT", "No rent", 1, 0 },
  { "GOOD", "Good-only", 1, 0 },
  { "HUM", "Hum", 1, 0 },
  { "LEVITATES", "Levitates", 1, 0 },
  { "UNUSED_OE1", "Unused", 0, 0 },
  { "UNUSED_OE2", "Unused", 0, 0 },
  { "WHOLEBODY", "Whole body", 1, 0 },
  { "WHOLEHEAD", "Whole head", 1, 0 },
  { "UNUSED_OE6", "Unused", 0, 0 }
};


//
// obj extra2 flags
//

_CONST flagDef objExtra2FlagDef[NUMB_OBJE2BIT_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_OBJE2BIT_FLAGS, 0 },
  { "SILVER", "Made of silver", 1, 0 },
  { "UNUSED_OE21", "Unused", 0, 0 },
  { "UNUSED_OE22", "Unused", 0, 0 },
  { "UNUSED_OE23", "Unused", 0, 0 },
  { "UNUSED_OE24", "Unused", 0, 0 },
  { "UNUSED_OE25", "Unused", 0, 0 },
  { "RETURNING", "Returns after thrown", 1, 0 },
  { "CAN_THROW1", "Can throw one room", 1, 0 },
  { "CAN_THROW2", "Can throw two rooms", 1, 0 },
  { "ARTIFACT", "Artifact", 1, 0 },
  { "UNUSED_OE26", "Unused", 0, 0 },
  { "UNUSED_OE27", "Unused", 0, 0 },
  { "UNUSED_OE28", "Unused", 0, 0 },
  { "UNUSED_OE29", "Unused", 0, 0 },
  { "UNUSED_OE210", "Unused", 0, 0 },
  { "UNUSED_OE211", "Unused", 0, 0 },
  { "ENLARGED", "Enlarged", 1, 0 },
  { "REDUCED", "Reduced", 1, 0 },
  { "POISONED", "Poisoned", 0, 0},
  { "FLY", "Fly", 0, 0},
  { "SAVE", "Save", 0, 0},
  { "ALTERED", "Altered", 0, 0},
  { "NOFLEE", "Noflee", 0, 0},
  { "NOREPAIR", "Norepair", 1, 0}

/*

 // old new combat flags

  { "ENCHANT_FIRE", "Enchanted against fire", 1, 0 },
  { "ENCHANT_ACID", "Enchanted against acid", 1, 0 },
  { "ENCHANT_COLD", "Enchanted against cold", 1, 0 },
  { "ENCHANT_LIGHTN", "Enchanted against lightning", 1, 0 },
  { "ENCHANT_GAS", "Enchanted against gas", 1, 0 },
  { "ENCHANT_GOOD", "Enchanted against good", 1, 0 },
  { "ENCHANT_EVIL", "Enchanted against evil", 1, 0 },
  { "NOMORE_ENCHANT", "Can not be further enchanted", 1, 0 }
*/
};


//
// obj anti flags
//

_CONST flagDef objAntiFlagDef[NUMB_OBJANTI_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_OBJANTI_FLAGS, 0 },
  { "ALLOW_ALL", "Anyone can use", 1, 0 },
  { "ALLOW_WAR", "Warrior-usable", 1, 0 },
  { "ALLOW_RAN", "Ranger/Hunter-usable", 1, 0 },
  { "ALLOW_PAL", "Paladin-usable", 1, 0 },
  { "ALLOW_ANP", "Anti-paladin-usable", 1, 0 },
  { "ALLOW_CLE", "Cleric-usable", 1, 0 },
  { "ALLOW_MON", "Monk-usable", 1, 0 },
  { "ALLOW_DRU", "Druid-usable", 1, 0 },
  { "ALLOW_SHA", "Shaman-usable", 1, 0 },
  { "ALLOW_SOR", "Sorcerer-usable", 1, 0 },
  { "ALLOW_NEC", "Necromancer-usable", 1, 0 },
  { "ALLOW_CON", "Elementalist-usable", 1, 0 },
  { "ALLOW_PSI", "Psionicist-usable", 1, 0 },
  { "ALLOW_THI", "Thief-usable", 1, 0 },
  { "ALLOW_ASS", "Assassin-usable", 1, 0 },
  { "ALLOW_MER", "Mercenary-usable", 1, 0 },
  { "ALLOW_BAR", "Bard-usable", 1, 0 },
  { "ANTI_HUMAN", "Anti-human", 1, 0 },
  { "ANTI_GREYELF", "Anti-grey elf", 1, 0 },
  { "ANTI_HALFELF", "Anti-half elf", 1, 0 },
  { "ANTI_DWARF", "Anti-mountain dwarf", 1, 0 },
  { "ANTI_HALFLING", "Anti-halfling", 1, 0 },
  { "ANTI_GNOME", "Anti-gnome", 1, 0 },
  { "ANTI_BARBARIAN", "Anti-barbarian", 1, 0 },
  { "ANTI_DUERGAR", "Anti-duergar", 1, 0 },
  { "ANTI_DROWELF", "Anti-drow", 1, 0 },
  { "ANTI_TROLL", "Anti-troll", 1, 0 },
  { "ANTI_OGRE", "Anti-ogre", 1, 0 },
  { "ANTI_GOODRACE", "Anti-good race", 1, 0 },
  { "ANTI_ORC", "Anti-orc", 1, 0 },
  { "ANTI_EVILRACE", "Anti-evil race", 1, 0 },
  { "UNUSED_OA6", "Unused", 0, 0 }
};


//
// obj anti2 flags
//

_CONST flagDef objAnti2FlagDef[NUMB_OBJANTI2_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_OBJANTI2_FLAGS, 0 },
  { "ANTI_THRIKREEN", "Anti-thrikreen", 1, 0 },
  { "ANTI_CENTAUR", "Anti-centaur", 1, 0 },
  { "ANTI_GITHYANKI", "Anti-githyanki", 1, 0 },
  { "ANTI_MINOTAUR", "Anti-minotaur", 1, 0 },
  { "ANTI_MALE", "Anti-male", 1, 0 },
  { "ANTI_FEMALE", "Anti-female", 1, 0 },
  { "ANTI_NEUTER", "Anti-neuter", 1, 0 },
  { "ANTI_AQUAELF", "Anti-aqua elf", 1, 0 },
  { "ANTI_SAHAUGIN", "Anti-sahaugin", 1, 0 },
  { "ANTI_GOBLIN", "Anti-goblin", 1, 0 },
  { "ANTI_RAKSASHA", "Anti-raksasha", 1, 0 },
  { "ANTI_FLIND", "Anti-flind", 1, 0 },
  { "ANTI_GITHZERAI", "Anti-githzerai", 1, 0 },
  { "ANTI_DUALWIELD", "No Dual-Wield", 1, 0 },
};


//
// obj wear flags
//

_CONST flagDef objWearFlagDef[NUMB_OBJWBIT_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_OBJWBIT_FLAGS, 0 },
  { "TAKE", "Takeable", 1, 0 },
  { "WEAR_FINGER", "Worn on finger", 1, 0 },
  { "WEAR_NECK", "Worn on neck", 1, 0 },
  { "WEAR_BODY", "Worn on body", 1, 0 },
  { "WEAR_HEAD", "Worn on head", 1, 0 },
  { "WEAR_LEGS", "Worn on legs", 1, 0 },
  { "WEAR_FEET", "Worn on feet", 1, 0 },
  { "WEAR_HANDS", "Worn on hands", 1, 0 },
  { "WEAR_ARMS", "Worn on arms", 1, 0 },
  { "WEAR_SHIELD", "Worn as shield", 1, 0 },
  { "WEAR_ABOUT", "Worn about body", 1, 0 },
  { "WEAR_WAIST", "Worn about waist", 1, 0 },
  { "WEAR_WRIST", "Worn on wrist", 1, 0 },
  { "WIELD", "Wieldable (weapon)", 1, 0 },
  { "HOLD", "Held", 1, 0 },
  { "THROW", "Thrown (not used)", 0, 0 },
  { "UNUSED_OWQ", "Unused", 0, 0 },
  { "WEAR_EYES", "Worn on eyes", 1, 0 },
  { "WEAR_FACE", "Worn on face", 1, 0 },
  { "WEAR_EARRING", "Worn as earring", 1, 0 },
  { "WEAR_QUIVER", "Worn as quiver", 1, 0 },
  { "WEAR_INSIGNIA", "Worn as badge/insignia", 1, 0 },
  { "WEAR_BACK", "Worn on back", 1, 0 },
  { "ATTACH_BELT", "Attachable to belt", 1, 0 },
  { "HORSE_BODY", "Worn on horse's body", 1, 0 },
  { "WEAR_TAIL", "Worn on tail", 1, 0 },
  { "WEAR_NOSE", "Worn in nose", 1, 0 },
  { "WEAR_HORN", "Worn on horns", 1, 0 }
};


//
// armor flags
//

// body armor

_CONST flagDef armorBodyFlagDef[NUMB_ARMOR_BODY_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_ARMOR_BODY_FLAGS, 0 },
  { "BODY_ALL", "Covers entire torso", 1, 0 },
  { "BODY_FRONT_UPPER", "Covers upper front torso", 1, 0 },
  { "BODY_FRONT_LOWER", "Covers lower front torso", 1, 0 },
  { "BODY_BACK_UPPER", "Covers upper back torso", 1, 0 },
  { "BODY_BACK_LOWER", "Covers lower back torso", 1, 0 },
  { "BODY_RIBS_UPPER", "Covers upper ribcage", 1, 0 },
  { "BODY_RIBS_LOWER", "Covers lower ribcage", 1, 0 },
  { "BODY_SHOULDERS", "Covers shoulders", 1, 0 }
};

// leg armor

_CONST flagDef armorLegsFlagDef[NUMB_ARMOR_LEGS_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_ARMOR_LEGS_FLAGS, 0 },
  { "LEGS_ALL", "Covers both legs entirely", 1, 0 },
  { "LEGS_FRONT_UPPER", "Covers upper front of both legs", 1, 0 },
  { "LEGS_FRONT_KNEES", "Covers front of knees on both legs", 1, 0 },
  { "LEGS_FRONT_LOWER", "Covers lower front of both legs", 1, 0 },
  { "LEGS_BACK_UPPER", "Covers upper back of both legs", 1, 0 },
  { "LEGS_BACK_KNEES", "Covers back of knees on both legs", 1, 0 },
  { "LEGS_BACK_LOWER", "Covers lower back of both legs", 1, 0 }
};

// arm armor

_CONST flagDef armorArmsFlagDef[NUMB_ARMOR_ARMS_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_ARMOR_ARMS_FLAGS, 0 },
  { "ARMS_ALL", "Covers both arms entirely", 1, 0 },
  { "ARMS_FRONT_UPPER", "Covers upper front of both arms", 1, 0 },
  { "ARMS_FRONT_ELBOW", "Covers front of elbows on both arms", 1, 0 },
  { "ARMS_FRONT_LOWER", "Covers lower front of both arms", 1, 0 },
  { "ARMS_BACK_UPPER", "Covers upper back of both arms", 1, 0 },
  { "ARMS_BACK_ELBOW", "Covers back of elbows on both arms", 1, 0 },
  { "ARMS_BACK_LOWER", "Covers lower back of both arms", 1, 0 }
};

// horse body armor

_CONST flagDef armorHorseFlagDef[NUMB_ARMOR_HORSE_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_ARMOR_HORSE_FLAGS, 0 },
  { "HORSE_ALL", "Covers entire horse's body", 1, 0 },
  { "HORSE_FRONT_TOP", "Covers top front of horse's body", 1, 0 },
  { "HORSE_FRONT_SIDES", "Covers front sides of horse's body", 1, 0 },
  { "HORSE_FRONT_BOTTOM", "Covers bottom front of horse's body", 1, 0 },
  { "HORSE_BACK_TOP", "Covers top back of horse's body", 1, 0 },
  { "HORSE_BACK_SIDES", "Covers back sides of horse's body", 1, 0 },
  { "HORSE_BACK_BOTTOM", "Covers bottom back of horse's body", 1, 0 },
  { "HORSE_ASS_END", "Covers ass-end of horse", 1, 0 }
};

// head armor

_CONST flagDef armorHeadFlagDef[NUMB_ARMOR_HEAD_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_ARMOR_HEAD_FLAGS, 0 },
  { "HEAD_ALL", "Covers head entirely", 1, 0 },
  { "HEAD_FRONT_UPPER", "Covers upper front of head", 1, 0 },
  { "HEAD_SIDES_UPPER", "Covers upper sides of head", 1, 0 },
  { "HEAD_BACK_UPPER", "Covers upper back of head", 1, 0 },
  { "HEAD_CROWN", "Covers crown", 1, 0 },
  { "HEAD_FRONT_LOWER", "Covers lower front of head", 1, 0 },
  { "HEAD_SIDES_LOWER", "Covers lower sides of head", 1, 0 },
  { "HEAD_BACK_LOWER", "Covers lower back of head", 1, 0 },
  { "HEAD_CHIN", "Covers chin", 1, 0 }
};

// neck armor

_CONST flagDef armorNeckFlagDef[NUMB_ARMOR_NECK_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_ARMOR_NECK_FLAGS, 0 },
  { "NECK_ALL", "Covers neck entirely", 1, 0 },
  { "NECK_FRONT", "Covers front of neck", 1, 0 },
  { "NECK_BACK", "Covers back of neck", 1, 0 }
};

// foot armor

_CONST flagDef armorFeetFlagDef[NUMB_ARMOR_FEET_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_ARMOR_FEET_FLAGS, 0 },
  { "FEET_ALL", "Covers feet entirely", 1, 0 },
  { "FEET_TOP", "Covers top of feet", 1, 0 },
  { "FEET_BOTTOM", "Covers bottom of feet", 1, 0 },
  { "FEET_TOES", "Covers toes of feet", 1, 0 },
  { "FEET_BACK", "Covers back of feet", 1, 0 }
};

// hand armor

_CONST flagDef armorHandsFlagDef[NUMB_ARMOR_HANDS_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_ARMOR_HANDS_FLAGS, 0 },
  { "HAND_ALL", "Covers hands entirely", 1, 0 },
  { "HAND_FINGERS", "Covers fingers of both hands", 1, 0 },
  { "HAND_HAND", "Covers main 'hand area' of both hands", 1, 0 },
  { "HAND_WRIST", "Covers both wrists", 1, 0 }
};

// about-body armor

_CONST flagDef armorAboutFlagDef[NUMB_ARMOR_ABOUT_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_ARMOR_ABOUT_FLAGS, 0 },
  { "ABOUT_ALL", "Covers entire 'about-body' area", 1, 0 },
  { "ABOUT_BACK_UPPER", "Covers upper back of torso", 1, 0 },
  { "ABOUT_BACK_LOWER", "Covers lower back of torso", 1, 0 },
  { "ABOUT_RIBS_UPPER", "Covers upper ribcage", 1, 0 },
  { "ABOUT_RIBS_LOWER", "Covers lower ribcage", 1, 0 },
  { "ABOUT_SHOULDERS", "Covers shoulders", 1, 0 }
};

// waist 'armor'

#if 0
_CONST flagDef armorWaistFlagDef[NUMB_ARMOR_WAIST_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_ARMOR_WAIST_FLAGS, 0 },
  { "WAIST_ALL", "Covers entire waist area", 1, 0 },
  { "WAIST_FRONT", "Covers front waist", 1, 0 },
  { "WAIST_BACK", "Covers back waist", 1, 0 }
};
#endif

// wrist armor

_CONST flagDef armorWristFlagDef[NUMB_ARMOR_WRIST_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_ARMOR_WRIST_FLAGS, 0 },
  { "WRIST_ALL", "Covers entire wrist area", 1, 0 },
  { "WRIST_LOWER_ARMS_FRONT", "Covers front of lower arms", 1, 0 },
  { "WRIST_LOWER_ARMS_BACK", "Covers back of lower arms", 1, 0 },
  { "WRIST_WRIST_FRONT", "Covers front of wrist", 1, 0 },
  { "WRIST_WRIST_BACK", "Covers back of wrist", 1, 0 }
};

//
// eye armor
//

_CONST flagDef armorEyesFlagDef[NUMB_ARMOR_EYES_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_ARMOR_EYES_FLAGS, 0 },
  { "EYE_LEFT", "Covers left eye", 1, 0 },
  { "EYE_RIGHT", "Covers right eye", 1, 0 },
  { "EYE_LEFT_TRANSP", "Covers left eye (transparent)", 1, 0 },
  { "EYE_RIGHT_TRANSP", "Covers right eye (transparent)", 1, 0 }
};

//
// face armor
//

_CONST flagDef armorFaceFlagDef[NUMB_ARMOR_FACE_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_ARMOR_FACE_FLAGS, 0 },
  { "FACE_ALL", "Covers entire face", 1, 0 },
  { "FACE_UPPER", "Covers upper face", 1, 0 },
  { "FACE_LOWER", "Covers lower face", 1, 0 }
};

//
// ear armor (heh)
//

#if 0
_CONST flagDef armorEarFlagDef[NUMB_ARMOR_EARS_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_ARMOR_EARS_FLAGS, 0 },
  { "EAR_ALL", "Covers entire ear area (face and ears)", 1, 0 },
  { "EAR_UPPER_FACE_SIDE", "Covers upper side of face", 1, 0 },
  { "EAR_EARS", "Covers ear", 1, 0 }
};
#endif


//
// mob action flags
//

_CONST flagDef mobActFlagDef[NUMB_MOBACT_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_MOBACT_FLAGS, 0 },
  { "SIZE_MINUS_ONE", "-1 Size Category", 1, 0 },
  { "SENTINEL", "Sentinel", 1, 0 },
  { "SCAVENGER", "Scavenger", 1, 0 },
  { "ISNPC", "Is NPC", 0, 1 },
  { "GUARDIAN", "Guardian", 1, 0 },
  { "AGGR", "Aggressive", 1, 0 },
  { "STAY_ZONE", "Stays in zone", 1, 0 },
  { "WIMPY", "Wimpy", 1, 0 },
  { "AGGR_EVIL", "Aggr. to evil", 1, 0 },
  { "AGGR_GOOD", "Aggr. to good", 1, 0 },
  { "AGGR_NEUT", "Aggr. to neut", 1, 0 },
  { "MEMORY", "Memory", 1, 0 },
  { "NO_PARALYZE", "Can't be paralyzed", 1, 0 },
  { "NO_SUMMON", "Can't be summoned", 1, 0 },
  { "NO_BASH", "Can't be bashed", 1, 0 },
  { "TEACHER", "Teacher", 1, 0 },
  { "OUTLAW", "Outlaw", 0, 0 },
  { "CANFLY", "Can fly", 1, 0 },
  { "CANSWIM", "Can swim", 1, 0 },
  { "NO_TELEKINESIS", "Can't be TKed", 1, 0 },
  { "UNUSED_MA_5", "Unused (Never set)", 0, 0 },
  { "PET", "Pet (Never set)", 0, 0 },
  { "NOEXP", "No Exp Value", 1, 0 },
  { "SIZE_PLUS_ONE", "+1 Size Category", 1, 0 },
  { "WITNESS", "Witness", 1, 0 },
  { "BREAK_CHARM", "Breaks charm", 1, 0 },
  { "PROTECTOR", "Protector", 1, 0 },
  { "MOUNT", "Mount", 1, 0 },
  { "AGGR_EVILR", "Aggr. evil races", 1, 0 },
  { "AGGR_GOODR", "Aggr. good races", 1, 0 },
  { "HUNTER", "Hunter", 1, 0 },
  { "UNUSED_MA6", "Unused", 0, 0 }
};


//
// affect1 flags
//

_CONST flagDef aff1FlagDef[NUMB_AFF1_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_AFF1_FLAGS, 0 },
  { "BLIND", "Blind", 0, 0 },
  { "INVIS", "Invisible", 1, 0 },
  { "FARSEE", "Farsee", 1, 0 },
  { "DET_INV", "Detect invis", 1, 0 },
  { "HASTE", "Haste", 1, 0 },
  { "SENSE_LIFE", "Sense life", 1, 0 },
  { "MINOR_GLOBE", "Minor globe", 1, 0 },
  { "STONE_SKIN", "Stoneskin", 1, 0 },
  { "UD_VISION", "Underdark vis", 1, 0 },
  { "UNUSED_1J", "Unused", 0, 0 },
  { "WRAITHFORM", "Wraithform", 0, 0 },
  { "WATERBREATH", "Waterbreathing", 1, 0 },
  { "KNOCKED_OUT", "Knocked out", 0, 0 },
  { "PROT_EVIL", "Prot evil", 1, 0 },
  { "BOUND", "Bound", 1, 0 },
  { "SLOW_POISON", "Slow poison", 1, 0 },
  { "PROT_GOOD", "Prot good", 1, 0 },
  { "SLEEP", "Slept", 0, 0 },
  { "SKILL_AWARE", "Skill aware", 0, 0 },
  { "SNEAK", "Sneak", 1, 0 },
  { "HIDE", "Hide", 1, 0 },
  { "FEAR", "Fear", 0, 0 },
  { "CHARM", "Charmed", 0, 0 },
  { "UNUSED_1X", "Unused", 0, 0 },
  { "BARKSKIN", "Barkskin", 1, 0 },
  { "INFRAVISION", "Infravision", 1, 0 },
  { "LEVITATE", "Levitate", 1, 0 },
  { "FLY", "Fly", 1, 0 },
  { "AWARE", "Aware", 1, 0 },
  { "PROT_FIRE", "Prot fire", 1, 0 },
  { "BIOFEEDBACK", "Biofeedback", 1, 0 },
  { "UNUSED_16", "Unused", 0, 0 }
};


//
// affect2 flags
//

_CONST flagDef aff2FlagDef[NUMB_AFF2_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_AFF2_FLAGS, 0 },
  { "FIRESHIELD", "Fireshield", 1, 0 },
  { "ULTRA", "Ultravision", 1, 0 },
  { "DET_EVIL", "Detect evil", 1, 0 },
  { "DET_GOOD", "Detect good", 1, 0 },
  { "DET_MAGIC", "Detect magic", 1, 0 },
  { "UNUSED_2F", "Unused", 0, 0 },
  { "PROT_COLD", "Prot cold", 1, 0 },
  { "PROT_LIGHT", "Prot lightning", 1, 0 },
  { "MINOR_PARA", "Minor paralyze", 0, 0 },
  { "MAJOR_PARA", "Major paralyze", 0, 0 },
  { "SLOWNESS", "Slowness", 1, 0 },
  { "MAJOR_GLOBE", "Major globe", 1, 0 },
  { "PROT_GAS", "Prot gas", 1, 0 },
  { "PROT_ACID", "Prot acid", 1, 0 },
  { "POISONED", "Poisoned", 0, 0 },
  { "SOULSHIELD", "Soulshield", 1, 0 },
  { "DUERGAR_HIDE", "Duergar hide", 0, 0 },
  { "MINOR_INVIS", "Minor invis", 1, 0 },
  { "VAMP_TOUCH", "Vampiric touch", 1, 0 },
  { "STUNNED", "Stunned", 0, 0 },
  { "UNUSED_2U ", "Unused", 0, 0 },
  { "UNUSED_2V", "Unused", 0, 0 },
  { "UNUSED_2W", "Unused", 0, 0 },
  { "UNUSED_2X", "Unused", 0, 0 },
  { "HOLDING_BREATH", "Holding breath", 0, 0 },
  { "UNUSED_2Z", "Unused", 0, 0 },
  { "DROWNING", "Drowning", 0, 0 },
  { "PASSDOOR", "Passdoor", 0, 0 },
  { "UNUSED_23", "Unused", 0, 0 },
  { "CASTING", "Casting", 0, 0 },
  { "UNUSED_25", "Unused", 0, 0 },
  { "UNUSED_26", "Unused", 0, 0 }
};

_CONST flagDef aff3FlagDef[NUMB_AFF3_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_AFF3_FLAGS, 0 },
  { "UNUSED_3A", "Unused", 0, 0 },
  { "TRACKING", "Tracking", 0, 0 },
  { "SINGING", "Singing", 0, 0 },
  { "ECTO_FORM", "Ectoplasmic Form", 1, 0 },
  { "UNUSED_3E", "Unused", 0, 0 },
  { "UNUSED_3F", "Unused", 0, 0 },
  { "SPIRIT_WARD", "Spirit Ward", 1, 0 },
  { "GR_WARD", "Greater Spirit Ward", 1, 0 },
  { "UNUSED_3I", "Unused", 0, 0 },
  { "SILVER", "Silver", 1, 0 },
  { "PLUSONE", "Plus One", 1, 0 },
  { "PLUSTWO", "Plus Two", 1, 0 },
  { "PLUSTHREE", "Plus Three", 1, 0 },
  { "PLUSFOUR", "Plus Four", 1, 0 },
  { "PLUSFIVE", "Plus Five", 1, 0 },
  { "ENLARGED", "Enlarged", 0, 0 },
  { "REDUCED", "Reduced", 0, 0 },
  { "COVER", "Cover", 1, 0 },
  { "FOUR_ARMS", "Four Arms", 1, 0 },
  { "INERTIAL_BARRIER", "Intertial Barrier", 1, 0 },
  { "INTELLECT_FORT", "Intellect Fortress", 1, 0 },
  { "COLDSHIELD", "Coldshield", 1, 0 },
  { "UNUSED_3X", "Unused", 0, 0 },
  { "SWIMMING", "Swimming", 0, 0 },
  { "TOWER_IRON_WILL", "Tower of Iron Will", 1, 0 },
  { "UNUSED_3Z", "Unused", 0, 0 },
  { "BLUR", "Blur", 1, 0 },
  { "UNUSED_32", "Unused", 0, 0 },
  { "ELEMENTAL_FORM", "Elemental Form", 0, 0 },
  { "PASS_WITHOUT_TRACE", "Pass Without Trace", 1, 0 },
  { "PALADIN_AURA", "Paladin Aura", 0, 0 },
  { "UNUSED_36", "Unused", 0, 0 }
};

_CONST flagDef aff4FlagDef[NUMB_AFF4_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_AFF4_FLAGS, 0 },
  { "LOOTER", "Looter", 0, 0 },
  { "DISEASED", "Diseased", 0, 0 },
  { "UNUSED_4C", "Unused", 0, 0 },
  { "SENSE_FOLLOWER", "Sense Follower", 0, 0 },
  { "UNUSED_4E", "Unused", 0, 0 },
  { "UNUSED_4F", "Unused", 0, 0 },
  { "UNUSED_4G", "Unused", 0, 0 },
  { "UNUSED_4H", "Unused", 0, 0 },
  { "HOLY_SACRIFICE", "Holy Sacrifice", 1, 0 },
  { "BATTLE_ECSTASY", "Battle Ecstasy", 1, 0 },
  { "DAZZLER", "Dazzler", 1, 0 },
  { "DAZZLEE", "Dazzled", 0, 0 },
  { "THROAT_CRUSH", "Throat Crush", 0, 0 },
  { "REGENERATION", "Regeneration", 1, 0 },
  { "BEARHUG", "Bearhug", 0, 0 },
  { "GRAPPLE", "Grappling", 0, 0 },
  { "GRAPPLED", "Grappled", 0, 0 },
  { "UNUSED_4R", "Unused", 0, 0 },
  { "NO_IMMOLATE", "No-Immolate", 1, 0 },
  { "UNUSED_4T", "Unused", 0, 0 }
};

//
// trap effect flags
//

_CONST flagDef trapEffFlagDef[NUMB_TRAP_EFF_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_TRAP_EFF_FLAGS, 0 },
  { "MOVE", "Triggers on movement (any dir)", 1, 0 },
  { "OBJECT", "Triggers on get/put", 1, 0 },
  { "ROOM", "Affects all in room when triggered", 1, 0 },
  { "NORTH", "Affects those who leave room north", 1, 0 },
  { "EAST", "Affects those who leave room east", 1, 0 },
  { "SOUTH", "Affects those who leave room south", 1, 0 },
  { "WEST", "Affects those who leave room west", 1, 0 },
  { "UP", "Affects those who leave room up", 1, 0 },
  { "DOWN", "Affects those who leave room down", 1, 0 },
  { "OPEN", "Triggers on being opened", 1, 0 },
  { "MULTI", "No Longer Used", 1, 0 },
  { "GLYPH", "Glyph (doesn't prevent movement)", 1, 0 },
  { "WEAR",  "Triggers upon wearing", 1, 0 },
  { "UNEQUIP", "Triggers upon unequipping", 1, 0 },
  { "STEAL", "Triggers when item is stolen", 1, 0 },
  { "ENCHANT", "Triggers when item is enchanted", 1, 0 },
  { "UNCURSE", "Triggers when item is uncursed", 1, 0 },
  { "THROW", "Triggers when item is thrown/shot", 1, 0 },
  { "EATDRINK", "Triggers when eaten/drank", 1, 0 },
  { "DRAG", "Triggers when dragged", 1, 0 },
};


//
// misc armor flags
//

_CONST flagDef armorMiscFlagDef[NUMB_OBJ_ARMOR_MISC_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_OBJ_ARMOR_MISC_FLAGS, 0 },
  { "SPIKED", "Armor is spiked", 1, 0 }
};


//
// object container flags
//

_CONST flagDef contFlagDef[NUMB_OBJ_CONT_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_OBJ_CONT_FLAGS, 0 },
  { "CLOSEABLE", "Closeable", 1, 0 },
  { "HARDPICK", "Hardpick", 1, 0 },
  { "CLOSED", "Closed", 1, 0 },
  { "LOCKED", "Locked", 1, 0 },
  { "PICKPROOF", "Cannot be picked", 1, 0 }
};


//
// totem sphere flags
//

_CONST flagDef totemSphereFlagDef[NUMB_OBJ_TOTEM_SPHERE_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_OBJ_TOTEM_SPHERE_FLAGS, 0 },
  { "LESS_ANIM", "Lesser animal", 1, 0 },
  { "GREA_ANIM", "Greater animal", 1, 0 },
  { "LESS_ELEM", "Lesser elemental", 1, 0 },
  { "GREA_ELEM", "Greater elemental", 1, 0 },
  { "LESS_SPIR", "Lesser spirit", 1, 0 },
  { "GREA_SPIR", "Greater spirit", 1, 0 }
};


//
// misc shield flags
//

_CONST flagDef shieldMiscFlagDef[NUMB_OBJ_SHIELD_MISC_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_OBJ_SHIELD_MISC_FLAGS, 0 },
  { "SPIKED", "Shield is spiked", 1, 0 }
};


//
// zone misc flags
//

_CONST flagDef zoneMiscFlagDef[NUMB_ZONEMISC_FLAGS + 1] =
{
  { "_FLAGS", "_FLAGS", NUMB_ZONEMISC_FLAGS, 0 },
  { "SILENT", "Silent", 1, 0 },
  { "SAFE", "Safe", 1, 0 },
  { "HOMETOWN", "Hometown", 1, 0 },
  { "NO-TELEPORT", "No Teleport", 1, 0 }
};
