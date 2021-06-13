/////////////////////////////////////////////////////////////////////////////
// Name:        ScrolledWindowBuffer.cpp
// Purpose:     Implementation of the ClassTemplate class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2009-07-12 04:34:08 +0800 (Sun, 12 Jul 2009) $
// $Revision: 70 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/ViewWindow/ScrolledWindowBuffer.h"
#include "app/gui/ViewWindow/ScrolledWindowHelper.h"
#include "app/gui/ViewWindow/ScoreboardBuffer.h"
#include "app/gui/ViewWindow/ScoreboardBoxBoundaries.h"
#include "app/gui/ViewWindow/ScoreboardColourBoxes.h"
#include "app/gui/ViewWindow/ProcessDrawingBuffer.h"
#include "app/gui/ViewWindow/RmScrolledWindow.h"
#include "app/gui/ViewCommon/ViewInformation.h"

#include "simulation/process/common/Process.h"
#include "simulation/process/common/ProcessDrawing.h"
#include "simulation/scoreboard/ScoreboardCollaboration.h"
#include "simulation/scoreboard/ScoreboardCoordinator.h"
#include "simulation/scoreboard/ScoreboardDependencyIterator.h"
#include "simulation/process/common/Process.h"
#include "core/utility/Utility.h"
// #include "core/common/Constants.h"

#define LOG_DEVICE_ORIGIN(d,label)  { int dorigX,dorigY; d.GetDeviceOrigin(&dorigX,&dorigY); wxPoint label##_device_origin(dorigX,dorigY); LOG_ONDRAW_SIMPLE(label##_device_origin); }
#define LOG_LOGICAL_ORIGIN(d,label) { int dlogicalX,dlogicalY; d.GetLogicalOrigin(&dlogicalX,&dlogicalY); wxPoint label##_logical_origin(dlogicalX,dlogicalY); LOG_ONDRAW_SIMPLE(label##_logical_origin); }

#define RETURN_NOT_OK_BUFFERCHANGERESPONSE(response,value) if (bcr_OK<response) { return (value); }

// #define LOG_LINE "(" << LINE_STRING << ") "
#define LOG_SIMPLE_MSG(msg,x) LOG_DEBUG << LOG_LINE << msg << #x "=" << rootmap::ToString(x).c_str()
#define LOG_DEBUG_HERE LOG_DEBUG << LOG_LINE


/*
// #define LOG_ONDRAW
#if defined LOG_ONDRAW
#define LOG_ONDRAW_FUNCTION(fn)  LOG_DEBUG_FUNCTION(fn)
#define LOG_ONDRAW_SIMPLE(x)     LOG_DEBUG_SIMPLE(x)
#define LOG_ONDRAW_SIMPLE_MSG(msg,x) LOG_SIMPLE_MSG(msg,x)
#else
#define LOG_ONDRAW_FUNCTION(fn)
#define LOG_ONDRAW_SIMPLE(x)
#endif // #if defined LOG_ONDRAW

//#define LOG_REPEATDRAW
#if defined LOG_REPEATDRAW
#define LOG_REPEATDRAW_FUNCTIREPEAT(fn) LOG_DEBUG_FUNCTIREPEAT(fn)
#define LOG_REPEATDRAW_SIMPLE(x) LOG_DEBUG_SIMPLE(x)
#else
#define LOG_REPEATDRAW_FUNCTIREPEAT(fn)
#define LOG_REPEATDRAW_SIMPLE(x)
#endif // #if defined LOG_REPEATDRAW

//#define LOG_RESIZE
#if defined LOG_RESIZE
#define LOG_RESIZE_FUNCTION(fn) LOG_FUNCTION(fn)
#define LOG_RESIZE_SIMPLE(x) LOG_SIMPLE(x)
#else
#define LOG_RESIZE_FUNCTION(fn)
#define LOG_RESIZE_SIMPLE(x)
#endif // #if defined LOG_RESIZE
*/

#include <limits.h>
// #include <fp.h>
// #include <stdlib.h>

// #define CS_VPANORAMA_UNIT 6
// #define CS_VIEW_SCROLL_AMOUNT CS_VPANORAMA_UNIT


#include "wx/gdicmn.h"
#include "wx/dcmemory.h"

namespace rootmap
{
    RootMapLoggerDefinition(ScrolledWindowBuffer);

    int ScrolledWindowBuffer::s_scrollAmountPixels = 6;

    using Utility::CSMin;
    using Utility::CSMax;


    ScrolledWindowBuffer::ScrolledWindowBuffer
    (ScoreboardCoordinator& scoord, RmScrolledWindow& parent, int pixelDepth)
        : ViewBuffer()
        , m_scoreboardCoordinator(scoord)
        , m_parent(parent)
        , m_pixelDepth(pixelDepth)
        , m_clientBoundsSoil(0.0, 0.0, 0.0, 0.0)
        , m_secondariesFitWithinV(false)
        , m_secondariesFitWithinH(false)
        , m_scoreboardBoundsColour(0, 0x4F, 0xFF)
        , m_scoreboardBoundaries(true)
        , m_boxBoundaries(__nullptr)
        , m_boxColours(__nullptr)
    {
        RootMapLoggerInitialisation("rootmap.ScrolledWindowBuffer");
        RootMapLogTrace("ScrolledWindowBuffer::ScrolledWindowBuffer");
    }


    BufferChangeResponse ScrolledWindowBuffer::Create
    (const rootmap::ViewInformation& viewInfo)
    {
        RootMapLogTrace("ScrolledWindowBuffer::Create");
        BufferChangeResponse response;
        wxRect view_bounds = viewInfo.GetBounds();

        response = ViewBuffer::Create(
            view_bounds.GetWidth(),
            view_bounds.GetHeight(),
            m_pixelDepth,
            viewInfo.GetScale(),
            viewInfo.GetViewDirection());

        if (response == bcr_OK)
        {
            ScoreboardDependencyIterator iterator(this, true);
            m_scoreboardCoordinator.IterateOverScoreboards(&iterator);

            return (CreateSecondaryBuffers(viewInfo));
        }

        return (response);
    }


    ScrolledWindowBuffer::~ScrolledWindowBuffer()
    {
        RootMapLogTrace("ScrolledWindowBuffer::~ScrolledWindowBuffer");

        ScoreboardDependencyIterator iterator(this, true);
        m_scoreboardCoordinator.IterateOverScoreboards(&iterator);

        DestroySecondaryBuffers();
    }

    void ScrolledWindowBuffer::DestroySecondaryBuffers()
    {
        RootMapLogTrace("ScrolledWindowBuffer::DestroySecondaryBuffers");

        delete m_boxBoundaries;
        m_boxBoundaries = __nullptr;
        delete m_boxColours;
        m_boxColours = __nullptr;

        for (ProcessBufferContainer::iterator iter = m_processBuffers.begin();
            iter != m_processBuffers.end();
            ++iter)
        {
            delete *iter;
        }
    }


    BufferChangeResponse ScrolledWindowBuffer::CreateSecondaryBuffers(const rootmap::ViewInformation& viewInfo)
    {
        RootMapLogTrace("ScrolledWindowBuffer::CreateSecondaryBuffers");
        m_scoreboardBoundaries = viewInfo.DoesBoundaries();

        DoublePt refpt(viewInfo.GetReferenceIndex(), GetViewDirection());
        SetReferences(refpt.h, refpt.v);

        // Set m_clientBoundsSoil. This is the DoubleRect, soil-bound, version of vbBoundsX
        DoubleRect soil_frame;
        Buffer_UnconvertRect(GetBounds(), m_clientBoundsSoil);

        //Initialise m_secondariesFitWithinH and m_secondariesFitWithinV. These remind us as to whether
        // the scoreboard is completely shown in secondary buffers in either H or V direction
        DoubleRect scoreboard_bounds;
        int tmp_int;
        m_scoreboardCoordinator.GetTotalScoreboardBounds(GetViewDirection(), &scoreboard_bounds);
        DetermineBufferWholeSizes(scoreboard_bounds, m_clientBoundsSoil, m_secondariesFitWithinH, m_secondariesFitWithinV, tmp_int, tmp_int, viewInfo.GetScale());

        BufferChangeResponse response = bcr_NONE;
        // make the box boundaries and colour boxes, if necessary
        if (viewInfo.DoesBoxes())
        {
            response = CreateBoxBoundaryBuffers();
            RETURN_NOT_OK_BUFFERCHANGERESPONSE(response, bcr_Failure)
        }

        if (viewInfo.DoesBoxColours())
        {
            response = CreateColourBoxBuffers();
            //if (response > bcr_OK)
            //{
            // Note to self: don't delete other buffers already made
            // Second Note to self: don't return here, need to SetRebuildLevel etc.
            // return (bcr_PartialFailure);
            //};
        }

        // and now any process' specific buffers
        response = CreateProcessBuffers(viewInfo);

        if (response > bcr_OK)
        {
            /* don't need to do anything significant. At this stage. I think.

            Assumption: if CreateProcessBuffers returns only bcr_PartialFailure,
            then whatever buffers have been made is good enough. If it returns
            bcr_Failure, then even though two buffers is enough to look at, it
            probably means that memory is so tight that if the window is resized,
            neither buffer will be resized successfully.  Therefore, just returning
            the value of CreateProcessBuffers is useful enough.
            */
        }

        SetRebuildLevel(bbl_Complete);

        //{
        //    ViewBufferPrep prep(this);
        //    Prepare();
        //}

        return (response);
    }


    /* __V__A__V__A__V__A__V__A__V__A__V__A__V__A__V__A__V__A__V__A__V__
    SetBounds
    Need to set the scrolled bounds, too. But not the same as the passed, just the same size.

    DEPENDENCIES:    Make sure that m_frame[H|V]Offset are set before calling SetBounds(),
                which includes before ViewBuffer::UpdateSize() & ::Update(), any::Resize()
                and anything other calls that you can ‘Find’ in the V* classes.
    Modifications    :
    95.07.20    Since during a resize with rescale neither vb[H|V]Reference or GetScale() are
            gauranteed to be properly set by this stage,  we can't really rely on them at all.
            We CAN rely on m_frame[H|V]Offset, since it is ours and we make sure to set it.
    95.08.29    moved ScrolledWindowBuffer->m_frame to ViewBuffer->vbBoundsX
            parameter is now a wxRect.
    __V__A__V__A__V__A__V__A__V__A__V__A__V__A__V__A__V__A__V__A__V__ */
    void ScrolledWindowBuffer::SetBounds(const wxRect& newBounds)
    {
        RootMapLogTrace("ScrolledWindowBuffer::SetBounds");
        ViewBuffer::SetBounds(newBounds);

        // Buffer_UnconvertRect(GetBounds(), m_clientBoundsSoil);
    }


    /* __V__A__V__A__V__A__V__A__V__A__V__A__V__A__V__A__V__A__V__A__V__
    Resize
    Overridden to offset the requested bounds so that the topLeft is [0, 0]. Also resets the
    soil drawing reference.

    Modifications    :
    95.06.22    instead of modifying the passed wxRect argument directly, use a local wxRect, so that
            the original calling routine can still use the wxRect it passes.
    95.07.24    now use a VCB_VewResizeInfo struct to pass extra required information.
    95.07.28    now sets vb[H|V]Reference. DUH.
    95.08.03    interface is now the virtual inherited one, which itself is rather different.
            Now uses a void pointer to an [obviously] anonymous struct. For this class,
            the struct is considered to be a Primary_ResizeInfo
    95.08.10    Added code to check if 2ndry buffers need resizing, and then check them if so.
    96.12.29    now also resize m_processBuffers (and so on, by recursion)
    96.12.31    m_processBuffers' Resize now iterative
    99.06.22    decided to remove the business of rescaling by option-resizing. Thus, no soil
            scale change will occur here, and hence no secondary buffers will change.
    __V__A__V__A__V__A__V__A__V__A__V__A__V__A__V__A__V__A__V__A__V__ */
    BufferChangeResponse ScrolledWindowBuffer::Resize(ResizeBufferChangeInfo* info)
    {
        RootMapLogTrace("ScrolledWindowBuffer::Resize");
        wxDateTime unow = wxDateTime::UNow();
        wxString unow_str = unow.Format("Resize at %H:%M:%S.%l", wxDateTime::GMT8);
        RootMapLogInfo(unow_str);

        double oldSoilScale = GetScale();

        BufferChangeResponse response = ViewBuffer::Resize(info);

        RETURN_NOT_OK_BUFFERCHANGERESPONSE(response, response)

            // m_clientBoundsSoil = info->bci_SoilFrame;

            /*****
            **    Insert appropriate secondary buffer resizing code here
            *****/
            /* Here is where we need to check if we are going to need to resize the secondary
            buffers. We don't actuall resize the buffers yet since the primary buffer is more
            important. We mainly need to check for SoilScale change here, before calling
            ViewBuffer::Resize(), since it changes the GetScale(). Conditions monitored are:
            1. Soil scale change // not, as of 99.06.22
            2. SoilFrame < ScoreboardBounds
            3. SB < SF but was not previously */
            int dummyShort;
        bool old2ndsFitWithinV = m_secondariesFitWithinV;
        bool old2ndsFitWithinH = m_secondariesFitWithinH;
        DetermineBufferWholeSizes(
            (info->bci_ScoreboardBounds),
            (info->bci_SoilFrame),
            m_secondariesFitWithinH,
            m_secondariesFitWithinV,
            dummyShort,
            dummyShort,
            GetScale());

        bool is_scale_change = ((info->bci_Scale > 0) && (info->bci_Scale != oldSoilScale));
        bool is_2ndsFitWithinV_change = ((m_secondariesFitWithinH) && (!old2ndsFitWithinH));
        bool is_2ndsFitWithinH_change = ((m_secondariesFitWithinV) && (!old2ndsFitWithinV));

        if (is_scale_change
            || (!m_secondariesFitWithinH) || (!m_secondariesFitWithinV)
            || (is_2ndsFitWithinV_change) || (is_2ndsFitWithinH_change))
        {
            // try to resize the colour boxes' buffer
            if (m_boxColours)
            {
                response = m_boxColours->Resize(info);
                if (response > bcr_OK)
                {
                    // ASSUMPTION: if the box colours buffer resize was unsuccessful
                    // then the app is running low on memory and we shouldn't resize
                    // the others either
                    DestroySecondaryBuffers();

                    // TODO: throw an exception
                    return bcr_Failure;
                }
            }

            // try to resize the scoreboard boxes' buffer
            if (m_boxBoundaries)
            {
                response = m_boxBoundaries->Resize(info);
                if (response > bcr_OK)
                {
                    // ASSUMPTION: if the box colours buffer resize was unsuccessful
                    // then the app is running low on memory and we shouldn't resize
                    // the others either
                    DestroySecondaryBuffers();

                    // TODO: throw an exception
                    return bcr_Failure;
                }
            }


            /* try to resize the processes' buffer. Here we assume that if one
            buffer fails, all subsequent ones will also. This is quick, but not
            so clever : freeing up a couple of pixmaps worth of memory might
            give another one a chance... */
            bool still_resize = true;
            for (ProcessBufferContainer::iterator iter = m_processBuffers.begin();
                iter != m_processBuffers.end(); ++iter)
            {
                if (still_resize)
                {
                    response = (*iter)->Resize(info);
                }
                else
                {
                    delete (*iter);
                    iter = m_processBuffers.erase(iter);
                }

                // assumption: if creating this process buffer failed, the rest will
                if (response > bcr_OK)
                {
                    still_resize = false;
                }
            }

            // if the 
            if (!still_resize)
            {
                return bcr_PartialFailure;
            }
        }
#if defined SCROLLWINDOWBUFFER_RESIZE_FIXED
#endif // #if defined SCROLLWINDOWBUFFER_RESIZE_FIXED

        return bcr_OK;
    }

#if defined SCROLLWINDOWBUFFER_SCROLL_USED
    /*
    Scroll
    Move the buffer's image by the requested soil centimetres. Overridden here because ViewBuffer
    doesn't, but because we need to at least do the following :
    1.    Call the inherited.
    2.    Offset the m_frame (equals the scrolled cartographer's frame).

    3.    Clear the buffer's image.
            OR
    3.    Scroll the buffer's image.

    4.    Resplat/Redraw the child buffers.

    Modifications    :
    95.07.20    Accounts for top viewing - negates the vertical scroll amount
            Rounds off for panorama units - PANO_MUL
    95.08.15    Do 4 seperate m_frame calculations (instead of OffsetRect())
            Add support for scrolling secondary buffers
    95.08.29    Commented out stuff about m_frame[[H|V]Offset]
    96.12.29    also Buffer_Scroll and Buffer_ScrollTo m_processBuffers (and the rest, by recursion)
    96.12.31    m_processBuffers' Buffer_Scroll and Buffer_ScrollTo now iterative
    97.01.11    now only scroll secondary buffers by the necessary amount in each direction
     */
    void ScrolledWindowBuffer::Scroll(double hDelta, double vDelta)
    {
        RootMapLogTrace("ScrolledWindowBuffer::XXXX");
        ViewBuffer::Scroll(hDelta, vDelta);

        /* Scroll the other way if looking from the top */
        double vDelta_adjusted = vDelta;

        /*****
        **    Secondary Buffer scrolling
        **    Don't tell the 2ndry buffer to scroll in a particular direction
        **    if it is the entire dimension of the scoreboard in that direction.
        *****/
        bool scroll_h = ((hDelta != 0.0) && (!m_secondariesFitWithinH));
        bool scroll_v = ((vDelta_adjusted != 0.0) && (!m_secondariesFitWithinV));
        bool scroll_secondaries = scroll_h || scroll_v;
        if (scroll_secondaries)
        {
            double hScrolled = (m_secondariesFitWithinH) ? 0.0 : hDelta;
            double vScrolled = (m_secondariesFitWithinV) ? 0.0 : vDelta_adjusted;

            if (m_boxBoundaries) m_boxBoundaries->Scroll(hScrolled, vScrolled);
            if (m_boxColours) m_boxColours->Scroll(hScrolled, vScrolled);

            //if (!DoesWrap())
            //{    
            //hScrolled = ((m_secondariesFitWithinH) && (DoesWrap())) ? 0.0 : hDelta;
            //vScrolled = ((m_secondariesFitWithinV) && (DoesWrap())) ? 0.0 : vDelta_adjusted;

            for (ProcessBufferContainer::iterator iter = m_processBuffers.begin();
                iter != m_processBuffers.end(); ++iter)
            {
                (*iter)->Scroll(hScrolled, vScrolled);
            }
            //}
        }

    }
#endif // #if defined SCROLLWINDOWBUFFER_SCROLL_USED


    /*
     * ScrollTo
     *
     * 1.    Call the inherited.
     *
     * 2.    Clear/Scroll the buffer's image (depending on m_secondariesFitWithinH|V)
     *
     * 3.    Resplat/Redraw the child buffers.
     */
    void ScrolledWindowBuffer::ScrollTo(double hPos, double vPos)
    {
        RootMapLogTrace("ScrolledWindowBuffer::ScrollTo");

        double hPosOriginal = GetHReference();
        double vPosOriginal = GetVReference();

        ViewBuffer::ScrollTo(hPos, vPos);
        GetViewInformation().SetReferencePoint(DoublePt(hPos, vPos));

        // Buffer_UnconvertRect(GetBounds(), m_clientBoundsSoil);

        /*****
        **    Secondary Buffer scrolling
        **    Don't tell the 2ndry buffer to scroll in a particular direction
        **    if it is the entire dimension of the scoreboard in that direction.
        *****/
        bool is_delta_h = (hPos != hPosOriginal);
        bool is_delta_v = (vPos != vPosOriginal);
        bool is_scroll_h = (is_delta_h && !m_secondariesFitWithinH);
        bool is_scroll_v = (is_delta_v && !m_secondariesFitWithinV);

        if (is_scroll_h || is_scroll_v)
        {
            double hScrollTo = (m_secondariesFitWithinH) ? 0.0 : hPos;
            double vScrollTo = (m_secondariesFitWithinV) ? 0.0 : vPos;

            LOG_INFO << LOG_LINE << "Scrolling secondary scoreboard buffers to {" << hScrollTo << "," << vScrollTo << "} (given {" << hPos << "," << vPos << "})";

            // for each secondary buffer do
            if (m_boxBoundaries)
            {
                m_boxBoundaries->ScrollTo(hScrollTo, vScrollTo);
            }
            if (m_boxColours)
            {
                m_boxColours->ScrollTo(hScrollTo, vScrollTo);
            }
        }
        else
        {
            LOG_INFO << LOG_LINE << "NOT Scrolling secondary scoreboard buffers";
        }

        // process (un-wrappable) buffers need an extra horizontal check
        is_scroll_h = (is_delta_h && ((!m_secondariesFitWithinH) || (!DoesWrap())));
        // never wrap in Vertical so !DoesWrap always true and hence test for m_secondariesFitWithinV is moot
        //is_scroll_v = is_delta_v;
        //Unfortunately, that is only partially true. Turning on Wrapping turns on
        //use of ScoreboardBuffer, which doesn't create a full-window height buffer.
        //TODO:create an issue for this problem
        //It is also only partially true because when viewing From Top, the wrap
        //applies in the Vertical also.
        is_scroll_v = (is_delta_v && ((!m_secondariesFitWithinV) || (!DoesWrap())));

        if (is_scroll_h || is_scroll_v)
        {
            double hScrollTo = (is_scroll_h) ? hPos : (DoesWrap() ? 0.0 : hPosOriginal);
            double vScrollTo = (is_scroll_v) ? vPos : (DoesWrap() ? 0.0 : vPosOriginal);

            LOG_INFO << LOG_LINE << "Scrolling secondary process buffers to {" << hScrollTo << "," << vScrollTo << "} (given {" << hPos << "," << vPos << "})";

            for (ProcessBufferContainer::iterator iter = m_processBuffers.begin();
                iter != m_processBuffers.end(); ++iter)
            {
                (*iter)->ScrollTo(hScrollTo, vScrollTo);
            }
        }
        else
        {
            LOG_INFO << LOG_LINE << "NOT Scrolling secondary process buffers";
        }
        LOG_DEBUG << "{is_scroll_h:" << Utility::ToString(is_scroll_h, Utility::bsrtruefalse)
            << ", is_delta_h:" << Utility::ToString(is_delta_h, Utility::bsrtruefalse)
            << ", m_secondariesFitWithinH:" << m_secondariesFitWithinH
            << ", DoesWrap():" << Utility::ToString(DoesWrap(), Utility::bsrtruefalse)
            << ", hPos:" << hPos
            << ", hPosOriginal:" << hPosOriginal
            << "}";
        LOG_DEBUG << "{is_scroll_v:" << Utility::ToString(is_scroll_v, Utility::bsrtruefalse)
            << ", is_delta_v:" << Utility::ToString(is_delta_v, Utility::bsrtruefalse)
            << ", m_secondariesFitWithinV:" << m_secondariesFitWithinV
            << ", DoesWrap():" << Utility::ToString(DoesWrap(), Utility::bsrtruefalse)
            << ", vPos:" << vPos
            << ", vPosOriginal:" << vPosOriginal
            << "}";
    }

    void ScrolledWindowBuffer::ProviderChanged(CCollaborator* aProvider, long reason, CCollaboratorInfo* /* info */)
    {
        RootMapLogTrace("ScrolledWindowBuffer::ProviderChanged");
        Scoreboard* scoreboard = dynamic_cast<Scoreboard *>(aProvider);

        if ((__nullptr != scoreboard)
            && (IsScoreboardVisible(scoreboard->GetScoreboardStratum()))
            )
        {
            if ((scoreboardEdittedCell == reason) || (scoreboardSimulationStepComplete == reason))
            {
                if (m_boxColours)
                {
                    m_boxColours->SetRebuildLevel(bbl_Redraw);
                }
                SetRebuildLevel(bbl_Reslap);
                m_parent.Refresh();
            }
            else
            {
                // not a message we care about
            }
        }
        else
        {
            // the given provider is NOT our scoreboard
            // TODO: who to pass this on to ?
        }
    } // ScrolledWindowBuffer::ProviderChanged


    /*
     * SetViewDirection
     * Remake all the secondary buffers at the new size.
     */
    BufferChangeResponse ScrolledWindowBuffer::SetViewDirection
    (ViewDirection newDirection,
        ViewDirectionBufferChangeInfo* info)
    {
        RootMapLogTrace("ScrolledWindowBuffer::SetViewDirection");
        // still need to set these from the scoreboardcoordinator :
        m_scoreboardCoordinator.GetTotalScoreboardBounds(newDirection, &(info->bci_ScoreboardBounds));
        // but maybe not these :
        // info->bci_VLayers = __nullptr;
        // info->bci_HLayers = __nullptr;

        BufferChangeResponse bcr = ViewBuffer::SetViewDirection(newDirection, info);

        m_clientBoundsSoil = info->bci_SoilFrame;

        /**********
        Set_Secondary-Buffer's_ViewDirection[s]
        **********/
        int dummyShort;
        double oldrefh = info->bci_ReferencePt.h;
        double oldrefv = info->bci_ReferencePt.v;

        /* Here we don't need to know what size the secondary buffers
        are going to be, just if they will fit within ourself */
        DetermineBufferWholeSizes((info->bci_ScoreboardBounds),
            (info->bci_SoilFrame),
            m_secondariesFitWithinH,
            m_secondariesFitWithinV,
            dummyShort,
            dummyShort,
            GetScale());

        /* if the secondary buffers fit in either direction, adjust the
        reference point that we pass on them so that they can ignore such stuff */
        if (m_secondariesFitWithinH)
        {
            info->bci_ReferencePt.h = 0.0;
        }
        if (m_secondariesFitWithinV)
        {
            info->bci_ReferencePt.v = 0.0;
        }

        /* now DO IT to the secondary buffers, in the usual order :
        1. box boundaries
        2. box colours
        3. process-specific drawings */
        if (m_boxBoundaries) m_boxBoundaries->SetViewDirection(newDirection, info);
        if (m_boxColours) m_boxColours->SetViewDirection(newDirection, info);

        /* reset the information's reference point, but only if wrapping
        is off. This allows the process buffers to be in the right place*/
        if (true) // (!DoesWrap())
        {
            info->bci_ReferencePt.h = oldrefh;
            info->bci_ReferencePt.v = oldrefv;
        }

        for (ProcessBufferContainer::iterator iter = m_processBuffers.begin();
            iter != m_processBuffers.end(); ++iter)
        {
            (*iter)->SetViewDirection(newDirection, info);
        }

        // Even if wrapping IS on, we still need to do this right here
        if (true) // (DoesWrap())
        {
            info->bci_ReferencePt.h = oldrefh;
            info->bci_ReferencePt.v = oldrefv;
        }

        return (bcr);
    }


#define SAVE_DEVICE_ORIGIN(d,label) int device_origin_X_##label,device_origin_Y_##label; \
                                    d.GetDeviceOrigin(&device_origin_X_##label,&device_origin_Y_##label)
#define ZERO_DEVICE_ORIGIN(d,label) d.SetDeviceOrigin(0,0)
#define RESTORE_DEVICE_ORIGIN(d,label) d.SetDeviceOrigin(device_origin_X_##label,device_origin_Y_##label)

#define SAVE_LOGICAL_ORIGIN(d,label) int logical_origin_X_##label,logical_origin_Y_##label; \
                                    d.GetLogicalOrigin(&logical_origin_X_##label,&logical_origin_Y_##label)
#define ZERO_LOGICAL_ORIGIN(d,label) d.SetLogicalOrigin(0,0)
#define RESTORE_LOGICAL_ORIGIN(d,label) d.SetLogicalOrigin(logical_origin_X_##label,logical_origin_Y_##label)

    /* DrawContents
    Draws the scoreboard boundaries. Ex VPanorama::DrawScoreboardBounds() code
    95.08.18    Moved secondary redrawing/reslapping code from Draw() to here
    95.08.23    Do SectRect of buffer and m_frame area before aligning
    95.08.24    If both m_secondariesFitWithin? are true, adjust the drawing bounds to infinity
            to suit. This is because even if the scoreboard is mostly scrolled out ouf sight,
            we still want the entire 2ndry buffer to be drawn. This in turn is because
            the 2ndry buffer will not be redrawn when the Buffer_Scroll()s if both
            m_secondariesFitWithin? are true
    95.08.29    Changed references of m_frame to vbBoundsX
            Changed some Rects to wxRects, especially for CopyFrom()s
    96.12.29    also RedrawBuffer for all m_processBuffers, iteratively
    97.01.11    Split main work into three sub-functions for clarity.
    */
    void ScrolledWindowBuffer::DrawContents
    (wxDC& dc,
        const wxRect& clientBoundsPix,
        const wxRect& /*virtualBoundsPix*/,
        const DoubleRect& /*virtualBoundsSoil*/)
    {
        RootMapLogTrace("ScrolledWindowBuffer::DrawContents");
        if (GetRebuildLevel() <= bbl_NONE) return;

        //TODO LOG_DEBUG << "DrawContents clientBoundsPix=" << ToString(clientBoundsPix)
        //          << " virtualBoundsPix=" << ToString(virtualBoundsPix)
        //          << " virtualBoundsSoil=" << ToString(virtualBoundsSoil);


        // ////////////////////
        // Calculate Drawing Areas
        ScrolledWindowHelper helper(GetViewDirection(), GetScale(), s_scrollAmountPixels);

        //DoublePt scoreboard_offset_soil(fabs(virtualBoundsSoil.left),fabs(virtualBoundsSoil.top));
        //LOG_ONDRAW_SIMPLE(scoreboard_offset_soil);

        { // ViewBufferPrep scope restriction
            // wxMemoryDC memDC;
            // memDC.SelectObject(GetBitmap());
            // SetDeviceContext(&memDC);
            ViewBufferPrep prep_this(this);

            GetDC().Clear();

            //wxPoint scoreboard_offset_pix;
            //helper.Soil2ScrollPixelPoint(scoreboard_offset_soil, scoreboard_offset_pix);

            DoublePt reference_pt(GetReferencePt());

            DoubleRect scoreboard_bounds_soil;
            m_scoreboardCoordinator.GetTotalScoreboardBounds(GetViewDirection(), &scoreboard_bounds_soil);

            wxRect scoreboard_bounds_pix;
            helper.Soil2ScrollPixelRect(scoreboard_bounds_soil, scoreboard_bounds_pix);

            // pix bounds should be positioned at 0,0
            wxRect client_bounds_pix = m_parent.GetClientBoundsPix();
            client_bounds_pix.SetPosition(wxPoint(0, 0));

            // soil bounds should be offset 
            DoubleRect client_bounds_soil = m_parent.GetClientBoundsSoil();

            //TODO LOG_DEBUG << "DrawContents() client_bounds_pix=" << ToString(client_bounds_pix) << " client_bounds_soil=" << ToString(client_bounds_soil);
            client_bounds_soil.SetPosition(reference_pt);
            //scoreboard_bounds_soil.SetPosition(reference_pt);
            //TODO LOG_DEBUG << "DrawContents() adjusted client_bounds_soil=" << ToString(client_bounds_soil);

            RedrawSecondaryBuffers(&scoreboard_bounds_soil, &client_bounds_soil, scoreboard_bounds_pix, client_bounds_pix);

            DrawScoreboardBounds(client_bounds_soil, scoreboard_bounds_soil);

            CopyTo(dc, GetBounds(), clientBoundsPix);
        } // ViewBufferPrep scope restriction

        // RESTORE_DEVICE_ORIGIN(dc,Clear);

        //LOG_INFO << "Finished DrawContents\n";
    }

    // this may or may not do anything useful.  For now, it is merely a required override.
    void ScrolledWindowBuffer::DrawContents(const DoubleRect& /* area */)
    {
        RootMapLogTrace("ScrolledWindowBuffer::DrawContents");
    }


    /*****
    **    Redraw and/or Reslap the secondary buffers in this order :
    **
    **    1. the scoreboard colour boxes
    **    2. the process specific drawings
    **    3. the scoreboard boxes
    *****/
    void ScrolledWindowBuffer::RedrawSecondaryBuffers
    (DoubleRect* scoreboardBoundsSoil,
        DoubleRect* clientBoundsSoil,
        const wxRect& scoreboardBoundsPix,
        const wxRect& clientBoundsPix)
    {
        RootMapLogTrace("ScrolledWindowBuffer::RedrawSecondaryBuffers");

        /*TODO LOG_DEBUG << "RedrawSecondaryBuffers scoreboardBoundsSoil=" << ToString(*scoreboardBoundsSoil).c_str()
                  << " scoreboardBoundsPix=" << ToString(scoreboardBoundsPix).c_str()
                  << " clientBoundsSoil=" << ToString(*clientBoundsSoil).c_str()
                  << " clientBoundsPix=" << ToString(clientBoundsPix).c_str();
                  */
                  // Pre-1. use appropriate bounds for repeatable buffers (ie. non-processes)
        DoubleRect* repeatable_bounds_soil;
        wxRect repeatable_bounds_pix;
        if (DoesRepeat())
        {
            repeatable_bounds_soil = clientBoundsSoil;
            repeatable_bounds_pix = clientBoundsPix;
        }
        else
        {
            repeatable_bounds_soil = scoreboardBoundsSoil;
            repeatable_bounds_pix = scoreboardBoundsPix;
        }

        // Pre-2. use appropriate bounds for wrappable buffers (ie. processes)
        DoubleRect* wrappable_bounds_soil;
        wxRect wrappable_bounds_pix;
        if (DoesWrap())
        {
            wrappable_bounds_soil = repeatable_bounds_soil;
            wrappable_bounds_pix = repeatable_bounds_pix;
        }
        else
        {
            wrappable_bounds_soil = clientBoundsSoil;
            wrappable_bounds_pix = clientBoundsPix;
        }


        // 1. the scoreboard colour box buffers
        RedrawBuffer(m_boxColours, repeatable_bounds_soil, repeatable_bounds_pix, false);

        // 2. Process Buffers
        for (ProcessBufferContainer::iterator iter = m_processBuffers.begin();
            iter != m_processBuffers.end(); ++iter)
        {
            RedrawBuffer((*iter), wrappable_bounds_soil, wrappable_bounds_pix);
        }

        // 3. the scoreboard box buffers
        RedrawBuffer(m_boxBoundaries, repeatable_bounds_soil, repeatable_bounds_pix);
    }


    /*
     * DrawScoreboardBounds
     * now really only draws the ground line - the outer boundary is drawn by the
     * box boundaries buffer
     */
    void ScrolledWindowBuffer::DrawScoreboardBounds
    (const DoubleRect& clientBoundsSoil,
        const DoubleRect& scoreboardBoundsSoil)
    {
        RootMapLogTrace("ScrolledWindowBuffer::DrawScoreboardBounds");
        // draw a line at level zero
        if (vTop != GetViewDirection())
        {
            Buffer_MoveTo(clientBoundsSoil.left, 0);
            Buffer_LineTo(clientBoundsSoil.right, 0, m_scoreboardBoundsColour);
        }
        //LOG_ONDRAW_SIMPLE(clientBoundsSoil);
        //LOG_ONDRAW_SIMPLE(scoreboardBoundsSoil);

        if (m_scoreboardBoundaries)
        {
            // and a blue frame around all the scoreboards
            Buffer_FrameRect(scoreboardBoundsSoil, m_scoreboardBoundsColour);
        }
    }

    void ScrolledWindowBuffer::DrawScoreboardBounds(DoubleRect* /* area */, Scoreboard* /* scoreboard */)
    {
    }


    /*
     * Redraw and Reslap secondary buffers.
     *
     * NOTE the need to have two seperate ViewBufferPrep's. This is to scope restrict
     * the DC selectobject behaviour, because creating a wxMask from a Bitmap
     * implicitly selects the bitmap into a DC
     */
    void ScrolledWindowBuffer::RedrawBuffer
    (ScoreboardBuffer* buffer,
        DoubleRect* drawArea,
        const wxRect& blitBoundsPix,
        bool createTransparencyMask)
    {
        RootMapLogTrace("ScrolledWindowBuffer::RedrawBuffer");

        if (!buffer) return;

        /*TODO LOG_DEBUG << "RedrawBuffer drawArea=" << ToString(*drawArea).c_str()
                  << " blitBoundsPix=" << ToString(blitBoundsPix).c_str();*/

                  // REDRAW
        if (buffer->GetRebuildLevel() >= bbl_Redraw)
        {
            // takes care of the memoryDC and buffer's bitmap
            ViewBufferPrep prep(buffer);

            buffer->Prepare();
            //buffer->SetXferMode(wxCOPY);
            buffer->Draw(*drawArea);
        }

        // RESLAP
        if (buffer->GetRebuildLevel() >= bbl_Reslap)
        {
            if (createTransparencyMask)
            {
                wxBitmap& bitmap = buffer->GetBitmap();
                wxMask* mask = new wxMask(bitmap, *wxWHITE);

                //bitmap->SetMask(__nullptr);
                // MSA This is OK, the bitmap now owns the resource:
                // http://docs.wxwidgets.org/stable/wx_wxbitmap.html#wxbitmapsetmask
                bitmap.SetMask(mask);
            }

            { // ViewBufferPrep scope restriction
                ViewBufferPrep prep(buffer);
                //buffer->SetXferMode(wxNO_OP);
                ReslapArea(buffer, blitBoundsPix);
            }
        } // if (buffer->GetRebuildLevel() >= bbl_Reslap)
    }


    /*

                  the destination_area_i moves for repeating
                       /     \
       +--------------/-------\------------+
       |             /         \           |
       | - +-------------+ - - - - - - + - |
       |   |             |                 |
       |   |             |             |   |
       |   |             |                 |
       |   |             |             |   |
       |   |             |                 |
       |   |             |             |   |
       |   |             |                 |
       |   |             |             |   |
       |   |             |                 |
       |   | src_area_i  |             |   |
       | - +-------------+ - - - - - - + - |
       |                                   |
       |                                   |
       |                                   |
       | dst_area_i                        |
       +-----------------------------------+


       Clipping the top and/or bottom of src_area_i :

       +-------------------------------------+       +-------------------------------------+
       |   |             |             |     |       |   |             |             |     |
       |   |             |                   |       |   |             |                   |
       |   |             |             |     |       |   |             |             |     |
       |   |             |                   |       |   |             |                   |
       |   |             |             |     |       |   |             |             |     |
       |   |             |                   |       |   |             |                   |
       |   | src_area_i  |             |     |       |   |             |             |     |
       | - +-------------+ - - - - - - + - - |       |   |             |                   |
       |                                     |       |   |             |             |     |
       |                                     |       |   |             |                   |
       |                                     |       |   |             |             |     |
       |                                     |       +-------------------------------------+
       |                                     |
       |                                     |
       | dst_area_i                          |
       +-------------------------------------+


    */
    void ScrolledWindowBuffer::ReslapArea(ScoreboardBuffer* srcBuffer, const wxRect& dstArea /*, const wxBitmap & bitmap */)
    {
        RootMapLogTrace("ScrolledWindowBuffer::ReslapArea");
        // variables suffixed with _i are integer values, _s are soil scaled (ie. floating point)


        // the overall dstArea of the source buffer
        wxRect src_area_i = srcBuffer->GetBounds();
        //TODO LOG_DEBUG << "ReslapArea dstArea=" << ToString(dstArea).c_str() << " src_area_i=" << ToString(src_area_i).c_str();

#if defined ROOTMAP_VIEW_FROM_TOP_FIXME
        if (GetViewDirection() != vTop)
#endif // #if defined ROOTMAP_VIEW_FROM_TOP_FIXME
        {
            // BEGIN : repeat code
            if ((DoesRepeat())) // && (IsHWhole()))
            {
                const int max_repeat = 100;

                double height_s;
                double width_s;

                // fetch the scoreboard width. This is the distance we
                // will move the splat to the right by each time
                m_scoreboardCoordinator.GetTotalScoreboardExtent(GetViewDirection(), &width_s, &height_s);
                int scoreboard_width_i = width_s * GetScale();

                // start by clipping the top and bottom of the src dstArea (shown above)
                src_area_i.SetTop(CSMax(src_area_i.GetTop(), dstArea.GetTop()));
                src_area_i.SetBottom(CSMin(src_area_i.GetBottom(), dstArea.GetBottom()));

                // create a destination dstArea aligned with this buffer's reference point
                wxRect destination_area_i(dstArea);
                destination_area_i.Intersect(src_area_i);
                AlignCopyArea(srcBuffer, destination_area_i);
                /*TODO LOG_DEBUG << LOG_LINE << "ReslapArea scoreboard_width_i=" << ToString(scoreboard_width_i).c_str()
                          << " src_area_i=" << ToString(src_area_i).c_str()
                          << " destination_area_i=" << ToString(destination_area_i).c_str();*/

                          // move the destination to the left
                while ((destination_area_i.GetRight() - scoreboard_width_i) > dstArea.GetLeft())
                {
                    destination_area_i.Offset(-scoreboard_width_i, 0);
                }

                // re-adjust the destination's left & right
                // destination_area_i.left = destination_area_i.right-1;
                // destination_area_i.right = destination_area_i.left+src_width;

                int repeat_count = 0;
                // loop from left to right, splatting as we go along
                while (destination_area_i.GetLeft() < dstArea.GetRight())
                {
                    wxRect dst_blit_area_i(destination_area_i);
                    //dst_blit_area_i.Intersect(dstArea);

                    wxRect src_blit_area_i(src_area_i);
                    //src_blit_area_i.Intersect(dstArea);
                    /*TODO LOG_DEBUG << LOG_LINE << "ReslapArea destination_area_i=" << ToString(destination_area_i).c_str()
                              << " dst_blit_area_i=" << ToString(dst_blit_area_i).c_str()
                              << " src_blit_area_i=" << ToString(src_blit_area_i).c_str()
                              << " repeat_count=" << repeat_count;*/

                              // splat
                    srcBuffer->CopyTo(GetDC(), src_blit_area_i, dst_blit_area_i);

                    // move over to the right one scoreboard's width
                    destination_area_i.Offset(+scoreboard_width_i, 0);

                    ++repeat_count;
                    if (repeat_count > max_repeat)
                    {
                        /*TODO LOG_WARN << "ReslapArea() Max'd out the repeat_count (max=" << ToString(max_repeat).c_str() << ")";*/
                        break;
                    }

                    //if (repeat_count>0)
                    //{
                    //    break;
                    //}
                }
            }
            else // just splat once
            {
                // END : repeat code
                // BEGIN : single-splat code
                // create a destination dstArea aligned with this buffer's reference point
                wxRect destination_area_i(dstArea);
                destination_area_i.Intersect(src_area_i);

                AlignCopyArea(srcBuffer, destination_area_i);

                //destination_area_i.SetSize(destination_area_i.GetSize()+wxSize(1,1));
                //src_area_i.SetSize(src_area_i.GetSize()+wxSize(1,1));

                // splat
                srcBuffer->CopyTo(GetDC(), src_area_i, destination_area_i);
                // END : single-splat code
            }
        }
#if defined ROOTMAP_VIEW_FROM_TOP_FIXME
        else /* (GetViewDirection() == vTop) */
        {
            // adjust the source dstArea bounds so as to compare it to ourselves
            src_area_i.Deflate(1, 1);

            // if the secondary buffer is the same size as this primary
            // buffer, we don't need to worry about upside-down'ing the dstArea
            // 
            // On the other hand, if we DO need to, then
            if (!EqualwxRect(&src_area_i, &vbBoundsX))
            {
                src_area_i.bottom++; src_area_i.right++;

                /* for splatting from the top, we need to use the bottom left of the destination
                port as the reference, instead of the top left. The four (4) lines of code where
                the bottom and top of up_side_down_area and src_area_i have mostly
                been */
                wxRect up_side_down_area;

                // get & calculate values for upside-down dstArea
                up_side_down_area.left = src_area_i.left;
                up_side_down_area.right = src_area_i.right;
                up_side_down_area.top = vbBounds.bottom - src_area_i.bottom;
                up_side_down_area.bottom = vbBounds.bottom - src_area_i.top;

                // find the intersection
                up_side_down_area.Intersect(dstArea);

                src_area_i.left = up_side_down_area.left;
                src_area_i.right = up_side_down_area.right;
                src_area_i.top = src_area_i.bottom - (up_side_down_area.bottom - up_side_down_area.top);
                src_area_i.bottom = CSMin(src_area_i.bottom, src_area_i.bottom - (vbBounds.top - vbBoundsX.top));

                AlignCopyArea(&up_side_down_area, &destination_area_i);
            }
            else
                // just align the dstArea
                AlignCopyArea(&src_area_i, &destination_area_i);

            if (DoesRepeat())
            {
                buffer->CopyFrom(otherPort, &src_area_i, &destination_area_i, 0);
            }
            else // just splat once
            {
                buffer->CopyFrom(otherPort, &src_area_i, &destination_area_i, 0);
            }
        }
#endif // #if defined ROOTMAP_VIEW_FROM_TOP_FIXME
    }


    void ScrolledWindowBuffer::UpdateBoxCharacteristic(EditCellInfo* eci)
    {
        RootMapLogTrace("ScrolledWindowBuffer::UpdateBoxCharacteristic");
        // pay no attention to this if not drawing box colours
        if (m_boxColours == 0) return;

        // rebuild a small amount if the update was relevant
        if (m_boxColours->UpdateBoxCharacteristic(eci))
        {
            SetRebuildLevel(bbl_Reslap);
        }
    }


    /* SetRepeat
    Doesn't get Draw() called. See SetBoxes() comment
    97.08.21    removed calls to ForceNextPrepare(),
                    now that SetRebuildLevel() does it
    01.08.11    VPanorama now owns the repeat flag */
    BufferChangeResponse ScrolledWindowBuffer::SetRepeat(bool fRepeat)
    {
        RootMapLogTrace("ScrolledWindowBuffer::SetRepeat");
        if (DoesRepeat() != fRepeat)
        {
            SetRebuildLevel(bbl_Reslap);
        }

        return (bcr_OK);
    }


    /* SetWrap
    Doesn't get Draw() called. See SetBoxes() comment
     */
    BufferChangeResponse ScrolledWindowBuffer::SetWrap(bool fWrap, WrapRepeatBufferChangeInfo* info)
    {
        RootMapLogTrace("ScrolledWindowBuffer::SetWrap");
        if (DoesWrap() != fWrap)
        {
            m_scoreboardCoordinator.GetTotalScoreboardBounds(GetViewInformation().GetViewDirection(), &(info->bci_ScoreboardBounds));
            for (ProcessBufferContainer::iterator iter = m_processBuffers.begin();
                iter != m_processBuffers.end(); ++iter)
            {
                (*iter)->SetWrap(fWrap, info);
            }

            SetRebuildLevel(bbl_Redraw);
        }

        return bcr_OK;
    }


    /* SetBoxes
    Doesn't get Draw() called. Routines that call this function need to do that.
    This is done so that draw can be called _after_ all changes are made
    97.08.21    removed call to ForceNextPrepare(),
                    now that SetRebuildLevel() does it
    99.07.25    all buffers now GWorlds, so we needn't surround creation with
            BeginDrawing() and EndDrawing() (which would set the current port up)
    1999.08.18 rearranged the logic a bit, removed the assignment of m_boxes */
    BufferChangeResponse ScrolledWindowBuffer::SetBoxes(bool fBoxes)
    {
        RootMapLogTrace("ScrolledWindowBuffer::SetBoxes");
        BufferChangeResponse response = bcr_OK;
        if (DoesBoxes() != fBoxes)
        {
            if ((fBoxes) && (__nullptr == m_boxBoundaries))
            {
                response = CreateBoxBoundaryBuffers();
                if (bcr_OK == response)
                {
                    SetRebuildLevel(bbl_Reslap);
                }
            }
            else
            {
                wxDELETE(m_boxBoundaries);
                SetRebuildLevel(bbl_Reslap);
            }
        }

        return (response);
    }


    /* SetDrawBoundaries
    Toggle whether the boundaries are drawn or not. By the time this is called, we can be
    certain that the new value is different, so we don't bother checking. This is also a good
    way of getting only the scoreboard boundaries redrawn.
    95.08.31    only need to rebuild if the boundaries are removed
    97.08.21    removed call to ForceNextPrepare(),
                    now that SetRebuildLevel() does it
    01.08.11    actual flag now owned by VPanorama */
    BufferChangeResponse ScrolledWindowBuffer::SetBoundaries(bool fBoundaries)
    {
        RootMapLogTrace("ScrolledWindowBuffer::SetBoundaries");
        m_scoreboardBoundaries = fBoundaries;
        SetRebuildLevel(bbl_Reslap);
        return (bcr_OK);
    }


    /* SetBoxColours
    Doesn't get Draw() called. See SetBoxes() comment
    97.08.21    removed call to ForceNextPrepare(),
                    now that SetRebuildLevel() does it
    99.07.25    all buffers now GWorlds, so we needn't surround creation with
            BeginDrawing() and EndDrawing() (which would set the current port up)
    1999.08.18 rearranged the logic a bit, removed the assignment of m_boxColours */
    BufferChangeResponse ScrolledWindowBuffer::SetBoxColours(bool fBoxColours)
    {
        RootMapLogTrace("ScrolledWindowBuffer::SetBoxColours");

        BufferChangeResponse response = bcr_OK;

        if (DoesBoxColours() != fBoxColours)
        {
            if ((fBoxColours) && (__nullptr == m_boxColours))
            {
                response = CreateColourBoxBuffers();
                if (bcr_OK == response)
                {
                    SetRebuildLevel(bbl_Reslap);
                }
            }
            else
            {
                wxDELETE(m_boxColours);
                SetRebuildLevel(bbl_Reslap);
            }
        }


        return (response);
    }


    BufferChangeResponse ScrolledWindowBuffer::SetCharacteristicColourInfo(const CharacteristicColours_t& info)
    {
        RootMapLogTrace("ScrolledWindowBuffer::SetCharacteristicColourInfo");
        if (__nullptr != m_boxColours)
        {
            m_boxColours->SetCharacteristicColourInfo(info);

            SetRebuildLevel(bbl_Complete);

            return bcr_OK;
        }

        return bcr_OK;
    }

    BufferChangeResponse ScrolledWindowBuffer::SetProcesses(const ProcessArray& new_processes)
    {
        BufferChangeResponse return_val = bcr_OK;

        // iterate over current processes, comparing processes which are added and deleted
        ProcessArray current_processes = GetViewInformation().GetDisplayProcesses();
        std::set<Process *> current_process_set(current_processes.begin(), current_processes.end());

        // iterate over the new processes. If we find a process not in the current
        // set, we should add it
        for (ProcessArray::const_iterator newit(new_processes.begin());
            new_processes.end() != newit; ++newit)
        {
            if (current_process_set.end() == current_process_set.find(*newit))
            {
                LOG_INFO << "Creating ProcessBuffer for " << (*newit)->GetProcessName();
                CreateProcessBuffer(*newit);
            }
        }


        // similarly, iterate over the new processes. If we find a process in the
        // current which is not in the new set, we should remove it
        std::set<Process *> new_process_set(new_processes.begin(), new_processes.end());
        for (ProcessArray::const_iterator currentit(current_processes.begin());
            current_processes.end() != currentit; ++currentit)
        {
            if (new_process_set.end() == new_process_set.find(*currentit))
            {
                LOG_INFO << "Removing ProcessBuffer for " << (*currentit)->GetProcessName();
                RemoveProcessBuffer(*currentit);
            }
        }

        return return_val;
    }


    BufferChangeResponse ScrolledWindowBuffer::SetScoreboard(const ScoreboardStratum& stratum, bool fTurnOn)
    {
        RootMapLogTrace("ScrolledWindowBuffer::SetScoreboard");
        if (__nullptr != m_boxBoundaries)
        {
            m_boxBoundaries->SetScoreboard(stratum, fTurnOn);
        }
        if (__nullptr != m_boxColours)
        {
            m_boxColours->SetScoreboard(stratum, fTurnOn);
        }
        for (ProcessBufferContainer::iterator iter = m_processBuffers.begin();
            iter != m_processBuffers.end(); ++iter)
        {
            (*iter)->SetScoreboard(stratum, fTurnOn);
        }

        SetRebuildLevel(bbl_Complete);
        return bcr_OK;
    }

    BufferChangeResponse ScrolledWindowBuffer::SetScoreboards(const ScoreboardFlags& flags)
    {
        RootMapLogTrace("ScrolledWindowBuffer::SetScoreboards");
        if (__nullptr != m_boxBoundaries)
        {
            m_boxBoundaries->SetScoreboards(flags);
        }
        if (__nullptr != m_boxColours)
        {
            m_boxColours->SetScoreboards(flags);
        }
        for (ProcessBufferContainer::iterator iter = m_processBuffers.begin();
            iter != m_processBuffers.end(); ++iter)
        {
            (*iter)->SetScoreboards(flags);
        }

        SetRebuildLevel(bbl_Complete);
        return bcr_OK;
    }


    BufferChangeResponse ScrolledWindowBuffer::CreateBoxBoundaryBuffers()
    {
        RootMapLogTrace("ScrolledWindowBuffer::CreateBoxBoundaryBuffers");
        BufferChangeResponse response = bcr_OK;

        if (__nullptr == m_boxBoundaries)
        {
            /* this is done here not so much as to determine the value of m_secondariesFitWithin?,
            since these will already be appropriate. Rather, it is done to calculate
            sbWidth & sbHeight in a predetermined, common, fashion. */
            int sbWidth, sbHeight;
            DoubleRect sbBounds;
            bool secondariesFitWithinH, secondariesFitWithinV;

            m_scoreboardCoordinator.GetTotalScoreboardBounds(GetViewDirection(),
                &sbBounds);

            DetermineBufferWholeSizes(sbBounds,
                m_clientBoundsSoil,
                secondariesFitWithinH,
                secondariesFitWithinV,
                sbWidth,
                sbHeight,
                GetScale());

            m_boxBoundaries = new ScoreboardBoxBoundaries(m_scoreboardCoordinator, GetViewInformation().GetScoreboardFlags());
            response = m_boxBoundaries->Create(sbWidth,
                sbHeight,
                GetPixelDepth(),
                GetScale(),
                GetViewDirection(),
                sbBounds,
                m_clientBoundsSoil);
            m_boxBoundaries->SetXferMode(wxCOPY);

            // if it worked, scroll it to the position
            // it is supposed to be.
            if (response <= bcr_OK)
            {
                m_boxBoundaries->ScrollTo
                ((m_secondariesFitWithinH) ? 0.0 : GetHReference(),
                    (m_secondariesFitWithinV) ? 0.0 : GetVReference());

                SetRebuildLevel(bbl_Reslap);
            }
        }

        return response;
    }


    BufferChangeResponse ScrolledWindowBuffer::CreateColourBoxBuffers()
    {
        RootMapLogTrace("ScrolledWindowBuffer::CreateColourBoxBuffers");
        BufferChangeResponse response = bcr_OK;

        if (__nullptr == m_boxColours)
        {
            int sbWidth, sbHeight;
            DoubleRect sbBounds;
            bool secondariesFitWithinH, secondariesFitWithinV;
            m_scoreboardCoordinator.GetTotalScoreboardBounds(GetViewDirection(), &sbBounds);

            DetermineBufferWholeSizes(sbBounds, m_clientBoundsSoil,
                secondariesFitWithinH, secondariesFitWithinV,
                sbWidth, sbHeight,
                GetScale());

            m_boxColours = new ScoreboardColourBoxes(m_scoreboardCoordinator, GetViewInformation().GetScoreboardFlags());
            response = m_boxColours->Create(
                sbWidth,
                sbHeight,
                GetPixelDepth(),
                GetScale(),
                GetViewDirection(),
                sbBounds,
                m_clientBoundsSoil);
            m_boxColours->SetXferMode(wxCOPY);

            // if it worked, scroll it to the position it is supposed to be.
            if (response <= bcr_OK)
            {
                m_boxColours->ScrollTo
                ((m_secondariesFitWithinH) ? 0.0 : GetHReference(),
                    (m_secondariesFitWithinV) ? 0.0 : GetVReference());
                m_boxColours->SetCharacteristicColourInfo(m_parent.GetViewInformation().GetCharacteristicColourInfo());
                SetRebuildLevel(bbl_Reslap);
            }
        }

        //    m_boxColours = (fBuild && (response==bcr_OK));

        return response;
    }

    /*
    99.08.12 updated to use the BufferChangeResponse of IProcessBuffer
    */
    BufferChangeResponse ScrolledWindowBuffer::CreateProcessBuffer(Process* process)
    {
        RootMapLogTrace("ScrolledWindowBuffer::CreateProcessBuffer");
        if (process == 0) return (bcr_Cancel);

        BufferChangeResponse response = bcr_OK;

        //m_parent.FillContentChangeInfo(&pcci, GetScale());

        DoubleRect sbBounds;
        m_scoreboardCoordinator.GetTotalScoreboardBounds(GetViewDirection(), &sbBounds);

        int sbWidth, sbHeight;
        bool fitsWithinH, fitsWithinV;
        DetermineBufferWholeSizes(sbBounds, m_clientBoundsSoil,
            fitsWithinH, fitsWithinV,
            sbWidth, sbHeight,
            GetScale());

        bool uses_full_width = (!fitsWithinH) || (!DoesWrap());//|| ( (DoesRepeat()) && (!fitsWithinH) );
        bool uses_full_height = (!fitsWithinV) || (!DoesWrap());//|| ( (vTop == GetViewDirection()) && (DoesRepeat()) );

        if (uses_full_width)
        {
            sbWidth = GetBounds().GetWidth();
        }
        if (uses_full_height)
        {
            sbHeight = GetBounds().GetHeight();
        }

        ProcessDrawingBuffer* process_buffer = new ProcessDrawingBuffer(
            &(process->getDrawing()), m_scoreboardCoordinator,
            DoesWrap(), DoesRepeat(),
            //RvH: 2D colouration pending
            rc_ColourStatic, //GetViewInformation().GetRootColourMode(),
            0.0, //GetViewInformation().GetBaseRootRf(),
            0.0, //GetViewInformation().GetBaseRootGf(),
            0.0, //GetViewInformation().GetBaseRootBf(),
            GetViewInformation().GetScoreboardFlags());
        response = process_buffer->Create(sbWidth,
            sbHeight,
            GetPixelDepth(),
            GetScale(),
            GetViewDirection(),
            sbBounds,
            m_clientBoundsSoil);

        if (response == bcr_OK)
        {
            double hScrollTo = (uses_full_width) ? GetHReference() : 0.0;
            double vScrollTo = (uses_full_height) ? GetVReference() : 0.0;
            LOG_INFO << LOG_LINE << "Scrolling NEW process buffer to {" << hScrollTo << "," << vScrollTo << "} (Reference {" << GetHReference() << "," << GetVReference() << "})";
            process_buffer->ScrollTo(hScrollTo, vScrollTo);

            AddProcessBuffer(process_buffer);
            process->getDrawing().AddDrawingBuffer(process_buffer);

            SetRebuildLevel(bbl_Reslap);
        }
        else
        {
            delete process_buffer; // In case creation failed
        }

        return (response);
    }

    /* 96.12.29    created. Runs around trying to find the buffer in question, then takes it
                out of circulation and destroys it. The function that calls this should
                make sure that the buffer is removed from the process' bufferlist
    97.01.03    now don't need to remove from the process' bufferlist, because
                the buffer's destructor helps that (now that it has acces to the process)
    97.08.21    removed call to ForceNextPrepare(), now that SetRebuildLevel() does it */
    void ScrolledWindowBuffer::RemoveProcessBuffer(ProcessDrawingBuffer* process_buffer)
    {
        RootMapLogTrace("ScrolledWindowBuffer::RemoveProcessBuffer");

        for (ProcessBufferContainer::iterator iter = m_processBuffers.begin();
            iter != m_processBuffers.end();
            ++iter)
        {
            if ((*iter) == process_buffer)
            {
                delete (*iter);
                m_processBuffers.erase(iter);

                SetRebuildLevel(bbl_Redraw);
                //Prepare();

                break;
            }
        }
    }

    void ScrolledWindowBuffer::RemoveProcessBuffer(Process* process)
    {
        RootMapLogTrace("ScrolledWindowBuffer::RemoveProcessBuffer");

        for (ProcessBufferContainer::iterator iter = m_processBuffers.begin();
            iter != m_processBuffers.end();
            ++iter)
        {
            ProcessDrawingBuffer* buffer = (*iter);

            if (buffer->GetParent()->GetProcess() == process)
            {
                // could (should?) do this in ProcessDrawingBuffer destructor
                //buffer->GetParent().RemoveDrawingBuffer(buffer);
                delete buffer;
                m_processBuffers.erase(iter);

                SetRebuildLevel(bbl_Reslap);
                //Prepare();

                break;
            }
        }
    }


    void ScrolledWindowBuffer::AddProcessBuffer(ProcessDrawingBuffer* process_buffer)
    {
        RootMapLogTrace("ScrolledWindowBuffer::AddProcessBuffer");
        m_processBuffers.push_back(process_buffer);
    }


    BufferChangeResponse ScrolledWindowBuffer::CreateProcessBuffers(const rootmap::ViewInformation& viewInfo)
    {
        RootMapLogTrace("ScrolledWindowBuffer::CreateProcessBuffers");
        BufferChangeResponse current_response;
        BufferChangeResponse best_response = bcr_Failure;
        BufferChangeResponse worst_response = bcr_OK;

        const ProcessArray& processes = viewInfo.GetDisplayProcesses();

        // iterate over the processes, making a buffer for each
        for (ProcessArray::const_iterator iter = processes.begin();
            iter != processes.end(); ++iter)
        {
            current_response = CreateProcessBuffer(*iter);

            best_response = (BufferChangeResponse)CSMin(best_response, current_response);
            worst_response = (BufferChangeResponse)CSMax(worst_response, current_response);

            // assumption: if creating this process buffer failed, the rest will
            if (current_response > bcr_OK)
            {
                break;
            }
        }

        // if any of the buffers built, make sure we refresh the window properly
        if (best_response == bcr_OK)
        {
            SetRebuildLevel(bbl_Reslap);
        }

        // if any of the buffers failed, ignore the rest of them
        if (worst_response > bcr_OK)
        {
            // although that means that we don't really need to do anything more...
            // ...maybe one day we will ?
        }

        // if NONE of the buffers were built, choose the most severe response
        if (best_response > bcr_OK)
        {
            best_response = bcr_Failure;
        }

        return (best_response);
    }

    void ScrolledWindowBuffer::Buffer_UnconvertRect(const wxRect& window_rect, DoubleRect& soil_rect)
    {
        RootMapLogTrace("ScrolledWindowBuffer::Buffer_UnconvertRect");
        double scale = GetScale();
        soil_rect.left = window_rect.GetLeft() / scale;
        soil_rect.right = window_rect.GetRight() / scale;
        soil_rect.top = window_rect.GetTop() / scale;
        soil_rect.bottom = window_rect.GetBottom() / scale;
    }

    void ScrolledWindowBuffer::AlignCopyArea(ScoreboardBuffer* buffer, wxRect& dstArea)
    {
        RootMapLogTrace("ScrolledWindowBuffer::AlignCopyArea");

        //LOG_ONDRAW_SIMPLE(dstArea);
        //int x_offset = Buffer_Convert_h(GetHReference());
        //int y_offset = Buffer_Convert_v(GetVReference());
        int x_offset = Buffer_Convert_h(0);
        int y_offset = Buffer_Convert_v(0);

        //LOG_ONDRAW_SIMPLE(wxPoint(x_offset,y_offset));

        if (buffer->IsWholeScoreboardH())
        {
            dstArea.Offset(x_offset, 0);
        }
        if (buffer->IsWholeScoreboardV())
        {
            dstArea.Offset(0, y_offset);
        }

        //LOG_ONDRAW_SIMPLE(dstArea);
    }


    void ScrolledWindowBuffer::PostTimestampNotification()
    {
        // processes should redraw during, box boundaries aren't changed by running the simulation
        if (__nullptr != m_boxColours)
        {
            m_boxColours->SetRebuildLevel(bbl_Redraw);
        }

        SetRebuildLevel(bbl_Reslap);
        //Refresh();
    }


#if defined ROOTMAP_USING_CALCULATEDRAWAREAS
    /*
       CalculateDrawAreas
    minor_area  the intersection of the scoreboards and the window frame
                this is used by the box boundary and colour box buffers
    major_area  the window frame. this is used by the process buffers
    */
    void ScrolledWindowBuffer::CalculateDrawAreas(DoubleRect & minor_area, DoubleRect & major_area, wxDC & dc)
    {
        RootMapLogTrace("ScrolledWindowBuffer::CalculateDrawAreas");
        m_scoreboardCoordinator.GetTotalScoreboardBounds(GetViewDirection(), &minor_area);

        LOG_DEBUG_HERE << "minor_area:" << ToString(minor_area).c_str() << " major_area:" << ToString(major_area).c_str();

        // same calculations as wxScrolledWindow::DoPrepareDC makes
        int ppuX, ppuY, startX, startY;
        m_parent.GetScrollPixelsPerUnit(&ppuX, &ppuY);
        m_parent.GetViewStart(&startX, &startY);
        // absolute values of device origin (non-negative)
        wxPoint device_origin_abs(startX*ppuX, startY*ppuY);

        ScrolledWindowHelper helper(GetViewDirection(), GetScale(), ppuX);
        DoublePt zero_offset_soil;
        helper.ScrollPixel2SoilPoint(device_origin_abs, zero_offset_soil);


        // we use the minimum of a) the scoreboard bounds and b) the window’s frame.
        // First we fetched the scoreboard bounds. If we find that the scoreboard
        // doesn't fit into the window in any particular direction, use the window's frame.
        if (!m_fitsWithinSecondV)
        {
#if defined ROOTMAP_VIEW_FROM_TOP_FIXME
            if (GetViewDirection() != vTop)
#endif // #if defined ROOTMAP_VIEW_FROM_TOP_FIXME
            {
                minor_area->top = CSMax(m_clientBoundsSoil.top, minor_area->top);
                minor_area->bottom = CSMin(m_clientBoundsSoil.bottom, minor_area->bottom);
            }
#if defined ROOTMAP_VIEW_FROM_TOP_FIXME
            else
            {
                minor_area.top = CSMin(m_clientBoundsSoil.top, minor_area.top);
                minor_area.bottom = CSMax(m_clientBoundsSoil.bottom, minor_area.bottom);
            }
#endif // #if defined ROOTMAP_VIEW_FROM_TOP_FIXME
        }

        if (!m_fitsWithinSecondH)
        {
            minor_area.left = CSMax(m_clientBoundsSoil.left, minor_area.left);
            minor_area.right = CSMin(m_clientBoundsSoil.right, minor_area.right);
        }

        major_area = m_clientBoundsSoil;

        minor_area.Offset(zero_offset_soil);
        major_area.Offset(zero_offset_soil);
    }
#endif // #if defined ROOTMAP_USING_CALCULATEDRAWAREAS


#ifdef _CS_DEBUG_REPEATPROCBUFFS_
    void ScrolledWindowBuffer::DumpProcessBufferRepeatRects()
    {
        RootMapLogTrace("ScrolledWindowBuffer::XXXX");
#if 0
        Str255 p_spacer = "\p\t";
        Str255 p_eol = "\p\r";
        char c_spacer[] = "\t";
        char c_eol[] = "\r";
        Str255 temp_pstr;
        char temp_cstr[512] = "\0";

#define WRITE_P_SPACER TCLpstrcat(temp_pstr, p_spacer); TCLptocstrcpy(temp_cstr, temp_pstr); vpb_debug_file->WriteSome((Ptr)temp_cstr, strlen(temp_cstr))
#define WRITE_P_EOL TCLpstrcat(temp_pstr, p_eol); TCLptocstrcpy(temp_cstr, temp_pstr); vpb_debug_file->WriteSome((Ptr)temp_cstr, strlen(temp_cstr))
#define WRITE_C_SPACER strcat(temp_cstr, c_spacer); vpb_debug_file->WriteSome((Ptr)temp_cstr, strlen(temp_cstr))
#define WRITE_C_EOL strcat(temp_cstr, c_eol); vpb_debug_file->WriteSome((Ptr)temp_cstr, strlen(temp_cstr))

        if (!vpb_debug_file)
        {
            FSSpec file_spec;
            char column_titles[] = "source\tdest\r";
            if (gIOManager->MakeOutputFileAndFSSpec("\pRepeatRects.debug", &file_spec, true))
            {
                try
                {
                    vpb_debug_file = new CDataFile;
                    vpb_debug_file->SpecifyFSSpec(&file_spec);
                    vpb_debug_file->Open(fsWrPerm);
                    vpb_debug_file->SetMark(0, fsFromStart);
                    vpb_debug_file->WriteSome((Ptr)column_titles, strlen(column_titles));
                }
                catch (...)
                {
                    wxDELETE(vpb_debug_file);
                }
            }
            else
                return;
        }
        else
        {
            vpb_debug_file->Open(fsWrPerm);
            vpb_debug_file->SetMark(0, fsFromLEOF);
        }

#endif

#if 0
        if (gDEBUGWINDOW == 0) return;

        char src_str[] = "src : ";
        char dst_str[] = "dst : ";
        char column_title[] = "rect : l, t, r, b\r";

        gDEBUGWINDOW->Clear();
        writec_(column_title);

        ViewBuffer* buffer = __nullptr;
        if (m_processBuffers)
            buffer = m_processBuffers;
        else
            return;

        wxRect source_area;
        wxRect destination_area;
        long source_width;
        long destination_width;

        buffer->GetBounds(&source_area);
        source_width = source_area.right - source_area.left;

        if ((GetViewDirection() != vTop) && (DoesRepeat()) && (vbHWhole))
        {
            double height, width;
            long scoreboard_width;
            long source_area_left;

            // fetch the scoreboard width. This is the distance we
            // will move the splat to the right by each time
            m_scoreboardCoordinator.GetTotalScoreboardExtent(GetViewDirection(), &width, &height);
            scoreboard_width = width * GetScale();

            // start by adjusting the top and bottom of the source
            source_area.top = CSMax(source_area.top, vbBoundsX.top);
            source_area.bottom = CSMin(source_area.bottom, vbBoundsX.bottom);
            // match it to the destination
            AlignCopyArea(&source_area, &destination_area);

            // move the destination to the left
            while (destination_area.right > vbBounds.left)
                destination_area.right -= scoreboard_width;

            // but not _past_ the left
            destination_area.right += scoreboard_width;
            destination_area.left = CSMax((destination_area.right - source_width), vbBounds.left);

            // also make sure the source’s left is AT, not past, the left
            source_area_left = source_area.left;
            source_area.left = source_area.right - (destination_area.right - destination_area.left);

            writec_(src_str); writelr_(&source_area); writeln_;
            writec_(dst_str); writer_(&destination_area); writeln_;

            // re-align the source’s left
            source_area.left = source_area_left;

            // adjust the destination's left & right
            destination_area.left = destination_area.right - 1;
            destination_area.right = destination_area.left + source_width;

            // loop from left to right, splatting as we go along
            while (destination_area.left < vbBounds.right)
            {
                // when the right side of the destination area goes past
                // the right side of the window panorama, hold it to there
                if (destination_area.right > vbBounds.right)
                {
                    destination_area.right = vbBounds.right;
                    source_area.right = source_area.left + (destination_area.right - destination_area.left);
                }

                writec_(src_str); writelr_(&source_area); writeln_;
                writec_(dst_str); writer_(&destination_area); writeln_;

                // adjust the destination's left & right
                destination_area.left += scoreboard_width;
                destination_area.right = destination_area.left + source_width;
            }
        }
#endif    
    }
#endif
} /* namespace rootmap */
