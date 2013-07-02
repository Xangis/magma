#if !defined(__apxprint_h)              // Sentry use file only if it's not already included.
#define __apxprint_h

/*  Main pounds
    
    Copyright © 1994. All Rights Reserved.

    SUBSYSTEM:    pounds.exe Application
    FILE:         APXPrint.H
    AUTHOR:       Slash


    OVERVIEW
    ========
    Class definition for APXPrintOut (TPrintOut).      
*/


#include <owl\owlpch.h>
#pragma hdrstop

#include <owl\printer.h>


class APXPrintOut : public TPrintout {
public:
    APXPrintOut (TPrinter *printer, const char far *title, TWindow* window, BOOL scale = TRUE) : TPrintout(title)
      { Printer = printer; Window = window; Scale = scale; MapMode = MM_ANISOTROPIC; }

    void GetDialogInfo (int& minPage, int& maxPage, int& selFromPage, int& selToPage);
    void BeginPrinting ();
    void BeginPage (TRect &clientR);
    void PrintPage (int page, TRect& rect, unsigned flags);
    void EndPage ();
    void SetBanding (BOOL b)        { Banding = b; }
    BOOL HasPage (int pageNumber);

protected:
    TWindow     *Window;
    BOOL        Scale;
    TPrinter    *Printer;
    int         MapMode;

    int         PrevMode;
    TSize       OldVExt, OldWExt;
    TRect       OrgR;
};


#endif          // __apxprint_h sentry.
