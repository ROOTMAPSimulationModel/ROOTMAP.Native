/////////////////////////////////////////////////////////////////////////////
// Name:        ScrolledWindowHelper.cpp
// Purpose:     Implementation of the ScrolledWindowHelper class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2008-06-16 02:07:36 +0800 (Mon, 16 Jun 2008) $
// $Revision: 1 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/ViewWindow/ScrolledWindowHelper.h"

#include "core/utility/Utility.h"

// calculates the height & width as
// integer_height = Convert(soil_bottom-soil_top);
#define DIFFERENCE_ALGORITHM_1 1
// calculates the height & width as
// integer_height = Convert(soil_bottom)-Convert(soil_top);
#define DIFFERENCE_ALGORITHM_2 2
#define USE_DIFFERENCE_ALGORITHM DIFFERENCE_ALGORITHM_1

// Uncomment this if the y-axis values need to be flipped for vTop
// #define FLIP_Y_AXIS_FOR_TOP_VIEWDIRECTION
using rootmap::Utility::CSMax;
using rootmap::Utility::CSMin;

ScrolledWindowHelper::ScrolledWindowHelper
(rootmap::ViewDirection viewDir,
    double soilScale,
    int scrollScale)
    : m_viewDirection(viewDir)
    , m_soilScale(soilScale)
    , m_scrollScale(scrollScale)
{
}

ScrolledWindowHelper::~ScrolledWindowHelper()
{
}

// //////////////////////////////
// Soil2ScrollUnit
// Adjusts a value from soil space to panorama bounds units (not pixel coordinates).
// This is rather bad in that it assumes the CPanorama's m_scrollScale == m_scrollScale.
// 
// uses floor and ceil in the intermediate calculations to match the
// ScrollUnit2Soil method
// //////////////////////////////
int ScrolledWindowHelper::Soil2ScrollUnit(double soilValue)
{
    double intermediate = soilValue * m_soilScale;
    //intermediate += (m_scrollScale/2);
    intermediate /= m_scrollScale;
    if (intermediate < 0)
    {
        intermediate = floor(intermediate);
    }
    else
    {
        intermediate = ceil(intermediate);
    }
    return intermediate;
    //return ((soilValue * m_soilScale) + (m_scrollScale/2)) / m_scrollScale;
}


// //////////////////////////////
// Soil2ScrollUnitPoint
// Adjusts a point from soil space to view window bounds units (not pixel coordinates).
// //////////////////////////////
void ScrolledWindowHelper::Soil2ScrollUnitPoint(
    rootmap::DoublePt& soilPoint,
    wxPoint& viewPoint)
{
    viewPoint.x = (Soil2ScrollUnit(soilPoint.h));
    viewPoint.y = (Soil2ScrollUnit(soilPoint.v));
}


// //////////////////////////////
// Soil2ScrollUnitRect
// Adjusts a rectangle from soil space to view window bounds units (not pixel coordinates).
// //////////////////////////////
void ScrolledWindowHelper::Soil2ScrollUnitRect(
    DoubleRect& soilRect,
    wxRect& viewRect)
{
    viewRect.SetTop(Soil2ScrollUnit(soilRect.top));
    viewRect.SetLeft(Soil2ScrollUnit(soilRect.left));
#if (USE_DIFFERENCE_ALGORITHM==DIFFERENCE_ALGORITHM_1)
    viewRect.SetHeight(Soil2ScrollUnit(soilRect.right - soilRect.left));
    viewRect.SetWidth(Soil2ScrollUnit(soilRect.bottom - soilRect.top));
#elif (USE_DIFFERENCE_ALGORITHM==DIFFERENCE_ALGORITHM_2)
    viewRect.SetHeight(Soil2ScrollUnit(soilRect.right) - Soil2ScrollUnit(soilRect.left));
    viewRect.SetWidth(Soil2ScrollUnit(soilRect.bottom) - Soil2ScrollUnit(soilRect.top));
#endif // #elif (USE_DIFFERENCE_ALGORITHM==whatever)


#if defined ROOTMAP_VIEW_FROM_TOP_FIXME
    if (m_viewDirection == vTop)
    {
        long viewDummy = viewRect.GetTop();
        viewRect.Set(viewRect.bottom);
        viewRect.Set(viewDummy);
    }
#endif // #if defined ROOTMAP_VIEW_FROM_TOP_FIXME
}

// //////////////////////////////
// ScrollUnit2Soil
// Adjusts a value from Panorama units (not pixel coordinates) to soil space (double).
// This is rather bad in that it assumes the CPanorama's m_scrollScale == m_scrollScale.
//
// uses floor and ceil in the intermediate calculations to match the
// Soil2ScrollUnit method
// //////////////////////////////
double ScrolledWindowHelper::ScrollUnit2Soil(int viewValue)
{
    double intermediate = (viewValue / m_soilScale);
    if (intermediate < 0)
    {
        intermediate = floor(intermediate);
    }
    else
    {
        intermediate = ceil(intermediate);
    }
    intermediate *= m_scrollScale;

    return intermediate;

    // add m_soilScale/2 to original figure, so it is rounded to the *nearest*
    // unit, not just truncated
    //return ((viewValue+(m_soilScale/2)) / m_soilScale) * m_scrollScale;
}

// //////////////////////////////
// ScrollUnit2SoilPoint
// Adjusts a wxPoint from Panorama units (not pixel coordinates) to soil space (DoublePt).
// //////////////////////////////
void ScrolledWindowHelper::ScrollUnit2SoilPoint(
    wxPoint& viewPoint,
    rootmap::DoublePt& soilPoint)
{
    soilPoint.h = ScrollUnit2Soil(viewPoint.x);
    soilPoint.v = ScrollUnit2Soil(viewPoint.y);
}

// //////////////////////////////
// ScrollUnit2SoilRect
// Adjusts a wxRect from Panorama units (not pixel coordinates) to soil space (DoubleRect).
// //////////////////////////////
void ScrolledWindowHelper::ScrollUnit2SoilRect(
    wxRect& viewRect,
    DoubleRect& soilRect)
{
    soilRect.top = ScrollUnit2Soil(viewRect.GetTop());
    soilRect.left = ScrollUnit2Soil(viewRect.GetLeft());
    soilRect.right = ScrollUnit2Soil(viewRect.GetRight());
    soilRect.bottom = ScrollUnit2Soil(viewRect.GetBottom());

#if defined ROOTMAP_VIEW_FROM_TOP_FIXME
    if (m_viewDirection == vTop)
    {
        double soilDummy = soilRect.top;
        soilRect.top = soilRect.bottom;
        soilRect.bottom = soilDummy;
    }
#endif // #if defined ROOTMAP_VIEW_FROM_TOP_FIXME
}


// //////////////////////////////
// Soil2ScrollPixel
// Adjusts a value from soil space to panorama pixels (NOT pixel coordinates).
// It assumes the m_scrollScale is identical in both axes
// 
// //////////////////////////////
int ScrolledWindowHelper::Soil2ScrollPixel(double soilValue)
{
    return Soil2ScrollUnit(soilValue) * m_scrollScale;
}


// //////////////////////////////
// Soil2ScrollPixelPoint
// Adjusts a point from soil space to panorama pixels (NOT unit coordinates).
// //////////////////////////////
void ScrolledWindowHelper::Soil2ScrollPixelPoint(
    rootmap::DoublePt& soilPoint,
    wxPoint& panoPixelPoint)
{
    panoPixelPoint.x = (Soil2ScrollPixel(soilPoint.h));
    panoPixelPoint.y = (Soil2ScrollPixel(soilPoint.v));
}


// //////////////////////////////
// Soil2ScrollPixelRect
// Adjusts a rectangle from soil space to panorama pixels (NOT unit coordinates).
// //////////////////////////////
void ScrolledWindowHelper::Soil2ScrollPixelRect(
    DoubleRect& soilRect,
    wxRect& panoPixelRect)
{
    panoPixelRect.SetTop(Soil2ScrollPixel(soilRect.top));
    panoPixelRect.SetLeft(Soil2ScrollPixel(soilRect.left));
#if (USE_DIFFERENCE_ALGORITHM==DIFFERENCE_ALGORITHM_1)
    panoPixelRect.SetWidth(Soil2ScrollPixel(soilRect.right - soilRect.left));
    panoPixelRect.SetHeight(Soil2ScrollPixel(soilRect.bottom - soilRect.top));
#elif (USE_DIFFERENCE_ALGORITHM==DIFFERENCE_ALGORITHM_2)
    panoPixelRect.SetWidth(Soil2ScrollPixel(soilRect.right) - Soil2ScrollPixel(soilRect.left));
    panoPixelRect.SetHeight(Soil2ScrollPixel(soilRect.bottom) - Soil2ScrollPixel(soilRect.top));
#endif // #elif (USE_DIFFERENCE_ALGORITHM==whatever)

#if defined ROOTMAP_VIEW_FROM_TOP_FIXME
    if (m_viewDirection == vTop)
    {
        long panoPixelDummy = panoPixelRect.GetTop();
        panoPixelRect.Set(panoPixelRect.GetBottom());
        panoPixelRect.Set(panoPixelDummy);
    }
#endif // #if defined ROOTMAP_VIEW_FROM_TOP_FIXME
}

// //////////////////////////////
// ScrollPixel2Soil
// Adjusts a value from Panorama pixel coordinates to soil space (double).
// This is assumes the CPanorama's m_scrollScale == m_scrollScale.
// //////////////////////////////
double ScrolledWindowHelper::ScrollPixel2Soil(int panoPixelValue)
{
    return panoPixelValue / m_soilScale;
}

// //////////////////////////////
// ScrollPixel2SoilPoint
// Adjusts a wxPoint from Panorama pixel coordinates to soil space (double).
// //////////////////////////////
void ScrolledWindowHelper::ScrollPixel2SoilPoint(
    const wxPoint& panoPixelPoint,
    rootmap::DoublePt& soilPoint)
{
    soilPoint.h = ScrollPixel2Soil(panoPixelPoint.x);
    soilPoint.v = ScrollPixel2Soil(panoPixelPoint.y);
}

// //////////////////////////////
// ScrollPixel2SoilRect
// Adjusts a wxRect from Panorama pixel coordinates to soil space (double).
// //////////////////////////////
void ScrolledWindowHelper::ScrollPixel2SoilRect(
    const wxRect& panoPixelRect,
    DoubleRect& soilRect)
{
    soilRect.top = ScrollPixel2Soil(panoPixelRect.GetTop());
    soilRect.left = ScrollPixel2Soil(panoPixelRect.GetLeft());
    soilRect.right = ScrollPixel2Soil(panoPixelRect.GetRight());
    soilRect.bottom = ScrollPixel2Soil(panoPixelRect.GetBottom());

#if defined ROOTMAP_VIEW_FROM_TOP_FIXME
    if (m_viewDirection == vTop)
    {
        double soilDummy = soilRect.top;
        soilRect.top = soilRect.bottom;
        soilRect.bottom = soilDummy;
    }
#endif // #if defined ROOTMAP_VIEW_FROM_TOP_FIXME
}


// //////////////////////////////
// ScrollPixel2SoilRect
// Adjusts a wxRect from Panorama pixel coordinates to soil space (double).
// //////////////////////////////
void ScrolledWindowHelper::ScrollPixel2ScrollUnitRect(
    wxRect& panoPixelRect,
    wxRect& panoUnitRect)
{
    panoUnitRect.SetTop(panoPixelRect.GetTop() / m_scrollScale);
    panoUnitRect.SetLeft(panoPixelRect.GetLeft() / m_scrollScale);

    panoUnitRect.SetWidth((panoPixelRect.GetRight() - panoPixelRect.GetLeft()) / m_scrollScale);
    panoUnitRect.SetHeight((panoPixelRect.GetBottom() - panoPixelRect.GetTop()) / m_scrollScale);
}

// //////////////////////////////
// 
// 
// //////////////////////////////
void ScrolledWindowHelper::Soil2FloatRect
(DoubleRect& soilRect,
    DoubleRect& fRect)
{
    soilRect.top = fRect.top;
    soilRect.left = fRect.left;
    soilRect.right = fRect.right;
    soilRect.bottom = fRect.bottom;

#if defined ROOTMAP_VIEW_FROM_TOP_FIXME
    if (m_viewDirection == vTop)
    {
        double soilDummy = soilRect.top;
        soilRect.top = soilRect.bottom;
        soilRect.bottom = soilDummy;
    }
#endif // #if defined ROOTMAP_VIEW_FROM_TOP_FIXME
}


// //////////////////////////////
// SectSoilRect
// Like the DoubleCoordinates function, except
// a) it doesn't return a boolean to say whether the resultant DoubleRect is Empty or not.
// b) if viewing from the top, we need to CSMax the bottom and CSMin the top, since
//     our soilRects are upside down.
// //////////////////////////////
void ScrolledWindowHelper::SectSoilRect(
    DoubleRect& src1,
    DoubleRect& src2,
    DoubleRect& destRect)
{
    destRect.left = CSMax(src1.left, src2.left);
    destRect.right = CSMin(src1.right, src2.right);

#if defined ROOTMAP_VIEW_FROM_TOP_FIXME
    if (m_viewDirection == vTop)
    {
        destRect.top = CSMin(src1.top, src2.top);
        destRect.bottom = CSMax(src1.bottom, src2.bottom);
    }
    else
#endif // #if defined ROOTMAP_VIEW_FROM_TOP_FIXME
    {
        destRect.top = CSMax(src1.top, src2.top);
        destRect.bottom = CSMin(src1.bottom, src2.bottom);
    }
}
