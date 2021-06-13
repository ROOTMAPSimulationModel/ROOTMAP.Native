/////////////////////////////////////////////////////////////////////////////
// Name:        RmScrolledWindow.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     24/12/2006 15:34:55
// RCS-ID:      
// Copyright:   ©2006 University of Tasmania
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "RmScrolledWindow.h"
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
////@end includes

#include "app/gui/common/IViewCoordinator.h"
#include "app/gui/ViewWindow/RmScrolledWindow.h"
#include "app/gui/ViewWindow/ScrolledWindowBuffer.h"
#include "app/gui/ViewWindow/ScrolledWindowHelper.h"
#include "app/gui/ViewWindow/ViewBufferStructures.h"

#include "core/utility/Utility.h"
#include "core/common/ExtraStuff.h"


//#define LOG_LOCAL
#if defined LOG_LOCAL
#define LOG_FUNCTION_LOCAL(fn) LOG_FUNCTION(fn)
#define LOG_SIMPLE_LOCAL(X) LOG_SIMPLE_LOCAL(X)
#else
#define LOG_FUNCTION_LOCAL(fn)
#define LOG_SIMPLE_LOCAL(X)
#endif

////@begin XPM images
////@end XPM images

/*!
 * RmScrolledWindow type definition
 */

IMPLEMENT_DYNAMIC_CLASS(RmScrolledWindow, wxScrolledWindow)

/*!
 * RmScrolledWindow event table definition
 */

    BEGIN_EVENT_TABLE(RmScrolledWindow, wxScrolledWindow)

    ////@begin RmScrolledWindow event table entries
    EVT_SIZE(RmScrolledWindow::OnSize)
    //EVT_PAINT( RmScrolledWindow::OnPaint )
    EVT_ERASE_BACKGROUND(RmScrolledWindow::OnEraseBackground)

    ////@end RmScrolledWindow event table entries

    END_EVENT_TABLE()

    using rootmap::DoubleRect;
using rootmap::ViewDirection;
using rootmap::ViewInformation;
using rootmap::IViewCoordinator;
using rootmap::ScrolledWindowBuffer;
using rootmap::ScoreboardCoordinator;
using rootmap::Utility::CSMin;
using rootmap::Utility::CSMax;
using rootmap::ViewDirectionBufferChangeInfo;
using rootmap::WrapRepeatBufferChangeInfo;


RootMapLoggerDefinition(RmScrolledWindow);


/*!
 * RmScrolledWindow constructors
 */

RmScrolledWindow::RmScrolledWindow()
    : m_buffer(__nullptr)
    , m_viewInformation(__nullptr)
    , m_viewCoordinator(__nullptr)
    , m_virtualBoundsPix(0, 0, 0, 0)
    , m_virtualBoundsSoil(0.0, 0.0, 0.0, 0.0)
    , m_scoreboardOffsetSoil(0.0, 0.0)
    , m_clientScoreboardOffsetSoil(0.0, 0.0)
    , m_clientOffsetPix(0, 0)
    , m_clientOffsetSoil(0.0, 0.0)
    , m_currentResizeInfo(__nullptr)
    , m_ignoreResizeCount(0)
{
    RootMapLoggerInitialisation("rootmap.RmScrolledWindow");
    RootMapLogTrace("RmScrolledWindow::RmScrolledWindow");
    Init();
}

RmScrolledWindow::RmScrolledWindow(wxWindow* parent, IViewCoordinator* viewCoordinator, ScoreboardCoordinator& scoreboardcoordinator, const ViewInformation& viewInfo, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : m_buffer(__nullptr)
    , m_viewInformation(__nullptr)
    , m_viewCoordinator(viewCoordinator)
    , m_virtualBoundsPix(0, 0, 0, 0)
    , m_virtualBoundsSoil(0.0, 0.0, 0.0, 0.0)
    , m_scoreboardOffsetSoil(0.0, 0.0)
    , m_clientScoreboardOffsetSoil(0.0, 0.0)
    , m_clientOffsetPix(0, 0)
    , m_clientOffsetSoil(0.0, 0.0)
    , m_currentResizeInfo(__nullptr)
{
    RootMapLoggerInitialisation("rootmap.RmScrolledWindow");
    RootMapLogTrace("RmScrolledWindow::RmScrolledWindow( wxWindow*, IViewCoordinator*, ScoreboardCoordinator&, const ViewInformation&, wxWindowID, const wxPoint&, const wxSize&, long)");
    Init();
    Create(parent, viewCoordinator, scoreboardcoordinator, viewInfo, id, pos, size, style);
}

/*!
 * RmScrolledWindow creator
 */

bool RmScrolledWindow::Create(wxWindow* parent, IViewCoordinator* viewCoordinator, ScoreboardCoordinator& scoreboardcoordinator, const ViewInformation& viewInfo, wxWindowID /* id */, const wxPoint& /* pos */, const wxSize& /* size */, long /* style */)
{
    m_viewCoordinator = viewCoordinator;

    SetParent(parent);
    CreateControls(scoreboardcoordinator, viewInfo);
    return true;
}

RmScrolledWindow::~RmScrolledWindow()
{
    if (m_viewInformation != __nullptr)
    {
        delete m_viewInformation;
        m_viewInformation = __nullptr;
    }
    if (m_currentResizeInfo != __nullptr)
    {
        delete m_currentResizeInfo;
        m_currentResizeInfo = __nullptr;
    }
    if (m_buffer != __nullptr)
    {
        delete m_buffer;
        m_buffer = __nullptr;
    }
}

/*!
 * Member initialisation
 */

void RmScrolledWindow::Init()
{
    ////@begin RmScrolledWindow member initialisation
    ////@end RmScrolledWindow member initialisation

    m_resizeTimer.SetResizeWindow(this);
}

/*!
 * Control creation for RmScrolledWindow
 */

void RmScrolledWindow::CreateControls(ScoreboardCoordinator& scoreboardcoordinator, const ViewInformation& viewInfo)
{
    ////@begin RmScrolledWindow content construction
    ////@end RmScrolledWindow content construction

    m_viewInformation = new ViewInformation(viewInfo);

    int width, height;
    GetClientSize(&width, &height);
    m_viewInformation->SetBounds(wxRect(0, 0, width, height));

    m_buffer = new ScrolledWindowBuffer(scoreboardcoordinator, *this, 24);
    m_buffer->Create(*m_viewInformation);

    EnableScrolling(true, true);
    SetScrollRate(ScrolledWindowBuffer::s_scrollAmountPixels, ScrolledWindowBuffer::s_scrollAmountPixels);
    //GetParent()->FitInside();

    UpdateVirtualSize(scoreboardcoordinator);

    // Create custom windows not generated automatically here.
    ////@begin RmScrolledWindow content initialisation
    ////@end RmScrolledWindow content initialisation
}

void RmScrolledWindow::OnDraw(wxDC& dc)
{
    if (__nullptr != m_currentResizeInfo)
    {
        return;
    }

    wxStopWatch stopwatch;
    stopwatch.Start(0);

    //::wxMilliSleep(500);
    // Does Nothing
    //wxScrolledWindow::OnDraw(dc);
    wxRect client_bounds_pix(GetClientBoundsPix());

    m_buffer->DrawContents(dc, client_bounds_pix, m_virtualBoundsPix, m_virtualBoundsSoil);
    //::wxMilliSleep(500);

    LOG_DEBUG << LINE_HERE << "DrawContents took " << stopwatch.Time() << "milliseconds";
}

void RmScrolledWindow::ScrollWindow(int dx, int dy, const wxRect* /*prect */)
{
    LOG_FUNCTION_LOCAL(RmScrolledWindow_ScrollWindow);

    // The inherited method just does the scroll bitmap thing which doesn't
    // really work for us so we Refresh()
    // wxScrolledWindow::ScrollWindow(dx, dy, prect);

    if (((0 != dx) || (0 != dy)) && (__nullptr != m_buffer))
    {
        int ppuX, ppuY, startX, startY;
        GetScrollPixelsPerUnit(&ppuX, &ppuY);
        GetViewStart(&startX, &startY);

        wxPoint viewStart(startX, startY);
        double scrollToX = ((ppuX * startX) + m_virtualBoundsPix.GetLeft()) / (m_viewInformation->GetScale());
        double scrollToY = ((ppuY * startY) + m_virtualBoundsPix.GetTop()) / (m_viewInformation->GetScale());
        rootmap::DoublePt scrollTo(scrollToX, scrollToY);

        // LOG_SIMPLE_LOCAL(viewStart);
        // LOG_SIMPLE_LOCAL(m_virtualBoundsPix);
        // LOG_SIMPLE_LOCAL(scrollTo);

        m_buffer->ScrollTo(scrollToX, scrollToY);

        m_viewCoordinator->updateViewInformation(*m_viewInformation);

        Refresh();
    }
}

/*!
 * Should we show tooltips?
 */

bool RmScrolledWindow::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap RmScrolledWindow::GetBitmapResource(const wxString& name)
{
    // Bitmap retrieval
    ////@begin RmScrolledWindow bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
    ////@end RmScrolledWindow bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon RmScrolledWindow::GetIconResource(const wxString& name)
{
    // Icon retrieval
    ////@begin RmScrolledWindow icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
    ////@end RmScrolledWindow icon retrieval
}

/*!
 * wxEVT_SIZE event handler for rmID_VIEW_SCROLLEDWINDOW
 */

void RmScrolledWindow::OnSize(wxSizeEvent& event)
{
    LOG_FUNCTION_LOCAL(RmScrolledWindow_OnSize);


    if ((__nullptr != m_buffer))// && (m_ignoreResizeCount<2))
    {
        wxString unow_str = wxDateTime::UNow().Format("OnSize ResizeTimer started at %H:%M:%S.%l", wxDateTime::GMT8);
        RootMapLogDebug(unow_str);
        // [Re]start the stopwatch. Apparently on Windoze timers are a
        // limited resource so this might fail. 
        bool success = m_resizeTimer.Start(600, wxTIMER_ONE_SHOT);

        RmAssert(success, "Could not start scrolled window resize timer");

        if (__nullptr == m_currentResizeInfo)
        {
            m_currentResizeInfo = new rootmap::ResizeBufferChangeInfo;

            // struct ViewBufferChangeInfo : public CCollaboratorInfo
            // set every time:
            //  wxRect    bci_BufferSize;

            m_currentResizeInfo->bci_Scale = GetViewInformation().GetScale();

            // may or may not need updating :
            m_currentResizeInfo->bci_ReferencePt = GetViewInformation().GetReferencePoint();

            // struct ScoreboardBufferChangeInfo : public ViewBufferChangeInfo

            // set every time:
            //  wxRect      bci_WindowBounds;

            m_buffer->GetScoreboardCoordinator().GetTotalScoreboardBounds(GetViewInformation().GetViewDirection(), &(m_currentResizeInfo->bci_ScoreboardBounds));

            // set every time:
            //  DoubleRect  bci_SoilFrame;
        }

        int width, height;
        GetClientSize(&width, &height);
        m_currentResizeInfo->bci_BufferSize.SetPosition(wxPoint(0, 0));
        m_currentResizeInfo->bci_BufferSize.SetSize(wxSize(width, height));
        m_currentResizeInfo->bci_SoilFrame = GetClientBoundsSoil();

        m_ignoreResizeCount = 0;
    }
    else
    {
        m_ignoreResizeCount += 1;
    }

    // we don't really handle this event per se, so allow it to go through
    // to the keeper
    event.Skip();
}

/*
 * Three Stages :
 * 1. MaintainBounds
 * 2. KeepScoreboardVisible
 * 3. MaintainFrames
 *
 * Darned wxWidgets uses scrolling/virtual coordinates entirely differently to
 * THINK Class Library. Whereas TCL had (0,0) wherever you told it (SetOrigin)
 * wx uses the very topleft corner as (0,0).
 *
 * Consider the following, where each line/column of text is a scroll unit's
 * worth.
 *
 * (0,0) unscrolled
 * + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
 * |                                                                         |
 *
 * |                                                                         |
 *
 * |                                                                         |
 *
 * |                 (21,8)   "Client Window"                                |
 *                      +------------------------------+
 * |                    | +--------+                   |                     |
 *                      |/|        |                   |
 * |                    / |        |                   |                     |
 *                     /| |        |                   |
 * |           Our (0,0)| |        |                   |                     |
 *                      | |        |                   |
 * |                    | |        |                   |                     |
 *                      +-+--------+-------------------+
 * |                      |        |                                         |
 *                        +--------+
 * |                      Scoreboard                                         |
 *
 * |                                                                         |
 *
 * |                                                                         |
 *
 * |                                                                         |
 *
 * |                                                                         |
 * + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
 *                             Virtual Size
 *
 * wherein our (0,0) is actually (23,9) units in, and the Client window is
 * currently scrolled to (21,8).  But when the already-prepared wxDC is passed
 * to our Draw(wxDC&) function, the Device Origin is correctly set to the
 * corresponding (-21*PixelsPerUnit, -8*PixelsPerUnit)
 *
 * Therefore, drawing is handled ok it seems. This function, however, needs to
 * account for the funny business.
 *
 * It is called during CreateControls, SetViewDirection and OnSize.
 *
 * Variable naming guide:
 * 1st :
 * virtual or scoreboard or client
 *
 * 2nd :
 * bounds (for rectangles)
 * offset (for points)
 * size   (for sizes)
 *
 * 3rd :
 * soil, pix, scru (soil, pixels, scroll units)
 *
 * eg. client_size_soil is the size of the visible window in soil units (cm)
 */
static DoubleRect GetVirtualSoilBounds(const DoubleRect& scoreboardBoundsSoil, const rootmap::DoublePt& clientSizeSoil, const rootmap::DoublePt& minVisibleScoreboard);

DoubleRect GetVirtualSoilBounds(const DoubleRect& scoreboardBoundsSoil, const rootmap::DoublePt& clientSizeSoil, const rootmap::DoublePt& minVisibleScoreboard)
{
    DoubleRect virtual_bounds_soil;
    virtual_bounds_soil.left = scoreboardBoundsSoil.left - clientSizeSoil.h + minVisibleScoreboard.h;
    virtual_bounds_soil.right = scoreboardBoundsSoil.right + clientSizeSoil.h - minVisibleScoreboard.h;
    virtual_bounds_soil.top = scoreboardBoundsSoil.top - clientSizeSoil.v + minVisibleScoreboard.v;
    virtual_bounds_soil.bottom = scoreboardBoundsSoil.bottom + clientSizeSoil.v - minVisibleScoreboard.v;
    return virtual_bounds_soil;
}

void RmScrolledWindow::UpdateVirtualSize(ScoreboardCoordinator& scoord)
{
    LOG_FUNCTION_LOCAL(RmScrolledWindow_UpdateVirtualSize);
    //wxLogMessage(wxT("RmScrolledWindow::UpdateVirtualSize (this=%p)"), this);

    ViewDirection view_dir = GetViewInformation().GetViewDirection();
    double soil_scale = GetViewInformation().GetScale();

    // 
    // 
    DoubleRect scoreboard_bounds_soil;
    scoord.GetTotalScoreboardBounds(view_dir, &scoreboard_bounds_soil);
    ScrolledWindowHelper helper(view_dir, soil_scale, ScrolledWindowBuffer::s_scrollAmountPixels);
    //wxLogMessage(wxT("scoreboard_bounds_soil=%s"),ToString(scoreboard_bounds_soil).c_str());
    LOG_SIMPLE_LOCAL(scoreboard_bounds_soil);

    /////////////////
    // The scoreboard visibilty is a function of panorama units, and the units
    // are a function of how large the scoreboard is - if the scoreboard is smaller
    // than the panorama units.
    //
    // Minimum amount of scoreboard that should be visible, in soil units
    const double min_visible_scoreboard_ = 2 * ScrolledWindowBuffer::s_scrollAmountPixels / soil_scale;

    // Minimum amount of scoreboard that should be visible, for each axis (with
    // a smaller allowance if the scoreboard is smaller than the overall value)
    rootmap::DoublePt min_visible_scoreboard_size_soil;
    min_visible_scoreboard_size_soil.h = CSMin(min_visible_scoreboard_, fabs((double)(scoreboard_bounds_soil.right - scoreboard_bounds_soil.left)));
    min_visible_scoreboard_size_soil.v = CSMin(min_visible_scoreboard_, fabs((double)(scoreboard_bounds_soil.bottom - scoreboard_bounds_soil.top)));
    LOG_SIMPLE_LOCAL(min_visible_scoreboard_size_soil);

    /////////////////
    // client frame in pixels
    int client_height, client_width;
    GetClientSize(&client_width, &client_height);
    /////////////////
    // client frame size in soil units
    rootmap::DoublePt client_size_soil(helper.ScrollPixel2Soil(client_width), helper.ScrollPixel2Soil(client_height));
    LOG_SIMPLE_LOCAL(client_size_soil);
    ////////////////
    // client frame size in scroll units
    // wxPoint client_size_scru(client_width/ScrolledWindowBuffer::s_scrollAmountPixels, client_height/ScrolledWindowBuffer::s_scrollAmountPixels);

    /////////////////
    // virtual scrollable size frame in soil units
    m_virtualBoundsSoil = GetVirtualSoilBounds(scoreboard_bounds_soil, client_size_soil, min_visible_scoreboard_size_soil);
    LOG_SIMPLE_LOCAL(m_virtualBoundsSoil);

#if defined ROOTMAP_VIEW_FROM_TOP_FIXME
    if (rootmap::vTop == view_dir)
    {
        /////////////////
        //    These calculations for top and bottom bounds for vTop are
        //    beyond normal logic. But they work.
        m_virtualBoundsSoil.top = 2 * soil_client_height - min_visible_scoreboard_size_soil.h;
        m_virtualBoundsSoil.bottom = min_visible_scoreboard_size_soil.h - scoreboard_bounds_soil.top;
    }
#endif // #if defined ROOTMAP_VIEW_FROM_TOP_FIXME

    /////////////////
    // set virtual scrollable size (pixels)
    helper.Soil2ScrollPixelRect(m_virtualBoundsSoil, m_virtualBoundsPix);
    SetVirtualSize(m_virtualBoundsPix.GetSize());
    LOG_SIMPLE_LOCAL(m_virtualBoundsPix);

    // scroll to the scroll-unit version of the reference point
    rootmap::DoublePt virtual_offset_soil(m_virtualBoundsSoil.left, m_virtualBoundsSoil.top);
    LOG_SIMPLE_LOCAL(virtual_offset_soil);

    m_clientScoreboardOffsetSoil = (GetViewInformation().GetReferencePoint().Abs());
    LOG_SIMPLE_LOCAL(m_clientScoreboardOffsetSoil);

    m_clientOffsetSoil = virtual_offset_soil;
    m_clientOffsetSoil.Offset(m_clientScoreboardOffsetSoil);
    m_clientOffsetSoil.Abs();
    LOG_SIMPLE_LOCAL(m_clientOffsetSoil);

    helper.Soil2ScrollPixelPoint(m_clientOffsetSoil, m_clientOffsetPix);
    LOG_SIMPLE_LOCAL(m_clientOffsetPix);

    wxPoint client_offset_scru;
    helper.Soil2ScrollUnitPoint(m_clientOffsetSoil, client_offset_scru);
    LOG_SIMPLE_LOCAL(client_offset_scru);

    //Scroll(0,0);
    Scroll(client_offset_scru.x, client_offset_scru.y);

    /////////////////
    // Offset of Scoreboard from topleft corner in soil units is
    //      abs(m_virtualBoundsSoil.top,m_virtualBoundsSoil.left)
    //DoublePt virtual_offset_soil(m_virtualBoundsSoil.left, m_virtualBoundsSoil.top);
    /////////////////
    // set the scoreboard's offset in scroll units
    //
    // HOWEVER nobody seems to care about this, it isn't used, no need to calculate it
    //m_scoreboardOffsetSoil = virtual_offset_soil;
    //m_scoreboardOffsetSoil.Abs();
    //LOG_SIMPLE_LOCAL(m_scoreboardOffsetSoil);

    // ////////////////////////////////////////
    // KeepScoreboardVisible
    // Keeps the scoreboard visible by the lesser of :
    // 1. two panorama units OR
    // 2. the scoreboard width/height.
    // 
    // Modifications    :
    // 95.07.21    Checks for if the scoreboard has gone above or to the left of the frame,
    //         as is the case when a resize with rescale has occured, and the scoreboard
    //         was at the very top left of the window before this down-scaling.
    // 95.07.28    If viewing from the top then the checking of the scoreboard_bounds_soil'
    //         top and bottom need to be reversed.
    // 95.08.31 Altered usage of scoreboard_bounds_soil, vpScoreboardSoilFrame & vpSoilBounds
    // 99.08.19 removed use of vpSoilBounds (after being unused for years)
    // ////////////////////////////////////////


    // double min_ref_h = CSMax((scoreboard_bounds_soil.left+min_visible_scoreboard_size_soil.h), soil_ref_pt.h);
    // double max_ref_h = CSMin((scoreboard_bounds_soil.right-min_visible_scoreboard_size_soil.h), soil_ref_pt.h);

    // double compareTop = scoreboard_bounds_soil.top+min_visible_scoreboard_size_soil.v;
    // double compareBottom = scoreboard_bounds_soil.bottom-min_visible_scoreboard_size_soil.v;
#if defined ROOTMAP_VIEW_FROM_TOP_FIXME
    if (rootmap::vTop == view_dir)
    {
        compareTop = scoreboard_bounds_soil.bottom;
        compareBottom = scoreboard_bounds_soil.top;
    }
#endif // #if defined ROOTMAP_VIEW_FROM_TOP_FIXME

    // double min_ref_v = CSMax(compareTop, soil_ref_pt.v);
    // double max_ref_v = CSMin(compareBottom, soil_ref_pt.v);

#if defined ROOTMAP_TODO
    // ////////////////////////////////////////
    // MaintainFrames()
    // 1.    Sets & Offsets the vpSoilFrame

    // 3.    Calculates vpScoreboardSoilFrame

    // 5.    Explicit brutal setting of vOrigin and hOrigin. This is a much more direct method of achieving
    //     achieving what we want than overriding CalcFrame() and calling that. Almost no functions
    //     call CalcFrame(), and certainly none that will do anything for _US_.
    // 6.    Explicit and brutal setting of aperture. This is not nice and not how i wanted it, but necessary.
    // ////////////////////////////////////////
    /*** Panorama's position ***
    Convert the vpReferenceIndex projection to panorama units and transfer */
    wxPoint newPos;
    wxPoint refPos;
    helper.Soil2ScrollUnitPoint(soil_ref_pt, client_ref_pt);
    newPos = refPos;

    int scroll_pos_h =
        Scroll(-2, -2);

#if defined ROOTMAP_VIEW_FROM_TOP_FIXME
    if (view_dir == vTop)
    {
        newPos.h = refPos.h;
        newPos.v = -refPos.v;
    }
#endif // #if defined ROOTMAP_VIEW_FROM_TOP_FIXME

    CPanorama::SetPosition(&newPos);
    CPane::SetFrameOrigin(newPos.h*hScale, newPos.v*vScale);
    hOrigin = frame.left; vOrigin = frame.top;
    aperture.bottom = frame.top + (aperture.bottom - aperture.top);
    aperture.top = frame.top;
    aperture.right = frame.left + (aperture.right - aperture.left);
    aperture.left = frame.left;

    /* Round the ReferenceIndex off to the PanoramaUnit */
    helper.ScrollUnit2SoilPoint(&refPos, &ref);
    SetReferencePt(ref);

    /*** vpSoilFrame ***
    = vpSoilFrame(zero'ed) offset by vpReferenceIndex */
    SetDoubleRect(&vpSoilFrame, ref.h, ref.v, soil_client_width + ref.h, soil_client_height + ref.v);

    /* Finally, make sure that the scrollpane's scrollbars are up to date */
    if (itsScrollPane != __nullptr)
    {
        itsScrollPane->AdjustScrollMax();
        itsScrollPane->Calibrate();
    }
#endif // #if defined ROOTMAP_TODO
}

wxRect RmScrolledWindow::GetClientBoundsPix() const
{
    int height, width;
    GetClientSize(&width, &height);
    wxRect client_bounds_pix(0, 0, width, height);

    int ppuX, ppuY, startX, startY;
    GetScrollPixelsPerUnit(&ppuX, &ppuY);
    GetViewStart(&startX, &startY);
    wxPoint client_offset_pix(ppuX * startX, ppuY * startY);

    LOG_DEBUG << "GetClientBoundsPix() (before offset) client_bounds_pix=TODO log rectangle coords";// << client_bounds_pix;

    client_bounds_pix.SetPosition(client_offset_pix);

    LOG_DEBUG << "GetClientBoundsPix() (after offset) client_bounds_pix=TODO log rectangle coords";// << ToString(client_bounds_pix).c_str();
    LOG_DEBUG << "GetClientBoundsPix() client_offset_pix=TODO log rectangle coords";// << ToString(client_offset_pix).c_str();

    return client_bounds_pix;
}

DoubleRect RmScrolledWindow::GetClientBoundsSoil() const
{
    wxRect client_bounds_pix = GetClientBoundsPix();
    // 
    DoubleRect client_bounds_soil;
    ScrolledWindowHelper helper(GetViewInformation().GetViewDirection(), GetViewInformation().GetScale(), ScrolledWindowBuffer::s_scrollAmountPixels);
    helper.ScrollPixel2SoilRect(client_bounds_pix, client_bounds_soil);
    LOG_DEBUG << "GetClientBoundsSoil() client_bounds_pix=TODO log rectangle coords";//  << ToString(client_bounds_pix).c_str() ;
    LOG_DEBUG << "GetClientBoundsSoil() client_bounds_soil=TODO log rectangle coords";// << ToString(client_bounds_soil).c_str();

    return client_bounds_soil;
}

rootmap::BufferChangeResponse RmScrolledWindow::DoResize()
{
    rootmap::BufferChangeResponse response = rootmap::bcr_OK;
    if ((__nullptr != m_currentResizeInfo) && (__nullptr != m_buffer))
    {
        if (__nullptr != m_currentResizeInfo)
        {
            //wxSize slightly_bigger(m_currentResizeInfo->bci_BufferSize.GetSize());
            //slightly_bigger += wxSize(1,1);
            //m_currentResizeInfo->bci_BufferSize.SetSize(slightly_bigger);

            response = m_buffer->Resize(m_currentResizeInfo);
            delete m_currentResizeInfo;
            m_currentResizeInfo = __nullptr;

            if (rootmap::bcr_OK == response)
            {
                UpdateVirtualSize(m_buffer->GetScoreboardCoordinator());

                Refresh();

                m_viewCoordinator->updateViewInformation(*m_viewInformation);
            }
        }

        // stop the timer
        m_resizeTimer.Stop();
    }

    return response;
}

void RmScrolledWindow::PostTimestampNotification()
{
    m_buffer->PostTimestampNotification();
    Refresh();
}

void RmScrolledWindow::ZoomIn()
{
    Zoom(+0.1);
}

void RmScrolledWindow::ZoomOut()
{
    Zoom(-0.1);
}

void RmScrolledWindow::Zoom(double deltaZ)
{
    ViewInformation& view_info = GetViewInformation();

    double zoomratio_org = view_info.GetZoomRatio();
    double zoomratio_new = zoomratio_org + deltaZ;

    // shouldn't reduce scale to below 10% (1/10th of original seems reasonable...)
    if (zoomratio_new < 0.1)
    {
        return;
    }

    // necessary for ReferencePt and other implied access
    view_info.SetZoomRatio(zoomratio_new);

    delete m_currentResizeInfo;
    m_currentResizeInfo = new rootmap::ResizeBufferChangeInfo;

    int width, height;
    GetClientSize(&width, &height);
    m_currentResizeInfo->bci_BufferSize.SetPosition(wxPoint(0, 0));
    m_currentResizeInfo->bci_BufferSize.SetSize(wxSize(width, height));
    m_currentResizeInfo->bci_Scale = view_info.GetScale();
    m_currentResizeInfo->bci_ReferencePt = view_info.GetReferencePoint();
    m_buffer->GetScoreboardCoordinator().GetTotalScoreboardBounds(GetViewInformation().GetViewDirection(), &(m_currentResizeInfo->bci_ScoreboardBounds));
    m_currentResizeInfo->bci_SoilFrame = GetClientBoundsSoil();

    rootmap::BufferChangeResponse response = DoResize();
    if (response > rootmap::bcr_OK)
    {
        // if this failed, revert to the original zoomration
        view_info.SetZoomRatio(zoomratio_org);
    }
    else
    {
        // otherwise, update the default
        m_viewCoordinator->updateViewInformation(*m_viewInformation);
    }
}


void RmScrolledWindow::UpdateViewDirection(ViewDirection viewDir)
{
    ViewInformation& view_info = GetViewInformation();
    ViewDirection old_direction = view_info.GetViewDirection();

    // necessary for ReferencePt and other implied access
    view_info.SetViewDirection(viewDir);

    ViewDirectionBufferChangeInfo* info = new ViewDirectionBufferChangeInfo;
    info->bci_BufferSize = GetClientBoundsPix();
    info->bci_Scale = view_info.GetScale();
    info->bci_ReferencePt = view_info.GetReferencePoint();
    info->bci_ScoreboardBounds = DoubleRect(0.0, 0.0, 0.0, 0.0);
    info->bci_SoilFrame = GetClientBoundsSoil();
    info->bci_VLayers = __nullptr;
    info->bci_HLayers = __nullptr;

    rootmap::BufferChangeResponse response = m_buffer->SetViewDirection(viewDir, info);
    if (response > rootmap::bcr_OK)
    {
        view_info.SetViewDirection(old_direction);
    }
    else
    {
        m_viewCoordinator->updateViewInformation(*m_viewInformation);
    }

    Refresh();
}

void RmScrolledWindow::UpdateBoundaries(rootmap::BooleanStateChange changeBoundaries)
{
    ViewInformation& view_info = GetViewInformation();
    bool did_boundaries = view_info.DoesBoundaries();
    bool should_do_boundaries = rootmap::Utility::NewBooleanState(changeBoundaries, did_boundaries);

    rootmap::BufferChangeResponse change_response = m_buffer->SetBoundaries(should_do_boundaries);
    if (rootmap::bcr_OK == change_response)
    {
        view_info.SetBoundaries(should_do_boundaries);
        m_viewCoordinator->updateViewInformation(*m_viewInformation);
        Refresh();
    }
}


void RmScrolledWindow::UpdateBoxes(rootmap::BooleanStateChange changeBoxes)
{
    ViewInformation& view_info = GetViewInformation();
    bool did_boxes = view_info.DoesBoxes();
    bool should_do_boxes = rootmap::Utility::NewBooleanState(changeBoxes, did_boxes);

    rootmap::BufferChangeResponse change_response = m_buffer->SetBoxes(should_do_boxes);
    if (rootmap::bcr_OK == change_response)
    {
        view_info.SetBoxes(should_do_boxes);
        m_viewCoordinator->updateViewInformation(*m_viewInformation);
        Refresh();
    }
}


void RmScrolledWindow::UpdateBoxColours(rootmap::BooleanStateChange changeBoxColours)
{
    ViewInformation& view_info = GetViewInformation();
    bool did_box_colours = view_info.DoesBoxColours();
    bool should_do_box_colours = rootmap::Utility::NewBooleanState(changeBoxColours, did_box_colours);

    rootmap::BufferChangeResponse change_response = m_buffer->SetBoxColours(should_do_box_colours);
    if (rootmap::bcr_OK == change_response)
    {
        view_info.SetBoxColours(should_do_box_colours);
        m_viewCoordinator->updateViewInformation(*m_viewInformation);
        Refresh();
    }
}

void RmScrolledWindow::UpdateRepeat(rootmap::BooleanStateChange updateRepeat)
{
    ViewInformation& view_info = GetViewInformation();
    bool did_repeat = view_info.DoesRepeat();
    bool should_do_repeat = rootmap::Utility::NewBooleanState(updateRepeat, did_repeat);

    rootmap::BufferChangeResponse change_response = m_buffer->SetRepeat(should_do_repeat);
    if (rootmap::bcr_OK == change_response)
    {
        view_info.SetRepeat(should_do_repeat);
        m_viewCoordinator->updateViewInformation(*m_viewInformation);
        Refresh();
    }
}


void RmScrolledWindow::UpdateWrap(rootmap::BooleanStateChange updateWrap)
{
    ViewInformation& view_info = GetViewInformation();
    bool did_wrap = view_info.DoesWrap();
    bool should_do_wrap = rootmap::Utility::NewBooleanState(updateWrap, did_wrap);

    WrapRepeatBufferChangeInfo* info = new WrapRepeatBufferChangeInfo;
    info->bci_BufferSize = GetClientBoundsPix();
    info->bci_Scale = view_info.GetScale();
    info->bci_ReferencePt = view_info.GetReferencePoint();
    info->bci_ScoreboardBounds = DoubleRect(0.0, 0.0, 0.0, 0.0);
    info->bci_SoilFrame = GetClientBoundsSoil();

    rootmap::BufferChangeResponse change_response = m_buffer->SetWrap(should_do_wrap, info);
    if (rootmap::bcr_OK == change_response)
    {
        view_info.SetWrap(should_do_wrap);
        m_viewCoordinator->updateViewInformation(*m_viewInformation);
        Refresh();
    }
}

void RmScrolledWindow::UpdateCharacteristicColourInfo(rootmap::CharacteristicColours_t& cct)
{
    m_buffer->SetCharacteristicColourInfo(cct);
    m_viewCoordinator->updateViewInformation(*m_viewInformation);
    Refresh();
}

void RmScrolledWindow::UpdateProcesses(const rootmap::ProcessIdentifierSet& processIds)
{
    rootmap::ProcessArray display_processes;
    m_viewCoordinator->findProcessesByProcessId(processIds, display_processes);

    m_buffer->SetProcesses(display_processes);
    m_viewInformation->SetDisplayProcesses(display_processes);
    m_viewCoordinator->updateViewInformation(*m_viewInformation);
    Refresh();
}

RootMapLoggerDefinition(RmResizeTimer);

RmResizeTimer::RmResizeTimer()
    : m_owner(__nullptr)
{
    RootMapLoggerInitialisation("rootmap.RmResizeTimer");
}

void RmResizeTimer::Notify()
{
    if (__nullptr != m_owner)
    {
        wxDateTime unow = wxDateTime::UNow();
        wxString unow_str = unow.Format("RmResizeTimer::Notify() at %H:%M:%S.%l", wxDateTime::GMT9);
        RootMapLogDebug(unow_str);

        m_owner->DoResize();
    }
}

/*!
 * wxEVT_PAINT event handler for rmID_VIEW_SCROLLEDWINDOW
 */

void RmScrolledWindow::OnPaint(wxPaintEvent& /* event */)
{
    ////@begin wxEVT_PAINT event handler for rmID_VIEW_SCROLLEDWINDOW in RmScrolledWindow.
    // Before editing this code, remove the block markers.
    wxPaintDC dc(this);
    ////@end wxEVT_PAINT event handler for rmID_VIEW_SCROLLEDWINDOW in RmScrolledWindow. 
}

/*!
 * wxEVT_ERASE_BACKGROUND event handler for rmID_VIEW_SCROLLEDWINDOW
 */

void RmScrolledWindow::OnEraseBackground(wxEraseEvent& /* event */)
{
    //event.Skip();
}
