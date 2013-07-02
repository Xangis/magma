/*  Project pounds

    Copyright © 1994-6. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         objdia2.cpp (2 of 3)
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file for implementation of TObjDialog (TDialog) and related.
*/

#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

// Classlib includes
#include <classlib\assoc.h>
#include <classlib\dict.h>
#include <classlib\sets.h>

// OWL includes
#include <owl\inputdia.h>

// C includes
#include <malloc.h>

// Local includes
#include "objdia.h"
//#include "xtradia.h"
#include "appdia.h"
//#include "mobdia.h"
#include "tobjlib.h"

BOOL TObjTypeLibrary::ListStr2Number(const char *pszPrompt, const char *pszItem, int32 *pnSlotValue)
{
   TStringAssoc assoc(pszPrompt, 0);
   TStringAssoc *pAssoc = m_ListDict.Find(assoc);
   if (!pAssoc) {
      // This category is not even defined.
      return FALSE;
   }

   const TStringSet& set = pAssoc->Value();
//   string strItem(pszItem);
//   const string *pstr = set.Find(strItem);
   TListItem strItem(0, pszItem);
   const TListItem *pstr = set.Find(strItem);
   if (!pstr) {
      return FALSE;
   }

   *pnSlotValue = pstr->nMerc;
   return TRUE;
}

BOOL TObjTypeLibrary::ListNumber2Str(const char *pszPrompt, int32 nSlotValue, char *pszItem)
{
   TStringAssoc assoc(pszPrompt, 0);
   TStringAssoc *pAssoc = m_ListDict.Find(assoc);
   if (!pAssoc) {
      // This category is not even defined.
      return FALSE;
   }

   const TStringSet& set = pAssoc->Value();
   for (TStringSetIterator it(set); it; it++) {
      if (it.Current().nMerc == nSlotValue) {
         strcpy(pszItem, it.Current().strRom.c_str());
         return TRUE;
      }
   }

   return FALSE;
}

void TObjTypeLibrary::ReloadList(const char *pszPrompt)
{
#ifdef __WIN32__
   // The first declaration was GPFing with BC4.02/NT3.51.  The alloca()
   // seems to work better, though I don't know why...
//   char szBuffer[10240];
   char *szBuffer = (char *) alloca(10240);
   char szSection[64];
   wsprintf(szSection, "%s-%s", m_szMode, pszPrompt);
//   GetPrivateProfileSection(szSection, szBuffer, sizeof szBuffer, "mzf.ini");
   GetPrivateProfileSection(szSection, szBuffer, 10240, "mzf.ini");

   TStringSet& set = *new TStringSet;
   BOOL f = FALSE;
   char *p = szBuffer;
   while (*p) {
      char *pEqual = strchr(p, '=');
      if (pEqual) {
         pEqual++;
         f = TRUE;
         char *pColon = strchr(p, ':');
         int32 nMerc = 0;
         if (pColon) {
            *pColon++ = '\0';
            nMerc = atol(pColon);
         }
//         set.Add(*new string(pEqual));
         set.Add(*new TListItem(nMerc, pEqual));
      }

      p += strlen(p)+1;
   }
#else
   TStringSet& set = *new TStringSet;

   char szSection[64];
   wsprintf(szSection, "%s-%s", m_szMode, pszPrompt);

   BOOL f = FALSE;
   for (int nItem = 1; ; nItem++) {
      char szItem[16];
      wsprintf(szItem, "%d", nItem);
      char szListItem[128];
      if (GetPrivateProfileString(szSection, szItem, "",
         szListItem, sizeof szListItem, "mzf.ini") == 0)
         break;

      f = TRUE;
      char *pColon = strchr(szListItem, ':');
      int32 nMerc = 0;
      if (pColon) {
         *pColon++ = '\0';
         nMerc = atol(pColon);
      }

      set.Add(*new TListItem(nMerc, szListItem));
   }
#endif

   // Did we find anything?
   if (f) {
      // Add the list we have built
      m_ListDict.Add(*new TStringAssoc(*new string(pszPrompt), set));
   } else
      // There is nothing here, no point in keeping an empty list around...
      delete &set;
}

void TObjTypeLibrary::ReloadFlag(const char *pszPrompt)
{
#ifdef __WIN32__
   char szBuffer[10240]; // @@@ Big enough?
   char szSection[64];
   wsprintf(szSection, "%s-%s", m_szMode, pszPrompt);
   GetPrivateProfileSection(szSection, szBuffer, sizeof szBuffer, "mzf.ini");

   TFlagSet& set = *new TFlagSet;
   BOOL f = FALSE;
   char *p = szBuffer;
   while (*p) {
      char *pEqual = strchr(p, '=');
      if (pEqual) {
         *pEqual++ = '\0';
         f = TRUE;

         int32 n;
         if (CurrentMode == poundsApp::RomMode ||
             CurrentMode == poundsApp::Rom24Mode)
            // It might be good to verify that p is a ROM flag here...
            n = TAreaDocument::flag_convert(*p);
         else
            n = atol(p);

         set.Add(*new TMudFlag(n, *new string(pEqual)));
      }

      p += strlen(p)+1;
   }
#else
   TFlagSet& set = *new TFlagSet;
   BOOL f = FALSE;

   char szSection[64];
   wsprintf(szSection, "%s-%s", m_szMode, pszPrompt);

   int32 nItem = 1;
   for (int n = 1; n<32; nItem <<= 1, n++) {
      char szItem[64];
      TAreaDocument::Flags2Str(CurrentMode, szItem, nItem);

      char szFlagItem[128];
      if (GetPrivateProfileString(szSection, szItem, "",
         szFlagItem, sizeof szFlagItem, "mzf.ini") == 0)
         break;

      f = TRUE;
      set.Add(*new TMudFlag(nItem, *new string(szFlagItem)));
   }
#endif

   // Did we find anything?
   if (f) {
      // Add the list we have built
      m_FlagDict.Add(*new TFlagAssoc(*new string(pszPrompt), set));
   } else
      // There is nothing here, no point in keeping an empty list around...
      delete &set;
}

#if 0
void TObjTypeLibrary::LoadComboBox(const char *pszPrompt, TComboBox *pComboBox)
{
   // Find out the list of values
   TStringAssoc assoc(pszPrompt, 0);
   TStringAssoc *pAssoc = m_ListDict.Find(assoc);
   if (!pAssoc) {
      // This category is not even defined.
      return;
   }

   for (TStringSetIterator itList(pAssoc->Value()); itList; itList++)
//      pComboBox->AddString(itList.Current().c_str());
      pComboBox->AddString(itList.Current().strRom.c_str());
}
#endif

void TObjTypeLibrary::LoadListBoxFromFlags(const char *pszPrompt, TListBox *pListBox, int32 nFlags)
{
   // Find out the list of values
   TFlagAssoc assoc(pszPrompt, 0);
   TFlagAssoc *pAssoc = m_FlagDict.Find(assoc);
   if (!pAssoc) {
      // This category is not even defined.
      return;
   }

   for (TFlagSetIterator itList(pAssoc->Value()); itList; itList++) {
      int n = pListBox->AddString(itList.Current().strName.c_str());
      if (n != LB_ERR) {
         int32 nF = itList.Current().nFlag;
         pListBox->SetItemData(n, nF);
         if (nF & nFlags)
            pListBox->SetSel(n, TRUE);
      }
   }

   // Place the cursor focus on the first item for keyboard users
   pListBox->SetCaretIndex(0, FALSE);
}

#if 0
void TObjTypeLibrary::LoadComboBoxFromFlags(const char *pszPrompt, TComboBox *pComboBox, int32 nFlags)
{
   // Find out the list of values
   TFlagAssoc assoc(pszPrompt, 0);
   TFlagAssoc *pAssoc = m_FlagDict.Find(assoc);
   if (!pAssoc) {
      // This category is not even defined.
      return;
   }

   for (TFlagSetIterator itList(pAssoc->Value()); itList; itList++) {
      int n = pComboBox->AddString(itList.Current().strName.c_str());
      if (n != CB_ERR) {
         int32 nF = itList.Current().nFlag;
         pComboBox->SetItemData(n, nF);
         if (nF & nFlags)
            pComboBox->SetSel(n, TRUE);
      }
   }
}
#endif

void TObjTypeLibrary::LoadListBox(const char *pszPrompt, TListBox *pListBox)
{
   // Find out the list of values
   TStringAssoc assoc(pszPrompt, 0);
   TStringAssoc *pAssoc = m_ListDict.Find(assoc);
   if (!pAssoc) {
      // This category is not even defined.
      return;
   }

   for (TStringSetIterator itList(pAssoc->Value()); itList; itList++)
//      pListBox->AddString(itList.Current().c_str());
      pListBox->AddString(itList.Current().strRom.c_str());
}

// Returns the number of bits in a mask of flags
int TObjTypeLibrary::Bits(int32 flag)
{
   int nBits = 0;
   while (flag) {
      if (flag&1)
         nBits++;
      flag >>= 1;
   }

   return nBits;
}

BOOL TObjTypeLibrary::AddFlag(const char *pszPrompt, int32 flag,
   const char *pszMeaning)
{
   if (Bits(flag) != 1)
      return FALSE;

   // See if there is a flag already in existence
   TFlagAssoc assoc(pszPrompt, 0);
   TFlagAssoc *pAssoc = m_FlagDict.Find(assoc);
   if (!pAssoc) {
      // This is the first occurance of this prompt, so we need a new
      // flag set
      pAssoc = new TFlagAssoc(*new string(pszPrompt), *new TFlagSet);
      if (!m_FlagDict.Add(*pAssoc))
         return FALSE;
   }

   // Add this value to our set
   TFlagSet& set = *(TFlagSet*)& pAssoc->Value();  // @@@CONST-CAST
   return set.Add(*new TMudFlag(flag, *new string(pszMeaning)));
}

BOOL TObjTypeLibrary::RemoveFlag(const char *pszPrompt, int32 flag)
{
   TFlagAssoc assoc(pszPrompt, 0);
   TFlagAssoc *pAssoc = m_FlagDict.Find(assoc);
   // Does this prompt even exist?
   if (!pAssoc)
      return FALSE;

   // The prompt exists.  But the set isn't a dictionary, we have to
   // cycle through it looking for the element to delete
   for (TFlagSetIterator itList(pAssoc->Value()); itList; itList++) {
      if (itList.Current().nFlag == flag) {
         TFlagSet *pSet =  const_cast<TFlagSet*>(&pAssoc->Value());
         return pSet->Detach(itList.Current());
      }
   }

   // We never found the flag we were looking for
   return FALSE;
}

BOOL TObjTypeLibrary::IsFlagItem(const char *pszPrompt, int32 flag)
{
//#ifdef _DEBUG
   char szBuffer[256];
//#endif

   TFlagAssoc assoc(pszPrompt, 0);
   TFlagAssoc *pAssoc = m_FlagDict.Find(assoc);
   if (!pAssoc) {
      // This category is not even defined.
//#ifdef _DEBUG
      wsprintf(szBuffer, "%s is not defined as a list category.", pszPrompt);
      MessageBox(0, szBuffer, "M.Z.F", MB_ICONHAND|MB_OK);
//#endif
      return FALSE;
   }

   const TFlagSet& set = pAssoc->Value();
   TMudFlag flagItem(flag, string(""));
   const TMudFlag *pflag = set.Find(flagItem);
   if (!pflag) {
//#ifdef _DEBUG
      wsprintf(szBuffer, "0x%x is not defined in category %s", flag, pszPrompt);
      MessageBox(0, szBuffer, "M.Z.F.", MB_ICONHAND|MB_OK);
//#endif
      return FALSE;
   }

   return TRUE;
}

sh_int TObjTypeLibrary::GetTypeFromString(const string& strType)
{
   TTypeAssoc assoc(strType, 0);
   TTypeAssoc *pAssoc = m_TypeDict.Find(assoc);

   // If no such string is in the dictionary, return 'unknown type' code
   if (!pAssoc)
      return 0;

   return pAssoc->Value()->nMercType;
}

const string& TObjTypeLibrary::GetStringFromType(sh_int nType)
{
// This can't be used, because it forces a temporary to be created,
// otherwise it is just another name for TDictionaryAsHashTableIterator
//   TDictionaryIterator<TTypeAssoc> itDict(m_TypeDict);

   TDictionaryAsHashTableIterator<TTypeAssoc> itDict(m_TypeDict);
   while (itDict) {
      const TTypeAssoc *pAssoc = &itDict.Current();
      if (pAssoc->Value()->nMercType == nType) {
         return pAssoc->Key();
      }

      itDict++;
   }

   static string strNull("");
   return strNull;
}

////////////////////////////////////////////////////////////////////////////
TTypeData::TTypeData(const char *psz1, const char *psz2, const char *psz3,
      const char *psz4, const char *psz5,
      int xn1, int xn2, int xn3, int xn4, int xn5):
   v0( psz1 ), v1( psz2 ), v2( psz3 ), v3( psz4 ), v4( psz5 )
{
   n0 = xn1; n1 = xn2; n2 = xn3; n3 = xn4; n4 = xn5;

//   nMercType = ITEM_FURNITURE;
   nMercType = 0;
}

TTypeData::TTypeData(const TTypeData& other):
   v0( other.v0 ), v1( other.v1 ), v2( other.v2 ), v3( other.v3 ), v4( other.v4 )
{
   n0 = other.n0; n1 = other.n1; n2 = other.n2; n3 = other.n3; n4 = other.n4;
   nMercType = other.nMercType;
}

TTypeData::TTypeData(const SlotType Slots[5]):
   v0( Slots[0].pszPrompt ),
   v1( Slots[1].pszPrompt ),
   v2( Slots[2].pszPrompt ),
   v3( Slots[3].pszPrompt ),
   v4( Slots[4].pszPrompt )
{
   n0 = Slots[0].nValueType;
   n1 = Slots[1].nValueType;
   n2 = Slots[2].nValueType;
   n3 = Slots[3].nValueType;
   n4 = Slots[4].nValueType;
}

TTypeData::TTypeData()
{
   n0 = n1 = n2 = n3 = n4 = -1;

   nMercType = ITEM_FURNITURE;
}



////////////////////////////////////////////////////////////////////////////
// TLibrary (ABSTRACT)


// Create a type library from INI file
TLibrary::TLibrary(const char *pszMode, poundsApp::currMode mode)
{
   strcpy(m_szMode, pszMode);
   CurrentMode = mode;
   m_fDirty = FALSE;
}

// Create a type library from existing type library
TLibrary::TLibrary(TLibrary& other)
{
   CurrentMode = other.CurrentMode;
   strcpy(m_szMode, other.m_szMode);
   m_fDirty = other.m_fDirty;
}

TLibrary::~TLibrary()
{
}

// Assignment
const TLibrary& TLibrary::operator=(/*const*/ TLibrary& other)
{
   CurrentMode = other.CurrentMode;
   strcpy(m_szMode, other.m_szMode);
   m_fDirty = other.m_fDirty;

   return *this;
}

////////////////////////////////////////////////////////////////////////////
//

TLibrary* CreateLibrary(poundsApp::currMode mode)
{
    return new TObjTypeLibrary(poundsApp::szModesShort[mode],
      mode);
}


////////////////////////////////////////////////////////////////////////////
int TMudFlag::operator==(const TMudFlag& other) const
{
   return (nFlag == other.nFlag);
}


