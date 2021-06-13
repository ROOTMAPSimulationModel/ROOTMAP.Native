#include "app/gui/ViewWindow/WrapRepeatBuffer.h"

#include "simulation/common/SprintUtility.h"
#include "core/utility/Utility.h"
#include "core/common/Exceptions.h"

// warning C4702: unreachable code - the occurances in this file are known
#pragma warning (disable:4702)

namespace rootmap
{
    using SprintUtility::StringOfViewDir;
    using Utility::CSAbs;

    RootMapLoggerDefinition(WrapRepeatBuffer);

    WrapRepeatBuffer::WrapRepeatBuffer
    (ScoreboardCoordinator& scoord,
        bool fWrap,
        bool fRepeat,
        const ScoreboardFlags& scoreboardFlags)
        : ScoreboardBuffer(scoord, scoreboardFlags)
        , m_wrap(fWrap)
        , m_repeat(fRepeat)
        , m_localViewDirection(vNONE)
        , m_currentH0(0.0)
        , m_currentV0(0.0)
        , m_currentH1(0.0)
        , m_currentV1(0.0)
        , m_currentHWrap(0)
        , m_currentVWrap(0)
        , m_scoreboardWidth(0.0)
        , m_scoreboardHeight(0.0)
    {
        RootMapLoggerInitialisation("rootmap.WrapRepeatBuffer");
    }

    BufferChangeResponse WrapRepeatBuffer::Create
    (int width,
        int height,
        int depth,
        double scale,
        ViewDirection direction,
        const DoubleRect& scoreboardBounds,
        const DoubleRect& soilFrame)
    {
        BufferChangeResponse response = bcr_NONE;

        if (m_wrap)
        {
            /*TODO LOG_DEBUG << LOG_LINE << "Creating ScoreboardBuffer {width:" << width
                << ", height:" << height
                << ", depth:" << depth
                << ", scale:" << scale
                << ", ViewDirection:" << StringOfViewDir(direction)
                << ", ScoreboardBounds:" << ToString(scoreboardBounds,true)
                << ", SoilFrame:" << ToString(soilFrame,true) << " }";*/
            response = (ScoreboardBuffer::Create(width, height, depth, scale, direction, scoreboardBounds, soilFrame));
        }
        else
        {
            ScoreboardBuffer::SetWholeness(false, false);
            LOG_DEBUG << LOG_LINE << "Creating ViewBuffer {width:" << width
                << ", height:" << height
                << ", depth:" << depth
                << ", scale:" << scale
                << ", ViewDirection:" << StringOfViewDir(direction) << " }";
            response = (ViewBuffer::Create(width, height, depth, scale, direction));
        }

        m_localViewDirection = direction;
        m_scoreboardBounds = scoreboardBounds;
        m_scoreboardWidth = m_scoreboardBounds.GetWidth();
        m_scoreboardHeight = m_scoreboardBounds.GetHeight();

        return (response);
    }

    WrapRepeatBuffer::~WrapRepeatBuffer()
    {
    }

    BufferChangeResponse WrapRepeatBuffer::SetViewDirection
    (ViewDirection newDirection,
        ViewDirectionBufferChangeInfo* info)
    {
        BufferChangeResponse response = bcr_NONE;

        if (m_wrap)
        {
            LOG_DEBUG << LOG_LINE << "Setting ScoreboardBuffer ViewDirection {ViewDirection:" << StringOfViewDir(newDirection) << " }";
            response = ScoreboardBuffer::SetViewDirection(newDirection, info);
        }
        else
        {
            ScoreboardBuffer::SetWholeness(false, false);
            LOG_DEBUG << LOG_LINE << "Setting ViewBuffer ViewDirection {ViewDirection:" << StringOfViewDir(newDirection) << " }";
            response = ViewBuffer::SetViewDirection(newDirection, info);
        }

        m_localViewDirection = newDirection;
        m_scoreboardBounds = info->bci_ScoreboardBounds;
        m_scoreboardWidth = m_scoreboardBounds.GetWidth();
        m_scoreboardHeight = m_scoreboardBounds.GetHeight();

        return (response);
    }


    BufferChangeResponse WrapRepeatBuffer::Resize(ResizeBufferChangeInfo* info)
    {
        BufferChangeResponse response = bcr_NONE;

        // MSA Why is this method never called, despite the fact that this buffer is often resized???
        // MSA update: it is called, when windows are tiled horizontally. WTF?

        if (m_wrap)
        {
            LOG_DEBUG << LOG_LINE << "Resizing ScoreboardBuffer {ResizeBufferChangeInfo:" << *info << " }";
            // this essentially makes a buffer the size of the scoreboard...
            response = (ScoreboardBuffer::Resize(info));
        }
        else
        {
            // ...while this makes a buffer the size of the window
            LOG_DEBUG << LOG_LINE << "Resizing ViewBuffer {ResizeBufferChangeInfo:" << *info << " }";
            ScoreboardBuffer::SetWholeness(false, false);
            response = (ViewBuffer::Resize(info));
        }

        m_scoreboardBounds = info->bci_ScoreboardBounds;
        m_scoreboardWidth = m_scoreboardBounds.GetWidth();
        m_scoreboardHeight = m_scoreboardBounds.GetHeight();

        return (response);
    }


    BufferChangeResponse WrapRepeatBuffer::SetWrap(Boolean fWrap, WrapRepeatBufferChangeInfo* info)
    {
        BufferChangeResponse response = bcr_NONE;

        if (fWrap != m_wrap)
        {
            //TODO LOG_DEBUG << LOG_LINE << "SetWrap {fWrap:" << ToString(fWrap) << ", WrapRepeatBufferChangeInfo:" << *info << " }";

            m_wrap = fWrap;

            ResizeBufferChangeInfo resize_info;
            resize_info.bci_BufferSize = info->bci_BufferSize;
            resize_info.bci_Scale = info->bci_Scale;
            resize_info.bci_ReferencePt = info->bci_ReferencePt;

            resize_info.bci_ScoreboardBounds = info->bci_ScoreboardBounds;
            resize_info.bci_SoilFrame = info->bci_SoilFrame;

            // Bypass the ScoreboardBuffer resize, since it recalculates
            // the new pixmap size, which in this case is already properly
            // calculated by VPanoramaBuffer::SetWrap() or ::BuildProcessBuffer()

            // MSA NO IT'S NOT

            // NEITHER OF THOSE METHODS EXIST

            // Certainly, VPanorama no longer exists. It is reincarnated in
            // ScrolledWindowBuffer. So, measurements are calculated in:
            // RmScrolledWindows::UpateWrap
            // ScrolledWindowBuffer::CreateProcessBuffer

            response = (Resize(&resize_info));
        }

        m_scoreboardBounds = info->bci_ScoreboardBounds;
        m_scoreboardWidth = m_scoreboardBounds.GetWidth();
        m_scoreboardHeight = m_scoreboardBounds.GetHeight();

        return (response);
    }

    BufferChangeResponse WrapRepeatBuffer::SetRepeat(bool fRepeat)
    {
        BufferChangeResponse response = bcr_NONE;

        if (fRepeat != m_repeat)
        {
            //TODO LOG_DEBUG << LOG_LINE << "SetRepeat {fRepeat:" << ToString(fRepeat) << "}";

            m_repeat = fRepeat;
            ForceNextPrepare();
        }

        return (response);
    }


    //static bool logical_xor(bool p, bool q);
    //bool logical_xor(bool p, bool q)
    //{
    //    return ( (p||q) && !(p&&q) );
    //}


    void WrapRepeatBuffer::WrapCoordinate
    (const DoubleCoordinate& dpt,
        double& h0,
        double& v0,
        int& h_wrap,
        int& v_wrap,
        double& h1,
        double& v1) const
    {
        DoubleCoordinateToPt(&dpt, h0, v0, m_localViewDirection);

        h_wrap = 0;
        v_wrap = 0;

        // Always wrap in the horizontal direction
        while ((h0 - (m_scoreboardWidth * h_wrap)) > m_scoreboardBounds.right)
        {
            ++h_wrap;
        }
        while ((h0 - (m_scoreboardWidth * h_wrap)) < m_scoreboardBounds.left)
        {
            --h_wrap;
        }

        // only wrap V if viewing from Top
        if (vTop == m_localViewDirection)
        {
            while ((v0 - (m_scoreboardHeight * v_wrap) > m_scoreboardBounds.bottom))
            {
                ++v_wrap;
            }
            while ((v0 - (m_scoreboardHeight * v_wrap) < m_scoreboardBounds.top))
            {
                --v_wrap;
            }
        }

        h1 = h0 - (m_scoreboardWidth * h_wrap);
        v1 = v0 - (m_scoreboardHeight * v_wrap);
    }

    void WrapRepeatBuffer::WrapBuffer_MoveTo(const DoubleCoordinate& dc)
    {
        if (m_wrap)
        {
            WrapCoordinate(dc, m_currentH0, m_currentV0, m_currentHWrap, m_currentVWrap, m_currentH1, m_currentV1);

            Buffer_MoveTo(m_currentH1, m_currentV1);
        }
        else
        {
            Buffer_MoveTo(dc);
        }
    }

    void WrapRepeatBuffer::WrapBuffer_LineTo(const DoubleCoordinate& dc)
    {
        if (m_wrap)
        {
            // h0 - original h
            // h1 - h wrapped to be within scoreboard bounds
            // h2 - h wrapped&mapped to adjacent virtual scoreboard - for drawing to
            double dst_h0, dst_v0;
            double dst_h1, dst_v1;
            int dst_h_wrap, dst_v_wrap; //destination horizontal&vertical wrap

            WrapCoordinate(dc, dst_h0, dst_v0, dst_h_wrap, dst_v_wrap, dst_h1, dst_v1);

            if (dst_h_wrap == m_currentHWrap)
            {
                if (dst_v_wrap == m_currentVWrap)
                {
                    LOG_DEBUG << LOG_LINE << "Both dst+current Wrap match";
                    // Excellent! both endpoints have same wrap amount in both
                    // directions, just draw as usual
                    Buffer_LineTo(dst_h1, dst_v1);
                }
                else
                {
                    // means viewdirection is vTop
                    LOG_ALERT << "Wrapping in Vertical direction not supported {m_currentVWrap:" << m_currentVWrap << ", dst_v_wrap:" << dst_v_wrap << "}";
                    //throw (new UnimplementedFunctionException("Cannot wrap in vertical direction in View from Top"));
                }
            }
            else
            {
                // the start and end points are in different scoreboards
                if (1 == CSAbs(dst_h_wrap - m_currentHWrap))
                {
                    // Not so bad, the endpoints are in adjacent virtual
                    // scoreboards. In this case, we just draw the line twice:
                    // once for each adjacent scoreboard. For this, we need to
                    // use wrapped&mapped values.

                    double currentH2 = m_currentH1 + (m_scoreboardWidth * (m_currentHWrap - dst_h_wrap));
                    double dst_h2 = dst_h1 + (m_scoreboardWidth * (dst_h_wrap - m_currentHWrap));

                    Buffer_Line(currentH2, m_currentV1, dst_h1, dst_v1);
                    Buffer_Line(m_currentH1, m_currentV1, dst_h2, dst_v1);
                }
                else
                {
                    //
                    // If they are not adjacent, we are in terrible trouble so far.
                    // ie. TODO work on non-adjacent scoreboard drawing
                    LOG_ALERT << "Wrapping not between adjacent scoreboards {m_currentHWrap:" << m_currentHWrap
                        << ", dst_h_wrap:" << dst_h_wrap << "}";
                    //throw (new UnimplementedFunctionException("Non-Adjacent Scoreboard Wrapped Drawing"));
                }
            }

            m_currentH0 = dst_h0;
            m_currentV0 = dst_v0;
            m_currentH1 = dst_h1;
            m_currentV1 = dst_v1;
            m_currentHWrap = dst_h_wrap;
            m_currentVWrap = dst_v_wrap;
        }
        else
        {
            Buffer_LineTo(dc);
        }
    }

    void WrapRepeatBuffer::WrapBuffer_LineTo(const DoubleCoordinate& dc, const wxColour& colour)
    {
        // colour setup - must do both pen and brush
        wxPen* tmp_pen = SetTemporaryPenColour(colour);
        WrapBuffer_LineTo(dc);
        RestorePenColour(tmp_pen);
    }

    void WrapRepeatBuffer::WrapBuffer_Line(const DoubleCoordinate& dcoord1, const DoubleCoordinate& dcoord2)
    {
        if (m_wrap)
        {
            // h0 - original h
            // h1 - h wrapped to be within scoreboard bounds
            // h2 - h wrapped&mapped to adjacent virtual scoreboard - for drawing to
            double src_h0, src_v0;
            double src_h1, src_v1;
            int src_h_wrap, src_v_wrap; //destination horizontal&vertical wrap
            WrapCoordinate(dcoord1, src_h0, src_v0, src_h_wrap, src_v_wrap, src_h1, src_v1);

            double dst_h0, dst_v0;
            double dst_h1, dst_v1;
            int dst_h_wrap, dst_v_wrap; //destination horizontal&vertical wrap
            WrapCoordinate(dcoord2, dst_h0, dst_v0, dst_h_wrap, dst_v_wrap, dst_h1, dst_v1);

            if (dst_h_wrap == src_h_wrap)
            {
                if (dst_v_wrap == src_v_wrap)
                {
                    // Excellent! both endpoints have same wrap amount in both
                    // directions, just draw as usual
                    Buffer_Line(src_h1, src_v1, dst_h1, dst_v1);
                }
                else
                {
                    // means viewdirection is vTop
                    LOG_ALERT << "Wrapping in Vertical direction not supported {src_v_wrap:" << src_v_wrap << ", dst_v_wrap:" << dst_v_wrap << "}";
                    //throw (new UnimplementedFunctionException("Cannot wrap in vertical direction in View from Top"));
                }
            }
            else
            {
                // the start and end points are in different scoreboards
                if (CSAbs(dst_h_wrap - src_h_wrap) < 2)
                {
                    // Not so bad, the endpoints are in adjacent virtual
                    // scoreboards. In this case, we just draw the line twice:
                    // once for each adjacent scoreboard. For this, we need to
                    // use wrapped&mapped values.

                    //LOG_DEBUG << LOG_LINE << "src+dst Wrap between adjacent scoreboards {Src:" << ToString(dcoord1) << ", Dst:" << ToString(dcoord2) << "}";

                    double src_h2 = src_h1 + (m_scoreboardWidth * (src_h_wrap - dst_h_wrap));
                    double dst_h2 = dst_h1 + (m_scoreboardWidth * (dst_h_wrap - src_h_wrap));

                    Buffer_Line(src_h2, src_v1, dst_h1, dst_v1);
                    Buffer_Line(src_h1, src_v1, dst_h2, dst_v1);
                }
                else
                {
                    //
                    // If they are not adjacent, we are in terrible trouble so far.
                    // ie. TODO work on non-adjacent scoreboard drawing
                    //throw (new UnimplementedFunctionException("Non-Adjacent Scoreboard Wrapped Drawing"));
                    LOG_ALERT << "Wrapping not between adjacent scoreboards {src_h_wrap:" << src_h_wrap
                        << ", dst_h_wrap:" << dst_h_wrap << "}";
                    //throw (new UnimplementedFunctionException("Non-Adjacent Scoreboard Wrapped Drawing"));
                }
            }

            m_currentH0 = dst_h0;
            m_currentV0 = dst_v0;
            m_currentH1 = dst_h1;
            m_currentV1 = dst_v1;
            m_currentHWrap = dst_h_wrap;
            m_currentVWrap = dst_v_wrap;
        }
        else
        {
            Buffer_Line(dcoord1, dcoord2);
        }
    }

    void WrapRepeatBuffer::WrapBuffer_Line(const DoubleCoordinate& dcoord1, const DoubleCoordinate& dcoord2, const wxColour& colour)
    {
        wxPen* tmp_pen = SetTemporaryPenColour(colour);
        WrapBuffer_Line(dcoord1, dcoord2);
        RestorePenColour(tmp_pen);
    }

    void WrapRepeatBuffer::WrapBuffer_FrameRect(const DoubleRect& fr)
    {
        Buffer_FrameRect(fr);
    }

    void WrapRepeatBuffer::WrapBuffer_FrameRect(const DoubleRect& fr, const wxColour& colour)
    {
        wxPen* tmp_pen = SetTemporaryPenColour(colour);
        WrapBuffer_FrameRect(fr);
        RestorePenColour(tmp_pen);
    }

    void WrapRepeatBuffer::WrapBuffer_FrameOval(const DoubleRect& fr)
    {
        Buffer_FrameOval(fr);
    }

    void WrapRepeatBuffer::WrapBuffer_FrameOval(const DoubleRect& fr, const wxColour& colour)
    {
        wxPen* tmp_pen = SetTemporaryPenColour(colour);
        WrapBuffer_FrameOval(fr);
        RestorePenColour(tmp_pen);
    }

    void WrapRepeatBuffer::WrapBuffer_PaintRect(const DoubleRect& fr)
    {
        Buffer_PaintRect(fr);
    }

    void WrapRepeatBuffer::WrapBuffer_PaintRect(const DoubleRect& fr, const wxColour& colour)
    {
        wxPen* tmp_pen = SetTemporaryPenColour(colour);
        wxBrush* tmp_brush = SetTemporaryBrushColour(colour);

        WrapBuffer_PaintRect(fr);

        RestoreBrushColour(tmp_brush);
        RestorePenColour(tmp_pen);
    }

    void WrapRepeatBuffer::WrapBuffer_PaintOval(const DoubleRect& fr)
    {
        Buffer_PaintOval(fr);
    }

    void WrapRepeatBuffer::WrapBuffer_PaintOval(const DoubleRect& fr, const wxColour& colour)
    {
        wxPen* tmp_pen = SetTemporaryPenColour(colour);
        wxBrush* tmp_brush = SetTemporaryBrushColour(colour);

        WrapBuffer_PaintOval(fr);

        RestoreBrushColour(tmp_brush);
        RestorePenColour(tmp_pen);
    }

    void WrapRepeatBuffer::WrapBuffer_Dot(const DoubleCoordinate& fc)
    {
        if (m_wrap)
        {
            WrapCoordinate(fc, m_currentH0, m_currentV0, m_currentHWrap, m_currentVWrap, m_currentH1, m_currentV1);

            Buffer_Dot(m_currentH1, m_currentV1);
        }
        else
        {
            Buffer_Dot(fc);
        }
    }

    void WrapRepeatBuffer::WrapBuffer_Dot(const DoubleCoordinate& fc, const wxColour& colour)
    {
        wxPen* tmp_pen = SetTemporaryPenColour(colour);
        WrapBuffer_Dot(fc);
        RestorePenColour(tmp_pen);
    }
} /* namespace rootmap */
