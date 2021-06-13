#ifndef ScoreboardDrawingIterator_H
#define ScoreboardDrawingIterator_H
/////////////////////////////////////////////////////////////////////////////
// Name:        ScoreboardDrawingIterator.h
// Purpose:     Declaration of the ScoreboardProcessDefaultIterator and
//              ScoreboardDrawingIterator classes
// Created:     06/06/2006
// Author:      RvH
// $Date: 2008-06-16 02:07:36 +0800 (Mon, 16 Jun 2008) $
// $Revision: 1 $
// Copyright:   ©2006 University of Tasmania
//
/////////////////////////////////////////////////////////////////////////////
#include "simulation/scoreboard/IScoreboardIterationUser.h"
#include "core/common/DoubleCoordinates.h"

namespace rootmap
{
    class Scoreboard;
    class ScoreboardBuffer;

    class ScoreboardDrawingIterator
        : public IScoreboardIterationUser
    {
    public:
        ScoreboardDrawingIterator(ScoreboardBuffer & buffer, const DoubleRect & area);
        virtual ~ScoreboardDrawingIterator();

        /**
         * UseScoreboard
         *
         * Called by the ScoreboardCoordinator for each scoreboard iteration.
         *
         * @param scoreboard - the current iteration of scoreboard to be used
         */
        virtual void UseScoreboard(Scoreboard * scoreboard);

    private:
        /// hidden copy constructor
        ScoreboardDrawingIterator(const ScoreboardDrawingIterator &);
        /// hidden assignment operator
        ScoreboardDrawingIterator & operator=(const ScoreboardDrawingIterator &);

        ///
        ///
        ScoreboardBuffer & m_buffer;

        ///
        ///
        const DoubleRect & m_area;

    }; // class ScoreboardDrawingIterator

} // namespace rootmap

#endif // #ifndef ScoreboardDrawingIterator_H
