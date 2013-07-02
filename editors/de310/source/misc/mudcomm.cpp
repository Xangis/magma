//
//  File: mudcomm.cpp    originally part of dikuEdit
//
//  Usage: big functions for retrieving command strings for all mud
//         commands
//
//  Copyright 1995-98 (C) Michael Glosenger
//


#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../types.h"
#include "../fh.h"
#include "mudcomm.h"

//
// getCommandStrn
//

const char *getCommandStrn(const ulong commandValue, const char verboseDisplay)
{
  if ((commandValue > MUDCOMM_HIGHEST) || (commandValue < MUDCOMM_LOWEST))
    return "command type out of range";

  switch (commandValue)
  {
    case MUDCOMM_NORTH      : return "north";
    case MUDCOMM_EAST       : return "east";
    case MUDCOMM_SOUTH      : return "south";
    case MUDCOMM_WEST       : return "west";
    case MUDCOMM_UP         : return "up";
    case MUDCOMM_DOWN       : return "down";
    case MUDCOMM_ENTER      : return "enter";
    case MUDCOMM_TURN       : return "turn";
    case MUDCOMM_KISS       : return "kiss";
    case MUDCOMM_CLIMB      : return "climb";
    case MUDCOMM_SWING      : return "swing";
    case MUDCOMM_GET        : return "get";
    case MUDCOMM_DRINK      : return "drink";
    case MUDCOMM_EAT        : return "eat";
    case MUDCOMM_WEAR       : return "wear";
    case MUDCOMM_WIELD      : return "wield";
    case MUDCOMM_LOOK       : return "look";
//    case MUDCOMM_TASTE      : return "taste";
    case MUDCOMM_SAY        : return "say";
    case MUDCOMM_SHOUT      : return "shout";
    case MUDCOMM_TELL       : return "tell";
    case MUDCOMM_BOUNCE     : return "bounce";
    case MUDCOMM_SMILE      : return "smile";
    case MUDCOMM_DANCE      : return "dance";
    case MUDCOMM_KILL       : return "kill";
    case MUDCOMM_CACKLE     : return "cackle";
    case MUDCOMM_LAUGH      : return "laugh";
    case MUDCOMM_GIGGLE     : return "giggle";
    case MUDCOMM_SHAKE      : return "shake";
    case MUDCOMM_PUKE       : return "puke";
    case MUDCOMM_GROWL      : return "growl";
    case MUDCOMM_SCREAM     : return "scream";
    case MUDCOMM_INSULT     : return "insult";
    case MUDCOMM_COMFORT    : return "comfort";
    case MUDCOMM_NOD        : return "nod";
    case MUDCOMM_SIGH       : return "sigh";
    case MUDCOMM_SULK       : return "sulk";
    case MUDCOMM_PART       : return "part";
    case MUDCOMM_TWIST      : return "twist";
//    case MUDCOMM_SING       : return "sing";
    case MUDCOMM_CHANT      : return "chant";
    case MUDCOMM_STAND      : return "stand";
    case MUDCOMM_SIT        : return "sit";
    case MUDCOMM_REST       : return "rest";
    case MUDCOMM_SLEEP      : return "sleep";
    case MUDCOMM_WAKE       : return "wake";
    case MUDCOMM_DUST       : return "dust";
    case MUDCOMM_HUNT       : return "hunt";
    case MUDCOMM_HUG        : return "hug";
    case MUDCOMM_SNUGGLE    : return "snuggle";
    case MUDCOMM_CUDDLE     : return "cuddle";
    case MUDCOMM_NUZZLE     : return "nuzzle";
    case MUDCOMM_CRY        : return "cry";
    case MUDCOMM_SHIMMY     : return "shimmy";
    case MUDCOMM_SWIM       : return "swim";
    case MUDCOMM_FORGE      : return "forge";
//    case MUDCOMM_BASH       : return "bash";
    case MUDCOMM_SMASH      : return "smash";
//    case MUDCOMM_SMELL      : return "smell";
    case MUDCOMM_DROP       : return "drop";
    case MUDCOMM_GOTO       : if (verboseDisplay) return "goto";
                              else return "\0";
    case MUDCOMM_WEATHER    : return "weather";
    case MUDCOMM_READ       : return "read";
    case MUDCOMM_POUR       : return "pour";
    case MUDCOMM_GRAB       : return "grab";
    case MUDCOMM_REMOVE     : if (verboseDisplay) return "remove";
                              else return "\0";
    case MUDCOMM_PUT        : return "put";
    case MUDCOMM_SHUTDOW    : if (verboseDisplay) return "shutdow";
                              else return "\0";
    case MUDCOMM_SAVE       : if (verboseDisplay) return "save";
                              else return "\0";
    case MUDCOMM_HIT        : return "hit";
    case MUDCOMM_STRING     : if (verboseDisplay) return "string";
                              else return "\0";
    case MUDCOMM_GIVE       : return "give";
    case MUDCOMM_QUIT       : if (verboseDisplay) return "quit";
                              else return "\0";
    case MUDCOMM_STAT       : if (verboseDisplay) return "stat";
                              else return "\0";
    case MUDCOMM_SETSKILL   : if (verboseDisplay) return "setskill";
                              else return "\0";
    case MUDCOMM_TIME       : if (verboseDisplay) return "time";
                              else return "\0";
    case MUDCOMM_LOAD       : if (verboseDisplay) return "load";
                              else return "\0";
    case MUDCOMM_PURGE      : if (verboseDisplay) return "purge";
                              else return "\0";
    case MUDCOMM_SHUTDOWN   : if (verboseDisplay) return "shutdown";
                              else return "\0";
    case MUDCOMM_IDEA       : if (verboseDisplay) return "idea";
                              else return "\0";
    case MUDCOMM_TYPO       : if (verboseDisplay) return "typo";
                              else return "\0";
    case MUDCOMM_BUG        : if (verboseDisplay) return "bug";
                              else return "\0";
    case MUDCOMM_WHISPER    : return "whisper";
    case MUDCOMM_CAST       : if (verboseDisplay) return "cast";
                              else return "\0";
    case MUDCOMM_AT         : if (verboseDisplay) return "at";
                              else return "\0";
    case MUDCOMM_ASK        : return "ask";
    case MUDCOMM_ORDER      : return "order";
    case MUDCOMM_SIP        : return "sip";
    case MUDCOMM_TASTE      : return "taste";
    case MUDCOMM_SNOOP      : if (verboseDisplay) return "snoop";
                              else return "\0";
    case MUDCOMM_FOLLOW     : return "follow";
    case MUDCOMM_RENT       : if (verboseDisplay) return "rent";
                              else return "\0";
    case MUDCOMM_OFFER      : return "offer";
    case MUDCOMM_POKE       : return "poke";
    case MUDCOMM_ADVANCE    : if (verboseDisplay) return "advance";
                              else return "\0";
    case MUDCOMM_ACCUSE     : return "accuse";
    case MUDCOMM_GRIN       : return "grin";
    case MUDCOMM_BOW        : return "bow";
    case MUDCOMM_OPEN       : return "open";
    case MUDCOMM_CLOSE      : return "close";
    case MUDCOMM_LOCK       : return "lock";
    case MUDCOMM_UNLOCK     : return "unlock";
    case MUDCOMM_LEAVE      : return "leave";
    case MUDCOMM_APPLAUD    : return "applaud";
    case MUDCOMM_BLUSH      : return "blush";
    case MUDCOMM_BURP       : return "burp";
    case MUDCOMM_CHUCKLE    : return "chuckle";
    case MUDCOMM_CLAP       : return "clap";
    case MUDCOMM_COUGH      : return "cough";
    case MUDCOMM_CURTSEY    : return "curtsey";
    case MUDCOMM_FART       : return "fart";
    case MUDCOMM_FLIP       : return "flip";
    case MUDCOMM_FONDLE     : return "fondle";
    case MUDCOMM_FROWN      : return "frown";
    case MUDCOMM_GASP       : return "gasp";
    case MUDCOMM_GLARE      : return "glare";
    case MUDCOMM_GROAN      : return "groan";
    case MUDCOMM_GROPE      : return "grope";
    case MUDCOMM_HICCUP     : return "hiccup";
    case MUDCOMM_LICK       : return "lick";
    case MUDCOMM_LOVE       : return "love";
    case MUDCOMM_MOAN       : return "moan";
    case MUDCOMM_NIBBLE     : return "nibble";
    case MUDCOMM_POUT       : return "pout";
    case MUDCOMM_PURR       : return "purr";
    case MUDCOMM_RUFFLE     : return "ruffle";
    case MUDCOMM_SHIVER     : return "shiver";
    case MUDCOMM_SHRUG      : return "shrug";
    case MUDCOMM_SING       : return "sing";
    case MUDCOMM_SLAP       : return "slap";
    case MUDCOMM_SMIRK      : return "smirk";
    case MUDCOMM_SNAP       : return "snap";
    case MUDCOMM_SNEEZE     : return "sneeze";
    case MUDCOMM_SNICKER    : return "snicker";
    case MUDCOMM_SNIFF      : return "sniff";
    case MUDCOMM_SNORE      : return "snore";
    case MUDCOMM_SPIT       : return "spit";
    case MUDCOMM_SQUEEZE    : return "squeeze";
    case MUDCOMM_STARE      : return "stare";
    case MUDCOMM_STRUT      : return "strut";
    case MUDCOMM_THANK      : return "thank";
    case MUDCOMM_TWIDDLE    : return "twiddle";
    case MUDCOMM_WAVE       : return "wave";
    case MUDCOMM_WHISTLE    : return "whistle";
    case MUDCOMM_WIGGLE     : return "wiggle";
    case MUDCOMM_WINK       : return "wink";
    case MUDCOMM_YAWN       : return "yawn";
    case MUDCOMM_SNOWBALL   : if (verboseDisplay) return "snowball";
                              else return "\0";
    case MUDCOMM_WRITE      : if (verboseDisplay) return "write";
                              else return "\0";
    case MUDCOMM_HOLD       : return "hold";
    case MUDCOMM_FLEE       : if (verboseDisplay) return "flee";
                              else return "\0";
    case MUDCOMM_SNEAK      : if (verboseDisplay) return "sneak";
                              else return "\0";
    case MUDCOMM_HIDE       : if (verboseDisplay) return "hide";
                              else return "\0";
    case MUDCOMM_BACKSTAB   : if (verboseDisplay) return "backstab";
                              else return "\0";
    case MUDCOMM_PICK       : return "pick";
    case MUDCOMM_STEAL      : return "steal";
    case MUDCOMM_BASH       : return "bash";
    case MUDCOMM_RESCUE     : if (verboseDisplay) return "rescue";
                              else return "\0";
    case MUDCOMM_KICK       : if (verboseDisplay) return "kick";
                              else return "\0";
    case MUDCOMM_FRENCH     : return "french";
    case MUDCOMM_COMB       : return "comb";
    case MUDCOMM_MASSAGE    : return "massage";
    case MUDCOMM_TICKLE     : return "tickle";
    case MUDCOMM_PRACTICE   : if (verboseDisplay) return "practice";
                              else return "\0";
    case MUDCOMM_PAT        : return "pat";
    case MUDCOMM_EXAMINE    : return "examine";
    case MUDCOMM_TAKE       : return "take";
    case MUDCOMM_INFO       : if (verboseDisplay) return "info";
                              else return "\0";
    case MUDCOMM_SPELLS     : return "spells";
    case MUDCOMM_PRACTISE   : if (verboseDisplay) return "practise";
                              else return "\0";
    case MUDCOMM_CURSE      : return "curse";
    case MUDCOMM_USE        : return "use";
    case MUDCOMM_WHERE      : if (verboseDisplay) return "where";
                              else return "\0";
    case MUDCOMM_LEVELS     : if (verboseDisplay) return "levels";
                              else return "\0";
    case MUDCOMM_REROLL     : if (verboseDisplay) return "reroll";
                              else return "\0";
    case MUDCOMM_PRAY       : if (verboseDisplay) return "pray";
                              else return "\0";
    case MUDCOMM_COLON      : if (verboseDisplay) return ":";
                              else return "\0";
    case MUDCOMM_BEG        : return "beg";
    case MUDCOMM_BLEED      : return "bleed";
    case MUDCOMM_CRINGE     : return "cringe";
    case MUDCOMM_DREAM      : return "dream";
    case MUDCOMM_FUME       : return "fume";
    case MUDCOMM_GROVEL     : return "grovel";
    case MUDCOMM_HOP        : return "hop";
    case MUDCOMM_NUDGE      : return "nudge";
    case MUDCOMM_PEER       : return "peer";
    case MUDCOMM_POINT      : return "point";
    case MUDCOMM_PONDER     : return "ponder";
    case MUDCOMM_PUNCH      : return "punch";
    case MUDCOMM_SNARL      : return "snarl";
    case MUDCOMM_SPANK      : return "spank";
    case MUDCOMM_STEAM      : return "steam";
    case MUDCOMM_TACKLE     : return "tackle";
    case MUDCOMM_TAUNT      : return "taunt";
    case MUDCOMM_THINK      : return "think";
    case MUDCOMM_WHINE      : return "whine";
    case MUDCOMM_WORSHIP    : return "worship";
    case MUDCOMM_YODEL      : return "yodel";
    case MUDCOMM_TOGGLE     : if (verboseDisplay) return "toggle";
                              else return "\0";
    case MUDCOMM_WIZLIST    : if (verboseDisplay) return "wizlist";
                              else return "\0";
    case MUDCOMM_CONSIDER   : if (verboseDisplay) return "consider";
                              else return "\0";
    case MUDCOMM_GROUP      : if (verboseDisplay) return "group";
                              else return "\0";
    case MUDCOMM_RESTORE    : if (verboseDisplay) return "restore";
                              else return "\0";
    case MUDCOMM_RETURN     : if (verboseDisplay) return "return";
                              else return "\0";
    case MUDCOMM_SWITCH     : if (verboseDisplay) return "switch";
                              else return "\0";
    case MUDCOMM_QUAFF      : return "quaff";
    case MUDCOMM_RECITE     : return "recite";
    case MUDCOMM_USERS      : if (verboseDisplay) return "users";
                              else return "\0";
    case MUDCOMM_POSE       : if (verboseDisplay) return "pose";
                              else return "\0";
    case MUDCOMM_SILENCE    : if (verboseDisplay) return "silence";
                              else return "\0";
    case MUDCOMM_WIZHELP    : if (verboseDisplay) return "wizhelp";
                              else return "\0";
    case MUDCOMM_CREDITS    : if (verboseDisplay) return "credits";
                              else return "\0";
    case MUDCOMM_DISBAND    : if (verboseDisplay) return "disband";
                              else return "\0";
    case MUDCOMM_VIS        : if (verboseDisplay) return "vis";
                              else return "\0";
    case MUDCOMM_PFLAGS     : if (verboseDisplay) return "pflags";
                              else return "\0";
    case MUDCOMM_POOFIN     : if (verboseDisplay) return "poofin";
                              else return "\0";
    case MUDCOMM_WIZMSG     : if (verboseDisplay) return "wizmsg";
                              else return "\0";
    case MUDCOMM_DISPLAY    : if (verboseDisplay) return "display";
                              else return "\0";
    case MUDCOMM_ECHOA      : if (verboseDisplay) return "echoa";
                              else return "\0";
    case MUDCOMM_DEMOTE     : if (verboseDisplay) return "demote";
                              else return "\0";
    case MUDCOMM_POOFOUT    : if (verboseDisplay) return "poofout";
                              else return "\0";
    case MUDCOMM_WIMPY      : if (verboseDisplay) return "wimpy";
                              else return "\0";
    case MUDCOMM_BALANCE    : if (verboseDisplay) return "balance";
                              else return "\0";
    case MUDCOMM_WIZLOCK    : if (verboseDisplay) return "wizlock";
                              else return "\0";
    case MUDCOMM_DEPOSIT    : if (verboseDisplay) return "deposit";
                              else return "\0";
    case MUDCOMM_WITHDRAW   : if (verboseDisplay) return "withdraw";
                              else return "\0";
    case MUDCOMM_IGNORE     : if (verboseDisplay) return "ignore";
                              else return "\0";
    case MUDCOMM_SETATTR    : if (verboseDisplay) return "setattr";
                              else return "\0";
    case MUDCOMM_TITLE      : if (verboseDisplay) return "title";
                              else return "\0";
    case MUDCOMM_AGGR       : if (verboseDisplay) return "aggr";
                              else return "\0";
    case MUDCOMM_GSAY       : if (verboseDisplay) return "gsay";
                              else return "\0";
    case MUDCOMM_CONSENT    : if (verboseDisplay) return "consent";
                              else return "\0";
    case MUDCOMM_SETBIT     : if (verboseDisplay) return "setbit";
                              else return "\0";
    case MUDCOMM_HITALL     : if (verboseDisplay) return "hitall";
                              else return "\0";
    case MUDCOMM_TRAP       : if (verboseDisplay) return "trap";
                              else return "\0";
    case MUDCOMM_MURDER     : if (verboseDisplay) return "murder";
                              else return "\0";
    case MUDCOMM_237        : if (verboseDisplay) return "#237";
                              else return "\0";
    case MUDCOMM_AUCTION    : if (verboseDisplay) return "auction";
                              else return "\0";
    case MUDCOMM_CHANNEL    : if (verboseDisplay) return "channel";
                              else return "\0";
    case MUDCOMM_FILL       : return "fill";
    case MUDCOMM_GOSSIP     : if (verboseDisplay) return "gossip";
                              else return "\0";
    case MUDCOMM_NOKILL     : if (verboseDisplay) return "nokill";
                              else return "\0";
    case MUDCOMM_PAGE       : if (verboseDisplay) return "page";
                              else return "\0";
    case MUDCOMM_MUDCOMMS   : if (verboseDisplay) return "commands";
                              else return "\0";
    case MUDCOMM_ATTRIBUTES : if (verboseDisplay) return "attributes";
                              else return "\0";
    case MUDCOMM_RULES      : if (verboseDisplay) return "rules";
                              else return "\0";
    case MUDCOMM_TRACK      : if (verboseDisplay) return "track";
                              else return "\0";
    case MUDCOMM_LISTEN     : return "listen";
    case MUDCOMM_DISARM     : if (verboseDisplay) return "disarm";
                              else return "\0";
    case MUDCOMM_PARRY      : if (verboseDisplay) return "parry";
                              else return "\0";
    case MUDCOMM_DELETE     : if (verboseDisplay) return "delete";
                              else return "\0";
    case MUDCOMM_BAN        : if (verboseDisplay) return "ban";
                              else return "\0";
    case MUDCOMM_ALLOW      : if (verboseDisplay) return "allow";
                              else return "\0";
    case MUDCOMM_PASSWORD   : if (verboseDisplay) return "password";
                              else return "\0";
    case MUDCOMM_DESCRIPTION : if (verboseDisplay) return "description";
                              else return "\0";
    case MUDCOMM_BRIBE      : return "bribe";
    case MUDCOMM_BONK       : return "bonk";
    case MUDCOMM_CALM       : return "calm";
    case MUDCOMM_RUB        : return "rub";
    case MUDCOMM_CENSOR     : return "censor";
    case MUDCOMM_CHOKE      : return "choke";
    case MUDCOMM_DROOL      : return "drool";
    case MUDCOMM_FLEX       : return "flex";
//    case MUDCOMM_JUMP       : return "jump";
    case MUDCOMM_LEAN       : return "lean";
    case MUDCOMM_MOON       : return "moon";
    case MUDCOMM_OGLE       : return "ogle";
    case MUDCOMM_PANT       : return "pant";
    case MUDCOMM_PINCH      : return "pinch";
    case MUDCOMM_PUSH       : return "push";
    case MUDCOMM_SCARE      : return "scare";
    case MUDCOMM_SCOLD      : return "scold";
    case MUDCOMM_SEDUCE     : return "seduce";
    case MUDCOMM_SHOVE      : return "shove";
    case MUDCOMM_SHUDDER    : return "shudder";
    case MUDCOMM_SHUSH      : return "shush";
    case MUDCOMM_SLOBBER    : return "slobber";
    case MUDCOMM_SMELL      : return "smell";
    case MUDCOMM_SNEER      : return "sneer";
    case MUDCOMM_SPIN       : return "spin";
    case MUDCOMM_SQUIRM     : return "squirm";
    case MUDCOMM_STOMP      : return "stomp";
    case MUDCOMM_STRANGLE   : return "strangle";
    case MUDCOMM_STRETCH    : return "stretch";
    case MUDCOMM_TAP        : return "tap";
    case MUDCOMM_TEASE      : return "tease";
    case MUDCOMM_TIPTOE     : return "tiptoe";
    case MUDCOMM_TWEAK      : return "tweak";
    case MUDCOMM_TWIRL      : return "twirl";
    case MUDCOMM_UNDRESS    : return "undress";
    case MUDCOMM_WHIMPER    : return "whimper";
    case MUDCOMM_EXCHANGE   : return "exchange";
    case MUDCOMM_RELEASE    : return "release";
    case MUDCOMM_SEARCH     : return "search";
    case MUDCOMM_FLY        : if (verboseDisplay) return "fly";
                              else return "\0";
    case MUDCOMM_LEVITATE   : if (verboseDisplay) return "levitate";
                              else return "\0";
    case MUDCOMM_SECRET     : if (verboseDisplay) return "secret";
                              else return "\0";
    case MUDCOMM_LOOKUP     : if (verboseDisplay) return "lookup";
                              else return "\0";
    case MUDCOMM_REPORT     : if (verboseDisplay) return "report";
                              else return "\0";
    case MUDCOMM_SPLIT      : if (verboseDisplay) return "split";
                              else return "\0";
    case MUDCOMM_WORLD      : if (verboseDisplay) return "world";
                              else return "\0";
    case MUDCOMM_JUNK       : if (verboseDisplay) return "junk";
                              else return "\0";
    case MUDCOMM_PETITION   : if (verboseDisplay) return "petition";
                              else return "\0";
    case MUDCOMM_DO         : if (verboseDisplay) return "do";
                              else return "\0";
    case MUDCOMM_APOSTROPHE : if (verboseDisplay) return "'";
                              else return "\0";
    case MUDCOMM_CARESS     : return "caress";
    case MUDCOMM_BURY       : if (verboseDisplay) return "bury";
                              else return "\0";
    case MUDCOMM_DONATE     : if (verboseDisplay) return "donate";
                              else return "\0";
    case MUDCOMM_DIG        : return "dig";
    case MUDCOMM_SCALE      : return "scale";
    case MUDCOMM_CUT        : return "cut";
    case MUDCOMM_SLASH      : return "slash";
    case MUDCOMM_HACK       : return "hack";
    case MUDCOMM_YELL       : return "yell";
    case MUDCOMM_RING       : return "ring";
//    case MUDCOMM_TUG        : return "tug";
    case MUDCOMM_JUMP       : return "jump";
    case MUDCOMM_TRACE      : return "trace";
    case MUDCOMM_WATER      : return "water";
    case MUDCOMM_TOUCH      : return "touch";
    case MUDCOMM_SCRATCH    : return "scratch";
    case MUDCOMM_WINCE      : return "wince";
    case MUDCOMM_TOSS       : return "toss";
    case MUDCOMM_FLAME      : return "flame";
    case MUDCOMM_ARCH       : return "arch";
    case MUDCOMM_AMAZE      : return "amaze";
    case MUDCOMM_BATHE      : return "bathe";
    case MUDCOMM_EMBRACE    : return "embrace";
    case MUDCOMM_BRB        : return "brb";
    case MUDCOMM_ACK        : return "ack";
    case MUDCOMM_CHEER      : return "cheer";
    case MUDCOMM_SNORT      : return "snort";
    case MUDCOMM_EYEBROW    : return "eyebrow";
    case MUDCOMM_BANG       : return "bang";
    case MUDCOMM_PILLOW     : return "pillow";
    case MUDCOMM_NAP        : return "nap";
    case MUDCOMM_NOSE       : return "nose";
    case MUDCOMM_RAISE      : return "raise";
    case MUDCOMM_HAND       : return "hand";
    case MUDCOMM_PULL       : return "pull";
    case MUDCOMM_TUG        : return "tug";
    case MUDCOMM_WET        : return "wet";
    case MUDCOMM_MOSH       : return "mosh";
    case MUDCOMM_WAIT       : return "wait";
    case MUDCOMM_HI5        : return "hi5";
    case MUDCOMM_ENVY       : return "envy";
    case MUDCOMM_FLIRT      : return "flirt";
    case MUDCOMM_BARK       : return "bark";
    case MUDCOMM_WHAP       : return "whap";
    case MUDCOMM_ROLL       : return "roll";
    case MUDCOMM_BLINK      : return "blink";
    case MUDCOMM_DUH        : return "duh";
    case MUDCOMM_GAG        : return "gag";
    case MUDCOMM_GRUMBLE    : return "grumble";
    case MUDCOMM_DROPKICK   : return "dropkick";
    case MUDCOMM_WHATEVER   : return "whatever";
    case MUDCOMM_FOOL       : return "fool";
    case MUDCOMM_NOOGIE     : return "noogie";
    case MUDCOMM_MELT       : return "melt";
    case MUDCOMM_SMOKE      : return "smoke";
    case MUDCOMM_WHEEZE     : return "wheeze";
    case MUDCOMM_BIRD       : return "bird";
    case MUDCOMM_BOGGLE     : return "boggle";
    case MUDCOMM_HISS       : return "hiss";
    case MUDCOMM_BITE       : return "bite";

    default : return "unrecognized type";
  }
}


//
// displayCommandList
//

void displayCommandList(const char displayAll)
{
  ulong i, numbItems = 1;
  char strn[256];

  _setbkcolor(0);
  _outtext("\n\n");

  for (i = MUDCOMM_LOWEST; i <= MUDCOMM_HIGHEST; i++)
  {
    if (getCommandStrn(i, displayAll)[0])
    {
      sprintf(strn, "  %u - %s\n", i, getCommandStrn(i, displayAll));
      _outtext(strn);

      numbItems += numbLinesStringNeeds(strn);
    }

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n\n");
}


//
// displayCommandList
//

void displayCommandList(char *searchStrn)
{
  ulong i, numbItems = 1;
  char strn[256], strn2[256];


  _setbkcolor(0);
  _outtext("\n\n");

  upstrn(searchStrn);

  for (i = MUDCOMM_LOWEST; i <= MUDCOMM_HIGHEST; i++)
  {
    strcpy(strn2, getCommandStrn(i, TRUE));
    upstrn(strn2);

    if (strstr(strn2, searchStrn))
    {
      sprintf(strn, "  %u - %s\n", i, getCommandStrn(i, TRUE));
      _outtext(strn);

      numbItems += numbLinesStringNeeds(strn);
    }

    if (numbItems >= (getScreenHeight() - 3))
    {
      displayColorString("\n&+CPress any key to continue or Q to quit..&n");
      if (toupper(getkey()) == 'Q')
      {
        _outtext("\n\n");

        return;
      }

      _outtext("\n\n");

      numbItems = 0;
    }
  }

  _outtext("\n\n");
}
