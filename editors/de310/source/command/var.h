#ifndef _VAR_H_

#define MAX_VARNAME_LEN    41
#define MAX_VARVAL_LEN    256

typedef struct _variable
{
  char varName[MAX_VARNAME_LEN];  // name of variable
  char varValue[MAX_VARVAL_LEN]; // value of variable
  char varType;       // type of variable - used for mass setting, etc

  struct _variable *Next;
} variable;

#define VARTYPE_LOWEST       0
#define VARTYPE_DISPVAR      0   // display config var
#define VARTYPE_CONFIGVAR    1   // 'config' config var
#define VARTYPE_CHECKVAR     2   // check config var
#define VARTYPE_USERVAR      3   // user-defined var
#define VARTYPE_OTHER        4   // shrug
#define VARTYPE_HIGHEST      4

#define VAR_ADDED       0
#define VAR_REPLACED    1
//#define VAR_DELETED     2  // don't need it baby
#define VAR_BLANK       3
#define VAR_SAMEVAL     4
#define VAR_ERROR       5
#define VAR_DISPLAY     6
#define VAR_CANNOT_BE_ZERO  7

#define _VAR_H_
#endif
