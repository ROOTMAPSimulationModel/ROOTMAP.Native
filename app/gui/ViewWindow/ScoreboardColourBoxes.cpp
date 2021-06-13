#include "app/gui/ViewWindow/ScoreboardColourBoxes.h"

#include "simulation/scoreboard/ScoreboardCollaboration.h"
#include "simulation/scoreboard/ScoreboardCoordinator.h"
#include "simulation/scoreboard/Scoreboard.h"

#include "core/scoreboard/BoundaryArray.h"
#include "core/utility/Utility.h"

//#define LOG_DRAWING
#if defined LOG_DRAWING
#define LOG_DRAWING_FUNCTION(fn) RootMapLogTrace(#fn)
#define LOG_DRAWING_SIMPLE(x) LOG_DEBUG << ToString(x);
#else
#define LOG_DRAWING_FUNCTION(fn)
#define LOG_DRAWING_SIMPLE(x)
#endif

namespace rootmap
{
    using Utility::CSMax;
    using Utility::CSMin;

    RootMapLoggerDefinition(ScoreboardColourBoxes);

    ScoreboardColourBoxes::ScoreboardColourBoxes(ScoreboardCoordinator& scoord,
        const ScoreboardFlags& scoreboardFlags)
        : ScoreboardBuffer(scoord, scoreboardFlags)
    {
        RootMapLoggerInitialisation("rootmap.ScoreboardColourBoxes");
        for (long int ci = 0; ci < colourElementsMax; ++ci)
        {
            m_colourInfo[ci].colourRangeMin = 0;
            m_colourInfo[ci].colourRangeMax = 0;
            m_colourInfo[ci].characteristicRangeMin = 0.0;
            m_colourInfo[ci].characteristicRangeMax = 0.0;
            m_colourInfo[ci].characteristicIndex = 0;
            m_colourInfo[ci].characteristicID = 0;
        }
    }

    ScoreboardColourBoxes::~ScoreboardColourBoxes()
    {
        // Does not own any pointers
    }

    void ScoreboardColourBoxes::DrawScoreboard(const DoubleRect& area, Scoreboard* scoreboard)
    {
        LOG_DRAWING_FUNCTION(ScoreboardColourBoxes_DrawScoreboard);

        if (GetRebuildLevel() <= bbl_Reslap) { return; }

        const ScoreboardStratum& stratum = scoreboard->GetScoreboardStratum();

        // //////////////
        // draw outer bounds (should be last)
        DoubleRect soil_rect;
        GetScoreboardCoordinator().GetScoreboardBounds(stratum, GetViewDirection(), &soil_rect);

        LOG_DRAWING_SIMPLE(area);
        LOG_DRAWING_SIMPLE(soil_rect);

        //
        // Vertical boundaries. Note the use of poorly named GuiUtility functions.
        // Or inappropriate functions.  Originally created for RmScoreboardTableHelper,
        // where a horizontal dimension gives the column headers :-(
        const BoundaryArray& boundaries_v = GetScoreboardCoordinator().GetVisibleVBoundaries(GetViewDirection(), stratum); // scoreboard->GetBoundaryArray(GuiUtility::getVisibleHorizontalDimension(GetViewDirection()));
        size_t num_layers_v = boundaries_v.GetNumLayers();

        const BoundaryArray& boundaries_h = GetScoreboardCoordinator().GetVisibleHBoundaries(GetViewDirection(), stratum); // scoreboard->GetBoundaryArray(GuiUtility::getVisibleVerticalDimension(GetViewDirection()));
        size_t num_layers_h = boundaries_h.GetNumLayers();

        double areaLeft = area.left;
        double areaRight = area.right;
        double areaTop = area.top;
        double areaBottom = area.bottom;

#if defined ROOTMAP_VIEW_FROM_TOP_FIXME
        if (vTop == GetViewDirection())
        {
            areaTop = area.bottom;
            areaBottom = area.top;
        }
#endif // #if defined ROOTMAP_VIEW_FROM_TOP_FIXME

        //CMYColor boxCMY;

        /* Set up some ranges for loop calculations */
        long colourRange[Yellow + 1];
        colourRange[Cyan] = static_cast<long>(m_colourInfo[Cyan].colourRangeMax) - static_cast<long>(m_colourInfo[Cyan].colourRangeMin);
        colourRange[Magenta] = static_cast<long>(m_colourInfo[Magenta].colourRangeMax) - static_cast<long>(m_colourInfo[Magenta].colourRangeMin);
        colourRange[Yellow] = static_cast<long>(m_colourInfo[Yellow].colourRangeMax) - static_cast<long>(m_colourInfo[Yellow].colourRangeMin);

        long rangeReversal[Yellow + 1] = { +1, +1, +1 };
        if (colourRange[Cyan] < 0)
        {
            rangeReversal[Cyan] = -1;
            colourRange[Cyan] = -colourRange[Cyan];
        }
        if (colourRange[Magenta] < 0)
        {
            rangeReversal[Magenta] = -1;
            colourRange[Magenta] = -colourRange[Magenta];
        }
        if (colourRange[Yellow] < 0)
        {
            rangeReversal[Yellow] = -1;
            colourRange[Yellow] = -colourRange[Yellow];
        }

        double characteristicRange[Yellow + 1];
        characteristicRange[Cyan] = m_colourInfo[Cyan].characteristicRangeMax - m_colourInfo[Cyan].characteristicRangeMin;
        characteristicRange[Magenta] = m_colourInfo[Magenta].characteristicRangeMax - m_colourInfo[Magenta].characteristicRangeMin;
        characteristicRange[Yellow] = m_colourInfo[Yellow].characteristicRangeMax - m_colourInfo[Yellow].characteristicRangeMin;

        LOG_DEBUG << "colourRange[Cyan]=" << colourRange[Cyan]
            << " (" << m_colourInfo[Cyan].colourRangeMin
            << "-" << m_colourInfo[Cyan].colourRangeMax
            << "), colourRange[Magenta]=" << colourRange[Magenta]
            << " (" << m_colourInfo[Magenta].colourRangeMin
            << "-" << m_colourInfo[Magenta].colourRangeMax
            << "), colourRange[Yellow]=" << colourRange[Yellow]
            << " (" << m_colourInfo[Yellow].colourRangeMin
            << "-" << m_colourInfo[Yellow].colourRangeMax << ")";

        LOG_DEBUG << "characteristicRange[Cyan]=" << characteristicRange[Cyan]
            << " (" << m_colourInfo[Cyan].characteristicRangeMin
            << "-" << m_colourInfo[Cyan].characteristicRangeMax
            << "), characteristicRange[Magenta]=" << characteristicRange[Magenta]
            << " (" << m_colourInfo[Magenta].characteristicRangeMin
            << "-" << m_colourInfo[Magenta].characteristicRangeMax
            << "), characteristicRange[Yellow]=" << characteristicRange[Yellow]
            << " (" << m_colourInfo[Yellow].characteristicRangeMin
            << "-" << m_colourInfo[Yellow].characteristicRangeMax << ")";

        // draw all the (screen-wise) horizontal layers
        for (size_t idx_v = 1; idx_v <= num_layers_v; ++idx_v)
        {
            DoubleRect boxRect;
            boxRect.top = boundaries_v.GetLayerTop(idx_v);
            boxRect.bottom = boundaries_v.GetLayerBottom(idx_v);

#if defined ROOTMAP_VIEW_FROM_TOP_FIXME
            if (GetViewDirection() == vTop)
            {
                double boxRectTop = boxRect.top;
                boxRect.top = boxRect.bottom;
                boxRect.bottom = boxRectTop;
            }
#endif // #if defined ROOTMAP_VIEW_FROM_TOP_FIXME

            // if any of the boxes in this (screen-wise) horizontal layer are visible thenÉ
            if ((boxRect.top <= areaBottom) || (boxRect.bottom >= areaTop))
            {
                // draw all the boxes from (screen-wise) left to right
                for (size_t idx_h = 1; idx_h <= num_layers_h; ++idx_h)
                {
                    boxRect.left = boundaries_h.GetLayerTop(idx_h);
                    boxRect.right = boundaries_h.GetLayerBottom(idx_h);
                    // if this box is visible, then
                    if ((boxRect.left <= areaRight) || (boxRect.right >= areaLeft))
                    {
                        ColourElementValue_t colour_val[colourElementsMax];
                        // calculate the C/M/Y and draw the resulting colour 
                        for (int cix = Cyan; cix <= Yellow; cix += 1)
                        {
                            double box_mean = scoreboard->GetMean(idx_h, idx_v, GetViewDirection(), m_colourInfo[cix].characteristicIndex);
                            double restricted_box_mean = CSMin(CSMax(m_colourInfo[cix].characteristicRangeMin, box_mean), m_colourInfo[cix].characteristicRangeMax);
                            signed long colourRangeFraction = (((restricted_box_mean - m_colourInfo[cix].characteristicRangeMin) / characteristicRange[cix]) * colourRange[cix]);
                            // signed long colourRangeFraction = Utility::RandomRange((signed long)255);

                            // if this colour field's range is negative
                            // 1: the colourRangeMin will be the max, and 2: rangeReversal will be -1
                            colour_val[cix] = m_colourInfo[cix].colourRangeMin + colourRangeFraction * rangeReversal[cix];

                            //if ( (1==idx_h) && (1==idx_v) )
                            //{
                            //LOG_DEBUG << "box_mean=" << box_mean
                            //<< " restricted_box_mean=" << restricted_box_mean
                            //<< " colourRangeFraction=" << colourRangeFraction
                            //<< " colour_val[" << cix << "]=" << colour_val[cix];
                            //LOG_DEBUG << "colour_val[" << cix << "] ="
                            //<< " m_colourInfo[" << cix << "].colourRangeMin"
                            //<< " + colourRangeFraction*rangeReversal[" << cix << "] => "
                            //<< m_colourInfo[cix].colourRangeMin << "+" << colourRangeFraction << "*" << rangeReversal[cix] << "";
                            //LOG_DEBUG << "m_colourInfo[cix].characteristicRangeMin=" << m_colourInfo[cix].characteristicRangeMin
                            //<< ", m_colourInfo[cix].characteristicRangeMax=" << m_colourInfo[cix].characteristicRangeMax << "";
                            //}
                        }
                        //ColourElementValue_t cyan = colourVal[Cyan];
                        //ColourElementValue_t magenta = colourVal[Magenta];
                        //ColourElementValue_t yellow = colourVal[Yellow];

                        wxColour boxRGB((255 - colour_val[Cyan]), (255 - colour_val[Magenta]), (255 - colour_val[Yellow]));

                        //if ( (1==idx_h) && (1==idx_v) )
                        //{
                        //LOG_DEBUG << "box[1,1] Red=" << boxRGB.Red() << ", Green=" << boxRGB.Green() << ", Blue=" << boxRGB.Blue();
                        //}

                        Buffer_PaintRect(boxRect, boxRGB);
                    }
                }
            }
        }
    }


    void ScoreboardColourBoxes::SetCharacteristic(CMYSelect cmy, const CharacteristicColourInfo& info)
    {
        m_colourInfo[cmy].colourRangeMin = info.colourRangeMin;
        m_colourInfo[cmy].colourRangeMax = info.colourRangeMax;

        m_colourInfo[cmy].characteristicRangeMin = info.characteristicRangeMin;
        m_colourInfo[cmy].characteristicRangeMax = info.characteristicRangeMax;

        m_colourInfo[cmy].characteristicIndex = info.characteristicIndex;
        m_colourInfo[cmy].characteristicID = info.characteristicID;

        SetRebuildLevel(bbl_Complete);
    }

    void ScoreboardColourBoxes::SetCharacteristicColourInfo(const CharacteristicColours_t& info)
    {
        m_colourInfo[0] = info[0];
        m_colourInfo[1] = info[1];
        m_colourInfo[2] = info[2];
        SetRebuildLevel(bbl_Complete);
    }

    bool ScoreboardColourBoxes::UpdateBoxCharacteristic(EditCellInfo* eci)
    {
        for (long int ci = 0; ci < colourElementsMax; ++ci)
        {
            if (m_colourInfo[ci].characteristicIndex == eci->characteristicIndex)
            {
                SetRebuildLevel(bbl_Complete);
                return true;
            }
        }

        return false;
    }
} /* namespace rootmap */
