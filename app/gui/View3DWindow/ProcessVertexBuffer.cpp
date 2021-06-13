/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessVertexBuffer.cpp
// Purpose:     Implementation of the ProcessVertexBuffer class
// Created:     20-06-2008 01:47:29
// Author:      RvH
// $Date: 2009-07-07 02:40:29 +0800 (Tue, 07 Jul 2009) $
// $Revision: 67 $
// Copyright:   ©2002-2008 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/View3DWindow/ProcessVertexBuffer.h"
#include "app/gui/ViewCommon/View3DInformation.h"
#include "simulation/process/common/ProcessDrawing.h"
#include "simulation/scoreboard/ScoreboardCoordinator.h"
#include "wx/gdicmn.h"

namespace rootmap
{
    ProcessVertexBuffer::ProcessVertexBuffer
    (ProcessDrawing* parent,
        ScoreboardCoordinator& scoord,
        const View3DInformation& viewInfo)
        : ScoreboardVertexBuffer(scoord, viewInfo)
        , m_parent(parent)
        , m_scoreboardCoordinator(scoord)
        , m_colour_mode(viewInfo.GetRootColourMode())
    {
        m_parent->AddDrawingBuffer(this);
        SetRootRadiusMultiplier(viewInfo.GetRootRadiusMultiplier());
        SetCylindricalRoots(viewInfo.DoesCylinders());
        SetInterstitialSpheres(viewInfo.DoesSpheres());
        SetConicalTips(viewInfo.DoesCones());
        SetStacksAndSlices(viewInfo.GetStacksAndSlices());
        m_baseColour[0] = viewInfo.GetBaseRootRf();
        m_baseColour[1] = viewInfo.GetBaseRootGf();
        m_baseColour[2] = viewInfo.GetBaseRootBf();
        m_baseColour[3] = 0.0; // Alpha not supported yet for general drawing
    }

    ProcessVertexBuffer::~ProcessVertexBuffer()
    {
        if (m_parent) m_parent->RemoveDrawingBuffer(this);
    }

    ProcessDrawing* ProcessVertexBuffer::GetParent()
    {
        return m_parent;
    }

    void ProcessVertexBuffer::DrawScoreboard(const DoubleRect& area, Scoreboard* scoreboard)
    {
        if (!m_parent) return;
        ScoreboardVertexBuffer::DrawScoreboard(area, scoreboard);

        m_parent->DrawScoreboard(area, scoreboard, this);
    }

    void ProcessVertexBuffer::MovePenTo(const DoubleCoordinate& fi)
    {
        Buffer_MoveTo(fi);
    }

    void ProcessVertexBuffer::DrawLineTo(const DoubleCoordinate& fi)
    {
        Buffer_LineTo(fi);
    }

    void ProcessVertexBuffer::DrawLineTo(const DoubleCoordinate& fi, wxColour& colour)
    {
        Buffer_LineTo(fi, colour);
    }

    void ProcessVertexBuffer::DrawLine(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2)
    {
        Buffer_Line(coord1, coord2);
    }

    void ProcessVertexBuffer::DrawLine(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, wxColour& colour)
    {
        Buffer_Line(coord1, coord2, colour);
    }

    void ProcessVertexBuffer::DrawDot(DoubleCoordinate& fi)
    {
        Buffer_Dot(fi);
    }

    void ProcessVertexBuffer::DrawDot(DoubleCoordinate& fi, wxColour& colour)
    {
        Buffer_Dot(fi, colour);
    }

    void ProcessVertexBuffer::RemoveDot(const DoubleCoordinate& fi)
    {
        Unbuffer_Dot(fi);
    }

    void ProcessVertexBuffer::RemoveAllDots()
    {
        Clear_Dots();
    }

    void ProcessVertexBuffer::DrawRootSegment(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const long& order, const double& radius, const PlantElementIdentifier& elid)
    {
        GLfloat r = 0.0, g = 0.0, b = 0.0;
        RootColourUtility::setColours(m_colour_mode, order, r, g, b, m_baseColour);
        wxColour c(255.0 * r, 255.0 * g, 255.0 * b);
        Buffer_Cylinder(coord1, coord2, radius, elid, c);
    }

    void ProcessVertexBuffer::DrawRootSegment(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const double& radius, const PlantElementIdentifier& elid, wxColour& colour)
    {
        Buffer_Cylinder(coord1, coord2, radius, elid, colour);
    }

    void ProcessVertexBuffer::DrawCone(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const double& radius1, const double radius2, wxColour& colour, const ViewDirection& /* direction */, const size_t& stacksNSlices, const bool& wireframe)
    {
        Buffer_Cone(coord1, coord2, radius1, radius2, colour, stacksNSlices, wireframe);
    }

    // MSA 09.11.05 Implementing 3D representation of wetting front...
    void ProcessVertexBuffer::DrawRectangle(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection /* direction */, double place, wxColour& colour)
    {
        DoubleBox dbox;
        // Get the sides/coordinates of the box given by "bc"
        GetScoreboardCoordinator().GetBoxSoil(&dbox, stratum, bc);
        RmGLColour* c = new RmGLColour(colour); // TODO fix memory leak
        Buffer_Rectangle(DoubleCoordinate(dbox.left, dbox.front, place), DoubleCoordinate(dbox.right, dbox.back, place), *c);
    }

    // MSA 10.08.19 ...and BoundingRectangularPrism
    void ProcessVertexBuffer::DrawRectangle(const DoubleBox& dbox)
    {
        wxColour colour(128, 0, 128, 63);
        RmGLColour* c = new RmGLColour(colour); // TODO fix memory leak
        Buffer_Rectangle(DoubleCoordinate(dbox.left, dbox.front, dbox.top), DoubleCoordinate(dbox.right, dbox.back, dbox.bottom), *c);
    }

    void ProcessVertexBuffer::DrawRectangle(const DoubleBox& dbox, ViewDirection vdir, double place)
    {
        wxColour colour(128, 0, 128, 84);
        DoubleCoordinate dc1, dc2;
        switch (vdir)
        {
        case(vTop): // XY plane
            dc1 = DoubleCoordinate(dbox.left, dbox.front, place);
            dc2 = DoubleCoordinate(dbox.right, dbox.back, place);
            break;
        case(vFront): // XZ plane
            dc1 = DoubleCoordinate(dbox.left, place, dbox.top);
            dc2 = DoubleCoordinate(dbox.right, place, dbox.bottom);
            break;
        case(vSide): // YZ plane
            dc1 = DoubleCoordinate(place, dbox.front, dbox.top);
            dc2 = DoubleCoordinate(place, dbox.back, dbox.bottom);
            break;
        case(vNONE):
            //RmAssert(false,"bad arg");
            return;
        }
        RmGLColour* c = new RmGLColour(colour); // TODO fix memory leak
        Buffer_Rectangle(dc1, dc2, *c);
    }

    void ProcessVertexBuffer::RemoveRectangle(const ScoreboardStratum& stratum, BoxCoordinate* bc, ViewDirection /* direction */, double place)
    {
        DoubleBox dbox;
        // Get the sides/coordinates of the box given by "bc"
        GetScoreboardCoordinator().GetBoxSoil(&dbox, stratum, bc);
        Unbuffer_Rectangle(DoubleCoordinate(dbox.left, dbox.front, place), DoubleCoordinate(dbox.right, dbox.back, place));
    }


    void ProcessVertexBuffer::DrawRectangle(const ScoreboardStratum&, BoxCoordinate*, ViewDirection, double)
    {
    }

    void ProcessVertexBuffer::DrawRectangle(const DoubleBox&, ViewDirection)
    {
    }

    void ProcessVertexBuffer::DrawOval(const ScoreboardStratum&, BoxCoordinate*, ViewDirection, double)
    {
    }

    void ProcessVertexBuffer::DrawOval(const DoubleBox&, ViewDirection, double)
    {
    }

    void ProcessVertexBuffer::DrawOval(const DoubleBox&, ViewDirection)
    {
    }

    void ProcessVertexBuffer::DrawOval(const DoubleBox&)
    {
    }

    void ProcessVertexBuffer::PaintRectangle(const ScoreboardStratum&, BoxCoordinate*, ViewDirection, double)
    {
    }

    void ProcessVertexBuffer::PaintRectangle(const DoubleBox&, ViewDirection, double)
    {
    }

    void ProcessVertexBuffer::PaintRectangle(const DoubleBox&, ViewDirection)
    {
    }

    void ProcessVertexBuffer::PaintRectangle(const DoubleBox&)
    {
    }

    void ProcessVertexBuffer::PaintOval(const ScoreboardStratum&, BoxCoordinate*, ViewDirection, double)
    {
    }

    void ProcessVertexBuffer::PaintOval(const DoubleBox&, ViewDirection, double)
    {
    }

    void ProcessVertexBuffer::PaintOval(const DoubleBox&, ViewDirection)
    {
    }

    void ProcessVertexBuffer::PaintOval(const DoubleBox&)
    {
    }

    void ProcessVertexBuffer::DrawRectangle(const DoubleBox&, ViewDirection, double, wxColour&)
    {
    }

    void ProcessVertexBuffer::DrawRectangle(const DoubleBox&, ViewDirection, wxColour&)
    {
    }

    void ProcessVertexBuffer::DrawRectangle(const DoubleBox&, wxColour&)
    {
    }

    void ProcessVertexBuffer::DrawOval(const ScoreboardStratum&, BoxCoordinate*, ViewDirection, double, wxColour&)
    {
    }

    void ProcessVertexBuffer::DrawOval(const DoubleBox&, ViewDirection, double, wxColour&)
    {
    }

    void ProcessVertexBuffer::DrawOval(const DoubleBox&, ViewDirection, wxColour&)
    {
    }

    void ProcessVertexBuffer::DrawOval(const DoubleBox&, wxColour&)
    {
    }

    void ProcessVertexBuffer::PaintRectangle(const ScoreboardStratum&, BoxCoordinate*, ViewDirection, double, wxColour&)
    {
    }

    void ProcessVertexBuffer::PaintRectangle(const DoubleBox&, ViewDirection, double, wxColour&)
    {
    }

    void ProcessVertexBuffer::PaintRectangle(const DoubleBox&, ViewDirection, wxColour&)
    {
    }

    void ProcessVertexBuffer::PaintRectangle(const DoubleBox&, wxColour&)
    {
    }

    void ProcessVertexBuffer::PaintOval(const ScoreboardStratum&, BoxCoordinate*, ViewDirection, double, wxColour&)
    {
    }

    void ProcessVertexBuffer::PaintOval(const DoubleBox&, ViewDirection, double, wxColour&)
    {
    }

    void ProcessVertexBuffer::PaintOval(const DoubleBox&, ViewDirection, wxColour&)
    {
    }

    void ProcessVertexBuffer::PaintOval(const DoubleBox&, wxColour&)
    {
    }

    void ProcessVertexBuffer::SetDeviceContext(wxDC*)
    {
    }

    wxBitmap& ProcessVertexBuffer::GetBitmap() { return wxNullBitmap; }

    void ProcessVertexBuffer::DissociateFromDrawing()
    {
        m_parent = __nullptr;
    }
} /* namespace rootmap */
