#ifndef _VARDEF_H_

#define VAR_TEXTEDIT_NAME   "TEXTEDIT"  // name of external desc editor
#ifdef __UNIX__
#  define DEF_TEXTEDIT_VAL    "vi"
#else
#  define DEF_TEXTEDIT_VAL    "EDIT"
#endif

#define VAR_SCREENWIDTH_NAME "SCREENWIDTH"
#define DEF_SCREENWIDTH_VAL  DEFAULT_SCREEN_WIDTH

#define VAR_SCREENHEIGHT_NAME "SCREENHEIGHT"
#define DEF_SCREENHEIGHT_VAL  DEFAULT_SCREEN_HEIGHT

#define VAR_ISMAPZONE_NAME "ISMAPZONE"
#define DEF_ISMAPZONE_VAL  FALSE

#define VAR_VNUMCHECK_NAME  "VNUMCHECK"  // check all vnums for validity?
#define DEF_VNUMCHECK_VAL   FALSE

#define VAR_OBJAFFECT_NAME  "OBJAFFECT"  // if TRUE, user can edit obj affects
#define DEF_OBJAFFECT_VAL   TRUE

#define VAR_CHECKLIMITS_NAME  "CHECKLIMITS"  // check zone limits when loading?
#define DEF_CHECKLIMITS_VAL   TRUE

#define VAR_CHECKZONEFLAGS_NAME  "CHECKZONEFLAGS"  // check zone flags of rooms
#define DEF_CHECKZONEFLAGS_VAL  TRUE               // against zone's?

#define VAR_FIRSTOBJEDESCKEY_NAME  "FIRSTOBJEDESCKEY"  // first edesc for obj..
#define DEF_FIRSTOBJEDESCKEY_VAL   TRUE

#define VAR_SHOWOBJFLAGS_NAME "SHOWOBJFLAGS"  // show obj flags next to name?
#define DEF_SHOWOBJFLAGS_VAL  TRUE

#define VAR_SHOWMOBFLAGS_NAME "SHOWMOBFLAGS"  // show mob flags next to name?
#define DEF_SHOWMOBFLAGS_VAL  TRUE

#define VAR_SHOWMOBPOS_NAME "SHOWMOBPOS"  // show mob pos after name?
#define DEF_SHOWMOBPOS_VAL  TRUE

#define VAR_SHOWOBJVNUM_NAME "SHOWOBJVNUM"  // show obj vnum after name?
#define DEF_SHOWOBJVNUM_VAL  TRUE

#define VAR_SHOWMOBVNUM_NAME "SHOWMOBVNUM"  // show mob vnum after name?
#define DEF_SHOWMOBVNUM_VAL  TRUE

#define VAR_SHOWMOBRIDEFOLLOW_NAME "SHOWMOBRIDEFOLLOW"  // show ride/follow
#define DEF_SHOWMOBRIDEFOLLOW_VAL  TRUE

#define VAR_SHOWROOMEXTRA_NAME "SHOWROOMEXTRA"  // show sector/flag info?
#define DEF_SHOWROOMEXTRA_VAL  TRUE

#define VAR_SHOWROOMVNUM_NAME "SHOWROOMVNUM"  // show room vnum?
#define DEF_SHOWROOMVNUM_VAL  TRUE

#define VAR_SHOWPRICESADJUSTED_NAME "SHOWPRICESADJUSTED"
#define DEF_SHOWPRICESADJUSTED_VAL  TRUE

#define VAR_INTERPCOLOR_NAME "INTERPCOLOR"   // interpret diku color codes?
#define DEF_INTERPCOLOR_VAL  TRUE

#define VAR_SHOWCOLOR_NAME "SHOWCOLOR"       // show diku color codes?
#define DEF_SHOWCOLOR_VAL  FALSE

#define VAR_MENUPROMPT_NAME "MENUPROMPT"   // stores menu prompt
#define DEF_MENUPROMPT_VAL  "&+CYour choice, sir? &n"

#define VAR_MAINPROMPT_NAME "MAINPROMPT"   // if == default, normal prompt is
#define DEF_MAINPROMPT_VAL  "default"      // shown

#define VAR_WALKCREATE_NAME "WALKCREATE"       // create rooms while walking
#define DEF_WALKCREATE_VAL  FALSE              // around?

#define VAR_SHOWEXITDEST_NAME "SHOWEXITDEST"  // show vnum of dest room next to
#define DEF_SHOWEXITDEST_VAL  FALSE           // exit name?

#define VAR_SHOWEXITFLAGS_NAME "SHOWEXITFLAGS"  // show exit flags next to dir?
#define DEF_SHOWEXITFLAGS_VAL  TRUE

#define VAR_STARTROOM_NAME "STARTROOM"  // room user starts in
#define DEF_STARTROOM_VAL  0

#define VAR_SROOMACTIVE_NAME "SAVESTARTROOM"  // save and use start room?
#define DEF_SROOMACTIVE_VAL  TRUE

#define VAR_SHOWMENUINFO_NAME "SHOWMENUINFO"  // show numb extra descs etc
#define DEF_SHOWMENUINFO_VAL  TRUE

#define VAR_MAINZONENAME_NAME "MAINZONENAME"  // main name of zone
#define DEF_MAINZONENAME_VAL  "\0"

#define VAR_LASTCOMMAND_NAME "LASTCOMMAND"  // last command typed
#define DEF_LASTCOMMAND_VAL  "\0"

#define VAR_SAVEEVERYXCOMMANDS_NAME "SAVEEVERYXCOMMANDS"
#define DEF_SAVEEVERYXCOMMANDS_VAL  FALSE

#define VAR_SAVEHOWOFTEN_NAME "SAVEHOWOFTEN"
#define DEF_SAVEHOWOFTEN_VAL  10

#define VAR_EDITUNEDITABLEFLAGS_NAME "EDITUNEDITABLEFLAGS"  // allow user to
#define DEF_EDITUNEDITABLEFLAGS_VAL  FALSE                   // edit any flag?

#define VAR_GETALLMOBS_NAME "GETALLMOBS"  // when typing "get/drop all",
#define DEF_GETALLMOBS_VAL  FALSE         // include mobs?  (not imped)

#define VAR_IGNOREZONES_NAME "IGNOREZONES"  // ignore the "S" in a zone
#define DEF_IGNOREZONES_VAL  FALSE          // file?  (stop on EOF instead)

#define VAR_NEGDESTOUTOFZONE_NAME "NEGDESTOUTOFZONE"  // is a negative dest.
#define DEF_NEGDESTOUTOFZONE_VAL  TRUE                // considered out of zone?

// check-related variables

#define VAR_CHECKMISSINGKEYS_NAME "CHECKMISSINGKEYS"  // check to see if keys
#define DEF_CHECKMISSINGKEYS_VAL  TRUE                // for doors/objs are
                                                      // loaded?

#define VAR_CHECKLOADED_NAME "CHECKLOADED"  // check all obj/mob types to
#define DEF_CHECKLOADED_VAL  FALSE          // make sure at least one is loaded?

#define VAR_CHECKSAVE_NAME "CHECKSAVE"  // run "check" with each save?
#define DEF_CHECKSAVE_VAL  FALSE

#define VAR_NOSAVEONCHECKERR_NAME "NOSAVEONCHECKERR"  // if TRUE, abort
#define DEF_NOSAVEONCHECKERR_VAL  TRUE                // saving if err found

#define VAR_SAVECHECKLOG_NAME "SAVECHECKLOG"  // write errors to CHECK.LOG?
#define DEF_SAVECHECKLOG_VAL  TRUE

#define VAR_PAUSECHECKSCREENFUL_NAME "PAUSECHECKSCREENFUL" // if TRUE, pause
#define DEF_PAUSECHECKSCREENFUL_VAL  TRUE

#define VAR_CHECKLONEROOM_NAME "CHECKLONEROOM"  // check for rooms with no
#define DEF_CHECKLONEROOM_VAL TRUE              // exits to/from them?

#define VAR_CHECKROOM_NAME "CHECKROOM"  // check various room thingies?
#define DEF_CHECKROOM_VAL  TRUE

#define VAR_CHECKEXIT_NAME "CHECKEXIT"  // check various exit thingies?
#define DEF_CHECKEXIT_VAL  TRUE

#define VAR_CHECKEXITDESC_NAME "CHECKEXITDESC"  // check for exit descs?
#define DEF_CHECKEXITDESC_VAL  FALSE

#define VAR_CHECKOBJ_NAME "CHECKOBJ"  // check various obj thingies?
#define DEF_CHECKOBJ_VAL  TRUE

#define VAR_CHECKOBJVAL_NAME "CHECKOBJVAL"  // check obj values based on obj
#define DEF_CHECKOBJVAL_VAL  TRUE           // type?

#define VAR_CHECKOBJDESC_NAME "CHECKOBJDESC"  // check for obj edescs?
#define DEF_CHECKOBJDESC_VAL  TRUE

#define VAR_CHECKMOBDESC_NAME "CHECKMOBDESC"  // check for mob descs?
#define DEF_CHECKMOBDESC_VAL  TRUE

#define VAR_CHECKMOB_NAME "CHECKMOB"  // check various mob thingies?
#define DEF_CHECKMOB_VAL  TRUE

#define VAR_CHECKEDESC_NAME "CHECKEDESC"  // check for edescs with no
#define DEF_CHECKEDESC_VAL  TRUE          // keywords or actual descs?

#define VAR_CHECKFLAGS_NAME "CHECKFLAGS"  // check for uneditable flags
#define DEF_CHECKFLAGS_VAL  TRUE          // set to non-defaults?

#define VAR_CHECKMOBCLASSEQ_NAME "CHECKMOBCLASSEQ"  // check mob class vs.
#define DEF_CHECKMOBCLASSEQ_VAL  FALSE              // eq flags?

#define VAR_CHECKMOBRACEEQ_NAME "CHECKMOBRACEEQ"  // check mob race vs.
#define DEF_CHECKMOBRACEEQ_VAL  FALSE             // eq flags?

#define VAR_CHECKMOBSEXEQ_NAME "CHECKMOBSEXEQ"  // check mob sex vs.
#define DEF_CHECKMOBSEXEQ_VAL  FALSE            // eq flags?

#define VAR_CHECKOBJMATERIAL_NAME "CHECKOBJMATERIAL"  // check for out-of-
#define DEF_CHECKOBJMATERIAL_VAL  TRUE                // range obj materials?

#define VAR_CHECKGUILDSTUFF_NAME "CHECKGUILDSTUFF"  // check "guild stuff"?
#define DEF_CHECKGUILDSTUFF_VAL  FALSE

#define _VARDEF_H_
#endif
