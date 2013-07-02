#if !defined(__pndsmdi1_h)              // Sentry, use file only if it's not already included.
#define __pndsmdi1_h

/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.exe Application
    FILE:         pndsmdi1.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for poundsMDIChild (TMDIChild).      
*/


#include <owl\owlpch.h>
#pragma hdrstop

#include <owl\editfile.h>
#include <owl\listbox.h>

#include "pounds.rh"            // Definition of all resources.


//{{TMDIChild = poundsMDIChild}}
class poundsMDIChild : public TMDIChild {
public:
    poundsMDIChild (TMDIClient &parent, const char far *title, TWindow *clientWnd, BOOL shrinkToClient = FALSE, TModule* module = 0);
    virtual ~poundsMDIChild ();

//{{poundsMDIChildVIRTUAL_BEGIN}}
public:
    virtual void Paint (TDC& dc, BOOL erase, TRect& rect);
    virtual BOOL CanClose ();
//{{poundsMDIChildVIRTUAL_END}}
//{{poundsMDIChildRSP_TBL_BEGIN}}
protected:
    void EvGetMinMaxInfo (MINMAXINFO far& minmaxinfo);
//{{poundsMDIChildRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(poundsMDIChild);
};    //{{poundsMDIChild}}


#endif                                      // __pndsmdi1_h sentry.
