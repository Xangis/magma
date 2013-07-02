#if !defined(__mobprog_h)              // Sentry, use file only if it's not already included.
#define __mobprog_h

/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         mobprog.h
	 AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for TMobProgDia (TDialog).      
*/

#include <owl\owlpch.h>
#pragma hdrstop

#include <owl\static.h>
#include <owl\edit.h>
#include <owl\listbox.h>
#include <OWL\DIALOG.H>

#include "pounds.rh"            // Definition of all resources.
#include "objmerc.h"

//{{TDialog = TMobProgDia}}
struct TMobProgDiaXfer {
//{{TMobProgDiaXFER_DATA}}
    TListBoxData  Triggers;
    char    Program[ 2048 ];
    char    TheMobile[ 255 ];
    char    FileName[ 255 ];
//{{TMobProgDiaXFER_DATA_END}}
};


class TMobProgDia : public TDialog {
   TArrayAsVector<TMobProgData*>* orig;
   TArrayAsVector<TMobProgData*> newprogs;
   TMobile *TheMob;
public:
    TMobProgDia (TWindow* parent, TMobile *pMob,
       TArrayAsVector<TMobProgData*>& mobprogs,
       TResId resId = IDD_MOBPROG, TModule* module = 0);
    virtual ~TMobProgDia ();

//{{TMobProgDiaXFER_DEF}}
protected:
    TListBox *Triggers;
    TEdit *Program;
    TStatic *TheMobile;
    TStatic *FileName;

//{{TMobProgDiaXFER_DEF_END}}

//{{TMobProgDiaRSP_TBL_BEGIN}}
protected:
    void CmTriggerSelChange ();
    void CmHelp ();
//{{TMobProgDiaRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TMobProgDia);
};    //{{TMobProgDia}}


#endif                                      // __mobprog_h sentry.

