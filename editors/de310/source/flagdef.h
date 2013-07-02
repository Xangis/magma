#ifndef _FLAGDEF_H_

#define FLAG_SHORT_LEN    26
#define FLAG_LONG_LEN     41

typedef struct _flagDef
{
  char flagShort[FLAG_SHORT_LEN];
  char flagLong[FLAG_LONG_LEN];
  char editable;  // editable w/o special switch?  (in first element of
                  // table, specifies number of flags in table)
  char defVal;    // default value (IS_NPC for mobs - 1, DEATH for rooms - 0,
                  // for example - hardly any flags will have def vals of 1)
} flagDef;

#define _FLAGDEF_H_
#endif
