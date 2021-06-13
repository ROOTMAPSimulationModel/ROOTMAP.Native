#ifndef ProcessDrawingBuffer_H
#define ProcessDrawingBuffer_H
/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessDrawingBuffer.h
// Purpose:     Declaration of the ProcessDrawingBuffer class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2009-07-07 02:40:29 +0800 (Tue, 07 Jul 2009) $
// $Revision: 67 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/ViewWindow/WrapRepeatBuffer.h"
#include "app/gui/ViewCommon/IProcessDrawingBuffer.h"
#include "app/gui/ViewCommon/Types.h"
#include "core/log/Logger.h"

namespace rootmap
{
    class ProcessDrawing;
    class Scoreboard;
    class ScoreboardCoordinator;

    class ProcessDrawingBuffer
        : public WrapRepeatBuffer
        , public IProcessDrawingBuffer
    {
    public:
        ProcessDrawingBuffer(ProcessDrawing* parent,
            ScoreboardCoordinator& scoord,
            bool fWrap,
            bool fRepeat,
            //RvH: 2D colouration pending
            RmColouration root_colour_mode,
            float base_red,
            float base_green,
            float base_blue,
            const ScoreboardFlags& scoreboardFlags);
        virtual ~ProcessDrawingBuffer();

        ProcessDrawing* GetParent();

        /**
         * Override of ScoreboardBuffer method that requests a scoreboard's
         * buffer to be drawn. This is usually called when a scoreboard is
         * turned on, or a full refresh is requested etc.  So the process
         * responsible for this buffer should do a full redraw.
         *
         * @param
         * @return
         */
        virtual void DrawScoreboard(const DoubleRect& area, Scoreboard* scoreboard);

        /*
         * Drawing commands.
         * The reasons for their being defined again here, instead of
         * overridding VBuffer, are:
         * a) VBuffer's drawing routines are not virtual, to increase speed.
         * b) Redefining them gives a chance to rename them more appropriately
         *    for public use and thus introduce a sensible set of routines.
         * c) With overriding, some of the routines might have become VERY messy
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

        /**
         * @param fi specifies *delta* values, ie. relative position
         */
        virtual void DrawLine(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2);

        /**
         * This function draws a rectangle in either the x, y, or z plane
         * (YZ, XZ or XY).  The "direction" parameter decides in which of the
         * planes the rectangle will be drawn:
         *  vFront  =   XZ
         *  vSide   =   YZ
         *  vTop    =   XY (ie. horizontal)
         *
         * The "place" parameter decides how far along in the "direction" the
         * rectangle goes.
         */
        virtual void DrawRectangle(const ScoreboardStratum&,
            BoxCoordinate* box,
            ViewDirection direction,
            double place);

        /**
         * This function also draws a rectangle in either the x, y, or z plane
         * (YZ, XZ or XY).  Here also, the "direction" parameter decides in
         * which of the planes the rectangle will be drawn.  The bounds of the
         * rectangle are defined by the projection within the box defined by
         * the two points given. If the rectangle is seen obliquely, it is
         * seen at "place". Other documentation may be more explanatory.
         */
        virtual void DrawRectangle(const DoubleBox& soil_box,
            ViewDirection direction,
            double place);

        /** This function is similar to the last, however the place is found
         * implicitly using the appropriate field of the DoubleBox's
         * StartCoordinate
         */
        virtual void DrawRectangle(const DoubleBox& soil_box, ViewDirection direction);

        /** This function is very similar to the last, however the view
         * direction is found implicitly.  If the two points given lie on one
         * of the three prevailing orthogonal planes, it will be drawn as such.
         * If the two coordinates are not in one of the three orthogonal
         * planes, ie. they form a rectangulary prism (or even a cube), it is
         * assumed that the rectangle is horizontal. That is, the 'z' value of
         * the second point will be ignored. */
        virtual void DrawRectangle(const DoubleBox& soil_box);

        /* Four oval drawing routines, similar in usage to the rectangle routines */
        virtual void DrawOval(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place);
        virtual void DrawOval(const DoubleBox& soil_box, ViewDirection direction, double place);
        virtual void DrawOval(const DoubleBox& soil_box, ViewDirection direction);
        virtual void DrawOval(const DoubleBox& soil_box);

        /* Four (4) Rectangle and four Oval PAINTING routines. These draw filled-in shapes */
        virtual void PaintRectangle(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place);
        virtual void PaintRectangle(const DoubleBox& soil_box, ViewDirection direction, double place);
        virtual void PaintRectangle(const DoubleBox& soil_box, ViewDirection direction);
        virtual void PaintRectangle(const DoubleBox& soil_box);

        virtual void PaintOval(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place);
        virtual void PaintOval(const DoubleBox& soil_box, ViewDirection direction, double place);
        virtual void PaintOval(const DoubleBox& soil_box, ViewDirection direction);
        virtual void PaintOval(const DoubleBox& soil_box);

        /* Drawing the dot, in the prevailing and selected colours */
        virtual void DrawDot(DoubleCoordinate& fi);

        // ///////////////////////////////////////////
        //
        // Same as above, but with a specified colour
        //
        // ///////////////////////////////////////////
        virtual void DrawLineTo(const DoubleCoordinate& fi, wxColour& colour);
        virtual void DrawLine(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, wxColour& colour);

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

        virtual void DrawDot(DoubleCoordinate& fi, wxColour& colour);

        virtual void DrawRootSegment(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const long& order, const double& radius, const PlantElementIdentifier& elid);
        virtual void DrawRootSegment(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const double& radius, const PlantElementIdentifier& elid, wxColour& colour);

        virtual void DrawCone(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const double& radius1, const double radius2, wxColour& colour, const ViewDirection& direction, const size_t& stacksNSlices = 64, const bool& wireframe = false);


        virtual void RemoveDot(const DoubleCoordinate& fc);
        // MSA 11.04.14 New, much more efficient way of removing dots
        virtual void RemoveAllDots();
        virtual void RemoveRectangle(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place);

        virtual void SetDeviceContext(wxDC* context);
        virtual wxBitmap& GetBitmap();

        virtual void DissociateFromDrawing();

    private:
        /*
         * called by the above functions. Once they have worked out the
         * projections onto the appropriate planes.  This is useful, because
         * once that maths is done, this process needn't do it again for the
         * other buffers it draws into
         * */
        virtual void Projected_FrameBoxRect(const DoubleBox& soil_box, ViewDirection direction, double place);
        virtual void Projected_FrameBoxOval(const DoubleBox& soil_box, ViewDirection direction, double place);
        virtual void Projected_PaintBoxRect(const DoubleBox& soil_box, ViewDirection direction, double place);
        virtual void Projected_PaintBoxOval(const DoubleBox& soil_box, ViewDirection direction, double place);
        virtual void Projected_DrawProjectedLine(const DoubleBox& soil_box, ViewDirection direction, double place);

        virtual void Projected_FrameBoxRect(const DoubleBox& soil_box, ViewDirection direction, double place, wxColour& colour);
        virtual void Projected_FrameBoxOval(const DoubleBox& soil_box, ViewDirection direction, double place, wxColour& colour);
        virtual void Projected_PaintBoxRect(const DoubleBox& soil_box, ViewDirection direction, double place, wxColour& colour);
        virtual void Projected_PaintBoxOval(const DoubleBox& soil_box, ViewDirection direction, double place, wxColour& colour);
        virtual void Projected_DrawProjectedLine(const DoubleBox& soil_box, ViewDirection direction, double place, wxColour& colour);
    private:
        RootMapLoggerDeclaration();

        ///
        /// member declaration
        ProcessDrawing* m_parent;

        RmColouration m_rootColourMode;

        float m_baseColour[4];
        float m_baseRootRf;
        float m_baseRootGf;
        float m_baseRootBf;

        DoubleCoordinate m_drawpoint;
    }; // class ProcessDrawingBuffer

    inline ProcessDrawing* ProcessDrawingBuffer::GetParent()
    {
        return m_parent;
    }
} /* namespace rootmap */

#endif // #ifndef ProcessDrawingBuffer_H
