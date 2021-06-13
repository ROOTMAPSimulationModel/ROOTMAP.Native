/////////////////////////////////////////////////////////////////////////////
// Name:        RmTableWindow.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     24/06/2006 20:16:57
// RCS-ID:      
// Copyright:   �2006 University of Tasmania
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _RMTABLEWINDOW_H_
#define _RMTABLEWINDOW_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "RmTableWindow.h"
#endif

/*!
 * Includes
 */

 ////@begin includes
#include "wx/xrc/xmlres.h"
#include "wx/mdi.h"
#include "wx/grid.h"
////@end includes

#include "app/gui/common/SimulationActivityWindow.h"

#include "core/common/Structures.h"
#include "core/log/Logger.h"

#include <vector>

/*!
 * Forward declarations
 */

 ////@begin forward declarations
class wxGrid;

////@end forward declarations

namespace rootmap
{
    class Scoreboard;
    class ScoreboardStratum;
    class ScoreboardCoordinator;
    class ScoreboardTableHelper;

    class LayerMenuHelper;
    class TableInformation;
    class ITableCoordinator;
} /* namespace rootmap */


/*!
 * Control identifiers
 */

 ////@begin control identifiers
#define rmID_TABLE_WINDOWFRAME 10014
#define SYMBOL_RMTABLEWINDOW_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL
#define SYMBOL_RMTABLEWINDOW_TITLE _("Table")
#define SYMBOL_RMTABLEWINDOW_IDNAME rmID_TABLE_WINDOWFRAME
#define SYMBOL_RMTABLEWINDOW_SIZE wxSize(399, 299)
#define SYMBOL_RMTABLEWINDOW_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

 /*!
  * RmTableWindow class declaration
  */

class RmTableWindow
    : public wxMDIChildFrame
    , public rootmap::SimulationActivityWindow
{
    DECLARE_CLASS(RmTableWindow)
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
#if defined RMTABLEWINDOW_DEFAULT_CONSTRUCTOR
    RmTableWindow();
#endif // #if defined RMTABLEWINDOW_DEFAULT_CONSTRUCTOR
    RmTableWindow(wxMDIParentFrame* parent,
        rootmap::ITableCoordinator* tableCoordinator,
        rootmap::ScoreboardCoordinator& scoreboardcoordinator,
        const rootmap::TableInformation& tableInfo,
        wxWindowID id = SYMBOL_RMTABLEWINDOW_IDNAME,
        const wxString& caption = SYMBOL_RMTABLEWINDOW_TITLE,
        const wxPoint& pos = SYMBOL_RMTABLEWINDOW_POSITION,
        const wxSize& size = SYMBOL_RMTABLEWINDOW_SIZE,
        long style = SYMBOL_RMTABLEWINDOW_STYLE);

    virtual ~RmTableWindow();

    bool Create(wxMDIParentFrame* parent,
        rootmap::ITableCoordinator* tableCoordinator,
        rootmap::ScoreboardCoordinator& scoreboardcoordinator,
        const rootmap::TableInformation& tableInfo,
        wxWindowID id = SYMBOL_RMTABLEWINDOW_IDNAME,
        const wxString& caption = SYMBOL_RMTABLEWINDOW_TITLE,
        const wxPoint& pos = SYMBOL_RMTABLEWINDOW_POSITION,
        const wxSize& size = SYMBOL_RMTABLEWINDOW_SIZE,
        long style = SYMBOL_RMTABLEWINDOW_STYLE);

    void CreateHelper(rootmap::ScoreboardCoordinator& scoreboardcoordinator, const rootmap::TableInformation& tableInfo);

    /// Creates the controls and sizers
    void CreateControls();

    /// This is how we instruct the table to populate initially
    const rootmap::TableInformation& GetTableInformation() const;

    /**
     *
     */
    void UpdateLayerStrings();
    void UpdateLayerMenu();
    void ReCheckViewDirectionTableMenuItem(rootmap::ViewDirection viewDir);
    void UpdateColumnLabelStaticText();
    void UpdateRowLabelStaticBitmap();
    void UpdateColumnLabels();
    void UpdateRowLabels();

    /**
     * wxMDIChildFrame override, sets up the Layer submenu
     */
    void Activate();
    void Raise();
    bool Show(bool show);

    /**
     * wxWindow override, to notify the ITableCoordinator that we are going
     */
    bool Destroy();

    /**
     * Thus named to avoid any possibility of confusion with a wx function name
     */
    wxMenuBar* GetMDIParentMenuBar();

    ////@begin RmTableWindow event handler declarations

    /// wxEVT_MENU_OPEN event handler for rmID_TABLE_WINDOWFRAME
    void OnMenuOpen(wxMenuEvent& event);

    /// wxEVT_MENU_CLOSE event handler for rmID_TABLE_WINDOWFRAME
    void OnMenuClose(wxMenuEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_NEXTLAYERITEM
    void OnTableMenuNextLayerItemClick(wxCommandEvent& event);

    /// wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_PREVIOUSLAYERITEM
    void OnTableMenuPreviousLayerItemClick(wxCommandEvent& event);

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

    /// wxEVT_CONTEXT_MENU event handler for rmID_TABLE_CORNERPANEL
    void OnContextMenu(wxContextMenuEvent& event);

    /// wxEVT_GRID_CELL_CHANGE event handler for rmID_TABLE_GRID
    void OnCellChanged(wxGridEvent& event);

    /// wxEVT_GRID_SELECT_CELL event handler for rmID_TABLE_GRID
    void OnSelectCell(wxGridEvent& event);

    /// wxEVT_GRID_EDITOR_HIDDEN event handler for rmID_TABLE_GRID
    void OnEditorHidden(wxGridEvent& event);

    /// wxEVT_GRID_EDITOR_SHOWN event handler for rmID_TABLE_GRID
    void OnEditorShown(wxGridEvent& event);

    /// wxEVT_GRID_COL_SIZE event handler for rmID_TABLE_GRID
    void OnColSize(wxGridSizeEvent& event);

    /// wxEVT_GRID_ROW_SIZE event handler for rmID_TABLE_GRID
    void OnRowSize(wxGridSizeEvent& event);

    /// wxEVT_GRID_RANGE_SELECT event handler for rmID_TABLE_GRID
    void OnRangeSelect(wxGridRangeSelectEvent& event);

    /// wxEVT_GRID_EDITOR_CREATED event handler for rmID_TABLE_GRID
    void OnEditorCreated(wxGridEditorCreatedEvent& event);

    /// wxEVT_SIZE event handler for rmID_TABLE_GRID
    void OnSize(wxSizeEvent& event);

    /// wxEVT_IDLE event handler for rmID_TABLE_GRID
    void OnIdle(wxIdleEvent& event);

    /// wxEVT_CHAR event handler for rmID_TABLE_GRID
    void OnChar(wxKeyEvent& event);

    ////@end RmTableWindow event handler declarations

    ////@begin RmTableWindow member function declarations
        /// Retrieves bitmap resources
    wxBitmap GetBitmapResource(const wxString& name);

    /// Retrieves icon resources
    wxIcon GetIconResource(const wxString& name);
    ////@end RmTableWindow member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

    ////@begin RmTableWindow member variables
    wxStaticText* m_columnLabelStaticText;
    wxStaticBitmap* m_rowLabelStaticBitmap;
    wxGrid* m_scoreboardGrid;
    ////@end RmTableWindow member variables

    /**
     * Dynamically adjusted event handler
     */
    void OnLayerSubMenuItem(wxCommandEvent& event);

    /**
     *
     */
    void OnActivate(wxActivateEvent& event);

private:
    RootMapLoggerDeclaration();

    /**
     * Sent to each observer by the SimulationEngine after each timestamp
     * is complete.  This is the ideal place for window updates to be
     * triggered.
     */
    virtual void PostTimestampNotification(const rootmap::PostOffice& postOffice);

    virtual void DisconnectNotification();

    ///
    /// Would prefer this to be a reference but cannot set it in the default
    /// constructor required by wx xrc
    rootmap::ITableCoordinator* m_coordinator;

    ///
    /// also prefer this to be a reference
    rootmap::ScoreboardTableHelper* m_tableHelper;

    /**
     *
     */
    rootmap::LayerMenuHelper* m_layerHelper;
};

#endif
// _RMTABLEWINDOW_H_
