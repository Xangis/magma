#if !defined(__pndsabtd_h)              // Sentry, use file only if it's not already included.
#define __pndsabtd_h

/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.exe Application
    FILE:         pndsabtd.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for poundsAboutDlg (TDialog).      
*/


#include <owl\owlpch.h>
#pragma hdrstop

#include "pounds.rh"                  // Definition of all resources.


//{{TDialog = poundsAboutDlg}}
class poundsAboutDlg : public TDialog {
public:
    poundsAboutDlg (TWindow *parent, TResId resId = IDD_ABOUT, TModule *module = 0);
    virtual ~poundsAboutDlg ();

//{{poundsAboutDlgVIRTUAL_BEGIN}}
public:
    void SetupWindow ();
//{{poundsAboutDlgVIRTUAL_END}}
};    //{{poundsAboutDlg}}


// Reading the VERSIONINFO resource.
class ProjectRCVersion {
public:
    ProjectRCVersion (TModule *module);
    virtual ~ProjectRCVersion ();

    BOOL GetProductName (LPSTR &prodName);
    BOOL GetProductVersion (LPSTR &prodVersion);
    BOOL GetCopyright (LPSTR &copyright);
    BOOL GetDebug (LPSTR &debug);

protected:
    LPBYTE      TransBlock;
    void FAR    *FVData;

private:
    // Don't allow this object to be copied.
    ProjectRCVersion (const ProjectRCVersion &);
    ProjectRCVersion & operator =(const ProjectRCVersion &);
};


#endif                                      // __pndsabtd_h sentry.
