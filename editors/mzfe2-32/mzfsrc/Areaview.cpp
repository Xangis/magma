/*  Project pounds

    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.apx Application
    FILE:         areaview.cpp
    AUTHOR:       Slash


    OVERVIEW
    ========
    Source file for implementation of TAreaListView (TView).
*/

#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include <owl\inputdia.h>

#include "areaview.h"

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TAreaListView, TListBox)
//{{TAreaListViewRSP_TBL_BEGIN}}
    EV_COMMAND(CM_EDITITEM, CmEditItem),
    EV_COMMAND(CM_EDITFIND, CmEditFind),
    EV_COMMAND(CM_EDITFINDNEXT, CmEditFindNext),
    EV_COMMAND(CM_EDITADD, CmEditAdd),
    EV_COMMAND(CM_EDITDELETE, CmEditDelete),
    EV_COMMAND_ENABLE(CM_EDITDELETE, CmEditDeleteEnabler),
    EV_COMMAND(CM_EDITCOPY, CmEditCopy),
    EV_COMMAND_ENABLE(CM_EDITCOPY, CmEditCopyEnable),
    EV_COMMAND(CM_EDITCUT, CmEditCut),
    EV_COMMAND_ENABLE(CM_EDITCUT, CmEditCopyEnable),
    EV_COMMAND_ENABLE(CM_EDITITEM, CmEditCopyEnable),
//{{TAreaListViewRSP_TBL_END}}
  EV_WM_GETDLGCODE,
  EV_NOTIFY_AT_CHILD(LBN_DBLCLK, CmEditItem),
  EV_NOTIFY_AT_CHILD(LBN_SELCHANGE, CmSelChange),
  EV_VN_ISWINDOW,
//  EV_VN_COMMIT,
//  EV_VN_REVERT,
  EV_VN_AREAAPPEND,
  EV_VN_AREADELETE,
  EV_VN_AREAMODIFY,
  EV_VN_REVERT,
//  EV_VN_DOCCLOSED,
END_RESPONSE_TABLE;


//{{TAreaListView Implementation}}


TAreaListView::TAreaListView (TAreaDocument& doc, TWindow *parent, int n):
    TView(doc), TListBox(parent, GetNextViewId(), 0,0,0,0), AreaDoc( &doc )
{
    // INSERT>> Your constructor code here.
    nWhich = n;

    Attr.Style &= ~(LBS_SORT);
    Attr.Style |= (WS_HSCROLL | LBS_NOINTEGRALHEIGHT| LBS_USETABSTOPS);
    szFindText[0] = '\0';
}


TAreaListView::~TAreaListView ()
{
    // INSERT>> Your destructor code here.

}


//const char far* TAreaListView::GetViewName ()
//{
//    return StaticName();
//}

BOOL TAreaListView::Create()
{
  TRY {
    TListBox::Create();   // throws exception TWindow::TXWindow
  }
  CATCH( (TXOwl& x) {
    Doc->PostError(IDS_NOMEMORYFORVIEW, MB_OK);
    return TRUE;   // cannot return FALSE - throws another exception
  })
//  if (Doc->GetDocPath() == 0) {
////@@@    CmEditClear();         // perform any clearing initialization
//    return TRUE;           // new file, no data to display
//  }
  if (!LoadData())
	 NotOK();
  return TRUE;
}

BOOL TAreaListView::LoadData()
{
  ClearList();
  int i = 0;
  char szBuffer[512];
//  const TVNumObj *o;
  while ((AreaDoc->GetItem(nWhich, i, szBuffer, sizeof szBuffer)) != 0)
     {
     DeleteString(i);
     InsertString(szBuffer, i);
     i++;
     }

  SetSelIndex(0);
  return TRUE;
}

UINT TAreaListView::EvGetDlgCode(MSG far*)
{
  UINT retVal = (UINT)DefaultProcessing();
  retVal |= DLGC_WANTCHARS;
  return retVal;
}

BOOL TAreaListView::VnAppend(ChangeNotice *pCD)
{
  if (pCD->nWhich == nWhich)
     {
     char szBuffer[512];
     AreaDoc->GetItem(pCD->nWhich, pCD->nIndex, szBuffer, sizeof szBuffer);
     DeleteString(pCD->nIndex);
     InsertString(szBuffer, pCD->nIndex);
     SetSelIndex(pCD->nIndex);
     }
  return TRUE;
}

BOOL TAreaListView::VnDelete(ChangeNotice *pCD)
{
  if (pCD->nWhich == nWhich)
     {
     DeleteString(pCD->nIndex);
     HandleMessage(WM_KEYDOWN, VK_DOWN); // force selection
     }
  return TRUE;
}

BOOL TAreaListView::VnModify(ChangeNotice *pCD)
{
  if (pCD->nWhich == nWhich)
     {
     char szBuffer[512];
     AreaDoc->GetItem(pCD->nWhich, pCD->nIndex, szBuffer, sizeof szBuffer);
     DeleteString(pCD->nIndex);
     InsertString(szBuffer, pCD->nIndex);
     SetSelIndex(pCD->nIndex);
     }
  return TRUE;
}

BOOL TAreaListView::VnRevert(BOOL /*clear*/)
{
  LoadData();
  return TRUE;
}

void TAreaListView::CmEditItem ()
{
    // INSERT>> Your code here.
    AreaDoc->EditItem(nWhich, GetSelIndex());
}

//DEFINE_RESPONSE_TABLE1(TRoomListView, TAreaListView)
//END_RESPONSE_TABLE;


void TAreaListView::CmEditFind ()
{
    // INSERT>> Your code here.
    if (TInputDialog(this, "MAKE.ZONES.FAST", "Search string:", szFindText,
       sizeof szFindText).Execute() == IDOK)
       {
       SetSelIndex(-1);
       CmEditFindNext();
       }
}


void TAreaListView::CmEditFindNext ()
{
    // INSERT>> Your code here.
    char szBuffer[512];
    int nNdx = GetSelIndex();
    nNdx = SetSelIndex((nNdx == LB_ERR) ? 0 : nNdx + 1);
    if (nNdx == LB_ERR)
       {
       MessageBeep(-1);
       return;
       }

    while (nNdx != LB_ERR)
       {
       GetSelString(szBuffer, sizeof szBuffer);

       if (strstr(szBuffer, szFindText))
          return;

       nNdx = SetSelIndex(nNdx+1);
       }

    MessageBox("Not found", "MAKE.ZONES.FAST", MB_ICONINFORMATION|MB_OK);
}


void TAreaListView::CmEditAdd ()
{
    // INSERT>> Your code here.
    AreaDoc->EditItem(nWhich, -1);
}


void TAreaListView::CmEditDelete ()
{
    // INSERT>> Your code here.
    int n = GetSelIndex();
    if (n != LB_ERR)
       AreaDoc->DeleteItem(nWhich, GetSelIndex());
}


void TAreaListView::CmEditDeleteEnabler (TCommandEnabler &tce)
{
    // INSERT>> Your code here.
    tce.Enable(GetSelIndex() != LB_ERR);
}

   // @@@ Note:  Certain views depend on data from other views.  For example,
   // Shops needs mobs, Resets needs mobs, objs and rooms, MobResets needs
   // mobs, objs, rooms and resets.
DEFINE_RESPONSE_TABLE1(TShopListView, TAreaListView)
  EV_VN_AREADELETE,
END_RESPONSE_TABLE;

BOOL TShopListView::VnDelete(ChangeNotice */*pCD*/)
{
   // @@@Should check for deleted mobiles
   int nSel = GetSelIndex();
   int nTop = GetTopIndex();
   LoadData();
//   return TAreaListView::VnDelete(pCD);
   SetSelIndex(nSel);
   SetTopIndex(nTop);
   return TRUE;
}

DEFINE_RESPONSE_TABLE1(TResetListView, TAreaListView)
  EV_VN_AREADELETE,
END_RESPONSE_TABLE;

BOOL TResetListView::VnDelete(ChangeNotice */*pCD*/)
{
   // @@@Should check for deleted objects, mobiles, resets
   int nSel = GetSelIndex();
   int nTop = GetTopIndex();
   LoadData();
//   return TAreaListView::VnDelete(pCD);
   SetSelIndex(nSel);
   SetTopIndex(nTop);
   return TRUE;
}

DEFINE_RESPONSE_TABLE1(TMobResetListView, TAreaListView)
  EV_VN_AREADELETE,
END_RESPONSE_TABLE;

BOOL TMobResetListView::VnDelete(ChangeNotice */*pCD*/)
{
   // @@@Should check for deleted objects, mobiles, rooms, resets
   int nSel = GetSelIndex();
   int nTop = GetTopIndex();
   LoadData();
//   return TAreaListView::VnDelete(pCD);
   SetSelIndex(nSel);
   SetTopIndex(nTop);
   return TRUE;
}



void TAreaListView::CmEditCopy ()
{
    // INSERT>> Your code here.
    AreaDoc->CopyItem(nWhich, GetSelIndex());
}


void TAreaListView::CmEditCopyEnable (TCommandEnabler &tce)
{
    // INSERT>> Your code here.
    tce.Enable(GetSelIndex() != LB_ERR);
}


void TAreaListView::CmEditCut ()
{
    // INSERT>> Your code here.
	 CmEditCopy();
    CmEditDelete();
}

bool TAreaListView::VnDocClosed(int omode)
{
  if (!(omode & ofWrite))  // make sure someone else's write
    return false;
  return true;
}

