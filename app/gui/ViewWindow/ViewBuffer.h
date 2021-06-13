#ifndef ViewBuffer_H
#define ViewBuffer_H
/////////////////////////////////////////////////////////////////////////////
// Name:        ClassTemplate.h
// Purpose:     Declaration of the ClassTemplate class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2009-07-10 06:43:25 +0800 (Fri, 10 Jul 2009) $
// $Revision: 69 $
// Copyright:   ©2006 University of Tasmania
// 
// An adapter for the wxBitmap/wxDC classes
// 
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/ViewWindow/ViewBufferStructures.h"

#include "core/common/DoubleCoordinates.h"
#include "core/common/RmAssert.h"
#include "core/common/Structures.h"
#include "core/common/Types.h"
#include "core/log/Logger.h"
#include "core/utility/Utility.h"

#include "wx/bitmap.h"
#include "wx/dc.h"
#include "wx/dcmemory.h"

namespace rootmap
{
    // Forward class declarations
    class BoundaryArray;

    class wxColourWrapper : public wxColour
    {
    public:
        // MSA 11.04.01 Have to implement comparison operators so this struct can be used to key a map.
        // They are internally consistent, but rather arbitrary.
        inline bool operator<(const wxColourWrapper& other) const
        {
            if (Red() == other.Red())
            {
                if (Green() == other.Green())
                {
                    if (Blue() == other.Blue())
                    {
                        return Alpha() < other.Alpha();
                    }
                    return Blue() < other.Blue();
                }
                return Green() < other.Green();
            }
            return Red() < other.Red();
        }

        inline bool operator>(const wxColourWrapper& other) const
        {
            if (Red() == other.Red())
            {
                if (Green() == other.Green())
                {
                    if (Blue() == other.Blue())
                    {
                        return Alpha() > other.Alpha();
                    }
                    return Blue() > other.Blue();
                }
                return Green() > other.Green();
            }
            return Red() > other.Red();
        }
    };

    typedef std::set<wxColourWrapper> ColourSet;

    /**
     *
     */
    class ViewBuffer
    {
    public:
        // Construction/Destruction
        ViewBuffer();
        ViewBuffer(int width,
            int height,
            int depth,
            double scale,
            ViewDirection direction);
        virtual BufferChangeResponse Create
        (int width,
            int height,
            int depth,
            double scale,
            ViewDirection direction);
        virtual ~ViewBuffer();

        // Accessing functions
        virtual const wxRect& GetBounds();
        virtual void SetBoundsOrigin(long hOrigin, long vOrigin);
        virtual void SetBounds(const wxRect& newBounds);

        /**
         * Resize
         * The size of the required display rectangle has obviously changed, plus possibly the scale.
         * The scale is set in SetScale(), but we need to resize or reallocate the pixmap's pixels block.
         */
        virtual BufferChangeResponse Resize(ResizeBufferChangeInfo* info);
        void DetermineBufferWholeSizes(const DoubleRect& sbBounds,
            const DoubleRect& sFrame,
            bool& fFitsWidth,
            bool& fFitsHeight,
            int& minWidth,
            int& minHeight,
            double soilScale);

        virtual wxRasterOperationMode GetXferMode() { return m_xferMode; }
        virtual void SetXferMode(int aXferMode);

        virtual int GetPixelDepth() const;

        bool IsHWhole() const;
        bool IsVWhole() const;

        // Image Copying functions
        virtual void CopyTo(wxDC& dc, const wxRect& fromRect, const wxRect& toRect);
        virtual void CopyFrom(wxDC& dc, const wxRect& fromRect, const wxRect& toRect);
        //     virtual void MakeStamp(GrafPtr aPort);
        //     virtual void Stamp(GrafPtr aPort);

        // Preparing the environment
        /** Set this instance's bitmap as the active drawing bitmap.
         * Lock pixel memory for drawing. wx port has made this redundant */
        virtual bool BeginDrawing();
        /** Reinstate the previous drawing bitmap. Unlock pixel memory for memory
         * efficiency/availability. wx has made this redundant */
        virtual void EndDrawing();

        virtual void Prepare(); //EX : Clear
        void ForceNextPrepare();
        BufferBuildLevel GetRebuildLevel() const;
        void SetRebuildLevel(BufferBuildLevel bbl);

        virtual void Draw(const DoubleRect& area); // = CropPlants Buffer.Update();
        virtual void DrawContents(const DoubleRect& area) = 0;

        /********
         ** All buffer conversion and drawing functions are prefixed with Buffer_ to distinguish them
         ** from their WrapBuffer_ and ProcessBuffer_ counterparts , which are functionally
         ** rather different
         ********/
        virtual double GetScale() const;
        virtual ViewDirection GetViewDirection() const;
        virtual DoublePt GetReferencePt() const;
        virtual void SetScale(double newScale);
        virtual void SetReferences(double xRef, double yRef);
        virtual BufferChangeResponse SetViewDirection(ViewDirection newDirection, ViewDirectionBufferChangeInfo* info);

        virtual void Scroll(double xDelta, double yDelta);
        virtual void ScrollTo(double h, double v);

        /**
         * Either current or proposed context must be null.
         * If this method is overridden, call this method *second*
         */
        virtual void SetDeviceContext(wxDC* context);
        virtual void SetPen(const wxPen& pen);
        /** see doco for wxPen::SetColour */
        virtual void SetPenColour(const wxColour& colour);
        /** see doco for wxPen::SetDashes
         * Not yet implemented */
        virtual void SetPenDashes(int n, wxDash* dashes);
        /** see doco for wxPen::SetStyle
         * Not yet implemented, defaults to wxSOLID */
        virtual void SetPenStyle(wxPenStyle style);
        /** see doco for wxPen::SetWidth */
        virtual void SetPenWidth(int width);
        virtual void SetBrush(const wxBrush& brush);
        /** see doco for wxBrush::SetColour */
        virtual void SetBrushColour(const wxColour& colour);
        /** see doco for wxBrush::SetStyle */
        virtual void SetBrushStyle(wxBrushStyle style);

        /** Now publicly available by popular demand */
        wxBitmap& GetBitmap();

    protected:
        // some simple accessors that we'd rather give only derived classes access to
        double GetHReference() const;
        double GetVReference() const;
        int GetDrawingRecursionLevel() const;
        wxDC& GetDC();


        int Buffer_Convert_h(double soilValue);
        int Buffer_Convert_v(double soilValue);
        void Buffer_ConvertPt(const DoublePt& soilPoint, wxPoint& viewPoint);
        void Buffer_ConvertRect(const DoubleRect& soilRect, wxRect& viewRect);

        void Buffer_MoveTo(double h, double v);
        void Buffer_MoveTo(const DoublePt& fp);
        void Buffer_MoveTo(const DoubleCoordinate& dc);

        // LineTo with prevailing colour
        void Buffer_LineTo(double h, double v);
        void Buffer_LineTo(const DoublePt& fp);
        void Buffer_LineTo(const DoubleCoordinate& dc);
        // LineTo with specified colour
        void Buffer_LineTo(double h, double v, const wxColour& colour);
        void Buffer_LineTo(const DoublePt& fp, const wxColour& colour);
        void Buffer_LineTo(const DoubleCoordinate& dc, const wxColour& colour);

        // Line with prevailing colour
        void Buffer_Line(double x1, double y1, double x2, double y2);
        void Buffer_Line(const DoublePt& fp1, const DoublePt& fp2);
        void Buffer_Line(const DoubleCoordinate& dcoord1, const DoubleCoordinate& dcoord2);
        // Line with specified colour
        void Buffer_Line(double x1, double y1, double x2, double y2, const wxColour& colour);
        void Buffer_Line(const DoublePt& fp1, const DoublePt& fp2, const wxColour& colour);
        void Buffer_Line(const DoubleCoordinate& dcoord1, const DoubleCoordinate& dcoord2, const wxColour& colour);


        void Buffer_FrameRect(const DoubleRect& fr);
        void Buffer_FrameRect(const DoubleRect& fr, const wxColour& colour);

        void Buffer_FrameOval(const DoubleRect& fr);
        void Buffer_FrameOval(const DoubleRect& fr, const wxColour& colour);

        void Buffer_PaintRect(const DoubleRect& fr);
        void Buffer_PaintRect(const DoubleRect& fr, const wxColour& colour);

        void Buffer_PaintOval(const DoubleRect& fr);
        void Buffer_PaintOval(const DoubleRect& fr, const wxColour& colour);

        // Dots with prevailing colour
        void Buffer_Dot(double h, double v);
        void Buffer_Dot(const DoublePt& /* fp */);
        void Buffer_Dot(const DoubleCoordinate& fc);
        // Dots with specified colour
        void Buffer_Dot(double h, double v, const wxColour& colour);
        void Buffer_Dot(const DoublePt& fp, const wxColour& colour);
        void Buffer_Dot(const DoubleCoordinate& fc, const wxColour& colour);

        void Unbuffer_Dot(const DoubleCoordinate& fc);
        void Unbuffer_Dot(double xPos, double yPos);
        void UnbufferAllDots();

        /* 3D stuff should be handled at a more abstract level, for instance in the
         * ProcessDrawingBuffer class. This implementation throws a ProcessDrawingException
         */
        void Buffer_FrameBox(const DoubleBox& /* fb */);
        void Buffer_FrameSpheroid(const DoubleBox& /* fb */);
        void Buffer_PaintBox(const DoubleBox& /* fb */);
        void Buffer_PaintSpheroid(const DoubleBox& /* fb */);

        wxPen* SetTemporaryPenColour(const wxColour& colour);
        wxBrush* SetTemporaryBrushColour(const wxColour& colour, wxBrushStyle style = wxBRUSHSTYLE_SOLID);
        void RestorePenColour(wxPen* tmpPen);
        void RestoreBrushColour(wxBrush* tmpBrush);

    private:
        RootMapLoggerDeclaration();

        ///
        ///
        wxRect m_bounds;

        // to temporarily support the MoveTo/LineTo/Line(deltaH,deltaV)
        // linedrawing of QuickDraw
        wxCoord m_currentPositionX;
        wxCoord m_currentPositionY;

        /**
         * Map of all colours that are used
         * MSA TODO Convert to a std::set    DONE
         */
        static ColourSet m_globalColoursInUse;

        typedef std::pair<wxCoord, wxCoord> XY;
        std::map<XY, const wxColour*> m_previousDotColourMap;


        bool m_xWhole;
        bool m_yWhole;
        bool m_unprepared;

        /*****
        **    Below we have the left and top of the buffer w.r.t. the soil's zero index (the topLeft of the soil scoreboard)
        **    This is different to the VCartographer's vpReferenceIndex, which is the position of the soil's zero index
        **    w.r.t. the topLeft of the panorama's frame.
        **
        **    Thus, the VPanorama's vpBuffer's xReference and yReference will be
        **    xReference = -(projection of vpReferenceIndex to horizontal dimension);
        **    yReference = -(projection of vpReferenceIndex to vertical dimension);
        **
        **    And the VPanoramaBuffer's vpbBoxBoundaryBuffer's xReference and yReference will be
        **    xReference = 0;
        **    yReference = (top of scoreboard projected in vertical dimension);
        *****/
        double m_xReference; // the left of the buffer w.r.t. the soil zero point
        double m_yReference; // the right of the buffer w.r.t. the soil zero point

        double m_soilScale; // pixels per cm of soil

        ///
        ///
        int m_begunDrawingRecursionLevel;

        ///
        ///
        wxRasterOperationMode m_xferMode;


        BufferBuildLevel m_rebuildLevel;

        ViewDirection m_viewDirection;

        ///
        ///
        wxBitmap* m_bitmap;

        wxDC* m_activeDC;
        wxPen* m_dcPen;
        wxBrush* m_dcBrush;


        friend class ViewBufferPrep;
        friend class ScrolledWindowBuffer;
    };

    class ViewBufferPrep
    {
    public:
        ViewBufferPrep(ViewBuffer* buffer);
        ~ViewBufferPrep();

    private:
        wxMemoryDC m_memoryDC;
        ViewBuffer* m_buffer;
    };

    inline ViewDirection ViewBuffer::GetViewDirection() const { return m_viewDirection; }
    inline DoublePt ViewBuffer::GetReferencePt() const { return DoublePt(m_xReference, m_yReference); }
    inline void ViewBuffer::ForceNextPrepare() { m_unprepared = true; }
    inline double ViewBuffer::GetHReference() const { return m_xReference; }
    inline double ViewBuffer::GetVReference() const { return m_yReference; }
    inline BufferBuildLevel ViewBuffer::GetRebuildLevel() const { return m_rebuildLevel; }
    inline bool ViewBuffer::IsHWhole() const { return m_xWhole; }
    inline bool ViewBuffer::IsVWhole() const { return m_yWhole; }
    inline int ViewBuffer::GetDrawingRecursionLevel() const { return m_begunDrawingRecursionLevel; }

    inline wxDC& ViewBuffer::GetDC()
    {
        RmAssert(__nullptr != m_activeDC, "No Device Context active");
        return (*m_activeDC);
    }

    inline wxBitmap& ViewBuffer::GetBitmap()
    {
        RmAssert(__nullptr != m_bitmap, "No bitmap");
        return (*m_bitmap);
    }

    // MSA 11.04.20 Newly inlined functions.

    /* SetRebuildLevel 95.08.16
    Does just that. Sets the data member, to the maximum requested so far
    97.08.21    included code from ForceNextPrepare, but have not called FNP()
                    replaced occurances of SRL() + FNP() with just SRL() */
    inline void ViewBuffer::SetRebuildLevel(BufferBuildLevel bbl)
    {
        m_rebuildLevel = Utility::CSMax(m_rebuildLevel, bbl);

        m_unprepared = true;
    }

    /*
    Convert_h
    Adjusts a value from soil space to horizontal Panorama pixel coordinates (not units).

    */
    inline int ViewBuffer::Buffer_Convert_h(double soilValue)
    {
        //TODO: replace these with c++ limits stuff
        //return    (int)CSMin(
        //                    CSMax(
        //                        (double)INT_MIN,
        //                        ((soilValue-m_xReference)*m_soilScale)),
        //                    (double)INT_MAX );
        return ((soilValue - m_xReference) * m_soilScale);
    }


    /*
    Convert_v
    Adjusts a value from soil space to vertical Panorama pixel coordinates (not units).


    Modifications
    ->95.07.20    making adjustments to basic formula and the way that m_yReference
                is treated, to be generally correct for all view directions.
    */
    inline int ViewBuffer::Buffer_Convert_v(double soilValue)
    {
        return ((soilValue - m_yReference) * m_soilScale);
        //return value;
        //return (int)CSMin(CSMax((double)INT_MIN, value), (double)INT_MAX);
    }


    // ///////////
    // MoveTo
    inline void ViewBuffer::Buffer_MoveTo(double xPos, double yPos)
    {
        //    LOG_PRIMITIVE_FUNCTION(ViewBuffer_Buffer_MoveTo);
        m_currentPositionX = ViewBuffer::Buffer_Convert_h(xPos);
        m_currentPositionY = ViewBuffer::Buffer_Convert_v(yPos);
        LOG_DEBUG << "Buffer_MoveTo(" << xPos << ", " << yPos << ") => ("
            << m_currentPositionX << ", " << m_currentPositionY << ")";
    }

    inline void ViewBuffer::Buffer_MoveTo(const DoublePt& fp)
    {
        Buffer_MoveTo(fp.h, fp.v);
    }

    inline void ViewBuffer::Buffer_MoveTo(const DoubleCoordinate& dc)
    {
        DoublePt fp;
        DoubleCoordinateToPt(&dc, &fp, m_viewDirection);
        ViewBuffer::Buffer_MoveTo(fp);
    }

    inline void ViewBuffer::Buffer_LineTo(const DoublePt& fp)
    {
        //wxPoint qdp;
        //ViewBuffer::Buffer_ConvertPt(fp, qdp);
        //Buffer_LineTo(qdp.h, qdp.v);
        Buffer_LineTo(fp.h, fp.v);
    }

    inline void ViewBuffer::Buffer_LineTo(const DoubleCoordinate& dc)
    {
        DoublePt fp;
        DoubleCoordinateToPt(&dc, &fp, m_viewDirection);
        Buffer_LineTo(fp);
    }

    // ///////////
    // LineTo colour
    inline void ViewBuffer::Buffer_LineTo(double h, double v, const wxColour& colour)
    {
        // colour setup - must do both pen and brush
        wxPen* tmp_pen = SetTemporaryPenColour(colour);

        Buffer_LineTo(h, v);

        // reset old pen and brush
        RestorePenColour(tmp_pen);
    }

    inline void ViewBuffer::Buffer_LineTo(const DoublePt& fp, const wxColour& colour)
    {
        // colour setup - must do both pen and brush
        wxPen* tmp_pen = SetTemporaryPenColour(colour);

        Buffer_LineTo(fp);

        // reset old pen and brush
        RestorePenColour(tmp_pen);
    }

    inline void ViewBuffer::Buffer_LineTo(const DoubleCoordinate& dc, const wxColour& colour)
    {
        // colour setup - must do both pen and brush
        wxPen* tmp_pen = SetTemporaryPenColour(colour);

        Buffer_LineTo(dc);

        // reset old pen and brush
        RestorePenColour(tmp_pen);
    }

    inline void ViewBuffer::Buffer_Line(const DoublePt& fp1, const DoublePt& fp2)
    {
        //wxPoint qdp1;
        //ViewBuffer::Buffer_ConvertPt(fp1, qdp1);
        //wxPoint qdp2;
        //ViewBuffer::Buffer_ConvertPt(fp2, qdp2);

        //Buffer_Line(qdp1.h, qdp1.v,qdp2.h, qdp2.v);
        Buffer_Line(fp1.h, fp1.v, fp2.h, fp2.v);
    }


    // ///////////
    // LineTo colour
    inline void ViewBuffer::Buffer_Line(double x1, double y1, double x2, double y2, const wxColour& colour)
    {
        // colour setup - must do both pen and brush
        wxPen* tmp_pen = SetTemporaryPenColour(colour);

        Buffer_Line(x1, y1, x2, y2);

        // reset old pen and brush
        RestorePenColour(tmp_pen);
    }

    inline void ViewBuffer::Buffer_Line(const DoublePt& fp1, const DoublePt& fp2, const wxColour& colour)
    {
        // colour setup - must do both pen and brush
        wxPen* tmp_pen = SetTemporaryPenColour(colour);

        Buffer_Line(fp1, fp2);

        // reset old pen and brush
        RestorePenColour(tmp_pen);
    }

    inline void ViewBuffer::Buffer_Line(const DoubleCoordinate& dcoord1, const DoubleCoordinate& dcoord2, const wxColour& colour)
    {
        // colour setup - must do both pen and brush
        wxPen* tmp_pen = SetTemporaryPenColour(colour);

        Buffer_Line(dcoord1, dcoord2);

        // reset old pen and brush
        RestorePenColour(tmp_pen);
    }


    inline void ViewBuffer::Buffer_FrameOval(const DoubleRect& fr)
    {
        wxRect qrect;
        Buffer_ConvertRect(fr, qrect);
        //qrect.Inflate(1,1);
        m_activeDC->DrawEllipse(qrect);
    }

    inline void ViewBuffer::Buffer_FrameOval(const DoubleRect& fr, const wxColour& colour)
    {
        // colour setup - only do pen. PaintXXXX also does brush, but FrameOval
        // assumes the Brush is already set to transparent
        wxPen* tmp_pen = SetTemporaryPenColour(colour);

        Buffer_FrameOval(fr);

        // reset old pen and brush
        RestorePenColour(tmp_pen);
    }

    inline void ViewBuffer::Buffer_PaintRect(const DoubleRect& fr)
    {
        /*
        actually just like Buffer_FrameRect, but assumes that the brush has been
        setup prior to calling
         */
        Buffer_FrameRect(fr);
    }

    /*
    Buffer_PaintOval
     */
    inline void ViewBuffer::Buffer_PaintOval(const DoubleRect& fr)
    {
        /*
        actually just like Buffer_FrameOval, but assumes that the brush has been
        setup prior to calling
         */
        Buffer_FrameOval(fr);
    }

    inline void ViewBuffer::Buffer_Dot(const DoublePt& fp)
    {
        Buffer_Dot(fp.h, fp.v);
    }

    inline void ViewBuffer::Buffer_Dot(const DoubleCoordinate& fc)
    {
        DoublePt pt;
        DoubleCoordinateToPt(&fc, &pt, m_viewDirection);
        Buffer_Dot(pt);
    }

    inline void ViewBuffer::Buffer_Dot(const DoublePt& fp, const wxColour& colour)
    {
        Buffer_Dot(fp.h, fp.v, colour);
    }

    inline void ViewBuffer::Buffer_Dot(const DoubleCoordinate& fc, const wxColour& colour)
    {
        DoublePt pt;
        DoubleCoordinateToPt(&fc, &pt, m_viewDirection);
        Buffer_Dot(pt, colour);
    }

    inline void ViewBuffer::Unbuffer_Dot(const DoubleCoordinate& fc)
    {
        DoublePt pt;
        DoubleCoordinateToPt(&fc, &pt, m_viewDirection);
        Unbuffer_Dot(pt.h, pt.v);
    }

    inline void ViewBuffer::RestorePenColour(wxPen* tmpPen)
    {
        m_activeDC->SetPen(*m_dcPen);
        delete tmpPen;
    }

    inline void ViewBuffer::RestoreBrushColour(wxBrush* tmpBrush)
    {
        m_activeDC->SetBrush(*m_dcBrush);
        delete tmpBrush;
    }
} /* namespace rootmap */
#endif // #ifndef ViewBuffer_H
