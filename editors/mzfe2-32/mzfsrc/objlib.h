#if !defined(__objlib_h)              // Sentry, use file only if it's not already included.
#define __objlib_h

/*  Project pounds

    Copyright © 1994-6. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         objlib.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for TLibrary.
*/

#include <owl\owlpch.h>
#pragma hdrstop

// Local includes
#include "objmerc.h"

const int FLAG_TYPE = 0;
const int WORD_TYPE = 1;
const int LIST_TYPE = 2;

////////////////////////////////////////////////////////////////////////////
// SlotType
//
// Prompts for Merc object slots

struct SlotType {
   char *pszPrompt;
   int   nValueType;
};

////////////////////////////////////////////////////////////////////////////
// TTypeData
//
// Definition of a Merc object for editing

class TTypeData {
public:
   TTypeData();
   TTypeData(const char *psz1, const char *psz2, const char *psz3,
      const char *psz4, const char *psz5,
      int xn1, int xn2, int xn3, int xn4, int xn5);
   TTypeData(const SlotType Slots[5]);
   TTypeData(const TTypeData& other);

   string v0, v1, v2, v3, v4;
   int n0, n1, n2, n3, n4;
   sh_int nMercType;
};

////////////////////////////////////////////////////////////////////////////
// TLibrary (ABSTRACT)
//
// Database of Merc object values

class TLibrary {
protected:
   char m_szMode[32];
   poundsApp::currMode CurrentMode; // File format
   BOOL m_fDirty;

public:
   // Create a type library from INI file
   TLibrary(const char *pszMode, poundsApp::currMode mode);

   // Create a type library from existing type library
   TLibrary(TLibrary&);

   virtual ~TLibrary();

   // Atributes
   BOOL IsDirty() const { return m_fDirty; }

   // Assignment
   const TLibrary& operator=(/*const*/ TLibrary&);

   // Merc type number <-> ROM type name
   virtual sh_int GetTypeFromString(const string& strType) = 0;
   virtual const string& GetStringFromType(sh_int nType) = 0;

   // Created new prompts
   virtual void SetTypeInfo(const string& pszType, int nV, const char *pszPrompt, int nData) = 0;
   virtual const char *GetPromptText(const string& pszType, int nV) = 0;
   virtual int GetTypeInfo(const string& pszType, int nV) = 0;
   virtual TTypeData* GetTypeEntry(const string& pszType) = 0;

   // Maintain lists of things
   virtual BOOL AddFlag(const char *pszPrompt, int32 flag, const char *pszMeaning) = 0;
   virtual void AddListItem(const char *pszPrompt, const char *pszItemRom, int32 nItemMerc) = 0;
   virtual BOOL RemoveFlag(const char *pszPrompt, int32 flag) = 0;
//   void RemoveListItem(const char *pszPrompt, const char *pszItem);
   virtual BOOL IsListItem(const char *pszPrompt, const char *pszItem) = 0;
   virtual BOOL IsFlagItem(const char *pszPrompt, int32 flag) = 0;
   virtual BOOL ListStr2Number(const char *pszPrompt, const char *pszItem, int32 *pnSlotValue) = 0;
   virtual BOOL ListNumber2Str(const char *pszPrompt, int32 nSlotValue, char *pszItem) = 0;

   // Write the type library to the INI file
   virtual BOOL Save() = 0;

   // Read the type library from the INI file
   virtual BOOL Reload() = 0;

   // Helpers to deal with Windows interface objects.  Combo box routines
   // are not needed, as TComboBox is a TListBox descendent.
   virtual void LoadListBox(const char *pszPrompt, TListBox *pListBox) = 0;
   virtual void LoadListBoxFromFlags(const char *pszPrompt, TListBox *pListBox, int32 nFlags) = 0;

   static int Bits(int32 flag);
};
#endif
