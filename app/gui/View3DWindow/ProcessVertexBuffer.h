#ifndef ProcessVertexBuffer_H
#define ProcessVertexBuffer_H
/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessVertexBuffer.h
// Purpose:     Declaration of the ProcessVertexBuffer class
// Created:     20-06-2008 01:47:29
// Author:      RvH
// $Date: 2009-07-07 02:40:29 +0800 (Tue, 07 Jul 2009) $
// $Revision: 67 $
// Copyright:   ©2002-2008 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/ViewCommon/IProcessDrawingBuffer.h"
#include "app/gui/View3DWindow/ScoreboardVertexBuffer.h"
#include "app/gui/ViewCommon/RootColourationModeUtility.h"

namespace rootmap
{
    class ProcessDrawing;
    class View3DInformation;
    class ScoreboardCoordinator;

    /**
     * ProcessVertexBuffer implements drawing for Processes in 3D.
     *
     * Implements (a.k.a "is-a") ScoreboardVertexBuffer: this means it participates
     * in DrawScoreboard requests for the View3D
     *
     * Implements IProcessDrawingBuffer: this means that it can be drawn to by
     * Processes, that all commands used by processes to draw are used.
     */
    class ProcessVertexBuffer
        : public ScoreboardVertexBuffer
        , public IProcessDrawingBuffer
    {
    public:
        ProcessVertexBuffer(ProcessDrawing* parent,
            ScoreboardCoordinator& scoord,
            const View3DInformation& viewInfo);

        ProcessDrawing* GetParent();

        virtual ~ProcessVertexBuffer();

        /**
         * Override of ScoreboardBuffer method that requests a scoreboard's
         * buffer to be drawn. This is usually called when a scoreboard is
         * turned on, or a full refresh is requested etc.  So the process
         * responsible for this buffer should do a full redraw.
         */
        virtual void DrawScoreboard(const DoubleRect& area, Scoreboard* scoreboard);

        /*
         * Drawing commands.
         */

         /*
          * Without further ado, let us commence with the basic acts of moving
          * to a point and drawing a line to a point
          */

          /**
           * @param fi specifies "absolute" position
           */
        virtual void MovePenTo(const DoubleCoordinate& fi);
        virtual void DrawLineTo(const DoubleCoordinate& fi);
        virtual void DrawLineTo(const DoubleCoordinate& fi, wxColour& colour);

        /**
         * @param fi specifies *delta* values, ie. relative position
         */
        virtual void DrawLine(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2);
        virtual void DrawLine(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, wxColour& colour);

        virtual void DrawDot(DoubleCoordinate& fi);
        virtual void DrawDot(DoubleCoordinate& fi, wxColour& colour);


        /*
         * Other IProcessDrawingBuffer methods, currently unsupported (and unused)
         *
         */

        virtual void DrawRectangle(const ScoreboardStratum&, BoxCoordinate* box, ViewDirection direction, double place);
        virtual void DrawRectangle(const DoubleBox& soil_box, ViewDirection direction, double place);
        virtual void DrawRectangle(const DoubleBox& soil_box, ViewDirection direction);
        virtual void DrawRectangle(const DoubleBox& soil_box);
        virtual void DrawOval(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place);
        virtual void DrawOval(const DoubleBox& soil_box, ViewDirection direction, double place);
        virtual void DrawOval(const DoubleBox& soil_box, ViewDirection direction);
        virtual void DrawOval(const DoubleBox& soil_box);
        virtual void PaintRectangle(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place);
        virtual void PaintRectangle(const DoubleBox& soil_box, ViewDirection direction, double place);
        virtual void PaintRectangle(const DoubleBox& soil_box, ViewDirection direction);
        virtual void PaintRectangle(const DoubleBox& soil_box);
        virtual void PaintOval(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place);
        virtual void PaintOval(const DoubleBox& soil_box, ViewDirection direction, double place);
        virtual void PaintOval(const DoubleBox& soil_box, ViewDirection direction);
        virtual void PaintOval(const DoubleBox& soil_box);
        virtual void DrawRectangle(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place, wxColour& colour);
        virtual void DrawRectangle(const DoubleBox& soil_box, ViewDirection direction, double place, wxColour& colour);
        virtual void DrawRectangle(const DoubleBox& soil_box, ViewDirection direction, wxColour& colour);
        virtual void DrawRectangle(const DoubleBox& soil_box, wxColour& colour);
        virtual void DrawOval(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place, wxColour& colour);
        virtual void DrawOval(const DoubleBox& soil_box, ViewDirection direction, double place, wxColour& colour);
        virtual void DrawOval(const DoubleBox& soil_box, ViewDirection direction, wxColour& colour);
        virtual void DrawOval(const DoubleBox& soil_box, wxColour& colour);
        virtual void PaintRectangle(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place, wxColour& colour);
        virtual void PaintRectangle(const DoubleBox& soil_box, ViewDirection direction, double place, wxColour& colour);
        virtual void PaintRectangle(const DoubleBox& soil_box, ViewDirection direction, wxColour& colour);
        virtual void PaintRectangle(const DoubleBox& soil_box, wxColour& colour);
        virtual void PaintOval(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place, wxColour& colour);
        virtual void PaintOval(const DoubleBox& soil_box, ViewDirection direction, double place, wxColour& colour);
        virtual void PaintOval(const DoubleBox& soil_box, ViewDirection direction, wxColour& colour);
        virtual void PaintOval(const DoubleBox& soil_box, wxColour& colour);

        virtual void RemoveDot(const DoubleCoordinate& fi);
        // MSA 11.04.14 New, much more efficient way of removing dots
        virtual void RemoveAllDots();
        virtual void RemoveRectangle(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place);

        // Root drawing methods
        /**
         *    This method uses the root order to derive root colour
         */
        virtual void DrawRootSegment(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const long& order, const double& radius, const PlantElementIdentifier& elid);
        virtual void DrawRootSegment(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const double& radius, const PlantElementIdentifier& elid, wxColour& colour);

        virtual void DrawCone(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const double& radius1, const double radius2, wxColour& colour, const ViewDirection& direction, const size_t& stacksNSlices = 64, const bool& wireframe = false);

        ScoreboardCoordinator& GetScoreboardCoordinator();

        virtual void DissociateFromDrawing();

    private:

        /**
         * member declaration
         */
        virtual void SetDeviceContext(wxDC* context);
        virtual wxBitmap& GetBitmap();

        /**
         *
         */
        ProcessDrawing* m_parent;

        ScoreboardCoordinator& m_scoreboardCoordinator;

        ///
        RmColouration m_colour_mode;
        float m_baseColour[4];
    }; // class ProcessVertexBuffer


    inline ScoreboardCoordinator& ProcessVertexBuffer::GetScoreboardCoordinator()
    {
        return m_scoreboardCoordinator;
    }
} /* namespace rootmap */

#endif // #ifndef ProcessVertexBuffer_H
