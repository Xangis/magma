#if !defined(__tobjdia_h)              // Sentry, use file only if it's not already included.
#define __tobjdia_h

/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         tobjlib.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for TObjTypeLibrary (TLibrary)
*/

#include <owl\owlpch.h>
#pragma hdrstop

#include "objlib.h"

typedef TDIAssociation<string, TTypeData> TTypeAssoc;

class TListItem {
public:
   TListItem(): nMerc(-1) {}            // Required to be in TSetAsVector
   TListItem(int32 n, const string& str): strRom(str), nMerc(n) {}
//   TListItem(const TListItem& o): strRom(o.strRom), nMerc(o.nMerc) {}

   int operator==(const TListItem& other) const
      { return (strRom == other.strRom); }

   int32 nMerc;
   string strRom;
};

typedef TSetAsVector<TListItem> TStringSet;
typedef TSetAsVectorIterator<TListItem> TStringSetIterator;

typedef TDDAssociation<string, TStringSet> TStringAssoc;

class TMudFlag {
public:
   TMudFlag(): nFlag( -1 ) {}
   TMudFlag(int32 n, string& s): nFlag( n ), strName( s ) {}

   int operator==(const TMudFlag& other) const;

   int32 nFlag;
   string strName;
};
typedef TSetAsVector<TMudFlag> TFlagSet;
typedef TSetAsVectorIterator<TMudFlag> TFlagSetIterator;
typedef TDDAssociation<string, TFlagSet> TFlagAssoc;

struct DataType {
   const char *pszName;
   sh_int nMercType;
//   SlotType Slots[5];
};

struct ListConv {
   const char *pszName;
   int32 nMercValue;
};

struct ListItemInfo {
   const char *pszItem;
//   char **pLI;
   ListConv *pLI;
};

struct PromptFlags {
   int32 flag;
   const char *pszMeaning;
};

struct FlagPromptInfo {
   const char *pszPrompt;
   PromptFlags *pPF;
};

class TObjTypeLibrary: public TLibrary {
protected:
   TDictionary<TTypeAssoc> m_TypeDict;
   TDictionary<TStringAssoc> m_ListDict;
   TDictionary<TFlagAssoc> m_FlagDict;

   void ReloadList(const char *pszPrompt);
   void ReloadFlag(const char *pszPrompt);

   void DefaultTypes (DataType[], SlotType[][5]);
   void DefaultFlags (FlagPromptInfo *);
   void DefaultLists (ListItemInfo *);

   void CreateLibraryFromTemplate();

public:
   // Create a type library from INI file
   TObjTypeLibrary(const char *pszMode, poundsApp::currMode mode);

   // Create a type library from existing type library
   TObjTypeLibrary(TObjTypeLibrary&);

   virtual ~TObjTypeLibrary();

   // Atributes
   BOOL IsDirty() const { return m_fDirty; }

   // Assignment
   const TObjTypeLibrary& operator=(/*const*/ TObjTypeLibrary&);

   // Merc type number <-> ROM type name
   virtual sh_int GetTypeFromString(const string& strType);
   virtual const string& GetStringFromType(sh_int nType);

   // Created new prompts
   virtual void SetTypeInfo(const string& pszType, int nV, const char *pszPrompt, int nData);
   virtual const char *GetPromptText(const string& pszType, int nV);
   virtual int GetTypeInfo(const string& pszType, int nV);
   virtual TTypeData* GetTypeEntry(const string& pszType);

   // Maintain lists of things
   virtual BOOL AddFlag(const char *pszPrompt, int32 flag, const char *pszMeaning);
   virtual void AddListItem(const char *pszPrompt, const char *pszItemRom, int32 nItemMerc);
   virtual BOOL RemoveFlag(const char *pszPrompt, int32 flag);
//   void RemoveListItem(const char *pszPrompt, const char *pszItem);
   virtual BOOL IsListItem(const char *pszPrompt, const char *pszItem);
   virtual BOOL IsFlagItem(const char *pszPrompt, int32 flag);
   virtual BOOL ListStr2Number(const char *pszPrompt, const char *pszItem, int32 *pnSlotValue);
   virtual BOOL ListNumber2Str(const char *pszPrompt, int32 nSlotValue, char *pszItem);

   // Write the type library to the INI file
   virtual BOOL Save();

   // Read the type library from the INI file
   virtual BOOL Reload();

   // Helpers to deal with Windows interface objects.  Combo box routines
   // are not needed, as TComboBox is a TListBox descendent.
   virtual void LoadListBox(const char *pszPrompt, TListBox *pListBox);
//   void LoadComboBox(const char *pszPrompt, TComboBox *pComboBox);
   virtual void LoadListBoxFromFlags(const char *pszPrompt, TListBox *pListBox, int32 nFlags);
//   void LoadComboBoxFromFlags(const char *pszPrompt, TComboBox *pComboBox, int32 nFlags);

   static int Bits(int32 flag);
};
#endif
