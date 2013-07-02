/*  Project pounds

	 Copyright © 1994. All Rights Reserved.

	 SUBSYSTEM:    pounds.apx Application
	 FILE:         mapview.cpp
	 AUTHOR:       Slash


	 OVERVIEW
	 ========
	 Source file for implementation of TMapView (TWindowView).
*/

#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include "mapview.h"

#ifndef __FLAT__
BOOL PASCAL PolyBezier(HDC hdc, const TPoint* lppt, DWORD cPoints);
#endif

//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TMapView, TWindowView)
//{{TMapViewRSP_TBL_BEGIN}}
    EV_WM_LBUTTONDOWN,
    EV_WM_SIZE,
    EV_COMMAND(CM_EDITITEM, CmEditItem),
    EV_COMMAND(CM_EDITADD, CmEditAdd),
    EV_COMMAND(CM_EDITCOPY, CmEditCopy),
//{{TMapViewRSP_TBL_END}}
  EV_VN_ISWINDOW,
  EV_VN_AREAAPPEND,
  EV_VN_AREADELETE,
  EV_VN_AREAMODIFY,
  EV_VN_REVERT,
//  EV_VN_DOCCLOSED,
  EV_LBN_SELCHANGE(101, CmListChanged),
  EV_LBN_DBLCLK(101, CmListSelected),
END_RESPONSE_TABLE;


//{{TMapView Implementation}}


TMapView::TMapView (TAreaDocument& doc, TWindow* parent):
	 TWindowView(doc, parent) /*, canvas( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT )*/

{
    // INSERT>> Your constructor code here.
    // @@@ Attr W and H aren't ready, here...
    RoomList = new TListBox(this, 101, 300, 0, 500, 500);
    RoomList->Attr.Style |= LBS_NOINTEGRALHEIGHT;
    RoomList->Attr.Style &= ~LBS_SORT;

    AreaDoc = &doc;

    // @@@ Cast-away const
    CurrentRoom = (TRoom *) AreaDoc->GetRoomFromIndex(0);

    LOGFONT logfont;
    SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof logfont,
       &logfont, FALSE);
    iconFont = new TFont(&logfont);

    RoomFlags = new TBitmap(GetApplication()->GetInstance(), BM_ROOMFLAGS);
    SectTypes = new TBitmap(GetApplication()->GetInstance(), BM_SECTTYPES);
}


TMapView::~TMapView ()
{
    Destroy();

    // INSERT>> Your destructor code here.
    delete iconFont;

    delete RoomFlags;
    delete SectTypes;
}

#define XOFF 5
#define YOFF 5
static int xLeft[]   = { 10, 110, 210 };
static int xRight[]  = { 90, 190, 290 };
static int yTop[]    = { 20, 100, 180 };
static int yBottom[] = { 80, 160, 240 };
static int Xs[] = { 1, 2, 1, 0, 2, 0 };
static int Ys[] = { 0, 1, 2, 1, 0, 2 };

static int xFrom[] = { 150, 190, 150, 110, 190, 110 };
static int yFrom[] = { 100, 130, 160, 130,  99, 159 };
static int xTo[]   = { 150, 210, 150,  89, 210,  89 };
static int yTo[]   = {  79, 130, 180, 130,  79, 180 };

void TMapView::SetupWindow()
{
  TWindowView::SetupWindow();

  int i = 0;
  char szBuffer[512];
//  const TVNumObj *o;
  while ((AreaDoc->GetItem(2, i, szBuffer, sizeof szBuffer)) != 0)
     {
     RoomList->AddString(szBuffer);
     i++;
     }

  RoomList->SetSelIndex(0);

  TRect r;
  GetClientRect(r);
  RoomList->MoveWindow(xRight[2]+10, 0, r.Width() - (xRight[2]+10), r.Height());
}

// Draw a single room
void TMapView::OutputRoom(TDC& dc, TRoom *Room, int vnum, int xWhich, int yWhich)
{
   char szBuffer[512];
  	TRect r(xLeft[xWhich], yTop[yWhich], xRight[xWhich], yBottom[yWhich]);
   dc.Rectangle(r);

   if (Room)
      {
      Room->GetName(szBuffer, sizeof szBuffer);

      TMemoryDC dcMem(dc);
      dcMem.SelectObject(*SectTypes);
      int xPos = xLeft[xWhich];
      int yPos = yBottom[yWhich];
      dc.BitBlt(xPos, yPos, 16, 16, dcMem, 16 * Room->sector_type, 0, SRCCOPY);
      xPos += 17;

      dcMem.SelectObject(*RoomFlags);
      int xChunk = 0;
      for (int32 nFlags = Room->room_flags; nFlags; nFlags >>= 1, xChunk += 16)
         if (nFlags & 1)
            {
            dc.BitBlt(xPos, yPos, 16, 16, dcMem, xChunk, 0, SRCCOPY);
            xPos += 17;
            }
      }
   else
      wsprintf(szBuffer, "#%d", vnum);

   // We track the first space, and look for the last space we can break on
   char *p = szBuffer;
   char *q = p;
   char *last_valid_break = 0;
   int yOff = 0;
   TSize size;
   for ( ; ; )
      {
      if (*q == ' ' || *q == '\0')
         {
         // Have we gone past our boundery?
         dc.GetTextExtent(p, int(q-p), size);
         if (size.cx > r.Width() - 2 * XOFF)
            {
            if (!last_valid_break)	   // Is there a safe place to break?
               last_valid_break = q;   // No, so break here
          	dc.ExtTextOut(xLeft[xWhich] + XOFF, yTop[yWhich] + YOFF + yOff,
               ETO_CLIPPED, &r, p, int(last_valid_break - p));
            q = p = last_valid_break + 1;
            last_valid_break = 0;
            yOff += size.cy;
            }
         // We are in boundery.  If this is a space, log it as 'valid'
         else if (*q)
            last_valid_break = q++;
         // We are in boundery, and done with our string
         else
            {
          	dc.ExtTextOut(xLeft[xWhich] + XOFF, yTop[yWhich] + YOFF + yOff,
               ETO_CLIPPED, &r, p, int(q - p));
            return;
            }
         }
      else
         q++;
      }
}

// Draw the room and its up to six adjacent rooms.
void TMapView::Paint(TDC& dc, BOOL erase, TRect& r)
{
   TWindowView::Paint(dc, erase, r);

   if (CurrentRoom)
      {
      TBrush brush((HBRUSH) GetStockObject(LTGRAY_BRUSH));
      dc.SelectObject(brush);
      dc.SelectObject(*iconFont);
      dc.SetBkMode(TRANSPARENT);

      OutputRoom(dc, CurrentRoom, 0, 1, 1);

      for (int i = 0; i < 6; i++)
         {
         if (CurrentRoom->is_exit[i])
            {
            TExit *pEx = &CurrentRoom->exit[i];

            // Calculate the mid-point of the segment
            int xMid = (xTo[i] + xFrom[i]) / 2;
            int yMid = (yTo[i] + yFrom[i]) / 2;

            // Is the exit to a room?
            if (pEx->to_room)
               {
               int yDif = abs(yTo[i] - yMid);
               int xDif = abs(xTo[i] - xMid);

               // Is the exit a loop back to the same room?
               if (pEx->to_room == CurrentRoom)
                  {
                  TPoint pts[4];
                  pts[0].x = xFrom[i];
                  pts[0].y = yFrom[i];
                  pts[1].x = xMid - yDif;
                  pts[1].y = yMid - xDif;
                  pts[2].x = xMid + yDif;
                  pts[2].y = yMid + xDif;
//                  pts[1].x = xTo[i];
//                  pts[1].y = yTo[i];
						pts[3] = pts[0];
#ifdef __FLAT__
						dc.PolyBezier(pts, nelems(pts));
#else
						PolyBezier(dc, pts, nelems(pts));
#endif
						}
               else
                  {
                  // Draw the room and a line to it
    				   OutputRoom(dc, pEx->to_room, pEx->vnum, Xs[i], Ys[i]);
                  dc.MoveTo(xFrom[i], yFrom[i]);
                  dc.LineTo(xTo[i], yTo[i]);

                  // If the exit is a door, draw a segment perpendicularly
                  // bisecting the exit segment
                  if (pEx->exit_info)
                     {
                     dc.MoveTo(xMid - yDif, yMid - xDif);
                     dc.LineTo(xMid + yDif, yMid + xDif);
                     }

                  // If the exit is one-way, draw an 'arrowhead'
                  if (!pEx->to_room->is_exit[TAreaDocument::rev_dir[i]] ||
                      pEx->to_room->exit[TAreaDocument::rev_dir[i]].to_room != CurrentRoom)
                     {
                     dc.MoveTo(xTo[i], yTo[i]);
                     dc.LineTo((xMid - yDif + xTo[i]) / 2, (yMid - xDif + yTo[i]) / 2);
                     dc.MoveTo(xTo[i], yTo[i]);
                     dc.LineTo((xMid + yDif + xTo[i]) / 2, (yMid + xDif + yTo[i]) / 2);
                     }
                  }
               }
            // The exit is just an 'exit description'.  Draw a half-line
            else
               {
               dc.MoveTo(xFrom[i], yFrom[i]);
               dc.LineTo(xMid, yMid);
               }
            }
         }
      }
}

BOOL TMapView::Create()
{
  TRY {
    TWindowView::Create();   // throws exception TWindow::TXWindow
  }
  CATCH( (TXOwl& x) {
    Doc->PostError(IDS_NOMEMORYFORVIEW, MB_OK);
    return TRUE;   // cannot return FALSE - throws another exception
  })
  if (Doc->GetDocPath() == 0) {
//@@@    CmEditClear();         // perform any clearing initialization
    return TRUE;           // new file, no data to display
  }
  if (!LoadData())
	 NotOK();
  return TRUE;
}

BOOL TMapView::LoadData()
{
  RoomList->ClearList();
  int i = 0;
  char szBuffer[512];
//  const TVNumObj *o;
  while ((AreaDoc->GetItem(2, i, szBuffer, sizeof szBuffer)) != 0)
     {
     RoomList->DeleteString(i);
     RoomList->InsertString(szBuffer, i);
     i++;
     }

  RoomList->SetSelIndex(0);
   return TRUE;
}

#if 0
void	TMapView::ExposeAll(TDC& dc, ExposeEvent *event )
{
/*
*    SET WINDOW	 - VIEWPORT TRANSFORM
*/
    if ( event ) graph.SetViewPort( Canvas() );
/*
*    DISPLAY RELATIONS
*/
    graph.FirstNode();
    do {
	if ( graph.FirstRelation() ) {
	    do ShowRelation(dc); while ( graph.NextRelation() );
	}
    } while ( graph.NextNode() );
/*
*    DISPLAY NODES
*/
    if ( !graph.FirstNode() ) return;
    do ShowNode(dc); while ( graph.NextNode() );
}

/*---------------------	  ShowNode    ------------------------*/
/* Shows current node as a rectangle and a text		      */
/*------------------------------------------------------------*/
void TMapView :: ShowNode(TDC& dc)
{

    DrawRectangle(dc, RectAngle( graph.ScreenPos().X() - NODESIZE_X / 2,
			      graph.ScreenPos().Y() - NODESIZE_Y / 2,
			      NODESIZE_X, NODESIZE_Y) );

    Text(dc, graph.GetNode() -> GetName(), graph.ScreenPos() );
}

/*---------------------	  ShowRelation	  ----------------------*/
/* Shows the current relation as a line and a text		*/
/*--------------------------------------------------------------*/
void TMapView :: ShowRelation(TDC& dc)
{
    MoveTo(dc, graph.ScreenPos() );
    LineTo(dc, graph.RelScreenPos() );
    Text(dc, graph.GetRelationName(),
	  Point( (graph.ScreenPos().X() + graph.RelScreenPos().X()) / 2,
		 (graph.ScreenPos().Y() + graph.RelScreenPos().Y()) / 2 ) );
}

//-------------------------------------------------------------------
RectAngle TMapView :: Canvas()
//-------------------------------------------------------------------
{
	return canvas;
}

//-------------------------------------------------------------------
void TMapView :: RePaint()
//-------------------------------------------------------------------
{
   Invalidate();
}

//-------------------------------------------------------------------
void TMapView :: Text(TDC& dc, char * text, Point p)
//-------------------------------------------------------------------
{
	TEXTMETRIC tm;
	dc.GetTextMetrics( tm );

	dc.TextOut(
		p.X() - tm.tmMaxCharWidth / 2,
		p.Y() - tm.tmHeight / 2,
		text);
}

//-------------------------------------------------------------------
void TMapView :: MoveTo(TDC& dc, Point p )
//-------------------------------------------------------------------
{
	dc.MoveTo(p.X(), p.Y() );
}

//-------------------------------------------------------------------
void TMapView :: LineTo(TDC& dc, Point p )
//-------------------------------------------------------------------
{
	dc.LineTo(p.X(), p.Y());
}

//-------------------------------------------------------------------
#pragma argsused
void TMapView :: DrawRectangle(TDC& dc, RectAngle& rect )
//-------------------------------------------------------------------
{
//	RECT r;
//	r.left = rect.HorPos();
//	r.top = rect.VerPos();
//	r.right = r.left + rect.Width();
//	r.bottom = r.top + rect.Height();
//	dc.FillRect( &r, GetStockObject( BLACK_BRUSH ) );

//   TBrush br(TColor::Black);
//   dc.FillRect(rect.HorPos(), rect.HorPos()+rect.Width(),
//               rect.VerPos(), rect.VerPos()+rect.Height(), br);
   // @@@ This seems inefficient...
}
#endif

BOOL TMapView::VnAppend(ChangeNotice *pCD)
{
  if (pCD->nWhich == 2)
     {
     LoadData();
     Invalidate();
     }

  return TRUE;
}
BOOL TMapView::VnModify(ChangeNotice *pCD)
{
  if (pCD->nWhich == 2)
     {
     LoadData();
     Invalidate();
     }

  return TRUE;
}

BOOL TMapView::VnDelete(ChangeNotice *pCD)
{
  if (pCD->nWhich == 2)
     {
     LoadData();
     if (pCD->pObj == CurrentRoom)
        // @@@ Cast-away const
        CurrentRoom = (TRoom *) AreaDoc->GetRoomFromIndex(0);

     Invalidate();
     }

  return TRUE;
}

BOOL TMapView::VnRevert(BOOL /*clear*/)
{
  LoadData();
  Invalidate();
  return TRUE;
}

#if 0
/************************************************************************
**    TEST FILE FOR graph (Dynamic Layout Alg)
**
**    MODUL - MANIPULATION OF THE GRAPH DATA STRUCTURE
**
** Author: dr. Szirmay-Kalos Laszlo (szirmay@fsz.bme.hu)
**	   Technical University of Budapest, Hungary
*************************************************************************/
//#include "fileio.h"

/************************************************************************/
/*    NODE								*/
/************************************************************************/
/*-------------------	node constructor      ------------------*/
/* Constructs node   object and initializes			*/
/* IN  : name and type (MOVEABLE or FIXED) of node		*/
/* OUT : -							*/
/*--------------------------------------------------------------*/
Node :: Node(pchar newname, TYPE ntype)
{
    newname[MAXNAME] = '\0';
    strcpy(name,newname);
    type  = ntype;
    force = vector(0.0, 0.0);
    pos	  = vector(0.0, 0.0);
    speed = vector(0.0, 0.0);
}


/************************************************************************/
/*    RELATION								*/
/************************************************************************/
/*------------------- relation constructor    ------------------*/
/* Constructs relation object and initializes			*/
/* IN  : name , related node, intensity,			*/
/*--------------------------------------------------------------*/
Relation :: Relation( pchar nname, Node * np, double r, int d )
{
    SetRelation( nname, r );
    relation_to = np;

    // @@@
    door = d;
}

/*-------------------  SetRelation     -------------------------*/
/* Change the name intensity and relation of a constructed rel	*/
/* IN  : name, intensity					*/
/*--------------------------------------------------------------*/
void Relation :: SetRelation( pchar nname, double r )
{
    strcpy( name, nname );
    intensity = r;
}

/************************************************************************/
/*    NODE ELEM								*/
/************************************************************************/
/*------------------- NodeElem	 constructor  ------------------*/
/* Constructs a node elem of a list object and initializes	*/
/* IN  : name and type (MOVEABLE or FIXED) of node		*/
/*--------------------------------------------------------------*/
NodeElem :: NodeElem( pchar name, TYPE type )
	  : Node( name, type )
{
    next_node = NULL;
    relation = NULL;
}


/************************************************************************/
/*    RELATION ELEM							*/
/************************************************************************/
/*------------------- RelationNode constructor	----------------*/
/* Constructs relation node of a list object and initializes	*/
/* IN  : name , related node, intensity,		       */
/*--------------------------------------------------------------*/
RelationElem :: RelationElem( pchar name, Node * p, double r, int door )
	      : Relation( name, p, r, door )
{
    next_relation = NULL;
}

/************************************************************************/
/*    GRAPH = NODE - RELATION DATA					*/
/*									*/
/*    The Graph data structure is a dynamic structure.			*/
/*									*/
/*    The nodes are placed on a singly linked list, where fix nodes	*/
/*    are on the beginning, and moveable nodes are on the end.		*/
/*    The nodes are also identified by serial numbers, the moveable	*/
/*    nodes are having positive while fixed nodes negative numbers.	*/
/*    Control pointers : currnode - points to the actual node		*/
/*			 relatenode - other node which forms a pair	*/
/*					with currnode for relation ops	*/
/*			 start_node - the beginning of the list		*/
/*			 last_node - the end of the list		*/
/*									*/
/*    The relations of a given node are stored on an other linked list	*/
/*    connected to the node of the given node. The relation node	*/
/*    contains name, type, intensity parameters and a pointer to the	*/
/*    related node. The relation of two node is stored on the		*/
/*    relation list of the node having smaller serial number!		*/
/*    Control pointers : currelation -points to the actual relation node*/
/*			 prevrelation - points to the relation just	*/
/*			    before currelation on the actual relation	*/
/*			    list.					*/
/*									*/
/*  STRUCTURE OVERVIEW: P = Node, R = RelationNode			*/
/*									*/
/*  start_node				lastnode			*/
/*     P ---> P ---> P ---> P ---> P ---> P ---> NULL			*/
/*     |	     ^	    |	   ^	  ^				*/
/*     R ------------|	    R------|	  |				*/
/*     |		    |		  |				*/
/*    NULL		    R-------------|				*/
/*			    |						*/
/*			   NULL						*/
/************************************************************************/
/*------------------ Graph Constructor -------------------------*/
/* Initializes Graph data structure				*/
/*--------------------------------------------------------------*/
Graph :: Graph()
{
    start_node = NULL;
    last_node  = NULL;
    currnode   = NULL;
    relatenode = NULL;
    currelation	 = NULL;
    prevrelation = NULL;
    nfixnode   = nmovnode = 0;
}

// @@@ Should do some deleting in here...
void Graph :: ClearNodes()
{
    start_node = NULL;
    last_node  = NULL;
    currnode   = NULL;
    relatenode = NULL;
    currelation	 = NULL;
    prevrelation = NULL;
    nfixnode   = nmovnode = 0;
}

/*------------------ RestoreNodes ----------------------------*/
/* Restores the node-relation data structure from a file	*/
/* The file type is TEXT.					*/
/* IN  : file name						*/
/* SIDE EFFECT: - node-relation data structure is destroyed	*/
/*		  then it is restored from the given file	*/
/*--------------------------------------------------------------*/
BOOL Graph :: RestoreNodes (TAreaDocument *AreaDoc)
{
#if 0
    char    s[MAXLINE + 1];
    char    node_name[MAXNAME + 1];
    char    rel_node_name[MAXNAME + 1];
    char    relation_name[MAXNAME + 1];
    double  x, y;
    double  relation;
    FileIO  fi ( "r" );
    BOOL    first_rel;

    if ( !fi.OpenFile ( file_name ) ) app.Error("Input file does not exists");
/*
*    RESTORE NODES
*/
    for ( ; ; ) {
/*
*    TRY TO INPUT NODE NAME
*/
	if ( !fi.GetKeyWord ( "NAME" ) ) {
/*
*    FAILED -> END OF NODE LIST
*/
	    first_rel = TRUE;
	    break;
	} else {
	    if ( !fi.GetOperator ( '=' ) ) app.Error( "= expected", fi.GetLineNum() );
	    if ( !fi.GetString(node_name,MAXNAME) ) app.Error( "Name expected", fi.GetLineNum() );
/*
*    TRY TO INPUT NODE POSITION
*/
	    if ( !fi.GetKeyWord("POSITION") ) {
/*
*    FAILED -> ASSUME NO POSITION, GENERATE RANDOMLY
*/
		x = (OVERWINDOW_X - WALL_MARGIN * 2.0) /
		    (double)RAND_MAX * (double)rand() + WALL_MARGIN;
		y = (OVERWINDOW_Y - WALL_MARGIN * 2.0) /
		    (double)RAND_MAX * (double)rand() + WALL_MARGIN;
		if ( !fi.GetKeyAgain("TYPE") ) app.Error( "TYPE expected", fi.GetLineNum() );
	    } else {
		if ( !fi.GetOperator ( '=' ) ) app.Error( "= expected", fi.GetLineNum() );
		if ( !fi.GetVariable( &x, 0.0, OVERWINDOW_X ) || !fi.GetVariable( &y, 0.0, OVERWINDOW_Y ) )
			app.Error( "Coordinate out of space", fi.GetLineNum() );
		if ( !fi.GetKeyWord("TYPE") )  app.Error( "TYPE expected", fi.GetLineNum() );
	    }
/*
*    TRY TO INPUT TYPE PARAMETERS
*/
	    if ( !fi.GetOperator ( '=' ) ) app.Error( "= expected", fi.GetLineNum() );
	    if ( !fi.GetString( s, MAXLINE ) ) app.Error("Line too long", fi.GetLineNum() );
/*
*    ADD NEW NODE TO THE DATA STRUCTURE AND CHECK THE NAME IF UNIQUE
*/
	    if ( strcmp( s, "FIXED" ) == 0 ) {
		if ( !AddNode(node_name, FIXED_NODE) )	  app.Error("Not unique node name", fi.GetLineNum() );
	    } else if ( strcmp( s, "MOVEABLE" ) == 0 ) {
		if ( !AddNode(node_name, MOVEABLE_NODE) ) app.Error("Not unique node name", fi.GetLineNum() );
	    } else					  app.Error("Invalid Node type", fi.GetLineNum() );

	    currnode -> Position( ) = vector( x, y );
	}
    }
/*
*    RESTORE RELATIONS
*/
    for( ; ; ) {
/*
*    TRY TO GET RELATION LIST HEAD, IF FAIL -> END OF INPUT
*/
	if ( first_rel ) {
	    if ( !fi.GetKeyAgain ( "RELATIONS" ) )  break;
	    first_rel = FALSE;
	} else {
	    if ( !fi.GetKeyWord ( "RELATIONS" ) )  break;
	}
	if ( !fi.GetKeyWord("OF") )  app.Error("OF expected", fi.GetLineNum() );
	if ( !fi.GetString( node_name, MAXNAME ) ) app.Error("Name too long", fi.GetLineNum() );
	if ( !fi.GetKeyWord ( "NODE" ) ) app.Error("NODE expected", fi.GetLineNum() );
/*
*    IDENTIFY NODE
*/
	if ( !SearchNode( node_name ) ) app.Error("Not declared Node", fi.GetLineNum() );
/*
*    TRY TO INPUT RELATION LIST
*/
	for( ; ; ) {		// get the whole relation list of this node
/*
*    TRY TO GET RELATION NAME
*/
	    if ( !fi.GetKeyWord ( "RELATION" ) ) {
/*
*    FAILED -> CHECK END OF RELATION MARKER
*/
		if ( !fi.GetKeyAgain ( "END" ) ) app.Error("END expected", fi.GetLineNum() );
		else break;
	    }
	    if ( !fi.GetString( relation_name, MAXNAME ) ) app.Error("Name too long", fi.GetLineNum() );
/*
*    CHECK IF NO-NAME RELATION
*/
	    if ( strcmp( relation_name, "*" ) == 0 ) relation_name[0] = '\0';
	    if ( !fi.GetOperator ( ':' ) ) app.Error(": expected", fi.GetLineNum() );
/*
*    TRY TO GET RELATED NODE WITH RELATION PARAMETERS
*/
	    if ( !fi.GetKeyWord("RELATED") )   app.Error("RELATED expected", fi.GetLineNum() );
	    if ( !fi.GetKeyWord("TO") )	       app.Error("TO expected", fi.GetLineNum() );
	    if ( !fi.GetString( rel_node_name, MAXNAME ) ) app.Error("Name expected", fi.GetLineNum() );
	    if ( !fi.GetKeyWord("WITH") )      app.Error("WIDTH expected", fi.GetLineNum() );
	    if ( !fi.GetKeyWord("INTENSITY") ) app.Error("INTENSITY expected", fi.GetLineNum() );

	    if ( !fi.GetVariable( &relation, -MAXRELATION, MAXRELATION ) )
		app.Error("Relation is out of range", fi.GetLineNum() );
/*
*    BUILD THE NEW RELATION INTO THE DATA STRUCTURE
*/
	    NodeElem * tmpnode = currnode;
	    if ( !RelSearchNode( rel_node_name ) ) app.Error("Not declared Node", fi.GetLineNum() );
	    AddRelation( relation_name, relation );
	    currnode = tmpnode;
	}
    }

    fi.CloseFile();
#else
   MessageBox(0, "This isn't the kind of map we want, sorry...", "MZF", MB_OK);
   ::SetCursor(::LoadCursor(0, IDC_WAIT));

   char szBuffer[256];

   // Turn all of the nodes into rooms
   int i = 0;
   const TRoom *r;
   while ((r = AreaDoc->GetRoomFromIndex(i++)) != 0)
     {
     wsprintf(szBuffer, "#%d", r->vnum);
     if (!AddNode(szBuffer, MOVEABLE_NODE))
        return FALSE;
     }

   // Turn all of the exits into relations
   char szBufferOther[256];
   i = 0;
   const TRoom *otherRoom;
   while ((r = AreaDoc->GetRoomFromIndex(i++)) != 0)
     {
     wsprintf(szBuffer, "#%d", r->vnum);
     for (int j = 0; j < nelems(r->exit); j++)
        if (r->is_exit[j])
           if ((otherRoom = r->exit[j].to_room) != 0)
              {
              wsprintf(szBufferOther, "#%d", otherRoom->vnum);
              // This can't be moved up, as it swaps with RelSearchNode...
              if ( !SearchNode( szBuffer ) )
                 return FALSE;
              if ( !RelSearchNode( szBufferOther ) )
                 return FALSE;

              AddRelation( "", (j < 4) ? 1.0 : 2.0, j );
              }

     }

  return TRUE;
#endif
}

#if 0
/*------------------  SaveNodes	  ----------------------------*/
/* Saves the node-relation data structure to a file name      */
/* The file type is TEXT.					*/
/* IN  : file name						*/
/*--------------------------------------------------------------*/
BOOL Graph :: SaveNodes ( pchar file_name )
{
    char s[MAXLINE];
    FileIO fo ( "w" );

    if ( !fo.OpenFile (file_name) ) return FALSE;
/*
*    SAVE NODES
*/
    if ( !FirstNode() ) {
	fo.CloseFile( );
	return TRUE;
    }

    do {
	sprintf(s,
		"NAME = %s POSITION =  %6.3lf %6.3lf TYPE = %s\n",
		currnode -> GetName(),
		currnode -> Position().X(),
		currnode -> Position().Y(),
		(currnode -> GetType() == FIXED_NODE ? "FIXED" : "MOVEABLE"));
	fo.PutString( s );
    } while ( NextNode() );

/*
*    SAVE RELATIONS
*/
    FirstNode();
    do {
	   sprintf(s,
		    "\nRELATIONS OF %s NODE\n",
		  currnode -> GetName());
	  fo.PutString( s );

	  if ( !FirstRelation() ) {
	    fo.PutString( "END\n" );	    // END OF RELATION MARKER
	      continue;
	  }

	  do {
	      if ( strlen( currelation -> GetName() ) != 0 )
		 sprintf(s, "RELATION %s : ", currelation -> GetName() );
	      else
		 sprintf(s, "RELATION * : ");
	      fo.PutString( s );

	      sprintf(s,
		      "RELATED TO %s WITH INTENSITY %6.3lf \n",
		      currelation -> GetOtherNode() -> GetName(),
		      currelation -> GetRelation() );
	      fo.PutString( s );
	  } while ( NextRelation() );

	  fo.PutString( "END\n" );	  // END OF RELATION MARKER

    } while ( NextNode() );

    fo.CloseFile( );
    return TRUE;
}
#endif

/*-------------------	  SetNodePos	  --------------------*/
/* Sets the position of a node				      */
/* IN  : new position						*/
/*--------------------------------------------------------------*/
void Graph :: SetNodePos ( vector p )
{
    if ( currnode != NULL ) currnode -> Position( ) = p;
}

/*------------------------ GetRelation	 -----------------------*/
/* Gets the relation intensity of the actual relation		*/
/* IN  : -							*/
/* OUT : intensity ( -MAXRELATION -> MAXRELATION )		*/
/*--------------------------------------------------------------*/
double Graph :: GetRelation( )
{
    if (currelation != NULL) return currelation -> GetRelation( );
    else		     return 0.0;
}

/*------------------------ GetRelationName ---------------------*/
/* Gets the name of the actual relation				*/
/* OUT : name of NULL if no relation				*/
/*--------------------------------------------------------------*/
pchar Graph :: GetRelationName( )
{
    if ( currelation != NULL ) return currelation -> GetName( );
    else		       return NULL;
}

/*------------------------   AddNode   -----------------------*/
/* Checks if a node having the same name exist and if not new */
/* node is allocated and added to the beginning of the list if*/
/* the node is FIXED or to the end if it is MOVEABLE	      */
/* IN  : name and type of the new node			      */
/* OUT : is this name unique ?				      */
/* SIDE EFFECT: currnode is set to the new node.	      */
/*		if FIXED node				      */
/*		  start_node adjusted, nfixnode incremented   */
/*		if MOVEABLE NODE			      */
/*		  last_node adjusted, nmovnode incremented    */
/*------------------------------------------------------------*/
BOOL Graph :: AddNode ( pchar name, char type )
{
/*
*    DECIDE IF THIS NAME IS UNIQUE, IF NOT RETURN ERROR
*/
    if ( SearchNode( name ) ) return FALSE;

    currnode = new NodeElem(name, type);

    if (start_node == NULL) {
/*
*    IF THIS IS THE FIRST NODE
*/
	start_node = last_node = currnode;
    } else {
	if ( type == FIXED_NODE ) {
/*
*    IF FIXED NODE -> ADD TO THE BEGINNING OF THE LIST
*/
	    currnode -> SetNext( start_node );
	    start_node = currnode;
	} else {
/*
*    IF MOVEABLE NODE -> ADD TO THE END OF THE LIST
*/
	    last_node -> SetNext( currnode );
	    last_node = currnode;
	}
    }
    if ( type == FIXED_NODE ) {
	nfixnode++;
	currnode -> SetSerNum( -nfixnode );
    } else {
	nmovnode++;
	currnode -> SetSerNum( nmovnode );
    }
    return TRUE;
}

/*------------------------  SearchNode	  --------------------*/
/* Searches node by name				      */
/* IN  : searched name					      */
/* OUT : is there node having this name ?		      */
/* SIDE EFFECT: currnode is set to the found node.	      */
/*------------------------------------------------------------*/
BOOL Graph :: SearchNode ( pchar name )
{
    if ( !FirstNode() ) return FALSE;
    do {
	if ( strcmp( currnode -> GetName(), name) == 0 ) return TRUE;
    } while ( NextNode () );

    return FALSE;
}

/*------------------------  RelSearchNode --------------------*/
/* Searches relate node by name				      */
/* IN  : picked position					*/
/* OUT : is there node in the pick aperture ?		      */
/* SIDE EFFECT:							*/
/*  To ensure that relatenode has greater serial number than  */
/*  currnode:						      */
/*     IF found node has smaller serial number than currnode*/
/*	      relatenode is set to currnode		    */
/*	      currnode is set to the found node		    */
/*     ELSE							*/
/*	      relatenode is set to the found node.	    */
/*  Initializes currelation to the relation of currnode and   */
/*  relatenode.						      */
/*--------------------------------------------------------------*/
BOOL Graph :: RelSearchNode ( pchar name )
{
    NodeElem * oldcurrnode = currnode;
    BOOL       found	   = SearchNode( name );

    if ( found ) {
	relatenode = currnode;
	currnode = oldcurrnode;
	SwapRelation( );
	SearchRelation( );
    } else {
	currelation = NULL;
	currnode = oldcurrnode;
    }
    return found;
}

/*------------------ SearchRelation ----------------------------*/
/* Search for a relation between currnode and relatenode.   */
/* If this relation doesnot exist currelation is NULL and	*/
/* prevrelation points to the end of the relation list of	*/
/* currnode.						      */
/* IN  : -							*/
/* OUT : EMPTY_LIST  - No relation list				*/
/*	   FIRST_FOUND - The first relation node found		*/
/*	   FOUND	 - Not the first node found		*/
/*	   NOT_FOUND	 - No such relation			*/
/* SIDE EFFECT: currelation= searched relation or NULL		*/
/*		prevrelation= the previous relation or the last */
/*			      node of the relation list or NULL */
/*			      if no node at all			*/
/*--------------------------------------------------------------*/
int Graph :: SearchRelation ( )
{
    currelation = currnode -> GetRelation( );
    prevrelation = currnode -> GetRelation( );
    if (currelation == NULL) return EMPTY_LIST;
    if (currelation -> GetOtherNode() == relatenode) return FIRST_FOUND;

    currelation = prevrelation -> GetNext();

    for ( ; ; ) {
	if (currelation == NULL)			 return NOT_FOUND;
	if (currelation -> GetOtherNode() == relatenode) return FOUND;
	prevrelation = currelation;
	currelation = prevrelation -> GetNext();
    }
}

/*------------------------  SwapRelation    --------------------*/
/*  To ensure that relatenode has greater serial number than  */
/*  currnode:						      */
/*	IF relatenode node has smaller serial number than   */
/*	relatenode and currnode are swapped		    */
/* IN  : -							*/
/* OUT : -							*/
/*--------------------------------------------------------------*/
void Graph :: SwapRelation ( )
{
    NodeElem * tmpnode;

    if ( currnode == NULL || relatenode == NULL ) return;

    if ( currnode -> GetSerNum() > relatenode -> GetSerNum() ) {
	tmpnode = currnode;
	currnode = relatenode;
	relatenode = tmpnode;
    }
}

/*--------------------- AddRelation ----------------------------*/
/* Adds new or changes the parameters of an existing relation.	*/
/* If this is a new relation RelationNode is allocated and	*/
/* placed on the end of relation list of currnode.	      */
/* The parameters are set according to the explicit parameters	*/
/*  and the implicit relatenode par.			      */
/* IN  : name,intensity, type					*/
/* OUT : -							*/
/* SIDE EFFECT: currelation= new or changed relation		*/
/*--------------------------------------------------------------*/
void Graph :: AddRelation ( pchar name, double rel, int d )
{
/*
*    CHECK IF THIS RELATION EXISTS OR FIND THE END OF RELATION LIST
*/
    switch ( SearchRelation( ) ) {

    case FIRST_FOUND:	     //	   THIS RELATION HAS BEEN ALREADY DEFINED
    case FOUND :
	currelation -> SetRelation( name, rel );
	return;

    case NOT_FOUND:	       //    NOT FIRST ADD NEW RELATION TO THE END OF LIST
	currelation = new RelationElem ( name, relatenode, rel, d );
	prevrelation -> SetNext ( currelation );
	return;

    case EMPTY_LIST:	    //	   THIS IS GOING TO BE THE FIRST
	currelation = new RelationElem ( name, relatenode, rel, d );
	currnode -> SetRelation( currelation );
	return;
    }
}

/*--------------------- FirstNode ----------------------------*/
/* Select currnode as start_node (beginning of the list	    */
/* IN  : -							*/
/* OUT : Are nodes on the list				      */
/*--------------------------------------------------------------*/
BOOL Graph :: FirstNode ( )
{
    if ( (currnode = start_node) == NULL ) return FALSE;
    else				   return TRUE;
}

/*--------------------- FirstMoveNode --------------------------*/
/* Select currnode as first moveable node on the list	    */
/* IN  : -							*/
/* OUT : Are moveable nodes on the list			      */
/*--------------------------------------------------------------*/
BOOL Graph :: FirstMoveNode ( )
{
    if ( (currnode = start_node) == NULL ) return FALSE;

    while ( currnode -> GetType() != MOVEABLE_NODE ) {
	currnode = currnode -> GetNext();
	if ( currnode == NULL ) return FALSE;
    }
    return TRUE;
}

/*---------------------	 NextNode ----------------------------*/
/* Let currnode be the next after currnode		      */
/* IN  : maximal serial number to be considered of ALL_NODES  */
/* OUT : Was it the last node?				      */
/* SIDE EFFECT: currnode = NULL if no more nodes	      */
/*------------------------------------------------------------*/
BOOL Graph :: NextNode ( int maxsernum )
{
    if ( maxsernum == ALL_NODES ) {
	if (( currnode = currnode -> GetNext() ) == NULL ) return FALSE;
    } else {
	if (( currnode = currnode -> GetNext() ) == NULL ||
	      currnode -> GetSerNum() > maxsernum ) return FALSE;
    }
    return TRUE ;
}

/*---------------------	 FirstRelation	------------------------*/
/* Select currrelation as first relation of the relation list	*/
/* of currnode.						      */
/* IN  : -							*/
/* OUT : Has the currnode any relation?			      */
/*--------------------------------------------------------------*/
BOOL Graph :: FirstRelation ( )
{
    if ( (currelation = currnode -> GetRelation()) == NULL ) {
	relatenode = NULL;
	return FALSE;
    } else {
	relatenode = (NodeElem *)( currelation -> GetOtherNode() );
	return TRUE;
    }
}

/*---------------------	 NextRelation --------------------------*/
/* Let currelation the next after currelation			*/
/* IN  : -							*/
/* OUT : Was it the last relation of the list?			*/
/*--------------------------------------------------------------*/
BOOL Graph :: NextRelation ( )
{
    if ( (currelation = currelation -> GetNext()) == NULL ) {
	relatenode = NULL;
	return FALSE;
    } else {
	relatenode = (NodeElem *)( currelation -> GetOtherNode() );
	return TRUE;
    }
}

/*------------------   RandomArrange ---------------------------*/
/* Random arrangement of nodes					*/
/*--------------------------------------------------------------*/
void Graph :: RandomArrange( )
{
/*
*    SKIP FIXED NODES
*/
    if ( !FirstMoveNode() ) return;
/*
*     MAIN CYCLE OF PLACING MOVEABLE NODES RANDOMLY
*/
    do {
	currnode -> Position( ) = vector((OVERWINDOW_X - WALL_MARGIN * 2.0) /
					 (double)RAND_MAX * (double)rand() + WALL_MARGIN,
					 (OVERWINDOW_Y - WALL_MARGIN * 2.0) /
					 (double)RAND_MAX * (double)rand() + WALL_MARGIN );
    } while ( NextNode() );
}

/************************************************************************/
/*  OBJECT SPACE							*/
/************************************************************************/
/*----------------- ObjectSpace Constructor --------------------*/
/* Initializes object space window				*/
/* IN  : -							*/
/* OUT : -							*/
/*--------------------------------------------------------------*/
ObjectSpace :: ObjectSpace( )
	:vwindow( 0, 0, (CoOrd)OVERWINDOW_Y, (CoOrd)OVERWINDOW_X ),
	 viewport( 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT )
{
}

/*------------------------ SetScale ----------------------------*/
/* Initializes window -> viewport transform			*/
/* IN  : -							*/
/* OUT : -							*/
/*--------------------------------------------------------------*/
void ObjectSpace :: SetScale()
{
    scale_x = (double)viewport.Width() / (double)vwindow.Width();
    scale_y = (double)viewport.Height() / (double)vwindow.Height();
}


/*--------------------- SetViewPort ----------------------------*/
/* Sets viewport (Canvas RectAngle)				*/
/* IN  : new viewport						*/
/* OUT : -							*/
/* SIDE EFFECT: Recalculates window->viewport transform		*/
/*--------------------------------------------------------------*/
void ObjectSpace :: SetViewPort( RectAngle v )
{
    viewport = v;
    SetScale();
}


/*--------------------- ScreenPos ------------------------------*/
/* Transform a point from object space to screen space		*/
/* IN  : object space position					*/
/* OUT : screen space coordinates of point			*/
/*--------------------------------------------------------------*/
Point ObjectSpace :: ScreenPos( vector p )
{
    CoOrd x = (CoOrd)((p.X()-(double)vwindow.HorPos()) * scale_x);
    CoOrd y = (CoOrd)((p.Y()-(double)vwindow.VerPos()) * scale_y);
    return Point(x,y);
}

/*--------------------- ScreenPos ------------------------------*/
/* Gets the position of a NODE in screen coordinate system	*/
/* IN  : pointer to the NODE					*/
/* OUT : screen space coordinates of NODE position		*/
/*--------------------------------------------------------------*/
Point ObjectSpace :: ScreenPos( NodeElem * pnode )
{
    return ScreenPos( pnode -> Position() );
}

/****************************************************************************
** DYNAMIC LAYOUT ALGORITHM OF GENERAL GRAPHS
**
** Author: dr. Szirmay-Kalos Laszlo (szirmay@fsz.bme.hu)
**	   Technical University of Budapest, Hungary
*****************************************************************************/

/*
*    CONSTANTS
*/
const double TIME_STEP = 0.1;	     // time step of diff equ

const double MAX_FORCE = 500.0;	     // force shows instability
const double MIN_FORCE = 2.0;	     // force cosidered as 0
const double MAX_TIME_SCALE = 10.0;  // scale of max time of solution

const double MINFRICTION = 0.6;	     // friction boundaries
const double MAXFRICTION = 0.9;
const double MINIINERTIA = 0.1;	     // inverse inertia boundaries
const double MAXIINERTIA = 0.4;

const double ZERO_DIST =	10.0;  // distance considered as 0
const double WALL_OUT_DRIVE =	80.0;  // forces of the wall
const double WALL_MARGIN_DRIVE = 1.0;

const double SCALECONSTRAINT = OVERWINDOW_X / 3.5 / MAXRELATION;
const double MINCONSTRAINT = OVERWINDOW_X / 7.0;     // minimal constraint


/****************************************************************************/
/* DYNAMIC LAYOUT base on MECHANICAL SYSTEM ANALOGY			    */
/* IN  : The serial number of the maximal moveable node to be considered    */
/* OUT : STOPPED  = All objects stopped					    */
/*	 INSTABLE = Instable, force goes to infinity			    */
/*	 TOO_LONG = Too much time elapsed				    */
/****************************************************************************/
int Graph :: DynamicLayout( int maxsernum )
/*--------------------------------------------------------------------------*/
{
/*
*    LOCALS
*/
    double constraint, friction, iinertia, dist;
    vector drive;			// drive forces
    vector direction;			// direction of drives
    double MAX_TIME = MAX_TIME_SCALE * (nmovnode + nfixnode + 1);
/*
*    INIT SPEED OF MOVEABLE NODES TO 0
*/
    if ( !FirstMoveNode() ) return STOPPED;
    do	currnode -> Speed( ) = vector(0.0, 0.0); while ( NextNode( maxsernum ) );
/*
*    MAIN CYCLE OF TIME IN THE SOLUTION OF DIFF EQUATION
*/
    for ( double t = 0.0 ; t < MAX_TIME ; t += TIME_STEP ) {
/*
*    INITIALIZE FORCE IN NODES TO 0
*/
	FirstNode();
	do currnode -> Force( ) = vector( 0.0, 0.0 ); while ( NextNode( maxsernum ) );
/*
*    CALCULATE FRICTION AND RESPONSE VALUES FROM t
*/
	friction = MINFRICTION + (MAXFRICTION - MINFRICTION) * t / MAX_TIME;
	iinertia = MAXIINERTIA - (MAXIINERTIA - MINIINERTIA) * t / MAX_TIME;
/*
*     CALCULATE DRIVE FORCE BETWEEN EACH PAIR OF NODES
*/
	FirstNode();
	do {
	  relatenode = currnode -> GetNext();
	  while ( relatenode != NULL && relatenode -> GetSerNum() <= maxsernum ) {

       // @@@ He is where it calculates the direction based on wanting to move
       // The objects closer...
	    direction = currnode -> Position( ) - relatenode -> Position( );
       dist  = direction.Size();
	    if ( dist < ZERO_DIST ) dist = ZERO_DIST;
/*
*    CALCULATE FORCE FROM THEIR RELATION
*/
	    switch( SearchRelation() ) {
	    case EMPTY_LIST:
	    case NOT_FOUND:
		constraint = MINCONSTRAINT + MAXRELATION * SCALECONSTRAINT;
		break;
	    case FOUND:
	    case FIRST_FOUND:
		constraint = MINCONSTRAINT + (MAXRELATION - currelation->GetRelation()) * SCALECONSTRAINT;
		break;
	    }

       // @@@This is my new code
//       const double Xs[] = { 0, -5.0, 0, 5.0, 5.0, -5.0 };
//       const double Ys[] = { 5.0, 0, -5.0, 0, 5.0, -5.0 };
////	    direction = currnode -> Position( ) - relatenode -> Position( ) -
////          vector(Xs[currelation->door], Ys[currelation->door]);
//	    direction = currnode -> Position( ) - relatenode -> Position( );
//       if (currelation)
//          direction = direction - vector(Xs[currelation->door], Ys[currelation->door]);
//	    dist  = direction.Size();
//	    if ( dist < ZERO_DIST ) dist = ZERO_DIST;

	    //	SET FORCE
	    drive = (constraint - dist) / dist * direction;
	    drive /= (double)(maxsernum + nfixnode);
	    currnode -> AddForce(drive);
	    relatenode -> AddForce(-drive);

	    relatenode = relatenode -> GetNext();
	  }
	} while ( NextNode( maxsernum ) );
/*
*    ADD ADDITIONAL FORCES AND DETERMINE MAXIMAL FORCE
*/
	double max_force = 0.0;

	FirstMoveNode();
	do {
/*
*   CALCULATE DRIVE FORCE OF BOUNDARIES AND ADD TO RELATION FORCES
*/
	   dist = currnode -> Position().X();
	   /*
	   *	FORCE OF LEFT WALL
	   */
	   if (dist < 0) {	      // OUT LEFT
	     drive = vector( -dist * WALL_OUT_DRIVE + WALL_MARGIN * WALL_MARGIN_DRIVE, 0.0 );
	     currnode -> AddForce(drive);
	   } else if (dist < WALL_MARGIN) {    // IN LEFT MARGIN
	     drive = vector((WALL_MARGIN - dist) * WALL_MARGIN_DRIVE, 0.0);
	     currnode -> AddForce(drive);
	   }
	   /*
	   *	FORCE OF THE RIGHT WALL
	   */
	   dist = currnode -> Position().X() - OVERWINDOW_X;

	   if (dist > 0) {	      // OUT RIGHT
	     drive = vector( -dist * WALL_OUT_DRIVE + WALL_MARGIN * WALL_MARGIN_DRIVE, 0.0);
	     currnode -> AddForce(drive);
	   } else if (-dist < WALL_MARGIN) {	// IN RIGHT MARGIN
	     drive = vector((-WALL_MARGIN - dist) * WALL_MARGIN_DRIVE, 0.0);
	     currnode -> AddForce(drive);
	   }

	   dist = currnode -> Position().Y();
	   /*
	   *	FORCE OF BOTTOM WALL
	   */
	   if (dist < 0) {	      // OUT BOTTOM
	     drive = vector(0.0, -dist * WALL_OUT_DRIVE + WALL_MARGIN * WALL_MARGIN_DRIVE );
	     currnode -> AddForce(drive);
	   } else if (dist < WALL_MARGIN) {    // IN BOTTOM MARGIN
	     drive = vector(0.0, (WALL_MARGIN - dist) * WALL_MARGIN_DRIVE);
	     currnode -> AddForce(drive);
	   }
	   /*
	   *	FORCE OF THE TOP WALL
	   */
	   dist = currnode -> Position().Y() - OVERWINDOW_Y;

	   if (dist > 0) {				      // OUT TOP
	     drive = vector( 0.0, -dist * WALL_OUT_DRIVE + WALL_MARGIN * WALL_MARGIN_DRIVE );
	     currnode -> AddForce(drive);
	   } else if (-dist < WALL_MARGIN) {		    // IN TOP MARGIN
	     drive = vector(0.0, (-WALL_MARGIN - dist) * WALL_MARGIN_DRIVE);
	     currnode -> AddForce(drive);
	   }
/*
*    MOVE NODE BY FORCE
*/
	    vector old_speed = currnode -> Speed( );
	    currnode -> Speed( ) = (1.0 - friction) * old_speed + iinertia * currnode -> Force( );
	    currnode -> Position( ) += 0.5 * (old_speed + currnode -> Speed( ) );

/*
*    CALCULATE MAXIMUM FORCE
*/
	    double abs_force = currnode -> Force().Size( );
	    if ( abs_force > max_force) max_force = abs_force;

	} while ( NextNode( maxsernum ) );
/*
*    STOP CALCULATION IF
*/
	if ( max_force < MIN_FORCE ) return STOPPED;  // All objects stopped
	if ( max_force > MAX_FORCE ) return INSTABLE; // Instable, force goes to infinity
    }
    return TOO_LONG; // Too much time elapsed
}

/************************************************************************/
/*    INITIAL PLACEMENT ALGORITHM					*/
/* OUT : STOPPED  = All objects stopped					*/
/*	 INSTABLE = Instable, force goes to infinity			*/
/*	 TOO_LONG = Too much time elapsed				*/
/************************************************************************/
int Graph :: Placement( )
/*----------------------------------------------------------------------*/
{
    vector	candidate;		// candidate position
    vector	relate_cent;		// center related objects
    vector	notrel_cent;		// center of related object
    vector	center( OVERWINDOW_X / 2, OVERWINDOW_Y / 2 );
    int		nrel;			// number of related objects
    int		nnotrel;		// displayed nodes
    double	perturb_x = OVERWINDOW_X / (double)RAND_MAX ;
    double	perturb_y = OVERWINDOW_Y / (double)RAND_MAX ;

/*
*    SKIP FIXED NODES
*/
    if ( !FirstMoveNode() ) return STOPPED;
/*
*     MAIN CYCLE OF INTRODUCING MOVABLE NODES STEP-BY-STEP
*/
    for( int inode = 1; ; inode++ ) {
/*
*    CALCULATE THE CENTER OF GRAVITY OF ALREADY INTRODUCED NODES
*    relate_cent IS FOR RELATED NODES
*    notrel_cent IS FOR NON_RELATED NODES
*/
	relate_cent = vector(0.0, 0.0);
	notrel_cent = vector(0.0, 0.0);
	nrel = 0;
	nnotrel = 0;		    // displayed nodes
	relatenode = currnode;

	for( FirstNode(); currnode != relatenode; NextNode() ) {
	    switch ( SearchRelation() ) {
	    case EMPTY_LIST:
	    case NOT_FOUND:
		    notrel_cent += currnode -> Position();
		    nnotrel++;
		    break;
	    case FIRST_FOUND:
	    case FOUND:
		    relate_cent += currnode -> Position();
		    nrel++;
		    break;
	    }
	}
	if ( nrel != 0 )       relate_cent /= (double)nrel;
	if ( nnotrel != 0 )    notrel_cent /= (double)nnotrel;
/*
*    IF THIS IS THE FIRST POINT -> PUT TO THE MIDDLE
*/
	if (nrel == 0 && nnotrel == 0) candidate = center;
	else
/*
*    IF NO NOT_RELATED NODE -> PUT TO THE CENTRE OF GRAVITY OF RELATED NODES
*/
	if ( nnotrel == 0 ) candidate = relate_cent;
	else
/*
*    IF NO RELATED NODE -> PUT TO THE MIRROR OF THE nrel_cent ON THE CENTRE
*/
	if ( nrel == 0 )  candidate = 2.0 * center - notrel_cent;
	else
/*
*    BOTH TYPE OF NODES EXIST ->
*    CALCULATE THE CANDIDATE POINT AS THE HALF MIRROR OF notrel_cent TO relate_cent
*/
	    candidate = 2.0 * relate_cent - 1.0 * notrel_cent;
/*
*    PERTURBATE RANDOMLY
*/
	candidate += vector( perturb_x / (double)(nfixnode + inode + 5) *
			     (double)( rand() - RAND_MAX / 2),
			     perturb_y / (double)(nfixnode + inode + 5) *
			     (double)( rand() - RAND_MAX / 2 ));
/*
*    DECIDE IF IT IS OUTSIDE -> FIND THE NEAREST INSIDE POINT
*/
	if ( candidate.X() < WALL_MARGIN)
	    candidate = vector( 2.0 * WALL_MARGIN, candidate.Y() );
	if ( candidate.X() > OVERWINDOW_X - WALL_MARGIN)
	    candidate = vector(OVERWINDOW_X - 2.0 * WALL_MARGIN, candidate.Y());

	if ( candidate.Y() < WALL_MARGIN)
	    candidate = vector( candidate.X(), 2.0 * WALL_MARGIN );
	if ( candidate.Y() > OVERWINDOW_Y - WALL_MARGIN)
	    candidate = vector(candidate.X(), OVERWINDOW_Y - 2.0 * WALL_MARGIN );

/*
*    SET POSITION OF THE NEW NODE
*/
	relatenode -> Position( ) = candidate;
/*
*    ARRANGE ALREADY DEFINED NODES BY DYNAMIC LAYOUT -> IGNORE EDGE CONSTRAINTS
*/
	NodeElem * oldcurrnode = currnode;
	char ret = DynamicLayout( inode );
	currnode = oldcurrnode;

	if ( ret != STOPPED || !NextNode() ) return ret;
    }
}

/*****************************************************************************
**    TEST FILE FOR graph (Dynamic Layout Alg)
**
**    MODUL   - 2D VECTOR OPERATIONS
**
** Author: dr. Szirmay-Kalos Laszlo (szirmay@fsz.bme.hu)
**	   Technical University of Budapest, Hungary
*****************************************************************************/
#include <math.h>

/*------------------ Overloaded + operator  --------------------*/
/* Adds two vectors						*/
/* IN  : reference of the two operands				*/
/* OUT : result vector						*/
/*--------------------------------------------------------------*/
vector operator+(vector& a, vector& b)
{
	vector sum;
	sum.x = a.x + b.x;
	sum.y = a.y + b.y;
	return sum;
}

/*--------------- Overloaded binary - operator	----------------*/
/* Subtract two vectors						*/
/* IN  : reference of the two operands				*/
/* OUT : result vector						*/
/*--------------------------------------------------------------*/
vector operator-(vector& a, vector& b)
{
	vector dif;
	dif.x = a.x - b.x;
	dif.y = a.y - b.y;
	return dif;
}

/*-------------- Overloaded unary - operator -------------------*/
/* Negates a vector						*/
/* IN  : operand						*/
/* OUT : negated vector						*/
/*--------------------------------------------------------------*/
vector operator-(vector& a)
{
	vector neg;
	neg.x = -a.x;
	neg.y = -a.y;
	return neg;
}

/*------------------ Overloaded * operator  --------------------*/
/* Multiplies a vector with a scalar				*/
/* IN  : vector and scalar operand				*/
/* OUT : result vector						*/
/*--------------------------------------------------------------*/
vector operator*(vector& a, double s)
{
	vector scaled;
	scaled.x = s * a.x;
	scaled.y = s * a.y;
	return scaled;
}

vector operator*(double s, vector& a)
{
	vector scaled;
	scaled.x = s * a.x;
	scaled.y = s * a.y;
	return scaled;
}

/*------------------------- Size -------------------------------*/
/* Calculates the absolute value of the vector			*/
/* IN  : -							*/
/* OUT : length							*/
/*--------------------------------------------------------------*/
double vector::Size()
{
	return sqrt( x * x + y * y );
}
#endif

void TMapView::EvLButtonDown (UINT modKeys, TPoint& point)
{
    TWindowView::EvLButtonDown(modKeys, point);

    // INSERT>> Your code here.

    // Did the mouse go down in a box?
   if (CurrentRoom)
      {
      for (int i = 0; i < 6; i++)
         {
         TRect r(xLeft[Xs[i]], yTop[Ys[i]], xRight[Xs[i]], yBottom[Ys[i]]);
         if (r.Contains(point))
            {
            if (CurrentRoom && CurrentRoom->is_exit[i] &&
                   CurrentRoom->exit[i].to_room)
               {
               CurrentRoom = CurrentRoom->exit[i].to_room;
               InvalidateRect(TRect(xLeft[0], yTop[0], xRight[2], yBottom[2]+20));
               int nNdx = AreaDoc->GetIndexFromRoom(CurrentRoom);
               if (nNdx >= 0)
                  RoomList->SetSelIndex(nNdx);
//               Invalidate();
               }

            return;
            }
         }

    if (TRect(xLeft[1], yTop[1], xRight[1], yBottom[1]).Contains(point))
       {
       int nNdx = AreaDoc->GetIndexFromRoom(CurrentRoom);
       if (nNdx != -1)
          AreaDoc->EditItem(2, nNdx);
       else
          MessageBeep(-1);
       }
    }
}


void TMapView::EvSize (UINT sizeType, TSize& size)
{
    TWindowView::EvSize(sizeType, size);

    // INSERT>> Your code here.
    RoomList->MoveWindow(xRight[2]+10, 0, size.cx - (xRight[2]+10), size.cy,
       TRUE);
}

void TMapView::CmListChanged()
{
   const TRoom* Room = AreaDoc->GetRoomFromIndex(RoomList->GetSelIndex());
   if (Room)
      {
      // @@@ CAST-AWAY CONST
      CurrentRoom = (TRoom* ) Room;
      InvalidateRect(TRect(xLeft[0], yTop[0], xRight[2], yBottom[2]+20));
//      Invalidate();
      }
   else
      MessageBeep(-1);
}

void TMapView::CmListSelected()
{
   int nNdx = AreaDoc->GetIndexFromRoom(CurrentRoom);
   if (nNdx != -1)
      AreaDoc->EditItem(2, nNdx);
   else
      MessageBeep(-1);
}

BOOL TMapView::VnIsWindow(HWND hWnd)
{
   return ((HWindow == hWnd) || IsChild(hWnd));
}


void TMapView::CmEditItem ()
{
    // INSERT>> Your code here.
    int nNdx = AreaDoc->GetIndexFromRoom(CurrentRoom);
    if (nNdx != -1)
       AreaDoc->EditItem(2, nNdx);
    else
       MessageBeep(-1);
}


void TMapView::CmEditAdd ()
{
    // INSERT>> Your code here.
    AreaDoc->EditItem(2, -1);
}


void TMapView::CmEditCopy ()
{
    // INSERT>> Your code here.
    int nNdx = AreaDoc->GetIndexFromRoom(CurrentRoom);
    if (nNdx != -1)
       AreaDoc->CopyItem(2, nNdx);
    else
       MessageBeep(-1);
}

bool TMapView::VnDocClosed(int omode)
{
  if (!(omode & ofWrite))  // make sure someone else's write
    return false;
  return true;
}

#ifndef __FLAT__
//============================================================================================
#define NumTValues 16

static class BezierConst {
   static float t3[], t2[], t1[], t0[];
   void bezier_const();
public:
   BezierConst() { bezier_const(); }

   class point2 { /* 2D point */
   public:
      float x, y;
   };

static void BezierConst::cubic_bezier(HDC hDC, point2& b0, point2& b1, point2& b2, point2& b3);
} BezierConstInstance;

// These routines implement the Win32 bezier function
float BezierConst::t3[NumTValues], BezierConst::t2[NumTValues],
      BezierConst::t1[NumTValues], BezierConst::t0[NumTValues];

// Cubic Bezier curve in Bernstein form
// compute the T, (1-T) values just once
void BezierConst::bezier_const()
{
   float t, d;
   for (int i=0; i<NumTValues; i++)
      {
      t = (float) (i+1)/NumTValues;
      d = 1.0-t;
      t3[i] = d*d*d;
      t2[i] = 3*d*d*t;
      t1[i] = 3*d*t*t;
      t0[i] = t*t*t;
      }
}

// Cubic Bezier Curve in Bernstein form
void BezierConst::cubic_bezier(HDC hDC, point2& b0, point2& b1, point2& b2, point2& b3)
{
   point2 oldp = b0, newp;
   for(int i=0; i<NumTValues; i++)
      {
      newp.x = t3[i]*b0.x + t2[i]*b1.x + t1[i]*b2.x + t0[i]*b3.x;
      newp.y = t3[i]*b0.y + t2[i]*b1.y + t1[i]*b2.y + t0[i]*b3.y;
//      newp.z = t3[i]*b0.z + t2[i]*b1.z + t1[i]*b2.z + t0[i]*b3.z;
      MoveTo(hDC, oldp.x, oldp.y);
      LineTo(hDC, newp.x, newp.y);
      oldp = newp;
      }
}

BOOL PASCAL PolyBezier(HDC hdc, const TPoint* lppt, DWORD cPoints)
{
	BezierConst::point2 pts[4];
   for (int i = 0; i < nelems(pts); i++)
      { pts[i].x = lppt[i].x; pts[i].y = lppt[i].y; }

   DWORD dw = GetCurrentPosition(hdc);
   for (i = 0; i+3 < cPoints; i += 3)
      BezierConst::cubic_bezier(hdc, pts[i], pts[i+1], pts[i+2], pts[i+3]);

   MoveTo(hdc, LOWORD(dw), HIWORD(dw));
   return TRUE;
}
#endif

