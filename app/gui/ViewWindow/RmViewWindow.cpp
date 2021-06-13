/////////////////////////////////////////////////////////////////////////////
// Name:        RmViewWindow.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     24/06/2006 16:35:53
// RCS-ID:      
// Copyright:   ©2006 University of Tasmania
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "RmViewWindow.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#include "wx/defs.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
#include "RmScrolledWindow.h"
////@end includes

#include "app/gui/common/IViewCoordinator.h"
#include "app/gui/ViewWindow/RmViewWindow.h"
#include "app/gui/ViewCommon/ViewInformation.h"
#include "app/gui/ViewProcessSelection/RmViewProcessSelectionDialog.h"
#include "app/gui/CharacteristicColourSelectionDialog/RmCharacteristicColourSelectionDialog.h"

#include "simulation/process/common/Process.h"

#include "core/common/RmAssert.h"

using rootmap::ViewInformation;
using rootmap::ProcessIdentifier;
using rootmap::ProcessIdentifierSet;
using rootmap::Process;
using rootmap::ProcessArray;

////@begin XPM images
////@end XPM images

/*!
 * RmViewWindow type definition
 */

IMPLEMENT_CLASS(RmViewWindow, wxMDIChildFrame)

/*!
 * RmViewWindow event table definition
 */

    BEGIN_EVENT_TABLE(RmViewWindow, wxMDIChildFrame)

    ////@begin RmViewWindow event table entries
    EVT_MENU_OPEN(RmViewWindow::OnMenuOpen)
    EVT_MENU_CLOSE(RmViewWindow::OnMenuClose)
    EVT_ERASE_BACKGROUND(RmViewWindow::OnEraseBackground)

    EVT_MENU(XRCID("rmID_VIEWMENU_FRONTVIEWITEM"), RmViewWindow::OnViewMenuFrontViewItemClick)

    EVT_MENU(XRCID("rmID_VIEWMENU_SIDEVIEWITEM"), RmViewWindow::OnViewMenuSideViewItemClick)

    EVT_MENU(XRCID("rmID_VIEWMENU_TOPVIEWITEM"), RmViewWindow::OnViewMenuTopViewItemClick)

    EVT_MENU(XRCID("rmID_VIEWMENU_REPEATITEM"), RmViewWindow::OnViewMenuRepeatItemClick)

    EVT_MENU(XRCID("rmID_VIEWMENU_WRAPITEM"), RmViewWindow::OnViewMenuWrapItemClick)

    EVT_MENU(XRCID("rmID_VIEWMENU_ZOOMINITEM"), RmViewWindow::OnViewMenuZoomInItemClick)

    EVT_MENU(XRCID("rmID_VIEWMENU_ZOOMOUTITEM"), RmViewWindow::OnViewMenuZoomOutItemClick)

    EVT_MENU(XRCID("rmID_VIEWMENU_BOUNDARIESITEM"), RmViewWindow::OnViewMenuBoundariesItemClick)

    EVT_MENU(XRCID("rmID_VIEWMENU_BOXESITEM"), RmViewWindow::OnViewMenuBoxesItemClick)

    EVT_MENU(XRCID("rmID_VIEWMENU_BOXCOLOURSITEM"), RmViewWindow::OnViewMenuBoxColoursItemClick)

    EVT_MENU(XRCID("rmID_VIEWMENU_PROCESSESITEM"), RmViewWindow::OnViewMenuProcessesItemClick)

    EVT_MENU(XRCID("rmID_VIEWMENU_CHARACTERISTICSITEM"), RmViewWindow::OnViewMenuCharacteristicsItemClick)

    EVT_MENU(XRCID("rmID_VIEWMENU_DETAILSITEM"), RmViewWindow::OnViewMenuDetailsItemClick)

    ////@end RmViewWindow event table entries

    END_EVENT_TABLE()

    /*!
     * RmViewWindow constructors
     */

    RmViewWindow::RmViewWindow()
    : m_scrolledWindow(__nullptr)
    , m_coordinator(__nullptr)
{
}

RmViewWindow::RmViewWindow(wxMDIParentFrame* parent, rootmap::IViewCoordinator* viewCoordinator, rootmap::ScoreboardCoordinator& scoreboardcoordinator, const ViewInformation& viewInfo, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
    : m_scrolledWindow(__nullptr)
    , m_coordinator(viewCoordinator)
{
    Create(parent, viewCoordinator, scoreboardcoordinator, viewInfo, id, caption, pos, size, style);
}

RmViewWindow::~RmViewWindow()
{
}

/*!
 * RmViewWindow creator
 */

bool RmViewWindow::Create(wxMDIParentFrame* parent, rootmap::IViewCoordinator* viewCoordinator, rootmap::ScoreboardCoordinator& scoreboardcoordinator, const ViewInformation& viewInfo, wxWindowID /* id */, const wxString& /* caption */, const wxPoint& /* pos */, const wxSize& /* size */, long /* style */)
{
    ////@begin RmViewWindow member initialisation
    m_scrolledWindow = __nullptr;
    ////@end RmViewWindow member initialisation
    m_coordinator = viewCoordinator; // may have been default constructed

    SetParent(parent);
    CreateControls(viewCoordinator, scoreboardcoordinator, viewInfo);
    Centre();

    m_baseTitle = GetTitle();
    wxString t = m_baseTitle;
    t << " | " << rootmap::Utility::ViewDirectionToString(viewInfo.GetViewDirection());
    SetTitle(t);

    RmAssert(__nullptr != m_scrolledWindow, "Scrolled Window not successfully created in CreateControls()");
    //m_scrolledWindow->Create(viewInfo);

    const int numaccelerators = 4;
    wxAcceleratorEntry accelerators[numaccelerators];
    accelerators[0].Set(wxACCEL_NORMAL, WXK_NUMPAD_ADD, XRCID("rmID_VIEWMENU_ZOOMINITEM"));
    accelerators[1].Set(wxACCEL_NORMAL, '+', XRCID("rmID_VIEWMENU_ZOOMINITEM"));

    accelerators[2].Set(wxACCEL_NORMAL, WXK_NUMPAD_SUBTRACT, XRCID("rmID_VIEWMENU_ZOOMOUTITEM"));
    accelerators[3].Set(wxACCEL_NORMAL, '-', XRCID("rmID_VIEWMENU_ZOOMOUTITEM"));

    wxAcceleratorTable accel(numaccelerators, accelerators);
    SetAcceleratorTable(accel);
    return true;
}

/*!
 * Control creation for RmViewWindow
 */

void RmViewWindow::CreateControls(rootmap::IViewCoordinator* viewCoordinator, rootmap::ScoreboardCoordinator& scoreboardcoordinator, const ViewInformation& viewInfo)
{
    ////@begin RmViewWindow content construction
    if (!wxXmlResource::Get()->LoadObject(this, GetParent(), wxT("rmID_VIEW_WINDOWFRAME"), wxT("wxMDIChildFrame")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_scrolledWindow = XRCCTRL(*this, "rmID_VIEW_SCROLLEDWINDOW", RmScrolledWindow);
    ////@end RmViewWindow content construction

    int width, height;
    GetClientSize(&width, &height);
    m_scrolledWindow->SetSize(width, height);
    m_scrolledWindow->Create(this, viewCoordinator, scoreboardcoordinator, viewInfo);

    // Create custom windows not generated automatically here.
    ////@begin RmViewWindow content initialisation
    ////@end RmViewWindow content initialisation
}

/*!
 * wxEVT_MENU_OPEN event handler for rmID_VIEW_WINDOWFRAME
 */

void RmViewWindow::OnMenuOpen(wxMenuEvent& event)
{
    wxMenu* menu = event.GetMenu();
    if (__nullptr != menu)
    {
        // What a lot fussing about to figure out if the menu of the event is
        // the View menu!  Can't just use the menu id as that isn't stored with
        // the menu. The only way seems to be to check if the Id of the first
        // item on the menu is the "New View" menu item.
        wxMenuItem* item0 = menu->FindItemByPosition(0);
        int item0id = item0->GetId();
        int itemNewViewId = XRCID("rmID_VIEWMENU_NEWVIEWITEM");

        if (item0id == itemNewViewId)
        {
            const ViewInformation& view_info = GetViewInformation();

            int current_id = 0;
#define ENABLE_ITEM(itemid, en_cond) \
            menu->Enable(XRCID(#itemid), en_cond)

#define CHECK_ENABLE_ITEM(itemid, en_cond, ck_cond) \
            current_id=XRCID(#itemid); \
            menu->Enable(current_id, en_cond); \
            menu->Check(current_id, ck_cond)


            CHECK_ENABLE_ITEM(rmID_VIEWMENU_FRONTVIEWITEM, true, (rootmap::vFront == view_info.GetViewDirection()));
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_SIDEVIEWITEM, true, (rootmap::vSide == view_info.GetViewDirection()));
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_TOPVIEWITEM, true, (rootmap::vTop == view_info.GetViewDirection()));
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_REPEATITEM, true, (view_info.DoesRepeat()));
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_WRAPITEM, true, (view_info.DoesWrap()));
            ENABLE_ITEM(rmID_VIEWMENU_ZOOMINITEM, true);
            ENABLE_ITEM(rmID_VIEWMENU_ZOOMOUTITEM, true);
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_BOUNDARIESITEM, true, (view_info.DoesBoundaries()));
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_BOXESITEM, true, (view_info.DoesBoxes()));
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_BOXCOLOURSITEM, true, (view_info.DoesBoxColours()));

            ENABLE_ITEM(rmID_VIEWMENU_PROCESSESITEM, true);
            ENABLE_ITEM(rmID_VIEWMENU_CHARACTERISTICSITEM, true);

            ENABLE_ITEM(rmID_VIEWMENU_SCOREBOARDSUBMENU, true);
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_SCOREBOARDSUBMENU_AIRITEM, false, false);
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_SCOREBOARDSUBMENU_LITTERITEM, false, false);
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_SCOREBOARDSUBMENU_SOILITEM, true, true);
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_SCOREBOARDSUBMENU_DRAINITEM, false, false);
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_SCOREBOARDSUBMENU_ALLITEM, true, false);

            ENABLE_ITEM(rmID_VIEWMENU_DETAILSITEM, false);
        }
    }
    else
    {
        //LOG_ERROR << "MenuEvent yielded NULL menu ?!?!";
    }
}

/*!
 * wxEVT_MENU_CLOSE event handler for rmID_VIEW_WINDOWFRAME
 */

void RmViewWindow::OnMenuClose(wxMenuEvent& event)
{
    // we ignore this event, but Skip()ing it doesn't seem to harm like Skip()ing
    // some other events is catastrophic
    ////@begin wxEVT_MENU_CLOSE event handler for rmID_VIEW_WINDOWFRAME in RmViewWindow.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_MENU_CLOSE event handler for rmID_VIEW_WINDOWFRAME in RmViewWindow. 
}

/*!
 * Should we show tooltips?
 */

bool RmViewWindow::ShowToolTips()
{
    return true;
}

wxBitmap RmViewWindow::GetBitmapResource(const wxString& name)
{
    // Bitmap retrieval
    ////@begin RmViewWindow bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
    ////@end RmViewWindow bitmap retrieval
}

/*
 *  MSA 09.09.23 Caller receives ownership of pointer to wxBitmap on the heap
 */
wxBitmap* RmViewWindow::GetBitmap()
{
    if (IsBeingDeleted()) return __nullptr;
    if (!IsVisible() || !IsShown() || !IsShownOnScreen()) return __nullptr;
    // MSA 09.09.23 Now returns a NEW copy of the video buffer;
    // hands pointer off to calling function.
    wxClientDC dc(this);
    wxSize sz = dc.GetSize();
    wxMemoryDC memDC;
    wxBitmap* bitmap = new wxBitmap(sz.GetWidth(), sz.GetHeight());
    memDC.SelectObject(*bitmap);
    memDC.Blit(0, 0, sz.GetWidth(), sz.GetHeight(), &dc, 0, 0);
    // Hand off this pointer to the calling class
    return bitmap;
}

/*!
 * Get icon resources
 */

wxIcon RmViewWindow::GetIconResource(const wxString& name)
{
    // Icon retrieval
    ////@begin RmViewWindow icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
    ////@end RmViewWindow icon retrieval
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_FRONTVIEWITEM
 */

void RmViewWindow::OnViewMenuFrontViewItemClick(wxCommandEvent& /* event */)
{
    m_scrolledWindow->UpdateViewDirection(rootmap::vFront);
    wxString t = m_baseTitle;
    t << " | Front";
    SetTitle(t);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_SIDEVIEWITEM
 */

void RmViewWindow::OnViewMenuSideViewItemClick(wxCommandEvent& /* event */)
{
    m_scrolledWindow->UpdateViewDirection(rootmap::vSide);
    wxString t = m_baseTitle;
    t << " | Side";
    SetTitle(t);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_TOPVIEWITEM
 */

void RmViewWindow::OnViewMenuTopViewItemClick(wxCommandEvent& /* event */)
{
    m_scrolledWindow->UpdateViewDirection(rootmap::vTop);
    wxString t = m_baseTitle;
    t << " | Top";
    SetTitle(t);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_REPEATITEM
 */

void RmViewWindow::OnViewMenuRepeatItemClick(wxCommandEvent& /* event */)
{
    m_scrolledWindow->UpdateRepeat(rootmap::Toggle);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_WRAPITEM
 */

void RmViewWindow::OnViewMenuWrapItemClick(wxCommandEvent& /* event */)
{
    m_scrolledWindow->UpdateWrap(rootmap::Toggle);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_ZOOMINITEM
 */

void RmViewWindow::OnViewMenuZoomInItemClick(wxCommandEvent& /* event */)
{
    m_scrolledWindow->ZoomIn();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_ZOOMOUTITEM
 */

void RmViewWindow::OnViewMenuZoomOutItemClick(wxCommandEvent& /* event */)
{
    m_scrolledWindow->ZoomOut();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_BOUNDARIESITEM
 */

void RmViewWindow::OnViewMenuBoundariesItemClick(wxCommandEvent& /* event */)
{
    m_scrolledWindow->UpdateBoundaries(rootmap::Toggle);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_BOXESITEM
 */

void RmViewWindow::OnViewMenuBoxesItemClick(wxCommandEvent& /* event */)
{
    m_scrolledWindow->UpdateBoxes(rootmap::Toggle);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_BOXCOLOURSITEM
 */

void RmViewWindow::OnViewMenuBoxColoursItemClick(wxCommandEvent& /* event */)
{
    m_scrolledWindow->UpdateBoxColours(rootmap::Toggle);
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_PROCESSESITEM
 */

void RmViewWindow::OnViewMenuProcessesItemClick(wxCommandEvent& /* event */)
{
    RmViewProcessSelectionDialog* window = new RmViewProcessSelectionDialog(this, rmID_VIEWPROCESSSELECTION_DIALOG, _("View Process Selection"));

    wxArrayString processNames;
    std::vector<ProcessIdentifier> allProcesses;
    m_coordinator->populateProcessStringArray(processNames, allProcesses, rootmap::ppsf_DrawingOnly);

    ProcessIdentifierSet display_process_ids;
    const ProcessArray& display_processes = GetViewInformation().GetDisplayProcesses();
    for (ProcessArray::const_iterator dpit(display_processes.begin());
        dpit != display_processes.end(); ++dpit)
    {
        display_process_ids.insert((*dpit)->GetProcessID());
    }

    //TODO: setup with current process ids
    window->Setup(this, processNames, allProcesses, display_process_ids);

    // We do not use the returnValue. The Cancel/OK/Apply is handled by the dialog.
    /*int returnValue =*/
    window->ShowModal();
    window->Destroy();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_CHARACTERISTICSITEM
 */

void RmViewWindow::OnViewMenuCharacteristicsItemClick(wxCommandEvent& /* event */)
{
    RmCharacteristicColourSelectionDialog* window = new RmCharacteristicColourSelectionDialog(this, rmID_CCSD_DIALOG, _("Characteristic Colour Selection"));
    window->Setup(this, GetViewInformation().GetCharacteristicColourInfo(), m_coordinator);

    // We do not use the returnValue. The Cancel/OK/Apply is handled by the dialog.
    /*int returnValue =*/
    window->ShowModal();
    window->Destroy();
}

/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_DETAILSITEM
 */

void RmViewWindow::OnViewMenuDetailsItemClick(wxCommandEvent& /* event */)
{
    RmAssert(false, "RmViewWindow::OnViewMenuDetailsItemClick not yet implemented");
}


/*!
 * wxEVT_ERASE_BACKGROUND event handler for rmID_VIEW_WINDOWFRAME
 */

void RmViewWindow::OnEraseBackground(wxEraseEvent& /* event */)
{
    // event IGNORED. We are buffering and will bitblit shortly
}

void RmViewWindow::PostTimestampNotification(const rootmap::PostOffice& /* postOffice */)
{
    m_scrolledWindow->PostTimestampNotification();
}

void RmViewWindow::DisconnectNotification()
{
    // No simulation-class members that need to be disconnected
    // (All member variables of RmScrolledWindow are app-class)
}

void RmViewWindow::Activate()
{
    wxMDIChildFrame::Activate();
}

bool RmViewWindow::Show(bool show)
{
    return wxMDIChildFrame::Show(show);
}

bool RmViewWindow::Destroy()
{
    // we are destroyed before we can call removeTableWindow, so that we need
    // to save a copy of the value of m_coordinator to use.
    rootmap::IViewCoordinator* old_coordinator = m_coordinator;
    RmViewWindow* old_this = this;

    bool ok = wxFrame::Destroy();
    if (ok)
    {
        old_coordinator->removeViewWindow(old_this);
    }
    return ok;
}

void RmViewWindow::UpdateCharacteristicColourInfo(rootmap::CharacteristicColours_t& cct)
{
    m_scrolledWindow->UpdateCharacteristicColourInfo(cct);
}

void RmViewWindow::UpdateProcesses(const ProcessIdentifierSet& display_process_ids)
{
    m_scrolledWindow->UpdateProcesses(display_process_ids);
}
