/////////////////////////////////////////////////////////////////////////////
// Name:        RmParentFrame.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     24/06/2006 16:16:14
// RCS-ID:      
// Copyright:   ©2006-2009 University of Tasmania
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "RmParentFrame.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "app/gui/ViewWindow/RmViewWindow.h"
#include "app/gui/View3DWindow/RmView3D.h"
#include "simulation/process/interprocess_communication/PostOffice.h"
#include "app/common/RmGetOpts.h"

#include "RmParentFrame.h"
#include "app/common/Types.h"
#include "app/common/FilesystemUtilities.h"
#include "app/gui/common/GuiSimulationEngine.h"
#include "app/gui/common/GuiIdentifierUtility.h"
#include "app/gui/TableWindow/RmTableWindow.h"

#include "simulation/scoreboard/ScoreboardCoordinator.h"
#include "simulation/data_access/tinyxmlconfig/TXDataAccessManager.h"

#include "core/scoreboard/ScoreboardStratum.h"
#include "core/common/RmAssert.h"

#include "wx/xrc/xmlres.h"
#include "wx/filedlg.h"
#include "wx/filefn.h"

#define ACTIVE_CHILD_PROCESS_EVENT \
    wxMDIChildFrame* child = GetActiveChild(); \
    if (0!=child) \
    { \
        child->GetEventHandler()->ProcessEvent(event);    \
    }


/*!
 * RmParentFrame type definition
 */

IMPLEMENT_CLASS(RmParentFrame, wxMDIParentFrame)

/*!
 * RmParentFrame event table definition
 */

    BEGIN_EVENT_TABLE(RmParentFrame, wxMDIParentFrame)

    ////@begin RmParentFrame event table entries
    EVT_MENU_OPEN(RmParentFrame::OnMenuOpen)
    EVT_MENU_CLOSE(RmParentFrame::OnMenuClose)
    EVT_CHAR(RmParentFrame::OnChar)
    EVT_CLOSE(RmParentFrame::OnClose)
    EVT_MENU(wxID_NEW, RmParentFrame::OnNewClick)
    EVT_MENU(wxID_OPEN, RmParentFrame::OnOpenClick)
    EVT_MENU(wxID_CLOSE_ALL, RmParentFrame::OnCloseAllClick)
    EVT_MENU(wxID_CLOSE, RmParentFrame::OnCloseClick)
    EVT_MENU(wxID_SAVE, RmParentFrame::OnSaveClick)
    EVT_MENU(wxID_SAVEAS, RmParentFrame::OnSaveasClick)
    EVT_MENU(wxID_EXIT, RmParentFrame::OnExitClick)
    EVT_MENU(wxID_UNDO, RmParentFrame::OnUndoClick)
    EVT_MENU(wxID_REDO, RmParentFrame::OnRedoClick)
    EVT_MENU(wxID_COPY, RmParentFrame::OnCopyClick)
    EVT_MENU(wxID_CUT, RmParentFrame::OnCutClick)
    EVT_MENU(wxID_PASTE, RmParentFrame::OnPasteClick)
    EVT_MENU(XRCID("rmID_EDITMENU_PASTESPECIALITEM"), RmParentFrame::OnEditMenuPasteSpecialItemClick)
    EVT_MENU(wxID_SELECTALL, RmParentFrame::OnSelectallClick)
    EVT_MENU(XRCID("rmID_RUNMENU_GOITEM"), RmParentFrame::OnRunMenuGoItemClick)
    EVT_MENU(XRCID("rmID_RUNMENU_STOPITEM"), RmParentFrame::OnRunMenuStopItemClick)
    // MSA 09.10.13    Implementing a simulation-pause function
    EVT_MENU(XRCID("rmID_RUNMENU_PAUSEITEM"), RmParentFrame::OnRunMenuPauseItemClick)
    EVT_MENU(XRCID("rmID_RUNMENU_SHOWPROGRESS"), RmParentFrame::OnRunMenuShowProgress)
    EVT_MENU(XRCID("rmID_RUNMENU_SETSTARTTIMEITEM"), RmParentFrame::OnRunMenuSetStartTimeItemClick)
    EVT_MENU(XRCID("rmID_VIEWMENU_NEWVIEWITEM"), RmParentFrame::OnViewMenuNewViewItemClick)
    EVT_MENU(XRCID("rmID_VIEWMENU_NEW3DVIEWITEM"), RmParentFrame::OnViewMenuNew3DViewItemClick)
    EVT_MENU(XRCID("rmID_VIEWMENU_FRONTVIEWITEM"), RmParentFrame::OnViewMenuFrontViewItemClick)
    EVT_MENU(XRCID("rmID_VIEWMENU_SIDEVIEWITEM"), RmParentFrame::OnViewMenuSideViewItemClick)
    EVT_MENU(XRCID("rmID_VIEWMENU_TOPVIEWITEM"), RmParentFrame::OnViewMenuTopViewItemClick)
    EVT_MENU(XRCID("rmID_VIEWMENU_REPEATITEM"), RmParentFrame::OnViewMenuRepeatItemClick)
    EVT_MENU(XRCID("rmID_VIEWMENU_WRAPITEM"), RmParentFrame::OnViewMenuWrapItemClick)
    EVT_MENU(XRCID("rmID_VIEWMENU_ZOOMINITEM"), RmParentFrame::OnViewMenuZoomInItemClick)
    EVT_MENU(XRCID("rmID_VIEWMENU_ZOOMOUTITEM"), RmParentFrame::OnViewMenuZoomOutItemClick)
    EVT_MENU(XRCID("rmID_VIEWMENU_BOUNDARIESITEM"), RmParentFrame::OnViewMenuBoundariesItemClick)
    EVT_MENU(XRCID("rmID_VIEWMENU_BOXESITEM"), RmParentFrame::OnViewMenuBoxesItemClick)
    EVT_MENU(XRCID("rmID_VIEWMENU_BOXCOLOURSITEM"), RmParentFrame::OnViewMenuBoxColoursItemClick)
    EVT_MENU(XRCID("rmID_VIEWMENU_PROCESSESITEM"), RmParentFrame::OnViewMenuProcessesItemClick)
    EVT_MENU(XRCID("rmID_VIEWMENU_CHARACTERISTICSITEM"), RmParentFrame::OnViewMenuCharacteristicsItemClick)
    EVT_MENU(XRCID("rmID_VIEWMENU_SCOREBOARDSUBMENU_AIRITEM"), RmParentFrame::OnViewMenuScoreboardSubmenuAirItemClick)
    EVT_MENU(XRCID("rmID_VIEWMENU_SCOREBOARDSUBMENU_LITTERITEM"), RmParentFrame::OnViewMenuScoreboardSubmenuLitterItemClick)
    EVT_MENU(XRCID("rmID_VIEWMENU_SCOREBOARDSUBMENU_SOILITEM"), RmParentFrame::OnViewMenuScoreboardSubmenuSoilItemClick)
    EVT_MENU(XRCID("rmID_VIEWMENU_SCOREBOARDSUBMENU_DRAINITEM"), RmParentFrame::OnViewMenuScoreboardSubmenuDrainItemClick)
    EVT_MENU(XRCID("rmID_VIEWMENU_SCOREBOARDSUBMENU_ALLITEM"), RmParentFrame::OnViewMenuScoreboardSubmenuAllItemClick)
    EVT_MENU(XRCID("rmID_VIEWMENU_DETAILSITEM"), RmParentFrame::OnViewMenuDetailsItemClick)
    EVT_MENU(XRCID("rmID_TABLEMENU_NEWTABLESUBMENU_AIRITEM"), RmParentFrame::OnTableMenuNewTableSubmenuAirItemClick)
    EVT_MENU(XRCID("rmID_TABLEMENU_NEWTABLESUBMENU_LITTERITEM"), RmParentFrame::OnTableMenuNewTableSubmenuLitterItemClick)
    EVT_MENU(XRCID("rmID_TABLEMENU_NEWTABLESUBMENU_SOILITEM"), RmParentFrame::OnTableMenuNewTableSubmenuSoilItemClick)
    EVT_MENU(XRCID("rmID_TABLEMENU_NEWTABLESUBMENU_DRAINITEM"), RmParentFrame::OnTableMenuNewTableSubmenuDrainItemClick)
    EVT_MENU(XRCID("rmID_TABLEMENU_PREVIOUSLAYERITEM"), RmParentFrame::OnTableMenuPreviousLayerItemClick)
    EVT_MENU(XRCID("rmID_TABLEMENU_NEXTLAYERITEM"), RmParentFrame::OnTableMenuNextLayerItemClick)
    EVT_MENU(XRCID("rmID_TABLEMENU_FROMFRONTITEM"), RmParentFrame::OnTableMenuFromFrontItemClick)
    EVT_MENU(XRCID("rmID_TABLEMENU_FROMSIDEITEM"), RmParentFrame::OnTableMenuFromSideItemClick)
    EVT_MENU(XRCID("rmID_TABLEMENU_FROMTOPITEM"), RmParentFrame::OnTableMenuFromTopItemClick)
    EVT_MENU(XRCID("rmID_TABLEMENU_CHARACTERISTICITEM"), RmParentFrame::OnTableMenuCharacteristicItemClick)
    EVT_MENU(XRCID("rmID_TABLEMENU_DETAILSITEM"), RmParentFrame::OnTableMenuDetailsItemClick)
    // MSA 10.01.21 Implementing Help menu About window
    EVT_MENU(XRCID("wxID_ABOUT"), RmParentFrame::OnHelpMenuAboutItemClick)

    ////@end RmParentFrame event table entries

    END_EVENT_TABLE()

    using rootmap::GuiIdentifierUtility;

/*!
 * RmParentFrame constructors
 */

RmParentFrame::RmParentFrame()
    : m_engine(__nullptr)
    , m_tx_dam(__nullptr)
{
}

RmParentFrame::RmParentFrame(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
    : m_engine(__nullptr)
    , m_tx_dam(__nullptr)
{
    Create(parent, id, caption, pos, size, style);
}

RmParentFrame::~RmParentFrame()
{
    delete m_tx_dam;
    delete m_engine;
}

/*!
 * RmParentFrame creator
 */

bool RmParentFrame::Create(wxWindow* parent, wxWindowID /* id */, const wxString& /* caption */, const wxPoint& /* pos */, const wxSize& /* size */, long /* style */)
{
    ////@begin RmParentFrame member initialisation
    m_engine = __nullptr;
    ////@end RmParentFrame member initialisation

    ////@begin RmParentFrame creation
    SetParent(parent);
    CreateControls();
    ////@end RmParentFrame creation

    wxAcceleratorEntry accelerators[2];
    accelerators[0].Set(wxACCEL_SHIFT, WXK_PAUSE, XRCID("rmID_RUNMENU_STOPITEM"));
    accelerators[1].Set(wxACCEL_NORMAL, WXK_PAUSE, XRCID("rmID_RUNMENU_STOPITEM"));
    wxAcceleratorTable accel(2, accelerators);
    SetAcceleratorTable(accel);

    int min_id, max_id;
    rootmap::GuiIdentifierUtility::instance().getLayerMenuItemIdentifierRange(min_id, max_id);
    Connect(min_id, max_id,
        wxEVT_COMMAND_MENU_SELECTED,
        wxCommandEventHandler(RmParentFrame::OnLayerSubMenuItem));

    return true;
}

void RmParentFrame::OnClose(wxCloseEvent& closeEvent)
{
    if (closeEvent.CanVeto())
    {
        if (!TryCloseSimulation())
        {
            closeEvent.Veto();
            return;
        }
    }
    else
    {
        DoCloseSimulation();
    }
    closeEvent.Skip(); // Confusingly, this actually lets the default close handler run (i.e. closes the frame)
}

/*!
 * Control creation for RmParentFrame
 */

void RmParentFrame::CreateControls()
{
    ////@begin RmParentFrame content construction
    if (!wxXmlResource::Get()->LoadObject(this, GetParent(), wxT("rmID_PARENT_WINDOWFRAME"), wxT("wxMDIParentFrame")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    ////@end RmParentFrame content construction

    // Create custom windows not generated automatically here.
    ////@begin RmParentFrame content initialisation
    ////@end RmParentFrame content initialisation
    wxMenuBar* menu_bar = GetMenuBar();

    if (__nullptr != menu_bar)
    {
        //wxMenu * help_menu = new wxMenu("Help");
        //help_menu->Append(wxID_HELP, _T("&About RootMap"), _T("About RootMap"));
        //menu_bar->Append(help_menu, "&Help");
        menu_bar->EnableTop(rootmap::RootMapMenuIndex_Edit, false);
        menu_bar->EnableTop(rootmap::RootMapMenuIndex_Run, false);
        menu_bar->EnableTop(rootmap::RootMapMenuIndex_View, false);
        menu_bar->EnableTop(rootmap::RootMapMenuIndex_Table, false);

        menu_bar->Enable(wxID_CLOSE_ALL, false);
        menu_bar->Enable(wxID_CLOSE, false);
        menu_bar->Enable(wxID_SAVE, false);
        menu_bar->Enable(wxID_SAVEAS, false);
    }
}

/*!
 * wxEVT_MENU_OPEN event handler for rmID_PARENT_WINDOWFRAME
 */

void RmParentFrame::OnMenuOpen(wxMenuEvent& event)
{
    bool doEnable = (__nullptr != m_engine);

    wxMenuBar* menu_bar = GetMenuBar();
    // this is mostly for sanity - there should be enough for us to disable
    int menu_count = menu_bar->GetMenuCount();
    RmAssert(menu_count == rootmap::RootMapMenus_NumberOf, "Incorrect number of Menus");

    /** MSA 09.11.25 Nothing in the Edit menu is yet supported.
     *  When the active child window does not have a handler associated with an Edit menu function,
     *  selecting that menu option causes an infinite-recursion stack overflow.
     *  Hence, Edit menu is disabled for now.
     */
     //menu_bar->EnableTop(rootmap::RootMapMenuIndex_Edit, doEnable);
    menu_bar->EnableTop(rootmap::RootMapMenuIndex_Run, doEnable);
    menu_bar->EnableTop(rootmap::RootMapMenuIndex_View, doEnable);
    menu_bar->EnableTop(rootmap::RootMapMenuIndex_Table, doEnable);

    menu_bar->Enable(wxID_CLOSE_ALL, doEnable);
    menu_bar->Enable(wxID_CLOSE, doEnable);
    menu_bar->Enable(wxID_SAVE, doEnable);
    menu_bar->Enable(wxID_SAVEAS, doEnable);

    m_engine->OnMenuOpen(event);

    wxMDIChildFrame* active_child = GetActiveChild();
    // Until "New Simulation" or "Open Simulation" is performed, there aren't
    // any child windows
    if (nullptr != active_child)
    {
        bool activeChildIsView = wxDynamicCast(active_child, RmViewWindow) != nullptr;
        bool activeChildIsView3D = !activeChildIsView
            && wxDynamicCast(active_child, RmView3D) != nullptr;
        bool activeChildIsTable = !activeChildIsView
            && !activeChildIsView3D
            && wxDynamicCast(active_child, RmTableWindow) != nullptr;
        if (activeChildIsTable)
        {
            DeactivateViewMenu(menu_bar);
        }
        else
        {
            DeactivateTableMenu(menu_bar);
        }
    }

    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_MENU_CLOSE event handler for rmID_PARENT_WINDOWFRAME
 */

void RmParentFrame::OnMenuClose(wxMenuEvent& event)
{
    ////@begin wxEVT_MENU_CLOSE event handler for rmID_PARENT_WINDOWFRAME in RmParentFrame.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_MENU_CLOSE event handler for rmID_PARENT_WINDOWFRAME in RmParentFrame. 
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_NEW
 */

void RmParentFrame::OnNewClick(wxCommandEvent& /* event */)
{
    RunSimulation();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_OPEN
 */

void RmParentFrame::OnOpenClick(wxCommandEvent& event)
{
    // Find a config directory
    wxFileDialog* fd = new wxFileDialog(this, "Locate rootmap.xml to open", rootmap::RmGetOpts::getInstance().getOpt(RMGETOPTS_CONFIGDIR).c_str(), "rootmap.xml", "*.xml");

    int result = fd->ShowModal();

    if (result == wxID_OK)
    {
        // Else: get the selected file and create the simulation.
        rootmap::RmGetOpts::getInstance().setOpt(RMGETOPTS_CONFIGDIR, fd->GetDirectory().c_str());
        rootmap::RmGetOpts::getInstance().setOpt(RMGETOPTS_CONFIGFILE, fd->GetFilename().c_str());

        wxDELETE(fd);

        RunSimulation();
    }
    else
    {
        event.Skip();
        wxDELETE(fd);
    }
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_CLOSE_ALL
 */

void RmParentFrame::OnCloseAllClick(wxCommandEvent& /* event */)
{
    TryCloseSimulation();
}

bool RmParentFrame::TryCloseSimulation()
{
    if (m_engine != __nullptr && m_engine->IsBusy())
    {
        if (wxOK == wxMessageBox("Warning! A simulation is currently processing.\nAre you sure you wish to terminate and close it?",
            "Simulation in process!", wxOK | wxCANCEL))
        {
            m_engine->SetCloseOnCompletion(true);
            m_engine->Stop();
        }
        else
        {
            return false; // (do nothing, user does not wish to close sim)
        }
    }
    else
    {
        DoCloseSimulation();
    }
    return true;
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_CLOSE
 */

void RmParentFrame::OnCloseClick(wxCommandEvent& /* event */)
{
    wxMDIChildFrame* c = GetActiveChild();
    if (c)
    {
        c->Destroy();
    }
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_EXIT
 */

void RmParentFrame::OnExitClick(wxCommandEvent& /* event */)
{
    if (TryCloseSimulation())
    {
        Destroy();
    }
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_RUNMENU_GOITEM
 */

void RmParentFrame::OnRunMenuGoItemClick(wxCommandEvent& /* event */)
{
    // May now pause the simulation
    wxMenuBar* menu_bar = GetMenuBar();
    menu_bar->Enable(XRCID("rmID_RUNMENU_PAUSEITEM"), true);
    m_engine->Run();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_NEWVIEWITEM
 */

void RmParentFrame::OnViewMenuNewViewItemClick(wxCommandEvent& /* event */)
{
    m_engine->NewViewWindow();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_FRONTVIEWITEM
 */

void RmParentFrame::OnViewMenuFrontViewItemClick(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_SIDEVIEWITEM
 */

void RmParentFrame::OnViewMenuSideViewItemClick(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_TOPVIEWITEM
 */

void RmParentFrame::OnViewMenuTopViewItemClick(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * Should we show tooltips?
 */

bool RmParentFrame::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap RmParentFrame::GetBitmapResource(const wxString& name)
{
    // Bitmap retrieval
    ////@begin RmParentFrame bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
    ////@end RmParentFrame bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon RmParentFrame::GetIconResource(const wxString& name)
{
    // Icon retrieval
    ////@begin RmParentFrame icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
    ////@end RmParentFrame icon retrieval
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_SAVE
 */

void RmParentFrame::OnSaveClick(wxCommandEvent& event)
{
    // Saving simulations not supported yet
    wxMessageBox("ROOTMAP does not yet support saving simulations from the GUI.",
        "Save unsupported", wxOK);
    event.Skip();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_SAVEAS
 */

void RmParentFrame::OnSaveasClick(wxCommandEvent& event)
{
    // Saving simulations not supported yet
    wxMessageBox("ROOTMAP does not yet support saving simulations from the GUI.",
        "Save As unsupported", wxOK);
    event.Skip();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_UNDO
 */

void RmParentFrame::OnUndoClick(wxCommandEvent& event)
{
    ////@begin wxEVT_COMMAND_MENU_SELECTED event handler for wxID_UNDO in RmParentFrame.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_COMMAND_MENU_SELECTED event handler for wxID_UNDO in RmParentFrame. 
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_REDO
 */

void RmParentFrame::OnRedoClick(wxCommandEvent& event)
{
    ////@begin wxEVT_COMMAND_MENU_SELECTED event handler for wxID_REDO in RmParentFrame.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_COMMAND_MENU_SELECTED event handler for wxID_REDO in RmParentFrame. 
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_COPY
 */

void RmParentFrame::OnCopyClick(wxCommandEvent& event)
{
    ////@begin wxEVT_COMMAND_MENU_SELECTED event handler for wxID_COPY in RmParentFrame.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_COMMAND_MENU_SELECTED event handler for wxID_COPY in RmParentFrame. 
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_CUT
 */

void RmParentFrame::OnCutClick(wxCommandEvent& event)
{
    ////@begin wxEVT_COMMAND_MENU_SELECTED event handler for wxID_CUT in RmParentFrame.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_COMMAND_MENU_SELECTED event handler for wxID_CUT in RmParentFrame. 
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_PASTE
 */

void RmParentFrame::OnPasteClick(wxCommandEvent& event)
{
    ////@begin wxEVT_COMMAND_MENU_SELECTED event handler for wxID_PASTE in RmParentFrame.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_COMMAND_MENU_SELECTED event handler for wxID_PASTE in RmParentFrame. 
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_EDITMENU_PASTESPECIALITEM
 */

void RmParentFrame::OnEditMenuPasteSpecialItemClick(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_SELECTALL
 */

void RmParentFrame::OnSelectallClick(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_RUNMENU_PAUSEITEM
 */
void RmParentFrame::OnRunMenuPauseItemClick(wxCommandEvent& /* event */)
{
    m_engine->PauseToggle();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_RUNMENU_STOPITEM
 */

void RmParentFrame::OnRunMenuStopItemClick(wxCommandEvent& /* event */)
{
    // Can't pause a stopped simulation
    wxMenuBar* menu_bar = GetMenuBar();
    menu_bar->Enable(XRCID("rmID_RUNMENU_PAUSEITEM"), false);
    m_engine->Stop();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_RUNMENU_SETSTARTTIMEITEM
 */

void RmParentFrame::OnRunMenuSetStartTimeItemClick(wxCommandEvent& event)
{
    // Setting start time not supported yet
    wxMessageBox("ROOTMAP does not yet support setting the simulation start time from the GUI.",
        "Currently unsupported", wxOK);
    event.Skip();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_RUNMENU_SHOWPROGRESS
 */

void RmParentFrame::OnRunMenuShowProgress(wxCommandEvent& /* event */)
{
    m_engine->ShowProgressWindow();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_REPEATITEM
 */

void RmParentFrame::OnViewMenuRepeatItemClick(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_WRAPITEM
 */

void RmParentFrame::OnViewMenuWrapItemClick(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_ZOOMINITEM
 */

void RmParentFrame::OnViewMenuZoomInItemClick(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_ZOOMOUTITEM
 */

void RmParentFrame::OnViewMenuZoomOutItemClick(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_BOUNDARIESITEM
 */

void RmParentFrame::OnViewMenuBoundariesItemClick(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_BOXESITEM
 */

void RmParentFrame::OnViewMenuBoxesItemClick(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_BOXCOLOURSITEM
 */

void RmParentFrame::OnViewMenuBoxColoursItemClick(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_PROCESSESITEM
 */

void RmParentFrame::OnViewMenuProcessesItemClick(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_CHARACTERISTICSITEM
 */

void RmParentFrame::OnViewMenuCharacteristicsItemClick(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_SCOREBOARDSUBMENU_AIRITEM
 */

void RmParentFrame::OnViewMenuScoreboardSubmenuAirItemClick(wxCommandEvent& event)
{
    // Air stratum views not supported yet
    wxMessageBox("ROOTMAP does not yet support Air stratum views.",
        "Currently unsupported", wxOK);
    event.Skip();
    //ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_SCOREBOARDSUBMENU_LITTERITEM
 */

void RmParentFrame::OnViewMenuScoreboardSubmenuLitterItemClick(wxCommandEvent& event)
{
    // Litter stratum views not supported yet
    wxMessageBox("ROOTMAP does not yet support Litter stratum views.",
        "Currently unsupported", wxOK);
    event.Skip();
    //ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_SCOREBOARDSUBMENU_SOILITEM
 */

void RmParentFrame::OnViewMenuScoreboardSubmenuSoilItemClick(wxCommandEvent& /* event */)
{
    // No change.
    //ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_SCOREBOARDSUBMENU_DRAINITEM
 */

void RmParentFrame::OnViewMenuScoreboardSubmenuDrainItemClick(wxCommandEvent& event)
{
    // Drain stratum views not supported yet
    wxMessageBox("ROOTMAP does not yet support Drain stratum views.",
        "Currently unsupported", wxOK);
    event.Skip();
    //ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_SCOREBOARDSUBMENU_ALLITEM
 */

void RmParentFrame::OnViewMenuScoreboardSubmenuAllItemClick(wxCommandEvent& event)
{
    wxMessageBox("ROOTMAP currently only supports Soil stratum views.",
        "Currently unsupported", wxOK);
    event.Skip();
    //ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_DETAILSITEM
 */

void RmParentFrame::OnViewMenuDetailsItemClick(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_NEWTABLESUBMENU_AIRITEM
 */

void RmParentFrame::OnTableMenuNewTableSubmenuAirItemClick(wxCommandEvent& event)
{
    // Air stratum tables not supported yet
    wxMessageBox("ROOTMAP does not yet support Air stratum tables.",
        "Currently unsupported", wxOK);
    event.Skip();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_NEWTABLESUBMENU_LITTERITEM
 */

void RmParentFrame::OnTableMenuNewTableSubmenuLitterItemClick(wxCommandEvent& event)
{
    // Litter stratum tables not supported yet
    wxMessageBox("ROOTMAP does not yet support Litter stratum tables.",
        "Currently unsupported", wxOK);
    event.Skip();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_NEWTABLESUBMENU_SOILITEM
 */

void RmParentFrame::OnTableMenuNewTableSubmenuSoilItemClick(wxCommandEvent& /* event */)
{
    // assumes that if this menu was clicked, there is a SimulationEngine
    m_engine->NewTableWindow(rootmap::ScoreboardStratum(rootmap::ScoreboardStratum::Soil));
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_NEWTABLESUBMENU_DRAINITEM
 */

void RmParentFrame::OnTableMenuNewTableSubmenuDrainItemClick(wxCommandEvent& event)
{
    // Drain stratum tables not supported yet
    wxMessageBox("ROOTMAP does not yet support Drain stratum tables.",
        "Currently unsupported", wxOK);
    event.Skip();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_NEXTLAYERITEM
 */

void RmParentFrame::OnTableMenuNextLayerItemClick(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_PREVIOUSLAYERITEM
 */

void RmParentFrame::OnTableMenuPreviousLayerItemClick(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_FROMFRONTITEM
 */

void RmParentFrame::OnTableMenuFromFrontItemClick(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_FROMSIDEITEM
 */

void RmParentFrame::OnTableMenuFromSideItemClick(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_FROMTOPITEM
 */

void RmParentFrame::OnTableMenuFromTopItemClick(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_CHARACTERISTICITEM
 */

void RmParentFrame::OnTableMenuCharacteristicItemClick(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_TABLEMENU_DETAILSITEM
 */

void RmParentFrame::OnTableMenuDetailsItemClick(wxCommandEvent& event)
{
    // MSA 09.11.25 Table-->Details not yet implemented (what would it even be?)
    wxMessageBox("ROOTMAP does not yet support Details function for tables.",
        "Currently unsupported", wxOK);
    event.Skip(true);
}


void RmParentFrame::DeactivateTableMenu(wxMenuBar* menuBar)
{
    // disables all but the New submenu
    wxMenu* table_menu = menuBar->GetMenu(rootmap::RootMapMenuIndex_Table);

    table_menu->Enable(XRCID("rmID_TABLEMENU_LAYERSUBMENU"), false);
    table_menu->Enable(XRCID("rmID_TABLEMENU_PREVIOUSLAYERITEM"), false);
    table_menu->Enable(XRCID("rmID_TABLEMENU_NEXTLAYERITEM"), false);
    table_menu->Enable(XRCID("rmID_TABLEMENU_FROMFRONTITEM"), false);
    table_menu->Enable(XRCID("rmID_TABLEMENU_FROMSIDEITEM"), false);
    table_menu->Enable(XRCID("rmID_TABLEMENU_FROMTOPITEM"), false);
    table_menu->Enable(XRCID("rmID_TABLEMENU_CHARACTERISTICITEM"), false);
    table_menu->Enable(XRCID("rmID_TABLEMENU_DETAILSITEM"), false);
}


void RmParentFrame::DeactivateViewMenu(wxMenuBar* menuBar)
{
    // disables all but the New View menu item
    wxMenu* view_menu = menuBar->GetMenu(rootmap::RootMapMenuIndex_View);

    view_menu->Enable(XRCID("rmID_VIEWMENU_FRONTVIEWITEM"), false);
    view_menu->Enable(XRCID("rmID_VIEWMENU_SIDEVIEWITEM"), false);
    view_menu->Enable(XRCID("rmID_VIEWMENU_TOPVIEWITEM"), false);
    view_menu->Enable(XRCID("rmID_VIEWMENU_REPEATITEM"), false);
    view_menu->Enable(XRCID("rmID_VIEWMENU_WRAPITEM"), false);
    view_menu->Enable(XRCID("rmID_VIEWMENU_ZOOMINITEM"), false);
    view_menu->Enable(XRCID("rmID_VIEWMENU_ZOOMOUTITEM"), false);
    view_menu->Enable(XRCID("rmID_VIEWMENU_BOUNDARIESITEM"), false);
    view_menu->Enable(XRCID("rmID_VIEWMENU_BOXESITEM"), false);
    view_menu->Enable(XRCID("rmID_VIEWMENU_BOXCOLOURSITEM"), false);
    view_menu->Enable(XRCID("rmID_VIEWMENU_PROCESSESITEM"), false);
    view_menu->Enable(XRCID("rmID_VIEWMENU_CHARACTERISTICSITEM"), false);
    view_menu->Enable(XRCID("rmID_VIEWMENU_SCOREBOARDSUBMENU"), false);
    view_menu->Enable(XRCID("rmID_VIEWMENU_DETAILSITEM"), false);
}

void RmParentFrame::DoCloseSimulation()
{
    wxWindowList wl = GetChildren();
    for (size_t i = 0; i < wl.size(); ++i)
    {
        if (wl[i])
        {
            wl[i]->Destroy();
        }
    }

    wxMenuBar* menu_bar = GetMenuBar();
    int menu_count = menu_bar->GetMenuCount();
    for (int menu_num = 1; menu_num < menu_count; ++menu_num)
    {
        menu_bar->EnableTop(menu_num, false);
    }

    delete m_engine;
    m_engine = __nullptr;
    delete m_tx_dam;
    m_tx_dam = __nullptr;
    menu_bar->Enable(wxID_CLOSE, false);
    menu_bar->Enable(wxID_CLOSE_ALL, false);
    menu_bar->Enable(wxID_SAVE, false);
    menu_bar->Enable(wxID_SAVEAS, false);
    menu_bar->Enable(wxID_NEW, true);
}

void RmParentFrame::OnChar(wxKeyEvent& event)
{
    event.Skip();
}

void RmParentFrame::OnLayerSubMenuItem(wxCommandEvent& event)
{
    ACTIVE_CHILD_PROCESS_EVENT
}

void RmParentFrame::OnViewMenuNew3DViewItemClick(wxCommandEvent& /* event */)
{
    m_engine->NewView3DWindow();
}

void RmParentFrame::OnHelpMenuAboutItemClick(wxCommandEvent& event)
{
    wxString str = "ROOTMAP v";
    str << rootmapNativeVersion << "\nCopyright " << wxDateTime::Now().GetYear() << "\nUniversity of Tasmania /\nUniversity of Western Australia\n";
    wxMessageBox(str, "About ROOTMAP");
    event.Skip();
}

void RmParentFrame::RunSimulation()
{
    wxMenuBar* menu_bar = GetMenuBar();
    int menu_count = menu_bar->GetMenuCount();

    // menu item numbering is zero-based,
    // we don't want to do disable the first menu (File), only disable some items on it

    /** MSA 09.11.25 Nothing in the Edit menu is yet supported.
     *  If/when the active child window does not have a handler associated with an Edit menu function,
     *  selecting that menu option causes an infinite-recursion stack overflow.
     *  Hence, Edit menu is disabled for now. So we start from menu_num=2.
     */
    for (int menu_num = 2; menu_num < menu_count; ++menu_num)
    {
        menu_bar->EnableTop(menu_num, true);
    }

    menu_bar->Enable(wxID_CLOSE_ALL, true);
    //menu_bar->Enable(wxID_CLOSE, true); depends on if a window is open
    menu_bar->Enable(wxID_SAVE, false);
    menu_bar->Enable(wxID_SAVEAS, true);

    // MSA 09.10.14 Can't pause a simulation before it starts
    menu_bar->Enable(XRCID("rmID_RUNMENU_PAUSEITEM"), false);


    auto opts = rootmap::RmGetOpts::getInstance();
    std::deque<wxString> subdirs;
    std::string configDir = (opts.isOptSet(RMGETOPTS_CONFIGDIR)) ? opts.getOpt(RMGETOPTS_CONFIGDIR) : "Configurations";
    // Ensure configuration directory path is absolute.
    configDir = rootmap::FilesystemUtilities::resolveAbsolutePath(configDir, subdirs, opts.getVersion()).c_str();

    std::string configFile = "rootmap.xml";
    if (opts.isOptSet(RMGETOPTS_CONFIGFILE))
    {
        configFile = opts.getOpt(RMGETOPTS_CONFIGFILE);
    }

    subdirs.clear();
    std::string outputDir = (opts.isOptSet(RMGETOPTS_OUTPUTDIR)) ? opts.getOpt(RMGETOPTS_OUTPUTDIR) : "Output";
    // Ensure output directory path is absolute.
    outputDir = rootmap::FilesystemUtilities::resolveAbsolutePath(outputDir, subdirs, opts.getVersion()).c_str();

    if (opts.isOptSet(RMGETOPTS_SEED))
    {
        std::string seedStr = opts.getOpt(RMGETOPTS_SEED);
        m_tx_dam = new rootmap::TXDataAccessManager(configDir, outputDir, configFile, seedStr);
    }
    else
    {
        m_tx_dam = new rootmap::TXDataAccessManager(configDir, outputDir, configFile);
    }

    // m_engine takes ownership of tx_dam
    m_engine = new rootmap::GuiSimulationEngine(*m_tx_dam, this);
    m_engine->Initialise();

    // MSA 09.11.23 Don't want to allow another File-->New call on top of this one
    menu_bar->Enable(wxID_NEW, false);
}
