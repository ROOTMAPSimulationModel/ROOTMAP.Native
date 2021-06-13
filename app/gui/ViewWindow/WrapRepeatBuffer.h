#ifndef WrapRepeatBuffer_H
#define WrapRepeatBuffer_H
#include "app/gui/ViewWindow/ScoreboardBuffer.h"
#include "simulation/scoreboard/ScoreboardCoordinator.h"
#include "core/log/Logger.h"
#include "core/common/Structures.h"

namespace rootmap
{
    /**
     * The WrapRepeatBuffer is an intermediate class between the Process buffer
     * and the Scoreboard/View buffer, which correctly draws elements in "Wrap"
     * mode.
     *
     * In this class, we override main ViewBuffer drawing routines so that if
     * this buffer is wrapping, everything gets drawn within the scoreboard,
     * regardless of how far OUTSIDE of the scoreboard an item's extents are.
     *
     * It also overrides some ScoreboardBuffer functions, to ensure that buffers
     * are created of the correct size if Wrapping is NOT on (full window).
     */
    class WrapRepeatBuffer : public ScoreboardBuffer
    {
    public:
        WrapRepeatBuffer(ScoreboardCoordinator& scoord,
            bool fWrap,
            bool fRepeat,
            const ScoreboardFlags& scoreboardFlags);

        /**
         *
         */
        virtual BufferChangeResponse Create
        (int width,
            int height,
            int depth,
            double scale,
            ViewDirection direction,
            const DoubleRect& scoreboardBounds,
            const DoubleRect& soilFrame);

        ~WrapRepeatBuffer();

        /**
         * Resize is intercepted to call either the ScoreboardBuffer or
         * ViewBuffer function as appropriate to the Wrap setting.
         */
        virtual BufferChangeResponse Resize(ResizeBufferChangeInfo* info);

        /**
         * SetViewDirection is intercepted
         */
        virtual BufferChangeResponse SetViewDirection(ViewDirection newDirection, ViewDirectionBufferChangeInfo* info);

        // specific functions
        virtual BufferChangeResponse SetWrap(bool fWrap, WrapRepeatBufferChangeInfo* info);
        virtual BufferChangeResponse SetRepeat(bool fRepeat);

        /** MoveTo moves the */
        void WrapBuffer_MoveTo(const DoubleCoordinate& dc);
        /** LineTo with prevailing colour */
        void WrapBuffer_LineTo(const DoubleCoordinate& dc);
        /** LineTo with specified colour */
        void WrapBuffer_LineTo(const DoubleCoordinate& dc, const wxColour& colour);

        /** Line with prevailing colour */
        void WrapBuffer_Line(const DoubleCoordinate& dcoord1, const DoubleCoordinate& dcoord2);
        /** Line with specified colour */
        void WrapBuffer_Line(const DoubleCoordinate& dcoord1, const DoubleCoordinate& dcoord2, const wxColour& colour);

        /** Frame a Rectangle (prevailing colour) */
        void WrapBuffer_FrameRect(const DoubleRect& fr);
        /** Frame a Rectangle (specific colour) */
        void WrapBuffer_FrameRect(const DoubleRect& fr, const wxColour& colour);
        /** Frame an Oval (prevailing colour) */
        void WrapBuffer_FrameOval(const DoubleRect& fr);
        /** Frame an Oval (specific colour) */
        void WrapBuffer_FrameOval(const DoubleRect& fr, const wxColour& colour);

        /** Paint (ie. fill in) a Rectangle (prevailing colour) */
        void WrapBuffer_PaintRect(const DoubleRect& fr);
        /** Paint (ie. fill in) a Rectangle (specific colour) */
        void WrapBuffer_PaintRect(const DoubleRect& fr, const wxColour& colour);

        /** Paint (ie. fill in) an Oval (prevailing colour) */
        void WrapBuffer_PaintOval(const DoubleRect& fr);
        /** Paint (ie. fill in) an Oval (specific colour) */
        void WrapBuffer_PaintOval(const DoubleRect& fr, const wxColour& colour);

        /** Draw a Dot (prevailing colour) */
        void WrapBuffer_Dot(const DoubleCoordinate& fc);
        /** Draw a Dot (specific colour) */
        void WrapBuffer_Dot(const DoubleCoordinate& fc, const wxColour& colour);
    private:
        RootMapLoggerDeclaration();

        /**  */
        bool m_wrap;

        /**  */
        bool m_repeat;

        /**  */
        ViewDirection m_localViewDirection;

        /**  */
        double m_currentH0;
        /**  */
        double m_currentV0;
        /** equal to (m_currentH0-(m_scoreboardWidth*m_wrapCountH)) */
        double m_currentH1;
        /** equal to (m_currentV0-(m_scoreboardHeight*m_wrapCountV)) */
        double m_currentV1;
        /**  */
        int m_currentHWrap;
        /**  */
        int m_currentVWrap;

        /**  */
        DoubleRect m_scoreboardBounds;
        /**  */
        double m_scoreboardWidth;
        /**  */
        double m_scoreboardHeight;


        /**
         *
         */
        void WrapCoordinate(const DoubleCoordinate& coord, double& h0, double& v0, int& h_wrap_cnt, int& v_wrap_cnt, double& h1, double& v1) const;
    }; // class WrapRepeatBuffer 
} /* namespace rootmap */

#endif // #ifndef WrapRepeatBuffer_H
