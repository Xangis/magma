#if !defined(__pndsmdic_h)              // Sentry, use file only if it's not already included.
#define __pndsmdic_h

/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.exe Application
    FILE:         pndsmdic.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for poundsMDIClient (TMDIClient).      
*/


#include <owl\owlpch.h>
#pragma hdrstop

#include <owl\opensave.h>

#include "pounds.rh"            // Definition of all resources.


//{{TMDIClient = poundsMDIClient}}
class poundsMDIClient : public TMDIClient {
public:
    int                     ChildCount;                 // Number of child window created. 

    poundsMDIClient ();
    virtual ~poundsMDIClient ();

    void OpenFile (const char *fileName = 0);

private:
    void LoadTextFile ();

//{{poundsMDIClientVIRTUAL_BEGIN}}
protected:
    virtual void SetupWindow ();
//{{poundsMDIClientVIRTUAL_END}}

//{{poundsMDIClientRSP_TBL_BEGIN}}
protected:
    void CmFilePrint ();
    void CmFilePrintSetup ();
    void CmFilePrintPreview ();
    void CmPrintEnable (TCommandEnabler &tce);
    void EvDropFiles (TDropInfo);
//{{poundsMDIClientRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(poundsMDIClient);
};    //{{poundsMDIClient}}


#endif                                      // __pndsmdic_h sentry.
