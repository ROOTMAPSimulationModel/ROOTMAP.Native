#ifndef ScrolledWindowBuffer_H
#define ScrolledWindowBuffer_H
/////////////////////////////////////////////////////////////////////////////
// Name:        ScrolledWindowBuffer.h
// Purpose:     Declaration of the ClassTemplate class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2009-07-07 02:40:29 +0800 (Tue, 07 Jul 2009) $
// $Revision: 67 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/ViewWindow/RmScrolledWindow.h"
#include "app/gui/ViewWindow/ViewBuffer.h"
#include "app/gui/ViewCommon/ViewInformation.h"

#include "simulation/scoreboard/ScoreboardCoordinator.h"
#include "simulation/scoreboard/ScoreboardFlags.h"

#include "core/log/Logger.h"
#include "core/macos_compatibility/CCollaborator.h"

// #include "ScoreboardCollaboration.h"

//#include "wx/gdicmn.h"
#include "wx/colour.h"
#include "wx/dc.h"

#include <list>

// #define kZeroNewBounds true
// #define kRestoreNewBounds false

// #define kFromBuffer true
// #define kToBuffer false

// #ifdef _CS_DEBUG_
// #define PANORAMA_BUFFER_DEBUG
// #ifdef PANORAMA_BUFFER_DEBUG
// //#define _CS_DEBUG_REPEATPROCBUFFS_
// //#define PANORAMA_BUFFER_DEBUG_CONTENTCHANGEINFO
// class CDataFile;
// #endif
// #endif

class RmScrolledWindow;

namespace rootmap
{
    // Forward class declarations
    class ProcessDrawingBuffer;
    class Process;

    class ScoreboardCoordinator;
    class ScoreboardBuffer;
    class ScoreboardColourBoxes;
    class ScoreboardBoxBoundaries;

    struct EditCellInfo;

    class ScrolledWindowBuffer
        : public ViewBuffer
        , public ::CCollaborator
    {
    public:
        // Firstly : Construction, Initialisation, Destruction
        ScrolledWindowBuffer(ScoreboardCoordinator& scoord, RmScrolledWindow& parent, int pixelDepth);
        virtual ~ScrolledWindowBuffer();

        BufferChangeResponse Create(const rootmap::ViewInformation& viewInfo);

        const ViewInformation& GetViewInformation() const;
        ViewInformation& GetViewInformation();
        bool DoesRepeat() const;
        bool DoesWrap() const;
        bool DoesBoxes() const;
        bool DoesBoxColours() const;
        bool DoesBoundaries() const;
        bool IsScoreboardVisible(const ScoreboardStratum& stratum) const;
        ScoreboardCoordinator& GetScoreboardCoordinator();
        const ScoreboardCoordinator& GetScoreboardCoordinator() const;

        // Secondly : overriding inherited functionality.
        // Part one - to align copy area rects
        virtual void SetBounds(const wxRect& newBounds);
#if defined SCROLLWINDOWBUFFER_SCROLL_USED
        virtual void Scroll(double hDelta, double vDelta);
#endif // #if defined SCROLLWINDOWBUFFER_SCROLL_USED
        virtual void ScrollTo(double hDelta, double vDelta);

        virtual void ProviderChanged(CCollaborator* aProvider, long reason, CCollaboratorInfo* info);


        // To update secondary buffers
        virtual BufferChangeResponse CreateSecondaryBuffers(const rootmap::ViewInformation& viewInfo);
        virtual void DestroySecondaryBuffers();

        virtual BufferChangeResponse Resize(ResizeBufferChangeInfo* info);
        virtual BufferChangeResponse SetViewDirection(ViewDirection newDirection, ViewDirectionBufferChangeInfo* info);

        virtual void DrawContents(wxDC& dc,
            const wxRect& clientBoundsPix,
            const wxRect& virtualBoundsPix,
            const DoubleRect& virtualBoundsSoil);
        virtual void DrawContents(const DoubleRect& area);
        void DrawScoreboardBounds(const DoubleRect& clientBoundsSoil, const DoubleRect& scoreboardBoundsSoil);
        void DrawScoreboardBounds(DoubleRect* area, Scoreboard* scoreboard);

        // Thirdly : new functionality
        virtual BufferChangeResponse SetRepeat(bool fRepeat);
        virtual BufferChangeResponse SetWrap(bool fWrap, WrapRepeatBufferChangeInfo* info);
        virtual BufferChangeResponse SetBoxes(bool fBoxes);
        virtual BufferChangeResponse SetBoundaries(bool fBoundaries);
        virtual BufferChangeResponse SetBoxColours(bool fBoxColours);
        virtual BufferChangeResponse SetScoreboard(const ScoreboardStratum& stratum, bool fTurnOn);
        virtual BufferChangeResponse SetScoreboards(const ScoreboardFlags& flags);
        virtual BufferChangeResponse SetCharacteristicColourInfo(const CharacteristicColours_t& info);

        virtual BufferChangeResponse SetProcesses(const ProcessArray& processes);

        virtual void RemoveProcessBuffer(ProcessDrawingBuffer* process_buffer);
        virtual void RemoveProcessBuffer(Process* process);

        void PostTimestampNotification();

    private:
        virtual BufferChangeResponse CreateBoxBoundaryBuffers();
        virtual BufferChangeResponse CreateColourBoxBuffers();
        virtual BufferChangeResponse CreateProcessBuffers(const ViewInformation& viewInfo);
        virtual BufferChangeResponse CreateProcessBuffer(Process* process);

        virtual void AddProcessBuffer(ProcessDrawingBuffer* process_buffer);

        void CalculateDrawAreas(DoubleRect& minor_area, DoubleRect& major_area, wxDC& dc);

        void RedrawSecondaryBuffers(DoubleRect* scoreboardBoundsSoil,
            DoubleRect* clientBoundsSoil,
            const wxRect& scoreboardBoundsPix,
            const wxRect& clientBoundsPix);

        void RedrawBuffer(ScoreboardBuffer* buffer,
            DoubleRect* drawArea,
            const wxRect& blitBoundsPix,
            bool createTransparencyMask = true);

        void ReslapArea(ScoreboardBuffer* buffer,
            const wxRect& area);

        void UpdateBoxCharacteristic(EditCellInfo* eci);

        void AlignCopyArea(ScoreboardBuffer* buffer, wxRect& dstArea);

        //#define PANORAMA_BUFFER_USE_AdjustNewBounds
#ifdef PANORAMA_BUFFER_USE_AdjustNewBounds
        void        AdjustNewBounds(Rect    *newBounds,
            Rect    *localBounds,
            bool    fZeroNewBounds);
#endif
        void Buffer_UnconvertRect(const wxRect& window_rect, DoubleRect& soil_rect);

        RootMapLoggerDeclaration();

        ///
        ///
        RmScrolledWindow& m_parent;

        ///
        ///
        ScoreboardCoordinator& m_scoreboardCoordinator;

        ///
        ///
        int m_pixelDepth;

        ///
        ///
        DoubleRect m_clientBoundsSoil;

        ///
        /// if the secondary buffers are as tall as the sb (false they are as tall as the client window)
        bool m_secondariesFitWithinV;

        ///
        /// if the secondary buffers are as wide as the sb (false they are as wide as the client window)
        bool m_secondariesFitWithinH;

        ///
        ///
        ::wxColour m_scoreboardBoundsColour;

        ///
        ///
        bool m_scoreboardBoundaries;

        ///
        /// Scoreboard Box Boundary Buffer
        ScoreboardBoxBoundaries* m_boxBoundaries;

        ///
        /// Scoreboard Colour Boxes Buffer
        ScoreboardColourBoxes* m_boxColours;

        ///
        /// type definition for a list of Process Buffers
        typedef std::list<ProcessDrawingBuffer *> ProcessBufferContainer;

        ///
        /// the drawing buffers
        ProcessBufferContainer m_processBuffers;

    public:
        static int s_scrollAmountPixels;
    }; // class ScrolledWindowBuffer


    inline const ViewInformation& ScrolledWindowBuffer::GetViewInformation() const
    {
        return m_parent.GetViewInformation();
    }

    inline ViewInformation& ScrolledWindowBuffer::GetViewInformation()
    {
        return m_parent.GetViewInformation();
    }

    inline bool ScrolledWindowBuffer::DoesRepeat() const
    {
        return GetViewInformation().DoesRepeat();
    }

    inline bool ScrolledWindowBuffer::DoesWrap() const
    {
        return GetViewInformation().DoesWrap();
    }

    inline bool ScrolledWindowBuffer::DoesBoxes() const
    {
        return GetViewInformation().DoesBoxes();
    }

    inline bool ScrolledWindowBuffer::DoesBoxColours() const
    {
        return GetViewInformation().DoesBoxColours();
    }

    inline bool ScrolledWindowBuffer::DoesBoundaries() const
    {
        return GetViewInformation().DoesBoundaries();
    }

    inline bool ScrolledWindowBuffer::IsScoreboardVisible(const ScoreboardStratum& stratum) const
    {
        return GetViewInformation().DoesScoreboard(stratum);
    }

    inline ScoreboardCoordinator& ScrolledWindowBuffer::GetScoreboardCoordinator()
    {
        return m_scoreboardCoordinator;
    }

    inline const ScoreboardCoordinator& ScrolledWindowBuffer::GetScoreboardCoordinator() const
    {
        return m_scoreboardCoordinator;
    }
} /* namespace rootmap */

#endif // #ifndef ScrolledWindowBuffer_H
