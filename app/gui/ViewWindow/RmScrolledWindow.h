/////////////////////////////////////////////////////////////////////////////
// Name:        RmScrolledWindow.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     24/12/2006 15:34:55
// RCS-ID:      
// Copyright:   ©2006 University of Tasmania
// Licence:     
//
// This class is contained in the actual view window and handles the scrolling
// around in the virtual space beyond the actual visible window. It replaces
// the VPanorama class from the MacOS CropSim
/////////////////////////////////////////////////////////////////////////////

#ifndef _RMSCROLLEDWINDOW_H_
#define _RMSCROLLEDWINDOW_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "RmScrolledWindow.h"
#endif

/*!
 * Includes
 */

 ////@begin includes
#include "wx/xrc/xmlres.h"
////@end includes

#include "app/gui/ViewWindow/ViewBuffer.h"

#include "simulation/common/Types.h"

#include "core/common/DoubleCoordinates.h"
#include "core/common/Types.h"
#include "core/log/Logger.h"

#include "wx/scrolwin.h"
/*!
 * Forward declarations
 */

 ////@begin forward declarations
class RmScrolledWindow;
////@end forward declarations
class RmResizeTimer;

namespace rootmap
{
    class ViewInformation;
    class IViewCoordinator;
    class ScrolledWindowBuffer;
    class ScoreboardCoordinator;
    struct ResizeBufferChangeInfo;
}

using rootmap::ViewInformation;
using rootmap::IViewCoordinator;
using rootmap::ViewDirection;
using rootmap::ScrolledWindowBuffer;
using rootmap::ScoreboardCoordinator;
using rootmap::DoubleRect;
/*!
 * Control identifiers
 */

 ////@begin control identifiers
#define rmID_VIEW_SCROLLEDWINDOW 10019
#define SYMBOL_RMSCROLLEDWINDOW_STYLE wxNO_BORDER|wxWANTS_CHARS|wxFULL_REPAINT_ON_RESIZE|wxHSCROLL|wxVSCROLL
#define SYMBOL_RMSCROLLEDWINDOW_IDNAME rmID_VIEW_SCROLLEDWINDOW
#define SYMBOL_RMSCROLLEDWINDOW_SIZE wxDefaultSize
#define SYMBOL_RMSCROLLEDWINDOW_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

 /**
  * RmResizeTimer class declaration
  */
#include "wx/timer.h"

class RmResizeTimer : public wxTimer
{
public:
    RmResizeTimer();

    void SetResizeWindow(RmScrolledWindow* owner)
    {
        m_owner = owner;
    }

    void Notify();

private:
    RootMapLoggerDeclaration();
    RmScrolledWindow* m_owner;
}; // class RmResizeTimer

/*!
 * RmScrolledWindow class declaration
 */

class RmScrolledWindow : public wxScrolledWindow
{
    DECLARE_DYNAMIC_CLASS(RmScrolledWindow)
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    RmScrolledWindow();
    RmScrolledWindow(wxWindow* parent, IViewCoordinator* viewCoordinator, ScoreboardCoordinator& scoreboardcoordinator, const ViewInformation& viewInfo, wxWindowID id = SYMBOL_RMSCROLLEDWINDOW_IDNAME, const wxPoint& pos = SYMBOL_RMSCROLLEDWINDOW_POSITION, const wxSize& size = SYMBOL_RMSCROLLEDWINDOW_SIZE, long style = SYMBOL_RMSCROLLEDWINDOW_STYLE);

    ~RmScrolledWindow();

    /// Creation
    bool Create(wxWindow* parent, IViewCoordinator* viewCoordinator, ScoreboardCoordinator& scoreboardcoordinator, const ViewInformation& viewInfo, wxWindowID id = SYMBOL_RMSCROLLEDWINDOW_IDNAME, const wxPoint& pos = SYMBOL_RMSCROLLEDWINDOW_POSITION, const wxSize& size = SYMBOL_RMSCROLLEDWINDOW_SIZE, long style = SYMBOL_RMSCROLLEDWINDOW_STYLE);

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls(ScoreboardCoordinator& scoreboardcoordinator, const ViewInformation& viewInfo);

    ////@begin RmScrolledWindow event handler declarations

    /// wxEVT_SIZE event handler for rmID_VIEW_SCROLLEDWINDOW
    void OnSize(wxSizeEvent& event);

    /// wxEVT_PAINT event handler for rmID_VIEW_SCROLLEDWINDOW
    void OnPaint(wxPaintEvent& event);

    /// wxEVT_ERASE_BACKGROUND event handler for rmID_VIEW_SCROLLEDWINDOW
    void OnEraseBackground(wxEraseEvent& event);

    ////@end RmScrolledWindow event handler declarations

    ////@begin RmScrolledWindow member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource(const wxString& name);

    /// Retrieves icon resources
    wxIcon GetIconResource(const wxString& name);
    ////@end RmScrolledWindow member function declarations

    /**
     * called during the window's OnPaint event handler. The DC is
     * already prepared
     */
    void OnDraw(wxDC& dc);

    /**
     * called by the window's OnScroll event handler
     */
    void ScrollWindow(int dx, int dy, const wxRect* prect);

    /// Should we show tooltips?
    static bool ShowToolTips();

    /**
     * accessor for this window's view information (this class is the owner)
     */
    const ViewInformation& GetViewInformation() const;

    /**
     * accessor for this window's view information (this class is the owner)
     */
    ViewInformation& GetViewInformation();

    /**
     * Retrieves the visible bounds of the scrolled window in pixels
     */
    wxRect GetClientBoundsPix() const;

    /**
     * Retrieves the visible bounds of the scrolled window in soil units
     */
    DoubleRect GetClientBoundsSoil() const;

    /**
     * Called  by RmResizeTimer::Notify
     */
    rootmap::BufferChangeResponse DoResize();

    /**
     * Sent to each observer by the SimulationEngine after each timestamp
     * is complete.  This is the ideal place for window updates to be
     * triggered.
     */
    void PostTimestampNotification();

    void ZoomIn();
    void ZoomOut();
    void Zoom(double deltaZ);

    /**
     *
     */
    void UpdateViewDirection(ViewDirection viewDir);

    void UpdateBoundaries(rootmap::BooleanStateChange changeBoundaries);
    void UpdateBoxes(rootmap::BooleanStateChange changeBoxes);
    void UpdateBoxColours(rootmap::BooleanStateChange changeBoxColours);
    void UpdateRepeat(rootmap::BooleanStateChange updateRepeat);
    void UpdateWrap(rootmap::BooleanStateChange updateWrap);

    void UpdateCharacteristicColourInfo(rootmap::CharacteristicColours_t& cct);

    void UpdateProcesses(const rootmap::ProcessIdentifierSet& processIds);

    ////@begin RmScrolledWindow member variables
    ////@end RmScrolledWindow member variables
private:
    /**
     * WAS: VPanorama::MaintainSoil
     */
    void UpdateVirtualSize(ScoreboardCoordinator& scoord);

    RootMapLoggerDeclaration();

    ///
    /// this window's child buffer
    ScrolledWindowBuffer* m_buffer;

    ///
    /// This class is owner of this information, it is a copy of that passed
    /// in during construction. Initially, the ReferenceIndex is used to set
    /// the scroll position. After that, the ReferenceIndex is set by scrolling
    /// and resizing activities
    ViewInformation* m_viewInformation;

    ///
    ///
    IViewCoordinator* m_viewCoordinator;

    ///
    ///
    wxRect m_virtualBoundsPix;

    ///
    ///
    DoubleRect m_virtualBoundsSoil;

    ///
    /// This is the offset of the scoreboard from
    /// the topleft of the virtual space in soil units
    rootmap::DoublePt m_scoreboardOffsetSoil;

    ///
    // essentially the absolute value of the reference point
    rootmap::DoublePt m_clientScoreboardOffsetSoil;

    ///
    ///
    wxPoint m_clientOffsetPix;

    ///
    ///
    rootmap::DoublePt m_clientOffsetSoil;

    ///
    /// if resizing is active, this is non-NULL and 
    rootmap::ResizeBufferChangeInfo* m_currentResizeInfo;

    ///
    ///
    int m_ignoreResizeCount;

    ///
    ///
    RmResizeTimer m_resizeTimer;
};

inline ViewInformation& RmScrolledWindow::GetViewInformation()
{
    return *m_viewInformation;
}

inline const ViewInformation& RmScrolledWindow::GetViewInformation() const
{
    return *m_viewInformation;
}

#endif
// _RMSCROLLEDWINDOW_H_
