#if !defined(__mapview_h)              // Sentry, use file only if it's not already included.
#define __mapview_h

/*  Project pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         mapview.h
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for TMapView (TWindowView).
*/

#include <owl\owlpch.h>
#pragma hdrstop

#include <OWL\DOCVIEW.H>

#include "pounds.rh"            // Definition of all resources.
#include "areadoc.h"


//{{TWindowView = TMapView}}
class TMapView : public TWindowView {
    TAreaDocument *AreaDoc;
public:
    TMapView (TAreaDocument& doc, TWindow* parent = 0);
    virtual ~TMapView ();

    static const char far* StaticName() { return "Map"; }
    virtual void Paint(TDC&, BOOL erase, TRect&);
//    void EvSize(UINT, TSize&);
    BOOL Create();
    BOOL CanClose()
       { return TWindow::CanClose() &&
         ( Doc->NextView(this) || Doc->NextView(0) != this || Doc->CanClose()); }

    BOOL LoadData();

    PRoom CurrentRoom;
    TFont* iconFont;		// This must be a pointer, as we don't have the LOGFONT
    							// in time to make it a value.
    TListBox *RoomList;
    TBitmap *RoomFlags;
    TBitmap *SectTypes;

    void OutputRoom(TDC& dc, TRoom *Room, int vnum, int xWhich, int yWhich);

    virtual void SetupWindow();

    void CmListChanged();
    void CmListSelected();

//{{TMapViewVIRTUAL_BEGIN}}
public:
    virtual const char far* GetViewName () { return StaticName(); }
//{{TMapViewVIRTUAL_END}}
    BOOL VnIsWindow(HWND hWnd);
    BOOL VnAppend(ChangeNotice *);
    BOOL VnModify(ChangeNotice *);
    BOOL VnDelete(ChangeNotice *);
    BOOL VnRevert(BOOL clear);

//{{TMapViewRSP_TBL_BEGIN}}
protected:
    void EvLButtonDown (UINT modKeys, TPoint& point);
    void EvSize (UINT sizeType, TSize& size);
    void CmEditItem ();
    void CmEditAdd ();
    void CmEditCopy ();
//{{TMapViewRSP_TBL_END}}
    bool VnDocClosed(int omode);
DECLARE_RESPONSE_TABLE(TMapView);
};    //{{TMapView}}


#endif                                      // __mapview_h sentry.

