//
//  File: delexitu.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for deleting exits
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"
#include "../const.h"

#include "room.h"
#include "exit.h"

extern char madeChanges;
extern _CONST char *exitnames[];

//
// deleteExitPromptPrompt : deletes an exit from a specified room, based
//                          on user input and exit info in exitAvail bitvect
//
//       room : room to delete exit from
//  exitAvail : specifies which exits exist in room
//

void deleteExitPromptPrompt(dikuRoom *room, const ulong exitAvail)
{
  char strn[256];
  usint ch;


  strcpy(strn, "\n&+cDelete ");

 // set colors of each choice based on which are available

  if (exitAvail & EXIT_NORTH_FLAG)
  {
    strcat(strn, "[&+CN&n&+c]orth, ");
  }
  else strcat(strn, "&+L[N]orth,&n&+c ");

  if ((exitAvail & EXIT_NORTHWEST_FLAG))
  {
    strcat(strn, "[&+C1&n&+c]Northwest, ");
  }
  else strcat(strn, "&+L[1]Northwest,&n&+c ");

  if ((exitAvail & EXIT_NORTHEAST_FLAG))
  {
    strcat(strn, "[&+C2&n&+c]Northeast, ");
  }
  else strcat(strn, "&+L[2]Northeast,&n&+c ");

  if ((exitAvail & EXIT_SOUTH_FLAG))
  {
    strcat(strn, "[&+CS&n&+c]outh, ");
  }
  else strcat(strn, "&+L[S]outh,&n&+c ");

  if ((exitAvail & EXIT_SOUTHWEST_FLAG))
  {
    strcat(strn, "[&+C3&n&+c]Southwest, ");
  }
  else strcat(strn, "&+L[3]Southwest,&n&+c ");

  if ((exitAvail & EXIT_SOUTHEAST_FLAG))
  {
    strcat(strn, "[&+C4&n&+c]Southeast, ");
  }
  else strcat(strn, "&+L[4]Southeast,&n&+c ");

  if ((exitAvail & EXIT_WEST_FLAG))
  {
    strcat(strn, "[&+CW&n&+c]est, ");
  }
  else strcat(strn, "&+L[W]est,&n&+c ");

  if ((exitAvail & EXIT_EAST_FLAG))
  {
    strcat(strn, "[&+CE&n&+c]ast, ");
  }
  else strcat(strn, "&+L[E]ast,&n&+c ");

  if ((exitAvail & EXIT_UP_FLAG))
  {
    strcat(strn, "[&+CU&n&+c]p, ");
  }
  else strcat(strn, "&+L[U]p,&n&+c ");

  if ((exitAvail & EXIT_DOWN_FLAG))
  {
    strcat(strn, "or [&+CD&n&+c]own exit (&+CQ to quit&n&+c)? &n");
  }
  else strcat(strn, "or &+L[D]own &n&+cexit (&+CQ to quit&n&+c)? &n");

  displayColorString(strn);


  do
  {
    ch = toupper(getkey());
  } while (((ch != 'N') || (!(exitAvail & EXIT_NORTH_FLAG))) &&
           ((ch != '1') || (!(exitAvail & EXIT_NORTHWEST_FLAG))) &&
           ((ch != '2') || (!(exitAvail & EXIT_NORTHEAST_FLAG))) &&
           ((ch != 'S') || (!(exitAvail & EXIT_SOUTH_FLAG))) &&
           ((ch != '3') || (!(exitAvail & EXIT_SOUTHWEST_FLAG))) &&
           ((ch != '4') || (!(exitAvail & EXIT_SOUTHEAST_FLAG))) &&
           ((ch != 'W') || (!(exitAvail & EXIT_WEST_FLAG))) &&
           ((ch != 'E') || (!(exitAvail & EXIT_EAST_FLAG))) &&
           ((ch != 'U') || (!(exitAvail & EXIT_UP_FLAG))) &&
           ((ch != 'D') || (!(exitAvail & EXIT_DOWN_FLAG))) && (ch != 'Q'));

  switch (ch)
  {
    case 'N' : deleteRoomExit(room->exits[NORTH], TRUE);
               room->exits[NORTH] = NULL;
               break;
    case '1' : deleteRoomExit(room->exits[NORTHWEST], TRUE);
               room->exits[NORTHWEST] = NULL;
               break;
    case '2' : deleteRoomExit(room->exits[NORTHEAST], TRUE);
               room->exits[NORTHEAST] = NULL;
               break;
    case 'S' : deleteRoomExit(room->exits[SOUTH], TRUE);
               room->exits[SOUTH] = NULL;
               break;
    case '3' : deleteRoomExit(room->exits[SOUTHWEST], TRUE);
               room->exits[SOUTHWEST] = NULL;
               break;
    case '4' : deleteRoomExit(room->exits[SOUTHEAST], TRUE);
               room->exits[SOUTHEAST] = NULL;
               break;
    case 'W' : deleteRoomExit(room->exits[WEST], TRUE);
               room->exits[WEST] = NULL;
               break;
    case 'E' : deleteRoomExit(room->exits[EAST], TRUE);
               room->exits[EAST] = NULL;
               break;
    case 'U' : deleteRoomExit(room->exits[UP], TRUE);
               room->exits[UP] = NULL;
               break;
    case 'D' : deleteRoomExit(room->exits[DOWN], TRUE);
               room->exits[DOWN] = NULL;
               break;
    case 'Q' : break;

    default : _outtext("\n\nERROR: deleteExit() - user input checking"
                       " incorrect - notify the author or sump'n\n\n");

              return;
  }

  if (ch != 'Q') madeChanges = TRUE;
}


//
// deleteExitPrompt : User interface to delete an exit from room - this
//                    function deletes exit specified if dir isn't USER_CHOICE
//                    and calls deleteExitPromptPrompt() otherwise
//
//       room : room deleting exit from
//        dir : direction to delete or USER_CHOICE
//

void deleteExitPrompt(dikuRoom *room, const char dir)
{
  ulong exitAvail;
  char strn[256], outstrn[256];


  if (dir != USER_CHOICE)
  {
    if (room->exits[dir])
    {
      deleteRoomExit(room->exits[dir], TRUE);
      room->exits[dir] = NULL;

      strcpy(strn, exitnames[dir]);
      strn[0] = toupper(strn[0]);

      sprintf(outstrn, "\n%s exit deleted.\n\n", strn);
      _outtext(outstrn);

      madeChanges = TRUE;
    }
    else _outtext("\nNo exit in that direction.\n\n");

    return;
  }

  exitAvail = getExitAvailFlags(room);

 // all exits taken up

  if (exitAvail == 0)
  {
    _outtext("\nThis room has no exits.\n\n");

    return;
  }

  deleteExitPromptPrompt(room, exitAvail);

  _outtext("\n\n");
}


//
// preDeleteExitPrompt : parses user input for deleteExitPrompt() - if
//                       not a valid exit direction, returns
//
//      args : user input
//      room : room to delete exit from
//

void preDeleteExitPrompt(const char *args, dikuRoom *room)
{
  char val;


  if (!strlen(args)) deleteExitPrompt(room, USER_CHOICE);
  else
  {
    val = getDirfromKeyword(args);
    if (val == NO_EXIT)
    {
      _outtext("\nDelete which exit?\n\n");
      return;
    }

    deleteExitPrompt(room, val);
  }
}
