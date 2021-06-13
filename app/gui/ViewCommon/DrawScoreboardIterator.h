#ifndef DrawScoreboardIterator_H
#define DrawScoreboardIterator_H
/////////////////////////////////////////////////////////////////////////////
// Name:        DrawScoreboardIterator.h
// Purpose:     Declaration of the DrawScoreboardIterator class
// Created:     06/06/2006
// Author:      RvH
// $Date: 2008-06-20 10:30:16 +0800 (Fri, 20 Jun 2008) $
// $Revision: 7 $
// Copyright:   ©2006 University of Tasmania
//
/////////////////////////////////////////////////////////////////////////////
#include "simulation/scoreboard/IScoreboardIterationUser.h"
#include "simulation/scoreboard/Scoreboard.h"
#include "core/common/DoubleCoordinates.h"

namespace rootmap
{
    class Scoreboard;
    class ScoreboardBuffer;

    template <typename BUFFER>
    class DrawScoreboardIterator
        : public IScoreboardIterationUser
    {
    private:
        /**
         */
        typename BUFFER& m_buffer;

        /**
         */
        const DoubleRect& m_area;

    public:
        DrawScoreboardIterator(typename BUFFER& buffer, const DoubleRect& area)
            : m_buffer(buffer)
            , m_area(area)
        {
            /* Do Nothing */
        }


        ~DrawScoreboardIterator()
        {
            /* Do Nothing */
        }


        /**
         * Called by the ScoreboardCoordinator for each scoreboard iteration.
         *
         * @param scoreboard - the current iteration of scoreboard to be used
         */
        void UseScoreboard(Scoreboard* scoreboard)
        {
            if (m_buffer.IsScoreboardVisible(scoreboard->GetScoreboardStratum()))
            {
                m_buffer.DrawScoreboard(m_area, scoreboard);
            }
        }

    private:
        /// hidden copy constructor
        DrawScoreboardIterator(const DrawScoreboardIterator&);
        /// hidden assignment operator
        DrawScoreboardIterator& operator=(const DrawScoreboardIterator&);
    }; // class DrawScoreboardIterator
} /* namespace rootmap */

#endif // #ifndef DrawScoreboardIterator_H
