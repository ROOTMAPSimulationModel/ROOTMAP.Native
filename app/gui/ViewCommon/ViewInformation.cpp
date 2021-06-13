/////////////////////////////////////////////////////////////////////////////
// Name:        ViewInformation.cpp
// Purpose:     Implementation of the ViewInformation class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2008-06-16 02:07:36 +0800 (Mon, 16 Jun 2008) $
// $Revision: 1 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/ViewCommon/ViewInformation.h"

#include "simulation/data_access/interface/ViewDAI.h"
#include "simulation/process/common/Process.h"
#include "simulation/process/common/ProcessCoordinator.h"

#include "core/common/Constants.h"

#include <stdlib.h>

namespace rootmap
{
    ViewInformation::ViewInformation()
        : m_view_direction(vFront)
        , m_view_referenceindex(0.0, 0.0, 0.0)
        , m_view_scale(5.0)
        , m_view_zoomratio(1.0)
        , m_view_repeat(kNotRepeat)
        , m_view_wrap(kNotWrap)
        , m_view_boxes(true)
        , m_view_boundaries(true)
        , m_view_boxcolours(true)
        , m_view_scoreboards_bitset(0)
    {
#if defined ROOTMAP_TODO
        SetRect(&(m_view_place), 0, 0, 0, 0);
#endif // #if defined ROOTMAP_TODO

        for (long ci = 0; ci <= 2; ++ci)
        {
            m_view_characteristicColourInfo[ci].colourRangeMin = 0;
            m_view_characteristicColourInfo[ci].colourRangeMax = colourElementMaxValue;
            m_view_characteristicColourInfo[ci].characteristicRangeMin = 0.0;
            m_view_characteristicColourInfo[ci].characteristicRangeMax = 30.0;

            // we can't set any real values based on real processes or characteristics
            // here, since these are not activated until NewFile() or OpenFile() are called.
            // We may as well just put in nothing at all
            m_view_characteristicColourInfo[ci].characteristicIndex = 0;
            m_view_characteristicColourInfo[ci].characteristicID = 0;
        }
    }

    ViewInformation::~ViewInformation()
    {
    }


    ViewInformation::ViewInformation(const ViewInformation& original)
    {
        // uses the assignment operator for this class !!
        *this = original;
    }

    ViewInformation& ViewInformation::operator=(const ViewDAI& data)
    {
        m_view_direction = data.GetViewDirection();
        m_view_referenceindex = data.GetReferenceIndex();
        m_view_scale = data.GetScale();
        m_view_zoomratio = 1.0; // data.GetZoomRatio();
        m_view_repeat = data.DoesRepeat();
        m_view_wrap = data.DoesWrap();
        m_view_boxes = data.DoesBoxes();
        m_view_boundaries = data.DoesBoundaries();
        m_view_boxcolours = data.DoesBoxColours();
        m_view_scoreboards_bitset = data.GetScoreboards();

        // the colours and processes in the data aren't ViewInformation-friendly,
        // they must be explicitly set by the appropriate mutators (a.k.a. Setters)
        std::vector<std::string> process_data = data.GetProcesses();

        ViewDAI::CharacteristicColourDataArray colour_data = data.GetCharacteristicColourInfo();
        for (unsigned int ci = 0; ci <= 2; ++ci)
        {
            m_view_characteristicColourInfo[ci].colourRangeMin = colour_data[ci].colourRangeMin;
            m_view_characteristicColourInfo[ci].colourRangeMax = colour_data[ci].colourRangeMax;
            m_view_characteristicColourInfo[ci].characteristicRangeMin = colour_data[ci].characteristicRangeMin;
            m_view_characteristicColourInfo[ci].characteristicRangeMax = colour_data[ci].characteristicRangeMax;
            m_view_characteristicColourInfo[ci].characteristicIndex = colour_data[ci].characteristicIndex;
            m_view_characteristicColourInfo[ci].characteristicID = colour_data[ci].characteristicID;
        }

        return *this;
    }

    ViewInformation& ViewInformation::operator=(const ViewInformation& rhs)
    {
        if (&rhs != this)
        {
            m_view_direction = rhs.m_view_direction;
            m_view_referenceindex = rhs.m_view_referenceindex;
            m_view_scale = rhs.m_view_scale;
            m_view_zoomratio = rhs.m_view_zoomratio;
            m_view_repeat = rhs.m_view_repeat;
            m_view_wrap = rhs.m_view_wrap;
            m_view_boxes = rhs.m_view_boxes;
            m_view_boundaries = rhs.m_view_boundaries;
            m_view_boxcolours = rhs.m_view_boxcolours;
            m_view_scoreboards_bitset = rhs.m_view_scoreboards_bitset;
            m_view_place = rhs.m_view_place;
            m_view_characteristicColourInfo[Yellow] = rhs.m_view_characteristicColourInfo[Yellow];
            m_view_characteristicColourInfo[Cyan] = rhs.m_view_characteristicColourInfo[Cyan];
            m_view_characteristicColourInfo[Magenta] = rhs.m_view_characteristicColourInfo[Magenta];

            SetDisplayProcesses(rhs.m_view_displayProcesses);
        }

        return (*this);
    }

    void ViewInformation::SetReferencePoint(const DoublePt& /* refpt */)
    {
        // MSA TODO update this 10.07.26
        //m_view_referenceindex.set(refpt, m_view_direction);
    }


    const ViewInformation::ProcessArray& ViewInformation::GetDisplayProcesses() const
    {
        return (m_view_displayProcesses);
    }

    ViewInformation::ProcessArray& ViewInformation::GetDisplayProcesses()
    {
        return (m_view_displayProcesses);
    }

    void ViewInformation::SetDisplayProcesses(const ViewInformation::ProcessArray& displayProcesses)
    {
        m_view_displayProcesses.assign(displayProcesses.begin(), displayProcesses.end());
    }

    bool ViewInformation::DoesScoreboard(const ScoreboardStratum& ss) const
    {
        return (m_view_scoreboards_bitset.test(static_cast<size_t>(ss.value())));
    }

    void ViewInformation::SetScoreboard(const ScoreboardStratum& ss, bool fTurnOn)
    {
        m_view_scoreboards_bitset.set(static_cast<size_t>(ss.value()), fTurnOn);
    }

    void ViewInformation::SetBounds(const wxRect& bounds)
    {
        m_view_place = bounds;
    }

    const wxRect& ViewInformation::GetBounds() const
    {
        return m_view_place;
    }

    const CharacteristicColours_t& ViewInformation::GetCharacteristicColourInfo() const
    {
        return (m_view_characteristicColourInfo);
    }

    void ViewInformation::SetCharacteristicColourInfo(const CharacteristicColours_t& cci)
    {
        m_view_characteristicColourInfo[Cyan] = cci[Cyan];
        m_view_characteristicColourInfo[Magenta] = cci[Magenta];
        m_view_characteristicColourInfo[Yellow] = cci[Yellow];
    }
} /* namespace rootmap */
