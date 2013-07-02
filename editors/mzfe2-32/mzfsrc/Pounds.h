#if !defined(__pounds_h)              // Sentry, use file only if it's not already included.
#define __pounds_h

/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.exe Application
    FILE:         pounds.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for poundsApp (TApplication).      
*/


#include <owl\owlpch.h>
#pragma hdrstop

#include <owl\statusba.h>
#include <owl\controlb.h>
#include <owl\buttonga.h>
#include <owl\editview.h>
#include <owl\listview.h>
#include <owl\docmanag.h>
#include <owl\filedoc.h>
#include <owl\printer.h>

#include <classlib\bags.h>


#include "pndsmdic.h"
#include "tooltip.h"
//#include "objlib.h"

#include "pounds.rh"            // Definition of all resources.


// TFileDrop class Maintains information about a dropped file, its name, where it was dropped,
// and whether or not it was in the client area
class TFileDrop {
public:
    operator == (const TFileDrop& other) const {return this == &other;}

    char*   FileName;
    TPoint  Point;
    BOOL    InClientArea;

    TFileDrop (char*, TPoint&, BOOL, TModule*);
    ~TFileDrop ();

    const char* WhoAmI ();
private:
    //
    // hidden to prevent accidental copying or assignment
    //
    TFileDrop (const TFileDrop&);
    TFileDrop & operator = (const TFileDrop&);
};

typedef TIBagAsVector<TFileDrop> TFileList;
typedef TIBagAsVectorIterator<TFileDrop> TFileListIter;

class TLibrary;

//{{TApplication = poundsApp}}
class poundsApp : public TApplication {
public:
    enum currMode { MercMode, TFCMode, EnvyMode, RomMode, Envy2Mode, OghamMode, Rom24Mode, NumModes } CurrentMode; // File format
    static const char *szModesShort[NumModes];
    BOOL fMOBprograms;
    TLibrary *GetLibrary() { return m_Library; }
    void SetMode(currMode newMode);

    TFont hFixedFont;											  // Font & Font size
    static int xFont;
    static int yFont;

    // Custom spell stuff
    int nSlotsUsed;

	 static BOOL HelpState;                   // Has the help engine been used.
	 TToolTip	 tooltip;							// Enable tool tips

private:
    class SpellAndSlot {
    public:
       SpellAndSlot(int n, const char *p): strName(p) { nSlot = n; }

       string strName;
       int nSlot;
    };

    BOOL            ContextHelp;                        // SHIFT-F1 state (context sensitive HELP)
    HCURSOR         HelpCursor;                         // Context sensitive help cursor
    TLibrary       *m_Library;

private:
    void SetupSpeedBar (TDecoratedMDIFrame *frame);
    void AddFiles (TFileList* files);
    void SetMenuForFormat();
    SpellAndSlot **ppCustomSpells;

public:
    poundsApp ();
    virtual ~poundsApp ();

    const char *SpellName(int nSlot) const;
    const char *SpellIndex(int nNdx, int *nSlot) const;
    int SpellValue(const char *p);

    poundsMDIClient  *mdiClient;

    // Public data members used by the print menu commands and Paint routine in MDIChild.
    TPrinter        *Printer;                           // Printer support.
    int             Printing;                           // Printing in progress.

//{{poundsAppVIRTUAL_BEGIN}}
public:
    virtual void InitMainWindow();
    virtual void InitInstance();
    virtual BOOL CanClose ();
    virtual BOOL ProcessAppMsg (MSG& msg);
//{{poundsAppVIRTUAL_END}}

//{{poundsAppRSP_TBL_BEGIN}}
protected:
    void EvNewView (TView& view);
    void EvCloseView (TView& view);
    void CmHelpAbout ();
    void CmHelpContents ();
    void CmHelpUsing ();
    void EvDropFiles (TDropInfo drop);
    void EvWinIniChange (char far* section);
    void CmFileFormat ();
    void CmHelpCredits ();
    void CmHelpMerc ();
    void CmHelpEnvy ();
    void CmHelpRom ();
//    void CmFileSaveEnabler (TCommandEnabler &tce);
//    void CmFileSaveEnabler (TCommandEnabler &tce);
//{{poundsAppRSP_TBL_END}}
  DECLARE_RESPONSE_TABLE(poundsApp);
};    //{{poundsApp}}


#endif                                      // __pounds_h sentry.
