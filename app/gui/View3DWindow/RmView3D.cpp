/////////////////////////////////////////////////////////////////////////////
// Name:        RmView3D.cpp
// Purpose:     
// Author:      RvH
// Created:     14/03/2008 22:27:33
// Modified:    $Date: 2009-07-07 02:40:29 +0800 (Tue, 07 Jul 2009) $
// Version:     $Revision: 67 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "RmView3D.h"
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
#include "RmGLCanvas.h"
////@end includes

#include "app/gui/common/IView3DCoordinator.h"
#include "simulation/process/common/Process.h"
#include "app/gui/ViewCommon/View3DInformation.h"
#include "app/gui/View3DWindow/RmView3D.h"
#include "app/gui/ViewProcessSelection/RmViewProcessSelectionDialog.h"
#include "simulation/common/Types.h"

using rootmap::ProcessIdentifier;
using rootmap::ProcessIdentifierSet;
using rootmap::ProcessArray;

////@begin XPM images
////@end XPM images


/*!
 * RmView3D type definition
 */

IMPLEMENT_CLASS(RmView3D, wxMDIChildFrame)


/*!
 * RmView3D event table definition
 */

    BEGIN_EVENT_TABLE(RmView3D, wxMDIChildFrame)

    ////@begin RmView3D event table entries
    EVT_MENU_OPEN(RmView3D::OnMenuOpen)

    EVT_MENU(XRCID("rmID_VIEWMENU_FRONTVIEWITEM"), RmView3D::OnViewMenuFrontViewItemClick)

    EVT_MENU(XRCID("rmID_VIEWMENU_SIDEVIEWITEM"), RmView3D::OnViewMenuSideViewItemClick)

    EVT_MENU(XRCID("rmID_VIEWMENU_TOPVIEWITEM"), RmView3D::OnViewMenuTopViewItemClick)

    EVT_MENU(XRCID("rmID_VIEWMENU_ZOOMINITEM"), RmView3D::OnViewMenuZoomInItemClick)

    EVT_MENU(XRCID("rmID_VIEWMENU_ZOOMOUTITEM"), RmView3D::OnViewMenuZoomOutItemClick)

    EVT_MENU(XRCID("rmID_VIEWMENU_BOUNDARIESITEM"), RmView3D::OnViewMenuBoundariesItemClick)

    EVT_MENU(XRCID("rmID_VIEWMENU_BOXESITEM"), RmView3D::OnViewMenuBoxesItemClick)

    EVT_MENU(XRCID("rmID_VIEWMENU_BOXCOLOURSITEM"), RmView3D::OnViewMenuBoxColoursItemClick)

    EVT_MENU(XRCID("rmID_VIEWMENU_PROCESSESITEM"), RmView3D::OnViewMenuProcessesItemClick)

    EVT_MENU(XRCID("rmID_VIEWMENU_DETAILSITEM"), RmView3D::OnViewMenuDetailsItemClick)

    ////@end RmView3D event table entries

    END_EVENT_TABLE()

    RootMapLoggerDefinition(RmView3D);

/*!
 * RmView3D constructors
 */

RmView3D::RmView3D()
    : m_glCanvas(__nullptr)
    , m_coordinator(__nullptr)
{
    Init();
    RootMapLoggerInitialisation("rootmap.RmView3D");
}

RmView3D::RmView3D(wxMDIParentFrame* parent, rootmap::IView3DCoordinator* viewCoordinator, rootmap::ScoreboardCoordinator& scoreboardcoordinator, const rootmap::View3DInformation& viewInfo, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
    : m_glCanvas(__nullptr)
    , m_coordinator(__nullptr)
{
    Init();
    RootMapLoggerInitialisation("rootmap.RmView3D");
    Create(parent, viewCoordinator, scoreboardcoordinator, viewInfo, id, caption, pos, size, style);
}


/*!
 * RmView3D creator
 */

bool RmView3D::Create(wxMDIParentFrame* parent, rootmap::IView3DCoordinator* viewCoordinator, rootmap::ScoreboardCoordinator& scoreboardcoordinator, const rootmap::View3DInformation& viewInfo, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
{
    ////@begin RmView3D creation
    wxMDIChildFrame::Create(parent, id, caption, pos, size, style);

    m_coordinator = viewCoordinator;

    CreateControls(viewCoordinator, scoreboardcoordinator, viewInfo);
    Centre();
    ////@end RmView3D creation
    return true;
}


/*!
 * RmView3D destructor
 */

RmView3D::~RmView3D()
{
    ////@begin RmView3D destruction
    delete m_glCanvas;
    m_glCanvas = __nullptr;
    ////@end RmView3D destruction
}


/*!
 * Member initialisation
 */

void RmView3D::Init()
{
    ////@begin RmView3D member initialisation
    m_glCanvas = __nullptr;
    ////@end RmView3D member initialisation
}


/*!
 * Control creation for RmView3D
 */

void RmView3D::CreateControls(rootmap::IView3DCoordinator* viewCoordinator, rootmap::ScoreboardCoordinator& scoreboardcoordinator, const rootmap::View3DInformation& viewInfo)
{
    ////@begin RmView3D content construction
    RmView3D* itemMDIChildFrame1 = this;

    m_glCanvas = new RmGLCanvas(itemMDIChildFrame1, viewCoordinator, scoreboardcoordinator, viewInfo, rmID_VIEW_GLCANVAS, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);

    // Connect events and objects
    //m_glCanvas->Connect(rmID_VIEW_GLCANVAS, wxEVT_MOUSEWHEEL, wxMouseEventHandler(RmGLCanvas::OnMouseWheel), __nullptr, this);
    ////@end RmView3D content construction

    // Create custom windows not generated automatically here.
    ////@begin RmView3D content initialisation
    ////@end RmView3D content initialisation
}


/*!
 * Should we show tooltips?
 */

bool RmView3D::ShowToolTips()
{
    return true;
}


wxBitmap RmView3D::GetBitmapResource(const wxString& name)
{
    // Bitmap retrieval
    ////@begin RmViewWindow bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
    ////@end RmViewWindow bitmap retrieval
}

/*
 *  Get a copy of the view's bitmap
 *  MSA 09.09.23 Caller receives ownership of pointer to wxBitmap on the heap
 */
wxBitmap* RmView3D::GetBitmap()
{
    if (IsBeingDeleted()) return __nullptr;
    if (!IsVisible() || !IsShown() || !IsShownOnScreen()) return __nullptr;
    // MSA 09.09.23 Now returns a NEW copy of the video buffer;
    // hands pointer off to calling function.

    // Based on code by Robert J. Lang http://www.nabble.com/copying-wxGlCanvas-window-to-a-bitmap-in-wxMac-td5689068.html

    // Read the OpenGL image into a pixel array
    GLint view[4];
    glGetIntegerv(GL_VIEWPORT, view);
    // MSA 09.09.24 Conversion to C++ memory management
    GLvoid* pixels = new unsigned char[3 * view[2] * view[3]];
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_BACK_LEFT);
    glReadPixels(0, 0, view[2], view[3], GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // Put the image into a wxImage
    wxImage image((int)view[2], (int)view[3]);
    image.SetData((unsigned char*)pixels);
    image = image.Mirror(false);
    wxBitmap* bitmap = new wxBitmap(image);

    // Need to delete[] pixels here? Has image copied the pixels data
    // or referred to it?
    // MSA 09.10.20    TESTING delete[]ing pixels
    //delete[] pixels;

    // Hand off this pointer to the calling class
    return bitmap;
}

/*!
 * Get icon resources
 */

wxIcon RmView3D::GetIconResource(const wxString& name)
{
    // Icon retrieval
    ////@begin RmView3D icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
    ////@end RmView3D icon retrieval
}


/*!
 * wxEVT_MENU_OPEN event handler for rmID_VIEW_WINDOWFRAME
 */

void RmView3D::OnMenuOpen(wxMenuEvent& event)
{
    wxMenu* menu = event.GetMenu();
    if (__nullptr != menu)
    {
        // What a lot of fussing about to figure out if the menu of the event is
        // the View menu!  Can't just use the menu id as that isn't stored with
        // the menu. The only way seems to be to check if the Id of the first
        // item on the menu is the "New View" menu item.
        wxMenuItem* item0 = menu->FindItemByPosition(0);
        int item0id = item0->GetId();
        int itemNewViewId = XRCID("rmID_VIEWMENU_NEWVIEWITEM");
        if (item0id == itemNewViewId)
        {
            const rootmap::View3DInformation& view_info = m_glCanvas->GetViewInformation();

            int current_id = 0;
#define ENABLE_ITEM(itemid, en_cond) \
            menu->Enable(XRCID(#itemid), en_cond)

#define CHECK_ENABLE_ITEM(itemid, en_cond, ck_cond) \
            current_id=XRCID(#itemid); \
            menu->Enable(current_id, en_cond); \
            menu->Check(current_id, ck_cond)


            CHECK_ENABLE_ITEM(rmID_VIEWMENU_FRONTVIEWITEM, true, false);
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_SIDEVIEWITEM, true, false);
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_TOPVIEWITEM, true, false);
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_REPEATITEM, false, false);
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_WRAPITEM, false, false);
            ENABLE_ITEM(rmID_VIEWMENU_ZOOMINITEM, true);
            ENABLE_ITEM(rmID_VIEWMENU_ZOOMOUTITEM, true);
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_BOUNDARIESITEM, true, (view_info.DoesBoundaries()));
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_BOXESITEM, true, (view_info.DoesBoxes()));
            // MSA 09.11.25 Box colours not (yet?) supported for View3D
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_BOXCOLOURSITEM, false/*true*/, false/*(view_info.DoesBoxColours())*/);

            ENABLE_ITEM(rmID_VIEWMENU_PROCESSESITEM, true);
            ENABLE_ITEM(rmID_VIEWMENU_CHARACTERISTICSITEM, false);

            ENABLE_ITEM(rmID_VIEWMENU_SCOREBOARDSUBMENU, false);
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_SCOREBOARDSUBMENU_AIRITEM, false, false);
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_SCOREBOARDSUBMENU_LITTERITEM, false, false);
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_SCOREBOARDSUBMENU_SOILITEM, false, true);
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_SCOREBOARDSUBMENU_DRAINITEM, false, false);
            CHECK_ENABLE_ITEM(rmID_VIEWMENU_SCOREBOARDSUBMENU_ALLITEM, false, true);

            ENABLE_ITEM(rmID_VIEWMENU_DETAILSITEM, false);
        }
    }
    else
    {
        //LOG_ERROR << "MenuEvent yielded NULL menu ?!?!";
    }
}


/*
    MSA 09.11.25 TODO
    Implement undo functionality for these?
    I can see that being something users would want -
    the ability to revert to an earlier viewpoint...
*/

void RmView3D::OnViewMenuFrontViewItemClick(wxCommandEvent& /* event */)
{
    m_glCanvas->SetViewpoint(rootmap::vFront);
}

void RmView3D::OnViewMenuSideViewItemClick(wxCommandEvent& /* event */)
{
    m_glCanvas->SetViewpoint(rootmap::vSide);
}

void RmView3D::OnViewMenuTopViewItemClick(wxCommandEvent& /* event */)
{
    m_glCanvas->SetViewpoint(rootmap::vTop);
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_ZOOMINITEM
 */

void RmView3D::OnViewMenuZoomInItemClick(wxCommandEvent& /* event */)
{
    double x, y, z, scale;
    m_glCanvas->GetTranslation(x, y, z, scale);

    z += (scale); // Zoom in/out 1 unit at a time (...?)
    m_glCanvas->SetTranslation(x, y, z);
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_ZOOMOUTITEM
 */

void RmView3D::OnViewMenuZoomOutItemClick(wxCommandEvent& /* event */)
{
    double x, y, z, scale;
    m_glCanvas->GetTranslation(x, y, z, scale);

    z -= (scale); // Zoom in/out 1 unit at a time (...?)
    m_glCanvas->SetTranslation(x, y, z);
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_BOUNDARIESITEM
 */

void RmView3D::OnViewMenuBoundariesItemClick(wxCommandEvent& event)
{
    m_glCanvas->SetBoundaries(event.IsChecked());
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_BOXESITEM
 */

void RmView3D::OnViewMenuBoxesItemClick(wxCommandEvent& event)
{
    m_glCanvas->SetBoxes(event.IsChecked());
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_BOXCOLOURSITEM
 */

void RmView3D::OnViewMenuBoxColoursItemClick(wxCommandEvent& event)
{
    m_glCanvas->SetBoxColours(event.IsChecked());
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_PROCESSESITEM
 */

void RmView3D::OnViewMenuProcessesItemClick(wxCommandEvent& /* event */)
{
    RmViewProcessSelectionDialog* window = new RmViewProcessSelectionDialog(this, rmID_VIEWPROCESSSELECTION_DIALOG, _("View Process Selection"));

    //m_coordinator->populateProcessStringArray(processNames, allProcesses, ppsf_DrawingOnly);

    wxArrayString processNames;
    std::vector<ProcessIdentifier> allProcesses;
    m_coordinator->populateProcessStringArray(processNames, allProcesses, rootmap::ppsf_DrawingOnly);

    ProcessIdentifierSet display_process_ids;
    const ProcessArray& display_processes = m_glCanvas->GetViewInformation().GetDisplayProcesses();
    for (ProcessArray::const_iterator dpit(display_processes.begin());
        dpit != display_processes.end(); ++dpit)
    {
        display_process_ids.insert((*dpit)->GetProcessID());
    }

    window->Setup(this, processNames, allProcesses, display_process_ids);

    /*int returnValue =*/
    window->ShowModal();
    window->Destroy();
}


/*!
 * wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_DETAILSITEM
 */

void RmView3D::OnViewMenuDetailsItemClick(wxCommandEvent& event)
{
    ////@begin wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_DETAILSITEM in RmView3D.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_COMMAND_MENU_SELECTED event handler for rmID_VIEWMENU_DETAILSITEM in RmView3D. 
}

bool RmView3D::Destroy()
{
    // we are destroyed before we can call removeView3DWindow, so that we need
    // to save a copy of the value of m_coordinator to use.
    rootmap::IView3DCoordinator* old_coordinator = m_coordinator;
    RmView3D* old_this = this;

    bool ok = wxFrame::Destroy();
    if (ok)
    {
        old_coordinator->removeView3DWindow(old_this);
    }
    return ok;
}

void RmView3D::PostTimestampNotification(const rootmap::PostOffice& /* postOffice */)
{
    m_glCanvas->PostTimestampNotification();
}


void RmView3D::DisconnectNotification()
{
    // No simulation-class members that need to be disconnected
    // (All member variables of RmGLCanvas are app-class)
}

void RmView3D::UpdateProcesses(const rootmap::ProcessIdentifierSet& processes)
{
    // MSA 09.11.17 Implementing in-program Process selection for 3D View
    rootmap::ProcessArray pa;
    m_coordinator->findProcessesByProcessId(processes, pa);
    m_glCanvas->SetProcesses(pa);
}
