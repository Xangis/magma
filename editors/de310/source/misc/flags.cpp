//
//  File: flags.cpp      originally part of dikuEdit
//
//  Usage: functions for handling commands related to flags
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <string.h>
#include <ctype.h>

#include "../fh.h"
#include "../types.h"
#include "../flagdef.h"
#include "../const.h"
#include "flags.h"

extern dikuRoom *roomHead;
extern dikuObject *objectHead;
extern dikuMob *mobHead;
extern _CONST flagDef roomFlagDef[], objExtraFlagDef[], objExtra2FlagDef[],
               objAntiFlagDef[], objAnti2FlagDef[], objWearFlagDef[],
               aff1FlagDef[], aff2FlagDef[], aff3FlagDef[], aff4FlagDef[],
               mobActFlagDef[];
extern char madeChanges;
extern ulong numbObjTypes, numbMobTypes;

//
// whichFlag : compares input versus the short names in the flag array pointed
//             to in flagArr, returning the index of the matching flag or -1
//             if no match
//

INLINEDEF long whichFlag(const char *input, const flagDef *flagArr)
{
  ulong i, numbFlags = flagArr[0].editable;


  for (i = 1; i <= numbFlags; i++)
  {
    if (!strcmp(input, flagArr[i].flagShort)) return i;
  }

  return -1;
}


//
// getFlagStrn : dumps short names into strn based on the flags that are set
//               in the bitvect flagVal and the names in flagArr
//

char *getFlagStrn(const ulong flagVal, const flagDef *flagArr, char *strn)
{
  ulong numbFlags = flagArr[0].editable, i;


  strn[0] = '\0';

  for (i = 0; i < numbFlags; i++)
  {
    if (flagVal & (1 << i))
    {
      strcat(strn, flagArr[i + 1].flagShort);
      strcat(strn, " ");
    }
  }

  remTrailingSpaces(strn);

  return strn;
}


//
// fixFlags : "fixes" flags in a bitvector by checking each bit to see
//            if it is not editable and not set to its default value.
//            if both conditions are true, the bit is toggled
//
//     flagVal : pointer to bitvector being changed
//     flagArr : array of flag info for flagVal
//

ulong fixFlags(ulong *flagVal, const flagDef *flagArr)
{
  ulong i, numbFlags = flagArr[0].editable, val, changed = 0;

  for (i = 0; i < numbFlags; i++)
  {
    val = (*flagVal & (1 << i));
    if (val) val = 1;

    if ((!flagArr[i + 1].editable) && (flagArr[i + 1].defVal != val))
    {
      *flagVal ^= (1 << i);
      changed++;
    }
  }

  return changed;
}


//
// fixAllFlags : "fixes" flags - that is, sets all uneditable flags that aren't
//               set to their default value to their default value
//

void fixAllFlags(void)
{
  dikuRoom *room = roomHead;
  dikuObject *obj = objectHead;
  dikuMob *mob = mobHead;
  char strn[256];
  ulong numb = 0, bitVects = 0;

  while (room)
  {
    numb += fixFlags(&(room->roomFlags.longIntFlags), roomFlagDef);
    bitVects++;

    room = room->Next;
  }

  while (obj)
  {
    numb += fixFlags(&(obj->extraBits.longIntFlags), objExtraFlagDef);
    bitVects++;
    numb += fixFlags(&(obj->extra2Bits.longIntFlags), objExtra2FlagDef);
    bitVects++;
    numb += fixFlags(&(obj->antiBits.longIntFlags), objAntiFlagDef);
    bitVects++;
    numb += fixFlags(&(obj->anti2Bits.longIntFlags), objAnti2FlagDef);
    bitVects++;
    numb += fixFlags(&(obj->wearBits.longIntFlags), objWearFlagDef);
    bitVects++;
    numb += fixFlags(&(obj->affect1Bits.longIntFlags), aff1FlagDef);
    bitVects++;
    numb += fixFlags(&(obj->affect2Bits.longIntFlags), aff2FlagDef);
    bitVects++;
    numb += fixFlags(&(obj->affect3Bits.longIntFlags), aff3FlagDef);
    bitVects++;
    numb += fixFlags(&(obj->affect4Bits.longIntFlags), aff4FlagDef);
    bitVects++;

    obj = obj->Next;
  }

  while (mob)
  {
    numb += fixFlags(&(mob->actionBits.longIntFlags), mobActFlagDef);
    bitVects++;
    numb += fixFlags(&(mob->affect1Bits.longIntFlags), aff1FlagDef);
    bitVects++;
    numb += fixFlags(&(mob->affect2Bits.longIntFlags), aff2FlagDef);
    bitVects++;
    numb += fixFlags(&(mob->affect3Bits.longIntFlags), aff3FlagDef);
    bitVects++;
    numb += fixFlags(&(mob->affect4Bits.longIntFlags), aff4FlagDef);
    bitVects++;

    mob = mob->Next;
  }

  sprintf(strn,
"\nReset %u uneditable non-default flag%s in %u bit vector%s.\n\n",
          numb, plural(numb), bitVects, plural(bitVects));

  _outtext(strn);

  if (numb) madeChanges = TRUE;
}


//
// which : much like the which command for duris..  except this one doesn't
//         have to take a room, obj, or mob arg (and it checks all the
//         flag fields)
//

void which(const char *args)
{
  dikuRoom *room = roomHead;
  dikuObject *obj = objectHead;
  dikuMob *mob = mobHead;

  char outStrn[512], flagName[256],
       foundSomething = FALSE, foundRoom = FALSE, foundObj = FALSE,
       displayedEqual = FALSE, checkWhat = 0, found = FALSE, notFlag = FALSE;

  ulong i = 0, bitVect, flagVal;
  long matched = 0, flagNumb;


  _setbkcolor(0);

  if (strlen(args) == 0)
  {
    _outtext("\nThe \"which\" command requires at least one argument.\n\n");
    return;
  }

  getArg(args, 1, flagName);

  if (strleft("ROOM", flagName)) checkWhat = ROOM_FLAGS;
  else if (strleft("OBJECT", flagName)) checkWhat = OBJ_FLAGS;
  else if (strleft("MOB", flagName)) checkWhat = MOB_FLAGS;
  else if (strleft("CHAR", flagName)) checkWhat = MOB_FLAGS;
  else checkWhat = ALL_FLAGS;

  if (checkWhat != ALL_FLAGS)
  {
    getArg(args, 2, flagName);

    if (!flagName[0])
    {
      _outtext("\n"
"Specify a flag name to look for as the second argument when checking rooms,\n"
"objects, or mobs specifically.\n\n");
      return;
    }
  }
  else strcpy(flagName, args);

  upstrn(flagName);

  if (flagName[0] == '!')
  {
    notFlag = TRUE;
    deleteChar(flagName, 0);
  }

  _setbkcolor(0);

 // check for flag existance

  if (checkWhat & ROOM_FLAGS)
  {
    if (whichFlag(flagName, roomFlagDef) != -1)
    {
      found = TRUE;
    }
  }

  if (checkWhat & OBJ_FLAGS)
  {
    if ((whichFlag(flagName, objExtraFlagDef) != -1) ||
        (whichFlag(flagName, objWearFlagDef) != -1) ||
        (whichFlag(flagName, objExtra2FlagDef) != -1) ||
        (whichFlag(flagName, objAntiFlagDef) != -1) ||
        (whichFlag(flagName, objAnti2FlagDef) != -1) ||
        (whichFlag(flagName, aff1FlagDef) != -1) ||
        (whichFlag(flagName, aff2FlagDef) != -1) ||
        (whichFlag(flagName, aff3FlagDef) != -1) ||
        (whichFlag(flagName, aff4FlagDef) != -1))
    {
      found = TRUE;
    }
  }

  if (checkWhat & MOB_FLAGS)
  {
    if ((whichFlag(flagName, mobActFlagDef) != -1) ||
        (whichFlag(flagName, aff1FlagDef) != -1) ||
        (whichFlag(flagName, aff2FlagDef) != -1) ||
        (whichFlag(flagName, aff3FlagDef) != -1) ||
        (whichFlag(flagName, aff4FlagDef) != -1))
    {
      found = TRUE;
    }
  }

  if (!found)
  {
    if (checkWhat == ALL_FLAGS)
    {
      _outtext("\n"
"That flag does not exist for any room, mob, or object bit vectors.\n\n");
      return;
    }

    if (checkWhat == ROOM_FLAGS)
    {
      _outtext("\n"
"That flag does not exist for any room bit vectors.\n\n");
      return;
    }

    if (checkWhat == OBJ_FLAGS)
    {
      _outtext("\n"
"That flag does not exist for any object bit vectors.\n\n");
      return;
    }

    if (checkWhat == MOB_FLAGS)
    {
      _outtext("\n"
"That flag does not exist for any mob bit vectors.\n\n");
      return;
    }

    _outtext("\nerror in which(): found is not set, but checkWhat is invalid\n\n");
    return;
  }

  _outtext("\n\n");

 // check for rooms with this flagname set

  if (checkWhat & ROOM_FLAGS)
  {
    flagNumb = whichFlag(flagName, roomFlagDef);

    if (flagNumb > 0)
    {
      while (room)
      {
        flagVal = 1 << (flagNumb - 1);

        if (notFlag ? (!(room->roomFlags.longIntFlags & flagVal)) :
                      (room->roomFlags.longIntFlags & flagVal))
        {
          sprintf(outStrn, "&n%s&n (#%u)\n", room->roomName, room->roomNumber);

          foundSomething = foundRoom = TRUE;
          displayColorString(outStrn);

          i += numbLinesStringNeeds(outStrn);

          if (i >= (getScreenHeight() - 3))
          {
            displayColorString("\n&+CPress any key to continue or Q to quit..&n");
            if (toupper(getkey()) == 'Q')
            {
              _outtext("\n\n");

              return;
            }

            _outtext("\n\n");

            i = 0;
          }
        }

        room = room->Next;
      } // while (room)
    } // flagnumb > 0
  }

  if (checkWhat & OBJ_FLAGS)
  {
   // we'll assume that no bitvects have flags with the same name..

    flagNumb = whichFlag(flagName, objExtraFlagDef);
    if (flagNumb != -1) matched = EXTR_FL;

    if (!matched)
    {
      flagNumb = whichFlag(flagName, objExtra2FlagDef);
      if (flagNumb != -1) matched = EXT2_FL;
    }

    if (!matched)
    {
      flagNumb = whichFlag(flagName, objWearFlagDef);
      if (flagNumb != -1) matched = WEAR_FL;
    }

    if (!matched)
    {
      flagNumb = whichFlag(flagName, objAntiFlagDef);
      if (flagNumb != -1) matched = ANTI_FL;
    }

    if (!matched)
    {
      flagNumb = whichFlag(flagName, objAnti2FlagDef);
      if (flagNumb != -1) matched = ANT2_FL;
    }

    if (!matched)
    {
      flagNumb = whichFlag(flagName, aff1FlagDef);
      if (flagNumb != -1) matched = AFF1_FL;
    }

    if (!matched)
    {
      flagNumb = whichFlag(flagName, aff2FlagDef);
      if (flagNumb != -1) matched = AFF2_FL;
    }

    if (!matched)
    {
      flagNumb = whichFlag(flagName, aff3FlagDef);
      if (flagNumb != -1) matched = AFF3_FL;
    }

    if (!matched)
    {
      flagNumb = whichFlag(flagName, aff4FlagDef);
      if (flagNumb != -1) matched = AFF4_FL;
    }

    if (!matched)
    {
      if (checkWhat != ALL_FLAGS)
      {
        _outtext("ERROR - no matching flag found for objects\n");
        i++;
      }
    }
    else
    {
      flagVal = 1 << (flagNumb - 1);

      while (obj)
      {
        switch (matched)
        {
          case EXTR_FL : bitVect = obj->extraBits.longIntFlags; break;
          case EXT2_FL : bitVect = obj->extra2Bits.longIntFlags; break;
          case WEAR_FL : bitVect = obj->wearBits.longIntFlags; break;
          case ANTI_FL : bitVect = obj->antiBits.longIntFlags; break;
          case ANT2_FL : bitVect = obj->anti2Bits.longIntFlags; break;
          case AFF1_FL : bitVect = obj->affect1Bits.longIntFlags; break;
          case AFF2_FL : bitVect = obj->affect2Bits.longIntFlags; break;
          case AFF3_FL : bitVect = obj->affect3Bits.longIntFlags; break;
          case AFF4_FL : bitVect = obj->affect4Bits.longIntFlags; break;

          default : _outtext("ERROR - matched is set to an invalid value\n");
                    i++;
        }

        if (notFlag ? (!(bitVect & flagVal)) : bitVect & flagVal)
        {
          if ((foundRoom) && (!displayedEqual))
          {
            _outtext("===\n");
            i++;
            displayedEqual = TRUE;
          }

          sprintf(outStrn, "%s&n (#%u)\n", obj->objShortName, obj->objNumber);

          foundSomething = foundObj = TRUE;
          displayColorString(outStrn);

          i += numbLinesStringNeeds(outStrn);

          if (i >= (getScreenHeight() - 3))
          {
            displayColorString("\n&+CPress any key to continue or Q to quit..&n");
            if (toupper(getkey()) == 'Q')
            {
              _outtext("\n\n");

              return;
            }

            _outtext("\n\n");

            i = 0;
          }
        }

        obj = obj->Next;
      }
    }
  }

  displayedEqual = FALSE;
  matched = 0;

  if (checkWhat & MOB_FLAGS)
  {
   // we'll assume that no bitvects have flags with the same name..

    flagNumb = whichFlag(flagName, mobActFlagDef);
    if (flagNumb != -1) matched = MACT_FL;

    if (!matched)
    {
      flagNumb = whichFlag(flagName, aff1FlagDef);
      if (flagNumb != -1) matched = AFF1_FL;
    }

    if (!matched)
    {
      flagNumb = whichFlag(flagName, aff2FlagDef);
      if (flagNumb != -1) matched = AFF2_FL;
    }

    if (!matched)
    {
      flagNumb = whichFlag(flagName, aff3FlagDef);
      if (flagNumb != -1) matched = AFF3_FL;
    }

    if (!matched)
    {
      flagNumb = whichFlag(flagName, aff4FlagDef);
      if (flagNumb != -1) matched = AFF4_FL;
    }

    if (!matched)
    {
      if (checkWhat != ALL_FLAGS)
      {
        _outtext("ERROR - no matching flag found for mobs\n");
        i++;
      }
    }
    else
    {
      flagVal = 1 << (flagNumb - 1);

      while (mob)
      {
        switch (matched)
        {
          case MACT_FL : bitVect = mob->actionBits.longIntFlags; break;
          case AFF1_FL : bitVect = mob->affect1Bits.longIntFlags; break;
          case AFF2_FL : bitVect = mob->affect2Bits.longIntFlags; break;
          case AFF3_FL : bitVect = mob->affect3Bits.longIntFlags; break;
          case AFF4_FL : bitVect = mob->affect4Bits.longIntFlags; break;


          default : _outtext("ERROR - matched is set to an invalid value\n");
                    i++;
        }

        if (notFlag ? (!(bitVect & flagVal)) : bitVect & flagVal)
        {
          if ((foundObj) && (!displayedEqual))
          {
            _outtext("===\n");
            i++;
            displayedEqual = TRUE;
          }

          sprintf(outStrn, "%s&n (#%u)\n", mob->mobShortName, mob->mobNumber);

          foundSomething = TRUE;
          displayColorString(outStrn);

          i += numbLinesStringNeeds(outStrn);

          if (i >= (getScreenHeight() - 3))
          {
            displayColorString("\n&+CPress any key to continue or Q to quit..&n");
            if (toupper(getkey()) == 'Q')
            {
              _outtext("\n\n");

              return;
            }

            _outtext("\n\n");

            i = 0;
          }
        }

        mob = mob->Next;
      }
    }
  }

  if (!foundSomething) _outtext("No matching rooms/objects/mobs found.\n");
  _outtext("\n");
}


//
// massSet : mass-set a particular bit in a bitvector on or off for all
//           rooms/object types/mob types
//

void massSet(const char *args)
{
  dikuRoom *room = roomHead;
  dikuObject *obj = objectHead;
  dikuMob *mob = mobHead;

  char flagName[256], temp[256], setWhat = 0, found = FALSE, setOn;

  ulong flagVal;
  long matched = 0, flagNumb;


  _setbkcolor(0);

  if (strlen(args) == 0)
  {
    _outtext("\nThe \"massset\" command requires three arguments.\n\n");
    return;
  }

  getArg(args, 1, flagName);

  if (strleft("ROOM", flagName)) setWhat = ROOM_FLAGS;
  else if (strleft("OBJECT", flagName)) setWhat = OBJ_FLAGS;
  else if (strleft("MOB", flagName)) setWhat = MOB_FLAGS;
  else if (strleft("CHAR", flagName)) setWhat = MOB_FLAGS;
  else
  {
    _outtext("\n"
"Specify one of room, object, or mob/char as the first argument.\n\n");
     return;
  }

  getArg(args, 2, flagName);
  if (!flagName[0])
  {
    _outtext("\n"
"Specify a flag name to set as the second argument.\n\n");
      return;
  }

  getArg(args, 3, temp);

  if (!strcmp(temp, "ON") || !strcmp(temp, "1")) setOn = TRUE;
  else if (!strcmp(temp, "OFF") || !strcmp(temp, "0")) setOn = FALSE;
  else
  {
    _outtext("\n"
"Specify whether you want to turn the specific flag on or off as the third\n"
"argument.\n\n");
    return;
  }

  upstrn(flagName);

  _setbkcolor(0);

 // check for flag existance

  if (setWhat == ROOM_FLAGS)
  {
    if (whichFlag(flagName, roomFlagDef) != -1)
    {
      found = TRUE;
    }
  }

  if (setWhat == OBJ_FLAGS)
  {
    if (!numbObjTypes)
    {
      _outtext("\nThere are no object types to set.\n\n");
      return;
    }

    if ((whichFlag(flagName, objExtraFlagDef) != -1) ||
        (whichFlag(flagName, objWearFlagDef) != -1) ||
        (whichFlag(flagName, objExtra2FlagDef) != -1) ||
        (whichFlag(flagName, objAntiFlagDef) != -1) ||
        (whichFlag(flagName, objAnti2FlagDef) != -1) ||
        (whichFlag(flagName, aff1FlagDef) != -1) ||
        (whichFlag(flagName, aff2FlagDef) != -1) ||
        (whichFlag(flagName, aff3FlagDef) != -1) ||
        (whichFlag(flagName, aff4FlagDef) != -1))
    {
      found = TRUE;
    }
  }

  if (setWhat == MOB_FLAGS)
  {
    if (!numbMobTypes)
    {
      _outtext("\nThere are no mob types to set.\n\n");
      return;
    }

    if ((whichFlag(flagName, mobActFlagDef) != -1) ||
        (whichFlag(flagName, aff1FlagDef) != -1) ||
        (whichFlag(flagName, aff2FlagDef) != -1) ||
        (whichFlag(flagName, aff3FlagDef) != -1) ||
        (whichFlag(flagName, aff4FlagDef) != -1))
    {
      found = TRUE;
    }
  }

  if (!found)
  {
    if (setWhat == ROOM_FLAGS)
    {
      _outtext("\n"
"That flag does not exist for any room bit vectors.\n\n");
      return;
    }

    if (setWhat == OBJ_FLAGS)
    {
      _outtext("\n"
"That flag does not exist for any object bit vectors.\n\n");
      return;
    }

    if (setWhat == MOB_FLAGS)
    {
      _outtext("\n"
"That flag does not exist for any mob bit vectors.\n\n");
      return;
    }

    _outtext("\nerror in massset(): found is not set, but setWhat is invalid\n\n");
    return;
  }

 // check for rooms with this flagname set

  if (setWhat == ROOM_FLAGS)
  {
    flagNumb = whichFlag(flagName, roomFlagDef);

    if (flagNumb > 0)
    {
      if (!roomFlagDef[flagNumb].editable &&
          !getEditUneditableFlagsVal())
      {
        _outtext("\nAlas, that flag value is not editable.\n\n");
        return;
      }

      while (room)
      {
        flagVal = 1 << (flagNumb - 1);

        if (setOn) room->roomFlags.longIntFlags |= flagVal;
        else room->roomFlags.longIntFlags &= ~flagVal;

        room = room->Next;
      }
    }
  }
  else
  if (setWhat == OBJ_FLAGS)
  {
   // we'll assume that no bitvects have flags with the same name..

    flagNumb = whichFlag(flagName, objExtraFlagDef);
    if (flagNumb != -1) matched = EXTR_FL;

    if (!matched)
    {
      flagNumb = whichFlag(flagName, objExtra2FlagDef);
      if (flagNumb != -1) matched = EXT2_FL;
    }

    if (!matched)
    {
      flagNumb = whichFlag(flagName, objWearFlagDef);
      if (flagNumb != -1) matched = WEAR_FL;
    }

    if (!matched)
    {
      flagNumb = whichFlag(flagName, objAntiFlagDef);
      if (flagNumb != -1) matched = ANTI_FL;
    }

    if (!matched)
    {
      flagNumb = whichFlag(flagName, objAnti2FlagDef);
      if (flagNumb != -1) matched = ANT2_FL;
    }

    if (!matched)
    {
      flagNumb = whichFlag(flagName, aff1FlagDef);
      if (flagNumb != -1) matched = AFF1_FL;
    }

    if (!matched)
    {
      flagNumb = whichFlag(flagName, aff2FlagDef);
      if (flagNumb != -1) matched = AFF2_FL;
    }

    if (!matched)
    {
      flagNumb = whichFlag(flagName, aff3FlagDef);
      if (flagNumb != -1) matched = AFF3_FL;
    }

    if (!matched)
    {
      flagNumb = whichFlag(flagName, aff4FlagDef);
      if (flagNumb != -1) matched = AFF4_FL;
    }

    if (!matched)
    {
      _outtext("ERROR - no matching flag found for objects\n");
    }
    else
    {
      flagVal = 1 << (flagNumb - 1);

      while (obj)
      {
        switch (matched)
        {
          case EXTR_FL :
             if (!objExtraFlagDef[flagNumb].editable &&
                 !getEditUneditableFlagsVal())
             {
               _outtext("\nAlas, that flag value is not editable.\n\n");
               return;
             }

             if (setOn) obj->extraBits.longIntFlags |= flagVal;
             else obj->extraBits.longIntFlags &= ~flagVal;
             break;

          case EXT2_FL :
             if (!objExtra2FlagDef[flagNumb].editable &&
                 !getEditUneditableFlagsVal())
             {
               _outtext("\nAlas, that flag value is not editable.\n\n");
               return;
             }
              if (setOn) obj->extra2Bits.longIntFlags |= flagVal;
             else obj->extra2Bits.longIntFlags &= ~flagVal;
             break;

          case WEAR_FL :
             if (!objWearFlagDef[flagNumb].editable &&
                 !getEditUneditableFlagsVal())
             {
               _outtext("\nAlas, that flag value is not editable.\n\n");
               return;
             }
              if (setOn) obj->wearBits.longIntFlags |= flagVal;
             else obj->wearBits.longIntFlags &= ~flagVal;
             break;

          case ANTI_FL :
             if (!objAntiFlagDef[flagNumb].editable &&
                 !getEditUneditableFlagsVal())
             {
               _outtext("\nAlas, that flag value is not editable.\n\n");
               return;
             }
              if (setOn) obj->antiBits.longIntFlags |= flagVal;
             else obj->antiBits.longIntFlags &= ~flagVal;
             break;

          case ANT2_FL :
             if (!objAnti2FlagDef[flagNumb].editable &&
                 !getEditUneditableFlagsVal())
             {
               _outtext("\nAlas, that flag value is not editable.\n\n");
               return;
             }
              if (setOn) obj->anti2Bits.longIntFlags |= flagVal;
             else obj->anti2Bits.longIntFlags &= ~flagVal;
             break;

          case AFF1_FL :
             if (!aff1FlagDef[flagNumb].editable &&
                 !getEditUneditableFlagsVal())
             {
               _outtext("\nAlas, that flag value is not editable.\n\n");
               return;
             }
              if (setOn) obj->affect1Bits.longIntFlags |= flagVal;
             else obj->affect1Bits.longIntFlags &= ~flagVal;
             break;

          case AFF2_FL :
             if (!aff2FlagDef[flagNumb].editable &&
                 !getEditUneditableFlagsVal())
             {
               _outtext("\nAlas, that flag value is not editable.\n\n");
               return;
             }
              if (setOn) obj->affect2Bits.longIntFlags |= flagVal;
             else obj->affect2Bits.longIntFlags &= ~flagVal;
             break;

          case AFF3_FL :
             if (!aff3FlagDef[flagNumb].editable &&
                 !getEditUneditableFlagsVal())
             {
               _outtext("\nAlas, that flag value is not editable.\n\n");
               return;
             }
              if (setOn) obj->affect3Bits.longIntFlags |= flagVal;
             else obj->affect3Bits.longIntFlags &= ~flagVal;
             break;

          case AFF4_FL :
             if (!aff4FlagDef[flagNumb].editable &&
                 !getEditUneditableFlagsVal())
             {
               _outtext("\nAlas, that flag value is not editable.\n\n");
               return;
             }
              if (setOn) obj->affect4Bits.longIntFlags |= flagVal;
             else obj->affect4Bits.longIntFlags &= ~flagVal;
             break;


          default : _outtext("ERROR - matched is set to an invalid value\n");
        }

        obj = obj->Next;
      }
    }
  }
  else
  if (setWhat == MOB_FLAGS)
  {
   // we'll assume that no bitvects have flags with the same name..

    flagNumb = whichFlag(flagName, mobActFlagDef);
    if (flagNumb != -1) matched = MACT_FL;

    if (!matched)
    {
      flagNumb = whichFlag(flagName, aff1FlagDef);
      if (flagNumb != -1) matched = AFF1_FL;
    }

    if (!matched)
    {
      flagNumb = whichFlag(flagName, aff2FlagDef);
      if (flagNumb != -1) matched = AFF2_FL;
    }

    if (!matched)
    {
      flagNumb = whichFlag(flagName, aff3FlagDef);
      if (flagNumb != -1) matched = AFF3_FL;
    }

    if (!matched)
    {
      flagNumb = whichFlag(flagName, aff4FlagDef);
      if (flagNumb != -1) matched = AFF4_FL;
    }

    if (!matched)
    {
      _outtext("ERROR - no matching flag found for mobs\n");
    }
    else
    {
      flagVal = 1 << (flagNumb - 1);

      while (mob)
      {
        switch (matched)
        {
          case MACT_FL :
             if (!mobActFlagDef[flagNumb].editable &&
                 !getEditUneditableFlagsVal())
             {
               _outtext("\nAlas, that flag value is not editable.\n\n");
               return;
             }

             if (setOn) mob->actionBits.longIntFlags |= flagVal;
             else mob->actionBits.longIntFlags &= ~flagVal;
             break;

          case AFF1_FL :
             if (!aff1FlagDef[flagNumb].editable &&
                 !getEditUneditableFlagsVal())
             {
               _outtext("\nAlas, that flag value is not editable.\n\n");
               return;
             }

             if (setOn) mob->affect1Bits.longIntFlags |= flagVal;
             else mob->affect1Bits.longIntFlags &= ~flagVal;
             break;

          case AFF2_FL :
             if (!aff2FlagDef[flagNumb].editable &&
                 !getEditUneditableFlagsVal())
             {
               _outtext("\nAlas, that flag value is not editable.\n\n");
               return;
             }

             if (setOn) mob->affect2Bits.longIntFlags |= flagVal;
             else mob->affect2Bits.longIntFlags &= ~flagVal;
             break;

          case AFF3_FL :
             if (!aff3FlagDef[flagNumb].editable &&
                 !getEditUneditableFlagsVal())
             {
               _outtext("\nAlas, that flag value is not editable.\n\n");
               return;
             }

             if (setOn) mob->affect3Bits.longIntFlags |= flagVal;
             else mob->affect3Bits.longIntFlags &= ~flagVal;
             break;

          case AFF4_FL :
             if (!aff4FlagDef[flagNumb].editable &&
                 !getEditUneditableFlagsVal())
             {
               _outtext("\nAlas, that flag value is not editable.\n\n");
               return;
             }

             if (setOn) mob->affect4Bits.longIntFlags |= flagVal;
             else mob->affect4Bits.longIntFlags &= ~flagVal;
             break;


          default : _outtext("ERROR - matched is set to an invalid value\n");
        }

        mob = mob->Next;
      }
    }
  }

  _outtext("\nDone.\n\n");
  madeChanges = TRUE;
}
