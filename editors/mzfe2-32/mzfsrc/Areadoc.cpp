/*  Project pounds

    Copyright © 1994-1997. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         areadoc.cpp
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file (1 of 3) for implementation of TAreaDocument (TFileDocument).
*/

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
 *  In order to use any part of this Envy Diku Mud, you must comply with   *
 *  the original Diku license in 'license.doc', the Merc license in        *
 *  'license.txt', as well as the Envy license in 'license.nvy'.           *
 *  In particular, you may not remove either of these copyright notices.   *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/***************************************************************************
*  ROM 2.4 is copyright 1993-1996 Russ Taylor                              *
*      Russ Taylor (rtaylor@pacinfo.com)                                   *
***************************************************************************/

#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

// OWL includes
#include <owl\inputdia.h>

// Local includes
#include "areadoc.h"
#include "objlib.h"

extern poundsApp *TheMuditorApp;

//{{TAreaDocument Implementation}}

// Reverse direction for exits
const	sh_int TAreaDocument::rev_dir[6] = { 2, 3, 0, 1, 5, 4 };

// @@@ Move into class
BOOL fFoundMOBprog;

TAreaDocument::TAreaDocument (TDocument* parent):
    TFileDocument(parent),
    TheHelps(100, 0, 10),
    TheMobiles(100, 0, 10),
    TheObjects(100, 0, 10),
    TheRooms(100, 0, 10),
    TheShops(10, 0, 5),
    TheResets(100, 0, 20),
    TheMobResets(20, 0, 10)
{
    // INSERT>> Your constructor code here.
    fIsOpen = FALSE;

    nDefaultSectorType = SECT_CITY;
    nDefaultSectorFlags = 0;

    nDefaultAction = ACT_STAY_AREA;
    nDefaultAffectedBy = 0;

    recall = -1;

    // For Troll's AreaData
    fUsesAreaData = FALSE;
    area_data.area_flags = 0;
    area_data.fCustomWeather = FALSE;
    static AREA_WEATHER NormalWeather = { 40, 80, 0, 100, 0, 50, 50, 50, 5 };
    area_data.Weather = NormalWeather;
    area_data.timezone = 0;

    m_Library = 0;
}

// Make all of these part of class
static void DeleteARoom(TRoom *& pRoom, void *)
{
   delete pRoom;
}

static void DeleteAHelp(THelpData *& pHelp, void *)
{
   delete pHelp;
}

static void DeleteAObject(TObject *& pObject, void *)
{
   delete pObject;
}

static void DeleteAMobile(TMobile *& pMobile, void *)
{
   delete pMobile;
}

static void DeleteAShop(TShopData *& pShop, void *)
{
   delete pShop;
}

static void DeleteAReset(TReset *& pReset, void *)
{
   delete pReset;
}

//static void DeleteAMobReset(TMobReset *& pMobReset, void *)
//{
//	delete pMobReset;
//}

TAreaDocument::~TAreaDocument ()
{
	 // INSERT>> Your destructor code here.
	 TheHelps.ForEach(DeleteAHelp, 0);
	 TheMobiles.ForEach(DeleteAMobile, 0);
	 TheObjects.ForEach(DeleteAObject, 0);
	 TheRooms.ForEach(DeleteARoom, 0);
	 TheShops.ForEach(DeleteAShop, 0);
	 TheResets.ForEach(DeleteAReset, 0);
//    TheMobResets.ForEach(DeleteAMobReset, 0);
}


BOOL TAreaDocument::IsOpen ()
{
    return fIsOpen;
}


BOOL TAreaDocument::Open (int /*mode*/, LPCSTR path)
{
    // I have no idea what this is for...  It's in the linedoc example
    if (path)
       SetDocPath(path);

    TheMuditorApp = MuditorApp =
       TYPESAFE_DOWNCAST(GetDocManager().GetApplication(), poundsApp);

    // @@@
    if (MuditorApp->CurrentMode == poundsApp::OghamMode)
       {
       GetDocManager().GetApplication()->GetMainWindow()->MessageBox(
          "Ogham file format is write-only.  Ogham files cannot be loaded.",
          "M.Z.F.", MB_ICONHAND|MB_OK);
       return TRUE;
       }

	 if (GetDocPath()) { // Handle File|Open
		 TPointer<TInStream> is = InStream(ofRead|ofBinary);
		 if (!is)
			return FALSE;

    fIsOpen = TRUE;
    fFoundMOBprog = FALSE;
    TRY {
       nNextRoomVNum = nNextObjVNum = nNextMobVNum = -1;
		 for ( ; ; )
			 {
			 char *word;

			 if ( fread_letter( *is ) != '#' )
				 {
				 SyntaxError(*is, "Looking for new section", "# not found");
				 break;
				 }

			 word = fread_word( *is, "Reading new section name" );
          strcpy(szSection, word);
          strcpy(szLastSuccess, "<none>");

				  if ( word[0] == '$'               )                 break;
			 else if ( !stricmp( word, "AREA"     ) ) load_area    (*is);
			 else if ( !stricmp( word, "HELPS"    ) ) load_helps   (*is);
			 else if ( !stricmp( word, "MOBILES"  ) ) load_mobiles (*is, MuditorApp->CurrentMode);
			 else if ( !stricmp( word, "MOBPROGS" ) ) load_mobprogs(*is);
			 else if ( !stricmp( word, "OBJECTS"  ) ) load_objects (*is, MuditorApp->CurrentMode);
			 else if ( !stricmp( word, "RESETS"   ) ) load_resets  (*is);
			 else if ( !stricmp( word, "ROOMS"    ) ) load_rooms   (*is, MuditorApp->CurrentMode);
			 else if ( !stricmp( word, "SHOPS"    ) ) load_shops   (*is);
			 else if ( !stricmp( word, "SPECIALS" ) ) load_specials(*is);
          // Following is Envy
          else if ( !stricmp( word, "RECALL"   ) ) load_recall  (*is);
          // Following is Troll
          else if ( !stricmp( word, "AREADATA" ) ) load_troll_area(*is);
          // Following is ROM
          else if ( !stricmp( word, "MOBOLD"   ) ) load_mobiles (*is, poundsApp::MercMode);
          else if ( !stricmp( word, "OBJOLD"   ) ) load_objects (*is, poundsApp::MercMode);
          else if ( !stricmp( word, "SOCIALS"  ) ) load_socials (*is);
          else if ( !stricmp( word, "CREATOR"  ) ) load_creator (*is);
			 else
				 {
				 SyntaxError(*is, "Looking for a section", "MZF does not understand this section type");
				 break;
				 }
			 }

       fix_exits();
       if (fFoundMOBprog && MuditorApp->fMOBprograms == FALSE)
          {
          MessageBox(0, "Warning:  You have MOBprograms, but have selected no "
             "MOBprograms in your configuration.  Your MOBprograms will be "
             "stripped when you exit unless you go into 'File|File Format' "
             "and turn on MOBprogram support", "M.Z.F.",
             MB_ICONINFORMATION|MB_OK);
          }
       }
       CATCH( (xmsg& x)
          {
          GetDocManager().GetApplication()->Error(x, 0, 0);
          return FALSE;
          }
       )
		 }
    else // Handle File|New
       {
       name = "{ 5 35} M.Z.F.  Prototype for New Area";
//       recall = ROOM_VNUM_TEMPLE;
       recall = -1;

       char szBuffer[8] = "9900";
       if (TInputDialog(GetDocManager().GetApplication()->GetMainWindow(),
          "MAKE.ZONES.FAST", "Enter beginning VNUM",
             szBuffer, sizeof szBuffer).Execute() != IDOK)
          return FALSE;

       nNextRoomVNum = nNextObjVNum = nNextMobVNum = (vnum_int) atoi(szBuffer);
       fIsOpen = TRUE;
       }

    return TRUE;
}

/*
 * Snarf an 'area' header line.
 */
void TAreaDocument::load_area( istream& is )
{
   recall  = -1;
   if (MuditorApp->CurrentMode == poundsApp::Rom24Mode) {
      fread_string(is, "Skipping ROM 2.4 file name" );
      shortname = fread_string(is, "Reading ROM 2.4 short name" );
      name = fread_string( is, "Reading zone name" );
      fread_number( is, "Skipping ROM 2.4 initial VNUM" );
      fread_number( is, "Skipping ROM 2.4 final VNUM" );
   } else {
      name = fread_string( is, "Reading zone name" );
   }
}



/*
 * Snarf a help section.
 */
void TAreaDocument::load_helps( istream& is )
{
    PHelpData pHelp;

    for ( ; ; )
    {
    pHelp = new THelpData(this);
	pHelp->level	= (sh_int) fread_number( is, "Reading help level" );
	pHelp->keyword	= fread_string( is, "Reading help keyword" );
	if ( pHelp->keyword[0] == '$' )
       {
		 pHelp.do_delete();
	    break;
       }
	pHelp->text	= fread_string( is, "Reading help text" );

   TheHelps.Add(pHelp);
   strcpy(szLastSuccess, pHelp->keyword.c_str());
    }

    return;
}



/*
 * Snarf a recall point.
 */
void TAreaDocument::load_recall( istream& is )
{
    recall = (sh_int) fread_number( is, "Reading recall room number" );

    if ( recall < 1 )
//        bug( "Load_recall:  %s invalid recall point", NULL );
       SyntaxError(is, "Reading recall room number", "Invalid room number");
}

// race table (ROM)
struct rom_race_type far TAreaDocument::rom_race_table[] =
{
    { "unique",		FALSE, 0, 0, 0, 0, 0, 0,
         0,
         0 },

    {	"human",		   TRUE,  0, 0, 0, 0, 0, 0,
         FORM_EDIBLE|FORM_SENTIENT|FORM_BIPED|FORM_MAMMAL,
         PART_HEAD|PART_ARMS|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_HANDS|PART_FEET|PART_FINGERS|PART_EAR|PART_EYE },

    {	"elf",			TRUE,	0,	AFF_INFRARED,	0,	0,	RES_CHARM, VULN_IRON,
         FORM_EDIBLE|FORM_SENTIENT|FORM_BIPED|FORM_MAMMAL,
         PART_HEAD|PART_ARMS|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_HANDS|PART_FEET|PART_FINGERS|PART_EAR|PART_EYE },

    {	"dwarf",		   TRUE,	0,	AFF_INFRARED,	0,	0,	RES_MAGIC|RES_POISON|RES_DISEASE, VULN_DROWNING,
	      FORM_EDIBLE|FORM_SENTIENT|FORM_BIPED|FORM_MAMMAL,
         PART_HEAD|PART_ARMS|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_HANDS|PART_FEET|PART_FINGERS|PART_EAR|PART_EYE },

    {	"giant",		   TRUE,	0,	0,		         0,	0,	RES_FIRE|RES_COLD, VULN_MENTAL|VULN_LIGHTNING,
	      FORM_EDIBLE|FORM_SENTIENT|FORM_BIPED|FORM_MAMMAL,
         PART_HEAD|PART_ARMS|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_HANDS|PART_FEET|PART_FINGERS|PART_EAR|PART_EYE },

    {	"bat",			FALSE, 0, AFF_FLYING|AFF_DARK_VISION, OFF_DODGE|OFF_FAST, 0, 0, VULN_LIGHT,
      	FORM_EDIBLE|FORM_ANIMAL|FORM_BIRD,
         PART_HEAD|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_FEET|PART_EAR|PART_EYE|PART_WINGS },

    {	"bear",			FALSE, 0, 0, OFF_CRUSH|OFF_DISARM|OFF_BERSERK, 0, RES_BASH|RES_COLD,	0,
	      FORM_EDIBLE|FORM_ANIMAL|FORM_MAMMAL,
         PART_HEAD|PART_ARMS|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_FEET|PART_EAR|PART_EYE|PART_CLAWS|PART_FANGS },

    {	"cat",			FALSE, 0, AFF_DARK_VISION,	OFF_FAST|OFF_DODGE, 0, 0, 0,
	      FORM_EDIBLE|FORM_ANIMAL|FORM_MAMMAL,
         PART_HEAD|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_FEET|PART_EAR|PART_EYE|PART_TAIL|PART_CLAWS|PART_FANGS },

    {	"centipede",	FALSE, 0, AFF_DARK_VISION,	0, 0,	RES_PIERCE|RES_COLD,	VULN_BASH,
         FORM_EDIBLE|FORM_POISON|FORM_ANIMAL|FORM_INSECT,
         PART_HEAD|PART_LEGS|PART_EYE },

    {	"dog",			FALSE, 0, 0, OFF_FAST, 0, 0, 0,
         FORM_EDIBLE|FORM_ANIMAL|FORM_MAMMAL,
         PART_HEAD|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_FEET|PART_EAR|PART_EYE|PART_CLAWS|PART_FANGS },

    {	"doll",			FALSE, 0, 0, 0, IMM_MAGIC,	RES_BASH|RES_LIGHT, VULN_SLASH|VULN_FIRE|VULN_ACID|VULN_LIGHTNING|VULN_ENERGY,
         FORM_OTHER|FORM_CONSTRUCT|FORM_BIPED|FORM_COLD_BLOOD,
         PART_HEAD|PART_ARMS|PART_LEGS|PART_HANDS|PART_FEET|PART_EYE },

    {	"fido",			FALSE, 0, 0, OFF_DODGE|ASSIST_RACE, 0,	0,	VULN_MAGIC,
         FORM_POISON|FORM_ANIMAL|FORM_MAMMAL,
         PART_HEAD|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_FEET|PART_EAR|PART_EYE|PART_TAIL|PART_FANGS },

    {	"fox",			FALSE, 0, AFF_DARK_VISION,	OFF_FAST|OFF_DODGE, 0, 0, 0,
         FORM_EDIBLE|FORM_ANIMAL|FORM_MAMMAL,
         PART_HEAD|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_FEET|PART_EAR|PART_EYE|PART_TAIL|PART_FANGS },

    {	"goblin",		FALSE, 0, AFF_INFRARED,	0, 0,	RES_DISEASE, VULN_MAGIC,
         FORM_EDIBLE|FORM_SENTIENT|FORM_BIPED|FORM_MAMMAL,
         PART_HEAD|PART_ARMS|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_HANDS|PART_FEET|PART_FINGERS|PART_EAR|PART_EYE },

    {	"hobgoblin",	FALSE, 0, AFF_INFRARED,	0,	0,	RES_DISEASE|RES_POISON,	0,
         FORM_EDIBLE|FORM_SENTIENT|FORM_BIPED|FORM_MAMMAL,
         PART_HEAD|PART_ARMS|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_HANDS|PART_FEET|PART_FINGERS|PART_EAR|PART_EYE|PART_TUSKS },

    {	"kobold",		FALSE, 0, AFF_INFRARED,	0,	0,	RES_POISON,	VULN_MAGIC,
	      FORM_EDIBLE|FORM_POISON|FORM_SENTIENT|FORM_BIPED|FORM_MAMMAL,
         PART_HEAD|PART_ARMS|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_HANDS|PART_FEET|PART_FINGERS|PART_EAR|PART_EYE|PART_TAIL
    },

    {	"lizard",		FALSE, 0, 0, 0, 0, RES_POISON, VULN_COLD,
         FORM_EDIBLE|FORM_ANIMAL|FORM_REPTILE|FORM_COLD_BLOOD,
         PART_HEAD|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_FEET|PART_EYE|PART_TAIL|PART_FANGS },

    {	"modron",		FALSE, 0, AFF_INFRARED,	ASSIST_RACE|ASSIST_ALIGN, IMM_CHARM|IMM_DISEASE|IMM_MENTAL|IMM_HOLY|IMM_NEGATIVE, RES_FIRE|RES_COLD|RES_ACID, 0,
         FORM_SENTIENT,
         PART_HEAD|PART_ARMS|PART_LEGS|PART_HANDS|PART_FEET|PART_EAR|PART_EYE },

    {	"orc",			FALSE, 0, AFF_INFRARED,	0,	0,	RES_DISEASE, VULN_LIGHT,
         FORM_EDIBLE|FORM_SENTIENT|FORM_BIPED|FORM_MAMMAL,
         PART_HEAD|PART_ARMS|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_HANDS|PART_FEET|PART_FINGERS|PART_EAR|PART_EYE },

    {	"pig",			FALSE, 0, 0, 0, 0, 0, 0,
         FORM_EDIBLE|FORM_ANIMAL|FORM_MAMMAL,
         PART_HEAD|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_FEET|PART_EAR|PART_EYE },

    {	"rabbit",		FALSE, 0, 0, OFF_DODGE|OFF_FAST, 0,	0,	0,
         FORM_EDIBLE|FORM_ANIMAL|FORM_MAMMAL,
         PART_HEAD|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_FEET|PART_EAR|PART_EYE },

    {	"school monster",	FALSE, ACT_NOALIGN, 0, 0, IMM_CHARM|IMM_SUMMON,	0,	VULN_MAGIC,
         FORM_EDIBLE|FORM_BIPED|FORM_MAMMAL,
         PART_HEAD|PART_ARMS|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_FEET|PART_EAR|PART_EYE|PART_TAIL|PART_CLAWS },

    {	"snake",		   FALSE, 0, 0, 0, 0, RES_POISON, VULN_COLD,
         FORM_EDIBLE|FORM_ANIMAL|FORM_WORM|FORM_REPTILE|FORM_SNAKE|FORM_COLD_BLOOD,
         PART_HEAD|PART_HEART|PART_BRAINS|PART_GUTS|PART_EYE|PART_LONG_TONGUE|PART_TAIL|PART_FANGS|PART_SCALES },

    {	"song bird",	FALSE, 0, AFF_FLYING, OFF_FAST|OFF_DODGE,	0,	0,	0,
         FORM_EDIBLE|FORM_ANIMAL|FORM_BIRD,
         PART_HEAD|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_FEET|PART_EYE|PART_WINGS },

    {	"troll",		   FALSE, 0, AFF_REGENERATION|AFF_INFRARED|AFF_DETECT_HIDDEN, OFF_BERSERK, 0,	RES_CHARM|RES_BASH,	VULN_FIRE|VULN_ACID,
         FORM_POISON|FORM_BIPED|FORM_MAMMAL,
         PART_HEAD|PART_ARMS|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_HANDS|PART_FEET|PART_FINGERS|PART_EAR|PART_EYE|PART_CLAWS|PART_FANGS },

    {	"water fowl",	FALSE, 0, AFF_SWIM|AFF_FLYING, 0, 0, RES_DROWNING,	0,
         FORM_EDIBLE|FORM_ANIMAL|FORM_BIRD,
         PART_HEAD|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_FEET|PART_EYE|PART_WINGS },

    {	"wolf",			FALSE, 0, AFF_DARK_VISION,	OFF_FAST|OFF_DODGE, 0, 0, 0,
	      FORM_EDIBLE|FORM_ANIMAL|FORM_MAMMAL,
         PART_HEAD|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_EAR|PART_EYE|PART_TAIL|PART_FANGS },

    {	"wyvern",		FALSE, 0, AFF_FLYING|AFF_DETECT_INVIS|AFF_DETECT_HIDDEN,	OFF_BASH|OFF_FAST|OFF_DODGE, IMM_POISON, 0,	VULN_LIGHT,
         FORM_POISON|FORM_DRAGON|FORM_COLD_BLOOD,
         PART_HEAD|PART_LEGS|PART_HEART|PART_BRAINS|PART_GUTS|PART_FEET|PART_EAR|PART_EYE|PART_TAIL|PART_FANGS|PART_SCALES },

    {	NULL, 0, 0, 0, 0, 0, 0 }
};

// Race ability bits.  What we have done here is to use a special bit
// mapping instead of the one used by Envy.  This mapping allows us to
// use the Envy 2.0 table as it is (so we can always update MZF by
// putting the race table from some other mud in it) but we interpret the
// bits as AFF_ bits.  This allows us to give the mobile the correct bits
// when we translate it to a codebase that doesn't have envy-style races.
#define RACE_NO_ABILITIES	      0
#define RACE_PC_AVAIL		      0
#define RACE_WATERBREATH	      AFF_GILLS
#define RACE_FLY		            AFF_FLYING
#define RACE_SWIM		            0
#define RACE_WATERWALK		      AFF_WATERWALK
#define RACE_PASSDOOR		      AFF_PASS_DOOR
#define RACE_INFRAVISION	      AFF_INFRARED
#define RACE_DETECT_ALIGN	      AFF_DETECT_GOOD|AFF_DETECT_EVIL
#define RACE_DETECT_INVIS	      AFF_DETECT_INVIS
#define RACE_DETECT_HIDDEN	      AFF_DETECT_HIDDEN
#define RACE_PROTECTION		      AFF_PROTECT_GOOD|AFF_PROTECT_EVIL
#define RACE_SANCT		         AFF_SANCTUARY
#define RACE_WEAPON_WIELD	      0

// Race table, Envy 2.0
/*const*/ struct envy_race_type far TAreaDocument::envy_race_table[ MAX_RACE ] =
{
    // The 'unique' race isn't a part of Envy, but it helps with
    // conversions to other codebases.
    { "unique",    RACE_NO_ABILITIES,      0, 0, 0, 0, 0, 0, "hit", "" },

    { "Human",     RACE_PC_AVAIL | RACE_WEAPON_WIELD,      3, 0, 0, 0, 0, 0,
	"punch", "Githyanki Vampire Werewolf Mindflayer"                     },
    { "Elf",       RACE_PC_AVAIL | RACE_INFRAVISION | RACE_DETECT_HIDDEN      |
	           RACE_WEAPON_WIELD,                      2, 0, 1, 0, 1, -1,
	"punch", "Drow Ogre Orc Kobold Troll Hobgoblin Dragon Vampire Werewolf Goblin Halfkobold"                                                           },
    { "Halfelf",   RACE_PC_AVAIL | RACE_INFRAVISION | RACE_WEAPON_WIELD,
	                                                   3, 0, 0, 0, 1, 0,
	"punch", "Drow Ogre Orc Kobold Troll Hobgoblin Dragon Vampire Werewolf Goblin"                                                                      },
    { "Drow",      RACE_PC_AVAIL | RACE_INFRAVISION | RACE_DETECT_HIDDEN      |
	           RACE_WEAPON_WIELD,                      2, 0, 0, 1, 1, 0,
	"punch", "Elf Halfelf Hobbit Githyanki Vampire Werewolf"             },
    { "Dwarf",     RACE_PC_AVAIL | RACE_INFRAVISION | RACE_DETECT_HIDDEN      |
	           RACE_WEAPON_WIELD,                      2, 0, 0, 0, -1, 1,
	"punch", "Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Dragon Vampire Werewolf Goblin Halfkobold"                                                 },
    { "Halfdwarf", RACE_PC_AVAIL | RACE_INFRAVISION | RACE_WEAPON_WIELD,
	                                                   2, 0, 0, 0, 0, 1,
	"punch", "Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Dragon Vampire Werewolf Goblin"                                                            },
    { "Hobbit",    RACE_PC_AVAIL | RACE_INFRAVISION | RACE_DETECT_HIDDEN      |
	           RACE_WEAPON_WIELD,                      2, 0, 0, 0, 1, -1,
	"punch", "Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Dragon Vampire Werewolf Goblin Halfkobold"                                                 },
    { "Giant",     RACE_WEAPON_WIELD,                      6, 2, -1, 0, -1, 1,
	"fist", "Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome"  },
    { "Ogre",      RACE_WEAPON_WIELD,                      6, 1, -1, 0, -1, 1,
	"fist",	"Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome"  },
    { "Orc",       RACE_INFRAVISION | RACE_WEAPON_WIELD,   4, 1, -1, 0, 0, 0,
	"punch", "Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome" },
    { "Kobold",    RACE_INFRAVISION | RACE_WEAPON_WIELD,   2, -1, -1, 0, 1, 0,
	"punch", "Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome Halfkobold"                                                                     },
    { "Minotaur",  RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD, 5, 2, 0, 0, -1, 1,
	"fist", "Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome"  },
    { "Troll",     RACE_INFRAVISION | RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD,
	                                                   7, 2, -1, 0, 0, 1,
	"fist", "Human Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome"                                                                           },
    { "Hobgoblin", RACE_INFRAVISION | RACE_WEAPON_WIELD,   3, 1, 0, -1, 0, 1,
	"punch", "Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome" },
    { "Insect",    RACE_NO_ABILITIES,                      0, 0, 0, 0, 0, -1,
	"bite", ""                                                           },
    { "Dragon",    RACE_FLY | RACE_INFRAVISION | RACE_DETECT_ALIGN |
	           RACE_DETECT_INVIS | RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD,
	                                                   9, 2, 2, 1, -3, 2,
	"claw", ""                                                           },
    { "Animal",    RACE_DETECT_HIDDEN,                     2, 0, 0, 0, 1, 0,
	"bite",	"Kobold Halfkobold"                                          },
    { "God",       RACE_WATERBREATH | RACE_FLY | RACE_SWIM | RACE_WATERWALK   |
	           RACE_PASSDOOR | RACE_INFRAVISION | RACE_DETECT_ALIGN       |
		   RACE_DETECT_INVIS | RACE_DETECT_HIDDEN | RACE_PROTECTION   |
		   RACE_SANCT | RACE_WEAPON_WIELD,         8, 3, 3, 3, 3, 3,
	"smite", ""                                                          },
    { "Undead",    RACE_PASSDOOR | RACE_INFRAVISION | RACE_DETECT_ALIGN       |
	           RACE_DETECT_INVIS | RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD,
	                                                   3, 1, 0, 0, -2, 1,
	"touch", "Human Elf Halfelf Drow Dwarf Halfdwarf Hobbit Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Goblin Faerie Gnome"                         },
    { "Harpy",     RACE_FLY | RACE_DETECT_INVIS,           3, 0, 0, 0, 2, 0,
	"claw", "Human Elf Halfelf Dwarf Halfdwarf Hobbit Gnome"             },
    { "Bear",      RACE_SWIM | RACE_DETECT_HIDDEN,         3, 1, 0, 0, -1, 1,
	"swipe", ""                                                          },
    { "Githyanki", RACE_WEAPON_WIELD,                      3, 0, 1, 0, 0, 0,
	"punch", "Mindflayer"                                                },
    { "Elemental", RACE_NO_ABILITIES,                      4, 1, 0, 0, 0, 1,
	"punch", ""                                                          },
    { "Bat",       RACE_FLY | RACE_INFRAVISION,            1, -1, 0, 0, 2, -1,
	"bite", ""                                                           },
    { "Plant",     RACE_NO_ABILITIES,                      1, 0, 0, 0, -1, 1,
	"swipe", ""                                                          },
    { "Rat",       RACE_PASSDOOR,                          0, -1, 0, 0, 2, -1,
	"bite", ""                                                           },
    { "Vampire",   RACE_FLY | RACE_PASSDOOR | RACE_INFRAVISION                |
	           RACE_DETECT_ALIGN | RACE_DETECT_INVIS | RACE_DETECT_HIDDEN |
		   RACE_WEAPON_WIELD,                      3, 1, 1, 0, 1, 2,
	"claw", "Human Elf Halfelf Drow Dwarf Halfdwarf Hobbit Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Werewolf Goblin Faerie Gnome"                 },
    { "Werewolf",  RACE_INFRAVISION | RACE_DETECT_ALIGN | RACE_DETECT_INVIS   |
	           RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD, 3, 2, 0, 1, 2, 3,
	"claw", "Human Elf Halfelf Drow Dwarf Halfdwarf Hobbit Giant Ogre Orc Kobold Minotaur Troll Hobgoblin Vampire Goblin Faerie Gnome"                  },
    { "Goblin",    RACE_INFRAVISION | RACE_WEAPON_WIELD,   2, -1, -1, -1, 1, 0,
	"punch", "Elf Halfelf Dwarf Halfdwarf Hobbit Vampire Werewolf Gnome" },
    { "Faerie",    RACE_FLY | RACE_INFRAVISION | RACE_DETECT_INVIS            |
	           RACE_DETECT_HIDDEN | RACE_WEAPON_WIELD, 1, -2, 1, 1, 1, -1,
	"punch", ""                                                          },
    { "Arachnid",  RACE_NO_ABILITIES | RACE_WEAPON_WIELD,  2, 0, 0, 0, 1, 0,
	"bite", ""                                                           },
    { "Mindflayer",RACE_INFRAVISION | RACE_WEAPON_WIELD,   3, 1, 2, 1, -1, 0,
	"punch", "Githyanki"                                                 },
    { "Object",    RACE_WATERBREATH,                       3, 3, 0, 0, 0, 3,
	"swing", ""                                                          },
    { "Mist",      RACE_FLY | RACE_PASSDOOR,               2, -1, 0, 0, 3, 0,
	"gas",   ""                                                          },
    { "Snake",     RACE_NO_ABILITIES,                      1, 0, 0, 0, 1, 0,
	"bite", ""                                                           },
    { "Worm",      RACE_PASSDOOR,                          0, 0, 0, 0, 0, 0,
	"slime", ""                                                          },
    { "Fish",      RACE_WATERBREATH | RACE_SWIM,           1, 0, 0, 0, 2, 0,
	"slap", ""                                                           },
    { "Hydra",     RACE_DETECT_HIDDEN,                     8, 2, 0, 0, -1, 2,
	"bite", ""                                                           },
    { "Lizard",    RACE_NO_ABILITIES,                      1, -1, 0, 0, 1, 0,
	"lash", ""                                                           },
    { "Gnome",     RACE_PC_AVAIL | RACE_INFRAVISION | RACE_WEAPON_WIELD,
	                                                   2, -1, 0, 1, 1, -1,
	"punch", "Drow Ogre Orc Kobold Troll Hobgoblin Dragon Vampire Werewolf Goblin" },
    { "Halfkobold", RACE_PC_AVAIL | RACE_INFRAVISION | RACE_WEAPON_WIELD,
	                                                  2, -2, -1, -2, 3, -2,
	"punch", "Ogre Orc Giant Troll Hobgoblin"                            }
};


// ROM 2.4
void TAreaDocument::LoadRom24Mobile(TMobile *pMobIndex, vnum_int vnum, istream& is)
{
//   char szBuffer[256];

   pMobIndex->vnum = vnum;

   pMobIndex->player_name = fread_string( is, "Reading mobile name" );
   pMobIndex->short_descr = fread_string( is, "Reading mobile short descr" );
   pMobIndex->long_descr  = fread_string( is, "Reading mobile long descr" );
   pMobIndex->description = fread_string( is, "Reading mobile LOOK descr" );

   if (!pMobIndex->long_descr.is_null())
         {
         pMobIndex->long_descr[0] = UPPER( pMobIndex->long_descr[0]  );

         // Strip trailing newline;
         int n = pMobIndex->long_descr.length();
         while (n && pMobIndex->long_descr[n-1] < ' ')
            n--;
         pMobIndex->long_descr.resize(n);
         }

   if (!pMobIndex->description.is_null())
         pMobIndex->description[0]	= UPPER( pMobIndex->description[0] );

   // ROM 2.x race attribute
   pMobIndex->strrace = fread_string( is, "Reading mobile race" );
   pMobIndex->race = race_lookup(pMobIndex->strrace);

   // We OR in the race table here, but we will remove these bits on write
   // for ROM 2.4 format.  For Merc&Envy, they will pick up the race ACT
   // bits.  We also remove the NPC bit, as the server is going to put that
   // on every mobile.
   pMobIndex->act         = (fread_flag( is, "Reading mobile ACT flags") |
                             rom_race_table[pMobIndex->race].act) & ~ACT_IS_NPC;

   pMobIndex->affected_by = fread_flag( is, "Reading mobile AFF flags" ) |
         rom_race_table[pMobIndex->race].aff;

   pMobIndex->pShop	  = NULL;
   pMobIndex->alignment	  = (sh_int) fread_number( is, "Reading mobile alignment" );

   // A 'mobile group' is a number, the mobile will group?/assist? other
   // mobiles with the same number here.  The convention is that it is
   // a number based on the vnum of the area file, so an area at 9900 will
   // have groups 9900, 9901, etc.
   pMobIndex->mob_group = (vnum_int) fread_number(is, "Reading ROM 2.4 <mobile group>");

   pMobIndex->level	  = (sh_int) fread_number( is, "Reading mobile level" );

   /*
    * The unused stuff is for imps who want to use the old-style
    * stats-in-files method.
    */
   pMobIndex->hitroll     = fread_number( is, "Reading mobile hit roll" );   /* Unused */
//   pMobIndex->ac          = fread_number( is, "Skipping mobile armor class" );   /* Unused */

   pMobIndex->hitnodice   = fread_number( is, "Reading mobile hit dice" );   /* Unused */
   /* 'd'		*/           fread_letter( is );   /* Unused */
   pMobIndex->hitsizedice = fread_number( is, "Reading mobile hit dice size" );   /* Unused */
   /* '+'		*/           fread_letter( is );   /* Unused */
   pMobIndex->hitplus	  = fread_number( is, "Reading mobile hit point bonus" );   /* Unused */

      /* read mana dice */
      pMobIndex->mana[DICE_NUMBER]	= (sh_int) fread_number( is, "Reading mobile mana dice" );
      /* d */	fread_letter( is );
      pMobIndex->mana[DICE_TYPE]	= (sh_int) fread_number( is, "Reading mobile mana dice size" );
      /* + */	fread_letter( is );
      pMobIndex->mana[DICE_BONUS]	= (sh_int) fread_number( is, "Reading mobile mana bonus" );

   pMobIndex->damnodice   = fread_number( is, "Reading mobile damage dice" );   /* Unused */
   /* 'd'		*/           fread_letter( is );   /* Unused */
   pMobIndex->damsizedice = fread_number( is, "Reading mobile damage dice size" );   /* Unused */
   /* '+'		*/           fread_letter( is );   /* Unused */
   pMobIndex->damplus     = fread_number( is, "Reading mobile damage bonus" );   /* Unused */
//      pMobIndex->dam_type		= (sh_int) fread_number( is, "Reading mobile damage type" );
   pMobIndex->strDamType = fread_word( is, "Reading mobile damage type" );

      /* read armor class */
      pMobIndex->rom_ac[AC_PIERCE]	= (sh_int) fread_number( is, "Reading mobile AC vs pierce" );
      pMobIndex->rom_ac[AC_BASH]		= (sh_int) fread_number( is, "Reading mobile AC vs bash" );
      pMobIndex->rom_ac[AC_SLASH]	= (sh_int) fread_number( is, "Reading mobile AC vs slash" );
      pMobIndex->rom_ac[AC_EXOTIC]	= (sh_int) fread_number( is, "Reading mobile AC vs exotic" );

      /* read flags and add in data from the race table */
      pMobIndex->off_flags		= fread_flag( is, "Reading mobile off flags" )
					| rom_race_table[pMobIndex->race].off;
      pMobIndex->imm_flags		= fread_flag( is, "Reading mobile imm flags" )
					| rom_race_table[pMobIndex->race].imm;
      pMobIndex->res_flags		= fread_flag( is, "Reading mobile res flags" )
					| rom_race_table[pMobIndex->race].res;
      pMobIndex->vuln_flags		= fread_flag( is, "Reading mobile vuln flags" )
					| rom_race_table[pMobIndex->race].vuln;

//      static const char *aszRomPosition[] = { "stand", "rest", "sleep" };
      string strStart = fread_word(is, "Reading mobile start pos");
      string strDefault = fread_word(is, "Reading mobile default pos");
//      pMobIndex->start_pos	= 0;
//      pMobIndex->default_pos = 0;
//      for (int i = 0; i < nelems(aszRomPosition); i++) {
//         if (strStart == aszRomPosition[i]) {
//            pMobIndex->start_pos = (sh_int) i;
//         }
//         if (strDefault == aszRomPosition[i]) {
//            pMobIndex->default_pos = (sh_int) i;
//         }
//      }
      pMobIndex->start_pos = position_lookup(strStart.c_str());
      pMobIndex->default_pos = position_lookup(strDefault.c_str());

      static const char *aszRomSexes[] = { "none", "male", "female", "either" };
      string strSex = fread_word(is, "Reading mobile sex");
      pMobIndex->sex = SEX_NEUTRAL;
      int i;
      for (i = 0; i < nelems(aszRomSexes); i++)
         if (strSex == aszRomSexes[i]) {
            pMobIndex->sex = (sh_int) i;
            break;
         }


      pMobIndex->gold		= fread_number( is, "Reading mobile gold" );

      pMobIndex->form		= fread_flag( is, "Reading mobile form" )
					| rom_race_table[pMobIndex->race].form;
      pMobIndex->parts		= fread_flag( is, "Reading mobile parts" )
					| rom_race_table[pMobIndex->race].parts;

      string strSize = fread_word(is, "Reading mobile size");
      // @@@ Move to document
      static const char *aszRomSizes[] = { "tiny", "small", "medium", "large", "huge", "giant" };
      pMobIndex->size = 2;
      for (i = 0; i < nelems(aszRomSizes); i++)
         if (strSize == aszRomSizes[i]) {
            pMobIndex->size = (sh_int) i;
            break;
         }

      //pMobIndex->material = material_lookup(fread_word( is, "Reading mobile material" ));
      pMobIndex->material = fread_word( is, "Reading mobile material" );

      for (;;) {
         char letter=fread_letter(is);
         if (letter=='>') {
            is.putback(letter);
            mprog_read_programs(is,pMobIndex);
         } else if (letter == 'F') {
            fread_word(is, "Skipping ROM 2.4 mobile strip type");
		      fread_flag(is, "Skipping ROM 2.4 mobile strip flag");
         } else {
            is.putback(letter);
            break;
         }
      }
}

// @@@ Make this part of class
static sh_int Rom23Size(char c)
{
   char *pszRom23Sizes = "TSMLHG";
   for (sh_int i = 0; *pszRom23Sizes; pszRom23Sizes++, i++)
      if (*pszRom23Sizes == c)
         return i;

   return SIZE_MEDIUM;
}

// Non ROM 2.4
void TAreaDocument::LoadMobile(TMobile *pMobIndex, vnum_int vnum, istream& is)
{
   char szBuffer[256];
   poundsApp::currMode mode = MuditorApp->CurrentMode;

   pMobIndex->vnum	     = vnum;
      pMobIndex->player_name = fread_string( is, "Reading mobile name" );
      pMobIndex->short_descr = fread_string( is, "Reading mobile short descr" );
      pMobIndex->long_descr  = fread_string( is, "Reading mobile long descr" );
      pMobIndex->description = fread_string( is, "Reading mobile LOOK descr" );

      if (!pMobIndex->long_descr.is_null())
         {
         pMobIndex->long_descr[0] = UPPER( pMobIndex->long_descr[0]  );

         // Strip trailing newline;
         int n = pMobIndex->long_descr.length();
         while (n && pMobIndex->long_descr[n-1] < ' ')
            n--;
         pMobIndex->long_descr.resize(n);
         }

      if (!pMobIndex->description.is_null())
         pMobIndex->description[0]	= UPPER( pMobIndex->description[0] );

      // ROM 2.x race attribute
      if (mode == poundsApp::RomMode)
         pMobIndex->race = race_lookup(fread_string( is, "Reading mobile race number" ));

      if (mode == poundsApp::RomMode)
      {
      pMobIndex->act         = fread_flag( is, "Reading mobile ACT flags" ) |
         /*ACT_IS_NPC |*/ rom_race_table[pMobIndex->race].act;

      pMobIndex->affected_by = fread_flag( is, "Reading mobile AFF flags" ) |
         rom_race_table[pMobIndex->race].aff;
      }
      else
      {
      pMobIndex->act	     = fread_number( is, "Reading mobile ACT_xxx bit flags" ) /*| ACT_IS_NPC*/;
      pMobIndex->affected_by = fread_number( is, "Reading mobile AFF_xxx bit flags" );
      }

   pMobIndex->pShop	  = NULL;
   pMobIndex->alignment	  = (sh_int) fread_number( is, "Reading mobile alignment" );
   char letter		  = fread_letter( is );
   pMobIndex->level	  = (sh_int) fread_number( is, "Reading mobile level" );

   if ( letter != 'S' ) {
      wsprintf(szBuffer, "Expected S in mobile vnum #%d structure, got %c",
         vnum, letter);
      SyntaxError(is, "Loading mobile", szBuffer);
   }

   /*
    * The unused stuff is for imps who want to use the old-style
    * stats-in-files method.
    */
   pMobIndex->hitroll     = fread_number( is, "Reading mobile hit roll" );   /* Unused */
   if (mode != poundsApp::RomMode)
      pMobIndex->ac          = fread_number( is, "Reading mobile armor class" );   /* Unused */

   pMobIndex->hitnodice   = fread_number( is, "Reading mobile hit dice" );   /* Unused */
   /* 'd'		*/           fread_letter( is );   /* Unused */
   pMobIndex->hitsizedice = fread_number( is, "Reading mobile hit dice size" );   /* Unused */
   /* '+'		*/           fread_letter( is );   /* Unused */
   pMobIndex->hitplus	  = fread_number( is, "Reading mobile hit point bonus" );   /* Unused */

   if (mode == poundsApp::RomMode)
      {
      /* read mana dice */
      pMobIndex->mana[DICE_NUMBER]	= (sh_int) fread_number( is, "Reading mobile mana dice" );
      /* d */	fread_letter( is );
      pMobIndex->mana[DICE_TYPE]	= (sh_int) fread_number( is, "Reading mobile mana dice size" );
      /* + */	fread_letter( is );
      pMobIndex->mana[DICE_BONUS]	= (sh_int) fread_number( is, "Reading mobile mana bonus" );
      }

   pMobIndex->damnodice   = fread_number( is, "Reading mobile damage dice" );   /* Unused */
   /* 'd'		*/           fread_letter( is );   /* Unused */
   pMobIndex->damsizedice = fread_number( is, "Reading mobile damage dice size" );   /* Unused */
   /* '+'		*/           fread_letter( is );   /* Unused */
   pMobIndex->damplus     = fread_number( is, "Reading mobile damage bonus" );   /* Unused */
   if (mode == poundsApp::RomMode)
      {
      // @@@ We need to set the ROM 2.4 strDamType here, in case we convert...
      pMobIndex->dam_type		= (sh_int) fread_number( is, "Reading mobile damage type" );

      /* read armor class */
      pMobIndex->rom_ac[AC_PIERCE]	= (sh_int) (fread_number( is, "Reading mobile AC vs pierce" ) * 10);
      pMobIndex->rom_ac[AC_BASH]		= (sh_int) (fread_number( is, "Reading mobile AC vs bash" ) * 10);
      pMobIndex->rom_ac[AC_SLASH]	= (sh_int) (fread_number( is, "Reading mobile AC vs slash" ) * 10);
      pMobIndex->rom_ac[AC_EXOTIC]	= (sh_int) (fread_number( is, "Reading mobile AC vs exotic" ) * 10);

      /* read flags and add in data from the race table */
      pMobIndex->off_flags		= fread_flag( is, "Reading mobile off flags" )
					| rom_race_table[pMobIndex->race].off;
      pMobIndex->imm_flags		= fread_flag( is, "Reading mobile imm flags" )
					| rom_race_table[pMobIndex->race].imm;
      pMobIndex->res_flags		= fread_flag( is, "Reading mobile res flags" )
					| rom_race_table[pMobIndex->race].res;
      pMobIndex->vuln_flags		= fread_flag( is, "Reading mobile vuln flags" )
					| rom_race_table[pMobIndex->race].vuln;

      /* vital statistics */
      pMobIndex->start_pos	= (sh_int) fread_number( is, "Reading mobile start pos" );
      pMobIndex->default_pos = (sh_int) fread_number( is, "Reading mobile default pos" );
      pMobIndex->sex			= (sh_int) fread_number( is, "Reading mobile sex" );
      pMobIndex->gold		= fread_number( is, "Reading mobile gold" );

      pMobIndex->form		= fread_flag( is, "Reading mobile form" )
					| rom_race_table[pMobIndex->race].form;
      pMobIndex->parts		= fread_flag( is, "Reading mobile parts" )
					| rom_race_table[pMobIndex->race].parts;

      /* size */
      char cSize = fread_letter( is /*, "Reading mobile size"*/ );
      pMobIndex->size = Rom23Size(cSize);
      //pMobIndex->material = material_lookup(fread_word( is, "Reading mobile material" ));
      pMobIndex->material = fread_word( is, "Reading mobile material" );
      }
   else // non-ROM
      {
      pMobIndex->gold = fread_number( is, "Skipping mobile gold amount" );   /* Unused */
      /* xp        */   fread_number( is, "Skipping mobile XP" );   /* Unused */
      /* position  */   fread_number( is, "Skipping mobile position" );   /* Unused */
      if (mode == poundsApp::Envy2Mode) {
         pMobIndex->strrace = fread_string( is, "Reading mobile race string" );
         pMobIndex->race = race_lookup(pMobIndex->strrace);
         // Give the mobile the race abilities, in case the mobile must be
         // converted to a codebase without envy races.
         pMobIndex->act |= envy_race_table[pMobIndex->race].race_abilities;
      } else
         /* start pos */   fread_number( is, "Skipping mobile start position" );   /* Unused */
      pMobIndex->sex = (sh_int) fread_number( is, "Reading mobile sex" );
      }

      letter=fread_letter(is);
      is.putback(letter);
      if (letter=='>')
	 mprog_read_programs(is,pMobIndex);
}


/*
 * Snarf a mob section.
 */
void TAreaDocument::load_mobiles( istream& is, poundsApp::currMode mode )
{
//    PMobile pMobIndex;
    TMobile *pMobIndex;
    char szBuffer[256];

    for ( ; ; )
    {
	char letter;
	vnum_int  vnum;

	letter				= fread_letter( is );
	if ( letter != '#' )
	{
       SyntaxError(is, "Looking for a mobile", "Did not find leading # symbol");
	}

	vnum				= (vnum_int) fread_number( is, "Reading mobile vnum" );
	if ( vnum == 0 )
	    break;

	if ( get_mob_index( vnum ) )
	{
      wsprintf(szBuffer, "Two mobiles both using VNUM #%d", vnum);
      SyntaxError(is, "Loading mobile", szBuffer);
	}

   pMobIndex = new TMobile(this);
   TRY {
      switch (mode) {
      case poundsApp::Rom24Mode:
         LoadRom24Mobile(pMobIndex, vnum, is);
         break;
      default:
         LoadMobile(pMobIndex, vnum, is);
         break;
      }

      if (nNextMobVNum < vnum)
       {
       // First mob seen?
       if (nNextMobVNum == -1)
          {
          if (nNextObjVNum == -1)
             nNextObjVNum = vnum;
          if (nNextRoomVNum == -1)
             nNextRoomVNum = vnum;
          }

       nNextMobVNum = vnum;
       }
    }
    CATCH( (xmsg& x) {
       // This mobile was invalid.  Clean up the work we did on it and rethrow.
       delete pMobIndex;
       throw;
    } )

    TheMobiles.Add(pMobIndex);
    strcpy(szLastSuccess, pMobIndex->short_descr.c_str());
    }
}


/* This routine reads in scripts of MOBprograms from a file */
#pragma argsused
PMobProgData TAreaDocument::mprog_file_read(const string& f, PMobProgData mprg,
   PMobile pMobIndex)
{
#if 1
//  MPROG_DATA* mprg2;
   PMobProgData mprg2;
//  FILE * progfile;
   char MOBProgfile[MAX_INPUT_LENGTH];
   char szMessage[256];
   int letter;

   fFoundMOBprog = TRUE;

//   wsprintf( MOBProgfile, "%s%s", MOB_DIR, f.c_str() );
   wsprintf( MOBProgfile, "%s%s", "", f.c_str() );
   ifstream progfile( MOBProgfile, ios::in | ios::nocreate | ios::binary );
   if (progfile.bad())
      {
      wsprintf(szMessage, "Couldn't open MOBprogram for Mobile #%d", pMobIndex->vnum);
      SyntaxError(progfile, "Opening MOBprogram file", szMessage);
//      bug("Mob:%d couldn't open mobprog file",pMobIndex->vnum);
      }

//   int letter;
   BOOL done=FALSE;
   mprg2=mprg;
   mprg2->MobFile = f;   // Filename of MOBprog
   switch (letter=fread_letter(progfile)) {
   case '>':
     break;
   case '|':
//       bug("empty mobprog file.", 0);
       SyntaxError(progfile, "Reading MOBprogram", "Empty MOBprogram file");
//       exit( 1 );
     break;
   default:
//       bug("in mobprog file syntax error.", 0);
       wsprintf(szMessage, "Expected > or | in MOBprogram, got %c", letter);
       SyntaxError(progfile, "Reading MOBprogram", szMessage);
//       exit( 1 );
   break;
   }

   while (!done)
      {
      mprg2->type = fread_word(progfile, "Reading MOBprogram trigger type");
      if (mprg2->type == "in_file_prog")
//         bug( "mprog file contains a call to file.", 0 );
         SyntaxError(progfile, "Loading MOBprogram file", "MOBprogram file contains another in_file_prog");
      else
         {
//         pMobIndex->progtypes = pMobIndex->progtypes | mprg2->type;
         mprg2->arglist=fread_string(progfile, "Reading MOBprogram trigger argument" );
         mprg2->comlist=fread_string(progfile, "Reading MOBprogram command list" );
         switch (letter=fread_letter(progfile)) {
         case '>':
            pMobIndex->mobprogs.Add(mprg2);
            mprg2 = new TMobProgData(this);
            mprg2->MobFile = f;   // Filename of MOBprog
            break;
         case '|':
             done=TRUE;
           break;
         default:
             wsprintf(szMessage, "Expecting > or |, got '%c'", letter);
             SyntaxError(progfile, "Reading MOBprograms", szMessage);
//           bug( "syntax error in mobprog.",0);
//           break;
         }
         }
      }

  return mprg2;
#else
   THROW(TXOwl("Sorry, Can't read MOBprograms yet"));
#endif
}

/* This procedure is responsible for reading any in_file MOBprograms.
 */
#pragma argsused
void TAreaDocument::mprog_read_programs(istream& is, PMobile pMobIndex)
{
//   MPROG_DATA * mprg;
   BOOL done=FALSE;
   char szBuffer[256];
   int letter;
   if ( (/*letter=*/fread_letter(is)) != '>' )
  {
//      bug( "Load_mobiles: vnum %d MOBPROG char", pMobIndex->vnum );
      wsprintf(szBuffer, "Missing > in MOBprogram for mobile vnum %d", pMobIndex->vnum);
      SyntaxError(is, "Reading MOBprogram", szBuffer);
//      exit( 1 );
  }

  fFoundMOBprog = TRUE;
  TMobProgData *mprg = new TMobProgData(this);

//  mprg=pMobIndex->mobprogs;
  while (!done)
  {
//    mprg->type=mprog_name_to_type(fread_word(fp));
    mprg->type=fread_word(is, "Reading MOBprogram trigger type");
//    switch (mprg->type)
//    {
//     case ERROR_PROG:
//        bug( "Load_mobiles: vnum %d MOBPROG type.", pMobIndex->vnum );
//        exit( 1 );
//      break;
//     case IN_FILE_PROG:
    if (mprg->type == "in_file_prog")
       {
        mprg=mprog_file_read(fread_string(is, "Reading MOBprogram filename"), mprg, pMobIndex);
        fread_to_eol(is);
        switch (letter=fread_letter(is))
        {
          case '>':
//             mprg->next=(MPROG_DATA *)alloc_perm(sizeof(MPROG_DATA));
//             mprg=mprg->next;
//             mprg->next=NULL;
           pMobIndex->mobprogs.Add(mprg);
           mprg = new TMobProgData(this);
           break;
          case '|':
//             mprg->next=NULL;
				 delete mprg;
             fread_to_eol(is);
             done=TRUE;
           break;
          default:
//             bug( "Load_mobiles: vnum %d bad MOBPROG.", pMobIndex->vnum );
             wsprintf(szBuffer, "Mobile vnum #%d expected > or | in MOBprogram, got %c",
                pMobIndex->vnum, letter);
             SyntaxError(is, "Reading MOBprogram", szBuffer);
//             exit( 1 );
           break;
        }
       }
//      break;
//     default:
     else
        {
//        pMobIndex->progtypes = pMobIndex->progtypes | mprg->type;
#if 1
        TArrayAsVector<TMobProgData*>* mobproglist = &pMobIndex->mobprogs;
#endif
        mprg->arglist=fread_string(is, "Reading MOBprogram trigger argument");
        fread_to_eol(is);
        mprg->comlist=fread_string(is, "Reading MOBprogram commands");
        fread_to_eol(is);
        switch (letter=fread_letter(is))
        {
          case '>':
           if (!mobproglist->Add(mprg))
//              bug("MProg_Read_Programs: Couldn't grow mobprog list", 0);
              SyntaxError(is, "Adding a new MOBprogram trigger", "Couldn't add trigger");
           mprg = new TMobProgData(this);
           break;
          case '|':
           if (!mobproglist->Add(mprg))
//              bug("MProg_Read_Programs: Couldn't grow mobprog list", 0);
              SyntaxError(is, "Adding a new MOBprogram trigger", "Couldn't add trigger");
             fread_to_eol(is);
             done=TRUE;
           break;
          default:
//             bug( "Load_mobiles: vnum %d bad MOBPROG.", pMobIndex->vnum );
             wsprintf(szBuffer, "Mobile vnum #%d expected > or |, got '%c'",
                letter);
             SyntaxError(is, "Reading MOBprogram", szBuffer);
//             exit( 1 );
           break;
        }
//      break;
       }
    }
//  }
}

/* Snarf a MOBprogram section from the area file.
 */
void TAreaDocument::load_mobprogs(istream &is)
{
   char letter;
   PMobile iMob;
   sh_int value;
   PMobProgData original, working;
   char szBuffer[256];

for (;;) switch (letter=fread_letter( is))
  {
  default:
     wsprintf(szBuffer, "Expected one of these characters: S, M, or *.  Got %c", letter);
     SyntaxError(is, "Reading a MOBprogram file name", szBuffer);
     break;
  case 'S':
  case 's':
     fread_to_eol(is);
     return;
  case '*':
     fread_to_eol(is);
     break;
  case 'M':
  case 'm':
     value = (sh_int) fread_number(is, "Reading a VNUM to attach MOBprogram");
     if ((iMob=get_mob_index(value))==NULL)
        {
        wsprintf(szBuffer, "MOBprogram for unknown mobile vnum #%d", value);
        SyntaxError(is, "Loading MOBprograms", szBuffer);
        }

     working = new TMobProgData(this);
     working=mprog_file_read(fread_word(is, "Reading MOBprogram from file"),working,iMob);
     iMob->mobprogs.Add(working);

     fread_to_eol(is);
     break;
   }
}

void TAreaDocument::LoadRom24Object(TObject *pObjIndex, vnum_int vnum, istream& is)
{
	pObjIndex->vnum			= vnum;
	pObjIndex->name			= fread_string( is, "Reading object name" );
	pObjIndex->short_descr	= fread_string( is, "Reading object short description" );
	pObjIndex->description	= fread_string( is, "Reading object description" );
   // Unlike Merc, ROM doesn't force case on its short_descr and description

	pObjIndex->material = fread_string( is, "Reading object material" );

// 	pObjIndex->item_type		= (sh_int) fread_number( is, "Reading object item type" );
   pObjIndex->strType = fread_word(is, "Reading ROM 2.4 object type");

   // Types unknown to Merc will be left at 0.
   // (@@@ We might consider loading the merc # here from the database)
   static const char *aszObjTypes[] = { "0", "light", "scroll", "wand", "staff",
      "weapon", "6", "7", "treasure", "armor", "potion", "11", "furniture",
      "trash", "14", "container", "16", "drink", "key", "food", "money",
      "21", "boat", "npc_corpse", "pc_corpse", "fountain", "pill"
   };
   //,
   //   "protect", "map", "portal", "warp_stone", "31", "gem", "jewelry" };
   pObjIndex->item_type = 0;
   for (int i = 0; i < nelems(aszObjTypes); i++) {
      if (pObjIndex->strType == aszObjTypes[i]) {
         pObjIndex->item_type = (sh_int) i;
         break;
      }
   }

   pObjIndex->extra_flags          = fread_flag( is, "Reading object extra flags" );
   pObjIndex->wear_flags           = fread_flag( is, "Reading object wear position bits" );

   pObjIndex->value[0] = fread_word( is, "Reading object value0" );
   pObjIndex->value[1] = fread_word( is, "Reading object value1" );
   pObjIndex->value[2] = fread_word( is, "Reading object value2" );
   pObjIndex->value[3] = fread_word( is, "Reading object value3" );
   pObjIndex->value[4] = fread_word( is, "Reading object value4" );

   // @@@ The following is an Envy-ism which isn't a good thing for
   // MZF.  We need to get rid of this.
#if 0
	// Translate character strings *value[] into integers:
   char szBuffer[32];
	switch ( pObjIndex->item_type )
	{
	case ITEM_PILL:
	case ITEM_POTION:
	case ITEM_SCROLL:
       wsprintf(szBuffer, "%d", SpellValue( pObjIndex->value[1].c_str()));
	    pObjIndex->value[1] = szBuffer;
       wsprintf(szBuffer, "%d", SpellValue( pObjIndex->value[2].c_str()));
	    pObjIndex->value[2] = szBuffer;
       wsprintf(szBuffer, "%d", SpellValue( pObjIndex->value[3].c_str()));
	    pObjIndex->value[3] = szBuffer;
       wsprintf(szBuffer, "%d", SpellValue( pObjIndex->value[4].c_str()));
	    pObjIndex->value[4] = szBuffer;
	    break;

	case ITEM_STAFF:
	case ITEM_WAND:
       wsprintf(szBuffer, "%d", SpellValue( pObjIndex->value[3].c_str()));
	    pObjIndex->value[3] = szBuffer;
	    break;
	}
#endif

 	pObjIndex->level		= (sh_int) fread_number( is, "Reading object level" );

	pObjIndex->weight		= fread_number( is, "Reading object weight" );
	pObjIndex->cost			= fread_number( is, "Reading object cost" );
	pObjIndex->condition = fread_letter( is /*, "Reading object condition"*/ );
//   fread_letter(is);

#if 0 // ROM doesn't do this, why should we?
	/*
	 * Check here for the redundancy of invisible light sources - Kahn
	 */
	if ( pObjIndex->item_type == ITEM_LIGHT
	    && IS_SET( pObjIndex->extra_flags, ITEM_INVIS ) )
	{
       // @@@ Add this to the warnings list when ready
       char szBuffer[128];
       wsprintf(szBuffer,
          "Object #%d: light source is invisible.  Make visible?", vnum );
       if (MessageBox(0, szBuffer, "MAKE.ZONES.FAST", MB_ICONQUESTION|MB_YESNO)
             == IDYES)
   	    REMOVE_BIT( pObjIndex->extra_flags, ITEM_INVIS );
	}
#endif

	for ( ; ; )	{
	    char letter = fread_letter( is );

		 if ( letter == 'A' ) {
          PAffect paf = new TAffect;
   		 paf->location		= (sh_int) fread_number( is, "Reading object apply location" );
	   	 paf->modifier		= (sh_int) fread_number( is, "Reading object apply amount" );
          pObjIndex->affected.Add(paf);
       } else if ( letter == 'E' ) {
          PExtraDescr ed = new TExtraDescr();

		    ed->keyword		= fread_string( is, "Reading object extra description keyword" );
  		    ed->description	= fread_string( is, "Reading object extra description" );
          pObjIndex->extra_descr.Add(ed);
       } else if ( letter == 'F' ) {
         letter = fread_letter(is);
         PAffect paf = new TAffect;
         int where;
         switch (letter) {
         case 'A':
            where = TO_AFFECTS;
		      break;
		   case 'I':
		      where = TO_IMMUNE;
		      break;
		   case 'R':
		      where = TO_RESIST;
		      break;
		   case 'V':
		      where = TO_VULN;
		      break;
		   default:
            SyntaxError(is, "Reading object F modifier", "Modifier not one of: AIRV");
         }

         paf->location = (sh_int) fread_number(is, "Reading ROM Object Flag Location");
         paf->modifier = (sh_int) fread_number(is, "Reading ROM object Flag Modifier");
         int32 bitvector = fread_flag(is, "Reading ROM Object Flag Bit Vector");
//         paf->bitvector = fread_flag(is, "Reading ROM Object Flag Bit Vector");

         // Unlike ROM, we or together the bit vectors at this point, as
         // ours are all permanent and we need to access them independently.
         switch(where) {
         case TO_AFFECTS:
            pObjIndex->affected_by |= bitvector;
            break;
         case TO_IMMUNE:
            pObjIndex->immunity |= bitvector;
            break;
         case TO_RESIST:
            pObjIndex->resistance |= bitvector;
            break;
         case TO_VULN:
            pObjIndex->vulnerability |= bitvector;
            break;
         };

         // Unlike ROM, we don't keep around any modifiers that don't
         // affect stats.  Bit modifiers are stored in the four bit
         // vectors we keep for each object.  We will regenerate the
         // bit modifiers when we write out the object
         if (/*paf->where != TO_AFFECTS ||*/ (paf->location == 0 && paf->modifier == 0))
            delete &*paf;
         else
            pObjIndex->affected.Add(paf);
      } else {
         is.putback(letter);
		   break;
	   }
	}
}

void TAreaDocument::LoadObject(TObject *pObjIndex, vnum_int vnum, istream& is)
{
   poundsApp::currMode mode = MuditorApp->CurrentMode;

	pObjIndex->vnum			= vnum;
	pObjIndex->name			= fread_string( is, "Reading object name" );
	pObjIndex->short_descr	= fread_string( is, "Reading object short description" );
	pObjIndex->description	= fread_string( is, "Reading object description" );
   if (!pObjIndex->short_descr.is_null())
   	pObjIndex->short_descr[0]	= LOWER( pObjIndex->short_descr[0] );
   if (!pObjIndex->description.is_null())
   	pObjIndex->description[0]	= UPPER( pObjIndex->description[0] );

   if (mode != poundsApp::RomMode)
	/* Action description */ fread_string( is, "Ignoring object action description" );
   else
		pObjIndex->material = fread_string( is, "Reading object material" );

   // Get a pointer to the library in case we need it
   TLibrary *pLibrary = const_cast<poundsApp*>(GetMZF())->GetLibrary();

   // @@@ IS this correct for ROM 2.3?  Are there multiple ROM 2.3 syntaxs?
 	pObjIndex->item_type		= (sh_int) fread_number( is, "Reading object item type" );
   pObjIndex->strType      = pLibrary->GetStringFromType(pObjIndex->item_type);

   if (mode == poundsApp::RomMode)
      {
      pObjIndex->extra_flags          = fread_flag( is, "Reading object extra flags" );
      pObjIndex->wear_flags           = fread_flag( is, "Reading object wear position bits" );
      }
   else
      {
      pObjIndex->extra_flags		= fread_number( is, "Reading object extra flags" );
      pObjIndex->wear_flags		= fread_number( is, "Reading object wear position bits" );
      }

   if (mode == poundsApp::EnvyMode || mode == poundsApp::Envy2Mode)
      {
      pObjIndex->value[0] = fread_string( is, "Reading object value0" );
      pObjIndex->value[1] = fread_string( is, "Reading object value1" );
      pObjIndex->value[2] = fread_string( is, "Reading object value2" );
      pObjIndex->value[3] = fread_string( is, "Reading object value3" );

      // Now, in Envy, the spells are saved as strings but everything else
      // is a number.  Some numbers, like weapon types and drink types we
      // want to convert to strings, so we can have an internal ROM-like
      // representation.
      for (int i = 0; i < 4; i++) {
         if (pObjIndex->value[i].length() > 32) {
            SyntaxError(is, "Reading object values", "Value to long--Check file format");
         }

         const char *pszPrompt = pLibrary->GetPromptText(pObjIndex->strType, i);
         if (strcmp(pszPrompt, "spell")) {
            // This is not a spell
            char szValue[64];
            if (pLibrary->ListNumber2Str(pszPrompt,
                  atoi(pObjIndex->value[i].c_str()), szValue)) {
               pObjIndex->value[i] = szValue;
            }
         }
      }
   } else if (mode == poundsApp::RomMode) {
//      int32 n;
//      char szBuffer[32];
//      n = fread_flag( is, "Reading object value0" );
//      wsprintf(szBuffer, "%ld", n);
//	   pObjIndex->value[0] = szBuffer;
//      n = fread_flag( is, "Reading object value1" );
//      wsprintf(szBuffer, "%ld", n);
//	   pObjIndex->value[1] = szBuffer;
//      n = fread_flag( is, "Reading object value2" );
//      wsprintf(szBuffer, "%ld", n);
//	   pObjIndex->value[2] = szBuffer;
//      n = fread_flag( is, "Reading object value3" );
//      wsprintf(szBuffer, "%ld", n);
//	   pObjIndex->value[3] = szBuffer;
//	   n = fread_flag( is, "Reading object value4" );
//      wsprintf(szBuffer, "%ld", n);
//	   pObjIndex->value[4] = szBuffer;

      // Now, in Rom 2.3, we are using numbers for everything.  Convert
      // to Rom 2.4 style strings.
      for (int i = 0; i < 5; i++) {
         int32 n = fread_flag( is, "Reading object value" );
         const char *pszPrompt = pLibrary->GetPromptText(pObjIndex->strType, i);
         char szValue[64];
         if (pLibrary->ListNumber2Str(pszPrompt, n, szValue)) {
            pObjIndex->value[i] = szValue;
         } else {
            char szBuffer[32];
            wsprintf(szBuffer, "%ld", n);
            pObjIndex->value[i] = szBuffer;
         }
      }
   } else {
      // Now, in Merc/TFC, we are using numbers for everything.  Convert
      // to Rom 2.4 style strings.
      for (int i = 0; i < 4; i++) {
         int32 n = fread_flag( is, "Reading object value" );
         const char *pszPrompt = pLibrary->GetPromptText(pObjIndex->strType, i);
         char szValue[64];
         if (pLibrary->ListNumber2Str(pszPrompt, n, szValue)) {
            pObjIndex->value[i] = szValue;
         } else {
            char szBuffer[32];
            wsprintf(szBuffer, "%ld", n);
            pObjIndex->value[i] = szBuffer;
         }
      }
//      int32 n[4];
//      char szBuffer[32];
//      n[0] = fread_number( is, "Reading object value0--Check file format" );
//      wsprintf(szBuffer, "%ld", n[0]);
//      pObjIndex->value[0] = szBuffer;
//      n[1] = fread_number( is, "Reading object value1--Check file format" );
//      wsprintf(szBuffer, "%ld", n[1]);
//      pObjIndex->value[1] = szBuffer;
//      n[2] = fread_number( is, "Reading object value2--Check file format" );
//      wsprintf(szBuffer, "%ld", n[2]);
//      pObjIndex->value[2] = szBuffer;
//      n[3] = fread_number( is, "Reading object value3--Check file format" );
//      wsprintf(szBuffer, "%ld", n[3]);
//      pObjIndex->value[3] = szBuffer;
////      pObjIndex->value[4] = "0";
//      pObjIndex->value[4] = "";
//
//      // @@@ Replace this with database stuff?
//      int i;
//      switch (pObjIndex->item_type) {
//      case ITEM_WEAPON:
//          pObjIndex->value[3] = TAreaDocument::DamageType(n[3]);
//          break;
//
//      case ITEM_SCROLL:
//      case ITEM_POTION:
//      case ITEM_PILL:
//          for (i = 1; i < 4; i++)
//             pObjIndex->value[i] = TAreaDocument::SpellName(n[i], TRUE);
//          break;
//
//       case ITEM_WAND:
//       case ITEM_STAFF:
//          pObjIndex->value[3] = TAreaDocument::SpellName(n[3], TRUE);
//          break;
//
//       case ITEM_DRINK_CON:
//		    pObjIndex->value[2] = TAreaDocument::LiquidType(n[2]);
////          pObjIndex->value[3] = (UPPER(pObjIndex->value[3][0]) == 'T');
//          break;
//       }
   }

   if (mode == poundsApp::RomMode)
   	pObjIndex->level		= (sh_int) fread_number( is, "Reading object level" );

	pObjIndex->weight		= fread_number( is, "Reading object weight" );
	pObjIndex->cost			= fread_number( is, "Ignoring object cost" );
   if (mode == poundsApp::RomMode)
      {
      /* condition */
		pObjIndex->condition = fread_letter( is /*, "Reading object condition"*/ );
      }
   else
   	/* Cost per day */ fread_number( is, "Ignoring object rent" );   /* Unused */

	/*
	 * Check here for the redundancy of invisible light sources - Kahn
	 */
	if ( pObjIndex->item_type == ITEM_LIGHT
	    && IS_SET( pObjIndex->extra_flags, ITEM_INVIS ) )
	{
       // @@@ Add this to the warnings list when ready
       char szBuffer[128];
       wsprintf(szBuffer,
          "Object #%d: light source is invisible.  Make visible?", vnum );
       if (MessageBox(0, szBuffer, "MAKE.ZONES.FAST", MB_ICONQUESTION|MB_YESNO)
             == IDYES)
   	    REMOVE_BIT( pObjIndex->extra_flags, ITEM_INVIS );
	}

	for ( ; ; )
	{
	    char letter;

	    letter = fread_letter( is );

		 if ( letter == 'A' )
	       {
          PAffect paf = new TAffect;
   		 paf->location		= (sh_int) fread_number( is, "Reading object apply location" );
	   	 paf->modifier		= (sh_int) fread_number( is, "Reading object apply amount" );
          pObjIndex->affected.Add(paf);
	       }

	    else if ( letter == 'E' )
	       {
          PExtraDescr ed = new TExtraDescr();

		    ed->keyword		= fread_string( is, "Reading object extra description keyword" );
  		    ed->description	= fread_string( is, "Reading object extra description" );
          pObjIndex->extra_descr.Add(ed);
	       }

	    else
	       {
          is.putback(letter);
		    break;
	       }
	}

#if 0
   if (mode == poundsApp::EnvyMode || mode == poundsApp::Envy2Mode)
      {
		char szBuffer[32];

	/*
	 * Translate character strings *value[] into integers:
	 */
	switch ( pObjIndex->item_type )
	{
	case ITEM_PILL:
	case ITEM_POTION:
	case ITEM_SCROLL:
       wsprintf(szBuffer, "%d", SpellValue( pObjIndex->value[1].c_str()));
	    pObjIndex->value[1] = szBuffer;
       wsprintf(szBuffer, "%d", SpellValue( pObjIndex->value[2].c_str()));
	    pObjIndex->value[2] = szBuffer;
       wsprintf(szBuffer, "%d", SpellValue( pObjIndex->value[3].c_str()));
	    pObjIndex->value[3] = szBuffer;
	    break;

	case ITEM_STAFF:
	case ITEM_WAND:
       wsprintf(szBuffer, "%d", SpellValue( pObjIndex->value[3].c_str()));
	    pObjIndex->value[3] = szBuffer;
	    break;
	}
   }
#endif
}

/*
 * Snarf an obj section.
 */
void TAreaDocument::load_objects( istream& is, poundsApp::currMode mode )
{
//    OBJ_INDEX_DATA *pObjIndex;
    PObject pObjIndex;
    char szBuffer[256];

    for ( ; ; ) {
      char letter	= fread_letter( is );
      if ( letter != '#' )
         SyntaxError(is, "Looking for a new object", "Found something other than #");

      vnum_int vnum = (vnum_int) fread_number( is, "Reading object VNUM" );
      if ( vnum == 0 )
         break;

      if ( get_obj_index( vnum ) ) {
         wsprintf(szBuffer, "Two objects trying to use vnum #%d", vnum);
         SyntaxError(is, "Reading objects", szBuffer);
      }

      pObjIndex = new TObject(this);
   switch (mode) {
   case poundsApp::Rom24Mode:
      LoadRom24Object(pObjIndex, vnum, is);
      break;
   default:
      LoadObject(pObjIndex, vnum, is);
      break;
   }

	if (nNextObjVNum < vnum)
       {
       // First obj seen?
       if (nNextObjVNum == -1)
          {
          if (nNextRoomVNum == -1)
             nNextRoomVNum = vnum;
          if (nNextMobVNum == -1)
             nNextMobVNum = vnum;
          }

       nNextObjVNum = vnum;
		 }

	 TheObjects.Add(pObjIndex);
	 strcpy(szLastSuccess, pObjIndex->short_descr.c_str());
	 }
}

void TAreaDocument::load_socials( istream& is )
{
   SyntaxError(is, "Reading #SOCIALS section", "MZF cannot read ROM socials");
}

void TAreaDocument::load_creator( istream& is )
{
   SyntaxError(is, "Reading #SOCIALS section", "MZF cannot read ROM creator section");
}

