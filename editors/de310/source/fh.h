// FH.H - forward declarations for DikuEdit, the all-purpose handy-dandy
//        Diku area editor

#ifndef _FH_H_

#include <stdio.h>
#include <string.h>
//#include "unistd.h"

#ifdef _WIN32
#  include <mapiutil.h>
#  include <mapiwin.h>
#  include <wincon.h>
#  include "graphcon.h"
#endif

#include <conio.h>

struct rccoord
{
  short row;
  short col;
};

void _clearscreen(int, int);
struct rccoord _gettextposition(void);
void _settextcolor(int);
int _gettextcolor(void);
int _getbkcolor(void);
void _settextposition(int, int);
struct rccoord _gettextposition(void);
void _outtext(char *);
void _setbkcolor(int);
#define utoa(a,b,c) (sprintf((b), "%(c)u", (a)))


#include "types.h"
#include "boolean.h"
#include "inline.h"
#include "flagdef.h"
#include "misc/strnnode.h"
#include "command/command.h"
#include "command/var.h"
#include "command/alias.h"
#include "room/room.h"
#include "misc/loaded.h"
#include "zone/dispzone.h"
#include "zone/readzon.h"
#include "zone/writezon.h"

// alias.cpp

void unaliasCmd(const char *, alias **);
char addAlias(alias **, char *, char *);
void addAliasArgs(char *, char, char, alias **);
void aliasCmd(char *, char, alias **);
char aliasHasArgs(char *);
char *expandAlias(char *, char *, char *);
char *replaceAliasVars(char *, char *);
void checkAliases(char *, alias *);

// check.cpp

void outCheckError(const char *, FILE *);
ulong checkFlags(FILE *, const ulong, const flagDef *, const char *, const char *, const ulong, const char);
ulong checkAllFlags(FILE *);
ulong checkRooms(FILE *);
ulong checkLoneRooms(FILE *);
char checkExitKeyLoaded(const roomExit *);
ulong checkMissingKeys(FILE *);
ulong checkLoaded(FILE *);
char checkExit(const roomExit *);
ulong checkExits(FILE *);
char checkExitDesc(const roomExit *);
ulong checkExitDescs(FILE *);
ulong checkObjects(FILE *);
ulong checkObjectValueRange(const dikuObject *, const uchar, const long, const long, const long, FILE *);
ulong checkObjValValidLevel(const dikuObject *, const char, FILE *);
ulong checkObjValValidSpellType(const dikuObject *, const char, FILE *);
void checkOAWLvWBredund(const dikuObject *, const char *, FILE *);
void checkOAWLvWBredund2(const dikuObject *, const char *, FILE *);
ulong checkObjArmorWearLocvsWearBits(const dikuObject *, FILE *);
ulong checkObjectValues(FILE *);
ulong checkObjMaterial(FILE *);
ulong checkMobs(FILE *);
ulong checkObjectDescs(FILE *);
ulong checkMobDescs(FILE *);
char checkEdesc(const extraDesc *);
ulong checkAllEdescs(FILE *);
char checkAll(void);

// clone.cpp

void cloneSumpn(const char *);
void cloneRoom(const ulong, const ulong);
void cloneObjectType(const ulong, const ulong);
void cloneMobType(const ulong, const ulong);

// command.cpp

void addCommand(command **, const char *, const uchar);
void checkCommands(const char *, command *, const char *, const void (*)(const uchar, const char *), const char);

// config.cpp

void displayEditConfigMenu(void);
char interpEditConfigMenu(const usint);
void editConfig(void);

// copydef.cpp

void copyDefault(const char *);

// copydesc.cpp

void copyRoomDesc(const ulong, const ulong);
void copyMobDesc(const ulong, const ulong);

// create.cpp

void createSumpn(const char *);

// crtdesc.cpp

extraDesc *createExtraDesc(extraDesc **, const char *);

// crtedit.cpp

void createEdit(const char *);
void createEditExit(const char *);

// crtexit.cpp

char createExit(roomExit **, const char);
char createExit(const ulong, const ulong, const char);

// crtmob.cpp

dikuMob *createMobType(const char, const long);
char createMobHereinRoom(dikuRoom *, dikuMob *, const ulong);

// crtmobhu.cpp

void createMobHereUser(void);
void createMobHereStrn(const char *);

// crtmobtu.cpp

long createMobTypeUser(const char *);

// crtobj.cpp

dikuObject *createObjectType(const char, const long);
void createObjectHereinRoom(dikuRoom *, dikuObject *, const ulong);
objectHere *createObjHere(const ulong, const char);

// crtobjhu.cpp

void createObjectHereUser(void);
void createObjectHereStrn(const char *);

// crtobjtu.cpp

long createObjectTypeUser(const char *);

// crtrexit.cpp

long createRoomExitPrompt(dikuRoom *, const ulong);
long createRoomExit(dikuRoom *, const long);
void preCreateExit(const char *, dikuRoom *);

// crtroom.cpp

INLINEDEC dikuRoom *createRoom(const char, const long);

// crtroomu.cpp

char createRoomPrompt(const char *);

// default.cpp

void readDefaultsFromFiles(const char *);
void writeDefaultFiles(const char *);
void editDefaultRoom(void);
void editDefaultObject(void);
void editDefaultMob(void);
void editDefaultExit(void);
void deleteDefaultRoom(void);
void deleteDefaultObject(void);
void deleteDefaultMob(void);
void deleteDefaultExit(void);

// delete.cpp

void deleteSumpn(const char *);

// delexitu.cpp

void deleteExitPromptPrompt(dikuRoom *, const ulong);
void deleteExitPrompt(dikuRoom *, const char);
void preDeleteExitPrompt(const char *, dikuRoom *);

// delmobhu.cpp

void deleteMobHereUser(const char *, dikuRoom *);

// delmobt.cpp

void deleteMobType(dikuMob *, const char);
void deleteMobTypeinList(dikuMob **, dikuMob *, const char);
void deleteMobTypeAssocLists(dikuMob *);

// delmobtu.cpp

char checkDeleteMobsofType(const dikuMob *);
void deleteMobTypeUser(const char *);
void deleteMobTypeUserPrompt(void);

// delobjh.cpp

void deleteObjHereList(objectHere *, const char);
void deleteObjHere(objectHere *, const char);
void deleteObjHereinList(objectHere **, objectHere *, const char);
void removeObjHerefromList(objectHere **, objectHere *);
void deleteAllObjHereofTypeInList(objectHere **, const dikuObject *, const char);
void deleteAllObjHeresofType(const dikuObject *, const char);

// delobjhu.cpp

void deleteObjectHereUser(const char *, dikuRoom *);

// delobjt.cpp

void deleteObjectType(dikuObject *, const char);
void deleteObjectTypeinList(dikuObject **, dikuObject *, const char);
void deleteObjectTypeAssocLists(dikuObject *);

// delobjtu.cpp

char checkDeleteObjectsofType(const dikuObject *);
void deleteObjectTypeUser(const char *);
void deleteObjectTypeUserPrompt(void);

// delqstu.cpp

void deleteQuestUser(const char *);
void deleteQuestUserPrompt(void);

// delroomu.cpp

char deleteRoomCommon(const long);
void deleteRoomUser(const char *);

// delshpu.cpp

void deleteShopUser(const char *);
void deleteShopUserPrompt(void);

// dispflag.cpp

void displayFlagMenu(const ulong, const flagDef *, const char *, const char *, uchar **, uchar **, uchar *);
void updateSingleFlag(const ulong, const uchar, const char, const uchar, const uchar *, const uchar *, const uchar);
void updateFlags(const ulong, const char, const uchar, const uchar *, const uchar *, const uchar, const uchar);

// display.cpp

void displayColorString(const char *, const char = TRUE);
void displayStringNodes(stringNode *);
INLINEDEC void displayPrompt(void);
char *remColorCodes(char *);

// dispmisc.cpp

void displayObjSizeList(void);
void displayCraftsmanshipList(void);
void displayArmorTypeList(void);
void displayArmorWornList(void);
void displayArmorThicknessList(void);
void displayShieldTypeList(void);
void displayShieldShapeList(void);
void displayShieldSizeList(void);
void displayMaterialList(void);
void displayInstrumentList(void);
void displaySpellList(const char * = NULL);
void displayLiquidList(void);
void displayDirectionList(void);
void displayWeaponDamageList(void);
void displayWeaponTypeList(void);
void displayMissileTypeList(void);
void displayLangTypeList(void);
void listCommands(command *);
char checkPause(ulong &);
void showKeyUsed(const char *);

// dispmob.cpp

void displayMobTypeList(void);
void displayMobTypeList(const char *);
void displayMobHereList(void);
void displayMobEqPos(const mobHere *, const char, const char *);
void displayMobEquip(const mobHere *);
void displayMobMisc(const mobHere *);
void displayMobPositionList(void);
void displayMobSexList(void);
void displayMobClassList(void);
void displayMobHometownList(void);
void displayMobSpeciesList(void);
void displayMobInfo(const char *, dikuRoom *);

// dispobj.cpp

void displayObjectTypeList(void);
void displayObjectTypeList(const char *, const char);
void displayObjectHereList(void);
char displayEntireObjectHereList(objectHere *, const char *, ulong *);
char displayEntireObjectHereList(objectHere *, const char *, ulong *, const char *, const long, char *);
char displayEntireObjectHereList(objectHere *, char *, ulong *, long, long, char *);
void displayObjTypeList(void);
void displayObjApplyTypeList(void);
void displayObjTrapDamList(void);
void displayObjectInfo(const char *, dikuRoom *);

// disproom.cpp

void displayRoomList(void);
void displayRoomList(const char *);
void displayRoomFlagsandSector(const dikuRoom *);
void displayExitList(const char *);
void displayDoorState(const roomExit *);
void displayExitDest(const roomExit *);
void displayRoom(const dikuRoom *);
void displayRoomInfo(const char *);
void displayRoomManaTypeList(void);
char *getRoomExitInfo(const roomExit *, const char *, char *);
void displayRoomExitInfo(const dikuRoom *);

// dispzone.cpp

void displayZoneResetModeList(void);
void addZoneExittoList(zoneExit **, roomExit *, char, dikuRoom *);
void displayZoneExits(void);
void displayZoneInfo(void);

// edesc.cpp

INLINEDEC ulong getNumbExtraDescs(extraDesc *);
extraDesc *copyExtraDescs(extraDesc *);
void deleteExtraDescs(extraDesc *);
extraDesc *copyOneExtraDesc(const extraDesc *);
void deleteOneExtraDesc(extraDesc *);
char compareExtraDescs(extraDesc *, extraDesc *);
char compareOneExtraDesc(extraDesc *, extraDesc *);
extraDesc *getEdescinList(extraDesc *, const char *);

// editable.cpp

editableListNode *createEditableList(dikuRoom *);
char checkEditableList(const char *, editableListNode *, char *, editableListNode **, char);
editableListNode *copyEditableList(editableListNode *);
void deleteEditableList(editableListNode *);
void deleteEditableinList(editableListNode **, editableListNode *);

// editchck.cpp

void displayEditCheckMenu(void);
char interpEditCheckMenu(usint);
void editCheck(void);

// editdesc.cpp

void displayEditExtraDescMenu(const extraDesc *);
char interpEditExtraDescMenu(const usint, extraDesc *);
uchar editExtraDesc(extraDesc *);
void deleteExtraDescUser(extraDesc **, uchar *, const char, void *);

// editdisp.cpp

void displayEditDisplayMenu(void);
char interpEditDisplayMenu(usint);
void editDisplay(void);

// editent.cpp

void editEntity(const char *, dikuRoom *);

// editexit.cpp

void displayEditExitStateMenu(const dikuRoom *, const roomExit *, const char *);
char interpEditExitStateMenu(const char, roomExit *);
void editExitState(const dikuRoom *, roomExit *, const char *);
void displayEditExitMenu(const dikuRoom *, const char *, const roomExit *);
char interpEditExitMenu(const char, const dikuRoom *, roomExit *, const char *);
void editExit(dikuRoom *, roomExit **, const char *, const uchar);
void preEditExit(const char *, dikuRoom *);

// editflag.cpp

char canEditFlag(const flagDef &, long);
char isFlagValid(const flagDef &, ulong);
uchar checkCommonFlagKeys(const usint, const flagDef *, ulong *, const uchar, const uchar *, const uchar *, const uchar, const uchar, const uchar);
uchar checkCommonFlagKeys(const usint, const flagDef *, long *, const uchar, const uchar *, const uchar *, const uchar, const uchar, const uchar);
char interpEditFlags(usint, const flagDef *, ulong *, const uchar, const uchar *, const uchar *, const uchar, ulong *);
char interpEditFlags(usint, const flagDef *, long *, const uchar, const uchar *, const uchar *, const uchar, ulong *templates);
void editFlags(const flagDef *, ulong *, const char *, const char *, ulong *, uchar);
void editFlags(const flagDef *, long *, const char *, const char *, ulong *, uchar);

// editichk.cpp

void displayEditMiscCheckMenu(void);
char interpEditMiscCheckMenu(usint);
void editMiscCheck(void);

// editmchk.cpp

void displayEditMobCheckMenu(void);
char interpEditMobCheckMenu(usint);
void editMobCheck(void);

// editmflg.cpp

void editMobActionFlags(dikuMob *);
void editMobAffect1Flags(dikuMob *);
void editMobAffect2Flags(dikuMob *);
void editMobAffect3Flags(dikuMob *);
void editMobAffect4Flags(dikuMob *);

// editmms2.cpp

void displayEditMobMisc2Menu(const dikuMob *);
char interpEditMobMisc2Menu(usint, dikuMob *);
void editMobMisc2(dikuMob *);

// editmms3.cpp

void displayEditMobMisc3Menu(const dikuMob *);
char interpEditMobMisc3Menu(usint, dikuMob *);
void editMobMisc3(dikuMob *);

// editmmsc.cpp

void displayEditMobMiscMenu(const dikuMob *);
char interpEditMobMiscMenu(usint, dikuMob *, dikuMob *);
void editMobMisc(dikuMob *, dikuMob *);

// editmob.cpp

void editMobTypeDesc(dikuMob *, const char);
void displayEditMobTypeMenu(const dikuMob *);
char interpEditMobTypeMenu(usint, dikuMob *, dikuMob *);
dikuMob *realEditMobType(dikuMob *, const char = TRUE);
void editMobType(dikuMob *, const char = TRUE);
char jumpMob(const dikuMob *, ulong *);

// editmobu.cpp

void editMobTypeStrn(const char *);
void editMobTypePrompt(void);

// editobj.cpp

void displayEditObjTypeMenu(dikuObject *);
char interpEditObjTypeMenu(usint, dikuObject *, dikuObject *, char *);
dikuObject *realEditObjType(dikuObject *, const char = TRUE);
void editObjType(dikuObject *, const char = TRUE);
char jumpObj(const dikuObject *, ulong *);

// editobju.cpp

void editObjectTypeStrn(const char *);
void editObjectTypePrompt(void);

// editochk.cpp

void displayEditObjCheckMenu(void);
char interpEditObjCheckMenu(usint);
void editObjCheck(void);

// editoexd.cpp

void displayEditObjExtraDescMenu(dikuObject *, extraDesc *, uchar *);
char interpEditObjExtraDescMenu(usint, dikuObject *, extraDesc **, uchar *, char *);
char editObjExtraDesc(dikuObject *, char *);

// editofl2.cpp

void editObjAffect1Flags(dikuObject *);
void editObjAffect2Flags(dikuObject *);
void editObjAffect3Flags(dikuObject *);
void editObjAffect4Flags(dikuObject *);

// editoflg.cpp

void editObjExtraFlags(dikuObject *, dikuObject *);
void editObjExtra2Flags(dikuObject *, dikuObject *);
void editObjWearFlags(dikuObject *, dikuObject *);
void editObjAntiFlags(dikuObject *, dikuObject *);
void editObjAnti2Flags(dikuObject *, dikuObject *);

// editoms2.cpp

void displayEditObjMisc2Menu(const dikuObject *);
char interpEditObjMisc2Menu(usint, dikuObject *);
void editObjMisc2(dikuObject *);

// editomsc.cpp

void displayEditObjMiscMenu(const dikuObject *);
void editObjValueField(dikuObject *, uchar);
void editObjApplyType(dikuObject *, uchar);
void editObjApplyValue(dikuObject *, uchar);
char interpEditObjMiscMenu(usint, dikuObject *);
void editObjMisc(dikuObject *);

// editotrp.cpp

void displayEditObjTrapInfoMenu(const dikuObject *);
char interpEditObjTrapInfoMenu(usint, dikuObject *);
void editObjTrapInfo(dikuObject *);

// editqst.cpp

void displayEditQuestMenu(quest *, const char *, const long);
char interpEditQuestMenu(usint, quest *, const char *, const long, dikuMob *);
dikuMob *realEditQuest(quest *, const char *, const long, dikuMob *, const char);
void editQuest(quest *, char *, long, dikuMob *, const char);
char jumpQuest(ulong *, ulong);

// editqsti.cpp

void displayEditQuestItemMenu(questItem *, const char *, const long, const char);
char interpEditQuestItemMenu(const usint, questItem *, const char *, const long, const char);
void editQuestItem(questItem *, const char *, const long, const char);

// editqstm.cpp

void displayEditQuestMessageMenu(questMessage *);
char interpEditQuestMessageMenu(char, questMessage *);
uchar editQuestMessage(questMessage *);

// editqstq.cpp

void displayEditQuestQuestMenu(questQuest *, const char *, const long);
char interpEditQuestQuestMenu(char, questQuest *, const char *, const long);
uchar editQuestQuest(questQuest *, const char *, const long);

// editqstu.cpp

void editQuestStrn(const char *);
void editQuestPrompt(void);

// editrchk.cpp

void displayEditRoomCheckMenu(void);
char interpEditRoomCheckMenu(const usint);
void editRoomCheck(void);

// editrexd.cpp

void displayEditRoomExtraDescMenu(const dikuRoom *, extraDesc *, uchar *);
char interpEditRoomExtraDescMenu(usint, dikuRoom *, extraDesc **, uchar *);
void editRoomExtraDesc(dikuRoom *);

// editrext.cpp

void displayEditRoomExitsMenu(const dikuRoom *, ulong *);
char interpEditRoomExitsMenu(usint, dikuRoom *, ulong *);
void editRoomExits(dikuRoom *);

// editrflg.cpp

void editRoomFlags(dikuRoom *);

// editrmsc.cpp

void displayEditRoomMiscMenu(const dikuRoom *);
char interpEditRoomMiscMenu(const usint, dikuRoom *);
void editRoomMisc(dikuRoom *);

// editroom.cpp

void editRoomDesc(dikuRoom *, const char);
void displayEditRoomMenu(const dikuRoom *);
char interpEditRoomMenu(const usint, dikuRoom *, dikuRoom *);
dikuRoom *editRoom(dikuRoom *, const char);
void preEditRoom(const char *);
char jumpRoom(const dikuRoom *, ulong *);

// editrsec.cpp

void displayEditRoomSectorMenu(const dikuRoom *);
char interpEditRoomSectorMenu(usint ch, dikuRoom *);
void editRoomSector(dikuRoom *);

// editshp.cpp

void displayEditShopMenu(shop *, const char *, const long);
char interpEditShopMenu(usint, shop *, const char *, const long, dikuMob *);
dikuMob *realEditShop(shop *, const char *, const long, dikuMob *, const char);
void editShop(shop *, char *, long, dikuMob *, const char);
char jumpShop(ulong *, ulong);

// editshpb.cpp

void displayEditShopBooleansMenu(shop *, const char *, const long);
char interpEditShopBooleansMenu(const usint, shop *, const char *, const long);
void editShopBooleans(shop *, const char *, const long);

// editshpc.cpp

void displayEditShopTimesMenu(shop *, const char *, const long);
char interpEditShopTimesMenu(const usint, shop *, const char *, const long);
void editShopTimes(shop *, const char *, const long);

// editshpm.cpp

void displayEditShopMessagesMenu(const char *, const long);
char interpEditShopMessagesMenu(const usint, shop *, const char *, const long);
void editShopMessages(shop *, const char *, const long);

// editshpr.cpp

void displayEditShopRacistMenu(shop *, const char *, const long);
char interpEditShopRacistMenu(const usint, shop *, const char *, const long);
void editShopRacist(shop *, const char *, const long);

// editshps.cpp

void displayEditShopSoldMenu(shop *, const char *, const long);
char interpEditShopSoldMenu(usint, shop *, const char *, const long);
void editShopSold(shop *, const char *, const long);

// editshpt.cpp

void displayEditShopBoughtMenu(shop *, const char *, const long);
char interpEditShopBoughtMenu(usint, shop *, const char *, const long);
void editShopBought(shop *, const char *, const long);

// editshpu.cpp

void editShopStrn(const char *);
void editShopPrompt(void);

// editzflg.cpp

void editZoneMiscFlags(dikuZone *);

// editzmsc.cpp

void displayEditZoneMiscMenu(dikuZone *);
void editZoneLowLifespan(dikuZone *);
void editZoneHighLifespan(dikuZone *);
void editZoneResetMode(dikuZone *);
char interpEditZoneMiscMenu(usint, dikuZone *);
void editZoneMisc(dikuZone *);

// editzone.cpp

void displayEditZoneMenu(const dikuZone *);
char interpEditZoneMenu(const usint, dikuZone *);
void editZone(dikuZone *);

// equip.cpp

uchar equipEquiponMob(mobHere *, objectHere *, const char);
void equipMobSpecific(char *, dikuRoom *, const char = TRUE);
void equipMob(char *, dikuRoom *, const char);

// exit.cpp

char getDirfromKeyword(const char *);
INLINEDEC char exitNeedsKey(const roomExit *, const long);
INLINEDEC void checkRoomExitKey(const roomExit *, const dikuRoom *, const char *);
char compareRoomExits(roomExit *, roomExit *);
void deleteRoomExit(roomExit *, const char);
roomExit *copyRoomExit(roomExit *, const char);
INLINEDEC ulong getExitAvailFlags(const dikuRoom *);
INLINEDEC char *getRoomExitsShortStrn(const dikuRoom *, char *);
char swapExits(const char *, dikuRoom *);
void swapExitsNorthSouth(void);
void swapExitsWestEast(void);
void swapExitsUpDown(void);
INLINEDEC char getWorldDoorType(const roomExit *);
INLINEDEC char getZoneDoorState(const roomExit *);
INLINEDEC void resetExits(const long, const long);
INLINEDEC void clearExits(const long, const char);
INLINEDEC roomExit *findCorrespondingExit(const long, const long, char *);
INLINEDEC const char *getExitStrn(const char);
INLINEDEC roomExit *getExitNode(const ulong, const char);
INLINEDEC roomExit *getExitNode(const dikuRoom *, const char);
void checkExit(roomExit *, const char *, const long);
INLINEDEC void checkAllExits(void);
char *getDescExitStrnforZonefile(const dikuRoom *, const roomExit *, char *, const char *);
INLINEDEC char getNumbExits(const dikuRoom *);
INLINEDEC char isExitOut(const roomExit *);
INLINEDEC char roomExitOutsideZone(const roomExit *);

// flags.cpp

long whichFlag(const char *, const flagDef *);
char *getFlagStrn(const ulong, const flagDef *, char *);
ulong fixFlags(ulong *, const flagDef *);
void fixAllFlags(void);
void which(const char *);
void massSet(const char *);

// getstrn.cpp

void dispGetStrnField(const char *, const ulong, const ulong, const char, const ulong, const char, const char, const char, const char *);
char *getStrn(char *, const ulong, const char, const char, const uchar, const char *, const char, const char);

// history.cpp

void addCommand(const char *, stringNode **);

// init.cpp

void startInit(void);

// initcomm.cpp

void initMainCommands(void);
void initLookupCommands(void);
void initCreateCommands(void);
void initLoadCommands(void);
void initEditCommands(void);
void initListCommands(void);
void initCreateEditCommands(void);
void initSetCommands(void);
void initDeleteCommands(void);
void initStatCommands(void);
void initSetLimitCommands(void);
void initCloneCommands(void);
void initCopyDescCommands(void);
void initCopyDefaultCommands(void);
void initCopyCommands(void);

// initscr.cpp

#ifdef __UNIX__
void initUnixCursesScreen(void);
#endif
#ifdef _WIN32
void initWindowsScreen(void);
#endif

// interp.cpp

void interpCommands(const char *, const char);

// inv.cpp

void showInventory(void);
void takeEntityFromEntity(char *, char *, dikuRoom *, const uchar);
void takeEntity(char *, dikuRoom *, const char);
void dropEntity(char *, dikuRoom *, const char);

// keywords.cpp

stringNode *createKeywordList(const char *);
char addKeywordtoList(stringNode **, const char *);
char *getReadableKeywordStrn(stringNode *, char *);
char scanKeyword(const char *, stringNode *);
char *createKeywordString(stringNode *, char *);

// unixgraph.cpp

#ifdef __UNIX__
void clrline(const int, const char = 7, const char = 0);
int getkey(void);
void _settextcolor(const uchar);
uchar _gettextcolor(void);
uchar _getbkcolor(void);
void _settextposition(const int, const int);
struct rccoord _gettextposition(void);
void _outtext(const char *);
void _setbkcolor(const uchar);
void clrscr(const char = 7, const char = 0);
void itoa(const long, char *, const int);
void utoa(const ulong, char *, const int);
#endif

// list.cpp

void list(const char *);

// load.cpp

void loadSumpn(const char *);

// loaded.cpp

void addEntity(const char, const ulong);
void setEntityOverride(const char, const ulong, const long, const long);
entityLoaded *getFirstEntityOverride(entityLoaded *);
void deleteEntityinLoaded(const char, const ulong);
void decEntity(const char, const ulong);
ulong getNumbEntities(const char, const ulong, const char);
entityLoaded *getNumbEntitiesNode(const char, const ulong);
void loadAllinObjHereList(objectHere *);
void loadAllinMobHereList(mobHere *);
void resetNumbLoaded(const char, const long, const long);
void displayLoadedList(const char *);
void setLimitArgs(const char *);
void setLimitArgsStartup(const char *);
void setLimitOverrideObj(const char *, const char);
void setLimitOverrideMob(const char *, const char);

// look.cpp

void displayMap(const dikuRoom *);
void displayContainerContents(objectHere *, const char);
void lookStuff(char *);
void lookExit(const roomExit *);
void lookInObj(const char *);
void look(char *);

// maincomm.cpp

void commandQuit(void);
const void mainExecCommand(const uchar, const char *);

// master.cpp

masterKeywordListNode *createMasterKeywordList(dikuRoom *);
stringNode *checkMasterKeywordList(const char *, masterKeywordListNode *, char *, masterKeywordListNode **);
void deleteMasterKeywordList(masterKeywordListNode *);

// menu.cpp

char checkMenuKey(const usint, const char);

// misc.cpp

INLINEDEC long getScreenHeight(void);
INLINEDEC long getScreenWidth(void);
INLINEDEC ulong numbLinesStringNeeds(const char *);
char confirmChanges(void);
char writeFiles(void);
void createPrompt(void);
void displayRecordSizeInfo(void);
INLINEDEC const char *getOnOffStrn(const uchar);
INLINEDEC const char *getYesNoStrn(const uchar);
INLINEDEC const char *getYesNoStrn(const void *);
INLINEDEC const char *plural(const long);
INLINEDEC const char *getVowelN(const char);
void checkPreArgs(const int, const char *[]);
void checkArgs(const int, const char *[]);
void renumberCurrent(void);
void renumberAll(const char *);
void lookup(const char *);
void where(const char *);
void dumpTextFile(const char *, const char);
INLINEDEC void displayVersionInfo(void);
void verifyZoneFlags(void);
INLINEDEC char getBoolYesNo(const char *);
INLINEDEC const char *getYesNoBool(const char);
void toggleGuildVar(void);
void at(const char *);
void displayLookupList(const char, ulong);
void preDisplayLookupList(const char *);
void displayMiscInfo(void);
char *getMoneyStrn(ulong, char *);
char isset(const ulong, const ulong);
void deleteUnusedObjMobTypes(void);
void resetEntityPointersByNumb(const char = TRUE, const char = TRUE);
void resetObjCond(void);
void fixGuildStuff(void);
void editRoomsOnly(void);
void editObjsOnly(void);
void editMobsOnly(void);
char validANSIletter(const char);
INLINEDEC uchar durisANSIcode(const char *, const ulong);

// misccomm.cpp

const void lookupExecCommand(const uchar, const char *);
const void createExecCommand(const uchar, const char *);
const void loadExecCommand(const uchar, const char *);
const void editExecCommand(const uchar, const char *);
const void listExecCommand(const uchar, const char *);
const void createEditExecCommand(const uchar, const char *);
const void deleteExecCommand(const uchar, const char *);
const void statExecCommand(const uchar, const char *);
const void setLimitExecCommandStartup(const uchar, const char *);
const void setLimitExecCommand(const uchar, const char *);
const void cloneExecCommand(const uchar, const char *);
const void copyDescExecCommand(const uchar, const char *);
const void copyDefaultExecCommand(const uchar, const char *);
const void copyExecCommand(const uchar, const char *);
void copyCommand(const char *);

// mob.cpp

INLINEDEC dikuMob *findMob(const register ulong);
dikuMob *copyMobType(const dikuMob *, const char);
uchar compareMobType(dikuMob *, dikuMob *);
INLINEDEC ulong getHighestMobNumber(void);
INLINEDEC ulong getLowestMobNumber(void);
INLINEDEC ulong getFirstFreeMobNumber(void);
INLINEDEC char *fixMobName(const char *, char *);
INLINEDEC char *fixMobNameShort(const char *, char *);
void rebuildMobLookup(void);
void renumberMobs(const char, const long);
INLINEDEC dikuMob *getPrevMob(const ulong);
INLINEDEC dikuMob *getNextMob(const ulong);
char checkDieStrnValidityShort(const char *);
char checkDieStrnValidity(const char *, const ulong, const char *);
void deleteMobsinInv(const dikuMob *);
void updateInvKeywordsMob(const dikuMob *);
INLINEDEC void resetLowHighMob(void);

// mobhere.cpp

ulong getNumbMobHereNodes(mobHere *);
mobHere *findMobHere(const ulong, dikuRoom *, ulong *, const char);
mobHere *copyMobHere(mobHere *, const char);
mobHere *copyMobHereList(mobHere *, const char);
void deleteMobHereList(mobHere *, const char);
void deleteMobHere(mobHere *, const char);
char checkForMobHeresofType(const dikuMob *);
void addMobHeretoList(mobHere **, mobHere *);
void deleteAllMobHereofTypeInList(mobHere **, const dikuMob *, const char);
void deleteAllMobHeresofType(const dikuMob *, const char);
void deleteMobHereinList(mobHere **, mobHere *, const char);
void resetMobHere(const ulong, const ulong, dikuRoom *);
char eqSlotFull(const mobHere *, const char);
char eqRestrict(const dikuMob *, const objExtraBitFlags, const objAntiBitFlags, const objAnti2BitFlags);
char handsFree(const mobHere *);
char getMobHereEquipSlot(const mobHere *, const dikuObject *, const char);
char canMobTypeEquip(const dikuMob *, const dikuObject *, const char);
char *getCanEquipErrStrn(const char, const char, char *);
char *getVerboseCanEquipStrn(const char, const char *, char *);
objectHere *getMobEquipPos(const mobHere *, const char);
char mobsCanEquipEquipped(const char);
char mobsEqinCorrectSlot(const char);
char mobEquippingSomething(const mobHere *);
ulong getNumbFollowers(const mobHere *);
dikuMob *getMatchingMob(const char *, const dikuRoom * = NULL, dikuMob * = NULL, mobHere * = NULL);

// mobtypes.cpp

const char *getMobSpeciesStrn(const char *);
const char *getMobPosStrn(const ulong);
const char *getMobSexStrn(const uchar);
const char *getMobHometownStrn(const char);
const char *getMobClassStrn(const uchar);
INLINEDEC int isAggro(const dikuMob *);
INLINEDEC int castingClass(const uchar);

// mobu.cpp

void renumberMobsUser(const char *);

// mountfol.cpp

void mountMob(const char *, dikuRoom *);
void unmountMob(const char *, dikuRoom *);
void followMob(const char *, dikuRoom *);
void unfollowMob(const char *, dikuRoom *);

// mudcomm.cpp

const char *getCommandStrn(const ulong, const char);
void displayCommandList(const char);
void displayCommandList(char *);

// object.cpp

INLINEDEC dikuObject *findObj(const register ulong);
INLINEDEC char *fixObjName(const char *, char *);
dikuObject *copyObjectType(dikuObject *, const char);
char compareObjectApply(const objApplyRec *, const objApplyRec *);
uchar compareObjectType(dikuObject *, dikuObject *);
INLINEDEC ulong getHighestObjNumber(void);
INLINEDEC ulong getLowestObjNumber(void);
INLINEDEC ulong getFirstFreeObjNumber(void);
void checkAndFixRefstoObj(const ulong, const ulong);
void rebuildObjLookup(void);
void renumberObjs(const char, const long);
INLINEDEC dikuObject *getPrevObj(const ulong);
INLINEDEC dikuObject *getNextObj(const ulong);
void deleteObjsinInv(const dikuObject *);
void updateInvKeywordsObj(const dikuObject *);
void resetLowHighObj(void);
dikuObject *getMatchingObj(const char *, dikuRoom * = NULL, dikuObject * = NULL, objectHere * = NULL);
char armorPosTypeMatters(const long);

// objectu.cpp

void renumberObjectsUser(const char *);

// objhere.cpp

char checkAllObjInsideForVnum(objectHere *, const long);
objectHere *findObjHere(const ulong, dikuRoom *, ulong *, const char = FALSE, const char = FALSE);
ulong getNumbObjectHereNodes(objectHere *);
objectHere *copyObjHere(objectHere *, const char);
objectHere *copyObjHereList(objectHere *, const char);
char checkEntireObjHereListForType(objectHere *, const dikuObject *);
char checkForObjHeresofType(const dikuObject *);
char scanObjHereListForLoadedContainer(objectHere *, const ulong);
char checkForObjHeresWithLoadedContainer(const ulong);
void resetObjHereList(const long, const long, objectHere *);
void resetAllObjHere(const long, const long, dikuRoom *);
void addObjHeretoList(objectHere **, objectHere *);
objectHere *objinInv(mobHere *, const dikuObject *);
objectHere *objinInv(mobHere *, const ulong);
char isObjTypeUsed(ulong);
void resetObjHEntityPointersByNumb(objectHere *);

// objtypes.cpp

const char *getObjTypeStrn(const ulong);
const char *getObjTypeStrnShort(const ulong);
const char *getObjApplyStrn(const ulong);
const char *getObjApplyStrnShort(const ulong);
const char *getObjCraftsmanshipStrnShort(const ulong);
const char *getObjCraftsmanshipStrn(const ulong);
const char *getObjSizeStrnShort(const long);
const char *getObjSizeStrn(const long);
const char *getMaterialStrn(const long);
const char *getLiqTypeStrn(const ulong);
const char *getWeapDamStrn(const ulong);
const char *getWeapTypeStrn(const ulong);
const char *getArmorTypeStrn(const long);
const char *getArmorWornStrn(const long);
const char *getArmorWornStrnShort(const long);
const char *getArmorThicknessStrn(const long);
const char *getShieldTypeStrn(const long);
const char *getShieldShapeStrn(const long);
const char *getShieldShapeStrnShort(const long);
const char *getShieldSizeStrn(const long);
const char *getShieldSizeStrnShort(const long);
const char *getSpellTypeStrn(const long);
const char *getInstrumentTypeStrn(const ulong);
const char *getMissileTypeStrn(const ulong);
const char *getLangTypeStrn(const ulong);
char *getTotemSphereStrn(const ulong, char *);
char *getObjTrapAffStrn(const ulong, char *);
const char *getObjTrapDamStrn(const long);
char *getObjValueStrn(const ulong, const ulong, const long, char *, char *, const char, const dikuObject *);
char checkForValueList(const ulong, const ulong);
char checkForVerboseAvail(const ulong, const ulong);
const char *checkForSearchAvail(const ulong, const ulong);
void searchObjValue(const ulong, const ulong);
void displayObjValueHelp(const ulong, const ulong, const char);
char fieldRefsObjNumb(const ulong, const ulong);
char fieldRefsRoomNumb(const ulong, const ulong);
void specialObjValEditRedundant(dikuObject *, const uchar, const flagDef *, const uchar, const char *);
char specialObjValEdit(dikuObject *, const uchar, const char);

// purge.cpp

void purge(char *);

// put.cpp

void putEntity(char *, dikuRoom *, char);

// qsttypes.cpp

char checkForQuestItemHelp(const char, const char);
void displayQuestItemHelp(const char, const char);
const char *getQuestItemTypeStrn(const char, const char);
void displayQuestItemTypeList(const char);

// quest.cpp

INLINEDEC quest *findQuest(ulong);
INLINEDEC ulong getLowestQuestMobNumber(void);
INLINEDEC ulong getHighestQuestMobNumber(void);
dikuMob *getPrevQuestMob(const ulong);
dikuMob *getNextQuestMob(const ulong);
char compareQuestMessage(questMessage *, questMessage *);
questMessage *copyQuestMessage(questMessage *);
questMessage *createQuestMessage(void);
void deleteQuestMessage(questMessage *);
void deleteQuestMessageList(questMessage *);
void addQuestMessagetoList(questMessage **, questMessage *);
void deleteQuestMessageinList(questMessage **, questMessage *);
questItem *createQuestItem(void);
void addQuestItemtoList(questItem **, questItem *);
void deleteQuestItem(questItem *);
void deleteQuestIteminList(questItem **, questItem *);
void deleteQuestItemList(questItem *);
questQuest *createQuestQuest(void);
void deleteQuestQuest(questQuest *);
void deleteQuestQuestList(questQuest *);
void addQuestQuesttoList(questQuest **, questQuest *);
void deleteQuestQuestinList(questQuest **, questQuest *);
questMessage *copyQuestMessageList(questMessage *);
questItem *copyQuestItemList(questItem *);
char compareQuestItemList(questItem *, questItem *);
char compareQuestQuest(questQuest *, questQuest *);
questQuest *copyQuestQuest(questQuest *);
questQuest *copyQuestQuestList(questQuest *);
quest *copyQuest(quest *);
quest *createQuest(void);
void deleteQuest(quest *);
void deleteQuestAssocLists(quest *);
char compareQuestMessageLists(questMessage *, questMessage *);
char compareQuestQuestLists(questQuest *, questQuest *);
char compareQuestInfo(quest *, quest *);
char *fixQuestRecvString(const char *, char *);
char *getQuestRecvString(questItem *, char *);
ulong getNumbMessageNodes(questMessage *);
ulong getNumbQuestNodes(questQuest *);
ulong getNumbItemNodes(questItem *);
questMessage *getMessageNodeNumb(const ulong, questMessage *);
questQuest *getQuestNodeNumb(const ulong, questQuest *);
questItem *getItemNodeNumb(const ulong, questItem *);
char *getQuestItemStrn(const questItem *, const char, char *);
ulong getNumbQuestMobs(void);
char checkForMobWithQuest(void);
void displayQuestList(void);
void displayQuestList(const char *);

// readmob.cpp

char convertMobMoney(char *, dikuMob *);
dikuMob *readMobFromFile(FILE *, uchar, uchar);
char readMobFile(const char * = NULL);

// readobj.cpp

char checkLastObjStrn(const char *, dikuObject *);
extraDesc *readObjExtraDesc(FILE *, dikuObject *, extraDesc *);
dikuObject *readObjectFromFile(FILE *, char *, const uchar, const uchar);
char readObjectFile(const char * = NULL);

// readqst.cpp

questMessage *readQuestFileMessage(FILE *, const ulong);
questQuest *readQuestFileQuest(FILE *, char *, char *, const ulong);
char readQuestFile(const char * = NULL);

// readset.cpp

void readSettingsFile(const char *);

// readshp.cpp

shop *readShopFromFile(FILE *);
char readShopFile(const char * = NULL);

// readwld.cpp

extraDesc *readRoomExtraDesc(FILE *, dikuRoom *, extraDesc *);
roomExit *readRoomExit(FILE *, dikuRoom *, const char *, const char);
dikuRoom *readRoomFromFile(FILE *, const uchar, const uchar);
char readWorldFile(const char * = NULL);

// readzon.cpp

void addLastObjLoaded(lastObjHereLoaded **, const ulong, objectHere *);
objectHere *getLastObjLoaded(lastObjHereLoaded *, const ulong);
void deleteLastObjLoadedList(lastObjHereLoaded *);
void addLimitSpec(limitSpecified **, const char, const long, const ulong);
void deleteLimitSpecList(limitSpecified *);
void setOverrideFromLimSpec(limitSpecified *);
void removeComments(char *);
void loadObj(const char *, lastObjHereLoaded **, limitSpecified **, const char, const ulong);
long loadMob(const char *, mobHere **, mobHere **, limitSpecified **, const char, const ulong);
void setDoorState(const char *, const ulong);
void putObjObj(const char *, lastObjHereLoaded **, limitSpecified **, char *, const ulong);
void equipMobObj(const char *, mobHere *, lastObjHereLoaded **, limitSpecified **, const ulong);
void giveMobObj(const char *, mobHere *, lastObjHereLoaded **, limitSpecified **, const ulong);
void mountMob(const char *, mobHere **, limitSpecified **, const ulong);
void followMob(const char *, mobHere *, mobHere **, limitSpecified **, const ulong);
char readZoneFile(const char * = NULL);

// room.cpp

INLINEDEC dikuRoom *findRoom(const register ulong);
void goDirection(const roomExit *, const ulong);
dikuRoom *copyRoomInfo(const dikuRoom *, const char, const char);
uchar compareRoomInfo(dikuRoom *, dikuRoom *);
void deleteRoomInfo(dikuRoom *, const char, const char, const char);
void deleteRoomAssocLists(dikuRoom *, const char, const char);
INLINEDEC char *fixRoomName(const char *, char *);
INLINEDEC char *fixRoomNameShort(const char *, char *);
INLINEDEC ulong getHighestRoomNumber(void);
INLINEDEC ulong getLowestRoomNumber(void);
INLINEDEC ulong getFirstFreeRoomNumber(void);
void checkAndFixRefstoRoom(const ulong, const ulong);
void rebuildRoomLookup(void);
void renumberRooms(const char, const long);
void updateAllObjMandElists(void);
void updateAllMobMandElists(void);
INLINEDEC char noExitsLeadtoRoom(const ulong);
INLINEDEC char noExitOut(const dikuRoom *);
long roomHasAllFollowers(void);
dikuRoom *getRoomKeyword(const char *, const char);
INLINEDEC ulong getNumbFreeRooms(void);
INLINEDEC void resetLowHighRoom(void);
INLINEDEC dikuRoom *getPrevRoom(const ulong);
INLINEDEC dikuRoom *getNextRoom(const ulong);
void checkMapTrueness(void);
int checkMapTruenessRedund(const dikuRoom *, const int, const int, int &);

// roomtype.cpp

const char *getManaFlagStrn(const ulong);
char *getRoomSectorStrn(const uchar, const char, const char, const char, char *);

// roomu.cpp

void gotoRoomStrn(const char *);
void gotoRoomPrompt(void);
char createGrid(const long, const long, const long, const char);
void createGridInterp(const char *);
char linkRooms(const ulong, const ulong, const char);
void linkRoomsInterp(const char *);
void renumberRoomsUser(const char *);

// setcomm.cpp

const void setExecCommandFile(const uchar, const char *);

// setrand.cpp

void setEntityRandomVal(const char *, dikuRoom *);

// shop.cpp

INLINEDEC shop *findShop(ulong);
INLINEDEC ulong getLowestShopMobNumber(void);
INLINEDEC ulong getHighestShopMobNumber(void);
dikuMob *getPrevShopMob(const ulong);
dikuMob *getNextShopMob(const ulong);
shop *createShop(void);
void deleteShop(shop *);
shop *copyShop(const shop *);
char compareShopInfo(const shop *, const shop *);
ulong getNumbShopMobs(void);
char checkForMobWithShop(void);
void displayShopList(void);
void displayShopList(const char *);
char *getShopRaceStrn(const char);
void displayShopRaceList(void);
ulong getNumbShopSold(const ulong *);
ulong getNumbShopBought(const ulong *);
shop *getShopinRoom(dikuRoom *);
dikuMob *getShopOwner(shop *);
char listShopSold(dikuRoom *);
mobHere *getMobHereShop(const shop *);
char checkAllShops(void);
void checkForDupeShopLoaded(void);

// stat.cpp

void statSumpn(const char *);

// strings.cpp

char *deleteChar(char *, const ulong);
char *insertChar(char *, const ulong, const char);
char *remLeadingSpaces(char *);
char *remTrailingSpaces(char *);
char *remSpacesBetweenArgs(char *);
char *nolf(char *);
usint numbArgs(const char *);
char *getArg(const char *, const ulong, char *);
char *upstrn(char *);
char *upfirstarg(char *);
char *lowstrn(register char *);
char strnumer(register const char *, const usint = 0);
char strleft(const char *, const char *);
char strright(const char *, const char *);
char *strnSect(const char *, char *, const ulong, ulong);
char *fixStringColor(const char *, char *, ulong);
char *fixString(const char *, char *, const ulong,
                const char);
ulong numbPercentS(const char *);
INLINEDEC char lastCharLF(const char *);
ulong numbLinefeeds(const char *);
ulong truestrlen(const char *);

// strnnode.cpp

ulong getNumbStringNodes(stringNode *);
void fixStringNodes(stringNode **, const char);
stringNode *readStringNodes(FILE *, const char, const char);
void writeStringNodes(FILE *, stringNode *);
void deleteStringNodes(stringNode *);
stringNode *copyStringNodes(stringNode *);
char compareStringNodes(stringNode *, stringNode *);
stringNode *editStringNodes(stringNode *, const char);

// system.cpp

#ifndef __UNIX__
void execShell(void);
#ifndef _WIN32
//void clrscr(const char = 7, const char = 0);
int getkey(void);
#endif
INLINEDEC void clrline(const uchar, const uchar = 7, const uchar = 0, const uchar = ' ');
ulong freeStack(void);
void displayFreeStack(const char = -1, const char = -1, const char = TRUE);
#endif

// template.cpp

void setTemplateArgs(const char *, const char, const char);

// unequip.cpp

void unequipMobSpecific(const char *, const char *, dikuRoom *);
void unequipMob(const char *, dikuRoom *);

// var.cpp

void unvarCmd(char *, variable **);
char addVar(variable **, char *, const char *);
void setVarArgs(const char *, const char, const char);
void varCmd(char *, const char, variable **);
long getVarNumb(variable *, const char *, char *, const long);
const char *getVarStrn(variable *, const char *, char *, char *, const char *);
void setVarBoolVal(variable **, char *, const char, const char);
char getVarBoolVal(variable *, const char *, char *, const char);
char varExists(variable *, const char *);
char isVarBoolean(variable *, const char *);
void toggleVar(variable **, char *);
variable *copyVarList(const variable *);
void deleteVarList(variable *);
char compareVarLists(variable *, variable *);

// var.cpp

void unvarCmd(char *, variable **);
char addVar(variable **, char *, const char *);
void setVarArgs(const char *, const char, const char);
void varCmd(char *, const char, variable **);
long getVarNumb(variable *, const char *, char *, const long);
const char *getVarStrn(variable *, const char *, char *, char *, const char *);
void setVarBoolVal(variable **, char *, const char, const char);
char getVarBoolVal(variable *, const char *, char *, const char);
char varExists(variable *, const char *);
char isVarBoolean(variable *, const char *);
void toggleVar(variable **, char *);
variable *copyVarList(const variable *);
void deleteVarList(variable *);
char compareVarLists(variable *, variable *);

// vardef.cpp

INLINEDEC long getScreenHeightVal(void);
INLINEDEC long getScreenWidthVal(void);
INLINEDEC long getSaveHowOftenVal(void);
INLINEDEC const char *getEditorName(void);
INLINEDEC char getIsMapZoneVal(void);
INLINEDEC char getVnumCheckVal(void);
INLINEDEC char getObjAffectVal(void);
INLINEDEC char getCheckLimitsVal(void);
INLINEDEC char getCheckZoneFlagsVal(void);
INLINEDEC char getFirstObjEdescKeyVal(void);
INLINEDEC char getShowObjFlagsVal(void);
INLINEDEC char getShowMobFlagsVal(void);
INLINEDEC char getShowMobPosVal(void);
INLINEDEC char getShowObjVnumVal(void);
INLINEDEC char getShowMobVnumVal(void);
INLINEDEC char getShowMobRideFollowVal(void);
INLINEDEC char getShowRoomExtraVal(void);
INLINEDEC char getShowRoomVnumVal(void);
INLINEDEC char getShowPricesAdjustedVal(void);
INLINEDEC char getInterpColorVal(void);
INLINEDEC char getShowColorVal(void);
INLINEDEC const char *getMenuPromptName(void);
INLINEDEC const char *getMainPromptStrn(void);
INLINEDEC char getWalkCreateVal(void);
INLINEDEC char getShowExitDestVal(void);
INLINEDEC char getShowExitFlagsVal(void);
INLINEDEC long getRoomStartVal(void);
INLINEDEC char getStartRoomActiveVal(void);
INLINEDEC char getShowMenuInfoVal(void);
INLINEDEC const char *getMainZoneNameStrn(void);
INLINEDEC const char *getLastCommandStrn(void);
INLINEDEC char getCheckSaveVal(void);
INLINEDEC char getCheckLoneRoomVal(void);
INLINEDEC char getCheckMissingKeysVal(void);
INLINEDEC char getCheckLoadedVal(void);
INLINEDEC char getCheckRoomVal(void);
INLINEDEC char getCheckExitVal(void);
INLINEDEC char getCheckExitDescVal(void);
INLINEDEC char getCheckObjVal(void);
INLINEDEC char getCheckObjValuesVal(void);
INLINEDEC char getCheckObjDescVal(void);
INLINEDEC char getCheckMobDescVal(void);
INLINEDEC char getCheckMobVal(void);
INLINEDEC char getCheckEdescVal(void);
INLINEDEC char getNoSaveonCheckErrVal(void);
INLINEDEC char getPauseCheckScreenfulVal(void);
INLINEDEC char getCheckFlagsVal(void);
INLINEDEC char getSaveCheckLogVal(void);
INLINEDEC char getEditUneditableFlagsVal(void);
INLINEDEC char getAllMobsVal(void);
INLINEDEC char getIgnoreZoneSVal(void);
INLINEDEC char getCheckMobClassEqVal(void);
INLINEDEC char getCheckMobRaceEqVal(void);
INLINEDEC char getCheckMobSexEqVal(void);
INLINEDEC char getCheckObjMaterialVal(void);
INLINEDEC char getCheckGuildStuffVal(void);
INLINEDEC char getNegDestOutofZoneVal(void);
INLINEDEC char getSaveEveryXCommandsVal(void);

// win32.cpp

#ifdef _WIN32
int checkWindowsVer(void);
int setupWindowsConsole(void);
WORD getForegroundValue(const uchar);
WORD getBackgroundValue(const uchar);
INLINEDEC void _settextposition(const usint, const usint);
INLINEDEC struct rccoord _gettextposition(void);
void _outtext(const char *, const int checkforLF = TRUE);
INLINEDEC int getkey(void);
void _settextcolor(const uchar);
INLINEDEC void _setbkcolor(const uchar);
INLINEDEC uchar _gettextcolor(void);
INLINEDEC uchar _getbkcolor(void);
INLINEDEC void clrscr(const uchar fg = 7, const uchar bg = 0);
INLINEDEC void utoa(const ulong, char *, const int);
#endif

// writemob.cpp

void writeMobtoFile(FILE *, dikuMob *);
void writeMobFile(const char * = NULL);

// writeobj.cpp

void writeObjecttoFile(FILE *, const dikuObject *);
void writeObjectFile(const char * = NULL);

// writeqst.cpp

void writeQuesttoFile(FILE *, quest *, const long);
void writeQuestFile(const char * = NULL);

// writeset.cpp

void writeSettingsFile(const char *, const char);

// writeshp.cpp

void writeShoptoFile(FILE *, shop *, const ulong);
void writeShopFile(const char * = NULL);

// writewld.cpp

void writeWorldExit(const roomExit *, const char *, FILE *);
void writeRoomtoFile(FILE *, const dikuRoom *);
void writeWorldFile(char * = NULL);

// writezon.cpp

void addExitWrittentoList(exitWritten **, exitWritten *);
void deleteExitWrittenList(exitWritten *);
exitWritten *checkExitWrittenListforExit(exitWritten *, const long, const char);
roomExit *checkForPairedExit(const roomExit *, const long, char *);
char writeAllExitsPairedRedundant(FILE *, const dikuRoom *, exitWritten **, roomExit *, const char *, const char, const char);
void writeAllExitsPaired(FILE *);
void writeAllObjInside(FILE *, objectHere *, const ulong, const char);
void writeLoadObjectCmd(const dikuRoom *, const objectHere *, FILE *);
void writeLoadMobCommand(const dikuRoom *, const mobHere *, FILE *);
void writeEquipMobCommand(const objectHere *, FILE *, char);
void writeGiveMobCommand(const objectHere *, FILE *);
void writeRideMobCommand(const dikuRoom *, const mobHere *, FILE *);
void writeMobFollowerCommands(const mobHere *, FILE *);
void writeZoneFile(const char * = NULL);

// zone.cpp

INLINEDEC char *fixZoneName(const char *, char *);
void setZoneNumb(const ulong, const char);
void setZoneNumbStrn(const char *);
void setZoneNumbPrompt(void);

// zonetype.cpp

const char *getZoneResetStrn(const ulong);

#define _FH_H_
#endif
