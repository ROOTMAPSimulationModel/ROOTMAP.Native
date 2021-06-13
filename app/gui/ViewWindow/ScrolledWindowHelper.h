#ifndef ScrolledWindowHelper_H
#define ScrolledWindowHelper_H
/////////////////////////////////////////////////////////////////////////////
// Name:        ScrolledWindowHelper.h
// Purpose:     Declaration of the ScrolledWindowHelper class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2008-06-16 02:07:36 +0800 (Mon, 16 Jun 2008) $
// $Revision: 1 $
// Copyright:   ©2006 University of Tasmania
//
// This class is a conversion helper for the RmScrolledWindow, to use when
// updating/maintaining the virtual bounds
/////////////////////////////////////////////////////////////////////////////

#include "core/common/Types.h"
#include "core/common/Structures.h"
#include "core/scoreboard/ScoreboardStratum.h"
#include "core/common/DoubleCoordinates.h"

#include "wx/gdicmn.h"

using rootmap::DoubleRect;
using rootmap::ViewDirection;

class ScrolledWindowHelper
{
public:
    ScrolledWindowHelper(ViewDirection viewDir, double scale, int scrollScale);
    virtual ~ScrolledWindowHelper();

    /**
     * Stuff about the what the function does relative to the scheme of things.
     * A good temporary place for design-level documentation.
     *
     * @param
     * @return
     */

     // *** UNIT [UN]CONVERSION ***
     // ie. Soil => ScrolledWindow Unit
    int Soil2ScrollUnit(double soilValue);
    void Soil2ScrollUnitPoint(rootmap::DoublePt& soilPoint, wxPoint& viewPoint);
    void Soil2ScrollUnitRect(DoubleRect& soilRect, wxRect& viewRect);

    double ScrollUnit2Soil(int viewValue);
    void ScrollUnit2SoilPoint(wxPoint& viewPoint, rootmap::DoublePt& soilPoint);
    void ScrollUnit2SoilRect(wxRect& viewRect, DoubleRect& soilRect);

    // *** PIXEL [UN]CONVERSION
    int Soil2ScrollPixel(double soilValue);
    void Soil2ScrollPixelPoint(rootmap::DoublePt& soilPoint, wxPoint& panoPixelPoint);
    void Soil2ScrollPixelRect(DoubleRect& soilRect, wxRect& panoPixelRect);

    double ScrollPixel2Soil(int panoPixelValue);
    void ScrollPixel2SoilPoint(const wxPoint& panoPixelPoint, rootmap::DoublePt& soilPoint);
    void ScrollPixel2SoilRect(const wxRect& panoPixelRect, DoubleRect& soilRect);

    // *** PIXEL <=> UNIT CONVERSION
    void ScrollPixel2ScrollUnitRect(wxRect& panoPixelRect, wxRect& panoUnitRect);
    void Soil2FloatRect(DoubleRect& soilRect, DoubleRect& fRect);
    void SectSoilRect(DoubleRect& src1, DoubleRect& src2, DoubleRect& destRect);
private:

    ///
    /// member declaration
    rootmap::ViewDirection m_viewDirection;

    double m_soilScale;

    int m_scrollScale;
}; // class ScrolledWindowHelper

#endif // #ifndef ScrolledWindowHelper_H
