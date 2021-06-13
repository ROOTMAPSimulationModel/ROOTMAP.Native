/////////////////////////////////////////////////////////////////////////////
// Name:        RmTableWindow.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     24/06/2006 20:16:57
// RCS-ID:      
// Copyright:   ©2006 University of Tasmania
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "RmTableWindow.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/image.h"

////@begin includes
////@end includes

#include "app/common/Types.h"
#include "app/gui/common/GuiIdentifierUtility.h"
#include "app/gui/common/ITableCoordinator.h"
#include "app/gui/TableWindow/RmTableWindow.h"
#include "app/gui/TableWindow/LayerMenuHelper.h"
#include "app/gui/TableWindow/ScoreboardTableHelper.h"
#include "app/gui/TableWindow/RmSelectTableCharacteristic.h"

#include "simulation/common/SimulationEngine.h"
#include "simulation/common/SprintUtility.h"
#include "simulation/process/common/Process.h"
#include "simulation/scoreboard/ScoreboardCoordinator.h"
#include "simulation/scoreboard/Scoreboard.h"

#include "core/macos_compatibility/MacResourceManager.h"
#include "core/scoreboard/ScoreboardUtility.h"
#include "core/scoreboard/ScoreboardStratum.h"
#include "core/common/Structures.h"
#include "core/common/RmAssert.h"
#include "core/utility/Utility.h"

using rootmap::TableInformation;

////@begin XPM images
////@end XPM images

/*!
 * RmTableWindow type definition
 */

IMPLEMENT_CLASS(RmTableWindow, wxMDIChildFrame)

/*!
 * RmTableWindow event table definition
 */

    BEGIN_EVENT_TABLE(RmTableWindow, wxMDIChildFrame)

    ////@begin RmTableWindow event table entries
    EVT_MENU_OPEN(RmTableWindow::OnMenuOpen)
    EVT_MENU_CLOSE(RmTableWindow::OnMenuClose)

    EVT_MENU(XRCID("rmID_TABLEMENU_NEXTLAYERITEM"), RmTableWindow::OnTableMenuNextLayerItemClick)

    EVT_MENU(XRCID("rmID_TABLEMENU_PREVIOUSLAYERITEM"), RmTableWindow::OnTableMenuPreviousLayerItemClick)

    EVT_MENU(XRCID("rmID_TABLEMENU_FROMFRONTITEM"), RmTableWindow::OnTableMenuFromFrontItemClick)

    EVT_MENU(XRCID("rmID_TABLEMENU_FROMSIDEITEM"), RmTableWindow::OnTableMenuFromSideItemClick)

    EVT_MENU(XRCID("rmID_TABLEMENU_FROMTOPITEM"), RmTableWindow::OnTableMenuFromTopItemClick)

    EVT_MENU(XRCID("rmID_TABLEMENU_CHARACTERISTICITEM"), RmTableWindow::OnTableMenuCharacteristicItemClick)

    EVT_MENU(XRCID("rmID_TABLEMENU_DETAILSITEM"), RmTableWindow::OnTableMenuDetailsItemClick)

    EVT_GRID_CELL_CHANGED(RmTableWindow::OnCellChanged)
    EVT_GRID_SELECT_CELL(RmTableWindow::OnSelectCell)
    EVT_GRID_EDITOR_HIDDEN(RmTableWindow::OnEditorHidden)
    EVT_GRID_EDITOR_SHOWN(RmTableWindow::OnEditorShown)
    EVT_GRID_COL_SIZE(RmTableWindow::OnColSize)
    EVT_GRID_ROW_SIZE(RmTableWindow::OnRowSize)
    EVT_GRID_RANGE_SELECT(RmTableWindow::OnRangeSelect)
    EVT_GRID_EDITOR_CREATED(RmTableWindow::OnEditorCreated)

    ////@end RmTableWindow event table entries

    END_EVENT_TABLE()

    using rootmap::Scoreboard;
using rootmap::ScoreboardCoordinator;
using rootmap::ITableCoordinator;
using rootmap::Dimension;
using rootmap::GuiIdentifierUtility;

RootMapLoggerDefinition(RmTableWindow);

/*!
 * RmTableWindow constructors
 */
#if defined RMTABLEWINDOW_DEFAULT_CONSTRUCTOR
RmTableWindow::RmTableWindow()
    : m_coordinator(__nullptr)
    , m_tableHelper(__nullptr)
    , m_columnLabelStaticText(__nullptr)
    , m_rowLabelStaticBitmap(__nullptr)
    , m_scoreboardGrid(__nullptr)
{
    RootMapLoggerInitialisation("rootmap.RmTableWindow");
}
#endif // #if defined RMTABLEWINDOW_DEFAULT_CONSTRUCTOR

RmTableWindow::RmTableWindow
(wxMDIParentFrame* parent,
    ITableCoordinator* tableCoordinator,
    ScoreboardCoordinator& scoreboardcoordinator,
    const rootmap::TableInformation& tableInfo,
    wxWindowID id,
    const wxString& caption,
    const wxPoint& pos,
    const wxSize& size,
    long style)
    : m_tableHelper(__nullptr)
    , m_columnLabelStaticText(__nullptr)
    , m_rowLabelStaticBitmap(__nullptr)
    , m_scoreboardGrid(__nullptr)
{
    RootMapLoggerInitialisation("rootmap.RmTableWindow");
    Create(parent, tableCoordinator, scoreboardcoordinator, tableInfo, id, caption, pos, size, style);
}

RmTableWindow::~RmTableWindow()
{
    int min_id, max_id;
    GuiIdentifierUtility::instance().getLayerMenuItemIdentifierRange(min_id, max_id);
    Disconnect(min_id, max_id,
        wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(RmTableWindow::OnLayerSubMenuItem));

    Disconnect(wxEVT_ACTIVATE,
        wxActivateEventHandler(RmTableWindow::OnActivate));

    // MSA 09.12.07 m_layerHelper's destruction isn't handled by wxWidgets (m_tableHelper is, though.)
    delete m_layerHelper;
}

/*!
 * rmTableWindow creator
 */

bool RmTableWindow::Create(
    wxMDIParentFrame* parent,
    ITableCoordinator* tableCoordinator,
    ScoreboardCoordinator& scoreboardcoordinator,
    const rootmap::TableInformation& tableInfo,
    wxWindowID /*id*/,
    const wxString& /*caption*/,
    const wxPoint& /*pos*/,
    const wxSize& /*size*/,
    long /*style*/)
{
    m_coordinator = tableCoordinator;
    CreateHelper(scoreboardcoordinator, tableInfo);
    ////@begin RmTableWindow creation
    SetParent(parent);
    CreateControls();
    ////@end RmTableWindow creation

    int min_id, max_id;
    GuiIdentifierUtility::instance().getLayerMenuItemIdentifierRange(min_id, max_id);
    Connect(min_id, max_id,
        wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(RmTableWindow::OnLayerSubMenuItem));

    Connect(wxEVT_ACTIVATE,
        wxActivateEventHandler(RmTableWindow::OnActivate));

    return true;
}


void RmTableWindow::CreateHelper
(ScoreboardCoordinator& scoreboardcoordinator,
    const rootmap::TableInformation& tableInfo)
{
    m_tableHelper = new rootmap::ScoreboardTableHelper(scoreboardcoordinator, tableInfo);
    RmAssert(m_tableHelper, "Scoreboard Table Window Helper not created!");

    m_layerHelper = new rootmap::LayerMenuHelper(*this, *m_tableHelper);
    RmAssert(m_layerHelper, "Table Window Layer Helper not created!");

    m_layerHelper->RebuildLayerMenu();
}


/*!
 * Control creation for rmTableWindow
 */

void RmTableWindow::CreateControls()
{
    RmAssert(m_tableHelper != 0, "Scoreboard Table Helper is not created");

    ////@begin RmTableWindow content construction
    if (!wxXmlResource::Get()->LoadObject(this, GetParent(), wxT("rmID_TABLE_WINDOWFRAME"), wxT("wxMDIChildFrame")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_columnLabelStaticText = XRCCTRL(*this, "rmID_TABLE_COLUMNLABEL_STATICTEXT", wxStaticText);
    m_rowLabelStaticBitmap = XRCCTRL(*this, "rmID_TABLE_ROWLABEL_STATICBITMAP", wxStaticBitmap);
    m_scoreboardGrid = XRCCTRL(*this, "rmID_TABLE_GRID", wxGrid);
    // Connect events and objects
    FindWindow(XRCID("rmID_TABLE_CORNERPANEL"))->Connect(XRCID("rmID_TABLE_CORNERPANEL"), wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(RmTableWindow::OnContextMenu), __nullptr, this);
    FindWindow(XRCID("rmID_TABLE_GRID"))->Connect(XRCID("rmID_TABLE_GRID"), wxEVT_SIZE, wxSizeEventHandler(RmTableWindow::OnSize), __nullptr, this);
    FindWindow(XRCID("rmID_TABLE_GRID"))->Connect(XRCID("rmID_TABLE_GRID"), wxEVT_IDLE, wxIdleEventHandler(RmTableWindow::OnIdle), __nullptr, this);
    FindWindow(XRCID("rmID_TABLE_GRID"))->Connect(XRCID("rmID_TABLE_GRID"), wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(RmTableWindow::OnContextMenu), __nullptr, this);
    FindWindow(XRCID("rmID_TABLE_GRID"))->Connect(XRCID("rmID_TABLE_GRID"), wxEVT_CHAR, wxKeyEventHandler(RmTableWindow::OnChar), __nullptr, this);
    ////@end RmTableWindow content construction

    // Create custom windows not generated automatically here.

    // set the defaults for cell sizes for data and labels
    m_scoreboardGrid->SetDefaultColSize(80);
    m_scoreboardGrid->SetDefaultRowSize(20);
    m_scoreboardGrid->SetRowLabelSize(80);
    m_scoreboardGrid->SetColLabelSize(20);

    // un-bold the label font
    wxFont label_font = m_scoreboardGrid->GetLabelFont();
    label_font.SetWeight(wxFONTWEIGHT_NORMAL);
    label_font.SetFamily(wxFONTFAMILY_SWISS);
    m_scoreboardGrid->SetLabelFont(label_font);

    // set editor and renderer to use floating values
    m_scoreboardGrid->SetDefaultEditor(new wxGridCellFloatEditor(10, 7));
    m_scoreboardGrid->SetDefaultRenderer(new wxGridCellFloatRenderer(10, 7));

    m_scoreboardGrid->SetTable(m_tableHelper, true); // MSA 09.12.07 Must take ownership of m_tableHelper here, so the wx destruction code calls its destructor correctly


    ////@begin RmTableWindow content initialisation
    ////@end RmTableWindow content initialisation
}


const rootmap::TableInformation& RmTableWindow::GetTableInformation() const
{
    return m_tableHelper->GetTableInformation();
}

void RmTableWindow::ReCheckViewDirectionTableMenuItem(rootmap::ViewDirection viewDir)
{
    wxMenuBar* menu_bar = GetMDIParentMenuBar();
    wxMenu* table_menu = menu_bar->GetMenu(rootmap::RootMapMenuIndex_Table);

    switch (viewDir)
    {
    case rootmap::vFront:
        table_menu->Check(rootmap::TableMenuItemIndex_Front, true);
        //table_menu->Check(rootmap::TableMenuItemIndex_Side,false);
        //table_menu->Check(rootmap::TableMenuItemIndex_Top,false);
        break;
    case rootmap::vSide:
        //table_menu->Check(rootmap::TableMenuItemIndex_Front,false);
        table_menu->Check(rootmap::TableMenuItemIndex_Side, true);
        //table_menu->Check(rootmap::TableMenuItemIndex_Top,false);
        break;
    case rootmap::vTop:
        //table_menu->Check(rootmap::TableMenuItemIndex_Front,false);
        //table_menu->Check(rootmap::TableMenuItemIndex_Side,false);
        table_menu->Check(rootmap::TableMenuItemIndex_Top, true);
        break;
    default:
        break;
    }
}

void RmTableWindow::Raise()
{
    wxWindow::Raise();

    LOG_DEBUG << "RmTableWindow::Raise {this:" << this << "}";

    UpdateColumnLabelStaticText();
    UpdateRowLabelStaticBitmap();
    UpdateColumnLabels();
    UpdateRowLabels();
}

bool RmTableWindow::Show(bool /*show*/)
{
    bool return_value = wxWindow::Show();

    // Logging has shown that this function gets called 100's of times per second for each window.
    // I don't know why this is, but its a bad function to call anything else from...
    // Might be a virtualisation issue
    //
    //LOG_DEBUG << "RmTableWindow::Show {this:" << this << "}";

    return return_value;
}

void RmTableWindow::Activate()
{
    wxMDIChildFrame::Activate();
    LOG_DEBUG << "RmTableWindow::Activate {this:" << this << "}";
}

void RmTableWindow::OnActivate(wxActivateEvent& /*event*/)
{
    LOG_DEBUG << "RmTableWindow::OnActivate {this:" << this << "}";
    m_layerHelper->ReplaceLayerMenu();
}

bool RmTableWindow::Destroy()
{
    LOG_DEBUG << "RmTableWindow::Destroy {this:" << this << "}";

    /**
     * This is one of the dumbest functions in wxWidgets. The base class performs
     *      delete this;
     *      return true;
     * Which is ridiculous. There must be a better way for a common gui
     * framework to handle removing a window.
     *
     * Issue 1 is that the only chance that it will return false is if WE override it
     *
     * Anyway. "this" is destroyed before it can call removeTableWindow, so that
     * we need to save a copy of the value of m_coordinator to use.
     *
     * Also, calling Disconnect invokes event handling on child windows, so that
     * needs to be performed first.
     */
     //rootmap::ITableCoordinator * old_coordinator = m_coordinator;
     //RmTableWindow * old_this = this;

     // MSA 09.12.08 I have shifted most of this functionality into this class's new dtor,
     // because Destroy() is not necessarily called in all of the possible ways to close
     // a RmTableWindow / a simulation / the whole program.
     // Cleaning up this class's relationships and member variables in the dtor
     // ensures that the cleanup is ALWAYS done, no matter how this window is ended.

     // However, removeTableWindow is called here and in DisconnectNotification()
     // rather than in the dtor. This is because m_coordinator is destroyed by the time we get 
     // to ~RmTableWindow() IF the whole program is shutting down.
     // There is no ill effect to calling removeTableWindow twice (it's just a std::list::remove() call).
    m_coordinator->removeTableWindow(this);

    bool ok = wxFrame::Destroy();
    if (!ok)
    {
        LOG_ALERT << "Window was not successfully Destroy()d ?!";
    }

    return ok;
}

void RmTableWindow::UpdateColumnLabelStaticText()
{
    const rootmap::TableInformation& table_info = GetTableInformation();
    std::string char_str = table_info.GetProcess()->GetCharacteristicName(table_info.GetCharacteristicNumber());
    std::string dir_str = rootmap::SprintUtility::StringOfViewDir(table_info.GetViewDirection());
    dir_str.insert(0, ", From ");
    dir_str.append(", Layer ");

    wxString label_str = char_str.c_str();
    label_str += dir_str.c_str();
    label_str += m_layerHelper->GetActiveItemString();

    m_columnLabelStaticText->SetLabel(label_str);
}

void RmTableWindow::UpdateRowLabelStaticBitmap()
{
    // determine text value
    rootmap::ViewDirection along_viewdir = rootmap::vSide;
    switch (GetTableInformation().GetViewDirection())
    {
    case rootmap::vFront: along_viewdir = rootmap::vTop;
        break;
    case rootmap::vSide: along_viewdir = rootmap::vTop;
        break;
    case rootmap::vTop: along_viewdir = rootmap::vSide;
        break;
    default: RmAssert(false, "Invalid ViewDirection");
        break;
    }
    std::string label = rootmap::SprintUtility::StringOfTableAxis(along_viewdir);
    label.insert(0, "Along ");

    // Draw !
    wxMemoryDC memDC;
    wxBitmap vert_bitmap = m_rowLabelStaticBitmap->GetBitmap();
    wxBitmap horiz_bitmap(static_cast<int>(vert_bitmap.GetHeight()), static_cast<int>(vert_bitmap.GetWidth()));

    memDC.SelectObject(horiz_bitmap);
    // memDC.SetBackground(wxColour(224,224,224)); // (*wxLIGHT_GREY);
    memDC.SetBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    memDC.Clear();
    memDC.SetTextForeground(*wxBLACK);
    memDC.SetBackgroundMode(wxTRANSPARENT);
    wxFont label_font = memDC.GetFont();
    label_font.SetWeight(wxFONTWEIGHT_NORMAL);
    label_font.SetFamily(wxFONTFAMILY_SWISS);
    memDC.SetFont(label_font);

    // drawing text in exactly the right spot is always so darned complicated!
    wxCoord width;
    wxCoord height;
    wxCoord descent;
    wxCoord externalLeading;
    GetTextExtent(label.c_str(), &width, &height, &descent, &externalLeading);
    wxRect rect(0, 0, vert_bitmap.GetHeight(), vert_bitmap.GetWidth());
    memDC.DrawLabel(label.c_str(), rect, wxALIGN_CENTRE);
    memDC.SelectObject(wxNullBitmap);

    // to do the pixel-level manipulation, we need to have images |-(
    wxImage horiz_image = horiz_bitmap.ConvertToImage();
    wxImage vert_image = vert_bitmap.ConvertToImage();

    unsigned char* horiz_data = horiz_image.GetData();
    unsigned char* vert_data = vert_image.GetData();

    size_t sizeof_data = vert_bitmap.GetHeight() * vert_bitmap.GetWidth();

    size_t horiz_height = vert_bitmap.GetWidth();
    size_t horiz_width = vert_bitmap.GetHeight();
    size_t vert_height = horiz_width;
    size_t vert_width = horiz_height;
    for (size_t index = 0; index < sizeof_data; ++index)
    {
        size_t real_index = index * 3; // one unsigned char for each of RGB
        size_t vert_x = index / vert_height;
        size_t vert_y = (vert_height - (index % vert_height)) - 1;
        size_t vert_y_offset = (vert_y * vert_width);
        size_t vert_index = (vert_y_offset + vert_x);
        size_t vert_real_index = vert_index * 3;

        vert_data[vert_real_index] = horiz_data[real_index];
        vert_data[vert_real_index + 1] = horiz_data[real_index + 1];
        vert_data[vert_real_index + 2] = horiz_data[real_index + 2];
    }

    wxBitmap new_bitmap(vert_image);
    m_rowLabelStaticBitmap->SetBitmap(new_bitmap);
}

void RmTableWindow::UpdateColumnLabels()
{
    m_tableHelper->UpdateColumnLabels();
}

void RmTableWindow::UpdateRowLabels()
{
    m_tableHelper->UpdateRowLabels();
}

wxMenuBar* RmTableWindow::GetMDIParentMenuBar()
{
    wxWindow* parent_win = GetParent();
    wxMDIParentFrame* parent_frame = wxDynamicCast(parent_win, wxMDIParentFrame);
    RmAssert(__nullptr != parent_frame, "TableWindow parent is not wxMDIParentFrame");

    wxMenuBar* menu_bar = parent_frame->GetMenuBar();
    RmAssert(__nullptr != menu_bar, "wxMDIParentFrame returned NULL MenuBar");

    return menu_bar;
}

/*!
 * wxEVT_MENU_OPEN event handler for rmID_TABLE_WINDOWFRAME
 */

void RmTableWindow::OnMenuOpen(wxMenuEvent& event)
{
    // only called when a table window is active/foremost. Nevertheless,...

    // here do only those things required if the window is the table menu
    wxMenu* table_menu = event.GetMenu();
    if (__nullptr != table_menu)
    {
        LOG_DEBUG << "RmTableWindow::OnMenuOpen {this:" << this << "}";
        // the View menu!  Can't just use the menu id as that isn't stored with
        // the menu. The only way seems to be to check if the Id of the first
        // item on the menu is the "New Table" menu item.
        wxMenuItem* item0 = table_menu->FindItemByPosition(0);
        int item0id = item0->GetId();
        int itemNewTableId = XRCID("rmID_TABLEMENU_NEWTABLESUBMENU");

        if (item0id == itemNewTableId)
        {
            const TableInformation& table_info = GetTableInformation();

            int current_id = 0;
#define ENABLE_ITEM(itemid, en_cond) \
            table_menu->Enable(XRCID(#itemid), en_cond)

#define CHECK_ENABLE_ITEM(itemid, en_cond, ck_cond) \
            current_id=XRCID(#itemid); \
            table_menu->Enable(current_id, en_cond); \
            table_menu->Check(current_id, ck_cond)

            rootmap::ViewDirection viewdir = table_info.GetViewDirection();

            ENABLE_ITEM(rmID_TABLEMENU_LAYERSUBMENU, true);
            ENABLE_ITEM(rmID_TABLEMENU_PREVIOUSLAYERITEM, (table_info.GetLayer() > 1));
            ENABLE_ITEM(rmID_TABLEMENU_NEXTLAYERITEM, (table_info.GetLayer() < static_cast<long>(m_layerHelper->GetNumLayers())));
            CHECK_ENABLE_ITEM(rmID_TABLEMENU_FROMFRONTITEM, (rootmap::vFront != viewdir), (rootmap::vFront == viewdir));
            CHECK_ENABLE_ITEM(rmID_TABLEMENU_FROMSIDEITEM, (rootmap::vSide != viewdir), (rootmap::vSide == viewdir));
            CHECK_ENABLE_ITEM(rmID_TABLEMENU_FROMTOPITEM, (rootmap::vTop != viewdir), (rootmap::vTop == viewdir));
            ENABLE_ITEM(rmID_TABLEMENU_CHARACTERISTICITEM, true);
            ENABLE_ITEM(rmID_TABLEMENU_DETAILSITEM, true);

            m_layerHelper->UpdateLayer();
        }
    }
    else
    {
        //LOG_ERROR << "MenuEvent yielded NULL menu ?!?!";
    }
}

/*!
 * wxEVT_MENU_CLOSE event handler for rmID_TABLE_WINDOWFRAME
 */

void RmTableWindow::OnMenuClose(wxMenuEvent& event)
{
    ////@begin wxEVT_MENU_CLOSE event handler for rmID_TABLE_WINDOWFRAME in rmTableWindow.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_MENU_CLOSE event handler for rmID_TABLE_WINDOWFRAME in rmTableWindow. 
}

/*!
 * wxEVT_CONTEXT_MENU event handler for rmID_TABLE_CORNERPANEL
 */

void RmTableWindow::OnContextMenu(wxContextMenuEvent& event)
{
    ////@begin wxEVT_CONTEXT_MENU event handler for rmID_TABLE_CORNERPANEL in rmTableWindow.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_CONTEXT_MENU event handler for rmID_TABLE_CORNERPANEL in rmTableWindow. 
}

/*!
 * wxEVT_GRID_CELL_CHANGE event handler for rmID_TABLE_GRID
 */

void RmTableWindow::OnCellChanged(wxGridEvent& event)
{
    ////@begin wxEVT_GRID_CELL_CHANGE event handler for rmID_TABLE_GRID in rmTableWindow.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_GRID_CELL_CHANGE event handler for rmID_TABLE_GRID in rmTableWindow. 
}

/*!
 * wxEVT_GRID_SELECT_CELL event handler for rmID_TABLE_GRID
 */

void RmTableWindow::OnSelectCell(wxGridEvent& event)
{
    ////@begin wxEVT_GRID_SELECT_CELL event handler for rmID_TABLE_GRID in rmTableWindow.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_GRID_SELECT_CELL event handler for rmID_TABLE_GRID in rmTableWindow. 
}

/*!
 * wxEVT_GRID_EDITOR_HIDDEN event handler for rmID_TABLE_GRID
 */

void RmTableWindow::OnEditorHidden(wxGridEvent& event)
{
    ////@begin wxEVT_GRID_EDITOR_HIDDEN event handler for rmID_TABLE_GRID in rmTableWindow.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_GRID_EDITOR_HIDDEN event handler for rmID_TABLE_GRID in rmTableWindow. 
}

/*!
 * wxEVT_GRID_EDITOR_SHOWN event handler for rmID_TABLE_GRID
 */

void RmTableWindow::OnEditorShown(wxGridEvent& event)
{
    ////@begin wxEVT_GRID_EDITOR_SHOWN event handler for rmID_TABLE_GRID in rmTableWindow.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_GRID_EDITOR_SHOWN event handler for rmID_TABLE_GRID in rmTableWindow. 
}

/*!
 * wxEVT_GRID_COL_SIZE event handler for rmID_TABLE_GRID
 */

void RmTableWindow::OnColSize(wxGridSizeEvent& event)
{
    ////@begin wxEVT_GRID_COL_SIZE event handler for rmID_TABLE_GRID in rmTableWindow.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_GRID_COL_SIZE event handler for rmID_TABLE_GRID in rmTableWindow. 
}

/*!
 * wxEVT_GRID_ROW_SIZE event handler for rmID_TABLE_GRID
 */

void RmTableWindow::OnRowSize(wxGridSizeEvent& event)
{
    ////@begin wxEVT_GRID_ROW_SIZE event handler for rmID_TABLE_GRID in rmTableWindow.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_GRID_ROW_SIZE event handler for rmID_TABLE_GRID in rmTableWindow. 
}

/*!
 * wxEVT_GRID_RANGE_SELECT event handler for rmID_TABLE_GRID
 */

void RmTableWindow::OnRangeSelect(wxGridRangeSelectEvent& event)
{
    ////@begin wxEVT_GRID_RANGE_SELECT event handler for rmID_TABLE_GRID in rmTableWindow.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_GRID_RANGE_SELECT event handler for rmID_TABLE_GRID in rmTableWindow. 
}

/*!
 * wxEVT_GRID_EDITOR_CREATED event handler for rmID_TABLE_GRID
 */

void RmTableWindow::OnEditorCreated(wxGridEditorCreatedEvent& event)
{
    ////@begin wxEVT_GRID_EDITOR_CREATED event handler for rmID_TABLE_GRID in rmTableWindow.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_GRID_EDITOR_CREATED event handler for rmID_TABLE_GRID in rmTableWindow. 
}


/*!
 * wxEVT_SIZE event handler for rmID_TABLE_GRID
 */

void RmTableWindow::OnSize(wxSizeEvent& event)
{
    ////@begin wxEVT_SIZE event handler for rmID_TABLE_GRID in rmTableWindow.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_SIZE event handler for rmID_TABLE_GRID in rmTableWindow. 
}

/*!
 * wxEVT_IDLE event handler for rmID_TABLE_GRID
 */

void RmTableWindow::OnIdle(wxIdleEvent& event)
{
    ////@begin wxEVT_IDLE event handler for rmID_TABLE_GRID in rmTableWindow.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_IDLE event handler for rmID_TABLE_GRID in rmTableWindow. 
}

/*!
 * Should we show tooltips?
 */

bool RmTableWindow::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap RmTableWindow::GetBitmapResource(const wxString& name)
{
    // Bitmap retrieval
    ////@begin RmTableWindow bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
    ////@end RmTableWindow bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon RmTableWindow::GetIconResource(const wxString& name)
{
    // Icon retrieval
    ////@begin RmTableWindow icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
    ////@end RmTableWindow icon retrieval
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_NEWTABLESUBMENU_SOILITEM
 */

 /*!
  * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_NEXTLAYERITEM
  */

void RmTableWindow::OnTableMenuNextLayerItemClick(wxCommandEvent& /*event*/)
{
    if (GetTableInformation().GetLayer() < static_cast<long>(m_layerHelper->GetNumLayers()))
    {
        if (m_tableHelper->updateLayer(0, +1) < rootmap::vrTryAgain)
        {
            m_layerHelper->UpdateLayer();
            UpdateColumnLabelStaticText();
        }
    }
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_PREVIOUSLAYERITEM
 */

void RmTableWindow::OnTableMenuPreviousLayerItemClick(wxCommandEvent& /*event*/)
{
    if (GetTableInformation().GetLayer() > 1)
    {
        if (m_tableHelper->updateLayer(0, -1) < rootmap::vrTryAgain)
        {
            // current layer has been updated in TableInformation
            m_layerHelper->UpdateLayer();
            UpdateColumnLabelStaticText();
        }
    }
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_FROMFRONTITEM
 */

void RmTableWindow::OnTableMenuFromFrontItemClick(wxCommandEvent& /*event*/)
{
    if (GetTableInformation().GetViewDirection() != rootmap::vFront)
    {
        if (m_tableHelper->updateViewDirection(rootmap::vFront) < rootmap::vrTryAgain)
        {
            m_layerHelper->RebuildLayerMenu();
            UpdateColumnLabelStaticText();
            UpdateRowLabelStaticBitmap();
            UpdateColumnLabels();
            UpdateRowLabels();
        }
        else
        {
            ReCheckViewDirectionTableMenuItem(GetTableInformation().GetViewDirection());
        }
    }
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_FROMSIDEITEM
 */

void RmTableWindow::OnTableMenuFromSideItemClick(wxCommandEvent& /*event*/)
{
    if (GetTableInformation().GetViewDirection() != rootmap::vSide)
    {
        if (m_tableHelper->updateViewDirection(rootmap::vSide) < rootmap::vrTryAgain)
        {
            m_layerHelper->RebuildLayerMenu();
            UpdateColumnLabelStaticText();
            UpdateRowLabelStaticBitmap();
            UpdateColumnLabels();
            UpdateRowLabels();
        }
        else
        {
            ReCheckViewDirectionTableMenuItem(GetTableInformation().GetViewDirection());
        }
    }
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_FROMTOPITEM
 */

void RmTableWindow::OnTableMenuFromTopItemClick(wxCommandEvent& /*event*/)
{
    if (GetTableInformation().GetViewDirection() != rootmap::vTop)
    {
        if (m_tableHelper->updateViewDirection(rootmap::vTop) < rootmap::vrTryAgain)
        {
            m_layerHelper->RebuildLayerMenu();
            UpdateColumnLabelStaticText();
            UpdateRowLabelStaticBitmap();
            UpdateColumnLabels();
            UpdateRowLabels();
        }
        else
        {
            ReCheckViewDirectionTableMenuItem(GetTableInformation().GetViewDirection());
        }
    }
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_CHARACTERISTICITEM
 */

void RmTableWindow::OnTableMenuCharacteristicItemClick(wxCommandEvent& /*event*/)
{
    wxArrayString characteristics;
    m_coordinator->populateCharacteristicStringArray(characteristics);

    RmSelectTableCharacteristic* window = new RmSelectTableCharacteristic(this, rmID_SELECTTABLECHARACTERISTIC_DIALOG, _("Select Table Characteristic"));
    window->m_listBox->Set(characteristics);
    int returnValue = window->ShowModal();

    wxString selected_name = window->m_listBox->GetStringSelection();
    window->Destroy();

    if ((wxID_CANCEL == returnValue) || (selected_name.empty()))
    {
        return;
    }

    long characteristic_number = -1;
    rootmap::Process* p = m_coordinator->findProcessCharacteristicNumber(selected_name, characteristic_number);

    RmAssert(__nullptr != p, "Could not find ProcessModule for selected Characteristic");

    if (m_tableHelper->updateProcess(p, characteristic_number) < rootmap::vrTryAgain)
    {
        //m_layerHelper->UpdateLayerMenu();
        UpdateColumnLabelStaticText();
        //UpdateRowLabelStaticBitmap();
        //UpdateColumnLabels();
        //UpdateRowLabels();
    }
    //else
    //{
    //    ReCheckViewDirectionTableMenuItem(GetTableInformation().GetViewDirection());
    //}
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_DETAILSITEM
 */

void RmTableWindow::OnTableMenuDetailsItemClick(wxCommandEvent& event)
{
    ////@begin wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_DETAILSITEM in RmTableWindow.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_DETAILSITEM in RmTableWindow. 
}


/*!
 * wxEVT_CHAR event handler for rmID_TABLE_GRID
 */

void RmTableWindow::OnChar(wxKeyEvent& event)
{
    ////@begin wxEVT_CHAR event handler for rmID_TABLE_GRID in RmTableWindow.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_CHAR event handler for rmID_TABLE_GRID in RmTableWindow. 
}

void RmTableWindow::OnLayerSubMenuItem(wxCommandEvent& event)
{
    int id = event.GetId();
    long position = m_layerHelper->FindIdPosition(id);

    LOG_INFO << "OnLayerSubMenuItem {Id:" << id
        << ", Position:" << position << "}";

    if ((position >= 0) &&
        (m_tableHelper->updateLayer(position, 0) < rootmap::vrTryAgain))
    {
        // current layer has been updated in TableInformation
        m_layerHelper->UpdateLayer();
        UpdateColumnLabelStaticText();
    }
    else
    {
        event.Skip();
    }
}

void RmTableWindow::PostTimestampNotification(const rootmap::PostOffice& /*postOffice*/)
{
    m_scoreboardGrid->BeginBatch();
    m_tableHelper->updateMessage();
    m_scoreboardGrid->EndBatch();
}

/**
 *    MSA 09.12.07 Perform necessary actions to allow this class to be destroyed without
 *    referring to any simulation code (as when the GUI is shut down with a simulation still open)
 */
void RmTableWindow::DisconnectNotification()
{
    m_tableHelper->getScoreboard().CancelDependency(m_tableHelper);
    m_coordinator->removeTableWindow(this);
}
