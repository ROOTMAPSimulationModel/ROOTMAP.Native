#ifndef IProcessDrawingBuffer_H
#define IProcessDrawingBuffer_H
/////////////////////////////////////////////////////////////////////////////
// Name:        IProcessDrawingBuffer.h
// Purpose:     Declaration of the IProcessDrawingBuffer class
// Created:     15-03-2008
// Author:      RvH
// $Date: 2009-07-07 02:40:29 +0800 (Tue, 07 Jul 2009) $
// $Revision: 67 $
// Copyright:   ©2002-2008 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "core/common/Types.h"
#include "core/common/Structures.h"
#include "core/common/DoubleCoordinates.h"
#include "simulation/common/Types.h"

class wxColour;
class wxBitmap;
class wxDC;

namespace rootmap
{
    class ProcessDrawing;
    class ScoreboardStratum;
    struct BoxCoordinate;

    class IProcessDrawingBuffer
    {
    protected:
        IProcessDrawingBuffer()
        {
        }

    public:
        virtual ~IProcessDrawingBuffer()
        {
        }

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
        virtual void MovePenTo(const DoubleCoordinate& fi) = 0;
        virtual void DrawLineTo(const DoubleCoordinate& fi) = 0;

        /**
         * @param fi specifies *delta* values, ie. relative position
         */
        virtual void DrawLine(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2) = 0;

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
            double place) = 0;

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
            double place) = 0;

        /** This function is similar to the last, however the place is found
         * implicitly using the appropriate field of the DoubleBox's
         * StartCoordinate
         */
        virtual void DrawRectangle(const DoubleBox& soil_box, ViewDirection direction) = 0;

        /** This function is very similar to the last, however the view
         * direction is found implicitly.  If the two points given lie on one
         * of the three prevailing orthogonal planes, it will be drawn as such.
         * If the two coordinates are not in one of the three orthogonal
         * planes, ie. they form a rectangulary prism (or even a cube), it is
         * assumed that the rectangle is horizontal. That is, the 'z' value of
         * the second point will be ignored. */
        virtual void DrawRectangle(const DoubleBox& soil_box) = 0;

        /* Four oval drawing routines, similar in usage to the rectangle routines */
        virtual void DrawOval(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place) = 0;
        virtual void DrawOval(const DoubleBox& soil_box, ViewDirection direction, double place) = 0;
        virtual void DrawOval(const DoubleBox& soil_box, ViewDirection direction) = 0;
        virtual void DrawOval(const DoubleBox& soil_box) = 0;

        /* Four (4) Rectangle and four Oval PAINTING routines. These draw filled-in shapes */
        virtual void PaintRectangle(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place) = 0;
        virtual void PaintRectangle(const DoubleBox& soil_box, ViewDirection direction, double place) = 0;
        virtual void PaintRectangle(const DoubleBox& soil_box, ViewDirection direction) = 0;
        virtual void PaintRectangle(const DoubleBox& soil_box) = 0;

        virtual void PaintOval(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place) = 0;
        virtual void PaintOval(const DoubleBox& soil_box, ViewDirection direction, double place) = 0;
        virtual void PaintOval(const DoubleBox& soil_box, ViewDirection direction) = 0;
        virtual void PaintOval(const DoubleBox& soil_box) = 0;

        /* Drawing the dot, in the prevailing and selected colours */
        virtual void DrawDot(DoubleCoordinate& fi) = 0;

        // ///////////////////////////////////////////
        //
        // Same as above, but with a specified colour
        //
        // ///////////////////////////////////////////
        virtual void DrawLineTo(const DoubleCoordinate& fi, wxColour& colour) = 0;
        virtual void DrawLine(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, wxColour& colour) = 0;

        virtual void DrawRectangle(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place, wxColour& colour) = 0;
        virtual void DrawRectangle(const DoubleBox& soil_box, ViewDirection direction, double place, wxColour& colour) = 0;
        virtual void DrawRectangle(const DoubleBox& soil_box, ViewDirection direction, wxColour& colour) = 0;
        virtual void DrawRectangle(const DoubleBox& soil_box, wxColour& colour) = 0;

        virtual void DrawOval(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place, wxColour& colour) = 0;
        virtual void DrawOval(const DoubleBox& soil_box, ViewDirection direction, double place, wxColour& colour) = 0;
        virtual void DrawOval(const DoubleBox& soil_box, ViewDirection direction, wxColour& colour) = 0;
        virtual void DrawOval(const DoubleBox& soil_box, wxColour& colour) = 0;

        virtual void PaintRectangle(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place, wxColour& colour) = 0;
        virtual void PaintRectangle(const DoubleBox& soil_box, ViewDirection direction, double place, wxColour& colour) = 0;
        virtual void PaintRectangle(const DoubleBox& soil_box, ViewDirection direction, wxColour& colour) = 0;
        virtual void PaintRectangle(const DoubleBox& soil_box, wxColour& colour) = 0;

        virtual void PaintOval(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place, wxColour& colour) = 0;
        virtual void PaintOval(const DoubleBox& soil_box, ViewDirection direction, double place, wxColour& colour) = 0;
        virtual void PaintOval(const DoubleBox& soil_box, ViewDirection direction, wxColour& colour) = 0;
        virtual void PaintOval(const DoubleBox& soil_box, wxColour& colour) = 0;

        virtual void DrawDot(DoubleCoordinate& fi, wxColour& colour) = 0;

        // MSA 09.10.15    Method for removing the dot, if found, at a given DoubleCoordinate
        virtual void RemoveDot(const DoubleCoordinate& fi) = 0;
        // MSA 11.04.14 New, much more efficient way of removing dots
        virtual void RemoveAllDots() = 0;
        virtual void RemoveRectangle(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place) = 0;

        /**
         *  MSA These DrawRootSegment() methods are my attempt to disassociate the Plant class
         *  from the specifics of drawing. Implementors of this interface should direct
         *  calls to DrawRootSegment() according to their drawing type.
         *  E.g. ProcessDrawingBuffer will buffer lines (with or without calculating colour)
         *  and ProcessVertexBuffer will buffer cylinders or lines, according to config setting.
         *
         *  Any class implementing IProcessDrawingBuffer but not involved in drawing roots (if that ever occurs)
         *  should just provide an empty implementation of these methods.
         */
        virtual void DrawRootSegment(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const long& order, const double& radius, const PlantElementIdentifier& elid) = 0;
        virtual void DrawRootSegment(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const double& radius, const PlantElementIdentifier& elid, wxColour& colour) = 0;

        virtual void DrawCone(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const double& radius1, const double radius2, wxColour& colour, const ViewDirection& direction, const size_t& stacksNSlices = 64, const bool& wireframe = false) = 0;

        virtual void SetDeviceContext(wxDC* context) = 0;
        virtual wxBitmap& GetBitmap() = 0;

        /**
         *    MSA 09.12.04 New method for dissociating the ProcessDrawingBuffer from the ProcessDrawing.
         *    This becomes necessary when the ProcessDrawing is deleted (from the Process dtor)
         *    out from underneath the ProcessDrawingBuffer,
         *    as when the [GUI] program is exited.
         *  Should be called by ProcessDrawing dtor.
         */
        virtual void DissociateFromDrawing() = 0;
    }; // class IProcessDrawingBuffer
} /* namespace rootmap */

#endif // #ifndef IProcessDrawingBuffer_H
