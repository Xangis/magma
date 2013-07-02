//
//  File: editofl2.cpp   originally part of dikuEdit
//
//  Usage: user-interface functions for editing affect object flags
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include "../fh.h"
#include "../types.h"
#include "../const.h"

#include "object.h"

extern _CONST flagDef aff1FlagDef[], aff2FlagDef[], aff3FlagDef[], aff4FlagDef[];
extern ulong objAff1FlagTemplates[], objAff2FlagTemplates[];
extern ulong objAff3FlagTemplates[], objAff4FlagTemplates[];

//
// editObjectAffect1Flags : Edit obj extra flags - "main" function
//
//   *obj : obj to edit
//

void editObjAffect1Flags(dikuObject *obj)
{
  char strn[256], newName[256];

  fixObjName(obj->objShortName, newName);

  sprintf(strn, "&+gEditing affect1 flags for obj #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          obj->objNumber, newName);

  editFlags(aff1FlagDef, &(obj->affect1Bits.longIntFlags), strn,
            "obj affect1 flags", objAff1FlagTemplates, 3);
}


//
// editObjAffect2Flags : Edit obj extra flags - "main" function
//
//   *obj : obj to edit
//

void editObjAffect2Flags(dikuObject *obj)
{
  char strn[256], newName[256];

  fixObjName(obj->objShortName, newName);

  sprintf(strn, "&+gEditing affect2 flags for obj #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          obj->objNumber, newName);

  editFlags(aff2FlagDef, &(obj->affect2Bits.longIntFlags), strn,
            "obj affect2 flags", objAff2FlagTemplates, 3);
}


//
// editObjAffect3Flags : Edit obj extra flags - "main" function
//
//   *obj : obj to edit
//

void editObjAffect3Flags(dikuObject *obj)
{
  char strn[256], newName[256];

  fixObjName(obj->objShortName, newName);

  sprintf(strn, "&+gEditing affect3 flags for obj #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          obj->objNumber, newName);

  editFlags(aff3FlagDef, &(obj->affect3Bits.longIntFlags), strn,
            "obj affect3 flags", objAff3FlagTemplates, 3);
}


//
// editObjAffect4Flags : Edit obj extra flags - "main" function
//
//   *obj : obj to edit
//

void editObjAffect4Flags(dikuObject *obj)
{
  char strn[256], newName[256];

  fixObjName(obj->objShortName, newName);

  sprintf(strn, "&+gEditing affect4 flags for obj #&+c%u&+w, &+L\"&n%s&+L\"&n\n\n",
          obj->objNumber, newName);

  editFlags(aff4FlagDef, &(obj->affect4Bits.longIntFlags), strn,
            "obj affect4 flags", objAff4FlagTemplates, 3);
}
