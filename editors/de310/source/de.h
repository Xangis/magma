// header for dikuEdit, the nifty little zone editor thingy

#ifndef _DE_H_

#include "types.h"
#include "misc/strnnode.h"
#include "boolean.h"


#define MAX_PROMPTINPUT_LEN  192    // max length of prompt input string

#define MAX_MAINNAME_LEN     MAX_VARVAL_LEN
                                    // max length of name of zone files

#define MAIN_PROMPT_COMM_NOT_FOUND  "\n&nInvalid command.  (Type \"HELP\" or \"?\" for help, \"COMMANDS\" for a command list.)\n\n"
                                    // string shown when invalid command
                                    // entered at main prompt

#define NUMB_FLAG_TEMPLATES  7      // number of flag templates

#define DE_VERSION           "3.10" // version string

#define DEFAULT_LOOKUP_ENTRIES  100000

#ifdef _WIN32
#  define DE_WIN32_CONSOLEAPP_NAME "BasterEd - The Basternae Area Editor"
#endif

#define _DE_H_
#endif
