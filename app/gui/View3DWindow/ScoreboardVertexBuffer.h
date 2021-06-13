#ifndef ScoreboardVertexBuffer_H
#define ScoreboardVertexBuffer_H
/////////////////////////////////////////////////////////////////////////////
// Name:        ScoreboardVertexBuffer.h
// Purpose:     Declaration of the ScoreboardVertexBuffer class
// Created:     20-03-2008 00:38:09
// Author:      RvH
// $Date: 2009-07-07 02:40:29 +0800 (Tue, 07 Jul 2009) $
// $Revision: 67 $
// Copyright:   ©2002-2008 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/View3DWindow/VertexBuffer.h"
#include "simulation/scoreboard/ScoreboardFlags.h"
#include "core/scoreboard/ScoreboardStratum.h"
#include "core/common/DoubleCoordinates.h"

namespace rootmap
{
    class Scoreboard;
    class ScoreboardStratum;
    class ScoreboardCoordinator;
    class View3DInformation;

    class ScoreboardVertexBuffer : public VertexBuffer
    {
    public:
        virtual ~ScoreboardVertexBuffer();

        /**
         * Required to use the DrawScoreboardIterator template class
         */
        virtual bool IsScoreboardVisible(const ScoreboardStratum& stratum) const;


        /**
         * ScoreboardCoordinator data member accessor
         */
        const ScoreboardCoordinator& GetScoreboardCoordinator() const;

        /**
         * ScoreboardCoordinator data member accessor
         */
        ScoreboardCoordinator& GetScoreboardCoordinator();

        /**
         * iterates over the scoreboards, calling DrawScoreboard() for each
         */
        virtual void DrawContents(const DoubleRect& area);

        void SetScoreboard(const ScoreboardStratum& stratum, bool fTurnOn);
        void SetScoreboards(const ScoreboardFlags& flags);

        /**
         * Required to use the DrawScoreboardIterator template class
         * @param
         */
        virtual void DrawScoreboard(const DoubleRect& area, Scoreboard* scoreboard) = 0;

    protected:
        /**
         * Abstract base class so must implement; only derived classes can call
         * constructor
         */
        ScoreboardVertexBuffer(ScoreboardCoordinator& scoord,
            const View3DInformation& viewInfo);

    private:

        ScoreboardVertexBuffer& operator=(const ScoreboardVertexBuffer&);
        /**
         */
        ScoreboardCoordinator& m_scoreboardCoordinator;

        /**
         */
        ScoreboardFlags m_visibleScoreboards;
    }; // class ScoreboardVertexBuffer

    inline const ScoreboardCoordinator& ScoreboardVertexBuffer::GetScoreboardCoordinator() const
    {
        return m_scoreboardCoordinator;
    }

    inline ScoreboardCoordinator& ScoreboardVertexBuffer::GetScoreboardCoordinator()
    {
        return m_scoreboardCoordinator;
    }

    inline bool ScoreboardVertexBuffer::IsScoreboardVisible(const ScoreboardStratum& stratum) const
    {
        return (m_visibleScoreboards.test(stratum.value()));
    }

    inline void ScoreboardVertexBuffer::SetScoreboard(const ScoreboardStratum& stratum, bool fTurnOn)
    {
        m_visibleScoreboards.set(stratum.value(), fTurnOn);
    }

    inline void ScoreboardVertexBuffer::SetScoreboards(const ScoreboardFlags& flags)
    {
        m_visibleScoreboards = flags;
    }
} /* namespace rootmap */

#endif // #ifndef ScoreboardVertexBuffer_H
