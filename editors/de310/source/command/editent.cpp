//
//  File: editent.cpp    originally part of dikuEdit
//
//  Usage: handling for entire 'edit' syntax
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>

#include "../types.h"
#include "../fh.h"

#include "../room/room.h"
#include "../misc/master.h"

extern command *editCmdHead;
extern editableListNode *inventory;

//
// editEntity : Edits something - first, runs through the arg string looking
//              for a keyword match with something in the room pointed to by
//              *room, then looks for other keywords
//

void editEntity(const char *args, dikuRoom *room)
{
  editableListNode *editNode;
  char whatMatched, strn[256], grabbing;


  if (strlen(args) == 0)
  {
    _outtext("\nThe \"edit\" command requires at least one argument.\n\n");
    return;
  }

 // check the editable entity list for a match

  grabbing = checkEditableList(args, room->editableListHead,
                               &whatMatched, &editNode, 1);

 // check inventory if room search failed

  if (whatMatched == NO_MATCH)
  {
    checkEditableList(args, inventory, &whatMatched, &editNode, grabbing);
  }

  switch (whatMatched)
  {
    case NO_MATCH       : break;

    case ENTITY_OBJECT  :
           editObjType(((objectHere *)(editNode->entityPtr))->objectPtr, TRUE);
//           updateAllMandElists();  // already done inside editObjType()
           return;

    case ENTITY_OBJ_ED  :
           if (editExtraDesc((extraDesc *)(editNode->entityPtr)))
           {
            // update all masterKeyword and editable lists

//             updateAllMandElists(TRUE, FALSE);  // update if room has objs
             updateAllObjMandElists();
           }

           return;

    case ENTITY_R_EDESC :
           if (editExtraDesc((extraDesc *)(editNode->entityPtr)))
           {
             deleteMasterKeywordList(room->masterListHead);
             room->masterListHead = createMasterKeywordList(room);

             deleteEditableList(room->editableListHead);
             room->editableListHead = createEditableList(room);
           }

           return;

    case ENTITY_MOB     :
           editMobType(((mobHere *)(editNode->entityPtr))->mobPtr, TRUE);
           return;

    default :
           sprintf(strn, "\nUnrecognized result (#%u)\n\n", whatMatched);
           _outtext(strn);
           return;
  }

  checkCommands(args, editCmdHead, "\n"
"Invalid argument(s) for EDIT - specify a keyword matching something in the room\n"
"or try one of <room|exit|object|mob|zone|quest|shop|desc|defroom|defobj|\n"
"defexit|defmob>.  Check the help or documentation for more details.\n\n",
                editExecCommand, FALSE);
}
