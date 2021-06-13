/////////////////////////////////////////////////////////////////////////////
// Name:        ScoreboardDrawingIterator.cpp
// Purpose:     Implementation of the ScoreboardDrawingIterator
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2008-06-16 02:07:36 +0800 (Mon, 16 Jun 2008) $
// $Revision: 1 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/ViewWindow/ScoreboardDrawingIterator.h"
#include "app/gui/ViewWindow/ScoreboardBuffer.h"

#include "simulation/scoreboard/Scoreboard.h"

namespace rootmap
{

    ScoreboardDrawingIterator::ScoreboardDrawingIterator(ScoreboardBuffer & buffer, const DoubleRect & area)
        : m_buffer(buffer)
        , m_area(area)
    { /* Do Nothing */
    }


    ScoreboardDrawingIterator::~ScoreboardDrawingIterator()
    { /* Do Nothing */
    }


    void ScoreboardDrawingIterator::UseScoreboard(Scoreboard * scoreboard)
    {
        if (m_buffer.IsScoreboardVisible(scoreboard->GetScoreboardStratum()))
        {
            m_buffer.DrawScoreboard(m_area, scoreboard);
        }
    }

} // namespace rootmap
