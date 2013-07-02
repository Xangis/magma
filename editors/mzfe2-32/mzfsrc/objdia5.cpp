/*  Project pounds

    Copyright © 1994-6. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         objdia5.cpp (5 of 5)
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

// Create a type library from INI file
TObjTypeLibrary::TObjTypeLibrary(const char *pszMode, poundsApp::currMode mode):
   TLibrary(pszMode, mode)
{
//   strcpy(m_szMode, pszMode);
//   CurrentMode = mode;
//   m_fDirty = FALSE;

   Reload();
}

TObjTypeLibrary::~TObjTypeLibrary()
{
   // @@@ Should delete stuff here, I think...
}

BOOL TObjTypeLibrary::Save()
{
   if (!m_fDirty)
      return FALSE;

   string strMode( m_szMode );
   string strType[4];

   strType[0] = "";
   strType[1] = ":flag";
   strType[2] = ":word";
   strType[3] = ":list";

   // Write the names of all of the types.  The order doesn't matter.
   int i = 1;
   for (TDictionaryAsHashTableIterator<TTypeAssoc> it(m_TypeDict); it; it++) {
      // Write the main entry for this type
      char szBuffer[32];
      wsprintf(szBuffer, "type%d", i++);
      // If the ROM object has a Merc type, we write that out after a
      // colon, so we will be able to translate from ROM types to Merc
      // types.
      char szBuffer2[64];
      sh_int nMercType = it.Current().Value()->nMercType;
      const char *pszType = it.Current().Key().c_str();
      if (nMercType)
         wsprintf(szBuffer2, "%s:%d", pszType, nMercType);
      else
         strcpy(szBuffer2, pszType);
      WritePrivateProfileString(strMode.c_str(), szBuffer, szBuffer2,
         "mzf.ini");

      // Write the slot names for this type
      WritePrivateProfileString((strMode + "-" + it.Current().Key()).c_str(), "v0",
         (it.Current().Value()->v0 + strType[it.Current().Value()->n0+1]).c_str(), "mzf.ini");
      WritePrivateProfileString((strMode + "-" + it.Current().Key()).c_str(), "v1",
         (it.Current().Value()->v1 + strType[it.Current().Value()->n1+1]).c_str(), "mzf.ini");
      WritePrivateProfileString((strMode + "-" + it.Current().Key()).c_str(), "v2",
         (it.Current().Value()->v2 + strType[it.Current().Value()->n2+1]).c_str(), "mzf.ini");
      WritePrivateProfileString((strMode + "-" + it.Current().Key()).c_str(), "v3",
         (it.Current().Value()->v3 + strType[it.Current().Value()->n3+1]).c_str(), "mzf.ini");
      WritePrivateProfileString((strMode + "-" + it.Current().Key()).c_str(), "v4",
         (it.Current().Value()->v4 + strType[it.Current().Value()->n4+1]).c_str(), "mzf.ini");
   }

   // Write all of the lists
   for (TDictionaryAsHashTableIterator<TStringAssoc> it2(m_ListDict); it2; it2++) {
      const TStringAssoc& assoc = it2.Current();
      char szSection[64];
      wsprintf(szSection, "%s-%s", strMode.c_str(), assoc.Key().c_str());
      i = 1;
      for (TStringSetIterator itList(assoc.Value()); itList; itList++) {
         char szKey[32];
         wsprintf(szKey, "%d", i++);
         char szValue[128];
         wsprintf(szValue, "%s:%d", itList.Current().strRom.c_str(), itList.Current().nMerc);
         WritePrivateProfileString(szSection, szKey, szValue, "mzf.ini");
//         WritePrivateProfileString(szSection, szKey, itList.Current().c_str(), "mzf.ini");
      }
   }

   // Write out all of the flags
   for (TDictionaryAsHashTableIterator<TFlagAssoc> it3(m_FlagDict); it3; it3++) {
      const TFlagAssoc& assoc = it3.Current();
      char szSection[64];
      wsprintf(szSection, "%s-%s", strMode.c_str(), assoc.Key().c_str());
      for (TSetAsVectorIterator<TMudFlag> itList(assoc.Value()); itList; itList++) {
         char szFlag[32];
         TAreaDocument::Flags2Str(CurrentMode, szFlag,
            itList.Current().nFlag);
         WritePrivateProfileString(szSection, szFlag,
            itList.Current().strName.c_str(), "mzf.ini");
      }
   }

   // Flush the .ini cache
   WritePrivateProfileString(0, 0, 0, "mzf.ini");

   return TRUE;
}

BOOL TObjTypeLibrary::Reload()
{
#ifdef __WIN32__
   char szBuffer[1024];
   if (GetPrivateProfileSection(m_szMode, szBuffer, sizeof szBuffer,
      "mzf.ini") == 0) {
      CreateLibraryFromTemplate();
      return FALSE;
   }
//   MessageBox(0, szBuffer, "@@@", MB_OK);

   // Load the types
   char *p = szBuffer;
   while (*p) {
      char *pEqual = strchr(p, '=');
      if (pEqual) {
         pEqual++;

         // Now, see if there is a :merctype defined
         char *pColonType = strchr(pEqual, ':');
         if (pColonType) {
            *pColonType++ = '\0';
         }
//         MessageBox(0, pEqual, "@@@", MB_OK);

         // First, lets see if this type is defined.  If it is, load it.
         char szType[64];
         strcpy(szType, m_szMode);
         strcat(szType, "-");
         strcat(szType, pEqual);
         char v[5][64];
         GetPrivateProfileString(szType, "v0", "", v[0], sizeof v[0], "mzf.ini");
         GetPrivateProfileString(szType, "v1", "", v[1], sizeof v[1], "mzf.ini");
         GetPrivateProfileString(szType, "v2", "", v[2], sizeof v[2], "mzf.ini");
         GetPrivateProfileString(szType, "v3", "", v[3], sizeof v[3], "mzf.ini");
         GetPrivateProfileString(szType, "v4", "", v[4], sizeof v[4], "mzf.ini");

         // Now, go through and determine types
         char *pColon;
         int n[5];
         for (int i = 0; i < nelems(n); i++) {
            pColon = strchr(v[i], ':');
            n[i] = -1;
            if (pColon) {
               *pColon++ = '\0';
               if (stricmp(pColon, "flag") == 0) {
                  n[i] = FLAG_TYPE;
                  ReloadFlag(v[i]);
               } else if (stricmp(pColon, "word") == 0) {
                  n[i] = WORD_TYPE;
               } else if (stricmp(pColon, "list") == 0) {
                  n[i] = LIST_TYPE;
//                  size_t nTest = stackavail();
//                  char szTest[256];
//                  wsprintf(szTest, "stack avail=%d", nTest);
//                  MessageBox(0, szTest, "@@@", MB_OK);
                  ReloadList(v[i]);
               }
            }
         }

         TTypeData *ptd = new TTypeData(v[0], v[1], v[2], v[3], v[4],
            n[0], n[1], n[2], n[3], n[4]);
         if (pColonType)
            ptd->nMercType = (sh_int) atoi(pColonType);

         TTypeAssoc *pAssoc = new TTypeAssoc(string(pEqual), ptd);
         m_TypeDict.Add(*pAssoc);
      }

      p += strlen(p)+1;
   }

   return TRUE;
#else
   //
   for (int nType = 1; ; nType++) {
      char szEntry[64];
      wsprintf(szEntry, "type%d", nType);
      char szRawType[64];
      int nLen = GetPrivateProfileString(m_szMode, szEntry, "",
         szRawType, sizeof szRawType, "mzf.ini");
      if (nLen == 0) {
         // We didn't find this type.  Did we find anything at all?
         if (nType == 1) {
            // We found nothing.  Must be a codebase we haven't used before
            CreateLibraryFromTemplate();
            return FALSE;
         }

         // We are out of types.  Give up.
         return TRUE;
      }

      // Now, see if there is a :merctype defined
      char *pColonType = strchr(szRawType, ':');
      if (pColonType)
         *pColonType++ = '\0';

      // First, lets see if this type is defined.  If it is, load it.
      char szType[64];
      strcpy(szType, m_szMode);
      strcat(szType, "-");
      strcat(szType, szRawType);
      char v[5][64];
      GetPrivateProfileString(szType, "v0", "", v[0], sizeof v[0], "mzf.ini");
      GetPrivateProfileString(szType, "v1", "", v[1], sizeof v[1], "mzf.ini");
      GetPrivateProfileString(szType, "v2", "", v[2], sizeof v[2], "mzf.ini");
      GetPrivateProfileString(szType, "v3", "", v[3], sizeof v[3], "mzf.ini");
      GetPrivateProfileString(szType, "v4", "", v[4], sizeof v[4], "mzf.ini");

      // Now, go through and determine types
      char *pColon;
      int n[5];
      for (int i = 0; i < nelems(n); i++) {
         pColon = strchr(v[i], ':');
         n[i] = -1;
         if (pColon) {
            *pColon++ = '\0';
            if (stricmp(pColon, "flag") == 0) {
               n[i] = FLAG_TYPE;
               ReloadFlag(v[i]);
            } else if (stricmp(pColon, "word") == 0) {
               n[i] = WORD_TYPE;
            } else if (stricmp(pColon, "list") == 0) {
               n[i] = LIST_TYPE;
               ReloadList(v[i]);
            }
         }
      }

      TTypeData *ptd = new TTypeData(v[0], v[1], v[2], v[3], v[4],
            n[0], n[1], n[2], n[3], n[4]);
      if (pColonType)
            ptd->nMercType = (sh_int) atoi(pColonType);

      TTypeAssoc *pAssoc = new TTypeAssoc(string(szRawType), ptd);
      m_TypeDict.Add(*pAssoc);
   }
#endif
}


