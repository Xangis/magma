//
//  File: vardef.cpp     originally part of dikuEdit
//
//  Usage: inline functions to obtain internal variable values,
//         respecting hardwired defaults
//
//  Copyright 1995-98 (C) Michael Glosenger
//

#include "fh.h"
#include "types.h"

#include "vardef.h"
#include "system.h"

extern variable *varHead;

//
// getScreenHeightVal
//

INLINEDEF long getScreenHeightVal(void)
{
  return getVarNumb(varHead, VAR_SCREENHEIGHT_NAME, NULL, DEF_SCREENHEIGHT_VAL);
}


//
// getScreenWidthVal
//

INLINEDEF long getScreenWidthVal(void)
{
  return getVarNumb(varHead, VAR_SCREENWIDTH_NAME, NULL, DEF_SCREENWIDTH_VAL);
}


//
// getIsMapZoneVal
//

INLINEDEF char getIsMapZoneVal(void)
{
  return getVarBoolVal(varHead, VAR_ISMAPZONE_NAME, NULL, DEF_ISMAPZONE_VAL);
}


//
// getSaveHowOftenVal
//

INLINEDEF long getSaveHowOftenVal(void)
{
  return getVarNumb(varHead, VAR_SAVEHOWOFTEN_NAME, NULL, DEF_SAVEHOWOFTEN_VAL);
}


//
// getEditorName
//

INLINEDEF const char *getEditorName(void)
{
  return getVarStrn(varHead, VAR_TEXTEDIT_NAME, NULL, NULL, DEF_TEXTEDIT_VAL);
}


//
// getVnumCheckVal
//

INLINEDEF char getVnumCheckVal(void)
{
  return getVarBoolVal(varHead, VAR_VNUMCHECK_NAME, NULL, DEF_VNUMCHECK_VAL);
}


//
// getZonePreVal
//

/*
INLINEDEF char getZonePreVal(void)
{
  return getVarBoolVal(varHead, VAR_ZONEPRE_NAME, NULL, DEF_ZONEPRE_VAL);
}
*/


//
// getObjAffectVal
//

INLINEDEF char getObjAffectVal(void)
{
  return getVarBoolVal(varHead, VAR_OBJAFFECT_NAME, NULL, DEF_OBJAFFECT_VAL);
}


//
// getCheckLimitsVal
//

INLINEDEF char getCheckLimitsVal(void)
{
  return getVarBoolVal(varHead, VAR_CHECKLIMITS_NAME, NULL,
                       DEF_CHECKLIMITS_VAL);
}


//
// getCheckZoneFlagsVal
//

INLINEDEF char getCheckZoneFlagsVal(void)
{
  return getVarBoolVal(varHead, VAR_CHECKZONEFLAGS_NAME, NULL,
                       DEF_CHECKZONEFLAGS_VAL);
}


//
// getFirstObjEdescKeyVal
//

INLINEDEF char getFirstObjEdescKeyVal(void)
{
  return getVarBoolVal(varHead, VAR_FIRSTOBJEDESCKEY_NAME, NULL,
                       DEF_FIRSTOBJEDESCKEY_VAL);
}


//
// getShowObjFlagsVal
//

INLINEDEF char getShowObjFlagsVal(void)
{
  return getVarBoolVal(varHead, VAR_SHOWOBJFLAGS_NAME, NULL,
                       DEF_SHOWOBJFLAGS_VAL);
}


//
// getShowMobFlagsVal
//

INLINEDEF char getShowMobFlagsVal(void)
{
  return getVarBoolVal(varHead, VAR_SHOWMOBFLAGS_NAME, NULL,
                       DEF_SHOWMOBFLAGS_VAL);
}


//
// getShowMobPosVal
//

INLINEDEF char getShowMobPosVal(void)
{
  return getVarBoolVal(varHead, VAR_SHOWMOBPOS_NAME, NULL,
                       DEF_SHOWMOBPOS_VAL);
}


//
// getShowObjVnumVal
//

INLINEDEF char getShowObjVnumVal(void)
{
  return getVarBoolVal(varHead, VAR_SHOWOBJVNUM_NAME, NULL,
                       DEF_SHOWOBJVNUM_VAL);
}


//
// getShowMobVnumVal
//

INLINEDEF char getShowMobVnumVal(void)
{
  return getVarBoolVal(varHead, VAR_SHOWMOBVNUM_NAME, NULL,
                       DEF_SHOWMOBVNUM_VAL);
}


//
// getShowMobRideFollowVal
//

INLINEDEF char getShowMobRideFollowVal(void)
{
  return getVarBoolVal(varHead, VAR_SHOWMOBRIDEFOLLOW_NAME, NULL,
                       DEF_SHOWMOBRIDEFOLLOW_VAL);
}


//
// getShowRoomExtraVal
//

INLINEDEF char getShowRoomExtraVal(void)
{
  return getVarBoolVal(varHead, VAR_SHOWROOMEXTRA_NAME, NULL,
                       DEF_SHOWROOMEXTRA_VAL);
}


//
// getShowRoomVnumVal
//

INLINEDEF char getShowRoomVnumVal(void)
{
  return getVarBoolVal(varHead, VAR_SHOWROOMVNUM_NAME, NULL,
                       DEF_SHOWROOMVNUM_VAL);
}


//
// getShowPricesAdjustedVal
//

INLINEDEF char getShowPricesAdjustedVal(void)
{
  return getVarBoolVal(varHead, VAR_SHOWPRICESADJUSTED_NAME, NULL,
                       DEF_SHOWPRICESADJUSTED_VAL);
}


//
// getInterpColorVal
//

INLINEDEF char getInterpColorVal(void)
{
  return getVarBoolVal(varHead, VAR_INTERPCOLOR_NAME, NULL,
                       DEF_INTERPCOLOR_VAL);
}


//
// getShowColorVal
//

INLINEDEF char getShowColorVal(void)
{
  return getVarBoolVal(varHead, VAR_SHOWCOLOR_NAME, NULL, DEF_SHOWCOLOR_VAL);
}


//
// getMenuPromptName
//

INLINEDEF const char *getMenuPromptName(void)
{
  return getVarStrn(varHead, VAR_MENUPROMPT_NAME, NULL, NULL,
                    DEF_MENUPROMPT_VAL);
}


//
// getMainPromptStrn
//

INLINEDEF const char *getMainPromptStrn(void)
{
  return getVarStrn(varHead, VAR_MAINPROMPT_NAME, NULL, NULL,
                    DEF_MAINPROMPT_VAL);
}


//
// getWalkCreateVal
//

INLINEDEF char getWalkCreateVal(void)
{
  return getVarBoolVal(varHead, VAR_WALKCREATE_NAME, NULL, DEF_WALKCREATE_VAL);
}


//
// getShowExitDestVal
//

INLINEDEF char getShowExitDestVal(void)
{
  return getVarBoolVal(varHead, VAR_SHOWEXITDEST_NAME, NULL,
                       DEF_SHOWEXITDEST_VAL);
}


//
// getShowExitFlagsVal
//

INLINEDEF char getShowExitFlagsVal(void)
{
  return getVarBoolVal(varHead, VAR_SHOWEXITFLAGS_NAME, NULL,
                       DEF_SHOWEXITFLAGS_VAL);
}


//
// getRoomStartVal
//

INLINEDEF long getRoomStartVal(void)
{
  return getVarNumb(varHead, VAR_STARTROOM_NAME, NULL, DEF_STARTROOM_VAL);
}


//
// getRoomStartActiveVal
//

INLINEDEF char getStartRoomActiveVal(void)
{
  return getVarBoolVal(varHead, VAR_SROOMACTIVE_NAME, NULL,
                       DEF_SROOMACTIVE_VAL);
}


//
// getShowMenuInfoVal
//

INLINEDEF char getShowMenuInfoVal(void)
{
  return getVarBoolVal(varHead, VAR_SHOWMENUINFO_NAME, NULL,
                       DEF_SHOWMENUINFO_VAL);
}


//
// getMainZoneNameStrn
//

INLINEDEF const char *getMainZoneNameStrn(void)
{
  return getVarStrn(varHead, VAR_MAINZONENAME_NAME, NULL, NULL,
                    DEF_MAINZONENAME_VAL);
}


//
// getLastCommandStrn
//

INLINEDEF const char *getLastCommandStrn(void)
{
  return getVarStrn(varHead, VAR_LASTCOMMAND_NAME, NULL, NULL,
                    DEF_LASTCOMMAND_VAL);
}


//
// getCheckSaveVal
//

INLINEDEF char getCheckSaveVal(void)
{
  return getVarBoolVal(varHead, VAR_CHECKSAVE_NAME, NULL,
                       DEF_CHECKSAVE_VAL);
}


//
// getCheckLoneRoomVal
//

INLINEDEF char getCheckLoneRoomVal(void)
{
  return getVarBoolVal(varHead, VAR_CHECKLONEROOM_NAME, NULL,
                       DEF_CHECKLONEROOM_VAL);
}


//
// getCheckMissingKeysVal
//

INLINEDEF char getCheckMissingKeysVal(void)
{
  return getVarBoolVal(varHead, VAR_CHECKMISSINGKEYS_NAME, NULL,
                       DEF_CHECKMISSINGKEYS_VAL);
}


//
// getCheckLoadedVal
//

INLINEDEF char getCheckLoadedVal(void)
{
  return getVarBoolVal(varHead, VAR_CHECKLOADED_NAME, NULL,
                       DEF_CHECKLOADED_VAL);
}


//
// getCheckRoomVal
//

INLINEDEF char getCheckRoomVal(void)
{
  return getVarBoolVal(varHead, VAR_CHECKROOM_NAME, NULL,
                       DEF_CHECKROOM_VAL);
}


//
// getCheckExitVal
//

INLINEDEF char getCheckExitVal(void)
{
  return getVarBoolVal(varHead, VAR_CHECKEXIT_NAME, NULL,
                       DEF_CHECKEXIT_VAL);
}


//
// getCheckExitDescVal
//

INLINEDEF char getCheckExitDescVal(void)
{
  return getVarBoolVal(varHead, VAR_CHECKEXITDESC_NAME, NULL,
                       DEF_CHECKEXITDESC_VAL);
}


//
// getCheckObjVal
//

INLINEDEF char getCheckObjVal(void)
{
  return getVarBoolVal(varHead, VAR_CHECKOBJ_NAME, NULL,
                       DEF_CHECKOBJ_VAL);
}


//
// getCheckObjValuesVal
//

INLINEDEF char getCheckObjValuesVal(void)
{
  return getVarBoolVal(varHead, VAR_CHECKOBJVAL_NAME, NULL,
                       DEF_CHECKOBJVAL_VAL);
}


//
// getCheckObjDescVal
//

INLINEDEF char getCheckObjDescVal(void)
{
  return getVarBoolVal(varHead, VAR_CHECKOBJDESC_NAME, NULL,
                       DEF_CHECKOBJDESC_VAL);
}


//
// getCheckMobDescVal
//

INLINEDEF char getCheckMobDescVal(void)
{
  return getVarBoolVal(varHead, VAR_CHECKMOBDESC_NAME, NULL,
                       DEF_CHECKMOBDESC_VAL);
}


//
// getCheckMobVal
//

INLINEDEF char getCheckMobVal(void)
{
  return getVarBoolVal(varHead, VAR_CHECKMOB_NAME, NULL,
                       DEF_CHECKMOB_VAL);
}


//
// getCheckEdescVal
//

INLINEDEF char getCheckEdescVal(void)
{
  return getVarBoolVal(varHead, VAR_CHECKEDESC_NAME, NULL,
                       DEF_CHECKEDESC_VAL);
}


//
// getNoSaveonCheckErrVal
//

INLINEDEF char getNoSaveonCheckErrVal(void)
{
  return getVarBoolVal(varHead, VAR_NOSAVEONCHECKERR_NAME, NULL,
                       DEF_NOSAVEONCHECKERR_VAL);
}


//
// getPauseCheckScreenfulVal
//

INLINEDEF char getPauseCheckScreenfulVal(void)
{
  return getVarBoolVal(varHead, VAR_PAUSECHECKSCREENFUL_NAME, NULL,
                       DEF_PAUSECHECKSCREENFUL_VAL);
}


//
// getCheckFlagsVal
//

INLINEDEF char getCheckFlagsVal(void)
{
  return getVarBoolVal(varHead, VAR_CHECKFLAGS_NAME, NULL,
                       DEF_CHECKFLAGS_VAL);
}


//
// getSaveCheckLogVal
//

INLINEDEF char getSaveCheckLogVal(void)
{
  return getVarBoolVal(varHead, VAR_SAVECHECKLOG_NAME, NULL,
                       DEF_SAVECHECKLOG_VAL);
}


//
// getEditUneditableFlagsVal
//

INLINEDEF char getEditUneditableFlagsVal(void)
{
  return getVarBoolVal(varHead, VAR_EDITUNEDITABLEFLAGS_NAME, NULL,
                       DEF_EDITUNEDITABLEFLAGS_VAL);
}


//
// getAllMobs
//

INLINEDEF char getAllMobsVal(void)
{
  return getVarBoolVal(varHead, VAR_GETALLMOBS_NAME, NULL, DEF_GETALLMOBS_VAL);
}


//
// getIgnoreZoneS
//

INLINEDEF char getIgnoreZoneSVal(void)
{
  return getVarBoolVal(varHead, VAR_IGNOREZONES_NAME, NULL,
                       DEF_IGNOREZONES_VAL);
}


//
// getCheckMobClassEq
//

INLINEDEF char getCheckMobClassEqVal(void)
{
  return getVarBoolVal(varHead, VAR_CHECKMOBCLASSEQ_NAME, NULL,
                       DEF_CHECKMOBCLASSEQ_VAL);
}


//
// getCheckMobRaceEq
//

INLINEDEF char getCheckMobRaceEqVal(void)
{
  return getVarBoolVal(varHead, VAR_CHECKMOBRACEEQ_NAME, NULL,
                       DEF_CHECKMOBRACEEQ_VAL);
}


//
// getCheckMobSexEq
//

INLINEDEF char getCheckMobSexEqVal(void)
{
  return getVarBoolVal(varHead, VAR_CHECKMOBSEXEQ_NAME, NULL,
                       DEF_CHECKMOBSEXEQ_VAL);
}


//
// getCheckObjMaterial(void)
//

INLINEDEF char getCheckObjMaterialVal(void)
{
  return getVarBoolVal(varHead, VAR_CHECKOBJMATERIAL_NAME, NULL,
                       DEF_CHECKOBJMATERIAL_VAL);
}


//
// getCheckGuildStuffVal(void)
//

INLINEDEF char getCheckGuildStuffVal(void)
{
  return getVarBoolVal(varHead, VAR_CHECKGUILDSTUFF_NAME, NULL,
                       DEF_CHECKGUILDSTUFF_VAL);
}


//
// getNegDestOutofZoneVal(void)
//

INLINEDEF char getNegDestOutofZoneVal(void)
{
  return getVarBoolVal(varHead, VAR_NEGDESTOUTOFZONE_NAME, NULL,
                       DEF_NEGDESTOUTOFZONE_VAL);
}


//
// getSaveEveryXCommandsVal(void)
//

INLINEDEF char getSaveEveryXCommandsVal(void)
{
  return getVarBoolVal(varHead, VAR_SAVEEVERYXCOMMANDS_NAME, NULL,
                       DEF_SAVEEVERYXCOMMANDS_VAL);
}
