/////////////////////////////////////////////////////////////////////////////
// Name:        RmViewProcessSelectionDialog.h
// Purpose:     
// Author:      RvH
// Created:     24/06/2006 16:16:14
// $Date: 2009-07-19 19:34:59 +0800 (Sun, 19 Jul 2009) $
// $Revision: 72 $
// Copyright:   ©2002-2008 University of Tasmania
/////////////////////////////////////////////////////////////////////////////

#ifndef RmViewProcessSelectionDialog_H
#define RmViewProcessSelectionDialog_H

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "RmViewProcessSelectionDialog.h"
#endif

/*!
 * Includes
 */

 ////@begin includes
#include "wx/xrc/xmlres.h"
////@end includes

#include "simulation/common/Types.h"

#include <set>
#include <vector>

/*!
 * Forward declarations
 */

 ////@begin forward declarations
 ////@end forward declarations

namespace rootmap
{
    class IViewProcessSelectionCallback;
}

/*!
 * Control identifiers
 */

 ////@begin control identifiers
#define rmID_VIEWPROCESSSELECTION_DIALOG 10007
#define SYMBOL_RMVIEWPROCESSSELECTIONDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxMAXIMIZE_BOX
#define SYMBOL_RMVIEWPROCESSSELECTIONDIALOG_TITLE _("View Process Selection")
#define SYMBOL_RMVIEWPROCESSSELECTIONDIALOG_IDNAME rmID_VIEWPROCESSSELECTION_DIALOG
#define SYMBOL_RMVIEWPROCESSSELECTIONDIALOG_SIZE wxSize(300, 300)
#define SYMBOL_RMVIEWPROCESSSELECTIONDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

 /*!
  * RmViewProcessSelectionDialog class declaration
  */

class RmViewProcessSelectionDialog : public wxDialog
{
    DECLARE_DYNAMIC_CLASS(RmViewProcessSelectionDialog)
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    RmViewProcessSelectionDialog();
    RmViewProcessSelectionDialog(wxWindow* parent, wxWindowID id = SYMBOL_RMVIEWPROCESSSELECTIONDIALOG_IDNAME, const wxString& caption = SYMBOL_RMVIEWPROCESSSELECTIONDIALOG_TITLE, const wxPoint& pos = SYMBOL_RMVIEWPROCESSSELECTIONDIALOG_POSITION, const wxSize& size = SYMBOL_RMVIEWPROCESSSELECTIONDIALOG_SIZE, long style = SYMBOL_RMVIEWPROCESSSELECTIONDIALOG_STYLE);

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = SYMBOL_RMVIEWPROCESSSELECTIONDIALOG_IDNAME, const wxString& caption = SYMBOL_RMVIEWPROCESSSELECTIONDIALOG_TITLE, const wxPoint& pos = SYMBOL_RMVIEWPROCESSSELECTIONDIALOG_POSITION, const wxSize& size = SYMBOL_RMVIEWPROCESSSELECTIONDIALOG_SIZE, long style = SYMBOL_RMVIEWPROCESSSELECTIONDIALOG_STYLE);

    /// Creates the controls and sizers
    void CreateControls();

    /**
     * Called by either ViewWindow or View3DWindow to setup the dialog list
     *
     * @param viewParent - the window as callback
     * @param processes - the array of strings to populate the list with
     * @param allProcessIds - for tracking all the processes
     * @param currentProcessIds - those processes that should be initially checked
     */
    void Setup(rootmap::IViewProcessSelectionCallback* viewParent,
        const wxArrayString& processes,
        const std::vector<rootmap::ProcessIdentifier>& allProcessIds,
        const std::set<rootmap::ProcessIdentifier>& currentProcessIds);

    ////@begin RmViewProcessSelectionDialog event handler declarations

    /// wxEVT_COMMAND_CHECKLISTBOX_TOGGLED event handler for rmID_VIEWPROCESSSELECTION_CHECKLISTBOX
    void OnCheckListBoxToggled(wxCommandEvent& event);

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for rmID_VIEWPROCESSSELECTION_BUTTON_SELECTALL
    void OnSelectAllClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for rmID_VIEWPROCESSSELECTION_BUTTON_SELECTNONE
    void OnSelectNoneClick(wxCommandEvent& event);

    ////@end RmViewProcessSelectionDialog event handler declarations

    ////@begin RmViewProcessSelectionDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource(const wxString& name);

    /// Retrieves icon resources
    wxIcon GetIconResource(const wxString& name);
    ////@end RmViewProcessSelectionDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

    ////@begin RmViewProcessSelectionDialog member variables
    wxCheckListBox* m_checkListBox;
private:
    rootmap::IViewProcessSelectionCallback* m_viewParent;
    ////@end RmViewProcessSelectionDialog member variables

    bool TransferDataFromWindow();
    void TransferToParent();

    std::vector<rootmap::ProcessIdentifier> m_processIds;
};

#endif // #ifndef RmViewProcessSelectionDialog_H
