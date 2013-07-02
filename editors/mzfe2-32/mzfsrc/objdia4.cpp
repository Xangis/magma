/*  Project pounds

    Copyright © 1994-6. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         objdia4.cpp (4 of 4)
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
//#include <owl\inputdia.h>

// C includes
//#include <malloc.h>

// Local includes
#include "objdia.h"
#include "appdia.h"
#include "tobjlib.h"

////////////////////////////////////////////////////////////////////////////
static unsigned HashValue( const string& t )
{
    return t.hash();
}

const char *TObjTypeLibrary::GetPromptText(const string& pszType, int nV)
{
   TTypeAssoc assoc(pszType, 0);
   TTypeAssoc *pAssoc = m_TypeDict.Find(assoc);
   if (!pAssoc)
      return "? Type";

   switch (nV) {
   case 0:
      return pAssoc->Value()->v0.c_str();
   case 1:
      return pAssoc->Value()->v1.c_str();
   case 2:
      return pAssoc->Value()->v2.c_str();
   case 3:
      return pAssoc->Value()->v3.c_str();
   case 4:
      return pAssoc->Value()->v4.c_str();
   }

   return "? V";
}

int TObjTypeLibrary::GetTypeInfo(const string& pszType, int nV)
{
   TTypeAssoc assoc(pszType, 0);
   TTypeAssoc *pAssoc = m_TypeDict.Find(assoc);
   if (!pAssoc)
      return -1;

   switch (nV) {
   case 0:
      return pAssoc->Value()->n0;
   case 1:
      return pAssoc->Value()->n1;
   case 2:
      return pAssoc->Value()->n2;
   case 3:
      return pAssoc->Value()->n3;
   case 4:
      return pAssoc->Value()->n4;
   }

   return -1;
}

TTypeData* TObjTypeLibrary::GetTypeEntry(const string& pszType)
{
   TTypeAssoc assoc(pszType, 0);
   TTypeAssoc *pAssoc = m_TypeDict.Find(assoc);
   if (!pAssoc)
      return 0;

   return const_cast<TTypeData*>(pAssoc->Value());
}

// Create a type library from existing type library
TObjTypeLibrary::TObjTypeLibrary(TObjTypeLibrary& other):
   TLibrary(other)
{
//   CurrentMode = other.CurrentMode;
//   strcpy(m_szMode, other.m_szMode);
//   m_fDirty = FALSE;

   for (TDictionaryAsHashTableIterator<TTypeAssoc> it(other.m_TypeDict); it; it++)
      m_TypeDict.Add(*new TTypeAssoc(it.Current().Key(), new TTypeData(*it.Current().Value())));

   for (TDictionaryAsHashTableIterator<TStringAssoc> it2(other.m_ListDict); it2; it2++) {
      TStringSet& set = *new TStringSet;
      for (TStringSetIterator itList(it2.Current().Value()); itList; itList++)
         set.Add(*new TListItem(itList.Current()));
//         set.Add(*new string(itList.Current()));
      m_ListDict.Add(*new TStringAssoc(it2.Current().Key(), set));
   }

   for (TDictionaryAsHashTableIterator<TFlagAssoc> it3(other.m_FlagDict); it3; it3++)
      m_FlagDict.Add(*new TFlagAssoc(it3.Current().Key(), it3.Current().Value()));
}

// Assignment
const TObjTypeLibrary& TObjTypeLibrary::operator=(/*const*/ TObjTypeLibrary& other)
{
   TLibrary::operator=(other);

//   m_fDirty = TRUE;
//   CurrentMode = other.CurrentMode;
//   strcpy(m_szMode, other.m_szMode);

   m_TypeDict.Flush();
   for (TDictionaryAsHashTableIterator<TTypeAssoc> it(other.m_TypeDict); it; it++)
      m_TypeDict.Add(*new TTypeAssoc(it.Current().Key(), new TTypeData(*it.Current().Value())));
//      m_TypeDict.Add(*new TTypeAssoc(it.Current()));

   m_ListDict.Flush();
   for (TDictionaryAsHashTableIterator<TStringAssoc> it2(other.m_ListDict); it2; it2++) {
      TStringSet& set = *new TStringSet;
      for (TStringSetIterator itList(it2.Current().Value()); itList; itList++)
         set.Add(*new TListItem(itList.Current()));
//         set.Add(*new string(itList.Current()));
      m_ListDict.Add(*new TStringAssoc(it2.Current().Key(), set));
   }

   m_FlagDict.Flush();
   for (TDictionaryAsHashTableIterator<TFlagAssoc> it3(other.m_FlagDict); it3; it3++)
      m_FlagDict.Add(*new TFlagAssoc(it3.Current().Key(), it3.Current().Value()));

   return *this;
}

void TObjTypeLibrary::SetTypeInfo(const string& pszType, int nV,
   const char *pszPrompt, int nData)
{
   m_fDirty = TRUE;

   TTypeAssoc assoc(pszType, 0);
   TTypeAssoc *pAssoc = m_TypeDict.Find(assoc);
   if (!pAssoc) {
      pAssoc = new TTypeAssoc(pszType, new TTypeData);
      m_TypeDict.Add(*pAssoc);
   }

   // @@@ const-cast
   TTypeData *pData = (TTypeData *) pAssoc->Value();

   switch (nV) {
   case 0:
      pData->v0 = pszPrompt;
      pData->n0 = nData;
      break;
   case 1:
      pData->v1 = pszPrompt;
      pData->n1 = nData;
      break;
   case 2:
      pData->v2 = pszPrompt;
      pData->n2 = nData;
      break;
   case 3:
      pData->v3 = pszPrompt;
      pData->n3 = nData;
      break;
   case 4:
      pData->v4 = pszPrompt;
      pData->n4 = nData;
      break;
   }
}

void TObjTypeLibrary::AddListItem(const char *pszPrompt, const char *pszItemRom, int32 nItemMerc)
{
   m_fDirty = TRUE;

   TStringAssoc assoc(pszPrompt, 0);
   TStringAssoc *pAssoc = m_ListDict.Find(assoc);
   if (!pAssoc) {
      // This category was not defined.  Define it.
      pAssoc = new TStringAssoc(*new string(pszPrompt), *new TStringSet);
      m_ListDict.Add(*pAssoc);
   }

   TStringSet& set = (TStringSet&) pAssoc->Value(); // @@@ CONST-cast
//   string& strItem = *new string(pszItem);
   TListItem& strItem = *new TListItem(nItemMerc, pszItemRom);
   if (!set.Add(strItem)) {
      delete &strItem;
   }
}

BOOL TObjTypeLibrary::IsListItem(const char *pszPrompt, const char *pszItem)
{
#ifdef _DEBUG
   char szBuffer[256];
#endif

   TStringAssoc assoc(pszPrompt, 0);
   TStringAssoc *pAssoc = m_ListDict.Find(assoc);
   if (!pAssoc) {
      // This category is not even defined.
#ifdef _DEBUG
      wsprintf(szBuffer, "%s is not defined as a list category.", pszPrompt);
      MessageBox(0, szBuffer, "M.Z.F", MB_ICONHAND|MB_OK);
#endif
      return FALSE;
   }

   const TStringSet& set = pAssoc->Value();
//   string strItem(pszItem);
//   const string *pstr = set.Find(strItem);
   TListItem strItem(0, pszItem);
   const TListItem *pstr = set.Find(strItem);
   if (!pstr) {
#ifdef _DEBUG
      wsprintf(szBuffer, "%s is not defined in category %s", pszItem, pszPrompt);
      MessageBox(0, szBuffer, "M.Z.F.", MB_ICONHAND|MB_OK);
#endif
      return FALSE;
   }

   return TRUE;
}

