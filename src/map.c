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
 *                                                                         *
 ***************************************************************************/

// This file will contain all functions related
// to surface maps
// - Veygoth

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

char lbuf[MAX_STRING_LENGTH];
/* The form of the EXTENDED_TERRAIN struct
struct extended_terrain
{
        int  extended_sector;
        char *room_title;
        char *room_description;
        int  main_sector;      // Actual base terrain type (not required)
        char *map_char;        // Ansi displayed for that sector
        int visibility;        // Standard range of squares a player can see (radius)
};
*/

const EXTENDED_TERRAIN map_info[MAX_EXTENDED_TERRAIN] =
{
 {0, "Unrecognized Sector Type", "No description available", 0, "?", 3 },
 {1, "&+BThe Ocean of the &n&+cF&+Cr&+Wo&+Cs&n&+ct &+CGoddess&n", "", 0, "&-B &n", 4 },
 {2, "&+yThe Barrier Cliffs&n", "", 0, "&+yM&n", 2 },
 {3, "&+WAn Icy Beach&n", "", 0, "&+W.&n", 2 },
 {4, "&+WSome Thin&n &+CIce&n &+WOver Lake Turol&n", "", 0, "&+ww&n", 2 },
 {5, "&+gThe Wetlands&n", "", 0, "&+m!&n", 2 },
 {6, "&+WWhite Sands Beach&n", "", 0, "&+Y.&n", 2 }, 
 {7, "&+yThe Great Mountain Range&n", "", 0, "&+yM&n", 2 },
 {8, "&+gThe Rolling&n &+GHighlands&n", "", 0, "&+y^&n", 2 },
 {9, "&+GGrassy Knolls&n", "", 0, "&+g.&n", 2 },
 {10, "&+WThe Great White Cliffs&n", "", 0, "&+y~&n", 2 },
 {11, "&+LBlack Cliffs&n&+w of &+LAsh&n", "", 0, "&+L~&n", 2 },
 {12, "&+LR&n&+wock&+Ly&n&+w Plains of &+LAsh&n", "", 0, "&+L.&n", 2 },
 {13, "&+YA Bleak Desert&n", "", 0, "&+y.&n", 2},
 {14, "&+LA&n&+y Rocky &+LIsland&n", "", 0, "&+y.&n", 2 },
 {15, "&+yAn Earthen Highway&n", "", 0, "&+y+&n", 2 },
 {16, "&+cThe Casparian River&n", "", 0, "&+br&n", 3 },
 {17, "&+cThe River of the Phoenix&n", "", 0, "&+Br&n", 3 },
 {18, "&+WA Dense Birch-Tree Forest&N", "", 0, "&+g*&n", 1 },
 {19, "&+gA Cedar Forest&n", "", 0, "&+g*&n", 1 },
 {20, "&+yA Broken, Forgotten Highway&n", "", 0, "&+L+&n", 2 },
 {21, "&+gThe &+GUpper &+BWetlands&n", "", 0, "&+m!&n", 2 },
 {22, "&+BA Straits of Quietus&n", "", 0, "&+b#&n", 3 },
 {23, "&+yThe Phiadnae Flats&n", "", 0, "&+y.&n", 2 },
 {24, "&+yA Dusty Path&n", "", 0, "&+y+&n", 2 },
 {25, "&+BThe Gulf of Phoenix&n", "", 0, "&+b#&n", 3 },
 {26, "&+gThe &+GGreat Forest&n&+g of the &+RPhoenix&n", "", 0, "&+G*&n", 1 },
 {27, "&+wAn &+WIcy &n&+wBeach&n", "", 0, "&+W.&n", 2 },
 {28, "&+WThe Vast Plains of the Snow Goddess&n", "", 0, "&+w~&n", 2 },
 {29, "&+rThe Vulcan Mountains&n", "", 0, "&+LM&n", 2 },
 {30, "&+gThe &+GJade &n&+gContinent&n", "", 0, "&+g.&n", 2 },
 {31, "&+gA Dense Dogwood Forest&n", "", 0, "&+G*&n", 2 },
 {32, "&+WA Massive Floating Icecap&n", "", 0, "&+W#&n", 3 },
 {33, "&+gA Strange, Dark Forest&n", "", 0, "&+g*&n", 1 },
 {34, "&+LThe Obsidian Plateau&n", "", 0, "&+L~&n", 2 },
 {35, "&+yThe Mountains of &+rDu'Maathe&n", "", 0, "&+yM&n", 2 },
 {36, "&+gThe &+cSea-&+gTowering &+yCliffs&n", "", 0, "&+y~&n", 2 },
 {37, "&+wA &+LBlack Beach&n&+w of&+L Ash&n", "", 0, "&+L.&n", 2 },
 {38, "&+yA Rocky Beach&n", "", 0, "&+y.&n", 2 },
 {39, "&+wA Common Merchant Dock&n", "", 0, "&+y.&n", 2 },
 {40, "&+yThe &+RRed-Sands&n&+y Beach&n", "", 0, "&+r.&n", 2 },
 {41, "&+GThe Port of the Empress&n", "", 0, "&+yo&n", 2 },
 {42, "&+CAn Icy Dock&n", "", 0, "&+yo&n", 2 },
 {43, "&+LA Section of Scorched Earth&n", "", 0, "&+Lx&n", 2 },
 {44, "&+yThe Buffalo Hills&n", "", 0, "&+y^&n", 2 },
 {45, "Zone Marker", "", 0, "&+R^&n", 2 },
 {46, "&+yThe &+rDusk&+y Mountains&n", "", 0, "&+yM&n", 2 },
 {47, "&+yThe Highway of the Empress&n", "", 0, "&+L+&n", 2 },
 {48, "&+ySome Rolling Sand Dunes&n", "", 0, "&+Y^&n", 2 },
 {49, "&+BThe Grasping Sea&n", "", 0, "&-b &n", 4},
 {50, "&+yA &+LWorn &n&+yTrade Path&n", "", 0, "&+y+&n", 2},
 {51, "&+GThe Rift Valley Jungle&n", "", 0, "&+G*&n", 2 },
 {52, "&+LJakar&n&+y Mountains&n", "", 0, "&+yM&n", 2 },
 {53, "&+yDirt&n&+cwater &+ySwamps&n", "", 0, "&+c!&n", 2 },
 {54, "&+GA &+YTropical &+GIsland&n", "", 0, "&+g.&n", 2 },
 {55, "&+gThe &+GCove&n&+g of the M&n&+ga&+Gn&n&+gt&+Gi&n&+gs&n", "", 0, "&+B.&n", 3 },
 {56, "&+cA Shallow Harbor&n", "", 0, "&+yo&n", 3 },
 {57, "&+gA Densely Wooded Island&n", "", 0, "&+g.&n", 2 },
 {58, "&+gA &+GClearing&n&+g in the Forest&n", "", 0, "&+g.&n", 2 },
 {59, "&+gA Secluded &+GTropical Island&n", "", 0, "&+g.&n", 2 },
 {60, "&+gA &+GClearing&n&+gin the &+YTropical &+GIslands&n", "", 0, "&+B.&n", 3 },
 {61, "&+gThe &+GIsland&n&+g of &+BSarythi&n", "", 0, "&+c.&n", 2 },
 {62, "&+cThe Cove of &+BDictynna&n", "", 0, "&+B.&n", 2 },
 {63, "&+gThe &+cShadowglade&n&+g Continent&n", "", 0, "&+g.&n", 2 },
 {64, "&+gThe &+cCove&+g of the &+cMu&+Cse&n&+cs&n", "", 0, "&+B*&n", 2 },
 {65, "&+yA Well-Travelled Path&n", "", 0, "&+y+&n", 2 },
 {66, "&+gThe Lower &n&+cShadowglade &+GPeninsula&n", "", 0, "&+g.&n", 2 },
 {67, "&+gThe &+cPeninsula &+CStrait&n", "", 0, "&+y.&n", 2 },
 {68, "&+gA Small &+GForest&n&+g Island&n", "", 0, "&+g.&n", 2 },
 {69, "&+gThe &+cBr&+Coo&n&+ck&n&+g of the &+mMuse&n", "", 0, "&-b&+cr&n", 3 },
 {70, "&+gThe &+GHighland&n &+gForests&n", "", 0, "&+g.&n", 2 },
 {71, "&+gThe Lower &+yPlains&n", "", 0, "&+y'&n", 2 },
 {72, "&+gThe &+yPrairie&n&+g Flatlands&n", "", 0, "&+y.&n", 2 },
 {73, "&+WThe Barrier Reefs&n", "", 0, "&+W=&n", 3 },
 {74, "&+gA Large &+yPrairie&n&+g Island&n", "", 0, "&+y.&n", 2 },
 {75, "&+BA Small Pond&n", "", 0, "&+cw&n", 3 },
 {76, "&+cThe Kruthil River&n", "", 0, "&+Br&n", 3 },
 {77, "&+cThe Hrurabi River&n", "", 0, "&+Br&n", 3 },
 {78, "&+gAn &n&+yOld Growth&+g Pine Forest&n", "", 0, "&+g*&n", 2 },
 {79, "&+gSome Shallow &+yMarshes&n", "", 0, "&+y!&n", 2 },
 {80, "&+yA Small Mountain Island&n", "", 0, "&+yM&n", 2 },
 {81, "&+LA Muddy Docking Port&n", "", 0, "&+yo&n", 2 },
 {82, "&+gThe &+GHighlands&n", "", 0, "&+g'&n", 2 },
 {83, "&+yA Well Travelled Dusty Road&N", "", 0, "&+y+&n", 2 },
 {84, "&+yA Forgotten Path&n", "", 0, "&+y+&n", 2 },
 {85, "&+yAn Overgrown Path&n", "", 0, "&+g+&n", 2 },
 {86, "&+GA Light Forest&n", "", 0, "&+g.&n", 2 },
 {87, "&+gThe Forest of &+GIndica&n", "", 0, "&+g.&n", 2 },
 {88, "&+cA &+CShallow&n&+c Cove&n", "", 0, "&+co&n", 3 },
 {89, "&+gA &n&+LShady&n&+g Wooded Island&n", "", 0, "&+g.&n", 2 },
 {90, "&+yA Dilapidated Dock&n", "", 0, "&+yo&n", 2 },
 {91, "&+gThe &+GForest&n&+g of &+LAutumn&n&+gglen&n", "", 0, "&+g*&n", 2 },
 {92, "&+LThick Volcanic Rock&n", "", 0, "&+L &n", 2 },
 {93, "&+LA Cavern of &+rVolcanic &n&+wAshes&n", "", 0, "&+L.&n", 2 },
 {94, "&+LA Narrow Dormant Volcanic Tunnel&n", "", 0, "&+L+&n", 2 },
 {95, "&+rThe Edge of a Volcanic Cavern&n", "", 0, "&+L*&n", 2 },
 {96, "&+RAn Active Volcano Pit&n", "", 0, "&+R*&n", 2 },
 {97, "&+RA Tunnel of Flowing Lava&n", "", 0, "&+r~&n", 2 },
 {98, "&+RA Sultry Cavern&n", "", 0, "&+m.&n", 2 },
 {99, "&+rA Humid Cave&n", "", 0, "&+r.&n", 2 },
 {100, "&+mA Large Cavern in the Underdark&n", "", 0, "&-m&+l.&n", 2 },
 {101, "&+wA Massive Stalagmite&n", "", 0, "&+y)&n", 2 },
 {102, "&+yA Massive Stalactite&n", "", 0, "&+y(&n", 2 },
 {103, "&+cThe Underground River of Mirkweyd&n", "", 0, "&+br&n", 2 },
 {104, "&+bA Natural Underground Spring&n", "", 0, "&+b@&n", 2 },
 {105, "&+cA &+CRapid&n&+c Section of the River&n", "", 0, "&+Br&n", 2 },
 {106, "&+cThe Mouth of the Mirkweyd River&n", "", 0, "&+br&n", 2 },
 {107, "&+yA Ledge Near the River&n", "", 0, "&+y.&n", 2 },
 {108, "&+bThe Sunless Sea&n", "", 0, "&-b&+cw&n", 2 },
 {109, "&+gA Damp Underdark Cavern&n", "", 0, "&+B.&n", 2 },
 {110, "&+BA Shallow Cove of the Sunless Sea&n", "", 0, "&+cc&n", 2 },
 {111, "&+mA Dark Underdark Cave&n", "", 0, "&+L.&n", 2 },
 {112, "&+LA &n&+gSmall Island&+L in the &n&+bSunless Sea&n", "", 0, "&+g.&n", 2 },
 {113, "&+gAn Island of Jagged Green Rock&n", "", 0, "&+gm&n", 2 },
 {114, "&+mA Strange Mushroom Grove&n", "", 0, "&+M*&n", 2 },
 {115, "&+LAn Island of Basalt Rock&n", "", 0, "&+L#&n", 2 },
 {116, "&+LSolid Rock&n", "", 0, "&+L &N", 2 },
 {117, "&+LA Dark Underdark Cave&n", "", 0, "&+w.", 2 },
 {118, "&+LThe &n&+yRatling &+LCaverns&n", "", 0, "&+L*&n", 2 },
 {119, "&+GS&n&+gl&+Gi&n&+gm&+Ge&n&+g-Covered Passages&n", "", 0, "&+g+&n", 2 },
 {120, "&+mA Massive Underdark Cavern&n", "", 0, "&+m.&n", 2 },
 {121, "&+wA Thin &+WM&n&+wa&+Lr&n&+wb&+Wl&n&+we Passage&N", "", 0, "&+w+&n", 2 },
 {122, "&+cThe Sunless River&n", "", 0, "&+br&n", 3},
 {123, "&+BA Steep Waterfall&", "", 0, "&+B@&n", 3},
 {124, "&+gA Slimy Cavern&n", "", 0, "&+g.&n", 2},
 {125, "&+yThe Lesser Crescent Chasm&n", "", 0, "&+y.&n", 2},
 {126, "&+LA Desolate Cavern&n", "", 0, "&-l&+b.&n", 2},
 {127, "&+yThe Greater Crescent Chasm&n", "", 0, "&+y.&n", 2},
 {128, "&+wThe Si&+Wlv&n&+wer&+c Lake &+wof the &+WPhoenix&n", "", 0, "&+Ww", 3},
 {129, "&+yA Cavern Ledge Along the Lake&n", "", 0, "&+y.&n", 2},
 {130, "&+LA Massive Stalagmite&n", "", 0, "&+y)&n", 2},
 {131, "&+LA Massive Stalactite&n", "", 0, "&+y(&n", 2},
 {132, "&+LThick Basalt Rock&n", "", 0, "&+L &n", 2},
 {133, "&+mThe Caverns of L'ristrizzen&N", "", 0, "&-m&+l.&n", 2},
 {134, "&+LThe Caves of the &+WS&N&+wp&+Li&N&+wd&+We&N&+wr&n", "", 0, "&+L+&n", 2},
 {135, "&+mA Massive Cavern within the Deep Underdark&N", "", 0, "&+m.&n", 2},
 {136, "&+yA Massive Stalagmite&N", "", 0, "&+y(&n", 2},
 {137, "&+yA Massive Stalactite&N", "", 0, "&+y)&n", 2},
 {138, "&+cA Clausterphobic Cavern&N", "", 0, "&+c.&n", 2},
 {139, "&+LA Dead End in the Caves&N", "", 0, "&+L+&n", 2},
 {140, "&+LA Cavern in the Deep Underdark&n", "", 0, "&-m&+b.&n", 2},
 {141, "&+GThe Cavern of Malch'hor Ganl&N", "", 0, "&-m&+g*&n", 2},
 {142, "&+bAn Underdark River&N", "", 0, "&+br&n", 2},
 {143, "&+BThe Base of a Waterfall&N", "", 0, "&+B@&n", 2},
 {144, "&+BThe Top of a Waterfall&N", "", 0, "&+B@&n", 2},
 {145, "&+BA &n&+bDeep Underdark &+BLake&N", "", 0, "&-B &n", 2},
 {146, "&+yA Steep Incline&N", "", 0, "&+y@&n", 2},
 {147, "&+yThe Top of a Steep Cliff&N", "", 0, "&+y@&n", 2},
 {148, "&+LA Narrow, Dark Cave&N", "", 0, "&N&+r+&n", 2},
 {149, "&+rThe Caverns of Everhate&N", "", 0, "&N&+r#&n", 2},
 {150, "&+gA Slimy Tunnel&N", "", 0, "&+G+&n", 2},
 {151, "&+mThe Cavern of Caranthazal&N", "", 0, "&+M.&n", 2},
 {152, "&+bThe Sunless Lake&N", "", 0, "&-b&+cw&n", 2},
 {153, "&+BA Thin Watery Cave&N", "", 0, "&+c+&n", 2},
 {154, "&+LThe Clawed Caverns&n", "", 0, "&-m&+g.&n", 2},
};

void do_worldmap( CHAR_DATA *ch, char *argument )
{
   int visibility;
   int vnum;
   int x;
   int y;
   int column;
   int row;
   int width;
   int height;
   int startvnum;
   char buf[MAX_STRING_LENGTH * 2];
#ifdef HARDCORE_DEBUG
   char debug[MAX_STRING_LENGTH];
#endif
   ROOM_INDEX_DATA *room;
   char boundchar[8];
    int  has_p = FALSE;
    int has_m = FALSE;
    CHAR_DATA *pch;
    int racewarside;

   if( ch->deleted || !ch->in_room )
     return;
   //send_to_char("\n\rAttempting to draw world map.\n\r", ch);

   racewarside = race_table[ch->race].racewar_side;
   visibility = 0;

   if( ch->in_room->vnum >= SURFACE_MAP1_START_VNUM && ch->in_room->vnum <= SURFACE_MAP1_END_VNUM )
   {
     switch( weather_info.sunlight )
     {
       case SUN_DARK:
       case MOON_SET:
       case MOON_RISE:
           if( racewarside == RACEWAR_EVIL )
               visibility = 4;
           else if( racewarside == RACEWAR_GOOD )
               visibility = 0;
           else
               visibility = 2;
           break;
       case SUN_SET:
       case SUN_RISE:
           if( racewarside == RACEWAR_EVIL )
               visibility = 1;
           else if( racewarside == RACEWAR_GOOD )
               visibility = 2;
           else
               visibility = 2;
           break;
       case SUN_LIGHT:
           if( racewarside == RACEWAR_EVIL )
               visibility = 0;
           else if( racewarside == RACEWAR_GOOD )
               visibility = 4;
           else
               visibility = 2;
           break;
       default:
           visibility = 0;
           break;
     }
     startvnum = SURFACE_MAP1_START_VNUM;
     width = SURFACE_MAP1_WIDTH;
     height = SURFACE_MAP1_HEIGHT;
     sprintf( boundchar, "&-B &n" );
   }
   else if( ch->in_room->vnum >= SURFACE_MAP2_START_VNUM && ch->in_room->vnum <= SURFACE_MAP2_END_VNUM )
   {
     switch( weather_info.sunlight )
     {
       case SUN_DARK:
       case MOON_SET:
       case MOON_RISE:
           if( racewarside == RACEWAR_EVIL )
               visibility = 4;
           else if( racewarside == RACEWAR_GOOD )
               visibility = 0;
           else
               visibility = 2;
           break;
       case SUN_SET:
       case SUN_RISE:
           if( racewarside == RACEWAR_EVIL )
               visibility = 1;
           else if( racewarside == RACEWAR_GOOD )
               visibility = 2;
           else
               visibility = 2;
           break;
       case SUN_LIGHT:
           if( racewarside == RACEWAR_EVIL )
               visibility = 0;
           else if( racewarside == RACEWAR_GOOD )
               visibility = 4;
           else
               visibility = 2;
           break;
       default:
           visibility = 0;
           break;
     }
     startvnum = SURFACE_MAP2_START_VNUM;
     width = SURFACE_MAP2_WIDTH;
     height = SURFACE_MAP2_HEIGHT;
     sprintf( boundchar, "&-B &n" );
   }
   else if( ch->in_room->vnum >= UD_MAP1_START_VNUM && ch->in_room->vnum <= UD_MAP1_END_VNUM )
   {
     switch( racewarside )
     {
       case RACEWAR_EVIL:
           visibility = 4; break;
       case RACEWAR_GOOD:
           visibility = 0; break;
       default:
           visibility = 2; break;
     }
     startvnum = UD_MAP1_START_VNUM;
     width = UD_MAP1_WIDTH;
     height = UD_MAP1_HEIGHT;
     sprintf( boundchar, "&+L &n" );
   }
   else if( ch->in_room->vnum >= UD_MAP2_START_VNUM && ch->in_room->vnum <= UD_MAP2_END_VNUM )
   {
     switch( racewarside )
     {
       case RACEWAR_EVIL:
           visibility = 4; break;
       case RACEWAR_GOOD:
           visibility = 0; break;
       default:
           visibility = 2; break;
     }
     startvnum = UD_MAP2_START_VNUM;
     width = UD_MAP2_WIDTH;
     height = UD_MAP2_HEIGHT;
     sprintf( boundchar, "&+L &n" );
   }
   else if( ch->in_room->vnum >= UD_MAP3_START_VNUM && ch->in_room->vnum <= UD_MAP3_END_VNUM )
   {
     switch( racewarside )
     {
       case RACEWAR_EVIL:
           visibility = 4; break;
       case RACEWAR_GOOD:
           visibility = 0; break;
       default:
           visibility = 2; break;
     }
     startvnum = UD_MAP3_START_VNUM;
     width = UD_MAP3_WIDTH;
     height = UD_MAP3_HEIGHT;
     sprintf( boundchar, "&+L &n" );
   }
   else
     return;

   sprintf( buf, "\n\r\n\r" );

   visibility += map_info[ch->in_room->extra_sector_info].visibility + ch->fly_level;

   // Gods with holylight on will see farther on the map.
   if( !IS_NPC( ch ) && IS_SET( ch->act, PLR_HOLYLIGHT ))
     visibility += 5;

   vnum = ch->in_room->vnum;

   column = (vnum - startvnum) % width;
   row = (vnum - startvnum) / width;
   if (visibility <= 0 && !is_affected( ch, AFF_BLIND ) )
       visibility = 1;
   for( x = (0 - visibility); x <= visibility; x++ )
   {
     strcat( buf, "     " );
     for( y = (0 - visibility); y <= visibility; y++ )
     {
        has_m = FALSE;
        has_p = FALSE;
        room = get_room_index( (vnum + ((x * width) + y)));
        if ( room && room->people ) {
          for( pch = room->people; pch; pch = pch->next_in_room) {
              if ( ( pch->fly_level == ch->fly_level ||
                 ch->fly_level == pch->fly_level + 1)  && can_see( ch, pch ) ) {
                if ( IS_NPC( pch ) ) {
                  if (pch->fly_level == ch->fly_level)
                    has_m = 1;
                  else
                    has_m = 2;
                  }
                else  {
                  if (pch->fly_level == ch->fly_level)
                    has_p = 1;
                  else
                    has_p = 2;
                  }
              }
          }
        }
        if( x == 0 && y == 0 )
          strcat( buf, "&+W@&n" );
        else if( (column + y) < 0 || (column + y) >= width ||
                 (row + x) < 0 || (row + x ) >= height ) {
          strcat( buf, boundchar );
        }
        else if( !( room = get_room_index( (vnum + ((x * width) + y))))) {
          strcat( buf, "&+L-&n" );
        }
        else if( has_m ) {
          if ( has_m == 1 )
            strcat( buf, MOD_BLINK "&+BM&n" MOD_CLEAR );
          else
            strcat( buf, MOD_BLINK "&+bM&n" MOD_CLEAR );
        }
        else if ( has_p ) {
          if ( has_p == 1 )
            strcat( buf, MOD_BLINK "&+WP&n" MOD_CLEAR );
          else
            strcat( buf, MOD_BLINK "&+YP&n" MOD_CLEAR );
        }
        else
        {
	  if( room->extra_sector_info
	    && room->extra_sector_info < MAX_EXTENDED_TERRAIN 
	    && room->extra_sector_info >= 0 )
	  {
#ifdef HARDCORE_DEBUG
sprintf( debug, "Point A in: Room: %d, extra_s_info: %d", room->vnum,
	room->extra_sector_info );
log_string( debug );
#endif
            strcat( buf, map_info[room->extra_sector_info].map_char );
            sprintf(lbuf, "Worldmap: room %d, ESI: %d, MC: %s",
              room->vnum, room->extra_sector_info, map_info[room->extra_sector_info].map_char);
//            log_string( lbuf );
	  }
	  else
            strcat( buf, boundchar );
	}
     }
     strcat( buf, "\n\r" );
  /* Added these two lines to make it smaller string size in send_to_chr,
   * so that the pager isnt needed when large maps fill the screen
   *
   * Lithos
   */
   send_to_char( buf, ch );
    buf[0] = '\0';
   }
   strcat( buf, "\n\r" );

   send_to_char( buf, ch );

   return;
}

void do_map( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH * 6];
    int w_scale, h_scale, width, height;
    int i, j, room, start;
    ROOM_INDEX_DATA *proom;

    last_fun( "do_map" );
    if ( !ch->in_room && ch->in_room->vnum < SURFACE_MAP1_START_VNUM ) {
        send_to_char( "You can only map when you are _on_ a map.\n\r", ch);
        return;
    }
    room = ch->in_room->vnum;
    if ( room >= SURFACE_MAP1_START_VNUM && room <= SURFACE_MAP1_END_VNUM ) {
        start = SURFACE_MAP1_START_VNUM;
        width = 330;
        height = 158;
        w_scale = 5;
    }
    else if ( room >= SURFACE_MAP2_START_VNUM && room <= SURFACE_MAP2_END_VNUM ) {
        start = SURFACE_MAP2_START_VNUM;
        width = 129;
        height = 89;
        w_scale = 2;
    }
    else if ( room >= UD_MAP1_START_VNUM && room <= UD_MAP1_END_VNUM ) {
        start = UD_MAP1_START_VNUM;
        width = 97;
        height = 112;
        w_scale = 2;
    }
    else if ( room >= UD_MAP2_START_VNUM && room <= UD_MAP2_END_VNUM ) {
        start = UD_MAP2_START_VNUM;
        width = 97;
        height = 57;
        w_scale = 2;    
    }    
    else if ( room >= UD_MAP3_START_VNUM && room <= UD_MAP3_END_VNUM ) {    
        start = UD_MAP3_START_VNUM;
        width = 142;
        height = 55;
        w_scale = 2;
    }        
    else
        return;
    h_scale = w_scale;
    for ( j= 0; j <= height; j+= h_scale ) {
        buf[0] = '\0';
        for ( i = 0; i <= width; i+= w_scale ) {
            room = (j*width) + i + start;
            proom = get_room_index( room );
            if( proom
              && proom->extra_sector_info
              && proom->extra_sector_info < MAX_EXTENDED_TERRAIN
              && proom->extra_sector_info >= 0 )
                strcat( buf, map_info[proom->extra_sector_info].map_char );
        }
        strcat( buf, "\n\r" );
        send_to_char( buf, ch );
    } //end for
    send_to_char( "\n\r", ch );
    return;
    
}

