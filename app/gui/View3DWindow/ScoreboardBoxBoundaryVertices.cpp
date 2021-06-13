/////////////////////////////////////////////////////////////////////////////
// Name:        ScoreboardBoxBoundaryVertices.cpp
// Purpose:     Implementation of the ScoreboardBoxBoundaryVertices class
// Created:     20-06-2008 05:40:29
// Author:      RvH
// $Date: 2009-07-07 02:40:29 +0800 (Tue, 07 Jul 2009) $
// $Revision: 67 $
// Copyright:   ©2002-2008 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/View3DWindow/ScoreboardBoxBoundaryVertices.h"
#include "app/gui/ViewCommon/View3DInformation.h"
#include "app/gui/common/GuiUtility.h"

#include "simulation/scoreboard/Scoreboard.h"
#include "simulation/scoreboard/ScoreboardCoordinator.h"

#include "core/scoreboard/BoundaryArray.h"
#include "core/utility/Utility.h"

namespace rootmap
{
    RootMapLoggerDefinition(ScoreboardBoxBoundaryVertices);

    ScoreboardBoxBoundaryVertices::ScoreboardBoxBoundaryVertices
    (ScoreboardCoordinator& scoord,
        const View3DInformation& viewInfo)
        : ScoreboardVertexBuffer(scoord, viewInfo)
        , m_drawBoundaries(viewInfo.DoesBoundaries())
        , m_drawBoxes(viewInfo.DoesBoxes())
        , m_drawInnerBoxes(viewInfo.DoesBoxColours())
        , m_boxColour(*wxLIGHT_GREY)
        , m_boundaryColour(*wxBLUE)
    {
        RootMapLoggerInitialisation("rootmap.ScoreboardBoxBoundaryVertices");
    }

    ScoreboardBoxBoundaryVertices::~ScoreboardBoxBoundaryVertices()
    {
    }

    void ScoreboardBoxBoundaryVertices::DrawScoreboard(const DoubleRect& area, Scoreboard* scoreboard)
    {
        ScoreboardVertexBuffer::DrawScoreboard(area, scoreboard);

        if ((!m_drawBoxes) && (!m_drawBoundaries) && (!m_drawInnerBoxes)) { return; }

        //
        const BoundaryArray& boundaries_x = scoreboard->GetBoundaryArray(X);
        const BoundaryArray& boundaries_y = scoreboard->GetBoundaryArray(Y);
        const BoundaryArray& boundaries_z = scoreboard->GetBoundaryArray(Z);

        size_t num_boundaries_x = boundaries_x.GetNumBoundaries();
        size_t num_boundaries_y = boundaries_y.GetNumBoundaries();
        size_t num_boundaries_z = boundaries_z.GetNumBoundaries();

        GLdouble x_min = boundaries_x.GetFirst();
        GLdouble y_min = boundaries_y.GetFirst();
        GLdouble z_min = boundaries_z.GetFirst();

        GLdouble x_max = boundaries_x.GetLast();
        GLdouble y_max = boundaries_y.GetLast();
        GLdouble z_max = boundaries_z.GetLast();

        RmGLColour original_colour = SetColour(m_boxColour);

        if (m_drawBoxes)
        {
            for (size_t boundary_index_x = 1; boundary_index_x <= num_boundaries_x; ++boundary_index_x)
            {
                GLdouble x_ = boundaries_x.GetBoundary(boundary_index_x);
                //Horizontal y=y_min->y_max (top)
                Buffer_Line(DoubleCoordinate(x_, y_min, z_min), DoubleCoordinate(x_, y_max, z_min));

                //Horizontal y=y_min->y_max (bottom)
                Buffer_Line(DoubleCoordinate(x_, y_min, z_max), DoubleCoordinate(x_, y_max, z_max));

                //Horizontal z=z_min->z_max (front)
                Buffer_Line(DoubleCoordinate(x_, y_min, z_min), DoubleCoordinate(x_, y_min, z_max));

                //Horizontal z=z_min->z_max (back)
                Buffer_Line(DoubleCoordinate(x_, y_max, z_min), DoubleCoordinate(x_, y_max, z_max));
            }

            for (size_t boundary_index_y = 1; boundary_index_y <= num_boundaries_y; ++boundary_index_y)
            {
                GLdouble y_ = boundaries_y.GetBoundary(boundary_index_y);
                DoubleCoordinate A(x_min, y_, z_min); // Top Left
                DoubleCoordinate B(x_max, y_, z_min); // Top Right
                DoubleCoordinate C(x_max, y_, z_max); // Bottom Right
                DoubleCoordinate D(x_min, y_, z_max); // Bottom Left

                //Horizontal (top)
                Buffer_Line(A, B);

                //Horizontal (right)
                Buffer_Line(B, C);

                //Horizontal (bottom)
                Buffer_Line(C, D);

                //Horizontal (left)
                Buffer_Line(D, A);
            }

            for (size_t boundary_index_z = 1; boundary_index_z <= num_boundaries_z; ++boundary_index_z)
            {
                GLdouble z_ = boundaries_z.GetBoundary(boundary_index_z);
                DoubleCoordinate E(x_min, y_min, z_); // Front Left
                DoubleCoordinate F(x_min, y_max, z_); // Back  Left
                DoubleCoordinate G(x_max, y_max, z_); // Back  Right
                DoubleCoordinate H(x_max, y_min, z_); // Front Right

                Buffer_Line(E, F);
                Buffer_Line(F, G);
                Buffer_Line(G, H);
                Buffer_Line(H, E);
            }
        }


        if (m_drawBoundaries)
        {
            SetColour(m_boundaryColour);

            DoubleCoordinate A(x_min, y_min, z_min); // Left  Front Top
            DoubleCoordinate B(x_min, y_max, z_min); // Left  Back  Top
            DoubleCoordinate C(x_max, y_max, z_min); // Right Back  Top
            DoubleCoordinate D(x_max, y_min, z_min); // Right Front Top

            DoubleCoordinate E(x_min, y_min, z_max);
            DoubleCoordinate F(x_min, y_max, z_max);
            DoubleCoordinate G(x_max, y_max, z_max);
            DoubleCoordinate H(x_max, y_min, z_max);

            // around the top of the scoreboard
            Buffer_Line(A, B);
            Buffer_Line(B, C);
            Buffer_Line(C, D);
            Buffer_Line(D, A);

            // around the bottom
            Buffer_Line(E, F);
            Buffer_Line(F, G);
            Buffer_Line(G, H);
            Buffer_Line(H, E);

            // verticals
            Buffer_Line(A, E);
            Buffer_Line(B, F);
            Buffer_Line(C, G);
            Buffer_Line(D, H);
        }


        RestoreColour(original_colour);
    }

    void ScoreboardBoxBoundaryVertices::SetBoundaries(bool fDraw)
    {
        if (m_drawBoundaries != fDraw)
        {
            m_drawBoundaries = fDraw;
            SetRebuildLevel(rootmap::bbl_Redraw);
        }
    }

    void ScoreboardBoxBoundaryVertices::SetBoxes(bool fDraw)
    {
        if (m_drawBoxes != fDraw)
        {
            m_drawBoxes = fDraw;
            SetRebuildLevel(rootmap::bbl_Redraw);
        }
    }

    void ScoreboardBoxBoundaryVertices::SetInnerBoxes(bool fDraw)
    {
        if (m_drawInnerBoxes != fDraw)
        {
            m_drawInnerBoxes = fDraw;
            SetRebuildLevel(rootmap::bbl_Redraw);
        }
    }
} /* namespace rootmap */
