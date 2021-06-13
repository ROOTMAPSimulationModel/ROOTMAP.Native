#ifndef ScoreboardBoxBoundaryVertices_H
#define ScoreboardBoxBoundaryVertices_H
/////////////////////////////////////////////////////////////////////////////
// Name:        ScoreboardBoxBoundaryVertices.h
// Purpose:     Declaration of the ScoreboardBoxBoundaryVertices class
// Created:     20-06-2008 05:40:29
// Author:      RvH
// $Date: 2009-07-07 02:40:29 +0800 (Tue, 07 Jul 2009) $
// $Revision: 67 $
// Copyright:   ©2002-2008 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/View3DWindow/ScoreboardVertexBuffer.h"
#include "core/log/Logger.h"

namespace rootmap
{
    class ScoreboardCoordinator;
    class View3DInformation;

    /**
     *
     * Overloads use of ViewInformation DoesBoxColours to mean DoesInnerBoxes
     * (see Issue71)
     */
    class ScoreboardBoxBoundaryVertices : public ScoreboardVertexBuffer
    {
    public:
        ScoreboardBoxBoundaryVertices(
            ScoreboardCoordinator& scoord,
            const View3DInformation& viewInfo);

        virtual ~ScoreboardBoxBoundaryVertices();

        /**
         * Override of ScoreboardBuffer method that requests a scoreboard's
         * buffer to be drawn. This is usually called when a scoreboard is
         * turned on, or a full refresh is requested etc.  So the process
         * responsible for this buffer should do a full redraw.
         */
        virtual void DrawScoreboard(const DoubleRect& area, Scoreboard* scoreboard);

        void SetBoundaries(bool fDraw);
        void SetBoxes(bool fDraw);
        void SetInnerBoxes(bool fDraw);

    private:
        RootMapLoggerDeclaration();

        /**
         */
        bool m_drawBoundaries;

        /**
         */
        bool m_drawBoxes;

        /**
         */
        bool m_drawInnerBoxes;

        /**
         */
        wxColour m_boundaryColour;

        /**
         */
        wxColour m_boxColour;
    }; // class ScoreboardBoxBoundaryVertices
} /* namespace rootmap */

#endif // #ifndef ScoreboardBoxBoundaryVertices_H
