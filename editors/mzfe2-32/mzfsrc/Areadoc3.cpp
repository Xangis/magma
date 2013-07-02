/*  Project pounds

    Copyright © 1994-1996. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         areadoc.cpp
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file (3 of 3) for implementation of TAreaDocument (TFileDocument).
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
 *  Rom Mud improvements copyright (C) Alander                             *
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

// MZF includes
#include "areadoc.h"

// OWL includes
#include <owl\combobox.h>

/*
 * Snarf a reset section.
 */
void TAreaDocument::load_resets( istream& is )
{
	 PReset pReset;
	 PMobReset pMobReset = 0;
	 char szBuffer[256];
	 PRoom pRoom;

	 if (name.is_null())
		 SyntaxError(is, "Loading resets", "There must be an #AREA section before the #RESETS section");

    for ( ; ; )
    {
    PExit pexit;
    PRoom pRoomIndex;
    char letter;

	if ( ( letter = fread_letter( is ) ) == 'S' )
	    break;

   if (letter == 'M')
      {
      if (pMobReset)
         TheMobResets.Add(pMobReset);
		pReset = pMobReset = new TMobReset(this);
      }
   else
		pReset = new TReset(this);

	if ( letter == '*' )
	{
	    pReset->comment = fread_to_eol( is );
       TheResets.Add(pReset);
	    continue;
	}

	pReset->command	= letter;
	/* if_flag */	  fread_number( is, "Ignoring reset if-flag" );
	pReset->arg1	= (sh_int) fread_number( is, "Reading reset arg1" );
	pReset->arg2	= (sh_int) fread_number( is, "Reading reset arg2" );
	pReset->arg3	= ( letter == 'G' || letter == 'R' ||
                      letter == 'C' || letter == 'F' )
			    ? (sh_int) 0 : (sh_int) fread_number( is, "Reading reset arg3" );
   if (MuditorApp->CurrentMode == poundsApp::Rom24Mode)
   	pReset->arg4	= (letter == 'P' || letter == 'M')
			    ? (sh_int) fread_number(is, "Reading ROM reset arg4") :
               (sh_int) 0;
	pReset->comment = fread_to_eol( is );

   TObject *obj;
	switch ( letter )
	{
	default:
//	    bug( "Load_resets: bad command '%c'.", letter );
		 wsprintf(szBuffer,
          "TAreaDocument::load_resets(): Unknown reset '%c'.  Try to continue?",
          letter);
		 if (MessageBox(0, szBuffer, "MAKE.ZONES.FAST", MB_ICONQUESTION|MB_YESNO)
              != IDYES)
          THROW( TXOwl("") );
	    break;

	case 'M':
	    /*pMobile =*/ get_mob_index  ( pReset->arg1 );
       pRoom = get_room_index ( pReset->arg3 );
       if (pRoom)
			 pRoom->MobResets.Add(pMobReset);
	    break;

	case 'O':
//	    get_obj_index  ( pReset->arg1 );
       pRoom = get_room_index ( pReset->arg3 );
       if (pRoom)
          pRoom->ObjResets.Add(pReset);
	    break;

	case 'P':
//	    get_obj_index  ( pReset->arg1 );
//	    get_obj_index  ( pReset->arg3 );
		 break;

	case 'E':
	    obj = get_obj_index  ( pReset->arg1 );
		 if (obj)
          obj->count = pReset->arg2;
       if (pMobReset)
          pMobReset->Wields.Add(pReset);
	    break;

	case 'G':
	    obj = get_obj_index  ( pReset->arg1 );
       if (obj)
			 obj->count = pReset->arg2;
       if (pMobReset)
          pMobReset->Inventory.Add(pReset);
	    break;

   case 'C':
   case 'F':
       if (pMobReset)
          pMobReset->Inventory.Add(pReset);
	    break;

	case 'D':
	    pRoomIndex = get_room_index( pReset->arg1 );

       // ECS added test for valid room to allow editor to handle zones that
       // reset doors in other zones.
       if (pRoomIndex)
			 {
          char szError[512];

          // Check for a valid door direction (N, S, E, W, U, D)
          if ( pReset->arg2 < 0 || pReset->arg2 > 5 )
             {
             pRoomIndex->GetName(szBuffer, sizeof szBuffer);
             wsprintf(szError, "%s: Door reset specifies invalid door %d.",
                szBuffer, pReset->arg2);
//             bug(szError, 0);
             SyntaxError(is, "Reading D reset", szBuffer);
             /* NOTREACHED */
             }

          // Check for a valid door position (Open, Closed, Locked)
          const char *szExitNames[] =
             { "North", "East", "South", "West", "Up", "Down" };
   	    if ( pReset->arg3 < 0 || pReset->arg3 > 2 )
             {
             pRoomIndex->GetName(szBuffer, sizeof szBuffer);
             wsprintf(szError, "%s: %s door is set into invalid position %d."
                "  Make it a door?", szBuffer, szExitNames[pReset->arg2],
					 pReset->arg3);
				 SyntaxError(is, "Checking for valid door position", szError);
				 }

			 // Verify that the room knows about the door
			 if ( !IS_SET( pRoomIndex->exit[pReset->arg2].exit_info, EX_ISDOOR ) )
				 {
				 pRoomIndex->GetName(szBuffer, sizeof szBuffer);
				 wsprintf(szError,
					 "%s: %s door is set into position, but isn't a door."
					 "  Make it a door?", szBuffer, szExitNames[pReset->arg2]);
				 if (MessageBox(0, szError, "MAKE.ZONES.FAST",
														  MB_ICONQUESTION|MB_YESNO) == IDYES)
                {
                pRoomIndex->exit[pReset->arg2].exit_info = EX_ISDOOR;
                pRoomIndex->is_exit[pReset->arg2] = TRUE;
                }
             }

          // @@@ Check for an already exiting reset for this door...
          pRoomIndex->exit[pReset->arg2].InitialPosition = pReset;
          pRoomIndex->exit[pReset->arg2].nInitialPosition = pReset->arg3;
          }

		 break;

	case 'R':
		 pRoomIndex		= get_room_index( pReset->arg1 );

	    if ( pReset->arg2 < 0 || pReset->arg2 > 6 )
			 SyntaxError(is, "Reading R randomize reset", "R reset specifies invalid number of exits");

		 if (pRoomIndex)
			 pRoomIndex->RandomDoor = pReset;

		 break;
	}

	TheResets.Add(pReset);
	}

	if (pMobReset)
		TheMobResets.Add(pMobReset);
}



/*
 * Snarf a room section.
 */
void TAreaDocument::load_rooms( istream& is, poundsApp::currMode mode)
{
   PRoom pRoomIndex;
   char szBuffer[256];

   if (name.is_null()) {
      SyntaxError(is, "Reading #ROOMS", "There must be an #AREA section before the #ROOMS section");
   }

   for ( ; ; ) {
      int  door;

      char letter = fread_letter( is );
      if ( letter != '#' ) {
         SyntaxError(is, "Looking for a room", "Room did not begin with # character");
      }

	   vnum_int vnum = (vnum_int) fread_number( is, "Reading room VNUM" );
	   if ( vnum == 0 )
		    break;

	   if ( get_room_index( vnum ) )	{
       wsprintf(szBuffer, "Room vnum #%d duplicated", vnum);
		 SyntaxError(is, "Reading rooms", szBuffer);
      }

	   pRoomIndex = new TRoom(this);
   	pRoomIndex->vnum		= vnum;
	   pRoomIndex->name		= fread_string( is, "Reading room name" );
   	pRoomIndex->description		= fread_string( is, "Reading room description" );
   	/* Area number */		  fread_number( is, "Ignoring room area number" );   /* Unused */
      if (MuditorApp->CurrentMode == poundsApp::RomMode || MuditorApp->CurrentMode == poundsApp::Rom24Mode)
      	pRoomIndex->room_flags		= fread_flag( is, "Reading room flags" );
      else
	      pRoomIndex->room_flags		= fread_number( is, "Reading room flags" );
   	pRoomIndex->sector_type		= (sh_int) fread_number( is, "Reading room sector type" );
	   for ( door = 0; door <= 5; door++ )
	       pRoomIndex->is_exit[door] = FALSE;

   	for ( ;; ) {
         letter = fread_letter( is );

         if ( letter == 'S' || letter == 's' )
            break;

         else if ( letter == 'H') // ROM healing room
            pRoomIndex->heal_rate = (sh_int) fread_number(is, "Reading ROM heal rate");

         else if ( letter == 'M') // ROM mana room
            pRoomIndex->mana_rate = (sh_int) fread_number(is, "Reading ROM mana rate");

         else if ( letter == 'C') { // ROM clan room
            if (!pRoomIndex->clan.is_null()) {
               SyntaxError(is, "Reading rooms", "Multiple ROM clan names");
            }

		      pRoomIndex->clan = fread_string(is, "Reading ROM clan name");
         } else if ( letter == 'D' ) {
            door = (sh_int) fread_number( is, "Reading door number" );
            if ( door < 0 || door > 5 ) {
               wsprintf(szBuffer, "Room vnum #%d has bad door number", vnum);
               SyntaxError(is, "Reading exit", szBuffer);
            }

            PExit pexit = &pRoomIndex->exit[door];
            pRoomIndex->is_exit[door] = TRUE;
      		pexit->description	= fread_string( is, "Reading exit description" );
      		pexit->keyword		= fread_string( is, "Reading exit keyword" );
		      pexit->exit_info	= 0;
      		int locks			= (int) fread_number( is, "Reading 'locks' value" );
      		pexit->key		= (vnum_int) fread_number( is, "Reading door key vnum" );
      		pexit->vnum		= (vnum_int) fread_number( is, "Reading exit 'to room'" );

            if (mode == poundsApp::Rom24Mode || mode == poundsApp::RomMode) {
               switch ( locks ) {
               case 1: pexit->exit_info = EX_ISDOOR;                break;
               case 2: pexit->exit_info = EX_ISDOOR | EX_PICKPROOF; break;

               // Just to be different, ROM has different numbers for NOPASS
               // than Envy has for PASSPROOF
               case 3: pexit->exit_info = EX_ISDOOR | EX_NOPASS;    break;
               case 4: pexit->exit_info = EX_ISDOOR|EX_NOPASS|EX_PICKPROOF; break;
               }
            } else {
               // This is the Envy version, but it works with Merc too
               switch ( locks ) {
               case 1: pexit->exit_info = EX_ISDOOR;                    break;
               case 2: pexit->exit_info = EX_ISDOOR | EX_PICKPROOF;     break;
               case 3: pexit->exit_info = EX_ISDOOR | EX_BASHPROOF;     break;
               case 4: pexit->exit_info = EX_ISDOOR | EX_PICKPROOF
										 | EX_BASHPROOF;                 break;
         		case 5: pexit->exit_info = EX_ISDOOR | EX_PASSPROOF;     break;
         		case 6: pexit->exit_info = EX_ISDOOR | EX_PICKPROOF
										 | EX_PASSPROOF;                 break;
         		case 7: pexit->exit_info = EX_ISDOOR | EX_BASHPROOF
		                         | EX_PASSPROOF;                 break;
           		case 8: pexit->exit_info = EX_ISDOOR | EX_PICKPROOF
		                         | EX_BASHPROOF | EX_PASSPROOF;  break;
               }
   		   }
	      } else if ( letter == 'E' ) {
            PExtraDescr ed;

            ed = new TExtraDescr();
            ed->keyword		= fread_string( is, "Reading room extra description keyword" );
            ed->description		= fread_string( is, "Reading room extra description" );
            pRoomIndex->extra_descr.Add(ed);
      	} else if (letter == 'O') {
            if (!!pRoomIndex->owner.is_null()) {
               SyntaxError(is, "Reading room", "Duplicate owner (ROM)");
            }

      		pRoomIndex->owner = fread_string(is, "Reading owner (ROM)");
         } else {
            wsprintf(szBuffer, "Vnum #%d has a flag that is not D, E, or S, or ROM flags H, M, C, or O: %c",
               vnum, letter);
            SyntaxError(is, "Loading room extras and doors", szBuffer);
	      }
	   }

    if (nNextRoomVNum < vnum)
		 {
       // First room seen?
       if (nNextRoomVNum == -1)
			 {
          if (nNextObjVNum == -1)
             nNextObjVNum = vnum;
          if (nNextMobVNum == -1)
             nNextMobVNum = vnum;
          }

       nNextRoomVNum = vnum;
       }
    TheRooms.Add(pRoomIndex);
    strcpy(szLastSuccess, pRoomIndex->name.c_str());
    }

    return;
}



/*
 * Snarf a shop section.
 */
void TAreaDocument::load_shops( istream& is )
{
	 PShopData pShop;

	 for ( ; ; )
	 {
	PMobile pMobIndex;
	int iTrade;

	pShop = new TShopData(this);
	pShop->keeper		= (sh_int) fread_number( is, "Reading shopkeeper vnum" );
	if ( pShop->keeper == 0 )
		 break;
	for ( iTrade = 0; iTrade < MAX_TRADE; iTrade++ )
		 pShop->buy_type[iTrade] = (sh_int) fread_number( is, "Reading shopkeeper trade types" );
	pShop->profit_buy	= (sh_int) fread_number( is, "Reading shopkeeper profit buy %" );
	pShop->profit_sell	= (sh_int) fread_number( is, "Reading shopkeeper profit sell %" );
	pShop->open_hour	= (sh_int) fread_number( is, "Reading shopkeeper open hour" );
	pShop->close_hour	= (sh_int) fread_number( is, "Reading shopkeeper close hour" );
	pShop->comment = 			  fread_to_eol( is );
	pMobIndex		= get_mob_index( pShop->keeper );
	// We check for a valid shopkeeper, as some zones (eg OfCol) make mobs in
	// other zones shopkeepers...
	if (pMobIndex)
		pMobIndex->pShop	= pShop;

	TheShops.Add(pShop);
	 }

    return;
}



/*
 * Snarf spec proc declarations.
 */
void TAreaDocument::load_specials( istream& is )
{
   char szBuffer[256];
   vnum_int vnum;

   for ( ; ; ) {
      PMobile pMobIndex;
	   char letter;

	   switch ( letter = fread_letter( is ) ) {
	      default:
            wsprintf(szBuffer, "Expecting *, M, or S, got '%c'", letter);
            SyntaxError(is, "Reading a special entry", szBuffer);

         case 'S':
				return;

	      case '*':
				// @@@ We lose all '*' comments in #SPECIALS
      	   fread_to_eol( is );
	         break;

         case 'M':
            vnum = (vnum_int) fread_number(is, "Reading mob vnum");
	         pMobIndex = get_mob_index(vnum);
            if (pMobIndex == 0) {
               char szBuffer[256];
//               wsprintf(szBuffer,
//                  "M reset specified for mobile %d, but mobile not in area file",
//                  vnum);
//               SyntaxError(is, "reading 'M' reset", szBuffer);
               wsprintf(szBuffer, "MZF encountered an M reset for mobile %d.  This "
                  "mobile is not in the area file.  MZF will not write this reset "
                  "when this area is saved.", vnum);
               MessageBox(0, szBuffer, "MAKE.ZONES.FAST", MB_ICONASTERISK|MB_OK);
          	   fread_to_eol( is );
            } else {
   	         pMobIndex->spec_fun = fread_word( is, "Reading spec-fun name" );
          	   pMobIndex->spec_comment = fread_to_eol( is );
            }
            break;
	      }
      }
}

#define UMIN( a, b )		( ( a ) < ( b ) ? ( a ) : ( b ) )
#define UMAX( a, b )		( ( a ) > ( b ) ? ( a ) : ( b ) )

/*
 * Troll-only
 *
 * Snarf an 'area' header line.   Check this format.  MUCH better.
 * Add fields too.
 *
 * Function taken from 'The Isles' code.  Walker.
 *
 * #AREADATA
 * Name   { All } Locke    Newbie School~
 * Repop  A teacher pops in the room and says, 'Repop coming!'~
 * Recall 3001
 * Weather <weather data>
 * Timezone 12
 * Flags 4096
 * End
 */
void TAreaDocument::load_troll_area( istream& is )
{
//    AREA_DATA *pArea;
	 char buf[ 128 ];
    char      *word;
//    BOOL      fMatch;
    int /*min, max,*/ n;	/* For James' new weather system.  Walker */

    for ( ; ; )
    {
	word   = fread_word( is, "Reading 'word'" );
//	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	    case 'N':
			 // @@@ Should display a warning if it's not "Name"
          if (stricmp(word, "Name") == 0)
             name = fread_string(is, "Reading Name field");

	       break;
	    case 'E':
		if ( !stricmp( word, "End" ) )
		{
          fUsesAreaData = TRUE;
		    return;
		}
		break;
	    case 'F':
//		   KEY( "Flags", area_data.area_flags, fread_number( is ) );
         if (stricmp(word, "Flags") == 0)
            area_data.area_flags = (sh_int) fread_number( is, "Reading Flags" );
	      break;
	    case 'R':
		if ( !stricmp( word, "Recall" ) )
		{
		    recall = (sh_int) fread_number( is, "Reading recall point" );
		    if ( recall < 1 )
		    {
			wsprintf( buf, "Load_recall:  %s invalid recall point", name.c_str() );
//			bug( buf, 0 );
         SyntaxError(is, "Reading recall point", buf);
//			area_data.recall = ROOM_VNUM_TEMPLE;
		    }
                }
		else if ( !stricmp( word, "Repop" ) )
         area_data.repop = fread_string( is, "Reading repop message" );
		else if ( !stricmp( word, "Reset" ) )
		{
		    fread_to_eol( is );
//		    fMatch = TRUE;
		}
		break;
	    case 'T':
		/* Read James' time zone field.  Walker */
                if ( !stricmp( word, "Timezone" ) )
		{
		    area_data.timezone = (sh_int) fread_number( is, "Reading timezone" );
		    if ( area_data.timezone < -24 || area_data.timezone > 24)
		    {
//			wsprintf( buf, "Load_timezone:  %s invalid time zone", name.c_str() );
//			bug( buf, 0 );
         SyntaxError(is, "Reading Timezone", "Invalid timezone");
//			area_data.timezone = 0;
		    }
		}
		break;
	    case 'W':
		/* For new weather system. James. */
		if ( !stricmp( word, "Weather" ) )
		{
			area_data.fCustomWeather = TRUE;
			n = (sh_int) fread_number( is, "Reading Min Temp" );
		    area_data.Weather.MinTemp = UMAX( -200, n);
			n = (sh_int) fread_number( is, "Reading Max Temp" );
		    area_data.Weather.MaxTemp = UMIN( 200, n);
			n = (sh_int) fread_number( is, "Reading Min Precip" );
		    area_data.Weather.MinPrecip = UMAX( 0, n);
			n = (sh_int) fread_number( is, "Reading Max Precip" );
		    area_data.Weather.MaxPrecip = UMIN( 100, n);
			n = (sh_int) fread_number( is, "Reading Min Wind" );
		    area_data.Weather.MinWind = UMAX( 0, n);
			n = (sh_int) fread_number( is, "Reading Max Wind" );
		    area_data.Weather.MaxWind = UMIN( 200, n);
			n = (sh_int) fread_number( is, "Reading Min Magic" );
		    area_data.Weather.MinMagic = UMAX( 0, n);
			n = (sh_int) fread_number( is, "Reading Max Magic" );
		    area_data.Weather.MaxMagic = UMIN( 100, n);
			n = (sh_int) fread_number( is, "Reading MaxChange" );
		    /*	 area_data.Weather.MaxChange = UMIN( 20, n);  */
		    area_data.Weather.MaxChange = n;
		}
         break;
      default:
			wsprintf(buf, "Unknown #AREADATA member: %s.  MEMBER WILL NOT SAVE!",
            word);
         MessageBox(0, buf, "M.Z.F.", MB_ICONHAND|MB_OK);
		} /* case */

      } /* for */
}


/*
 * Translate all room exits from virtual to real.
 * Has to be done after all rooms are read in.
 * Check for bad or suspicious reverse exits.
 */
void TAreaDocument::fix_exits( void )
{
//		 EXIT_DATA       *pexit;
       PExit pexit, pexit_rev;
//		 EXIT_DATA       *pexit_rev;
//		 ROOM_INDEX_DATA *pRoomIndex;
//		 ROOM_INDEX_DATA *to_room;
       PRoom pRoomIndex, to_room;
//                 char             buf     [ MAX_STRING_LENGTH ];
//    extern const int              rev_dir [ ];
//		 int              iHash;
		 int              door;

//    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
//    {
//	for ( pRoomIndex  = room_index_hash[iHash];
//	      pRoomIndex;
//	      pRoomIndex  = pRoomIndex->next )
      for ( TArrayAsVectorIterator<TRoom *> it(TheRooms); it; it++)
//      for ( TArrayAsVectorIterator<TVNumObj *> it(TheRooms); it; it++)
	{
       pRoomIndex = it.Current();
//       pRoomIndex = (TRoom *) it.Current();
//	    bool fexit;
       BOOL fexit;

	    fexit = FALSE;
	    for ( door = 0; door <= 5; door++ )
	    {
//		if ( ( pexit = pRoomIndex->exit[door] ) )
		if ( ( pRoomIndex->is_exit[door] ) )
		{
		    pexit = &pRoomIndex->exit[door];
		    fexit = TRUE;
		    if ( pexit->vnum <= 0 )
			pexit->to_room = NULL;
		    else
			pexit->to_room = get_room_index( pexit->vnum );
		}
		 }

		 if ( !fexit )
		SET_BIT( pRoomIndex->room_flags, ROOM_NO_MOB );
	}
//    }

#if 0 // This checks for exit coherence, there is no need...
//    for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
//    {
//	for ( pRoomIndex  = room_index_hash[iHash];
//	      pRoomIndex;
//	      pRoomIndex  = pRoomIndex->next )
		for ( it.Restart(); it; it++)
	{
		 for ( door = 0; door <= 5; door++ )
		 {
//		if (   ( pexit     = pRoomIndex->exit[door]       )
		if (   ( pRoomIndex->is_exit[door]       )
			 && ( to_room   = pRoomIndex->exit[door].to_room               )
//		    && ( pexit_rev = to_room->exit[rev_dir[door]] )
			 && ( to_room->is_exit[rev_dir[door]] )
//		    &&   pexit_rev->to_room != pRoomIndex )
			 &&   to_room->exit[rev_dir[door]].to_room != pRoomIndex )
		{
			 wsprintf( buf, "Fix_exits: %d:%d -> %d:%d -> %d.",
				 pRoomIndex->vnum, door,
				 pRoomIndex->exit[door].vnum,    rev_dir[door],
				 to_room->exit[rev_dir[door]].vnum );
		    bug( buf, 0 );
		}
	    }
	}
//    }
#endif
}

/*
 * Read a letter from a file.
 */
char TAreaDocument::fread_letter( istream& is )
{
    int c;

    do {
      c = is.get();
    } while ( isspace( c ) );

    return (char) c;
}



/*
 * Read a number from a file.
 */
int32 TAreaDocument::fread_number( istream& is, const char *operation )
{
    int c;
    int32  number;
    BOOL sign;

    // Skip leading spaces
    do {
	    c = is.get();
    } while ( isspace( c ) );

    number = 0;

    // Set sign == TRUE if there is a leading - sign
    sign   = FALSE;
    if ( c == '+' )
    {
	    c = is.get();
    }
    else if ( c == '-' )
    {
		 sign = TRUE;
	    c = is.get();
    }

    // If we don't have a digit here we are not at a number.
    if ( !isdigit( c ) )
    {
#if 0
       is.putback( (char) c );
       string s = fread_to_eol( is );
       MessageBox(0, s.c_str(), "fread_number() given this mess", MB_OK);
#endif
       SyntaxError(is, operation, "Bad number format");
    }

    // Read in the number
    while ( isdigit(c) )
    {
	    number = number * 10 + c - '0';
	    c = is.get();
    }

    // If it was a negative number, make it negative
    if ( sign )
	    number = 0 - number;

	 // If the number is | combined with another number, read the next number
    if ( c == '|' )
	    number += fread_number( is, operation );
    else if ( c != ' ' )
       is.putback( (char) c);

    return number;
}



/*
 * Read and allocate space for a string from a file.
 * These strings are read-only and shared.
 * Strings are hashed:
 *   each string prepended with hash pointer to prev string,
 *   hash code is simply the string length.
 * This function takes 40% to 50% of boot-up time.
 * @@@ This function GP faults if it reads a string longer than MAX_STRING_LEN
 */
string TAreaDocument::fread_string( istream& is, const char *operation )
{
    char *plast;
    int c;
    char szBuffer[MAX_STRING_LENGTH];

    plast = szBuffer;

    /*
     * Skip blanks.
     * Read first char.
     */
    do
    {
	c = is.get();
    }
    while ( isspace( c ) );

    if ( ( *plast++ = (char) c ) == '~' )
       return "";

    for ( ;; )
    {
	/*
	 * Back off the char type lookup,
	 *   it was too dirty for portability.
	 *   -- Furey
	 */
	switch ( *plast = (char) is.get() )
	{
	default:
	    plast++;
	    break;

	case EOF:
       SyntaxError(is, operation, "Unexpected End of File");

	case '~':
	    plast++;
		plast[-1] = '\0';
		    return string(szBuffer);
	}
    }
}



/*
 * Read to end of line (for comments).
 */
string TAreaDocument::fread_to_eol( istream& is )
{
    char szBuffer[MAX_STRING_LENGTH];
    char *plast = szBuffer;
    int c;

    do
    {
	c = is.get();
   *plast++ = (char) c;
    }
    while ( c != '\n' && c != '\r' );

    plast[-1] = '\0';
    do
    {
	c = is.get();
    }
    while ( c == '\n' || c == '\r' );

//    ungetc( c, fp );
    is.putback( (char) c);
    return string(szBuffer);
}



/*
 * Read one word (into static buffer).
 */
char *TAreaDocument::fread_word( istream& is, const char *operation )
{
    static char word[MAX_INPUT_LENGTH];

    // Skip any leading spaces
    int cEnd;
    do {
	    cEnd = is.get();
    } while ( isspace( cEnd ) && !is.bad());

    // Determine if this word ends with a special quote character, or
    // ordinary whitespace.  Set up to read the word.
    char *pword;
    if ( cEnd == '\'' || cEnd == '"' ) {
        pword = word;
    } else {
        word[0] = (char) cEnd;
        pword = word+1;
        cEnd = ' ';
    }

    // Read the word
    for ( ; pword < word + MAX_INPUT_LENGTH; pword++ ) {
        *pword = (char) is.get();
        if ((cEnd == ' ' ? isspace( *pword ) : *pword == cEnd) ||
            is.eof() || is.bad()) {
           if ( cEnd == ' ' )
               is.putback(*pword);
	        *pword = '\0';
	        return word;
        }
    }

    SyntaxError(is, operation, "Word too long");
    return NULL;
}

/*
 * Read ROM-style flags
 */
int32 TAreaDocument::fread_flag( istream& is, const char *operation )
{
   int32 number = 0;
   int c;

   do {
	    c = is.get();
   } while ( isspace( c ) );

   // Merc/Envy negative numbers (like -1 for lights, thanks Rainman)
   if (c == '-')
      return -fread_flag(is, operation);
      
   // ROM race flags
   if (!isdigit(c)) {
	   while (('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z')) {
          // Since ROM flags go to 32 bit, if we get bit 33 or above
          // we have a problem
          if ('e' < c && c <= 'z')
             SyntaxError(is, operation, "Invalid flag character");

	       number |= flag_convert((char) c);
	       c = is.get();
	   }
   }

   // Merc/Envy race numbers
   while (isdigit(c)) {
	   number = number * 10 + c - '0';
	   c = is.get();
   }

   if (c == '|')
	   number += fread_flag( is, operation );
   else if  ( c != ' ')
      is.putback( (char) c);

   return number;
}

// I've noticed some zones double up lower case characters, just like
// in merc.h, e.g. 'aa'.  @@@ Should MZF write them out as single or
// double charcters?
int32 TAreaDocument::flag_convert(char letter)
{
#if 0
   int32 bitsum = 0;
   char i;

   if ('A' <= letter && letter <= 'Z') {
	   bitsum = 1;
	   for (i = letter; i > 'A'; i--)
	      bitsum *= 2;
   } else if ('a' <= letter && letter <= 'z') {
	   bitsum = 67108864L; /* 2^26 @@@ Is this still valid for ROM 2.4? */
	   for (i = letter; i > 'a'; i --)
	      bitsum *= 2;
   }

   return bitsum;
#else
   if ('A' <= letter && letter <= 'Z') {
      return 1L << (letter - 'A');
   } else if ('a' <= letter && letter <= 'z') {
      return 1L << (26 + letter - 'a');
   } else
      return 0;
#endif
}

// Big concern here is that the old beginning # and the new beginning # might
// overlap.
void TAreaDocument::Renumber( sh_int nNewBase )
{
//    sh_int new_vnum;
   sh_int nNewRoomBase = nNewBase, nNewObjBase = nNewBase, nNewMobBase = nNewBase;

   // First, assign the new # to all of the objects
   for ( TArrayAsVectorIterator<TRoom *> itRoom(TheRooms); itRoom; itRoom++)
      itRoom.Current()->new_vnum = nNewRoomBase++;

   for ( TArrayAsVectorIterator<TMobile *> itMob(TheMobiles); itMob; itMob++)
      itMob.Current()->new_vnum = nNewMobBase++;

   for ( TArrayAsVectorIterator<TObject *> itObj(TheObjects); itObj; itObj++)
      itObj.Current()->new_vnum = nNewObjBase++;

   // Now, update all of the exits:
   for ( itRoom.Restart(); itRoom; itRoom++)
      for (int i = 0; i < 6; i++)
         if (itRoom.Current()->is_exit[i])
            {
            TRoom *r = itRoom.Current()->exit[i].to_room;
            if (r)
               {
               itRoom.Current()->exit[i].vnum = r->new_vnum;
               // ECS Added key renumbering 10/5/94
               if (itRoom.Current()->exit[i].key > 0)
                  {
                  TObject *pKey = get_obj_index(itRoom.Current()->exit[i].key);
                  if (pKey)
                     itRoom.Current()->exit[i].key = pKey->new_vnum;
                  }
               }
            }

   // Now, update all of the resets
   TMobile *pMobile;
   TRoom   *pRoom;
   TObject *pObj;
   for (TArrayAsVectorIterator<TReset *> itRes(TheResets); itRes; itRes++)
       {
       TReset *pReset = itRes.Current();
       switch (pReset->command) {

       case 'M': // Place mobile
	       pMobile = get_mob_index( pReset->arg1 );
          if (pMobile)
             pReset->arg1 = pMobile->new_vnum;
          pRoom = get_room_index( pReset->arg3 );
          if (pRoom)
             pReset->arg3 = pRoom->new_vnum;
          break;

       case 'O':	// Put an object in the room
	       pObj = get_obj_index( pReset->arg1 );
          if (pObj)
             pReset->arg1 = pObj->new_vnum;

          pRoom = get_room_index( pReset->arg3 );
          if (pRoom)
             pReset->arg3 = pRoom->new_vnum;
    	    break;

	    case 'P':  // Put an object in an object
	       pObj = get_obj_index( pReset->arg1 );
          if (pObj)
             pReset->arg1 = pObj->new_vnum;

	       pObj = get_obj_index( pReset->arg3 );
          if (pObj)
             pReset->arg1 = pObj->new_vnum;
   	    break;

	    case 'E':	// Equip a mobile
	    case 'G':  // Give a mobile some inventory
	       pObj = get_obj_index( pReset->arg1 );
          if (pObj)
             pReset->arg1 = pObj->new_vnum;
   	    break;

//       case 'C':
//       case 'F':

       case 'D': // Set a door
       case 'R': // Randomize a room
   	    pRoom = get_room_index( pReset->arg1 );
          if (pRoom)
             pReset->arg1 = pRoom->new_vnum;
   	    break;
	    }
       }

   // Now, update the shops
   for ( TArrayAsVectorIterator<TShopData *> itShop(TheShops); itShop; itShop++)
      {
	   pMobile = get_mob_index( itShop.Current()->keeper );
      if (pMobile)
         itShop.Current()->keeper = pMobile->new_vnum;
      }

   // We have now updated everything that is safe to update (we haven't touched
   // resets FOR OTHER ZONES or Slash's spec-fun cargo in long description.)

   // Complete the renumbering
   for ( itRoom.Restart(); itRoom; itRoom++)
      itRoom.Current()->vnum = itRoom.Current()->new_vnum;

   for ( itMob.Restart(); itMob; itMob++)
      itMob.Current()->vnum = itMob.Current()->new_vnum;

   for ( itObj.Restart(); itObj; itObj++)
      itObj.Current()->vnum = itObj.Current()->new_vnum;

   // Make the views redraw themselves
   SetDirty(TRUE);
   NotifyViews(vnRevert, TRUE);

//   // Success!!!
//   MessageBox(0, "Ok", "MAKE.ZONES.FAST", MB_ICONINFORMATION|MB_OK);
}

void TAreaDocument::SyntaxError(istream& is, const char *operation, const char *msg)
{
   // Go back to the beginning of the area file and count the number of newlines
   // it took to get here
   int iLine = 0;
   if (!is.bad() /*&& !is.eof()*/)
      {
      long lPos = is.tellg();

      // Remove EOF bit, before seeking to the beginning.  Otherwise it
      // will fail.  I'm not sure if that is a C++ standard or Borland's
      // library.
      is.clear();

      // Advance until we are back where the error occured.
      is.seekg(0);
   	for ( ; is.tellg() < lPos; iLine++ )
	   	while ( is.get() != '\n' && !is.bad() && !is.eof())
		      ;
      }

   char szBuffer[512];
   wsprintf(szBuffer, "Section: #%s\r\nLast Success: %s\r\nOperation: %s\r\n"
      "Line: %d\r\n Bug: %s", szSection, szLastSuccess, operation, iLine, msg);
   THROW( TXOwl(szBuffer) );
}

/* returns race number, for ROM/Envy2 */
sh_int TAreaDocument::race_lookup (const string& name) const
{
   if (GetMode() == poundsApp::RomMode ||
       GetMode() == poundsApp::Rom24Mode) {
      for (sh_int race = 0; rom_race_table[race].name != NULL; race++) {
   	   if (!name.is_null() && LOWER(name[0]) == LOWER(rom_race_table[race].name[0])
	         && !str_prefix(name.c_str(), rom_race_table[race].name))
	       return race;
      }

   } else if (GetMode() == poundsApp::Envy2Mode) {
      for (sh_int index = 0; index < MAX_RACE; index++ )
           if ( !str_prefix(name.c_str(), envy_race_table[index].name ) )
   	    return index;
   }

   return 0;   // Race 0 is 'unique', and has no bits
}

/*
 * Compare strings, case insensitive, for prefix matching.
 * Return TRUE if astr not a prefix of bstr
 *   (compatibility with historical functions).
 */
BOOL TAreaDocument::str_prefix( const char *astr, const char *bstr ) const
{
    if ( astr == NULL )
    {
//	bug( "Strn_cmp: null astr.", 0 );
	return TRUE;
    }

    if ( bstr == NULL )
    {
//	bug( "Strn_cmp: null bstr.", 0 );
	return TRUE;
    }

    for ( ; *astr; astr++, bstr++ )
    {
	if ( LOWER(*astr) != LOWER(*bstr) )
		 return TRUE;
    }

	 return FALSE;
}

///* returns material number */
//int TAreaDocument::material_lookup (const char *name)
//{
//    // The original ROM version is empty, as well...
//    return 0;
//}

#if 0
// Read one word (into static buffer).  Single (') or double (") quotes
// at the start of a word causes us to return the entire quoted phrase.
// Don't remove quoting.
//
// Having the quotes makes things harder for MZF, but it makes
// it more flexible about ROM 2.4 objects it doesn't understand.
//
// (The proceeding is incorrect.  We need to remove the quotes after all,
// so we disable this routine.--Slash)
char *TAreaDocument::fread_word2( istream& is, const char *operation )
{
   static char word [ MAX_INPUT_LENGTH ];
   char *pword = word;

   // Skip leading spaces
   int cEnd;
   do {
	    cEnd = is.get();
   } while ( isspace( cEnd ) );

   *pword++ = (char) cEnd;
   if ( cEnd != '\'' && cEnd != '"' )
      cEnd = ' ';

   // Keep looking for the end of the word (white space, except for quoted
   // phrases), until we run out of buffer space.
   for ( ; pword < word + MAX_INPUT_LENGTH; pword++ ) {
      *pword = (char) is.get();

      // Did we reach the end of the word?
   	if ( cEnd == ' ' ? isspace( *pword ) : *pword == cEnd ) {
         // If this wasn't quoted, put back the white space
         if ( cEnd == ' ' )
            is.putback(*pword);

         // This was quoted, so preserve the final quote character
         else
            pword++;

	      *pword = '\0';
   	    return word;
	   }
   }

   // We ran out of buffer space.  Some kind of error has occured.
   SyntaxError(is, operation, "Word too long");
   return 0;
}
#endif

int32 TAreaDocument::Str2Flag(const char *p)
{
   int32 n;

   // Convert ROM-style flags
   for (n = 0; ('A' <= *p && *p <= 'Z') || ('a' <= *p && *p <= 'f'); p++)
      n |= flag_convert(*p);

   // Convert Envy-style flags
   while (*p) {
      int nEnvy = 0;
      for ( ; isdigit(*p); p++) {
	      nEnvy = nEnvy * 10 + *p - '0';
      }

      if (*p == '|' || *p == '\0')
         n |= nEnvy;
      else
         return n;

      if (*p)
         p++;
   }

   return n;
}

void TAreaDocument::Flags2Str(poundsApp::currMode mode, char *p, int32 nFlags)
{
   if (nFlags == 0) {
      *p++ = '0';
   } else {
      if (mode == poundsApp::RomMode || mode == poundsApp::Rom24Mode) {
         static char szFlagTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcde";
         for (int i = 0; i < 32 && nFlags; i++, nFlags >>= 1)
            if (nFlags&1)
               *p++ = szFlagTable[i];
      } else {
         DWORD dw = 1;
         BOOL fFirst = TRUE;
         for (int i = 0; i < 32 && nFlags; i++, nFlags >>= 1, dw <<= 1)
            if (nFlags&1) {
               if (fFirst)
                  fFirst = FALSE;
               else
                  *p++ = '|';
               p+= wsprintf(p, "%lu", dw);
            }
      }
   }

   *p = '\0';
}

// Note that these are the legal ROM positions for an area file,
// not all legal ROM positions.  If you must add positions to this,
// also note that the first position is considered the 'default' if
// MZF encounters a position it doesn't know.
const char *TAreaDocument::aszRomPosition[4] = {
    "stand", "sit", "rest", "sleep"
};

// Note that these are the legal ROM positions for an area file,
// not all legal ROM positions
sh_int TAreaDocument::position_lookup(const char *p)
{
   for (sh_int i = 0; i < nelems(aszRomPosition); i++)
      if (stricmp(p, aszRomPosition[i]) == 0)
         return i;

   return 0;
}

const char *TAreaDocument::position_name(sh_int n)
{
   if (n >= nelems(aszRomPosition))
      return 0;
//      THROW( TXOwl("position_name() parameter error") );

   return aszRomPosition[n];
}

void TAreaDocument::LoadPositionComboBox(TComboBoxData *pCBData, int n)
{
    pCBData->Clear();
    for (int i = 0; i < nelems(aszRomPosition); i++)
        pCBData->AddStringItem(aszRomPosition[i], i, n == i);
}

