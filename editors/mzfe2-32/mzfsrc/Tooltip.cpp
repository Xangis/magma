// =========================================================
// TOOLTIP.CPP
//
// Tooltip classes for OWL 2
//
// Author:  Steve Saxon (Compuserve: 100321,2355)
// Written: 26th June 1994
// =========================================================

#include <shellapi.h>
#include <owl\owlpch.h>
#pragma hdrstop

#include "tooltip.h"

#define TIP_DELAY_FIRST	600
#define TIP_DELAY_AFTER	150

#define ID_TIMER	1000

static BOOL 	bEnableHints = FALSE;	// prevents menus displaying hints!!

static TToolTip *ptTooltip;				// pointer used by KbdProc
static HHOOK	hookKbd;				// hookchain used by KbdProc

LRESULT CALLBACK KbdProc (int code, WPARAM wParam, LPARAM lParam);

DEFINE_RESPONSE_TABLE1 (TToolTip, TWindow)
  EV_WM_TIMER,
END_RESPONSE_TABLE;

// ================================================================ //

TToolTipFont::TToolTipFont () : TFont ("MS Sans Serif", -9)
{
}

// ================================================================ //

TToolTip::TToolTip (Tip::Style _style, TFont* _font) : TWindow (NULL, "")
{
	style			= _style;
	font			= _font;

	Attr.Style 		= WS_POPUP;
	Attr.ExStyle 	= WS_EX_TOPMOST;

	uiTimer			= NULL;
	bEnabled		= TRUE;

#ifdef __WIN32__
	::hookKbd		= SetWindowsHookEx (WH_KEYBOARD, KbdProc, NULL, GetCurrentThreadId ());
#else
	::hookKbd		= SetWindowsHookEx (WH_KEYBOARD, KbdProc, GetApplication ()->GetInstance (), GetCurrentTask ());
#endif

	::ptTooltip		= this;

	Create ();
}

TToolTip::~TToolTip ()
{
	KillTipTimer ();

	UnhookWindowsHookEx (hookKbd);

	delete font;
}

void TToolTip::GetWindowClass (WNDCLASS &wc)
{
	TWindow::GetWindowClass (wc);

	// no background brush
	wc.hbrBackground = NULL;

	wc.style |= CS_SAVEBITS;
}

LPSTR TToolTip::GetClassName ()
{
	return "Steve:Tooltip";
}

void TToolTip::Paint (TDC &dc, BOOL, TRect &)
{
	char	szText[50];
	TRect	client;

	// get the tooltip text
	GetWindowText (szText, sizeof (szText));
	GetClientRect (client);

	if (style & Tip::Shadow)
	{
		TMemoryDC	dcMem (dc);
		TBitmap	bmp (client.right, client.bottom);

		client.right	-= 2;
		client.bottom	-= 2;
		
		{
			BYTE		byData[] = { 0x55, 0, 0xAA, 0, 0x55, 0, 0xAA, 0, 0x55, 0, 0xAA, 0, 0x55, 0, 0xAA, 0 };
			TBitmap		bm (8, 8, 1, 1, byData);
			TBrush		brush (bm);

			dcMem.SelectObject (bmp);

			dcMem.FillRect (client, (HBRUSH) GetStockObject (WHITE_BRUSH));

			dcMem.SelectObject (brush);
			dcMem.SelectStockObject (NULL_PEN);
			dcMem.SetTextColor (TColor::Black);
			dcMem.SetBkColor (TColor::White);

			if (style & Tip::RoundedBorder)
			{
				int rad = client.Height () / 2;

				dcMem.RoundRect (client, TPoint (rad, rad));
			}
			else
			{
				dcMem.Rectangle (client);
			}
		}

		TRect	rcTemp = client.OffsetBy (2, 2);

		dc.SetTextColor (RGB (128, 128, 128));
		dc.BitBlt (rcTemp, dcMem, TPoint (0, 0), SRCAND);

		client.right	-= 2;
		client.bottom	-= 2;
	}

	TPen	pen (GetSysColor (COLOR_WINDOWFRAME));
	TBrush	brush (RGB (255, 255, 128));

	dc.SelectObject (pen);
	dc.SelectObject (brush);

	if (style & Tip::RoundedBorder)
	{
		int rad = client.Height () / 2;

		dc.RoundRect (client, TPoint (rad, rad));
	}
	else
	{
		dc.Rectangle (client);
	}

	// set up the device context
	dc.SetBkMode (TRANSPARENT);
	dc.SelectObject (*font);

	// draw the text
	dc.SetTextColor (TColor::Black);
	dc.DrawText (szText, -1, client, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
}

void TToolTip::SetCaption (const char far* title)
{
	static DWORD dwTickCount = 0;

	TWindow::SetCaption (title);

	// if the caption is missing, hide the tip window
	if (title == NULL || !*title || !bEnabled)
	{
		KillTipTimer ();

		Show (SW_HIDE);

		dwTickCount = GetTickCount ();
	}
	else
	{
		DWORD dwTickNew = GetTickCount ();

		// work out the extent of the text
		{
			TClientDC	dc (HWindow);

			dc.SelectObject (*font);
			sizeText = dc.GetTextExtent (title, lstrlen (title));

			sizeText.cx	+= 5;
			sizeText.cy	+= 4;

			Show (SW_HIDE);
		}

		// create the timer - this will send a WM_TIMER message
		// after 'TIP_DELAY_xxxx' milliseconds

		if (dwTickNew - dwTickCount > 0)
		{
			uiTimer = SetTimer (ID_TIMER, TIP_DELAY_FIRST);
		}
		else
		{
			uiTimer = SetTimer (ID_TIMER, TIP_DELAY_AFTER);
		}

		dwTickCount = 0;
	}
}

void TToolTip::PositionTip ()
{
	// position the tip relative to the mouse
	TPoint	ptMouse;
	TSize	scr (GetSystemMetrics (SM_CXSCREEN), GetSystemMetrics (SM_CYSCREEN));

	GetCursorPos (ptMouse);

	ptMouse.x	-= 2;
	ptMouse.y	+= 22;

	TRect	rc (ptMouse, sizeText);

	if (style & Tip::RoundedBorder)
	{
		rc = rc.InflatedBy (1, 1);
	}

	// check x screen position
	if (rc.left < 0)
	{
		rc.Offset (-rc.left + 2, 0);
	}
	else
	{
		if (rc.right > scr.cx)
		{
			rc.Offset (scr.cx - rc.right - 2, 0);
		}
	}

	// check y screen position
	if (rc.bottom > scr.cy)
	{
		rc.Offset (0, -42);
	}

	if (style & Tip::Shadow)
	{
		rc.right	+= 4;
		rc.bottom	+= 4;
	}

	SetWindowPos (NULL, rc, SWP_NOZORDER | SWP_NOACTIVATE);
}

void TToolTip::KillTipTimer ()
{
	// destroy the timer
	if (uiTimer)
	{
		KillTimer (ID_TIMER);
		uiTimer = NULL;
	}
}

void TToolTip::ShowNow ()
{
	// position the tip window
	PositionTip ();

	// show the tip window
	Show (SW_SHOWNA);
	UpdateWindow ();
}

void TToolTip::EvTimer (UINT)
{
	// timer message received - show the tip window!
	if (uiTimer)
	{
		KillTipTimer ();	// prevent further timer messages

		ShowNow ();
	}
}

// ================================================================ //
// override class for control bar
// ================================================================ //

DEFINE_RESPONSE_TABLE1 (TTipControlBar, TControlBar)
  EV_WM_MOUSEMOVE,
  EV_WM_LBUTTONDOWN,
END_RESPONSE_TABLE;

TTipControlBar::TTipControlBar (TToolTip& tip, TWindow* parent, TTileDirection direction, TFont* font, TModule* module)
	 : 	TControlBar (parent, direction, font, module),
		tooltip (tip)
{
	SetHintMode(TGadgetWindow::EnterHints);
}

void TTipControlBar::EvMouseMove (UINT modKeys, TPoint& point)
{
	if (!Capture && !GadgetFromPoint (point))
	{
		// hide the tip window if not over a gadget
		tooltip.HideTip ();
	}

	bEnableHints = TRUE;

	TControlBar::EvMouseMove (modKeys, point);

	bEnableHints = FALSE;
}

void TTipControlBar::EvLButtonDown (UINT modKeys, TPoint& point)
{
	// hide the tip window if mouse-button pressed
	tooltip.HideTip ();

	TControlBar::EvLButtonDown (modKeys, point);
}

// ================================================================ //
// override class for status bar
// ================================================================ //

DEFINE_RESPONSE_TABLE1 (TTipStatusBar, TStatusBar)
  EV_WM_MOUSEMOVE,
  EV_WM_LBUTTONDOWN,
END_RESPONSE_TABLE;

TTipStatusBar::TTipStatusBar (TToolTip& tip, TWindow* parent, TGadget::TBorderStyle borderStyle, UINT modeIndicators, TFont *font, TModule* module)
	: 	TStatusBar (parent, borderStyle, modeIndicators, font, module),
		tooltip (tip)
{
  	bShowTips = FALSE;
}

void TTipStatusBar::SetHintText (const char *lpszText)
{
	// when hint message displayed on the status bar,
	// pick out the tooltip text and display it (with delay)

	if (lpszText != NULL)
	{
		static char	buf[128];

		lstrcpy (buf, lpszText);

		// locate the tooltip text
		for (int n = 0; buf[n] && buf[n] != '\n'; n++) ;

		if (buf[n])
		{
			buf[n++] = '\0';
		}

		// only display hints from gadgets (not menus!)
		if (bEnableHints)
		{
			tooltip.SetCaption (buf + n);
		}

		if (bShowTips)
		{
			SetTextEx (buf);
		}
		else
		{
			TStatusBar::SetHintText (buf);
		}
	}
	else
	{
		tooltip.SetCaption (NULL);

		if (bShowTips)
		{
			SetTextEx (NULL);
		}
		else
		{
			TStatusBar::SetHintText (NULL);
		}
	}
}

void TTipStatusBar::EvMouseMove (UINT modKeys, TPoint& point)
{
	if (bShowTips)
	{
		if (!Capture && !GadgetFromPoint (point))
		{
			// hide the tip window if not over a gadget
			tooltip.HideTip ();
		}

		bEnableHints = TRUE;

		TStatusBar::EvMouseMove (modKeys, point);

		bEnableHints = FALSE;
	}
	else
	{
		TStatusBar::EvMouseMove (modKeys, point);
	}
}

void TTipStatusBar::EvLButtonDown (UINT modKeys, TPoint& point)
{
	if (bShowTips)
	{
		// hide the tip window if mouse-button pressed
		tooltip.HideTip ();
	}

	TStatusBar::EvLButtonDown (modKeys, point);
}

void TTipStatusBar::UseTips ()
{
	bShowTips = TRUE;

	Margins.Units = TMargins::BorderUnits;

	if (Direction == Horizontal)
	{
		Margins.Left = Margins.Right = 6;
		Margins.Top = Margins.Bottom = 2;
	}
}

// =========================================================

LRESULT CALLBACK KbdProc (int code, WPARAM wParam, LPARAM lParam)
{
	if (code >= 0)
	{
		ptTooltip->HideTip ();
	}

	return CallNextHookEx (hookKbd, code, wParam, lParam);
}


