/////////////////////////////////////////////////////////////////////////////
// Name:        ClassTemplate.cpp
// Purpose:     Implementation of the ClassTemplate class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2009-07-10 06:43:25 +0800 (Fri, 10 Jul 2009) $
// $Revision: 69 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/ViewWindow/ViewBuffer.h"

#include "simulation/process/common/ProcessDrawingException.h"

#include "core/common/DoubleCoordinates.h"
#include "core/common/RmAssert.h"
#include "core/common/Structures.h"

#include "wx/dc.h"
#include "wx/dcmemory.h"

#include <limits.h>

//#define LOG_PRIMITIVES
#if defined LOG_PRIMITIVES
#define LOG_PRIMITIVE_FUNCTION(fn) LOG_FUNCTION(fn)
#define LOG_PRIMITIVE_SIMPLE(x) LOG_SIMPLE(x)
#else
#define LOG_PRIMITIVE_FUNCTION(fn)
#define LOG_PRIMITIVE_SIMPLE(x)
#endif

//#define LOG_COPYRECTS
#if defined LOG_COPYRECTS
#define LOG_COPYRECT_FUNCTION(fn) LOG_FUNCTION(fn)
#define LOG_COPYRECT_SIMPLE(x) LOG_SIMPLE(x)
#else
#define LOG_COPYRECT_FUNCTION(fn)
#define LOG_COPYRECT_SIMPLE(x)
#endif

namespace rootmap
{
    RootMapLoggerDefinition(ViewBuffer);

    ColourSet ViewBuffer::m_globalColoursInUse = ColourSet();

    ViewBuffer::ViewBuffer()
        : m_bitmap(__nullptr)
        , m_bounds(0, 0, 0, 0)
        , m_xferMode(wxCOPY)
        , m_unprepared(true)
        , m_begunDrawingRecursionLevel(0)
        , m_xReference(0)
        , m_yReference(0)
        , m_currentPositionX(0.0)
        , m_currentPositionY(0.0)
        , m_xWhole(false)
        , m_yWhole(false)
        , m_soilScale(0.0)
        , m_viewDirection(vNONE)
        , m_rebuildLevel(bbl_Complete)
        , m_activeDC(__nullptr)
        , m_dcPen(new wxPen(*wxBLACK_PEN))
        , m_dcBrush(__nullptr)
    {
        RootMapLoggerInitialisation("rootmap.ViewBuffer");
        m_dcBrush = new wxBrush(*wxBLACK, wxBRUSHSTYLE_TRANSPARENT);
    }

    ViewBuffer::ViewBuffer
    (int width,
        int height,
        int depth,
        double scale,
        ViewDirection direction)
        : m_bitmap(__nullptr)
        , m_bounds(0, 0, width, height)
        , m_xferMode(wxCOPY)
        , m_unprepared(true)
        , m_begunDrawingRecursionLevel(0)
        , m_xReference(0)
        , m_yReference(0)
        , m_currentPositionX(0.0)
        , m_currentPositionY(0.0)
        , m_xWhole(false)
        , m_yWhole(false)
        , m_soilScale(scale)
        , m_viewDirection(direction)
        , m_rebuildLevel(bbl_Complete)
        , m_activeDC(__nullptr)
        , m_dcPen(new wxPen(*wxBLACK_PEN))
        , m_dcBrush(__nullptr)
    {
        RootMapLoggerInitialisation("rootmap.ViewBuffer");
        m_dcBrush = new wxBrush(*wxBLACK, wxBRUSHSTYLE_TRANSPARENT);
        Create(width, height, depth, scale, direction);
    }

    /*
    */
    BufferChangeResponse ViewBuffer::Create
    (int width,
        int height,
        int depth,
        double scale,
        ViewDirection direction)
    {
        //LOG_FUNCTION(ViewBuffer_Create);

        m_bounds = wxRect(0, 0, width, height);
        //LOG_SIMPLE(m_bounds);

        m_bitmap = new wxBitmap(width, height, depth);
        RmAssert(__nullptr != m_bitmap, "Could not create Bitmap");

        m_soilScale = scale;
        m_viewDirection = direction;

        return (bcr_OK);
    }


    /*
    The destructor
    */
    ViewBuffer::~ViewBuffer()
    {
        delete m_dcPen;
        m_dcPen = __nullptr;
        // MSA OK to delete this here?
        // Allocate on construction, deallocate on destruction and all that?
        delete m_bitmap;
        m_bitmap = __nullptr;
    }


    const wxRect& ViewBuffer::GetBounds()
    {
        return m_bounds;
    }


    void ViewBuffer::SetBoundsOrigin(long xOrigin, long yOrigin)
    {
        wxRect newBounds(m_bounds);
        newBounds.SetPosition(wxPoint(xOrigin, yOrigin));

        SetBounds(newBounds);
    }


    /*
    SetBounds
    Sets all known relevant data members PLUS pixmap bounds
    95.08.04    also sets m_bitmap's bitmapRect
    95.08.28    uses a wxRect which isn't necessarily zeroed.
    95.08.29    Adjusts the m_boundsOffset rect before setting other data members
            Also sets vb[H|V]Reference. Similarly modified SetReferences()
     */
    void ViewBuffer::SetBounds(const wxRect& newBounds)
    {
        m_bounds = newBounds;
        m_bounds.SetPosition(wxPoint(0, 0));
        //LOG_SIMPLE(m_bounds);

        // TODO: Resize() ?
    }

    int ViewBuffer::GetPixelDepth() const
    {
        return m_bitmap->GetDepth();
    }


    BufferChangeResponse ViewBuffer::Resize(ResizeBufferChangeInfo* info)
    {
        wxRect new_bounds(wxPoint(0, 0), info->bci_BufferSize.GetSize());
        new_bounds.SetPosition(wxPoint(0, 0));


        if (new_bounds.GetSize() != m_bounds.GetSize())
        {
            // taking the make-before-break approach
            wxBitmap* new_bitmap = new wxBitmap(new_bounds.GetWidth(), new_bounds.GetHeight(), GetPixelDepth());
            if ((__nullptr != new_bitmap) && (new_bitmap->Ok()))
            {
                delete m_bitmap;
                m_bitmap = new_bitmap;
            }
            else
            {
                delete new_bitmap; // In case it's not null but also not OK
                // TODO: throw an appropriate exception!
                return bcr_Failure;
            }
        }
        else if (info->bci_Scale == GetScale())
        {
            // if the sizes are equal and the scales also, return immediately
            return bcr_Cancel;
        }

        SetScale(info->bci_Scale);

        SetReferences(info->bci_ReferencePt.h, info->bci_ReferencePt.v);

        m_bounds = new_bounds;

        SetRebuildLevel(bbl_Complete);

        return bcr_OK;
    }


    /* DetermineBufferWholeSizes
    Calculates two sets of things :
    1. IF the buffer (ie. scoreboard) CAN fit entirely within the
       window (ie. soil frame)
    2. What the buffer size will be what ever the outcome of 1.

    WARNING !! Make sure that m_soilScale is set correctly before using this function !! */
    void ViewBuffer::DetermineBufferWholeSizes
    (const DoubleRect& sbBounds,
        const DoubleRect& sFrame,
        bool& fFitsWidth,
        bool& fFitsHeight,
        int& minWidth,
        int& minHeight,
        double soilScale)
    {
        double sbWidth = sbBounds.right - sbBounds.left;
        double soilWidth = sFrame.right - sFrame.left;
        //TODO: should m_xWhole be *set* here ?
        m_xWhole = fFitsWidth = (sbWidth <= soilWidth);
        // rounding is performed by flooring the result of adding 0.5
        double tmp_width = floor((Utility::CSMin(sbWidth, soilWidth) * soilScale) + 0.5);
        minWidth = static_cast<int>(Utility::CSMin(tmp_width, static_cast<double>(INT_MAX)));

        double sbHeight = static_cast<double>(fabs(sbBounds.bottom - sbBounds.top));
        double fHeight = static_cast<double>(fabs(sFrame.top - sFrame.bottom));
        //TODO: should m_yWhole be *set* here ?
        m_yWhole = fFitsHeight = (sbHeight <= fHeight);
        double tmp_height = floor((Utility::CSMin(sbHeight, fHeight) * soilScale) + 0.5);
        minHeight = static_cast<int>(Utility::CSMin(tmp_height, static_cast<double>(INT_MAX)));

        RmAssert(minWidth > 0, "minWidth determined to be less than 1");
        RmAssert(minHeight > 0, "minHeight determined to be less than 1");
    }

    /* GetXferMode [/SetXferMode]
    Returns [/sets] the mode which CopyBits functions (ie. CopyFrom(),
    CopyTo(), Stamp(), MakeStamp() ) will use to do its bit transfers
    int ViewBuffer::GetXferMode()
    {
        return (m_xferMode);
    }
    */

    void ViewBuffer::SetXferMode(int aXferMode)
    {
        m_xferMode = (wxRasterOperationMode)aXferMode;
        if (__nullptr != m_activeDC)
        {
            m_activeDC->SetLogicalFunction(m_xferMode);
        }
    }

    /*
    SetScale
    Set the soil to pixel ratio of Buffer drawing.

    95.06.22    Check to see that the requested scale is greater than zero befor setting. This
            allows the scale to easily be left as is by calling functions if that is what they
            want, just by passing a non-positive value.
    95.08.16    Also sets m_rebuildLevel and m_unprepared.
    95.08.17    Calls SetRebuildLevel() and ForceNextPrepare() instead of directly
            manipulating the relevant data members.
    95.09.05    now check for newScale > 0.00002, 'cause floats are only accurate to 7 or 8
            decimal places
    97.08.21    remove call to ForceNextPrepare(), now that SetRebuildLevel() does it */
    void ViewBuffer::SetScale(double newScale)
    {
        if ((newScale > 0.00002) && (newScale != m_soilScale))
        {
            m_soilScale = newScale;
            SetRebuildLevel(bbl_Complete);
        }
    }


    /* SetReferences 95.08.03
    Set the soil origin offsets*/
    void ViewBuffer::SetReferences(double xRef, double yRef)
    {
        m_xReference = xRef;
        m_yReference = yRef;

#if defined ROOTMAP_VIEW_FROM_TOP_FIXME
        //TODO: is this necessary if we Set the Axis in the Bitmap ?
        // if (m_viewDirection == vTop) yRef=-yRef;
#endif // #if defined ROOTMAP_VIEW_FROM_TOP_FIXME

        SetRebuildLevel(bbl_Redraw);
    }

    BufferChangeResponse ViewBuffer::SetViewDirection
    (ViewDirection newDirection,
        ViewDirectionBufferChangeInfo* info)
    {
        m_viewDirection = newDirection;

        // values we're interested in : scale and reference point
        SetScale(info->bci_Scale);
        SetReferences(info->bci_ReferencePt.h, info->bci_ReferencePt.v);

        SetRebuildLevel(bbl_Complete);

        return bcr_OK;
    }

    void ViewBuffer::CopyTo(wxDC& toDC, const wxRect& fromRect, const wxRect& toRect)
    {
        LOG_COPYRECT_FUNCTION(ViewBuffer_CopyTo);
        RmAssert(__nullptr != m_activeDC, "No Device Context active");
        LOG_COPYRECT_SIMPLE(fromRect);
        LOG_COPYRECT_SIMPLE(toRect);

        toDC.Blit(toRect.GetTopLeft(), toRect.GetSize(), m_activeDC, fromRect.GetTopLeft(), wxCOPY, true); // GetXferMode());
    }

    void ViewBuffer::CopyFrom(wxDC& fromDC, const wxRect& fromRect, const wxRect& toRect)
    {
        LOG_COPYRECT_FUNCTION(ViewBuffer_CopyFrom);
        RmAssert(__nullptr != m_activeDC, "No Device Context active");
        LOG_COPYRECT_SIMPLE(fromRect);
        LOG_COPYRECT_SIMPLE(toRect);

        m_activeDC->Blit(toRect.GetTopLeft(), toRect.GetSize(), &fromDC, fromRect.GetTopLeft(), GetXferMode());
    }


    bool ViewBuffer::BeginDrawing()
    {
        RmAssert(__nullptr != m_bitmap, "No Bitmap available");

        if (m_begunDrawingRecursionLevel <= 0)
        {
            //wxWidgets: no-op
        }

        ++m_begunDrawingRecursionLevel;

        return true;
    }


    void ViewBuffer::EndDrawing()
    {
        --m_begunDrawingRecursionLevel;
        if (m_begunDrawingRecursionLevel <= 0)
        {
            m_begunDrawingRecursionLevel = 0;
            //SetPort(m_savePort);
        }
    }


    void ViewBuffer::Prepare()
    {
        if (m_unprepared)
        {
            if (BeginDrawing())
            {
                RmAssert(__nullptr != m_activeDC, "No Device Context active");
                m_activeDC->Clear();
                //EraseRect(&(m_bitmap->bitmapRect));
                EndDrawing();
                m_unprepared = false;
            } // if (BeginDrawing())
        } // if (m_unprepared)
    } // Prepare()

    /* ForceNextPrepare 95.08.16
    Forces the next call to Prepare() to actually clear the buffer
    void ViewBuffer::ForceNextPrepare()
    {
        m_unprepared = true;
    }*/

    /*
    Draw 95.08.08
    Try not to override this. It does important setting up. Override this only if you really
    need to change the area parameter for a common, distinguishing purpose. Override
    DrawContents() to do custom drawing.
    95.08.16    Set m_rebuildLevel to just less than bbl_MinimumRedrawLevel, rather than bbl_NONE
    2006.XX.YY Set m_rebuildLevel to just less than bbl_Redraw, so that a reslap will still take place
    */
    void ViewBuffer::Draw(const DoubleRect& area)
    {
        if (m_rebuildLevel >= bbl_MinimumRedrawLevel)
        {
            //wxMemoryDC memDC;
            //SetDeviceContext(&memDC);
            //memDC.SelectObject(GetBitmap());

            if (BeginDrawing())
            {
                RmAssert(__nullptr != m_activeDC, "No Device Context active");
                DrawContents(area);
                EndDrawing();
            }

            //SetDeviceContext(__nullptr);
            //memDC.SelectObject(wxNullBitmap);

            m_rebuildLevel = (BufferBuildLevel)(bbl_Redraw - 1);
        }
    }


    /* DrawContents 95.08.05
    Called by Draw() to draw the contents of the Buffer. This is the
    preferred function to override. This default has nothing to do. */
    //void ViewBuffer::DrawContents(const DoubleRect &  /*area*/)
    //{
    //}

    double ViewBuffer::GetScale() const
    {
        return m_soilScale;
    }


    /*
    ConvertPt
    Adjusts a point from soil space to view window bounds pixel coordinates (not units)
     */
    void ViewBuffer::Buffer_ConvertPt(
        const DoublePt& soilPoint,
        wxPoint& viewPoint)
    {
        viewPoint.x = ViewBuffer::Buffer_Convert_h(soilPoint.h);
        viewPoint.y = ViewBuffer::Buffer_Convert_v(soilPoint.v);
    }


    /*
    ConvertRect
    Adjusts a rectangle from soil space to view window bounds pixel coordinates (not units)
    If we are view from the top, we need to correct for the flipping effect by reversing
    the top and bottom.

    Modifications    :
    95.07.18        Doesn't swap the top and bottom, since calling functions _WILL_ now
                make sure that the top of the soil wxRect is the higher on the screen
                (ie. the greater value). So far, this is taken care of by
                Engine::GetScoreboardBounds()
                Engine::GetTotalScoreboardBounds()
                (called by VPanorama functions)
                VPanorama::Buffer_UnconvertRect()
     */
    void ViewBuffer::Buffer_ConvertRect(
        const DoubleRect& soilRect,
        wxRect& viewRect)
    {
        viewRect.SetLeft(ViewBuffer::Buffer_Convert_h(soilRect.left));
        viewRect.SetRight(ViewBuffer::Buffer_Convert_h(soilRect.right));

        viewRect.SetTop(ViewBuffer::Buffer_Convert_v(soilRect.top));
        viewRect.SetBottom(ViewBuffer::Buffer_Convert_v(soilRect.bottom));
    }


    // ///////////
    // LineTo
    void ViewBuffer::Buffer_LineTo(double xPos, double yPos)
    {
        wxCoord old_posX = m_currentPositionX;
        wxCoord old_posY = m_currentPositionY;
        m_currentPositionX = ViewBuffer::Buffer_Convert_h(xPos);
        m_currentPositionY = ViewBuffer::Buffer_Convert_v(yPos);

        LOG_DEBUG << "Buffer_LineTo(" << xPos << ", " << yPos << ") => m_activeDC->DrawLine("
            << old_posX << ", " << old_posY << ", " << m_currentPositionX << ", " << m_currentPositionY << ")";

        m_activeDC->DrawLine(old_posX, old_posY, m_currentPositionX, m_currentPositionY);
    }

    // ///////////
    // Line
    void ViewBuffer::Buffer_Line(double x1, double y1, double x2, double y2)
    {
        wxCoord posX = ViewBuffer::Buffer_Convert_h(x1);
        wxCoord posY = ViewBuffer::Buffer_Convert_v(y1);
        m_currentPositionX = ViewBuffer::Buffer_Convert_h(x2);
        m_currentPositionY = ViewBuffer::Buffer_Convert_v(y2);

        //LOG_DEBUG
        //<< "Buffer_Line(x1=" << x1 << ", y1=" << y1 << ", x2=" << x2 << ", y2=" << y2 << ") => "
        //<< "m_activeDC->DrawLine(" << posX << ", " << posY << ", " << m_currentPositionX << ", " << m_currentPositionY << ")";

        m_activeDC->DrawLine(posX, posY, m_currentPositionX, m_currentPositionY);
    }

    void ViewBuffer::Buffer_Line(const DoubleCoordinate& dcoord1, const DoubleCoordinate& dcoord2)
    {
        DoublePt fp1;
        DoubleCoordinateToPt(&dcoord1, &fp1, m_viewDirection);

        DoublePt fp2;
        DoubleCoordinateToPt(&dcoord2, &fp2, m_viewDirection);

        Buffer_Line(fp1, fp2);
    }

    void ViewBuffer::Buffer_FrameRect(const DoubleRect& frame_rect_soil)
    {
        LOG_PRIMITIVE_FUNCTION(ViewBuffer_Buffer_FrameRect);
        wxRect frame_rect_pix;
        Buffer_ConvertRect(frame_rect_soil, frame_rect_pix);

        //qrect.Inflate(1,1);
        // m_activeDC->DrawRectangle(qrect.GetLeft(), qrect.GetTop(), qrect.GetWidth(), qrect.GetHeight());
        // m_activeDC->DrawRectangle(qrect.GetTopLeft(), qrect.GetSize());
        /*TODO LOG_DEBUG << "Buffer_FrameRect" << ToString(frame_rect_soil).c_str()
                  << " => m_activeDC->DrawLine" << ToString(frame_rect_pix).c_str();
    */
        m_activeDC->DrawRectangle(frame_rect_pix);
    }

    void ViewBuffer::Buffer_FrameRect(const DoubleRect& fr, const wxColour& colour)
    {
        // colour setup - only do pen. PaintRect also does brush, but FrameRect
        // sets the Brush style to transparent
        wxPen* tmp_pen = SetTemporaryPenColour(colour);
        wxBrush* tmp_brush = SetTemporaryBrushColour(colour, wxBRUSHSTYLE_TRANSPARENT);

        Buffer_FrameRect(fr);

        // reset old pen and brush
        RestorePenColour(tmp_pen);
        RestoreBrushColour(tmp_brush);
    }

    void ViewBuffer::Buffer_PaintRect(const DoubleRect& fr, const wxColour& colour)
    {
        // colour setup - must do both pen and brush
        wxPen* tmp_pen = SetTemporaryPenColour(colour);
        wxBrush* tmp_brush = SetTemporaryBrushColour(colour, wxBRUSHSTYLE_SOLID);

        // paint it
        Buffer_PaintRect(fr);

        // reset old pen and brush
        RestorePenColour(tmp_pen);
        RestoreBrushColour(tmp_brush);
    }

    void ViewBuffer::Buffer_PaintOval(const DoubleRect& fr, const wxColour& colour)
    {
        // colour setup - must do both pen and brush
        wxPen* tmp_pen = SetTemporaryPenColour(colour);
        wxBrush* tmp_brush = SetTemporaryBrushColour(colour);

        // paint it
        Buffer_PaintOval(fr);

        // reset old pen and brush
        RestorePenColour(tmp_pen);
        RestoreBrushColour(tmp_brush);
    }

    /*
    Buffer_Dot
    Does the work that things like Nitrogen ions might ask for.
     */
    void ViewBuffer::Buffer_Dot(double xPos, double yPos)
    {
        m_currentPositionX = Buffer_Convert_h(xPos);
        m_currentPositionY = Buffer_Convert_v(yPos);
        //LOG_DEBUG << "DrawPoint(" << xPos << ", " << yPos << ") => (" << m_currentPositionX << ", " << m_currentPositionY << ")";


        // MSA 11.04.15 Superseded: the simple "overwrite with white" method
        // gives a much better-looking result.
        // Let the colour pointer remain NULL. It won't be used.
        const wxColourWrapper* colourPtr = __nullptr;
        /*

        // This wxColourWrapper is allocated on the stack, as we don't need it to outlive this method call.
        wxColourWrapper c = wxColourWrapper();
        m_activeDC->GetPixel(m_currentPositionX,m_currentPositionY,&c);

        ColourSet::iterator iter = m_globalColoursInUse.find(c);
        if(iter==m_globalColoursInUse.end())
        {
            colourPtr = new wxColourWrapper(c); // Copy construct it on the heap
            m_globalColoursInUse.insert(*colourPtr);
        }
        else
        {
            colourPtr = &(*iter);
        }
        */
        m_previousDotColourMap[XY(m_currentPositionX, m_currentPositionY)] = colourPtr;

        m_activeDC->DrawPoint(m_currentPositionX, m_currentPositionY);
    }

    void ViewBuffer::Buffer_Dot(double xPos, double yPos, const wxColour& colour)
    {
        // colour setup - must do both pen and brush
        wxPen* tmp_pen = SetTemporaryPenColour(colour);

        m_currentPositionX = Buffer_Convert_h(xPos);
        m_currentPositionY = Buffer_Convert_v(yPos);

        // MSA 11.04.15 Superseded: the simple "overwrite with white" method
        // gives a much better-looking result.
        // Let this colour pointer remain NULL. It won't be used.
        const wxColourWrapper* colourPtr = __nullptr;

        /*
        // This wxColourWrapper is allocated on the stack, as we don't need it to outlive this method call.
        wxColourWrapper c = wxColourWrapper();
        m_activeDC->GetPixel(m_currentPositionX,m_currentPositionY,&c);

        ColourSet::iterator iter = m_globalColoursInUse.find(c);
        if(iter==m_globalColoursInUse.end())
        {
            colourPtr = new wxColourWrapper(c); // Copy construct it on the heap
            m_globalColoursInUse.insert(*colourPtr);
        }
        else
        {
            colourPtr = &(*iter);
        }
        */
        m_previousDotColourMap[XY(m_currentPositionX, m_currentPositionY)] = colourPtr;

        m_activeDC->DrawPoint(m_currentPositionX, m_currentPositionY);

        RestorePenColour(tmp_pen);
    }

    void ViewBuffer::Unbuffer_Dot(double xPos, double yPos)
    {
        // MSA 11.04.15 Superseded: the simple "overwrite with white" method
        // gives a much better-looking result.

        /*
        wxCoord px = m_currentPositionX;
        wxCoord py = m_currentPositionY;
        m_currentPositionX = Buffer_Convert_h(xPos);
        m_currentPositionY = Buffer_Convert_v(yPos);
        XY xy(m_currentPositionX,m_currentPositionY);
        std::map<XY,const wxColour*>::iterator iter = m_previousDotColourMap.find(xy);
        if(iter==m_previousDotColourMap.end())
        {
            // MSA is this necessary?
            m_currentPositionX = px;
            m_currentPositionY = py;
            return;    // This dot was not previously buffered.
        }

        const wxColour * prevColour = iter->second;

        // colour setup - must do both pen and brush
        wxPen   * tmp_pen   = SetTemporaryPenColour(*prevColour);



        m_activeDC->DrawPoint(m_currentPositionX , m_currentPositionY);

        delete iter->second;
        m_previousDotColourMap.erase(iter);

        RestorePenColour(tmp_pen);
        */

        // colour setup - must do both pen and brush
        wxPen* tmp_pen = SetTemporaryPenColour(*wxWHITE);

        m_currentPositionX = Buffer_Convert_h(xPos);
        m_currentPositionY = Buffer_Convert_v(yPos);

        m_activeDC->DrawPoint(m_currentPositionX, m_currentPositionY);

        RestorePenColour(tmp_pen);
    }

    // MSA 11.04.15 Unfortunately, the "restore previous colour" approach doesn't work well - it tends to result in 
    // the majority of the 2D view being painted over with the Dot colour.
    void ViewBuffer::UnbufferAllDots()
    {
        // MSA 11.04.15 Superseded: the simple "overwrite with white" method
        // gives a much better-looking result.

        for (std::map<XY, const wxColour*>::iterator iter = m_previousDotColourMap.begin(); iter != m_previousDotColourMap.end(); ++iter)
        {
            /*
            const wxColour * prevColour = iter->second;

            // colour setup - must do both pen and brush
            wxPen   * tmp_pen = SetTemporaryPenColour(*prevColour);
            */
            wxPen* tmp_pen = SetTemporaryPenColour(*wxWHITE);

            m_currentPositionX = iter->first.first;
            m_currentPositionY = iter->first.second;
            m_activeDC->DrawPoint(m_currentPositionX, m_currentPositionY);

            RestorePenColour(tmp_pen);
        }
        m_previousDotColourMap.clear();
    }

    /*
    Scroll
    Positive values scroll the buffer contents up and left. In side and front
    viewing, anyway.
     */
    void ViewBuffer::Scroll(double xDelta, double yDelta)
    {
        if (xDelta != 0.0)
        {
            m_xReference += xDelta;
        }

        if (yDelta != 0.0)
        {
            m_yReference += yDelta;
        }

        if ((xDelta > 0.0) || (yDelta != 0.0))
        {
            SetRebuildLevel(bbl_Redraw);
        }
    }

    /*
    Scroll
    Positive values scroll the buffer contents up and left. In side and front viewing, anyway.
    95.08.16    Sets m_rebuildLevel and m_unprepared.
    95.08.17    Calls SetRebuildLevel() and ForceNextPrepare() instead.
    97.08.21    remove call to ForceNextPrepare(), now that SetRebuildLevel() does it
     */
    void ViewBuffer::ScrollTo(double xPos, double yPos)
    {
        //LOG_SIMPLE(DoublePt(m_xReference,m_yReference));
        /* Scroll Horizontally */
        if (m_xReference != xPos)
        {
            m_xReference = xPos;
            SetRebuildLevel(bbl_Redraw);
        }

        /* Scroll Vertically, in the opposite direction if from the top */
        if (m_yReference != yPos)
        {
            m_yReference = yPos;
            SetRebuildLevel(bbl_Redraw);
        }
        //LOG_SIMPLE(DoublePt(m_xReference,m_yReference));
    }

    void ViewBuffer::SetDeviceContext(wxDC* context)
    {
        RmAssert(((__nullptr == m_activeDC) || (__nullptr == context)), "Either current or proposed DeviceContext must be null");

        m_activeDC = context;

        if (__nullptr != m_activeDC)
        {
            m_activeDC->SetPen(*m_dcPen);
            m_activeDC->SetBrush(*m_dcBrush);
            // ensures Clear() whites out the b/g
            m_activeDC->SetBackground(*wxWHITE_BRUSH);

            m_activeDC->SetLogicalFunction(m_xferMode);

#if defined ROOTMAP_VIEW_FROM_TOP_FIXME
            // this is the magic where the View From Top flips the y-axis
            // NOTE: the way the doco is worded is tricky - you set it to true
            // if you *DO* want the y-axis flipped y=bottom->up
            m_activeDC->SetAxisOrientation(true, (vTop == m_viewDirection));
#endif // #if defined ROOTMAP_VIEW_FROM_TOP_FIXME
        }
    }

    void ViewBuffer::SetPen(const wxPen& pen)
    {
        try {
            delete m_dcPen;
            m_dcPen = new wxPen(pen);
            if (__nullptr != m_activeDC)
            {
                m_activeDC->SetPen(*m_dcPen);
            }
        }
        catch (const std::exception&)
        {
        }
    }

    void ViewBuffer::SetPenColour(const wxColour& colour)
    {
        m_dcPen->SetColour(colour);

        if (__nullptr != m_activeDC)
        {
            m_activeDC->SetPen(*m_dcPen);
        }
    }

    void ViewBuffer::SetPenDashes(int n, wxDash* dashes)
    {
        m_dcPen->SetDashes(n, dashes);

        if (__nullptr != m_activeDC)
        {
            m_activeDC->SetPen(*m_dcPen);
        }
    }

    void ViewBuffer::SetPenStyle(wxPenStyle style)
    {
        m_dcPen->SetStyle(style);

        if (__nullptr != m_activeDC)
        {
            m_activeDC->SetPen(*m_dcPen);
        }
    }

    void ViewBuffer::SetPenWidth(int width)
    {
        m_dcPen->SetWidth(width);

        if (__nullptr != m_activeDC)
        {
            m_activeDC->SetPen(*m_dcPen);
        }
    }

    void ViewBuffer::SetBrush(const wxBrush& brush)
    {
        delete m_dcBrush;
        m_dcBrush = new wxBrush(brush);

        if (__nullptr != m_activeDC)
        {
            m_activeDC->SetBrush(*m_dcBrush);
        }
    }

    void ViewBuffer::SetBrushColour(const wxColour& colour)
    {
        m_dcBrush->SetColour(colour);

        if (__nullptr != m_activeDC)
        {
            m_activeDC->SetBrush(*m_dcBrush);
        }
    }

    void ViewBuffer::SetBrushStyle(wxBrushStyle style)
    {
        m_dcBrush->SetStyle(style);

        if (__nullptr != m_activeDC)
        {
            m_activeDC->SetBrush(*m_dcBrush);
        }
    }

    wxPen* ViewBuffer::SetTemporaryPenColour(const wxColour& colour)
    {
        wxPen* tmp_pen = new wxPen(*m_dcPen);
        tmp_pen->SetColour(colour);
        m_activeDC->SetPen(*tmp_pen);
        return (tmp_pen);
    }

    wxBrush* ViewBuffer::SetTemporaryBrushColour(const wxColour& colour, wxBrushStyle style)
    {
        wxBrush* tmp_brush = new wxBrush(*m_dcBrush);
        tmp_brush->SetColour(colour);
        tmp_brush->SetStyle(style);
        m_activeDC->SetBrush(*tmp_brush);
        return (tmp_brush);
    }

    ViewBufferPrep::ViewBufferPrep(ViewBuffer* buffer)
        : m_buffer(buffer)
    {
        m_memoryDC.SelectObject(buffer->GetBitmap());
        buffer->SetDeviceContext(&m_memoryDC);
    }

    ViewBufferPrep::~ViewBufferPrep()
    {
        m_buffer->SetDeviceContext(__nullptr);
        m_memoryDC.SelectObject(wxNullBitmap);
    }

    void ViewBuffer::Buffer_FrameBox(const DoubleBox& /* fb */)
    {
        throw ProcessDrawingException("ViewBuffer::Buffer_FrameBox(const DoubleBox &) not supported at this level");
    }

    void ViewBuffer::Buffer_FrameSpheroid(const DoubleBox& /* fb */)
    {
        throw ProcessDrawingException("ViewBuffer::Buffer_FrameSpheroid(const DoubleBox &) not supported at this level");
    }

    void ViewBuffer::Buffer_PaintBox(const DoubleBox& /* fb */)
    {
        throw ProcessDrawingException("ViewBuffer::Buffer_PaintBox(const DoubleBox &) not supported at this level");
    }

    void ViewBuffer::Buffer_PaintSpheroid(const DoubleBox& /* fb */)
    {
        throw ProcessDrawingException("ViewBuffer::Buffer_PaintSpheroid(const DoubleBox &) not supported at this level");
    }
} /* namespace rootmap */
