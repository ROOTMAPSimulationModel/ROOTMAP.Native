/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessDrawing.cpp
// Purpose:     Implementation of the ProcessDrawing class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2009-07-10 06:43:25 +0800 (Fri, 10 Jul 2009) $
// $Revision: 69 $
// Copyright:   ©2006 University of Tasmania
//
// IMPORTANT NOTE: This class breaks the file-placement paradigm by not being
// with its header file. The reasoning behind this is that i wish to keep 
// process drawing efficient, so do not wish to have an abstract interface in
// simulation/process/common/src that is implemented in app, where the drawing
// logic actually is. The intent is to have a command-line app for batch runs
// that does no drawing.
//
// The logic is split here because it would not be so easy at the IProcessDrawingBuffer
// level.
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/ViewCommon/IProcessDrawingBuffer.h"

#include "simulation/process/common/ProcessDrawing.h"
#include "simulation/process/common/Process.h"

#include "core/common/RmAssert.h"

namespace rootmap
{
    ProcessDrawing::ProcessDrawing(Process* p)
        : m_process(p)
        , m_singleBuffer(__nullptr)
    {
    }

    ProcessDrawing::~ProcessDrawing()
    {
        for (ProcessBufferContainer::iterator iter = m_drawingBuffers.begin();
            iter != m_drawingBuffers.end(); ++iter)
        {
            (*iter)->DissociateFromDrawing();
        }
        // Nothing to delete, as this class does not own any of its pointer data members
    }

    void ProcessDrawing::BeginDrawing(IProcessDrawingBuffer* /* buffer */)
    {
        //wxMemoryDC memDC;
        //memDC.SelectObject(buffer->GetBitmap());
        //buffer->SetDeviceContext(&memDC);
    }

    void ProcessDrawing::EndDrawing(IProcessDrawingBuffer* /* buffer */)
    {
        //buffer->SetDeviceContext(__nullptr);
        //memDC.SelectObject(wxNullBitmap);
    }

    void ProcessDrawing::AddDrawingBuffer(IProcessDrawingBuffer* buffer)
    {
        m_drawingBuffers.push_back(buffer);
    }

    void ProcessDrawing::RemoveDrawingBuffer(IProcessDrawingBuffer* buffer)
    {
        for (ProcessBufferContainer::iterator iter = m_drawingBuffers.begin();
            iter != m_drawingBuffers.end();
            ++iter
            )
        {
            if ((*iter) == buffer)
            {
                m_drawingBuffers.erase(iter);
                break;
            }
        }

        if (buffer == m_singleBuffer)
        {
            m_singleBuffer = __nullptr;
        }
    }

    void ProcessDrawing::DrawScoreboard(const DoubleRect& area, Scoreboard* scoreboard, IProcessDrawingBuffer* buffer)
    {
        if (__nullptr != buffer)
        {
            RmAssert(__nullptr == m_singleBuffer, "Already in single-buffer drawing mode");
            m_singleBuffer = buffer;
        }

        m_process->DrawScoreboard(area, scoreboard);

        m_singleBuffer = __nullptr;
    }


#define ROOTMAP_PROCESS_BUFFERS_FIXED
#if defined ROOTMAP_PROCESS_BUFFERS_FIXED
    //#define PROCESS_BUFFER_SETUP Boolean needs_enddrawing=false; if (qd.thePort != (GrafPtr)vbPort) { BeginDrawing(); needs_enddrawing=true; }
    //#define PROCESS_BUFFER_TAKEDOWN if (needs_enddrawing) EndDrawing();

#define PROCESS_BUFFER_SETUP
#define PROCESS_BUFFER_TEARDOWN

    //#define BITMAPSETUP_BEGINENDRAWING
#if defined BITMAPSETUP_BEGINENDRAWING
#define PROCESS_BITMAP_SETUP(b) BeginDrawing(b);
#define PROCESS_BITMAP_TEARDOWN(b) EndDrawing(b);
#else
#define PROCESS_BITMAP_SETUP(b) wxMemoryDC memDC; memDC.SelectObject(b->GetBitmap()); b->SetDeviceContext(&memDC); 
#define PROCESS_BITMAP_TEARDOWN(b)  b->SetDeviceContext(__nullptr); memDC.SelectObject(wxNullBitmap);

#endif // #if defined BITMAPSETUP_BEGINENDRAWING


    //#define PROCESS_BUFFER_SETUP Boolean needs_enddrawing=false; if (qd.thePort != (GrafPtr)vbPort) { BeginDrawing(); needs_enddrawing=true; }
    //#define PROCESS_BUFFER_TAKEDOWN if (needs_enddrawing) EndDrawing();

#define PROCESS_BUFFER_ITERATION_0(function) \
    PROCESS_BUFFER_SETUP \
    if (0!=m_singleBuffer) \
    { \
        m_singleBuffer->function(); \
    } \
    else \
    { \
        for ( ProcessBufferContainer::iterator iter = m_drawingBuffers.begin(); \
                iter != m_drawingBuffers.end() ; \
                ++iter \
            ) \
        { \
            PROCESS_BITMAP_SETUP((*iter)) \
            (*iter)->function(); \
            PROCESS_BITMAP_TEARDOWN((*iter)) \
        } \
    } \
    PROCESS_BUFFER_TEARDOWN


#define PROCESS_BUFFER_ITERATION_1(function, arg1) \
    PROCESS_BUFFER_SETUP \
    if (0!=m_singleBuffer) \
    { \
        m_singleBuffer->function(arg1); \
    } \
    else \
    { \
        for ( ProcessBufferContainer::iterator iter = m_drawingBuffers.begin(); \
                iter != m_drawingBuffers.end() ; \
                ++iter \
            ) \
        { \
            PROCESS_BITMAP_SETUP((*iter)) \
            (*iter)->function(arg1); \
            PROCESS_BITMAP_TEARDOWN((*iter)) \
        } \
    } \
    PROCESS_BUFFER_TEARDOWN


#define PROCESS_BUFFER_ITERATION_2(function, arg1, arg2) \
    PROCESS_BUFFER_SETUP \
    if (0!=m_singleBuffer) \
    { \
        m_singleBuffer->function(arg1, arg2); \
    } \
    else \
    { \
        for ( ProcessBufferContainer::iterator iter = m_drawingBuffers.begin(); \
                iter != m_drawingBuffers.end() ; \
                ++iter \
            ) \
        { \
            PROCESS_BITMAP_SETUP((*iter)) \
            (*iter)->function(arg1, arg2); \
            PROCESS_BITMAP_TEARDOWN((*iter)) \
        } \
    } \
    PROCESS_BUFFER_TEARDOWN


#define PROCESS_BUFFER_ITERATION_3(function, arg1, arg2, arg3) \
    PROCESS_BUFFER_SETUP \
    if (0!=m_singleBuffer) \
    { \
        m_singleBuffer->function(arg1, arg2, arg3); \
    } \
    else \
    { \
        for ( ProcessBufferContainer::iterator iter = m_drawingBuffers.begin(); \
                iter != m_drawingBuffers.end() ; \
                ++iter \
            ) \
        { \
            PROCESS_BITMAP_SETUP((*iter)) \
            (*iter)->function(arg1, arg2, arg3); \
            PROCESS_BITMAP_TEARDOWN((*iter)) \
        } \
    } \
    PROCESS_BUFFER_TEARDOWN


#define PROCESS_BUFFER_ITERATION_4(function, arg1, arg2, arg3, arg4) \
    PROCESS_BUFFER_SETUP \
    if (0!=m_singleBuffer) \
    { \
        m_singleBuffer->function(arg1, arg2, arg3, arg4); \
    } \
    else \
    { \
        for ( ProcessBufferContainer::iterator iter = m_drawingBuffers.begin(); \
                iter != m_drawingBuffers.end() ; \
                ++iter \
            ) \
        { \
            PROCESS_BITMAP_SETUP((*iter)) \
            (*iter)->function(arg1, arg2, arg3, arg4); \
            PROCESS_BITMAP_TEARDOWN((*iter)) \
        } \
    } \
    PROCESS_BUFFER_TEARDOWN


#define PROCESS_BUFFER_ITERATION_5(function, arg1, arg2, arg3, arg4, arg5) \
    PROCESS_BUFFER_SETUP \
    if (0!=m_singleBuffer) \
    { \
        m_singleBuffer->function(arg1, arg2, arg3, arg4, arg5); \
    } \
    else \
    { \
        for ( ProcessBufferContainer::iterator iter = m_drawingBuffers.begin(); \
                iter != m_drawingBuffers.end() ; \
                ++iter \
            ) \
        { \
            PROCESS_BITMAP_SETUP((*iter)) \
            (*iter)->function(arg1, arg2, arg3, arg4, arg5); \
            PROCESS_BITMAP_TEARDOWN((*iter)) \
        } \
    } \
    PROCESS_BUFFER_TEARDOWN

#define PROCESS_BUFFER_ITERATION_8(function, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
    PROCESS_BUFFER_SETUP \
    if (0!=m_singleBuffer) \
    { \
        m_singleBuffer->function(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8); \
    } \
    else \
    { \
        for ( ProcessBufferContainer::iterator iter = m_drawingBuffers.begin(); \
                iter != m_drawingBuffers.end() ; \
                ++iter \
            ) \
        { \
            PROCESS_BITMAP_SETUP((*iter)) \
            (*iter)->function(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8); \
            PROCESS_BITMAP_TEARDOWN((*iter)) \
        } \
    } \
    PROCESS_BUFFER_TEARDOWN


#else
#define PROCESS_BUFFER_ITERATION_1(function, arg1)
#define PROCESS_BUFFER_ITERATION_2(function, arg1, arg2)
#define PROCESS_BUFFER_ITERATION_3(function, arg1, arg2, arg3)
#define PROCESS_BUFFER_ITERATION_4(function, arg1, arg2, arg3, arg4)
#define PROCESS_BUFFER_ITERATION_5(function, arg1, arg2, arg3, arg4, arg5)
#define PROCESS_BUFFER_ITERATION_6(function, arg1, arg2, arg3, arg4, arg5, arg6)
#endif // #if defined ROOTMAP_PROCESS_BUFFERS_FIXED


    void ProcessDrawing::MovePenTo(const DoubleCoordinate& fi)
    {
        PROCESS_BUFFER_ITERATION_1(MovePenTo, fi)
    }

    void ProcessDrawing::DrawLineTo(const DoubleCoordinate& fi)
    {
        PROCESS_BUFFER_ITERATION_1(DrawLineTo, fi)
    }

    void ProcessDrawing::DrawLine(const DoubleCoordinate& from_coord, const DoubleCoordinate& to_coord)
    {
        PROCESS_BUFFER_ITERATION_2(DrawLine, from_coord, to_coord)
    }

    void ProcessDrawing::DrawRectangle
    (const ScoreboardStratum& stratum,
        BoxCoordinate* bc,
        ViewDirection direction,
        double place)
    {
        PROCESS_BUFFER_ITERATION_4(DrawRectangle, stratum, bc, direction, place)
    }

    void ProcessDrawing::DrawRectangle(const DoubleBox& soil_box, ViewDirection direction, double place)
    {
        PROCESS_BUFFER_ITERATION_3(DrawRectangle, soil_box, direction, place)
    }

    void ProcessDrawing::DrawRectangle(const DoubleBox& soil_box, ViewDirection direction)
    {
        PROCESS_BUFFER_ITERATION_2(DrawRectangle, soil_box, direction)
    }

    /*
     * as the comments in the .h file say: If the two points given do not fall
     * on one of the three available orthogonal planes
     *          X, Y or Z (a.k.a. YZ, XZ or XY)
     * in other words they form a rectangular prism, the rectangle will be
     * presumed to be horizontal. That is, in the Z (XY) plane, at the level of
     * the first point.
     */
    void ProcessDrawing::DrawRectangle(const DoubleBox& soil_box)
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

    void ProcessDrawing::DrawOval(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place)
    {
        PROCESS_BUFFER_ITERATION_4(DrawOval, stratum, bc, direction, place)
    }

    void ProcessDrawing::DrawOval(const DoubleBox& soil_box, ViewDirection direction, double place)
    {
        PROCESS_BUFFER_ITERATION_3(DrawOval, soil_box, direction, place)
    }

    void ProcessDrawing::DrawOval(const DoubleBox& soil_box, ViewDirection direction)
    {
        PROCESS_BUFFER_ITERATION_2(DrawOval, soil_box, direction)
    }

    /* as the comments in the .h file say: If the two points given do not fall on one of the three available
    orthogonal planes (X, Y or Z (or YZ, XZ or XY)), in other words they form a rectangular prism, the
    rectangle will be presumed to be horizontal, that is, in the Z (XY) plane, at the level of the first point.
    Now THAT'S a long winded and probably ungrammatical sentence. */
    void ProcessDrawing::DrawOval(const DoubleBox& soil_box)
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

    void ProcessDrawing::DrawDot(DoubleCoordinate& fi)
    {
        PROCESS_BUFFER_ITERATION_1(DrawDot, fi)
    }


    /* The core code of these specified-colour routines is identical to the prevailing-colour
    routines. The difference is that the buffer's fore colour is set to the specified, and restored
    at the end. Once that is done, they also need to pass on to the next buffer's coloured routine */
    void ProcessDrawing::DrawLineTo(const DoubleCoordinate& fi, wxColour& colour)
    {
        PROCESS_BUFFER_ITERATION_2(DrawLineTo, fi, colour)
    }

    void ProcessDrawing::DrawLine(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, wxColour& colour)
    {
        PROCESS_BUFFER_ITERATION_3(DrawLine, coord1, coord2, colour)
    }

    void ProcessDrawing::DrawRectangle
    (const ScoreboardStratum& stratum,
        BoxCoordinate* bc,
        ViewDirection direction,
        double place,
        wxColour& colour)
    {
        PROCESS_BUFFER_ITERATION_5(DrawRectangle, stratum, bc, direction, place, colour)
    }

    void ProcessDrawing::DrawRectangle(const DoubleBox& soil_box, ViewDirection direction, double place, wxColour& colour)
    {
        PROCESS_BUFFER_ITERATION_4(DrawRectangle, soil_box, direction, place, colour)
    }

    void ProcessDrawing::DrawRectangle(const DoubleBox& soil_box, ViewDirection direction, wxColour& colour)
    {
        PROCESS_BUFFER_ITERATION_3(DrawRectangle, soil_box, direction, colour)
    }

    void ProcessDrawing::DrawRectangle(const DoubleBox& soil_box, wxColour& colour)
    {
        PROCESS_BUFFER_ITERATION_2(DrawRectangle, soil_box, colour)
    }

    void ProcessDrawing::DrawOval(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection direction, double place, wxColour& colour)
    {
        PROCESS_BUFFER_ITERATION_5(DrawOval, stratum, bc, direction, place, colour)
    }

    void ProcessDrawing::DrawOval(const DoubleBox& soil_box, ViewDirection direction, double place, wxColour& colour)
    {
        PROCESS_BUFFER_ITERATION_4(DrawOval, soil_box, direction, place, colour)
    }

    void ProcessDrawing::DrawOval(const DoubleBox& soil_box, ViewDirection direction, wxColour& colour)
    {
        PROCESS_BUFFER_ITERATION_3(DrawOval, soil_box, direction, colour)
    }

    void ProcessDrawing::DrawOval(const DoubleBox& soil_box, wxColour& colour)
    {
        PROCESS_BUFFER_ITERATION_2(DrawOval, soil_box, colour)
    }

    void ProcessDrawing::DrawDot(DoubleCoordinate& fi, wxColour& colour)
    {
        PROCESS_BUFFER_ITERATION_2(DrawDot, fi, colour)
    }

    void ProcessDrawing::RemoveDot(const DoubleCoordinate& fi)
    {
        PROCESS_BUFFER_ITERATION_1(RemoveDot, fi)
    }

    void ProcessDrawing::RemoveAllDots()
    {
        PROCESS_BUFFER_ITERATION_0(RemoveAllDots)
    }

    void ProcessDrawing::RemoveRectangle
    (const ScoreboardStratum& stratum,
        BoxCoordinate* bc,
        ViewDirection direction,
        double place)
    {
        PROCESS_BUFFER_ITERATION_4(RemoveRectangle, stratum, bc, direction, place)
    }

    void ProcessDrawing::DrawRootSegment(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const long& order, const double& radius, const PlantElementIdentifier& elid)
    {
        PROCESS_BUFFER_ITERATION_5(DrawRootSegment, coord1, coord2, order, radius, elid)
    }

    void ProcessDrawing::DrawRootSegment(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const double& radius, const PlantElementIdentifier& elid, wxColour& colour)
    {
        PROCESS_BUFFER_ITERATION_5(DrawRootSegment, coord1, coord2, radius, elid, colour)
    }

    void ProcessDrawing::DrawCone(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const double& radius1, const double& radius2, wxColour& colour, const ViewDirection& direction, const size_t& stacksNSlices, const bool& wireframe)
    {
        PROCESS_BUFFER_ITERATION_8(DrawCone, coord1, coord2, radius1, radius2, colour, direction, stacksNSlices, wireframe)
    }
} /* namespace rootmap */
