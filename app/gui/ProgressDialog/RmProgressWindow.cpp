/////////////////////////////////////////////////////////////////////////////
// Name:        RmProgressWindow.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     24/06/2006 16:16:14
// RCS-ID:      
// Copyright:   ©2006 University of Tasmania
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "RmProgressWindow.h"
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
#include "simulation/process/interprocess_communication/PostOffice.h"
#include "app/gui/common/IProgressCoordinator.h"

#include "RmProgressWindow.h"

////@begin XPM images
////@end XPM images

/*!
 * RmProgressWindow type definition
 */

IMPLEMENT_DYNAMIC_CLASS(RmProgressWindow, wxDialog)

/*!
 * RmProgressWindow event table definition
 */

    BEGIN_EVENT_TABLE(RmProgressWindow, wxDialog)

    ////@begin RmProgressWindow event table entries
    EVT_CLOSE(RmProgressWindow::OnCloseWindow)

    ////@end RmProgressWindow event table entries

    END_EVENT_TABLE()

    /*!
     * RmProgressWindow constructors
     */

    RmProgressWindow::RmProgressWindow()
    : m_coordinator(__nullptr)
    , m_currentTimeStaticText(__nullptr)
    , m_endTimeStaticText(__nullptr)
    , m_currentlyProcessingStaticText(__nullptr)
    , m_progressGauge(__nullptr)
{
}

RmProgressWindow::RmProgressWindow(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
    : m_coordinator(__nullptr)
    , m_currentTimeStaticText(__nullptr)
    , m_endTimeStaticText(__nullptr)
    , m_currentlyProcessingStaticText(__nullptr)
    , m_progressGauge(__nullptr)
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * RmProgressWindow creator
 */

bool RmProgressWindow::Create(wxWindow* parent, wxWindowID /* id */, const wxString& /* caption */, const wxPoint& /* pos */, const wxSize& /* size */, long /* style */)
{
    ////@begin RmProgressWindow member initialisation
    // MSA 09.11.26 Now done in constructor initialisation list
    /*
    m_currentTimeStaticText = __nullptr;
    m_endTimeStaticText = __nullptr;
    m_currentlyProcessingStaticText = __nullptr;
    m_progressGauge = __nullptr;
    */
    ////@end RmProgressWindow member initialisation

    ////@begin RmProgressWindow creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    SetParent(parent);
    CreateControls();
    Centre();
    ////@end RmProgressWindow creation
    return true;
}

/*!
 * Control creation for RmProgressWindow
 */

void RmProgressWindow::CreateControls()
{
    ////@begin RmProgressWindow content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("rmID_PROGRESS_DIALOG")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_currentTimeStaticText = XRCCTRL(*this, "rmID_PROGRESS_CURRENTTIMEVALUE_STATIC", wxStaticText);
    m_endTimeStaticText = XRCCTRL(*this, "rmID_PROGRESS_ENDTIMEVALUE_STATIC", wxStaticText);
    m_currentlyProcessingStaticText = XRCCTRL(*this, "rmID_PROGRESS_CURRENTLYPROCESSINGVALUE_STATIC", wxStaticText);
    m_progressGauge = XRCCTRL(*this, "rmID_PROGRESS_GAUGE", wxGauge);
    ////@end RmProgressWindow content construction

    // Create custom windows not generated automatically here.
    ////@begin RmProgressWindow content initialisation
    ////@end RmProgressWindow content initialisation
}

/*!
 * Should we show tooltips?
 */

bool RmProgressWindow::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap RmProgressWindow::GetBitmapResource(const wxString& name)
{
    // Bitmap retrieval
    ////@begin RmProgressWindow bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
    ////@end RmProgressWindow bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon RmProgressWindow::GetIconResource(const wxString& name)
{
    // Icon retrieval
    ////@begin RmProgressWindow icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
    ////@end RmProgressWindow icon retrieval
}

void RmProgressWindow::PreRunNotification(const rootmap::PostOffice& postOffice)
{
    UpdateAllStaticText(postOffice);
    UpdateProgressGauge(postOffice);
}

void RmProgressWindow::PreTimestampNotification(const rootmap::PostOffice& postOffice)
{
    UpdateAllStaticText(postOffice);
}

void RmProgressWindow::MidTimestampNotification(const rootmap::PostOffice& postOffice)
{
    UpdateAllStaticText(postOffice);
}

void RmProgressWindow::PostTimestampNotification(const rootmap::PostOffice& postOffice)
{
    UpdateProgressGauge(postOffice);
}

void RmProgressWindow::PostRunNotification(const rootmap::PostOffice& postOffice)
{
    UpdateAllStaticText(postOffice);
    UpdateProgressGauge(postOffice);
}

void RmProgressWindow::DisconnectNotification()
{
    // No simulation-class members that need to be disconnected
}

void RmProgressWindow::Initialise(const rootmap::PostOffice& postOffice, rootmap::IProgressCoordinator* coordinator)
{
    m_coordinator = coordinator;
    UpdateAllStaticText(postOffice);
    UpdateProgressGauge(postOffice);
}

void RmProgressWindow::UpdateAllStaticText(const rootmap::PostOffice& postOffice)
{
    UpdateCurrentTimeStaticText(postOffice);
    UpdateEndTimeStaticText(postOffice);
    UpdateDestinationStaticText(postOffice);
}

void RmProgressWindow::UpdateCurrentTimeStaticText(const rootmap::PostOffice& postOffice)
{
    wxDateTime currentTime(postOffice.GetNow());
    wxString currentString = currentTime.Format("%Y/%m/%d %H:%M:%S");
    m_currentTimeStaticText->SetLabel(currentString);
}

void RmProgressWindow::UpdateEndTimeStaticText(const rootmap::PostOffice& postOffice)
{
    rootmap::ProcessTime_t end_time_t = postOffice.GetNearestEnd();
    rootmap::ProcessTime_t start_time_t = postOffice.GetStart();

    wxString endString;
    if (end_time_t == start_time_t)
    {
        wxDateTime endTime(postOffice.GetDefaultRunTime() + start_time_t);
        endString = endTime.Format("%Y/%m/%d %H:%M:%S");
    }
    else
    {
        wxDateTime endTime(end_time_t);
        endString = endTime.Format("%Y/%m/%d %H:%M:%S");
    }

    m_endTimeStaticText->SetLabel(endString);
}

void RmProgressWindow::UpdateDestinationStaticText(const rootmap::PostOffice& postOffice)
{
    rootmap::ProcessTime_t end_time_t = postOffice.GetNearestEnd();
    rootmap::ProcessTime_t current_time_t = postOffice.GetNow();
    rootmap::ProcessTime_t start_time_t = postOffice.GetStart();

    if ((end_time_t == current_time_t) || (start_time_t == current_time_t))
    {
        m_currentlyProcessingStaticText->SetLabel("Not Running");
    }
    else
    {
        std::string nextDst;
        postOffice.GetNextDestinationName(nextDst);
        m_currentlyProcessingStaticText->SetLabel(nextDst.c_str());
    }
}

void RmProgressWindow::UpdateProgressGauge(const rootmap::PostOffice& postOffice)
{
    rootmap::ProcessTime_t end_time_t = postOffice.GetNearestEnd();
    rootmap::ProcessTime_t current_time_t = postOffice.GetNow();
    rootmap::ProcessTime_t start_time_t = postOffice.GetStart();

    double end_start_diff = end_time_t - start_time_t;
    if (0 == end_start_diff)
    {
        // no good trying to div by zero...
        m_progressGauge->SetValue(0);
    }
    else
    {
        double progress = ((current_time_t - start_time_t * 1.0) / (end_start_diff));
        int progress_ = m_progressGauge->GetRange() * progress;
        m_progressGauge->SetValue(progress_);
    }
}

bool RmProgressWindow::Destroy()
{
    // we are destroyed before we can call removeTableWindow, so that we need
    // to save a copy of the value of m_coordinator to use.
    rootmap::IProgressCoordinator* old_coordinator = m_coordinator;
    RmProgressWindow* old_this = this;

    bool ok = wxWindow::Destroy();
    if (ok)
    {
        old_coordinator->removeProgressWindow(old_this);
    }

    return ok;
}


/*!
 * wxEVT_CLOSE_WINDOW event handler for rmID_PROGRESS_DIALOG
 */

void RmProgressWindow::OnCloseWindow(wxCloseEvent& event)
{
    m_coordinator->removeProgressWindow(this);

    event.Skip();
}
