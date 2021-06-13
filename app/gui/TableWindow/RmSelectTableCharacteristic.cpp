/////////////////////////////////////////////////////////////////////////////
// Name:        RmSelectTableCharacteristic.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     10/02/2007 18:07:28
// RCS-ID:      
// Copyright:   ©2006 University of Tasmania
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "RmSelectTableCharacteristic.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "RmSelectTableCharacteristic.h"

////@begin XPM images
////@end XPM images

/*!
 * RmSelectTableCharacteristic type definition
 */

IMPLEMENT_DYNAMIC_CLASS(RmSelectTableCharacteristic, wxDialog)

/*!
 * RmSelectTableCharacteristic event table definition
 */

    BEGIN_EVENT_TABLE(RmSelectTableCharacteristic, wxDialog)

    ////@begin RmSelectTableCharacteristic event table entries
    ////@end RmSelectTableCharacteristic event table entries

    END_EVENT_TABLE()

    /*!
     * RmSelectTableCharacteristic constructors
     */

    RmSelectTableCharacteristic::RmSelectTableCharacteristic()
{
    Init();
}

RmSelectTableCharacteristic::RmSelectTableCharacteristic(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
{
    Init();
    Create(parent, id, caption, pos, size, style);
}

/*!
 * SelectTableCharacteristic creator
 */

bool RmSelectTableCharacteristic::Create(wxWindow* parent, wxWindowID /*id*/, const wxString& /*caption*/, const wxPoint& /*pos*/, const wxSize& /*size*/, long /*style*/)
{
    ////@begin RmSelectTableCharacteristic creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    SetParent(parent);
    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    ////@end RmSelectTableCharacteristic creation
    return true;
}

/*!
 * Member initialisation
 */

void RmSelectTableCharacteristic::Init()
{
    ////@begin RmSelectTableCharacteristic member initialisation
    m_listBox = __nullptr;
    ////@end RmSelectTableCharacteristic member initialisation
}

/*!
 * Control creation for SelectTableCharacteristic
 */

void RmSelectTableCharacteristic::CreateControls()
{
    ////@begin RmSelectTableCharacteristic content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("rmID_SELECTTABLECHARACTERISTIC_DIALOG")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_listBox = XRCCTRL(*this, "rmID_SELECTTABLECHARACTERISTIC_DIALOG_LISTBOX", wxListBox);
    ////@end RmSelectTableCharacteristic content construction

    // Create custom windows not generated automatically here.
    ////@begin RmSelectTableCharacteristic content initialisation
    ////@end RmSelectTableCharacteristic content initialisation
}

/*!
 * Should we show tooltips?
 */

bool RmSelectTableCharacteristic::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap RmSelectTableCharacteristic::GetBitmapResource(const wxString& name)
{
    // Bitmap retrieval
    ////@begin RmSelectTableCharacteristic bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
    ////@end RmSelectTableCharacteristic bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon RmSelectTableCharacteristic::GetIconResource(const wxString& name)
{
    // Icon retrieval
    ////@begin RmSelectTableCharacteristic icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
    ////@end RmSelectTableCharacteristic icon retrieval
}
