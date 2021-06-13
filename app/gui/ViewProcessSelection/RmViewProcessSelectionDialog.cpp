/////////////////////////////////////////////////////////////////////////////
// Name:        RmViewProcessSelectionDialog.cpp
// Purpose:     
// Author:      RvH
// Created:     24/06/2006 16:16:14
// $Date: 2009-07-19 19:34:59 +0800 (Sun, 19 Jul 2009) $
// $Revision: 72 $
// Copyright:   ©2002-2008 University of Tasmania
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "RmViewProcessSelectionDialog.h"
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

#include "RmViewProcessSelectionDialog.h"
#include "app/gui/common/IViewProcessSelectionCallback.h"

////@begin XPM images
////@end XPM images

/*!
 * RmViewProcessSelectionDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS(RmViewProcessSelectionDialog, wxDialog)

/*!
 * RmViewProcessSelectionDialog event table definition
 */

    BEGIN_EVENT_TABLE(RmViewProcessSelectionDialog, wxDialog)

    ////@begin RmViewProcessSelectionDialog event table entries
    EVT_CHECKLISTBOX(XRCID("rmID_VIEWPROCESSSELECTION_CHECKLISTBOX"), RmViewProcessSelectionDialog::OnCheckListBoxToggled)

    EVT_BUTTON(XRCID("rmID_VIEWPROCESSSELECTION_BUTTON_SELECTALL"), RmViewProcessSelectionDialog::OnSelectAllClick)

    EVT_BUTTON(XRCID("rmID_VIEWPROCESSSELECTION_BUTTON_SELECTNONE"), RmViewProcessSelectionDialog::OnSelectNoneClick)

    ////@end RmViewProcessSelectionDialog event table entries

    END_EVENT_TABLE()

    /*!
     * RmViewProcessSelectionDialog constructors
     */

    RmViewProcessSelectionDialog::RmViewProcessSelectionDialog()
    : m_checkListBox(__nullptr)
    , m_viewParent(__nullptr)
{
}

RmViewProcessSelectionDialog::RmViewProcessSelectionDialog(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * RmViewProcessSelectionDialog creator
 */

bool RmViewProcessSelectionDialog::Create(wxWindow* parent, wxWindowID /*id*/, const wxString& /*caption*/, const wxPoint& /*pos*/, const wxSize& /*size*/, long /*style*/)
{
    ////@begin RmViewProcessSelectionDialog member initialisation
    //m_viewParent = __nullptr;
    //m_checkListBox = __nullptr;
    ////@end RmViewProcessSelectionDialog member initialisation

    ////@begin RmViewProcessSelectionDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    SetParent(parent);
    CreateControls();
    Centre();
    ////@end RmViewProcessSelectionDialog creation
    return true;
}

/*!
 * Control creation for RmViewProcessSelectionDialog
 */

void RmViewProcessSelectionDialog::CreateControls()
{
    ////@begin RmViewProcessSelectionDialog content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("rmID_VIEWPROCESSSELECTION_DIALOG")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_checkListBox = XRCCTRL(*this, "rmID_VIEWPROCESSSELECTION_CHECKLISTBOX", wxCheckListBox);
    ////@end RmViewProcessSelectionDialog content construction

    // Create custom windows not generated automatically here.
    ////@begin RmViewProcessSelectionDialog content initialisation
    ////@end RmViewProcessSelectionDialog content initialisation
}

void RmViewProcessSelectionDialog::Setup
(rootmap::IViewProcessSelectionCallback* viewParent,
    const wxArrayString& processes,
    const std::vector<rootmap::ProcessIdentifier>& processIds,
    const std::set<rootmap::ProcessIdentifier>& selectedProcessIds)
{
    m_viewParent = viewParent;
    m_processIds.assign(processIds.begin(), processIds.end());

    m_checkListBox->Set(processes);

    //use indices coz thats what the checkbox list uses
    //for ( std::vector<rootmap::ProcessIdentifier>::const_iterator iter(processIds.begin());
    //        iter!=processIds.end() ; ++iter)
    for (size_t item = 0; item < processIds.size(); ++item)
    {
        // Check item based on presence of id in current set
        m_checkListBox->Check(item, (selectedProcessIds.find(processIds[item]) != selectedProcessIds.end()));
    }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for rmID_VIEWPROCESSSELECTION_BUTTON_SELECTALL
 */

void RmViewProcessSelectionDialog::OnSelectAllClick(wxCommandEvent& /*event*/)
{
    for (size_t item = 0; item < m_processIds.size(); ++item)
    {
        m_checkListBox->Check(item, true);
    }
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for rmID_VIEWPROCESSSELECTION_BUTTON_SELECTNONE
 */

void RmViewProcessSelectionDialog::OnSelectNoneClick(wxCommandEvent& /*event*/)
{
    for (size_t item = 0; item < m_processIds.size(); ++item)
    {
        m_checkListBox->Check(item, false);
    }
}

/*!
 * Should we show tooltips?
 */

bool RmViewProcessSelectionDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap RmViewProcessSelectionDialog::GetBitmapResource(const wxString& name)
{
    // Bitmap retrieval
    ////@begin RmViewProcessSelectionDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
    ////@end RmViewProcessSelectionDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon RmViewProcessSelectionDialog::GetIconResource(const wxString& name)
{
    // Icon retrieval
    ////@begin RmViewProcessSelectionDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
    ////@end RmViewProcessSelectionDialog icon retrieval
}


/*!
 * wxEVT_COMMAND_CHECKLISTBOX_TOGGLED event handler for rmID_VIEWPROCESSSELECTION_CHECKLISTBOX
 */

void RmViewProcessSelectionDialog::OnCheckListBoxToggled(wxCommandEvent& event)
{
    // Actually nothing to do here. We don't care about the state of checked
    // items until either "Apply" or "OK" buttons are pressed.
    /*int item = */
    event.GetSelection();
    if (event.IsChecked())
    {
    }

    ////@begin wxEVT_COMMAND_CHECKLISTBOX_TOGGLED event handler for rmID_VIEWPROCESSSELECTION_CHECKLISTBOX in RmViewProcessSelectionDialog.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_COMMAND_CHECKLISTBOX_TOGGLED event handler for rmID_VIEWPROCESSSELECTION_CHECKLISTBOX in RmViewProcessSelectionDialog. 
}

bool RmViewProcessSelectionDialog::TransferDataFromWindow()
{
    TransferToParent();

    return (wxWindowBase::TransferDataFromWindow());
}

void RmViewProcessSelectionDialog::TransferToParent()
{
    std::set<rootmap::ProcessIdentifier> currentProcessIds;
    for (size_t item = 0; item < m_processIds.size(); ++item)
    {
        if (m_checkListBox->IsChecked(item))
        {
            currentProcessIds.insert(m_processIds[item]);
        }
    }
    m_viewParent->UpdateProcesses(currentProcessIds);
}
