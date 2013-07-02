#ifndef __tooltip_h
#define __tooltip_h

#include <owl\controlb.h>
#include <owl\statusba.h>
#include <owl\buttonga.h>

class TTipControlBar;
class TTipStatusBar;

class TToolTipFont : public TFont
{

public:
	TToolTipFont ();
};

class Tip
{
public:
	enum Style {
		SquareBorder	= 0x00,
		RoundedBorder	= 0x01,
		Shadow			= 0x02,
		};
};

class TToolTip : private TWindow
{
	friend	TTipControlBar;
	friend	TTipStatusBar;

	UINT	uiTimer;
	BOOL	bEnabled;
	TSize	sizeText;

	Tip::Style	style;
	TFont* 	font;
	
	void	KillTipTimer ();
	void	ShowNow ();
	void	PositionTip ();

protected:
	void	GetWindowClass (WNDCLASS &wc);
	LPSTR	GetClassName ();

	void 	Paint (TDC &dc, BOOL erase, TRect &rc);
	void	EvTimer (UINT uiTimerId);

	void	SetCaption (const char far* title);

public:
	TToolTip (Tip::Style style = Tip::SquareBorder, TFont* font = new TToolTipFont);
	~TToolTip ();

	BOOL	AreTipsEnabled () 					{ return bEnabled; }
	void	EnableTips (BOOL bEnable = TRUE) 	{ bEnabled = bEnable; }
	void	HideTip ()							{ SetCaption (NULL) ; }

	DECLARE_RESPONSE_TABLE (TToolTip);
};

class TTipControlBar : public TControlBar
{
	TToolTip&	tooltip;

public:
	TTipControlBar (TToolTip& tip, TWindow* parent = 0, TTileDirection direction = Horizontal, TFont* font = new TGadgetWindowFont, TModule* module = 0);

	void 	EvMouseMove (UINT modKeys, TPoint& point);
	void 	EvLButtonDown (UINT modKeys, TPoint& point);

	void 	SetHintText (const char *lpszText);

	DECLARE_RESPONSE_TABLE (TTipControlBar);
};

class TTipStatusBar : public TStatusBar
{
	TToolTip&	tooltip;
	BOOL		bShowTips;

public:
	TTipStatusBar (	TToolTip& 				tip,
					TWindow*				parent = 0,
					TGadget::TBorderStyle 	borderStyle = TGadget::Recessed,
					UINT                  	modeIndicators = 0,
					TFont*                	font = new TGadgetWindowFont,
					TModule*              	module = 0);

	void 			EvMouseMove (UINT modKeys, TPoint& point);
	void 			EvLButtonDown (UINT modKeys, TPoint& point);

	void 			SetHintText (const char *lpszText);
	virtual void 	SetTextEx (const char *lpszText) { SetText (lpszText); }
	void			UseTips ();

	DECLARE_RESPONSE_TABLE (TTipStatusBar);
};

#endif