/////////////////////////////////////////////////////////////////////////////
// Name:        ScoreboardBoxBoundaries.cpp
// Purpose:     Implementation of the ClassTemplate class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2008-06-16 02:07:36 +0800 (Mon, 16 Jun 2008) $
// $Revision: 1 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/ViewWindow/ScoreboardBoxBoundaries.h"
#include "app/gui/common/GuiUtility.h"

#include "simulation/scoreboard/Scoreboard.h"
#include "simulation/scoreboard/ScoreboardCoordinator.h"

#include "core/scoreboard/BoundaryArray.h"
#include "core/utility/Utility.h"

//#define LOG_DRAWING
#if defined LOG_DRAWING
#define LOG_DRAWING_FUNCTION(fn) RootMapLogTrace(#fn)
#define LOG_DRAWING_SIMPLE(x) LOG_DEBUG << ToString(x);
#else
#define LOG_DRAWING_FUNCTION(fn)
#define LOG_DRAWING_SIMPLE(x)
#endif

namespace rootmap
{
    RootMapLoggerDefinition(ScoreboardBoxBoundaries);

    ScoreboardBoxBoundaries::ScoreboardBoxBoundaries
    (ScoreboardCoordinator& scoord,
        const ScoreboardFlags& scoreboardFlags)
        : ScoreboardBuffer(scoord, scoreboardFlags)
        , m_innerColour(*wxLIGHT_GREY)
        , m_outerColour(*wxBLUE)
    {
        RootMapLoggerInitialisation("rootmap.ScoreboardBoxBoundaries");
    }


    BufferChangeResponse ScoreboardBoxBoundaries::Create
    (int width,
        int height,
        int depth,
        double scale,
        ViewDirection direction,
        const DoubleRect& scoreboardBounds,
        const DoubleRect& soilFrame)
    {
        return (ScoreboardBuffer::Create(width, height, depth, scale, direction, scoreboardBounds, soilFrame));
    }

    void ScoreboardBoxBoundaries::DrawScoreboard(const DoubleRect& /* area */, Scoreboard* scoreboard)
    {
        LOG_DRAWING_FUNCTION(ScoreboardBoxBoundaries_DrawScoreboard);

        const ScoreboardStratum& stratum = scoreboard->GetScoreboardStratum();

        if (GetRebuildLevel() <= bbl_Reslap)
        {
            return;
        }

        // //////////////
        // draw outer bounds (should be last)
        DoubleRect soil_rect;
        GetScoreboardCoordinator().GetScoreboardBounds(stratum, GetViewDirection(), &soil_rect);

        LOG_DRAWING_SIMPLE(area);
        LOG_DRAWING_SIMPLE(soil_rect);

        //
        // Vertical boundaries. Note the use of poorly named GuiUtility functions.
        // Or inappropriate functions.  Originally created for RmScoreboardTableHelper,
        // where a horizontal dimension gives the column headers :-(
        const BoundaryArray& boundaries_v = scoreboard->GetBoundaryArray(GuiUtility::getDimensionForColumns(GetViewDirection()));
        size_t num_boundaries_v = boundaries_v.GetNumBoundaries();
        for (size_t boundary_index_v = 1; boundary_index_v <= num_boundaries_v; ++boundary_index_v)
        {
            double boundary_pos_v = boundaries_v.GetBoundary(static_cast<long>(boundary_index_v));
            Buffer_Line(boundary_pos_v, soil_rect.top, boundary_pos_v, soil_rect.bottom, m_innerColour);
        }

        const BoundaryArray& boundaries_h = scoreboard->GetBoundaryArray(GuiUtility::getDimensionForRows(GetViewDirection()));
        size_t num_boundaries_h = boundaries_h.GetNumBoundaries();
        for (size_t boundary_index_h = 1; boundary_index_h <= num_boundaries_h; ++boundary_index_h)
        {
            double boundary_pos_h = boundaries_h.GetBoundary(boundary_index_h);
            Buffer_Line(soil_rect.left, boundary_pos_h, soil_rect.right, boundary_pos_h, m_innerColour);
        }

        //Buffer_FrameRect(soil_rect, m_outerColour);
    }
} /* namespace rootmap */
