/////////////////////////////////////////////////////////////////////////////
// Name:        RmViewWindow.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     24/06/2006 16:35:53
// RCS-ID:      
// Copyright:   ©2006 University of Tasmania
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _RMVIEWWINDOW_H_
#define _RMVIEWWINDOW_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "RmViewWindow.h"
#endif

/*!
 * Includes
 */
#include "app/gui/common/SimulationActivityWindow.h"
#include "app/gui/common/IViewProcessSelectionCallback.h"
#include "app/gui/ViewWindow/RmScrolledWindow.h"
#include "simulation/common/Types.h"

 ////@begin includes
#include "wx/xrc/xmlres.h"
#include "wx/mdi.h"
////@end includes

/*!
 * Forward declarations
 */

 ////@begin forward declarations
class RmScrolledWindow;

////@end forward declarations
//

namespace rootmap
{
    class ViewInformation;
}

/*!
 * Control identifiers
 */

 ////@begin control identifiers
#define rmID_VIEW_WINDOWFRAME 10001
#define SYMBOL_RMVIEWWINDOW_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxFULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN
#define SYMBOL_RMVIEWWINDOW_TITLE _("View")
#define SYMBOL_RMVIEWWINDOW_IDNAME rmID_VIEW_WINDOWFRAME
#define SYMBOL_RMVIEWWINDOW_SIZE wxSize(400, 600)
#define SYMBOL_RMVIEWWINDOW_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

 /*!
  * RmViewWindow class declaration
  */

class RmViewWindow : public wxMDIChildFrame, public rootmap::SimulationActivityWindow, public rootmap::IViewProcessSelectionCallback
{
    DECLARE_CLASS(RmViewWindow)
    DECLARE_EVENT_TABLE()

    RootMapLoggerDeclaration();

public:
    /// Constructors
    RmViewWindow();
    RmViewWindow(wxMDIParentFrame* parent, rootmap::IViewCoordinator* viewCoordinator, rootmap::ScoreboardCoordinator& scoreboardcoordinator, const rootmap::ViewInformation& viewInfo, wxWindowID id = SYMBOL_RMVIEWWINDOW_IDNAME, const wxString& caption = SYMBOL_RMVIEWWINDOW_TITLE, const wxPoint& pos = SYMBOL_RMVIEWWINDOW_POSITION, const wxSize& size = SYMBOL_RMVIEWWINDOW_SIZE, long style = SYMBOL_RMVIEWWINDOW_STYLE);

    ~RmViewWindow();
    bool Create(wxMDIParentFrame* parent, rootmap::IViewCoordinator* viewCoordinator, rootmap::ScoreboardCoordinator& scoreboardcoordinator, const rootmap::ViewInformation& viewInfo, wxWindowID id = SYMBOL_RMVIEWWINDOW_IDNAME, const wxString& caption = SYMBOL_RMVIEWWINDOW_TITLE, const wxPoint& pos = SYMBOL_RMVIEWWINDOW_POSITION, const wxSize& size = SYMBOL_RMVIEWWINDOW_SIZE, long style = SYMBOL_RMVIEWWINDOW_STYLE);

    /// Creates the controls and sizers
    void CreateControls(rootmap::IViewCoordinator* viewCoordinator, rootmap::ScoreboardCoordinator& scoreboardcoordinator, const rootmap::ViewInformation& viewInfo);

    ////@begin RmViewWindow event handler declarations

    /// wxEVT_MENU_OPEN event handler for rmID_VIEW_WINDOWFRAME
    void OnMenuOpen(wxMenuEvent& event);

    /// wxEVT_MENU_CLOSE event handler for rmID_VIEW_WINDOWFRAME
    void OnMenuClose(wxMenuEvent& event);

    /// wxEVT_ERASE_BACKGROUND event handler for rmID_VIEW_WINDOWFRAME
    void OnEraseBackground(wxEraseEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_FRONTVIEWITEM
    void OnViewMenuFrontViewItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_SIDEVIEWITEM
    void OnViewMenuSideViewItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_TOPVIEWITEM
    void OnViewMenuTopViewItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_REPEATITEM
    void OnViewMenuRepeatItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_WRAPITEM
    void OnViewMenuWrapItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_ZOOMINITEM
    void OnViewMenuZoomInItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_ZOOMOUTITEM
    void OnViewMenuZoomOutItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_BOUNDARIESITEM
    void OnViewMenuBoundariesItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_BOXESITEM
    void OnViewMenuBoxesItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_BOXCOLOURSITEM
    void OnViewMenuBoxColoursItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_PROCESSESITEM
    void OnViewMenuProcessesItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_CHARACTERISTICSITEM
    void OnViewMenuCharacteristicsItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_DETAILSITEM
    void OnViewMenuDetailsItemClick(wxCommandEvent& event);

    ////@end RmViewWindow event handler declarations

    ////@begin RmViewWindow member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource(const wxString& name);
    wxBitmap* GetBitmap();

    /// Retrieves icon resources
    wxIcon GetIconResource(const wxString& name);
    ////@end RmViewWindow member function declarations
    /**
     *
     */
    const rootmap::ViewInformation& GetViewInformation() const;

    /**
     *
     */
    void UpdateCharacteristicColourInfo(rootmap::CharacteristicColours_t& cct);

    /// Should we show tooltips?
    static bool ShowToolTips();

    ////@begin RmViewWindow member variables
    RmScrolledWindow* m_scrolledWindow;
    ////@end RmViewWindow member variables

    //wxMDIChildFrame override to activate menus
    virtual void Activate();
    virtual bool Show(bool show);

    /**
     * wxWindow override, to notify the IViewCoordinator that we are going
     */
    bool Destroy();

private:
    rootmap::IViewCoordinator* m_coordinator;
    wxString m_baseTitle;

    /**
     * Sent to each observer by the SimulationEngine after each timestamp
     * is complete.  This is the ideal place for window updates to be
     * triggered.
     */
    virtual void PostTimestampNotification(const rootmap::PostOffice& postOffice);

    virtual void DisconnectNotification();

    /**
     * Called by the View Process Selection dialog to update this window
     */
    virtual void UpdateProcesses(const std::set<rootmap::ProcessIdentifier>& processes);
};

inline const rootmap::ViewInformation& RmViewWindow::GetViewInformation() const
{
    return m_scrolledWindow->GetViewInformation();
}

#endif
// _RMVIEWWINDOW_H_
