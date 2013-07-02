#if !defined(__areaview_h)              // Sentry, use file only if it's not already included.
#define __areaview_h

/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         areaview.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for TAreaListView (TView).
*/

#include <owl\owlpch.h>
#pragma hdrstop

#include <OWL\DOCVIEW.H>
#include <owl\listbox.h>

#include "pounds.rh"            // Definition of all resources.
#include "areadoc.h"            // Area database routines

//{{TView = TAreaListView}}
class TAreaListView : public TView, public TListBox {
protected:
    TAreaDocument *AreaDoc;
    int nWhich;
    char szFindText[256];
    BOOL LoadData();
public:
    TAreaListView (TAreaDocument& doc, TWindow *parent, int n);
    virtual ~TAreaListView ();

    TWindow* GetWindow()  {return (TWindow*)this;}
    BOOL     SetDocTitle(const char far* docname, int index)
                          {return TListBox::SetDocTitle(docname, index); }
    //
    // overridden virtuals from TWindow
    //
    BOOL CanClose()
       { return TListBox::CanClose() &&
         ( Doc->NextView(this) || Doc->NextView(0) != this || Doc->CanClose()); }
//    BOOL CanClose()   {return TListBox::CanClose() /*&& Doc->CanClose()*/;}
    BOOL Create();
//    static const char far* StaticName() { return "TAreaListView View"; }

//{{TAreaListViewVIRTUAL_BEGIN}}
protected:
    UINT EvGetDlgCode(MSG far*);
    BOOL VnIsWindow(HWND hWnd) {return HWindow == hWnd;}
    bool VnDocClosed(int omode);
    void CmSelChange(){} // to prevent interpreting as unprocessed accelerator
//    virtual const char far* GetViewName ();
//{{TAreaListViewVIRTUAL_END}}

//{{TAreaListViewRSP_TBL_BEGIN}}
protected:
    void CmEditItem ();
    void CmEditFind ();
    void CmEditFindNext ();
    void CmEditAdd ();
    void CmEditDelete ();
    void CmEditDeleteEnabler (TCommandEnabler &tce);
    void CmEditCopy ();
    void CmEditCopyEnable (TCommandEnabler &tce);
    void CmEditCut ();
//{{TAreaListViewRSP_TBL_END}}
    BOOL VnAppend(ChangeNotice *);
    BOOL VnModify(ChangeNotice *);
    BOOL VnDelete(ChangeNotice *);
    BOOL VnRevert(BOOL clear);
DECLARE_RESPONSE_TABLE(TAreaListView);
};    //{{TAreaListView}}

//==============================================================================
class TRoomListView : public TAreaListView {
public:
    TRoomListView (TAreaDocument& doc, TWindow *parent=0):
       TAreaListView(doc, parent, 2) {}

    static const char far* StaticName() { return "Rooms"; }

protected:
    virtual const char far* GetViewName () { return StaticName(); }
//DECLARE_RESPONSE_TABLE(TRoomListView);
};

//==============================================================================
class TObjectListView : public TAreaListView {
public:
    TObjectListView (TAreaDocument& doc, TWindow *parent=0):
       TAreaListView(doc, parent, 1) {}

    static const char far* StaticName() { return "Objects"; }

protected:
    virtual const char far* GetViewName () { return StaticName(); }
};

//==============================================================================
class TMobileListView : public TAreaListView {
public:
    TMobileListView (TAreaDocument& doc, TWindow *parent=0):
       TAreaListView(doc, parent, 0) {}

    static const char far* StaticName() { return "Mobiles"; }

protected:
    virtual const char far* GetViewName () { return StaticName(); }
};

//==============================================================================
class THelpListView : public TAreaListView {
public:
    THelpListView (TAreaDocument& doc, TWindow *parent=0):
       TAreaListView(doc, parent, 3) {}

    static const char far* StaticName() { return "Helps"; }

protected:
    virtual const char far* GetViewName () { return StaticName(); }
};

//==============================================================================
class TShopListView : public TAreaListView {
public:
    TShopListView (TAreaDocument& doc, TWindow *parent=0):
       TAreaListView(doc, parent, 4) {}

    static const char far* StaticName() { return "Shops"; }

protected:
    virtual const char far* GetViewName () { return StaticName(); }
    BOOL VnDelete(ChangeNotice *);
DECLARE_RESPONSE_TABLE(TShopListView);
};

//==============================================================================
class TResetListView : public TAreaListView {
public:
    TResetListView (TAreaDocument& doc, TWindow *parent=0):
       TAreaListView(doc, parent, 5) {}

    static const char far* StaticName() { return "Resets"; }

protected:
    virtual const char far* GetViewName () { return StaticName(); }
    BOOL VnDelete(ChangeNotice *);
DECLARE_RESPONSE_TABLE(TResetListView);
};

//==============================================================================
class TMobResetListView : public TAreaListView {
public:
    TMobResetListView (TAreaDocument& doc, TWindow *parent=0):
       TAreaListView(doc, parent, 6) {}

    static const char far* StaticName() { return "Mob Resets"; }

protected:
    virtual const char far* GetViewName () { return StaticName(); }
    BOOL VnDelete(ChangeNotice *);
DECLARE_RESPONSE_TABLE(TMobResetListView);
};

#endif                                      // __areaview_h sentry.

