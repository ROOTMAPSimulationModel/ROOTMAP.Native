/////////////////////////////////////////////////////////////////////////////
// Name:        ScoreboardVertexBuffer.cpp
// Purpose:     Implementation of the ScoreboardVertexBuffer class
// Created:     20-03-2008 00:38:09
// Author:      RvH
// $Date: 2009-07-07 02:40:29 +0800 (Tue, 07 Jul 2009) $
// $Revision: 67 $
// Copyright:   ©2002-2008 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/View3DWindow/ScoreboardVertexBuffer.h"
#include "app/gui/ViewCommon/View3DInformation.h"
#include "app/gui/ViewCommon/DrawScoreboardIterator.h"
#include "simulation/scoreboard/ScoreboardCoordinator.h"

namespace rootmap
{
    ScoreboardVertexBuffer::ScoreboardVertexBuffer
    (ScoreboardCoordinator& scoord,
        const View3DInformation& viewInfo)
        : VertexBuffer()
        , m_scoreboardCoordinator(scoord)
        , m_visibleScoreboards(viewInfo.GetScoreboardFlags())
    {
    }

    ScoreboardVertexBuffer::~ScoreboardVertexBuffer()
    {
    }

    void ScoreboardVertexBuffer::DrawContents(const DoubleRect& area)
    {
        DrawScoreboardIterator<ScoreboardVertexBuffer>* iterator = new DrawScoreboardIterator<ScoreboardVertexBuffer>(*this, area);
        m_scoreboardCoordinator.IterateOverScoreboards(iterator);
        delete iterator;
    }

    void ScoreboardVertexBuffer::DrawScoreboard(const DoubleRect& /* area */, Scoreboard* scoreboard)
    {
        SetZMultiplier(scoreboard->GetScoreboardStratum().isBelowZero() ? -1.0 : 1.0);
    }
} /* namespace rootmap */
