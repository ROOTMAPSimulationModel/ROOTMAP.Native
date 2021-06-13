/////////////////////////////////////////////////////////////////////////////
// Name:        RmSplashWindow.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     24/06/2006 16:16:14
// RCS-ID:      
// Copyright:   ©2006 University of Tasmania
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "RmSplashWindow.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "RmSplashWindow.h"

/*!
 * RmSplashWindow type definition
 */

IMPLEMENT_DYNAMIC_CLASS(RmSplashWindow, wxDialog)

/*!
 * RmSplashWindow event table definition
 */

BEGIN_EVENT_TABLE(RmSplashWindow, wxDialog)

////@begin RmSplashWindow event table entries
////@end RmSplashWindow event table entries

END_EVENT_TABLE()

RmSplashWindow::RmSplashWindow()
{
}

RmSplashWindow::RmSplashWindow(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * RmSplashWindow creator
 */

bool RmSplashWindow::Create(wxWindow* parent, wxWindowID /*id*/, const wxString& /*caption*/, const wxPoint& /*pos*/, const wxSize& /*size*/, long /*style*/)
{
    ////@begin RmSplashWindow member initialisation
    ////@end RmSplashWindow member initialisation

    ////@begin RmSplashWindow creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    SetParent(parent);
    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    ////@end RmSplashWindow creation
    return true;
}

/*!
 * Control creation for RmSplashWindow
 */

void RmSplashWindow::CreateControls()
{
    ////@begin RmSplashWindow content construction
    if (ShowToolTips())
        this->SetToolTip(_("Click in the window to dismiss"));
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("rmID_SPLASH_DIALOG")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    ////@end RmSplashWindow content construction

    // Create custom windows not generated automatically here.
    ////@begin RmSplashWindow content initialisation
    ////@end RmSplashWindow content initialisation
}

/*!
 * Should we show tooltips?
 */

bool RmSplashWindow::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap RmSplashWindow::GetBitmapResource(const wxString& name)
{
    // Bitmap retrieval
    ////@begin RmSplashWindow bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
    ////@end RmSplashWindow bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon RmSplashWindow::GetIconResource(const wxString& name)
{
    // Icon retrieval
    ////@begin RmSplashWindow icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
    ////@end RmSplashWindow icon retrieval
}
