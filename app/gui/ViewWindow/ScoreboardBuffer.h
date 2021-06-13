#ifndef ScoreboardBuffer_H
#define ScoreboardBuffer_H
/////////////////////////////////////////////////////////////////////////////
// Name:        ScoreboardBuffer.h
// Purpose:     Declaration of the ScoreboardBuffer class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2008-06-16 02:07:36 +0800 (Mon, 16 Jun 2008) $
// $Revision: 1 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/ViewWindow/ViewBuffer.h"

#include "simulation/scoreboard/ScoreboardFlags.h"

#include "core/common/Structures.h"
#include "core/scoreboard/ScoreboardStratum.h"

namespace rootmap
{
    class Scoreboard;
    class ScoreboardCoordinator;

    class ScoreboardBuffer : public ViewBuffer
    {
    public:
        TCL_DECLARE_CLASS

            ScoreboardBuffer(ScoreboardCoordinator& scoord,
                const ScoreboardFlags& scoreboardFlags);

        virtual BufferChangeResponse Create
        (int width,
            int height,
            int depth,
            double scale,
            ViewDirection direction,
            const DoubleRect& scoreboardBounds,
            const DoubleRect& soilFrame);

        virtual ~ScoreboardBuffer();

        bool IsWholeScoreboardH() const;
        bool IsWholeScoreboardV() const;

        /**
         * overridden to ensure ....
         */
        virtual BufferChangeResponse Resize(ResizeBufferChangeInfo* info);

        /**
         * overridden to ensure ....
         */
        virtual BufferChangeResponse SetViewDirection(ViewDirection newDirection, ViewDirectionBufferChangeInfo* info);

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

        /**
         * called once for each scoreboard. By the time this is called, the derived
         * class can be sure that :
         * - the general drawing environment is already setup
         * - the scoreboard *is* supposed to be visible in the window
         */
        virtual void DrawScoreboard(const DoubleRect& area, Scoreboard* scoreboard) = 0;

        bool IsScoreboardVisible(const ScoreboardStratum& stratum) const;

        void SetScoreboard(const ScoreboardStratum& stratum, bool fTurnOn);
        void SetScoreboards(const ScoreboardFlags& flags);

    protected:
        /**
         * For use by the wrap repeat buffer class when it has chosen to create
         * a ViewBuffer, not a ScoreboardBuffer
         */
        void SetWholeness(bool fWholeH, bool fWholeV);

    private:
        void UpdateScoreboardData();

        ScoreboardCoordinator& m_scoreboardCoordinator;
        DoubleRect m_scoreboardBounds;

        //double               m_scoreboardBoundsLeft;
        //double               m_scoreboardBoundsRight;
        //double               m_scoreboardBoundsTop;
        //double               m_scoreboardBoundsBottom;
        double m_scoreboardHeight;
        double m_scoreboardWidth;
        ScoreboardFlags m_visibleScoreboards;
        ///
        /// true if the scoreboard fits completely horizontally in the client
        /// window as yielded by DetermineBufferWholeSizes
        bool m_isWholeH;
        ///
        /// true if the scoreboard fits completely vertically in the client
        /// window as yielded by DetermineBufferWholeSizes
        bool m_isWholeV;
    }; // class ScoreboardBuffer

    inline const ScoreboardCoordinator& ScoreboardBuffer::GetScoreboardCoordinator() const
    {
        return m_scoreboardCoordinator;
    }

    inline ScoreboardCoordinator& ScoreboardBuffer::GetScoreboardCoordinator()
    {
        return m_scoreboardCoordinator;
    }

    inline bool ScoreboardBuffer::IsScoreboardVisible(const ScoreboardStratum& stratum) const
    {
        return (m_visibleScoreboards.test(stratum.value()));
    }

    inline void ScoreboardBuffer::SetScoreboard(const ScoreboardStratum& stratum, bool fTurnOn)
    {
        m_visibleScoreboards.set(stratum.value(), fTurnOn);
    }

    inline void ScoreboardBuffer::SetScoreboards(const ScoreboardFlags& flags)
    {
        m_visibleScoreboards = flags;
    }

    inline bool ScoreboardBuffer::IsWholeScoreboardH() const
    {
        return m_isWholeH;
    }

    inline bool ScoreboardBuffer::IsWholeScoreboardV() const
    {
        return m_isWholeV;
    }

    inline void ScoreboardBuffer::SetWholeness(bool fWholeH, bool fWholeV)
    {
        m_isWholeH = fWholeH;
        m_isWholeV = fWholeV;
    }
} /* namespace rootmap */
#endif // #ifndef ScoreboardBuffer_H
