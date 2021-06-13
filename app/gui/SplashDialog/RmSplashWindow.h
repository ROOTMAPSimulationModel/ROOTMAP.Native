/////////////////////////////////////////////////////////////////////////////
// Name:        RmSplashWindow.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     24/06/2006 16:16:14
// RCS-ID:      
// Copyright:   ©2006 University of Tasmania
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _RMSPLASHWINDOW_H_
#define _RMSPLASHWINDOW_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "RmSplashWindow.h"
#endif

#include "wx/xrc/xmlres.h"

 /*!
  * Control identifiers
  */

  ////@begin control identifiers
#define rmID_SPLASH_DIALOG 10036
#define SYMBOL_RMSPLASHWINDOW_STYLE 0
#define SYMBOL_RMSPLASHWINDOW_TITLE _("Splash")
#define SYMBOL_RMSPLASHWINDOW_IDNAME rmID_SPLASH_DIALOG
#define SYMBOL_RMSPLASHWINDOW_SIZE wxDefaultSize
#define SYMBOL_RMSPLASHWINDOW_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */
#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

class RmSplashWindow : public wxDialog
{
    DECLARE_DYNAMIC_CLASS(RmSplashWindow)
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    RmSplashWindow();
    RmSplashWindow(wxWindow* parent, wxWindowID id = SYMBOL_RMSPLASHWINDOW_IDNAME, const wxString& caption = SYMBOL_RMSPLASHWINDOW_TITLE, const wxPoint& pos = SYMBOL_RMSPLASHWINDOW_POSITION, const wxSize& size = SYMBOL_RMSPLASHWINDOW_SIZE, long style = SYMBOL_RMSPLASHWINDOW_STYLE);

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = SYMBOL_RMSPLASHWINDOW_IDNAME, const wxString& caption = SYMBOL_RMSPLASHWINDOW_TITLE, const wxPoint& pos = SYMBOL_RMSPLASHWINDOW_POSITION, const wxSize& size = SYMBOL_RMSPLASHWINDOW_SIZE, long style = SYMBOL_RMSPLASHWINDOW_STYLE);

    /// Creates the controls and sizers
    void CreateControls();

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource(const wxString& name);

    /// Retrieves icon resources
    wxIcon GetIconResource(const wxString& name);

    /// Should we show tooltips?
    static bool ShowToolTips();
};

#endif
// _RMSPLASHWINDOW_H_
