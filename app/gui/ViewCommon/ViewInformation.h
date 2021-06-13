#ifndef ViewInformation_H
#define ViewInformation_H
/////////////////////////////////////////////////////////////////////////////
// Name:        ViewInformation.h
// Purpose:     Declaration of the ViewInformation class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2008-06-16 02:07:36 +0800 (Mon, 16 Jun 2008) $
// $Revision: 1 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////

#include "simulation/common/Types.h"
#include "simulation/scoreboard/ScoreboardFlags.h"

#include "core/common/Structures.h"
#include "core/common/DoubleCoordinates.h"
#include "core/scoreboard/ScoreboardStratum.h"

#include "wx/gdicmn.h"

#include <vector>


namespace rootmap
{
    class Process;
    class ViewDAI;

    class ViewInformation
    {
    public:
        // default constructor and destructor
        ViewInformation();
        ~ViewInformation();

        // copy constructor
        ViewInformation(const ViewInformation& original);
        // assignment from View DAI
        ViewInformation& operator=(const ViewDAI& data);

        ViewDirection GetViewDirection() const;
        const DoubleCoordinate& GetReferenceIndex() const;
        DoubleCoordinate& GetReferenceIndex();

        /**
         * uses the ReferenceIndex and ViewDirection to determine the
         * reference point from the current direction
         */
        DoublePt GetReferencePoint() const;

        void GetReferenceIndex(DoubleCoordinate& fc) const;

        double GetScale() const;
        double GetZoomRatio() const;
        bool DoesRepeat() const;
        bool DoesWrap() const;
        bool DoesBoxes() const;
        bool DoesBoundaries() const;
        bool DoesBoxColours() const;

        // short int GetScoreboards();
        bool DoesScoreboard(const ScoreboardStratum& ss) const;
        const ScoreboardFlags& GetScoreboardFlags() const;

        void SetViewDirection(ViewDirection vd);
        void SetReferenceIndex(DoubleCoordinate& fc);
        void SetReferenceIndex(DoubleCoordinate* fc);
        /**
         * uses the view direction to set the reference index
         */
        void SetReferencePoint(const DoublePt& refpt);
        //DEPRECATED - only set zoom ratio
        //void SetScale(double v);
        void SetZoomRatio(double v);
        void SetRepeat(bool v);
        void SetWrap(bool v);
        void SetBoxes(bool v);
        void SetBoundaries(bool v);
        void SetBoxColours(bool v);
        void SetScoreboard(const ScoreboardStratum& ss, bool fTurnOn);

        // accessor + mutator for list of processes being displayed
        typedef std::vector<Process *> ProcessArray;
        const ProcessArray& GetDisplayProcesses() const;
        ProcessArray& GetDisplayProcesses();
        size_t GetNumDisplayProcesses();
        void SetDisplayProcesses(const ProcessArray& displayProcesses);

        // accessor + mutator for the placement data member
        void SetBounds(const wxRect& bounds);
        const wxRect& GetBounds() const;

        // accessor + mutator for the scoreboard colour boxes' information
        const CharacteristicColours_t& GetCharacteristicColourInfo() const;
        void SetCharacteristicColourInfo(const CharacteristicColours_t& cci);

        // assignment operator
        ViewInformation& operator=(const ViewInformation& rhs);

    private:
        // array of processes being displayed
        ProcessArray m_view_displayProcesses;

        //private:
        wxRect m_view_place; // default = from 'WIND' resource

        // box colour information
        CharacteristicColours_t m_view_characteristicColourInfo;

        //public:
        DoubleCoordinate m_view_referenceindex; // default = { 0.0, 0.0, 0.0 }
        double m_view_scale; // default = 
        double m_view_zoomratio;
        ViewDirection m_view_direction; // default = vFront

        ScoreboardFlags m_view_scoreboards_bitset; //

        bool m_view_repeat; // default = false
        bool m_view_wrap; // default = false
        bool m_view_boxes; // default = true
        bool m_view_boundaries; // default = true
        bool m_view_boxcolours; // default = true
        //short int        m_view_scoreboards;            // default = 0xFFFF (all ON)
    };

    inline ViewDirection ViewInformation::GetViewDirection() const { return (m_view_direction); }
    inline const DoubleCoordinate& ViewInformation::GetReferenceIndex() const { return m_view_referenceindex; }
    inline DoubleCoordinate& ViewInformation::GetReferenceIndex() { return m_view_referenceindex; }
    inline void ViewInformation::GetReferenceIndex(DoubleCoordinate& fc) const { fc = m_view_referenceindex; }

    inline DoublePt ViewInformation::GetReferencePoint() const
    {
        DoublePt ref_pt(m_view_referenceindex, m_view_direction);
        return ref_pt;
    }

    inline double ViewInformation::GetScale() const { return m_view_scale * m_view_zoomratio; }
    inline double ViewInformation::GetZoomRatio() const { return m_view_zoomratio; }
    inline bool ViewInformation::DoesRepeat() const { return m_view_repeat; }
    inline bool ViewInformation::DoesWrap() const { return m_view_wrap; }
    inline bool ViewInformation::DoesBoxes() const { return m_view_boxes; }
    inline bool ViewInformation::DoesBoundaries() const { return m_view_boundaries; }
    inline bool ViewInformation::DoesBoxColours() const { return m_view_boxcolours; }
    inline const ScoreboardFlags& ViewInformation::GetScoreboardFlags() const { return m_view_scoreboards_bitset; }
    inline void ViewInformation::SetViewDirection(ViewDirection vd) { m_view_direction = vd; }
    inline void ViewInformation::SetReferenceIndex(DoubleCoordinate& fc) { m_view_referenceindex = fc; }
    inline void ViewInformation::SetReferenceIndex(DoubleCoordinate* fc) { m_view_referenceindex = *fc; }
    inline void ViewInformation::SetZoomRatio(double v) { m_view_zoomratio = v; }
    inline void ViewInformation::SetRepeat(bool v) { m_view_repeat = v; }
    inline void ViewInformation::SetWrap(bool v) { m_view_wrap = v; }
    inline void ViewInformation::SetBoxes(bool v) { m_view_boxes = v; }
    inline void ViewInformation::SetBoundaries(bool v) { m_view_boundaries = v; }
    inline void ViewInformation::SetBoxColours(bool v) { m_view_boxcolours = v; }
    inline size_t ViewInformation::GetNumDisplayProcesses() { return m_view_displayProcesses.size(); }
} /* namespace rootmap */

#endif // #ifndef ViewInformation_H
