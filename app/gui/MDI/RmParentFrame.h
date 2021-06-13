/////////////////////////////////////////////////////////////////////////////
// Name:        RmParentFrame.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     24/06/2006 16:16:14
// RCS-ID:      
// Copyright:   ©2006 University of Tasmania
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _RMPARENTFRAME_H_
#define _RMPARENTFRAME_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "RmParentFrame.h"
#endif

/*!
 * Includes
 */

 ////@begin includes
#include "wx/mdi.h"

////@end includes

/*!
 * Forward declarations
 */

 ////@begin forward declarations
namespace rootmap
{
    class GuiSimulationEngine;
    class TXDataAccessManager;
} /* namespace rootmap */
////@end forward declarations

/*!
 * Control identifiers
 */

 ////@begin control identifiers
#define rmID_PARENT_WINDOWFRAME 10000
#define SYMBOL_RMPARENTFRAME_STYLE wxDEFAULT_FRAME_STYLE|wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxCLOSE_BOX|wxNO_BORDER
#define SYMBOL_RMPARENTFRAME_TITLE _("RootMap")
#define SYMBOL_RMPARENTFRAME_IDNAME rmID_PARENT_WINDOWFRAME
#define SYMBOL_RMPARENTFRAME_SIZE wxSize(1000, 700)
#define SYMBOL_RMPARENTFRAME_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

 /*!
  * RmParentFrame class declaration
  */

class RmParentFrame : public wxMDIParentFrame
{
    DECLARE_CLASS(RmParentFrame)
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    RmParentFrame();
    RmParentFrame(wxWindow* parent, wxWindowID id = SYMBOL_RMPARENTFRAME_IDNAME, const wxString& caption = SYMBOL_RMPARENTFRAME_TITLE, const wxPoint& pos = SYMBOL_RMPARENTFRAME_POSITION, const wxSize& size = SYMBOL_RMPARENTFRAME_SIZE, long style = SYMBOL_RMPARENTFRAME_STYLE);
    ~RmParentFrame();

    bool Create(wxWindow* parent, wxWindowID id = SYMBOL_RMPARENTFRAME_IDNAME, const wxString& caption = SYMBOL_RMPARENTFRAME_TITLE, const wxPoint& pos = SYMBOL_RMPARENTFRAME_POSITION, const wxSize& size = SYMBOL_RMPARENTFRAME_SIZE, long style = SYMBOL_RMPARENTFRAME_STYLE);

    /// Creates the controls and sizers
    void CreateControls();

    ////@begin RmParentFrame event handler declarations

    /// wxEVT_MENU_OPEN event handler for rmID_PARENT_WINDOWFRAME
    void OnMenuOpen(wxMenuEvent& event);

    /// wxEVT_MENU_CLOSE event handler for rmID_PARENT_WINDOWFRAME
    void OnMenuClose(wxMenuEvent& event);

    /// wxEVT_CHAR event handler for rmID_PARENT_WINDOWFRAME
    void OnChar(wxKeyEvent& event);

    /// wxEVT_CLOSE event handler for rmID_PARENT_WINDOWFRAME
    void OnClose(wxCloseEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_NEW
    void OnNewClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_OPEN
    void OnOpenClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_CLOSE_ALL
    bool TryCloseSimulation();
    void OnCloseAllClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_CLOSE
    void OnCloseClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_SAVE
    void OnSaveClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_SAVEAS
    void OnSaveasClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_EXIT
    void OnExitClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_UNDO
    void OnUndoClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_REDO
    void OnRedoClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_COPY
    void OnCopyClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_CUT
    void OnCutClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_PASTE
    void OnPasteClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_EDITMENU_PASTESPECIALITEM
    void OnEditMenuPasteSpecialItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_SELECTALL
    void OnSelectallClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_RUNMENU_GOITEM
    void OnRunMenuGoItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_RUNMENU_STOPITEM
    void OnRunMenuStopItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_RUNMENU_PAUSEITEM
    void OnRunMenuPauseItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_RUNMENU_SHOWPROGRESS
    void OnRunMenuShowProgress(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_RUNMENU_SETSTARTTIMEITEM
    void OnRunMenuSetStartTimeItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_NEWVIEWITEM
    void OnViewMenuNewViewItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_NEW3DVIEWITEM
    void OnViewMenuNew3DViewItemClick(wxCommandEvent& event);

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

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_SCOREBOARDSUBMENU_AIRITEM
    void OnViewMenuScoreboardSubmenuAirItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_SCOREBOARDSUBMENU_LITTERITEM
    void OnViewMenuScoreboardSubmenuLitterItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_SCOREBOARDSUBMENU_SOILITEM
    void OnViewMenuScoreboardSubmenuSoilItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_SCOREBOARDSUBMENU_DRAINITEM
    void OnViewMenuScoreboardSubmenuDrainItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_SCOREBOARDSUBMENU_ALLITEM
    void OnViewMenuScoreboardSubmenuAllItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_DETAILSITEM
    void OnViewMenuDetailsItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_NEWTABLESUBMENU_AIRITEM
    void OnTableMenuNewTableSubmenuAirItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_NEWTABLESUBMENU_LITTERITEM
    void OnTableMenuNewTableSubmenuLitterItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_NEWTABLESUBMENU_SOILITEM
    void OnTableMenuNewTableSubmenuSoilItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_NEWTABLESUBMENU_DRAINITEM
    void OnTableMenuNewTableSubmenuDrainItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_PREVIOUSLAYERITEM
    void OnTableMenuPreviousLayerItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_NEXTLAYERITEM
    void OnTableMenuNextLayerItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_FROMFRONTITEM
    void OnTableMenuFromFrontItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_FROMSIDEITEM
    void OnTableMenuFromSideItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_FROMTOPITEM
    void OnTableMenuFromTopItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_CHARACTERISTICITEM
    void OnTableMenuCharacteristicItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_DETAILSITEM
    void OnTableMenuDetailsItemClick(wxCommandEvent& event);

    // MSA 10.01.21 wxEVT_COMMAND_MENU_SELECTED event handler for wxID_ABOUT
    void OnHelpMenuAboutItemClick(wxCommandEvent& event);


    ////@end RmParentFrame event handler declarations

    ////@begin RmParentFrame member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource(const wxString& name);

    /// Retrieves icon resources
    wxIcon GetIconResource(const wxString& name);
    ////@end RmParentFrame member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();


    /**
     * Dynamically added event handler for Table windows
     */
    void OnLayerSubMenuItem(wxCommandEvent& event);


    ////@begin RmParentFrame member variables
private:
    rootmap::GuiSimulationEngine* m_engine;
    rootmap::TXDataAccessManager* m_tx_dam;
    ////@end RmParentFrame member variables

    void DeactivateTableMenu(wxMenuBar* menuBar);
    void DeactivateViewMenu(wxMenuBar* menuBar);

    // Helper function for actually creating the simulation.
    void RunSimulation();

public:
    void DoCloseSimulation();
};

#endif
// _RMPARENTFRAME_H_
