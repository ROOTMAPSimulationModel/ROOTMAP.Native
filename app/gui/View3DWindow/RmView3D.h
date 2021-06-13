/////////////////////////////////////////////////////////////////////////////
// Name:        RmView3D.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     14/06/2008 22:27:33
// RCS-ID:      
// Copyright:   ©2006 University of Tasmania
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef RmView3D_H
#define RmView3D_H

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "RmView3D.h"
#endif

/*!
 * Includes
 */

#include "app/gui/common/IViewProcessSelectionCallback.h"
#include "app/gui/common/SimulationActivityWindow.h"
#include "core/log/Logger.h"

 ////@begin includes
#include "wx/mdi.h"
#include "wx/glcanvas.h"
////@end includes

/*!
 * Forward declarations
 */

 ////@begin forward declarations
class RmGLCanvas;

////@end forward declarations

namespace rootmap
{
    class IView3DCoordinator;
    class ScoreboardCoordinator;
    class View3DInformation;
}

/*!
 * Control identifiers
 */

 ////@begin control identifiers
#define rmID_VIEW3D_WINDOWFRAME 10011
#define rmID_VIEW_GLCANVAS 10013
#define SYMBOL_RMVIEW3D_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxCLIP_CHILDREN
#define SYMBOL_RMVIEW3D_TITLE _("View 3D")
#define SYMBOL_RMVIEW3D_IDNAME rmID_VIEW3D_WINDOWFRAME
#define SYMBOL_RMVIEW3D_SIZE wxSize(640, 480)
#define SYMBOL_RMVIEW3D_POSITION wxDefaultPosition

////@end control identifiers


/*!
 * RmView3D class declaration
 */

class RmView3D : public wxMDIChildFrame, public rootmap::SimulationActivityWindow, public rootmap::IViewProcessSelectionCallback
{
    DECLARE_CLASS(RmView3D)
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    RmView3D();
    RmView3D(wxMDIParentFrame* parent, rootmap::IView3DCoordinator* viewCoordinator, rootmap::ScoreboardCoordinator& scoreboardcoordinator, const rootmap::View3DInformation& viewInfo, wxWindowID id = SYMBOL_RMVIEW3D_IDNAME, const wxString& caption = SYMBOL_RMVIEW3D_TITLE, const wxPoint& pos = SYMBOL_RMVIEW3D_POSITION, const wxSize& size = SYMBOL_RMVIEW3D_SIZE, long style = SYMBOL_RMVIEW3D_STYLE);

    bool Create(wxMDIParentFrame* parent, rootmap::IView3DCoordinator* viewCoordinator, rootmap::ScoreboardCoordinator& scoreboardcoordinator, const rootmap::View3DInformation& viewInfo, wxWindowID id = SYMBOL_RMVIEW3D_IDNAME, const wxString& caption = SYMBOL_RMVIEW3D_TITLE, const wxPoint& pos = SYMBOL_RMVIEW3D_POSITION, const wxSize& size = SYMBOL_RMVIEW3D_SIZE, long style = SYMBOL_RMVIEW3D_STYLE);

    /// Destructor
    ~RmView3D();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls(rootmap::IView3DCoordinator* viewCoordinator, rootmap::ScoreboardCoordinator& scoreboardcoordinator, const rootmap::View3DInformation& viewInfo);

    ////@begin RmView3D event handler declarations

    /// wxEVT_MENU_OPEN event handler for rmID_VIEW3D_WINDOWFRAME
    void OnMenuOpen(wxMenuEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_FRONTVIEWITEM
    void OnViewMenuFrontViewItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_SIDEVIEWITEM
    void OnViewMenuSideViewItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_TOPVIEWITEM
    void OnViewMenuTopViewItemClick(wxCommandEvent& event);

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

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_DETAILSITEM
    void OnViewMenuDetailsItemClick(wxCommandEvent& event);

    /// wxEVT_MOUSEWHEEL event handler for rmID_VIEW_GLCANVAS
    void OnMouseWheel(wxMouseEvent& event);

    ////@end RmView3D event handler declarations

    ////@begin RmView3D member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource(const wxString& name);

    wxBitmap* GetBitmap();

    /// Retrieves icon resources
    wxIcon GetIconResource(const wxString& name);
    ////@end RmView3D member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

    /**
     * wxWindow override, to notify the IView3DCoordinator that we are going
     */
    bool Destroy();

    ////@begin RmView3D member variables
    RmGLCanvas* m_glCanvas;
    ////@end RmView3D member variables

private:
    RootMapLoggerDeclaration();

    rootmap::IView3DCoordinator* m_coordinator;

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

#endif // #ifndef RmView3D_H
