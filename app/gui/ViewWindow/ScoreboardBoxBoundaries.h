#ifndef ScoreboardBoxBoundaries_H
#define ScoreboardBoxBoundaries_H
/////////////////////////////////////////////////////////////////////////////
// Name:        ScoreboardBoxBoundaries.h
// Purpose:     Declaration of the ClassTemplate class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2008-06-16 02:07:36 +0800 (Mon, 16 Jun 2008) $
// $Revision: 1 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/ViewWindow/ScoreboardBuffer.h"
#include "core/log/Logger.h"

#include "wx/colour.h"

namespace rootmap
{
    class ScoreboardBoxBoundaries : public ScoreboardBuffer
    {
    public:
        ScoreboardBoxBoundaries(ScoreboardCoordinator& scoord,
            const ScoreboardFlags& scoreboardFlags);

        virtual void DrawScoreboard(const DoubleRect& area, Scoreboard* scoreboard);
        virtual BufferChangeResponse Create
        (int width,
            int height,
            int depth,
            double scale,
            ViewDirection direction,
            const DoubleRect& scoreboardBounds,
            const DoubleRect& soilFrame);

    private:
        RootMapLoggerDeclaration();

        wxColour m_innerColour;
        wxColour m_outerColour;
    };
} /* namespace rootmap */

#endif // #ifndef ScoreboardBoxBoundaries_H
