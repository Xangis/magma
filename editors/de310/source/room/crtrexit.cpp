//
//  File: crtrexit.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions used to create exits in rooms
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"
#include "../const.h"

#include "exit.h"
#include "room.h"

extern dikuRoom *currentRoom;
extern char madeChanges;
extern _CONST char *exitnames[];

//
// createRoomExitPrompt : user interface for creating an exit in some
//                        user-selected direction - cannot create an exit
//                        where one already exists.  function returns
//                        the 'flag' value of exit created if successful,
//                        NO_EXIT otherwise
//
//        room : room to create exit in
//   exitAvail : bitvector created by getExitAvailFlags() that specifies
//               which exits exist
//

long createRoomExitPrompt(dikuRoom *room, const ulong exitAvail)
{
  char strn[256];
  usint ch;


  strcpy(strn, "\n&+cCreate ");

 // set colors of each choice based on which are available

  if (!(exitAvail & EXIT_NORTH_FLAG))
  {
    strcat(strn, "[&+CN&n&+c]orth, ");
  }
  else strcat(strn, "&+L[N]orth,&n&+c ");

  if (!(exitAvail & EXIT_NORTHWEST_FLAG))
  {
    strcat(strn, "[&+C1&n&+c]Northwest, ");
  }
  else strcat(strn, "&+L[1]Northwest,&n&+c ");

  if (!(exitAvail & EXIT_NORTHEAST_FLAG))
  {
    strcat(strn, "[&+C2&n&+c]Northeast, ");
  }
  else strcat(strn, "&+L[2]Northeast,&n&+c ");

  if (!(exitAvail & EXIT_SOUTH_FLAG))
  {
    strcat(strn, "[&+CS&n&+c]outh, ");
  }
  else strcat(strn, "&+L[S]outh,&n&+c ");

  if (!(exitAvail & EXIT_SOUTHWEST_FLAG))
  {
    strcat(strn, "[&+C3&n&+c]Southwest, ");
  }
  else strcat(strn, "&+L[3]Southwest,&n&+c ");

  if (!(exitAvail & EXIT_SOUTHEAST_FLAG))
  {
    strcat(strn, "[&+C4&n&+c]Southeast, ");
  }
  else strcat(strn, "&+L[4]Southeast,&n&+c ");

  if (!(exitAvail & EXIT_WEST_FLAG))
  {
    strcat(strn, "[&+CW&n&+c]est, ");
  }
  else strcat(strn, "&+L[W]est,&n&+c ");

  if (!(exitAvail & EXIT_EAST_FLAG))
  {
    strcat(strn, "[&+CE&n&+c]ast, ");
  }
  else strcat(strn, "&+L[E]ast,&n&+c ");

  if (!(exitAvail & EXIT_UP_FLAG))
  {
    strcat(strn, "[&+CU&n&+c]p, ");
  }
  else strcat(strn, "&+L[U]p,&n&+c ");

  if (!(exitAvail & EXIT_DOWN_FLAG))
  {
    strcat(strn, "or [&+CD&n&+c]own exit (&+CQ to quit&n&+c)? &n");
  }
  else strcat(strn, "or &+L[D]own &n&+cexit (&+CQ to quit&n&+c)? &n");

  displayColorString(strn);


  do
  {
    ch = toupper(getkey());
  } while (((ch != 'N') || ((exitAvail & EXIT_NORTH_FLAG))) &&
           ((ch != '1') || ((exitAvail & EXIT_NORTHWEST_FLAG))) &&
           ((ch != '2') || ((exitAvail & EXIT_NORTHEAST_FLAG))) &&
           ((ch != 'S') || ((exitAvail & EXIT_SOUTH_FLAG))) &&
           ((ch != '3') || ((exitAvail & EXIT_SOUTHWEST_FLAG))) &&
           ((ch != '4') || ((exitAvail & EXIT_SOUTHEAST_FLAG))) &&
           ((ch != 'W') || ((exitAvail & EXIT_WEST_FLAG))) &&
           ((ch != 'E') || ((exitAvail & EXIT_EAST_FLAG))) &&
           ((ch != 'U') || ((exitAvail & EXIT_UP_FLAG))) &&
           ((ch != 'D') || ((exitAvail & EXIT_DOWN_FLAG))) && (ch != 'Q'));

  switch (ch)
  {
    case 'N' : createExit(&room->exits[NORTH], TRUE);  return EXIT_NORTH_FLAG;
    case '1' : createExit(&room->exits[NORTHWEST], TRUE);  return EXIT_NORTHWEST_FLAG;
    case '2' : createExit(&room->exits[NORTHEAST], TRUE);  return EXIT_NORTHEAST_FLAG;
    case 'S' : createExit(&room->exits[SOUTH], TRUE);  return EXIT_SOUTH_FLAG;
    case '3' : createExit(&room->exits[SOUTHWEST], TRUE);  return EXIT_SOUTHWEST_FLAG;
    case '4' : createExit(&room->exits[SOUTHEAST], TRUE);  return EXIT_SOUTHEAST_FLAG;
    case 'W' : createExit(&room->exits[WEST], TRUE);  return EXIT_WEST_FLAG;
    case 'E' : createExit(&room->exits[EAST], TRUE);  return EXIT_EAST_FLAG;
    case 'U' : createExit(&room->exits[UP], TRUE);  return EXIT_UP_FLAG;
    case 'D' : createExit(&room->exits[DOWN], TRUE);  return EXIT_DOWN_FLAG;
    case 'Q' : _outtext("\n\n");  return NO_EXIT;

    default : _outtext("\n\n"
"ERROR: createRoomExitPrompt() - user input checking incorrect - notify the\n"
"author or sump'n\n\n");
              getkey();

              return NO_EXIT;
  }
}


//
// createRoomExit : User interface to create an exit in a particular room
//                  pointed to by room.  Function only allows user to create
//                  an exit if the exit "slot" is empty and returns NO_EXIT
//                  on failure for whatever reason, a bitvector with exit
//                  info (constructed by getExitAvailFlags()) if exitdir
//                  is equal to USER_CHOICE, or the exit direction if exitdir
//                  specified a certain direction and the creation was
//                  successful
//
//     room : pointer to room where the exit is being created
//  exitDir : direction to create exit in, or a few other special values
//            (defined in room/exit.h)
//

long createRoomExit(dikuRoom *room, const long exitdir)
{
//  usint ch;
  long exitAvail;
  char strn[256], strn2[64];


  if (!room)
  {
    _outtext(
"\n\nWhoopsie - *room pointer passed to createRoomExit() is NULL.\n\n");

    return NO_EXIT;
  }

  if ((exitdir != USER_CHOICE) && ((exitdir < 0) || (exitdir >= NUMB_EXITS)))
  {
    _outtext("\n\nexitdir passed to createRoomExit is out of range\n\n");

    return NO_EXIT;
  }

 // if not user choice, try to create the exit in the direction specified

  if (exitdir != USER_CHOICE)
  {
    if (currentRoom->exits[exitdir])
    {
     // proper grammar!

      if ((exitdir == DOWN) || (exitdir == UP))
      {
        sprintf(strn, "\nExit already exists %s.\n\n",
                exitnames[exitdir]);
      }
      else
      {
        sprintf(strn, "\nExit already exists to the %s.\n\n",
                exitnames[exitdir]);
      }

      _outtext(strn);

      return NO_EXIT;
    }
    else
    {
      createExit(&room->exits[exitdir], TRUE);

      strcpy(strn2, exitnames[exitdir]);
      strn2[0] = toupper(strn2[0]);

      sprintf(strn, "\n%s exit created.\n\n", strn2);
      _outtext(strn);

      madeChanges = TRUE;

      return exitdir;
    }
  }

  exitAvail = getExitAvailFlags(room);


 // all exits taken up - bleh - magic numbers rule

  if (exitAvail == EXIT_ALL_EXITS_FLAG)
  {
    _outtext("\nNo exit slots available - move or delete some exits.\n\n");

    return NO_EXIT;
  }

  return createRoomExitPrompt(room, exitAvail);
}


//
// preCreateExit : parses the args string and passes it to createRoomExit()
//                 if it specifies a valid direction - room pointer specifies
//                 where to create exit
//
//    args : user input
//    room : pointer to room to create exit in
//

void preCreateExit(const char *args, dikuRoom *room)
{
  char arg1[256], val;

  getArg(args, 1, arg1);

  if (!strlen(arg1)) createRoomExit(room, USER_CHOICE);
  else
  {
    val = getDirfromKeyword(arg1);
    if (val == NO_EXIT)
    {
      _outtext("\nCreate which exit?\n\n");
      return;
    }

    createRoomExit(room, val);
  }
}
