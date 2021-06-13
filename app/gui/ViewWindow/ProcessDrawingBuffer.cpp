/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessDrawingBuffer.cpp
// Purpose:     Implementation of the ProcessDrawingBuffer class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2009-07-10 06:43:25 +0800 (Fri, 10 Jul 2009) $
// $Revision: 69 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/ViewWindow/ProcessDrawingBuffer.h"
#include "app/gui/ViewCommon/RootColourationModeUtility.h"
#include "simulation/process/common/ProcessDrawing.h"
#include "simulation/process/common/Process.h"
#include "simulation/common/SprintUtility.h"


namespace rootmap
{
    RootMapLoggerDefinition(ProcessDrawingBuffer);

    ProcessDrawingBuffer::ProcessDrawingBuffer
    (ProcessDrawing* parent,
        ScoreboardCoordinator& scoord,
        bool fWrap,
        bool fRepeat,
        //RvH: 2D colouration pending
        RmColouration root_colour_mode,
        float base_red,
        float base_green,
        float base_blue,
        const ScoreboardFlags& scoreboardFlags)
        : WrapRepeatBuffer(scoord, fWrap, fRepeat, scoreboardFlags)
        , m_parent(parent)
        , m_rootColourMode(root_colour_mode)
        , m_baseRootRf(base_red)
        , m_baseRootGf(base_green)
        , m_baseRootBf(base_blue)
        //, m_baseColour(base_red,base_green,base_blue,0.0) // error C2536: cannot specify explicit initializer for arrays
    {
        m_baseColour[0] = base_red;
        m_baseColour[1] = base_green;
        m_baseColour[2] = base_blue;
        m_baseColour[3] = 0.0; // Alpha unused at present
        RootMapLoggerInitialisation("rootmap.ProcessDrawingBuffer");
    }

    ProcessDrawingBuffer::~ProcessDrawingBuffer()
    {
        if (m_parent) m_parent->RemoveDrawingBuffer(this);
    }

    void ProcessDrawingBuffer::DrawScoreboard(const DoubleRect& area, Scoreboard* scoreboard)
    {
        if (!m_parent) return;
        LOG_DEBUG << "Drawing Scoreboard {Process:" << m_parent->GetProcess()->GetProcessName()
            << ", Scoreboard:" << SprintUtility::StringOf(scoreboard->GetScoreboardStratum()) << "}";
        m_parent->DrawScoreboard(area, scoreboard, this);
    }

#define PROCESS_BUFFER_SETUP 
    //Boolean needs_enddrawing=false; if (qd.thePort != (GrafPtr)vbPort) { BeginDrawing(); needs_enddrawing=true; }
#define PROCESS_BUFFER_TAKEDOWN
    //if (needs_enddrawing) EndDrawing();
    void ProcessDrawingBuffer::MovePenTo(const DoubleCoordinate& fi)
    {
        PROCESS_BUFFER_SETUP
            WrapBuffer_MoveTo(fi);
        PROCESS_BUFFER_TAKEDOWN
    }

    void ProcessDrawingBuffer::DrawLineTo(const DoubleCoordinate& fi)
    {
        PROCESS_BUFFER_SETUP
            WrapBuffer_LineTo(fi);
        PROCESS_BUFFER_TAKEDOWN
    }

    void ProcessDrawingBuffer::DrawLine(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2)
    {
        PROCESS_BUFFER_SETUP
            WrapBuffer_Line(coord1, coord2);
        PROCESS_BUFFER_TAKEDOWN
    }


    void ProcessDrawingBuffer::DrawRectangle
    (const ScoreboardStratum& stratum,
        BoxCoordinate* bc,
        ViewDirection direction,
        double place)
    {
        /* declare ourselves some local variables. A DoubleBox is declared rather than
        a DoubleBox, because the Engine function uses that type */
        DoubleBox dbox;

        // Get the sides/coordinates of the box given by "bc"
        GetScoreboardCoordinator().GetBoxSoil(&dbox, stratum, bc);

        /* once the box sides are fetched, we needn't fetch them again, so we use a different
        version of this function which uses the pre-fetched box dimensions. That function also
        automatically recurses through the other buffers for us */
        DrawRectangle(dbox, direction, place);
    }

    void ProcessDrawingBuffer::DrawRectangle(const DoubleBox& soil_box, ViewDirection direction, double place)
    {
        // call a lower-level function to draw the rectangle for this buffer
        Projected_FrameBoxRect(soil_box, direction, place);
    }

    void ProcessDrawingBuffer::DrawRectangle(const DoubleBox& soil_box, ViewDirection direction)
    {
        double place;

        if (direction == vSide)
        {
            place = soil_box.front;
        }
        else if (direction == vFront)
        {
            place = soil_box.left;
        }
        else
        {
            place = soil_box.top;
        }

        // draw the rectangle. also recurses through the other buffers
        DrawRectangle(soil_box, direction, place);
    }

    /* as the comments in the .h file say: If the two points given do not fall on one of the three available
    orthogonal planes (X, Y or Z (or YZ, XZ or XY)), in other words they form a rectangular prism, the
    rectangle will be presumed to be horizontal, that is, in the Z (XY) plane, at the level of the first point.
    Now THAT'S a long winded and probably ungrammatical sentence. */
    void ProcessDrawingBuffer::DrawRectangle(const DoubleBox& soil_box)
    {
        ViewDirection view_direction;
        double place;

        if (soil_box.left == soil_box.right)
        {
            view_direction = vSide;
            place = soil_box.left;
        }
        else if (soil_box.front == soil_box.back)
        {
            view_direction = vFront;
            place = soil_box.front;
        }
        else
        {
            view_direction = vTop;
            place = soil_box.top;
        }

        // draw the rectangle. also recurses through the other buffers
        DrawRectangle(soil_box, view_direction, place);
    }

    void ProcessDrawingBuffer::DrawOval
    (const ScoreboardStratum& stratum,
        BoxCoordinate* bc,
        ViewDirection direction,
        double place)
    {
        /* declare ourselves some local variables. A DoubleBox is declared rather than
        a DoubleBox, because the Engine function uses that type */
        DoubleBox dbox;

        // Get the sides/coordinates of the box given by "bc"
        GetScoreboardCoordinator().GetBoxSoil(&dbox, stratum, bc);

        /* once the box sides are fetched, we needn't fetch them again, so we use a different
        version of this function which uses the pre-fetched box dimensions. That function also
        automatically recurses through the other buffers for us */
        DrawOval(dbox, direction, place);
    }

    void ProcessDrawingBuffer::DrawOval(const DoubleBox& soil_box, ViewDirection direction, double place)
    {
        // call a lower-level function to draw the rectangle for this buffer
        Projected_FrameBoxOval(soil_box, direction, place);
    }

    void ProcessDrawingBuffer::DrawOval(const DoubleBox& soil_box, ViewDirection direction)
    {
        double place;

        if (direction == vSide) // then
            place = soil_box.front;
        else if (direction == vFront)
            place = soil_box.left;
        else
            place = soil_box.top;

        // draws and recurses
        DrawOval(soil_box, direction, place);
    }

    /* as the comments in the .h file say: If the two points given do not fall on one of the three available
    orthogonal planes (X, Y or Z (or YZ, XZ or XY)), in other words they form a rectangular prism, the
    rectangle will be presumed to be horizontal, that is, in the Z (XY) plane, at the level of the first point.
    Now THAT'S a long winded and probably ungrammatical sentence. */
    void ProcessDrawingBuffer::DrawOval(const DoubleBox& soil_box)
    {
        ViewDirection view_direction;
        double place;

        if (soil_box.front == soil_box.back)
        {
            view_direction = vSide;
            place = soil_box.front;
        }
        else if (soil_box.left == soil_box.right)
        {
            view_direction = vFront;
            place = soil_box.left;
        }
        else
        {
            view_direction = vTop;
            place = soil_box.top;
        }

        // draws and recurses
        DrawOval(soil_box, view_direction, place);
    }

    void ProcessDrawingBuffer::DrawDot(DoubleCoordinate& fi)
    {
        PROCESS_BUFFER_SETUP

            WrapBuffer_Dot(fi);

        PROCESS_BUFFER_TAKEDOWN
    }

    void ProcessDrawingBuffer::PaintRectangle(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place)
    {
        wxUnusedVar(stratum);
        wxUnusedVar(bc);
        wxUnusedVar(direction);
        wxUnusedVar(place);
    }

    void ProcessDrawingBuffer::PaintRectangle(const DoubleBox& soil_box, ViewDirection direction, double place)
    {
        wxUnusedVar(soil_box);
        wxUnusedVar(direction);
        wxUnusedVar(place);
    }

    void ProcessDrawingBuffer::PaintRectangle(const DoubleBox& soil_box, ViewDirection direction)
    {
        wxUnusedVar(soil_box);
        wxUnusedVar(direction);
    }

    void ProcessDrawingBuffer::PaintRectangle(const DoubleBox& soil_box)
    {
        wxUnusedVar(soil_box);
    }

    void ProcessDrawingBuffer::PaintOval(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place)
    {
        wxUnusedVar(stratum);
        wxUnusedVar(bc);
        wxUnusedVar(direction);
        wxUnusedVar(place);
    }

    void ProcessDrawingBuffer::PaintOval(const DoubleBox& soil_box, ViewDirection direction, double place)
    {
        wxUnusedVar(soil_box);
        wxUnusedVar(direction);
        wxUnusedVar(place);
    }

    void ProcessDrawingBuffer::PaintOval(const DoubleBox& soil_box, ViewDirection direction)
    {
        wxUnusedVar(soil_box);
        wxUnusedVar(direction);
    }

    void ProcessDrawingBuffer::PaintOval(const DoubleBox& soil_box)
    {
        wxUnusedVar(soil_box);
    }

    /* The core code of these specified-colour routines is identical to the prevailing-colour
    routines. The difference is that the buffer's fore colour is set to the specified, and restored
    at the end. Once that is done, they also need to pass on to the next buffer's coloured routine */
    void ProcessDrawingBuffer::DrawLineTo(const DoubleCoordinate& fi, wxColour& colour)
    {
        PROCESS_BUFFER_SETUP
            WrapBuffer_LineTo(fi, colour);
        PROCESS_BUFFER_TAKEDOWN
    }

    void ProcessDrawingBuffer::DrawLine(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, wxColour& colour)
    {
        PROCESS_BUFFER_SETUP
            WrapBuffer_Line(coord1, coord2, colour);
        PROCESS_BUFFER_TAKEDOWN
    }


    void ProcessDrawingBuffer::DrawRectangle
    (const ScoreboardStratum& stratum,
        BoxCoordinate* bc,
        ViewDirection direction,
        double place,
        wxColour& colour)
    {
        /* For comments, see the non-colour-specific version of this function */
        DoubleBox dbox;
        GetScoreboardCoordinator().GetBoxSoil(&dbox, stratum, bc);
        DrawRectangle(dbox, direction, place, colour);
    }

    void ProcessDrawingBuffer::DrawRectangle(const DoubleBox& soil_box, ViewDirection direction, double place, wxColour& colour)
    {
        PROCESS_BUFFER_SETUP
            Projected_FrameBoxRect(soil_box, direction, place, colour);
        PROCESS_BUFFER_TAKEDOWN
    }

    void ProcessDrawingBuffer::DrawRectangle(const DoubleBox& soil_box, ViewDirection direction, wxColour& colour)
    {
        double place;

        if (vSide == direction)
        {
            place = soil_box.front;
        }
        else if (vFront == direction)
        {
            place = soil_box.left;
        }
        else
        {
            place = soil_box.top;
        }

        // calling this automatically recurses through the other buffers
        DrawRectangle(soil_box, direction, place, colour);
    }

    void ProcessDrawingBuffer::DrawRectangle(const DoubleBox& soil_box, wxColour& colour)
    {
        ViewDirection view_direction;
        double place;

        if (soil_box.front == soil_box.back)
        {
            view_direction = vSide;
            place = soil_box.front;
        }
        else if (soil_box.left == soil_box.right)
        {
            view_direction = vFront;
            place = soil_box.left;
        }
        else
        {
            view_direction = vTop;
            place = soil_box.top;
        }

        // calling this automatically recurses through the other buffers
        DrawRectangle(soil_box, view_direction, place, colour);
    }

    void ProcessDrawingBuffer::DrawOval(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place, wxColour& colour)
    {
        /* For comments, see the non-colour-specific version of this function */
        DoubleBox dbox;
        GetScoreboardCoordinator().GetBoxSoil(&dbox, stratum, bc);
        DrawOval(dbox, direction, place, colour);
    }

    void ProcessDrawingBuffer::DrawOval(const DoubleBox& soil_box, ViewDirection direction, double place, wxColour& colour)
    {
        PROCESS_BUFFER_SETUP
            Projected_FrameBoxOval(soil_box, direction, place, colour);
        PROCESS_BUFFER_TAKEDOWN
    }

    void ProcessDrawingBuffer::DrawOval(const DoubleBox& soil_box, ViewDirection direction, wxColour& colour)
    {
        double place;

        if (vSide == direction)
        {
            place = soil_box.front;
        }
        else if (vFront == direction)
        {
            place = soil_box.left;
        }
        else
        {
            place = soil_box.top;
        }

        // draws and recurses
        DrawOval(soil_box, direction, place, colour);
    }

    void ProcessDrawingBuffer::DrawOval(const DoubleBox& soil_box, wxColour& colour)
    {
        ViewDirection view_direction;
        double place;

        if (soil_box.front == soil_box.back)
        {
            view_direction = vSide;
            place = soil_box.front;
        }
        else if (soil_box.left == soil_box.right)
        {
            view_direction = vFront;
            place = soil_box.left;
        }
        else
        {
            view_direction = vTop;
            place = soil_box.top;
        }

        // draws and recurses
        DrawOval(soil_box, view_direction, place, colour);
    }

    void ProcessDrawingBuffer::DrawDot(DoubleCoordinate& fi, wxColour& colour)
    {
        PROCESS_BUFFER_SETUP
            WrapBuffer_Dot(fi, colour);
        PROCESS_BUFFER_TAKEDOWN
    }

    void ProcessDrawingBuffer::RemoveDot(const DoubleCoordinate& fc)
    {
        // MSA 09.10.15 Cheeky 2D cop-out - but just the same as in the original Process modules...
        //DrawDot(fc,*wxWHITE);
        // MSA 11.04.14 Finally updated.
        Unbuffer_Dot(fc);
        // MSA 11.04.15 Aaand reverted. The kludgy first attempt looks better!
        wxColour c = *wxWHITE;
        DoubleCoordinate fcc = fc;
        DrawDot(fcc, c);
    }

    void ProcessDrawingBuffer::RemoveAllDots()
    {
        UnbufferAllDots();
    }

    void ProcessDrawingBuffer::RemoveRectangle(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place)
    {
        // MSA 09.11.05    White overwrite again, as above
        wxColour c = *wxWHITE;
        DrawRectangle(stratum, bc, direction, place, c);
    }


    void ProcessDrawingBuffer::PaintRectangle(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place, wxColour& colour)
    {
        wxUnusedVar(stratum);
        wxUnusedVar(bc);
        wxUnusedVar(direction);
        wxUnusedVar(place);
        wxUnusedVar(colour);
    }

    void ProcessDrawingBuffer::PaintRectangle(const DoubleBox& soil_box, ViewDirection direction, double place, wxColour& colour)
    {
        wxUnusedVar(soil_box);
        wxUnusedVar(direction);
        wxUnusedVar(place);
        wxUnusedVar(colour);
    }

    void ProcessDrawingBuffer::PaintRectangle(const DoubleBox& soil_box, ViewDirection direction, wxColour& colour)
    {
        wxUnusedVar(soil_box);
        wxUnusedVar(direction);
        wxUnusedVar(colour);
    }

    void ProcessDrawingBuffer::PaintRectangle(const DoubleBox& soil_box, wxColour& colour)
    {
        wxUnusedVar(soil_box);
        wxUnusedVar(colour);
    }

    void ProcessDrawingBuffer::PaintOval(const ScoreboardStratum& WXUNUSED(stratum), BoxCoordinate* WXUNUSED(bc), ViewDirection WXUNUSED(direction), double WXUNUSED(place), wxColour& WXUNUSED(colour))
    {
    }

    void ProcessDrawingBuffer::PaintOval(const DoubleBox& soil_box, ViewDirection direction, double place, wxColour& colour)
    {
        wxUnusedVar(soil_box);
        wxUnusedVar(direction);
        wxUnusedVar(place);
        wxUnusedVar(colour);
    }

    void ProcessDrawingBuffer::PaintOval(const DoubleBox& soil_box, ViewDirection direction, wxColour& colour)
    {
        wxUnusedVar(soil_box);
        wxUnusedVar(direction);
        wxUnusedVar(colour);
    }

    void ProcessDrawingBuffer::PaintOval(const DoubleBox& soil_box, wxColour& colour)
    {
        wxUnusedVar(soil_box);
        wxUnusedVar(colour);
    }

    void ProcessDrawingBuffer::Projected_FrameBoxRect(const DoubleBox& soilbox, ViewDirection direction, double place)
    {
        PROCESS_BUFFER_SETUP

            /* if we are looking in the direction such that we will see the rectangle face-on,
            ie. as a rectangle, we can draw that */
            if (direction == GetViewDirection())
            {
                DoubleRect drect;
                DoubleBoxToRect(soilbox, &drect, direction);
                WrapBuffer_FrameRect(drect);
            }
        /* Otherwise, we should just draw a line */
            else
            {
                Projected_DrawProjectedLine(soilbox, direction, place);
            }

        PROCESS_BUFFER_TAKEDOWN
    }

    void ProcessDrawingBuffer::Projected_FrameBoxOval(const DoubleBox& soilbox, ViewDirection direction, double place)
    {
        PROCESS_BUFFER_SETUP

            /* if we are looking in the direction such that we will see the rectangle face-on,
            ie. as a rectangle, we can draw that */
            if (direction == GetViewDirection())
            {
                DoubleRect drect;

                DoubleBoxToRect(soilbox, &drect, direction);
                WrapBuffer_FrameOval(drect);
            }
        /* Otherwise, we should just draw a line */
            else
            {
                Projected_DrawProjectedLine(soilbox, direction, place);
            }

        PROCESS_BUFFER_TAKEDOWN
    }

    void ProcessDrawingBuffer::Projected_PaintBoxRect(const DoubleBox& soilbox, ViewDirection direction, double place)
    {
        PROCESS_BUFFER_SETUP

            /* if we are looking in the direction such that we will see the rectangle face-on,
            ie. as a rectangle, we can draw that */
            if (direction == GetViewDirection())
            {
                DoubleRect drect;

                DoubleBoxToRect(soilbox, &drect, direction);
                WrapBuffer_PaintRect(drect);
            }
        /* Otherwise, we should just draw a line */
            else
            {
                Projected_DrawProjectedLine(soilbox, direction, place);
            }

        PROCESS_BUFFER_TAKEDOWN
    }

    void ProcessDrawingBuffer::Projected_PaintBoxOval(const DoubleBox& soilbox, ViewDirection direction, double place)
    {
        PROCESS_BUFFER_SETUP

            /* if we are looking in the direction such that we will see the rectangle face-on,
            ie. as a rectangle, we can draw that */
            if (direction == GetViewDirection())
            {
                DoubleRect drect;

                DoubleBoxToRect(soilbox, &drect, direction);
                WrapBuffer_PaintOval(drect);
            }
        /* Otherwise, we should just draw a line */
            else
            {
                Projected_DrawProjectedLine(soilbox, direction, place);
            }

        PROCESS_BUFFER_TAKEDOWN
    }

    /*
     * Drawing a projected line is not as easy as is sounds. It involves not
     * just drawing only one of the 12 edges of a rectangular prism (box). It
     * is trickier than that, because the line is drawn somewhere WITHIN the
     * box, parallel to one of the sides.
     *
     * Another aspect of this is that this is the most common way a rectangle
     * will be seen.  It occurs in two-thirds of the viewing/rectangle
     * situations. Plus, viewing a horizontal rectangle from the front or side
     * will almost certainly be very common. WHAT THIS MEANS is that this needs
     * to be fast, and hence, simple.
     *
     * The trick seems to be to project the two defining, diagonally opposite
     * points, along the appropriate edge to the point required. Then draw
     * between the two points, which will now be on opposing edges rather than
     * opposing vertices.
     */
    void ProcessDrawingBuffer::Projected_DrawProjectedLine
    (const DoubleBox& soilbox,
        ViewDirection direction,
        double place)
    {
        PROCESS_BUFFER_SETUP

            DoubleCoordinate start(soilbox.front, soilbox.left, soilbox.top);
        DoubleCoordinate end(soilbox.back, soilbox.right, soilbox.bottom);
        switch (direction)
        {
        case vFront:
            start.x = end.x = place;
            break;
        case vSide:
            start.y = end.y = place;
            break;
        case vTop:
            start.z = end.z = place;
            break;
        default:
            break;
        }

        WrapBuffer_MoveTo(start);
        WrapBuffer_LineTo(end);

        PROCESS_BUFFER_TAKEDOWN
    }


    /*
     * Projected with Colour
     *
     *
     *
     */
    void ProcessDrawingBuffer::Projected_FrameBoxRect(const DoubleBox& soilbox, ViewDirection direction, double place, wxColour& colour)
    {
        PROCESS_BUFFER_SETUP

            /* if we are looking in the direction such that we will see the rectangle face-on,
            ie. as a rectangle, we can draw that */
            if (direction == GetViewDirection())
            {
                DoubleRect drect;
                DoubleBoxToRect(soilbox, &drect, direction);
                WrapBuffer_FrameRect(drect, colour);
            }
        /* Otherwise, we should just draw a line */
            else
            {
                Projected_DrawProjectedLine(soilbox, direction, place, colour);
            }

        PROCESS_BUFFER_TAKEDOWN
    }

    void ProcessDrawingBuffer::Projected_FrameBoxOval(const DoubleBox& soilbox, ViewDirection direction, double place, wxColour& colour)
    {
        PROCESS_BUFFER_SETUP

            /* if we are looking in the direction such that we will see the rectangle face-on,
            ie. as a rectangle, we can draw that */
            if (direction == GetViewDirection())
            {
                DoubleRect drect;

                DoubleBoxToRect(soilbox, &drect, direction);
                WrapBuffer_FrameOval(drect, colour);
            }
        /* Otherwise, we should just draw a line */
            else
            {
                Projected_DrawProjectedLine(soilbox, direction, place, colour);
            }

        PROCESS_BUFFER_TAKEDOWN
    }

    void ProcessDrawingBuffer::Projected_PaintBoxRect(const DoubleBox& soilbox, ViewDirection direction, double place, wxColour& colour)
    {
        PROCESS_BUFFER_SETUP

            /* if we are looking in the direction such that we will see the rectangle face-on,
            ie. as a rectangle, we can draw that */
            if (direction == GetViewDirection())
            {
                DoubleRect drect;

                DoubleBoxToRect(soilbox, &drect, direction);
                WrapBuffer_PaintRect(drect, colour);
            }
        /* Otherwise, we should just draw a line */
            else
            {
                Projected_DrawProjectedLine(soilbox, direction, place, colour);
            }

        PROCESS_BUFFER_TAKEDOWN
    }

    void ProcessDrawingBuffer::Projected_PaintBoxOval(const DoubleBox& soilbox, ViewDirection direction, double place, wxColour& colour)
    {
        PROCESS_BUFFER_SETUP

            /* if we are looking in the direction such that we will see the rectangle face-on,
            ie. as a rectangle, we can draw that */
            if (direction == GetViewDirection())
            {
                DoubleRect drect;

                DoubleBoxToRect(soilbox, &drect, direction);
                WrapBuffer_PaintOval(drect, colour);
            }
        /* Otherwise, we should just draw a line */
            else
            {
                Projected_DrawProjectedLine(soilbox, direction, place, colour);
            }

        PROCESS_BUFFER_TAKEDOWN
    }

    /*
     * Drawing a projected line is not as easy as is sounds. It involves not
     * just drawing only one of the 12 edges of a rectangular prism (box). It
     * is trickier than that, because the line is drawn somewhere WITHIN the
     * box, parallel to one of the sides.
     *
     * Another aspect of this is that this is the most common way a rectangle
     * will be seen.  It occurs in two-thirds of the viewing/rectangle
     * situations. Plus, viewing a horizontal rectangle from the front or side
     * will almost certainly be very common. WHAT THIS MEANS is that this needs
     * to be fast, and hence, simple.
     *
     * The trick seems to be to project the two defining, diagonally opposite
     * points, along the appropriate edge to the point required. Then draw
     * between the two points, which will now be on opposing edges rather than
     * opposing vertices.
     */
    void ProcessDrawingBuffer::Projected_DrawProjectedLine(const DoubleBox& soilbox, ViewDirection direction, double place, wxColour& colour)
    {
        PROCESS_BUFFER_SETUP

            DoubleCoordinate start(soilbox.front, soilbox.left, soilbox.top);
        DoubleCoordinate end(soilbox.back, soilbox.right, soilbox.bottom);
        switch (direction)
        {
        case vFront:
            start.x = end.x = place;
            break;
        case vSide:
            start.y = end.y = place;
            break;
        case vTop:
            start.z = end.z = place;
            break;
        default:
            break;
        }

        WrapBuffer_Line(start, end, colour);

        PROCESS_BUFFER_TAKEDOWN
    }

    void ProcessDrawingBuffer::SetDeviceContext(wxDC* context)
    {
        ViewBuffer::SetDeviceContext(context);
    }

    wxBitmap& ProcessDrawingBuffer::GetBitmap()
    {
        return ViewBuffer::GetBitmap();
    }

    void ProcessDrawingBuffer::DissociateFromDrawing()
    {
        m_parent = __nullptr;
    }

    // Root drawing methods (plant element ID & root order unused at present; may be used in future)

    void ProcessDrawingBuffer::DrawRootSegment(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const long& order, const double& /* radius */, const PlantElementIdentifier& /* elid */)
    {
        float r, g, b;
        RootColourUtility::setColours(m_rootColourMode, order, r, g, b, m_baseColour);
        PROCESS_BUFFER_SETUP
            WrapBuffer_Line(coord1, coord2, wxColour(255.0 * r, 255.0 * g, 255.0 * b));
        PROCESS_BUFFER_TAKEDOWN
    }

    void ProcessDrawingBuffer::DrawRootSegment(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const double& /* radius */, const PlantElementIdentifier& /* elid */, wxColour& colour)
    {
        PROCESS_BUFFER_SETUP
            WrapBuffer_Line(coord1, coord2, colour);
        PROCESS_BUFFER_TAKEDOWN
    }

    void ProcessDrawingBuffer::DrawCone(const DoubleCoordinate& /* coord1 */, const DoubleCoordinate& /* coord2 */, const double& /* radius1 */, const double /* radius2 */, wxColour& /* colour */, const ViewDirection& /* direction */, const size_t& /* stacksNSlices */, const bool& /* wireframe */)
    {
        // MSA 10.07.10 TODO: Implement drawing BoundingCylinder outlines in 2D view
    }
} /* namespace rootmap */
