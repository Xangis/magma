/*  Project pounds

    Copyright © 1994-1996. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         area2.cpp
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file for implementation of Merc-like objects
*/

#include "areadoc.h"

#ifndef __FLAT__
char HUGE *_hstrcpy(char HUGE *dest, const char HUGE *src)
{
   char HUGE *p = dest;
	while ((*dest++ = *src++) != 0)
		;

	return p;
}
#else
#define _hstrcpy strcpy
#endif

//==============================================================================
TMercObj::TMercObj(TAreaDocument *doc)
{
   AreaDoc = doc;
}

THelpData::THelpData(TAreaDocument *doc): TMercObj(doc)
{
   level = -1;
}

int THelpData::operator==(const THelpData& other) const
{
   return level == other.level &&
      keyword == other.keyword &&
      text == other.text;
}

const TMercObj *THelpData::GetName(char *lpOut, int /*nOutSize*/) const
{
   wsprintf(lpOut, "%d: %s", level, keyword.c_str());
   return this;
}

TShopData::TShopData(TAreaDocument *doc): TMercObj(doc)
{
    keeper = -1;
    for (int i = 0; i < nelems(buy_type); i++)
       buy_type[i] = 0;
    profit_buy = profit_sell = 100;
    open_hour = 0;
    close_hour = 23;
};

TShopData::TShopData(const TShopData& other): TMercObj(other)
{
    keeper = other.keeper;
    for (int i = 0; i < nelems(buy_type); i++)
       buy_type[i] = other.buy_type[i];
    profit_buy = other.profit_buy;
    profit_sell = other.profit_sell;
    open_hour = other.open_hour;
    close_hour = other.close_hour;
    comment = other.comment;
}

int TShopData::operator==(const TShopData& other) const
{
    if (keeper != other.keeper ||
         profit_buy != other.profit_buy ||
         profit_sell != other.profit_sell ||
         open_hour != other.open_hour ||
         close_hour != other.close_hour ||
         comment != other.comment)
       return FALSE;

    for (int i = 0; i < MAX_TRADE; i++)
       if (buy_type[i] != other.buy_type[i])
          return FALSE;

    return TRUE;
}

const TMercObj *TShopData::GetName(char *lpOut, int nOutSize) const
{
    const TMobile *mob = AreaDoc->GetMobile(keeper);
    if (mob)
       mob->GetName(lpOut, nOutSize);
    else
       wsprintf(lpOut, "#%d (Shopkeeper is not a member of this zone)", keeper);

    return this;
}

TAffect::TAffect()
{
   location = modifier = 0;
//   bitvector = 0;
//   where = TO_OBJECT;
}

TAffect::TAffect(const TAffect& other)
{
    location = other.location;
    modifier = other.modifier;

    // ROM 2.4
//    where = other.where;
//    bitvector = other.bitvector;
}

int TAffect::operator==(const TAffect& other) const
{
    return location == other.location &&
       modifier == other.modifier /*&&
       where == other.where &&
       bitvector == other.bitvector*/;
}

//TMobile::TMobile(TAreaDocument *doc): TMercObj(doc)
TMobile::TMobile(TAreaDocument *doc): TVNumObj(doc),
    mobprogs(1, 0, 5)
{
//	 vnum = -1;
	 sex = SEX_MALE;
    level = 0;
    act = 0;
    affected_by = 0;
    alignment = 0;

	 hitroll = 0;
	 ac = 0;
	 hitnodice = 0;
	 hitsizedice = 0;
	 hitplus = 0;
	 damnodice = 0;
	 damsizedice = 0;
	 damplus = 0;
	 gold = 0;

    // ROM stuff
    mana[0] = mana[1] = mana[2] = 0;
    rom_ac[0] = rom_ac[1] = rom_ac[2] = rom_ac[3] = 0;
    dam_type = 0;
    off_flags = OFF_DODGE|OFF_DISARM|OFF_TRIP|ASSIST_VNUM;
    off_flags = 0;
    imm_flags = 0;
    res_flags = 0;
    vuln_flags = 0;
    start_pos = 0;
    default_pos = 0;
    race = 0;
    form = FORM_EDIBLE|FORM_SENTIENT|FORM_BIPED|FORM_MAMMAL;
    parts = PART_HEAD|PART_ARMS|PART_LEGS|PART_HEART|
                               PART_BRAINS|PART_GUTS;

    size = SIZE_MEDIUM;

    // ROM 2.4 stuff
    mob_group = 0;
    strDamType = "pound";
    anti_action = 0;
    anti_affect = 0;
    anti_offensive = 0;
    anti_immunity = 0;
    anti_resistance = 0;
    anti_vulnerability = 0;
    anti_form = 0;
    anti_parts = 0;
}

TMobile::TMobile(const TMobile& other): TVNumObj(other),
    spec_fun( other.spec_fun ),
    spec_comment( other.spec_comment ),
    player_name( other.player_name ),
    short_descr( other.short_descr ),
    long_descr( other.long_descr ),
    description( other.description ),
    mobprogs(1, 0, 5)
{
//@@@	 PShopData	pShop;
	 sex = other.sex;
	 level = other.level;
	 act = other.act;
	 affected_by = other.affected_by;
	 alignment = other.alignment;

    // The following are unused by Merc, we read and write them however
	 hitroll = other.hitroll;
	 ac = other.ac;
	 hitnodice = other.hitnodice;
	 hitsizedice = other.hitsizedice;
	 hitplus = other.hitplus;
	 damnodice = other.damnodice;
	 damsizedice = other.damsizedice;
	 damplus = other.damplus;
	 gold = other.gold;

    // ROM stuff
    for (int i = 0; i < nelems(mana); i++)
       mana[i] = other.mana[i];
    for (i = 0; i < nelems(rom_ac); i++)
       rom_ac[i] = other.rom_ac[i];
    dam_type = other.dam_type;
    off_flags = other.off_flags;
    imm_flags = other.imm_flags;
    res_flags = other.res_flags;
    vuln_flags = other.vuln_flags;
    start_pos = other.start_pos;
    default_pos = other.default_pos;
    race = other.race;
    form = other.form;
    parts = other.parts;
    size = other.size;

    // ROM 2.4 stuff
    mob_group = other.mob_group;
    strDamType = other.strDamType;
    anti_action = other.anti_action;
    anti_affect = other.anti_affect;
    anti_offensive = other.anti_offensive;
    anti_immunity = other.anti_immunity;
    anti_resistance = other.anti_resistance;
    anti_vulnerability = other.anti_vulnerability;
    anti_form = other.anti_form;
    anti_parts = other.anti_parts;

    // Rom/Envy 2
    race = other.race;
    strrace = other.strrace;

    mobprogs = other.mobprogs;      // @@@Should copy
}

TMobile::~TMobile()
{
   mobprogs.ForEach(TMobProgData::DeleteAMobProg, 0);
}

const TMobile& TMobile::operator=(const TMobile& other)
{
    spec_fun = other.spec_fun;
    spec_comment = other.spec_comment;
    player_name = other.player_name;
    short_descr = other.short_descr;
    long_descr = other.long_descr;
    description = other.description;

    vnum = other.vnum;   // @@@ Should some other function do this???

//@@@	 PShopData	pShop;
	 sex = other.sex;
	 level = other.level;
	 act = other.act;
	 affected_by = other.affected_by;
	 alignment = other.alignment;

    // The following are unused by Merc, we read and write them however
	 hitroll = other.hitroll;
	 ac = other.ac;
	 hitnodice = other.hitnodice;
	 hitsizedice = other.hitsizedice;
	 hitplus = other.hitplus;
	 damnodice = other.damnodice;
	 damsizedice = other.damsizedice;
	 damplus = other.damplus;
	 gold = other.gold;
    

    // ROM stuff
    for (int i = 0; i < nelems(mana); i++)
       mana[i] = other.mana[i];
    for (i = 0; i < nelems(rom_ac); i++)
       rom_ac[i] = other.rom_ac[i];
    dam_type = other.dam_type;
    off_flags = other.off_flags;
    imm_flags = other.imm_flags;
    res_flags = other.res_flags;
    vuln_flags = other.vuln_flags;
    start_pos = other.start_pos;
    default_pos = other.default_pos;
    race = other.race;
    form = other.form;
    parts = other.parts;
    size = other.size;

    // ROM 2.4 stuff
    mob_group = other.mob_group;
    strDamType = other.strDamType;
    anti_action = other.anti_action;
    anti_affect = other.anti_affect;
    anti_offensive = other.anti_offensive;
    anti_immunity = other.anti_immunity;
    anti_resistance = other.anti_resistance;
    anti_vulnerability = other.anti_vulnerability;
    anti_form = other.anti_form;
    anti_parts = other.anti_parts;

    // Rom/Envy 2
    race = other.race;
    strrace = other.strrace;

    mobprogs = other.mobprogs;      // @@@Should copy

    return *this;
}

const TVNumObj *TMobile::GetName(char *lpOut, int /*nOutSize*/) const
{
   wsprintf(lpOut, "#%d: ", vnum);
   if ( is_affected( AFF_INVISIBLE )   ) strcat( lpOut, "(Invis) "      );
   if ( is_affected( AFF_HIDE )        ) strcat( lpOut, "(Hide) "       );
//   if ( is_affected( AFF_CHARM )       ) strcat( lpOut, "(Charmed) "    );
   if ( is_affected( AFF_PASS_DOOR )   ) strcat( lpOut, "(Translucent) ");
   if ( is_affected( AFF_FAERIE_FIRE ) ) strcat( lpOut, "(Pink Aura) "  );
   if ( IS_EVIL( this ) )                strcat( lpOut, "(Red Aura) "   );
   if ( IS_GOOD( this ) )                strcat( lpOut, "(Blue Aura) "   );
   if ( is_affected( AFF_SANCTUARY )   ) strcat( lpOut, "(White Aura) " );

   strcat(lpOut, short_descr.c_str());

   return this;
}

char HUGE *TMobile::MobileToText(char HUGE *p) const
{
	_hstrcpy(p, description.c_str());
   p += strlen(description.c_str());
   *p++ = '\r'; *p++ = '\n';

	_hstrcpy(p, long_descr.c_str());
   p += strlen(long_descr.c_str());
   *p++ = '\r'; *p++ = '\n';

   *p++ = '\r'; *p++ = '\n';

   return p;
}

int TMobile::operator==(const TMobile& other) const
{
    // Note:  Doesn't check vnum...  Don't know if it should...

    if (spec_fun != other.spec_fun ||
        spec_comment != other.spec_comment ||
   	  player_name != other.player_name ||
   	  short_descr != other.short_descr ||
   	  long_descr != other.long_descr ||
   	  description != other.description ||
   	  sex != other.sex ||
   	  level != other.level ||
   	  act != other.act ||
   	  affected_by != other.affected_by ||
   	  alignment != other.alignment)
       return FALSE;

    // The following are unused by Merc, we read and write them however
 	 if (hitroll != other.hitroll ||
	     ac != other.ac ||
	     hitnodice != other.hitnodice ||
	     hitsizedice != other.hitsizedice ||
	     hitplus != other.hitplus ||
	     damnodice != other.damnodice ||
	     damsizedice != other.damsizedice ||
	     damplus != other.damplus ||
	     gold != other.gold)
       return FALSE;


    // ROM stuff
    for (int i = 0; i < nelems(mana); i++)
       if (mana[i] != other.mana[i])
         return FALSE;

    for (i = 0; i < nelems(rom_ac); i++)
       if (rom_ac[i] != other.rom_ac[i])
         return FALSE;

    if (dam_type != other.dam_type ||
        off_flags != other.off_flags ||
        imm_flags != other.imm_flags ||
        res_flags != other.res_flags ||
        vuln_flags != other.vuln_flags ||
        start_pos != other.start_pos ||
        default_pos != other.default_pos ||
        race != other.race ||
        form != other.form ||
        parts != other.parts ||
        size != other.size)
        return FALSE;

    // ROM 2.4 stuff
    if (mob_group != other.mob_group ||
        strDamType != other.strDamType)
        return FALSE;

    // Rom/Envy 2
    if (race != other.race ||
        strrace != other.strrace)
        return FALSE;

    // @@@ Doesn't check shops or mobprogs
//	 PShopData	pShop;
//    TArrayAsVector<TMobProgData*> mobprogs;
   return TRUE;
}

//TExtraDescr::TExtraDescr(TAreaDocument *Doc): TMercObj(Doc)
//{
//}
TExtraDescr::TExtraDescr(const TExtraDescr& other):
   keyword( other.keyword ), description( other.description )
{
}

int TExtraDescr::operator==(const TExtraDescr& other) const
{
    return keyword == other.keyword &&
       description == other.description;
}

// @@@Move these to class
static void DeleteAnExtra(TExtraDescr *& pExtra, void *)
{
   delete pExtra;
}

static void DeleteAnAffect(TAffect *& pAffect, void *)
{
   delete pAffect;
}

//TObject::TObject(TAreaDocument *Doc): TMercObj(Doc)
TObject::TObject(TAreaDocument *Doc): TVNumObj(Doc),
   extra_descr(5, 0, 5), affected(5, 0, 5)
{
//    vnum = -1;
    item_type = 0;
    extra_flags = 0;
    wear_flags = 0;
    weight = 0;
    cost = 0;
    count = 100;

    // ROM
    condition = 'P'; // Perfect condition, 100%
	 level = 0;

    // These ROM flags come from the affect structure.  We keep them here
    affected_by = 0;
    immunity = 0;
    resistance = 0;
    vulnerability = 0;
};

TObject::TObject(const TObject& other): TVNumObj(other),
    name( other.name ),
    short_descr( other.short_descr ),
    description( other.description ),
    extra_descr(5, 0, 5), affected(5, 0, 5)
{
    vnum = other.vnum; // @@@

    item_type = other.item_type;
    extra_flags = other.extra_flags;
    wear_flags = other.wear_flags;
    weight = other.weight;
    cost = other.cost;
    count = other.count;
    for (int i = 0; i < nelems(value); i++)
       value[i] = other.value[i];

   extra_descr.Flush(); // @@@ How do I delete the elements?
   for (TArrayAsVectorIterator<TExtraDescr *> it(other.extra_descr); it; it++)
      extra_descr.Add(new TExtraDescr(*it.Current()));
   affected.Flush(); // @@@ How do I delete the elements?
   for (TArrayAsVectorIterator<TAffect *> it2(other.affected); it2; it2++)
      affected.Add(new TAffect(*it2.Current()));

   // ROM
   condition = other.condition;
	material = other.material;
	level = other.level;

   // These ROM flags come from the affect structure.  We keep them here
   affected_by = other.affected_by;
   immunity = other.immunity;
   resistance = other.resistance;
   vulnerability = other.vulnerability;

   // Rom 2.4
   strType = other.strType;
}

TObject::~TObject()
{
   extra_descr.ForEach(DeleteAnExtra, 0);
   affected.ForEach(DeleteAnAffect, 0);
}

const TObject& TObject::operator=(const TObject& other)
{
    name = other.name;
    short_descr = other.short_descr;
    description = other.description;

    vnum = other.vnum; // @@@

    item_type = other.item_type;
    extra_flags = other.extra_flags;
    wear_flags = other.wear_flags;
    weight = other.weight;
    cost = other.cost;
    count = other.count;
    for (int i = 0; i < nelems(value); i++)
       value[i] = other.value[i];

   extra_descr.Flush(); // @@@ How do I delete the elements?
   for (TArrayAsVectorIterator<TExtraDescr *> it(other.extra_descr); it; it++)
      extra_descr.Add(new TExtraDescr(*it.Current()));
   affected.Flush(); // @@@ How do I delete the elements?
   for (TArrayAsVectorIterator<TAffect *> it2(other.affected); it2; it2++)
      affected.Add(new TAffect(*it2.Current()));

   // ROM
   condition = other.condition;
	material = other.material;
	level = other.level;

   // These ROM flags come from the affect structure.  We keep them here
   affected_by = other.affected_by;
   immunity = other.immunity;
   resistance = other.resistance;
   vulnerability = other.vulnerability;

   // Rom 2.4
   strType = other.strType;

   return *this;
}

const TVNumObj *TObject::GetName(char *lpOut, int /*@@@nOutSize*/) const
{
   poundsApp::currMode mode = AreaDoc->GetMode();
   wsprintf(lpOut, "#%d: ", vnum);

   if ( is_obj_stat(ITEM_INVIS) )        strcat( lpOut, "(Invis) "     );
   if ( is_obj_stat(ITEM_EVIL )   )      strcat( lpOut, "(Red Aura) "  );
   if ( is_obj_stat(ITEM_MAGIC )  )      strcat( lpOut, "(Magical) "   );
   if ( is_obj_stat(ITEM_GLOW )     )    strcat( lpOut, "(Glowing) "   );
   if ( is_obj_stat(ITEM_HUM )      )    strcat( lpOut, "(Humming) "   );
   if (mode == poundsApp::EnvyMode || mode == poundsApp::Envy2Mode)
      if ( is_obj_stat(ITEM_POISONED ) ) strcat( lpOut, "(Poisoned) "  );
   if (mode == poundsApp::TFCMode)
      if ( is_obj_stat(ITEM_METALLIC ) ) strcat( lpOut, "(Metallic) "  );

   strcat(lpOut, short_descr.c_str());
   return this;
}

char HUGE *TObject::ObjToText(char HUGE *p) const
{
	_hstrcpy(p, short_descr.c_str());
   p += strlen(short_descr.c_str());
   *p++ = '\r'; *p++ = '\n';

	_hstrcpy(p, description.c_str());
   p += strlen(description.c_str());
   *p++ = '\r'; *p++ = '\n';

   for (TArrayAsVectorIterator<TExtraDescr *> it(extra_descr); it; it++)
      {
		_hstrcpy(p, it.Current()->description.c_str());
      p += strlen(it.Current()->description.c_str());
      *p++ = '\r'; *p++ = '\n';
      }

   *p++ = '\r'; *p++ = '\n';

   return p;
}

int TObject::operator==(const TObject& other) const
{
   if (name != other.name ||
    short_descr != other.short_descr ||
    description != other.description ||
    item_type != other.item_type ||
    extra_flags != other.extra_flags ||
    wear_flags != other.wear_flags ||
    weight != other.weight ||
    // TFC only
    count != other.count ||
    // Unused
    cost != other.cost)
      return FALSE;

   for (int i = 0; i < nelems(value); i++)
      if (value[i] != other.value[i])
         return FALSE;

   // Compare Extra Descr
   int n;
   if ((n = extra_descr.GetItemsInContainer()) !=
       other.extra_descr.GetItemsInContainer())
      return FALSE;

   for (i = 0; i < n; i++)
      if (!(*extra_descr[i] == *other.extra_descr[i]))
         return FALSE;

   // Compare Affected
   if ((n = affected.GetItemsInContainer()) !=
       other.affected.GetItemsInContainer())
      return FALSE;

   for (i = 0; i < n; i++)
      if (!(*affected[i] == *other.affected[i]))
         return FALSE;


   // ROM
   if (condition != other.condition ||
       material != other.material ||
	    level != other.level)
       return FALSE;

   if (affected_by != other.affected_by ||
      immunity != other.immunity ||
      resistance != other.resistance ||
      vulnerability != other.vulnerability)
      return FALSE;

   // Rom 2.4
   if (strType != other.strType)
      return FALSE;

   return TRUE;
}

TExit::TExit(TAreaDocument *doc): TMercObj(doc)
{
   to_room = 0;
   vnum = -1;
   exit_info = 0;
   key = -1;

   InitialPosition = 0;  // No corresponding reset
   nInitialPosition = 3; // (Leave door alone at reset time)
};


int TExit::operator==(const TExit& other) const
{
    return (vnum == other.vnum &&
       exit_info == other.exit_info &&
       key == other.key &&
       keyword == other.keyword &&
       description == other.description);
    // @@@ Should I check door position?
}

TReset::TReset(TAreaDocument *doc): TMercObj(doc)
{
    command = '*';
    arg1 = arg2 = arg3 = 0;
    arg4 = -1;
}

int TReset::operator==(const TReset& other) const
{
    return (command == other.command &&
       arg1 == other.arg1 &&
       arg2 == other.arg2 &&
       arg3 == other.arg3 &&
       arg4 == other.arg4 &&
       comment == other.comment);
}

const TMercObj *TReset::GetName(char *lpOut, int /*@@@nOutSize*/) const
{
    char szMob[512];
    char szLoc[512];
    char szObj1[512];
    char szObj2[512];
    const TMobile *pMob;
    const TRoom *pLoc;
    const TObject *pObj1, *pObj2;
    static char *szRandomClass[] = { "WEAPON", "ARMOR", "POTION", "MISC" };

    switch (command)
       {
       case '*': // comment
          lpOut[0] = '\0';
          break;
       case 'M': // read a mobile
          pMob = AreaDoc->GetMobile(arg1);
          if (pMob)
             pMob->GetName(szMob, sizeof szMob);
          else
             wsprintf(szMob, "#%d: ???", arg1);

          pLoc = AreaDoc->GetRoom(arg3);
          if (pLoc)
             pLoc->GetName(szLoc, sizeof szLoc);
          else
             wsprintf(szLoc, "#%d: ???", arg3);

          // @@@ We should list the ROM 2.4 arg4 here, but we don't
          // have a pointer to the area file format...
          wsprintf(lpOut, "%s to %s (%d)", szMob, szLoc, arg2);
          break;
       case 'O': // read an object
          pObj1 = AreaDoc->GetObject(arg1);
          if (pObj1)
             pObj1->GetName(szObj1, sizeof szObj1);
          else
             wsprintf(szObj1, "#%d: ???", arg1);

          pLoc = AreaDoc->GetRoom(arg3);
          if (pLoc)
             pLoc->GetName(szLoc, sizeof szLoc);
          else
             wsprintf(szLoc, "#%d: ???", arg3);

          wsprintf(lpOut, "%s to %s", szObj1, szLoc);
          break;
       case 'P': // put object in object
          pObj1 = AreaDoc->GetObject(arg1);
          if (pObj1)
             pObj1->GetName(szObj1, sizeof szObj1);
          else
             wsprintf(szObj1, "#%d: ???", arg1);

          pObj2 = AreaDoc->GetObject(arg3);
          if (pObj2)
             pObj2->GetName(szObj2, sizeof szObj2);
          else
             wsprintf(szObj2, "#%d: ???", arg3);

          wsprintf(lpOut, "%s contains a %s", szObj2, szObj1);
          break;
       case 'G': // give object to mobile
          pObj1 = AreaDoc->GetObject(arg1);
          if (pObj1)
             pObj1->GetName(szObj1, sizeof szObj1);
          else
             wsprintf(szObj1, "#%d: ???", arg1);

          wsprintf(lpOut, "Inventory: %s (Limit: %d)", szObj1, arg2);
          break;
       case 'C': // give random object to mobile
          wsprintf(lpOut, "Inventory: RANDOM %s (%d)", szRandomClass[arg1], arg2);
          break;
       case 'E': // equip object to mobile
          pObj1 = AreaDoc->GetObject(arg1);
          if (pObj1)
             pObj1->GetName(szObj1, sizeof szObj1);
          else
             wsprintf(szObj1, "#%d: ???", arg1);

          wsprintf(lpOut, "Wields/Wears a %s at %d (Limit: %d)", szObj1, arg3,
             arg2);
          break;
       case 'F': // equip random object to mobile
          wsprintf("Wears/Wields: RANDOM %s (%d)", szRandomClass[arg1], arg2);
          break;
       case 'D': // set state of door
          pLoc = AreaDoc->GetRoom(arg1);
          if (pLoc)
             pLoc->GetName(szLoc, sizeof szLoc);
          else
             wsprintf(szLoc, "#%d: ???", arg1);

          wsprintf(lpOut, "Room %s Door %d is %d", szLoc, arg2, arg3);
          break;
       case 'R': // randomize room exits
          pLoc = AreaDoc->GetRoom(arg1);
          if (pLoc)
             pLoc->GetName(szLoc, sizeof szLoc);
          else
             wsprintf(szLoc, "#%d: ???", arg1);

          wsprintf(lpOut, "Room %s Random doors 0-%d", szLoc, arg2);
          break;
       default:
          wsprintf(lpOut, "UNKNOWN RESET %c (%d, %d, %d) ", command,
             arg1, arg2, arg3);
          break;
       };

    strcat(lpOut, comment.c_str());

    return this;
}

TReset::TReset(const TReset& other):
   // Ancestors
   TMercObj(other),

   // Members
   comment(other.comment)
{
    command = other.command;
    arg1 = other.arg1;
    arg2 = other.arg2;
    arg3 = other.arg3;
    arg4 = other.arg4;
//    comment = other.comment;
}

const TReset& TReset::operator=(const TReset& other)
{
    command = other.command;
    arg1 = other.arg1;
    arg2 = other.arg2;
    arg3 = other.arg3;
    arg4 = other.arg4;
    comment = other.comment;

    return *this;
}

TMobReset::TMobReset(const TMobReset& other):
   TReset(other),
   Wields(5, 0, 5),
   Inventory(5, 0, 5)
{
   int n = other.Wields.GetItemsInContainer();
   for (int i = 0; i < n; i++)
      Wields.Add(new TReset(*other.Wields[i]));

   n = other.Inventory.GetItemsInContainer();
   for (i = 0; i < n; i++)
      Inventory.Add(new TReset(*other.Inventory[i]));
}

const TMobReset& TMobReset::operator=(const TMobReset& other)
{
   TReset::operator=(other);

   Wields.Flush();
   int n = other.Wields.GetItemsInContainer();
   for (int i = 0; i < n; i++)
      Wields.Add(new TReset(*other.Wields[i]));

   Inventory.Flush();
   n = other.Inventory.GetItemsInContainer();
   for (i = 0; i < n; i++)
      Inventory.Add(new TReset(*other.Inventory[i]));

   return *this;
}

TMobReset::TMobReset(TAreaDocument *doc): TReset(doc),
   Wields(5, 0, 5),
   Inventory(5, 0, 5)
{
   command = 'M';
}

int TMobReset::operator==(const TMobReset& other) const
{
   if (!TReset::operator==(other))
      return FALSE;
      
   // Compare wields/inv
   int n;
   if ((n = Wields.GetItemsInContainer()) !=
       other.Wields.GetItemsInContainer())
      return FALSE;

   for (int i = 0; i < n; i++)
      if (!(*Wields[i] == *other.Wields[i]))
         return FALSE;

   if ((n = Inventory.GetItemsInContainer()) !=
       other.Inventory.GetItemsInContainer())
      return FALSE;

   for (i = 0; i < n; i++)
      if (!(*Inventory[i] == *other.Inventory[i]))
         return FALSE;

   return TRUE;
}

const TMercObj *TMobReset::GetName(char *lpOut, int nOutSize) const
{
   // @@@
   return TReset::GetName(lpOut, nOutSize);
}

//TRoom::TRoom(TAreaDocument *doc): TMercObj(doc)
TRoom::TRoom(TAreaDocument *doc): TVNumObj(doc),
   extra_descr(5, 0, 5),
   MobResets(5, 0, 5),
   ObjResets(5, 0, 5)
{
   for (int i = 0; i < nelems(is_exit); i++)
      is_exit[i] = FALSE;

//    vnum = -1;
   sector_type = 0;
   room_flags = 0;

   // ROM
   heal_rate = 100;
   mana_rate = 100;

   // Resets
   RandomDoor = 0;
//    MessageBox(0, "Room Constructed", "@@@", MB_OK);
}

const TVNumObj *TRoom::GetName(char *lpOut, int /*@@@nOutSize*/) const
{
   wsprintf(lpOut, "#%d: %s", vnum, name.c_str());
   return this;
}

char HUGE *TRoom::RoomToText(char HUGE *p) const
{
	_hstrcpy(p, name.c_str());
   p += strlen(name.c_str());
   *p++ = '\r'; *p++ = '\n';

	_hstrcpy(p, description.c_str());
   p += strlen(description.c_str());
   *p++ = '\r'; *p++ = '\n';

   for (TArrayAsVectorIterator<TExtraDescr *> it(extra_descr); it; it++)
      {
		_hstrcpy(p, it.Current()->description.c_str());
      p += strlen(it.Current()->description.c_str());
      *p++ = '\r'; *p++ = '\n';
      }

   *p++ = '\r'; *p++ = '\n';

   return p;
}

TRoom::TRoom(const TRoom& other): TVNumObj(other),
   // Members
   name( other.name ), description( other.description ),
   extra_descr(5, 0, 5),
   MobResets(5, 0, 5),
   ObjResets(5, 0, 5),

   // ROM
   clan( other.clan ),
   owner( other.owner )
{
   for (int i = 0; i < nelems(exit); i++)
      if ((is_exit[i] = other.is_exit[i]) != FALSE)
         exit[i] = other.exit[i];

   room_flags = other.room_flags;
   sector_type = other.sector_type;

   extra_descr.Flush(); // @@@ How do I delete the elements?
   for (TArrayAsVectorIterator<TExtraDescr *> it(other.extra_descr); it; it++)
      extra_descr.Add(new TExtraDescr(*it.Current()));

//   ObjResets.Flush(); // @@@ How do I delete the elements?
   for (TArrayAsVectorIterator<TReset *> it2(other.ObjResets); it2; it2++)
      ObjResets.Add(new TReset(*it2.Current()));

//   MobResets.Flush(); // @@@ How do I delete the elements?
   for (TArrayAsVectorIterator<TMobReset *> it3(other.MobResets); it3; it3++)
//      MobResets.Add(new TMobReset(*it3.Current()));
      MobResets.Add(it3.Current());

   // ROM
   heal_rate = other.heal_rate;
   mana_rate = other.mana_rate;

   // Resets
   RandomDoor = other.RandomDoor;
//   MessageBox(0, "Room Constructed from room", "@@@", MB_OK);
}

const TRoom& TRoom::operator=(const TRoom& other)
{
   vnum = other.vnum;   // @@@ Should some other function do this???

   name = other.name;
   description = other.description;

   for (int i = 0; i < nelems(exit); i++)
      if ((is_exit[i] = other.is_exit[i]) != FALSE)
         exit[i] = other.exit[i];

   room_flags = other.room_flags;
   sector_type = other.sector_type;

   extra_descr.Flush(); // @@@ How do I delete the elements
//   for (TListIteratorImp<TExtraDescr *> it(other.extra_descr); it; it++)
   for (TArrayAsVectorIterator<TExtraDescr *> it(other.extra_descr); it; it++)
      extra_descr.Add(new TExtraDescr(*it.Current()));

   ObjResets.Flush(); // @@@ How do I delete the elements?
   for (TArrayAsVectorIterator<TReset *> it2(other.ObjResets); it2; it2++)
      ObjResets.Add(new TReset(*it2.Current()));

   MobResets.Flush();
   for (TArrayAsVectorIterator<TMobReset *> it3(other.MobResets); it3; it3++)
//      MobResets.Add(new TMobReset(*it3.Current()));
      // We must duplicate the mob resets by pointer (as opposed to value), or
      // we wind up with copies that cannot be found when editing the room in
      // it's dialog.
      MobResets.Add(it3.Current());

   // ROM
   heal_rate = other.heal_rate;
   mana_rate = other.mana_rate;
   clan = other.clan;
   owner = other.owner;

   // Resets
   RandomDoor = other.RandomDoor;

   return *this;
}

TRoom::~TRoom()
{
   extra_descr.ForEach(DeleteAnExtra, 0);
}

TMobProgData::TMobProgData(TAreaDocument *doc): TMercObj(doc)
{
}

const TMercObj *TMobProgData::GetName(char *lpOut, int /*nOutSize*/) const
{
    wsprintf(lpOut, "%s %s %s", type.c_str(), arglist.c_str(), comlist.c_str());
    return this;
}

TVNumObj::TVNumObj(TAreaDocument *doc): TMercObj(doc)
{
    vnum = -1;
}

TVNumObj::TVNumObj(const TVNumObj& other):
   TMercObj( other.AreaDoc ), vnum( other.vnum )
{
}

int TRoom::operator==(const TRoom& other) const
{
    // Doesn't check vnum, don't know if it should
    if (name != other.name ||
        description != other.description ||
        room_flags != other.room_flags ||
        sector_type != other.sector_type)
       return FALSE;

    for (int i = 0; i < nelems(is_exit); i++)
       if (is_exit[i] != other.is_exit[i])
          return FALSE;

    for (i = 0; i < nelems(is_exit); i++)
       if (is_exit[i] && !(exit[i] == other.exit[i]))
          return FALSE;

    // ROM
    if (heal_rate != other.heal_rate ||
         mana_rate != other.mana_rate ||
         clan != other.clan ||
         owner != other.owner)
      return FALSE;

    // @@@ Not checking extra descr
//    TArrayAsVector<TExtraDescr*> extra_descr;
    return TRUE;
}

TMobProgData::TMobProgData(TMobProgData& other):
   TMercObj(other.AreaDoc),
   type(other.type),
   arglist(other.arglist),
   comlist(other.comlist),
   MobFile(other.MobFile)
{
}

int TMobProgData::operator==(const TMobProgData& other) const
{
   return (type == other.type &&
           arglist == other.arglist &&
           comlist == other.comlist &&
           MobFile == other.MobFile);
}

const TMobProgData& TMobProgData::operator=(const TMobProgData& other)
{
   type = other.type;
   arglist = other.arglist;
   comlist = other.comlist;
   MobFile = other.MobFile;

   return *this;
}

void TMobProgData::DeleteAMobProg(TMobProgData *& p, void *)
{
   delete p;
}

//    MPROG_DATA *next;
//    int         type;
//    string      type;
//    string      arglist;
//    string      comlist;
//    string      MobFile;   // Filename of MOBprogam

