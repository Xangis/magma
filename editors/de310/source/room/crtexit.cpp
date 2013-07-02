//
//  File: crtexit.cpp    originally part of dikuEdit
//
//  Usage: functions used to create exit records
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <iostream>

#include "../types.h"
#include "../fh.h"

#include "exit.h"
#include "../misc/strnnode.h"

using namespace std;

extern roomExit *defaultExit;
extern ulong numbExits;
extern char madeChanges;

//
// createExit : Function to create a new exit
//
//   **exitNode : pointer to pointer to exit to create
//

char createExit(roomExit **exitNode, const char incNumbExits)
{
  roomExit *newExit;


 // create a new exit

  newExit = new roomExit;
  if (!newExit)
  {
    cout << "\n\nError creating new roomExit in createExit().  "
            "(memory problem)  Aborting.\n\n";

    return TRUE;
  }

  if (defaultExit)
  {
    newExit = copyRoomExit(defaultExit, FALSE);
  }
  else
  {
    memset(newExit, 0, sizeof(roomExit));
    newExit->keywordListHead = createKeywordList("~");

    newExit->destRoom = -1;
  }


  if (incNumbExits) numbExits++;
  createPrompt();

  *exitNode = newExit;

  madeChanges = TRUE;

  return FALSE;
}


//
// createExit : returns TRUE if error, FALSE otherwise
//

char createExit(const ulong roomNumb, const ulong destNumb, const char exitDir)
{
  roomExit *exitRec;
  dikuRoom *srcRoom = findRoom(roomNumb);


  if (!srcRoom)
  {
    _outtext("\ncreateExit(): couldn't find srcRoom\n\n");
    return TRUE;
  }

  if ((exitDir < 0) || (exitDir >= NUMB_EXITS))
  {
    _outtext("\ncreateExit(): error in exitDir\n\n");
    return TRUE;
  }

  createExit(&exitRec, TRUE);
  if (!exitRec) return TRUE;

  exitRec->destRoom = destNumb;

  srcRoom->exits[exitDir] = exitRec;

  return FALSE;
}
