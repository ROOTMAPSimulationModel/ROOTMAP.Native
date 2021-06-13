/////////////////////////////////////////////////////////////////////////////
// Name:        ScoreboardBuffer.cpp
// Purpose:     Implementation of the ScoreboardBuffer class
// Created:     DD-MM-YYYY
// Author:      RvH
// $Date: 2009-07-07 02:40:29 +0800 (Tue, 07 Jul 2009) $
// $Revision: 67 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/ViewWindow/ScoreboardBuffer.h"
#include "app/gui/ViewCommon/DrawScoreboardIterator.h"

#include "simulation/scoreboard/ScoreboardCoordinator.h"

namespace rootmap
{
    ScoreboardBuffer::ScoreboardBuffer(ScoreboardCoordinator& scoord,
        const ScoreboardFlags& scoreboardFlags)
        : ViewBuffer()
        , m_scoreboardCoordinator(scoord)
        , m_visibleScoreboards(scoreboardFlags)
        , m_isWholeH(true)
        , m_isWholeV(true)
    {
    }

    ScoreboardBuffer::~ScoreboardBuffer()
    {
    }

    BufferChangeResponse ScoreboardBuffer::Create
    (int /* width */,
        int /* height */,
        int depth,
        double scale,
        ViewDirection direction,
        const DoubleRect& scoreboardBounds,
        const DoubleRect& soilFrame)
    {
        int new_width, new_height;
        DetermineBufferWholeSizes(scoreboardBounds,
            soilFrame,
            m_isWholeH, m_isWholeV,
            new_width, new_height,
            scale);
        return (ViewBuffer::Create(new_width, new_height, depth, scale, direction));
    }

    /* SetViewDirection
    overridden because : VBuffer's does not resize the bitmap to the size
        of the scoreboard[s] in the current direction*/
    BufferChangeResponse ScoreboardBuffer::SetViewDirection
    (ViewDirection newDirection,
        ViewDirectionBufferChangeInfo* info)
    {
        BufferChangeResponse bcr;

        /* If the inherited SetViewDirection function was successful,
        then we can go and do a Resize - which in turn makes us the
        size of the scoreboard in the current direction */
        if ((bcr = ViewBuffer::SetViewDirection(newDirection, info)) == bcr_OK)
        {
            ResizeBufferChangeInfo resize_info;
            resize_info.bci_BufferSize = info->bci_BufferSize;
            resize_info.bci_Scale = info->bci_Scale;
            resize_info.bci_ReferencePt = info->bci_ReferencePt;
            //        resize_info.bci_WindowBounds     = info->bci_WindowBounds;
            resize_info.bci_ScoreboardBounds = info->bci_ScoreboardBounds;
            resize_info.bci_SoilFrame = info->bci_SoilFrame;

            bcr = Resize(&resize_info);
        }

        UpdateScoreboardData();

        return bcr;
    }

    /*
    Overridden/Intercepted because VBuffer::Resize uses the first rect int the info, which
    is the cciPanoramaFrame. We want it to use the cciScoreboardFrame
    */
    BufferChangeResponse ScoreboardBuffer::Resize(ResizeBufferChangeInfo* info)
    {
        // so now find out if we fit into the window
        int bufferV, bufferH;
        DetermineBufferWholeSizes(info->bci_ScoreboardBounds,
            info->bci_SoilFrame,
            m_isWholeH, m_isWholeV,
            bufferH, bufferV,
            info->bci_Scale);

        ResizeBufferChangeInfo resize_info;
        resize_info.bci_BufferSize.SetTopLeft(wxPoint(0, 0));
        resize_info.bci_BufferSize.SetSize(wxSize(bufferH, bufferV));
        resize_info.bci_Scale = (info->bci_Scale > 0.00000001) ? info->bci_Scale : GetScale();
        resize_info.bci_ReferencePt.h = (m_isWholeH) ? 0 : info->bci_ReferencePt.h;
        resize_info.bci_ReferencePt.v = (m_isWholeV) ? 0 : info->bci_ReferencePt.v;
        //    resize_info.bci_WindowBounds     = info->bci_WindowBounds;
        resize_info.bci_ScoreboardBounds = info->bci_ScoreboardBounds;
        resize_info.bci_SoilFrame = info->bci_SoilFrame;

        //TODO: do we need to still offset the buffer size ?
        resize_info.bci_BufferSize.Offset(resize_info.bci_ReferencePt.h * resize_info.bci_Scale,
            resize_info.bci_ReferencePt.v * resize_info.bci_Scale);

        // NOW we're ready to resize by the usual means
        return (ViewBuffer::Resize(&resize_info));
    }

    void ScoreboardBuffer::UpdateScoreboardData()
    {
        m_scoreboardCoordinator.GetTotalScoreboardBounds(GetViewDirection(), &m_scoreboardBounds);

        //m_scoreboardBoundsLeft = soil_rect.left;
        //m_scoreboardBoundsRight = soil_rect.right;
        //m_scoreboardBoundsTop = soil_rect.top;
        //m_scoreboardBoundsBottom = soil_rect.bottom;

        m_scoreboardCoordinator.GetTotalScoreboardExtent(GetViewDirection(), &m_scoreboardWidth, &m_scoreboardHeight);
    }


    void ScoreboardBuffer::DrawContents(const DoubleRect& area)
    {
        DrawScoreboardIterator<ScoreboardBuffer>* iterator = new DrawScoreboardIterator<ScoreboardBuffer>(*this, area);
        m_scoreboardCoordinator.IterateOverScoreboards(iterator);
        delete iterator;
    }
} /* namespace rootmap */
