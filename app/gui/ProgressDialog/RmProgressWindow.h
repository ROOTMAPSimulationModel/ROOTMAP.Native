/////////////////////////////////////////////////////////////////////////////
// Name:        RmProgressWindow.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     24/06/2006 16:16:14
// RCS-ID:      
// Copyright:   ©2006 University of Tasmania
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _RMPROGRESSWINDOW_H_
#define _RMPROGRESSWINDOW_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "RmProgressWindow.h"
#endif

#include "app/gui/common/SimulationActivityWindow.h"
#include "wx/xrc/xmlres.h"

/*!
 * Forward declarations
 */
namespace rootmap
{
    class GuiSimulationEngine;
    class PostOffice;
    class IProgressCoordinator;
}

/*!
 * Control identifiers
 */

 ////@begin control identifiers
#define rmID_PROGRESS_DIALOG 10021
#define SYMBOL_RMPROGRESSWINDOW_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_RMPROGRESSWINDOW_TITLE _("Progress")
#define SYMBOL_RMPROGRESSWINDOW_IDNAME rmID_PROGRESS_DIALOG
#define SYMBOL_RMPROGRESSWINDOW_SIZE wxSize(300, 120)
#define SYMBOL_RMPROGRESSWINDOW_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

 /*!
  * RmProgressWindow class declaration
  */

class RmProgressWindow : public wxDialog, public rootmap::SimulationActivityWindow
{
    DECLARE_DYNAMIC_CLASS(RmProgressWindow)
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    RmProgressWindow();
    RmProgressWindow(wxWindow* parent, wxWindowID id = SYMBOL_RMPROGRESSWINDOW_IDNAME, const wxString& caption = SYMBOL_RMPROGRESSWINDOW_TITLE, const wxPoint& pos = SYMBOL_RMPROGRESSWINDOW_POSITION, const wxSize& size = SYMBOL_RMPROGRESSWINDOW_SIZE, long style = SYMBOL_RMPROGRESSWINDOW_STYLE);

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = SYMBOL_RMPROGRESSWINDOW_IDNAME, const wxString& caption = SYMBOL_RMPROGRESSWINDOW_TITLE, const wxPoint& pos = SYMBOL_RMPROGRESSWINDOW_POSITION, const wxSize& size = SYMBOL_RMPROGRESSWINDOW_SIZE, long style = SYMBOL_RMPROGRESSWINDOW_STYLE);

    /// Creates the controls and sizers
    void CreateControls();

    ////@begin RmProgressWindow event handler declarations

    /// wxEVT_CLOSE_WINDOW event handler for rmID_PROGRESS_DIALOG
    void OnCloseWindow(wxCloseEvent& event);

    ////@end RmProgressWindow event handler declarations

    ////@begin RmProgressWindow member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource(const wxString& name);

    /// Retrieves icon resources
    wxIcon GetIconResource(const wxString& name);
    ////@end RmProgressWindow member function declarations

    /**
     * Sent to each observer by the SimulationEngine just before the processes
     * are sent the Start message. The simulation is marked as being in
     * "starting" phase at this point
     *
     * @param postOffice reference to the postOffice.
     */
    virtual void PreRunNotification(const rootmap::PostOffice& postOffice);

    /**
     * Sent to each observer by the SimulationEngine before each timestamp
     * is run.
     *
     * @param postOffice reference to the postOffice.
     */
    virtual void PreTimestampNotification(const rootmap::PostOffice& postOffice);

    /**
    * May be sent to each observer by the SimulationEngine after each
    * PostOffice message is processed.
    *
    * @param postOffice reference to the postOffice.
    */
    virtual void MidTimestampNotification(const rootmap::PostOffice& postOffice);

    /**
     * Sent to each observer by the SimulationEngine after each timestamp
     * is complete.  This is the ideal place for window updates to be
     * triggered.
     *
     * Note that "Post" in this sense is not the verb but the prefix,
     * meaning "after" or "later"
     *
     * @param postOffice reference to the postOffice.
     */
    virtual void PostTimestampNotification(const rootmap::PostOffice& postOffice);

    /**
     * Sent to each observer by the SimulationEngine just after the processes
     * are sent the Ending message (but before control is returned to the
     * user). The simulation is in "stopping" phase at this point.
     *
     * @param postOffice reference to the postOffice.
     */
    virtual void PostRunNotification(const rootmap::PostOffice& postOffice);

    virtual void DisconnectNotification();

    /// Should we show tooltips?
    static bool ShowToolTips();

    /**
     * Sets the static fields to initial values
     */
    void Initialise(const rootmap::PostOffice& postOffice, rootmap::IProgressCoordinator* coordinator);

    void UpdateAllStaticText(const rootmap::PostOffice& postOffice);
    void UpdateCurrentTimeStaticText(const rootmap::PostOffice& postOffice);
    void UpdateEndTimeStaticText(const rootmap::PostOffice& postOffice);
    void UpdateDestinationStaticText(const rootmap::PostOffice& postOffice);
    void UpdateProgressGauge(const rootmap::PostOffice& postOffice);

    /**
     * wxWindow override, to notify the IProgressCoordinator that we are going
     */
    bool Destroy();


    ////@begin RmProgressWindow member variables
    wxStaticText* m_currentTimeStaticText;
    wxStaticText* m_endTimeStaticText;
    wxStaticText* m_currentlyProcessingStaticText;
    wxGauge* m_progressGauge;
    ////@end RmProgressWindow member variables
private:
    /**
     *
     */
    rootmap::IProgressCoordinator* m_coordinator;

    /**
     *
     */
     //rootmap::PostOffice * m_postOffice;
};

#endif
// _RMPROGRESSWINDOW_H_
