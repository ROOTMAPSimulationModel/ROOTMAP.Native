/////////////////////////////////////////////////////////////////////////////
// Name:        VertexBuffer.cpp
// Purpose:     Implementation of the VertexBuffer class
// Created:     20-03-2008 00:38:09
// Author:      RvH
// $Date: 2009-07-07 02:40:29 +0800 (Tue, 07 Jul 2009) $
// $Revision: 67 $
// Copyright:   ©2002-2008 University of Tasmania
/////////////////////////////////////////////////////////////////////////////

#include "app/gui/View3DWindow/VertexBuffer.h"
#include "core/utility/Vec3d.h"
#include "app/gui/View3DWindow/RootCylinder.h"
#include <algorithm>
#include "gl/glu.h"
#include <intrin.h>


namespace rootmap
{
    const static double RAD_TO_DEG = (180.0 / 3.141592653589793238);

    ColourSet VertexBuffer::m_globalColoursInUse = ColourSet();

    VertexBuffer::VertexBuffer()
        : m_firstLineTo(false)
        , m_rebuildLevel(bbl_Complete)
        , m_prepared(false)
        , m_root_radius_multiplier(1.0)
        , m_cylindricalRoots(true)
        , m_interstitialSpheres(false)
        , m_conicalRootTips(false)
        , m_stacksAndSlices(4)
        , m_antialias(false)
        , m_zMultiplier(1.0)
        , m_colouration(RM_COLOUR_STATIC)
        , m_quadric(gluNewQuadric())
    {
        RootMapLoggerInitialisation("rootmap.VertexBuffer");
    }

    VertexBuffer::~VertexBuffer()
    {
        gluDeleteQuadric(m_quadric);
        for (VertexArrayColourMap::iterator iter = m_linesByColour.begin(); iter != m_linesByColour.end(); ++iter)
        {
            delete iter->second; // Delete the vector of vertices off the heap
        }
    }


    void VertexBuffer::Prepare()
    {
        if (false == m_prepared)
        {
            for (VertexArrayColourMap::iterator iter = m_linesByColour.begin(); iter != m_linesByColour.end(); ++iter)
            {
                iter->second->clear(); // Empty the vector of vertices
            }
            m_lineStripList.clear();
            m_lineStripListColours.clear();
            m_cylinderList.clear();
            m_cones.clear();
            m_rectangleArray.clear();
            m_rectangleColourArray.clear();

            // MSA 09.12.03 This code preallocates some space for future storage.
            // However, it also causes a lot more memory to be leaked.
            // Commented out for now, 'cause I'm not interested in 3D buffering speed at the moment.
            // The question is:
            // WHY does this leak??? I'm using STL containers to store structs and classes with
            // well-defined default constructors and destructors, structs and classes with
            // no heap usage themselves.
            // Why can the runtime not deal with this????
            m_dotArray.reserve(ESTIMATED_DOT_MAX);
            m_dotColours.reserve(ESTIMATED_COLOUR_MAX);
            m_rectangleArray.reserve(2 * ESTIMATED_RECTANGLE_MAX);
            m_rectangleColourArray.reserve(ESTIMATED_COLOUR_MAX);
            m_prepared = true;
        }
    }

    // This is a very kludgy sorting predicate.
    // Should work OK for BoundingCylinders aligned with the Z-axis, but no guarantees otherwise...
    bool FurtherFromCamera(const GLConeData& lhs, const GLConeData& rhs)
    {
        double lx, ly, lz, rx, ry, rz;

        GLdouble model_view[16];
        glGetDoublev(GL_MODELVIEW_MATRIX, model_view);

        GLdouble projection[16];
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        gluProject(lhs.origin.x, lhs.origin.y, lhs.origin.z, model_view, projection, viewport, &lx, &ly, &lz);
        gluProject(rhs.origin.x, rhs.origin.y, rhs.origin.z, model_view, projection, viewport, &rx, &ry, &rz);

        return lz > rz;
    }


    void VertexBuffer::DrawScene()
    {
        if (m_rebuildLevel >= bbl_Redraw)
        {
            Prepare();
            DoubleRect r(0.0, 0.0, 0.0, 0.0);
            DrawContents(r);

            m_rebuildLevel = static_cast<BufferBuildLevel>(bbl_Redraw - 1);
        }

        glDisable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        const wxDateTime start = wxDateTime::UNow();
        // Draw lines
        if (!m_linesByColour.empty())
        {
            glBegin(GL_LINES);
            for (VertexArrayColourMap::const_iterator iter = m_linesByColour.begin(); iter != m_linesByColour.end(); ++iter)
            {
                const VertexArray* vaptr = iter->second;
                glColor3fv(iter->first->values);
                const size_t vertexCount = vaptr->size();
                RmAssert(vertexCount % 2 == 0, "Error: Array of line segments has a stray unpaired vertex");
                for (size_t idx = 0; idx < vertexCount; ++idx)
                {
                    glVertex3dv(&(vaptr->at(idx).x));
                    glVertex3dv(&(vaptr->at(++idx).x));
                }
            }
            glEnd();
        }

        // Draw particles
        if (!m_dotArray.empty())
        {
            glBegin(GL_POINTS);
            const size_t vertexCount = m_dotArray.size();
            size_t currentColourIndex = 0;
            size_t dotsInThisColourRemaining = 0;

            for (size_t i = 0; i < vertexCount; ++i, --dotsInThisColourRemaining)
            {
                if (dotsInThisColourRemaining < 1)
                {
                    // Set the colour, and determine how many lines to draw in this colour before changing it
                    glColor3fv(m_dotColours[currentColourIndex].first->values);
                    dotsInThisColourRemaining = m_dotColours[currentColourIndex].second;
                    ++currentColourIndex;
                }
                const DoubleCoordinate* d = m_dotArray[i];
                glVertex3d(d->x, d->y, m_zMultiplier * d->z);
            }
            glEnd();
        }

        // Draw line strips
        if (!m_lineStripListColours.empty())
        {
            // MSA 11.04.05 TODO optimise line strips. Low priority.
            ColourList::iterator lsc_iter(m_lineStripListColours.begin());
            VertexArrayList::const_iterator ls_iter(m_lineStripList.begin());
            const RmGLColour* prev_c = *lsc_iter;

            for (; ls_iter != m_lineStripList.end() && lsc_iter != m_lineStripListColours.end();
                ++ls_iter, ++lsc_iter)
            {
                const RmGLColour* c = *lsc_iter;
                if (c != prev_c)
                {
                    glColor3fv(c->values);
                }
                prev_c = c;
                const VertexArray& ls_array = *ls_iter;

                glBegin(GL_LINE_STRIP);
                for (VertexArray::const_iterator ls_array_iter(ls_array.begin());
                    ls_array_iter != ls_array.end(); ++ls_array_iter)
                {
                    const RmGLVertex& v = *ls_array_iter;
                    glVertex3dv(&(v.x));
                }
                glEnd();
            }
        }

        // Draw roots
        if (!m_cylinderList.empty())
        {
            if (m_antialias)
            {
                // Antialiasing for nice-looking cylinders
                // MSA TODO actually make this work
                glEnable(GL_POLYGON_SMOOTH);
                glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
            }

            // Now draw the root segments
            glColor4fv(m_defaultColour.values);
            RootCylinder::SetCurrentRGB(m_defaultColour.values[0], m_defaultColour.values[1], m_defaultColour.values[2]);
            const RootCylinderList::const_iterator end = m_cylinderList.end();
            for (RootCylinderList::const_iterator rc_iter = m_cylinderList.begin();
                rc_iter != end; ++rc_iter)
            {
                glPushMatrix();
                RootCylinder rc = (*rc_iter);
                // Overdraw automatically comes on if Stacks'n'Slices > 16
                // Is this an acceptable way to do this, as it's a function of cylinder quality?
                if (m_stacksAndSlices > 16)
                {
                    gluQuadricDrawStyle(m_quadric, GLU_SILHOUETTE);
                    rc.Draw(m_quadric, m_stacksAndSlices); // RootCylinder::Draw() applies translation/rotation and colour change, then draws
                    if (m_interstitialSpheres) gluSphere(m_quadric, rc.radius, m_stacksAndSlices, m_stacksAndSlices);
                    // Lighten colour very slightly for underlying cylinder
                    rc.blue += 0.05;
                    rc.red += 0.05;
                    rc.green += 0.05;
                    // Bypass the translation/rotation in root cylinder to redraw in the same place
                    rc.must_move = false;
                    gluQuadricDrawStyle(m_quadric, GLU_FILL);
                }
                rc.Draw(m_quadric, m_stacksAndSlices);
                if (m_interstitialSpheres) gluSphere(m_quadric, rc.radius, m_stacksAndSlices, m_stacksAndSlices);
                glPopMatrix();
            }

            if (m_antialias)
            {
                // Disable antialiasing, as it's not necessary for dots, scoreboard box lines, etc
                glDisable(GL_POLYGON_SMOOTH);
            }
        }

        // Draw rectangles
        if (!m_rectangleArray.empty())
        {
            const size_t recmax = m_rectangleArray.size();

            size_t currentColourIndex = 0;
            size_t rectanglesInThisColourRemaining = 0;

            const RmGLColour* c = __nullptr;

            for (size_t i = 0; i < recmax; i += 2, --rectanglesInThisColourRemaining)
            {
                if (rectanglesInThisColourRemaining < 1)
                {
                    // Get the current colour, and determine how many rectangles to draw in this colour before changing it
                    c = m_rectangleColourArray[currentColourIndex].first;
                    rectanglesInThisColourRemaining = m_rectangleColourArray[currentColourIndex].second;
                    ++currentColourIndex;
                }

                glPushMatrix();

                const RmGLVertex& v1 = m_rectangleArray[i];
                const RmGLVertex& v2 = m_rectangleArray[i + 1];
                double x1 = 0, y1 = 0, x2 = 0, y2 = 0;
                // Determine which plane the rectangle is in
                if (v1.z == v2.z)
                {
                    // Apply transformation here as necessary to offset to correct z-value
                    glTranslated(0, 0, v1.z);
                    x1 = v1.x;
                    y1 = v1.y;
                    x2 = v2.x;
                    y2 = v2.y;
                }
                else if (v1.y == v2.y)
                {
                    // Apply transformation & rotation here as necessary to offset to correct y-value
                    glTranslated(v1.x, v1.y, v1.z); // Move to start point of rectangle
                    glRotated(90, 1, 0, 0); // Rotate to transform (x,z) to (x,y)
                    x2 = v2.x - v1.x;
                    y2 = v2.z - v1.z;
                }
                else if (v1.x == v2.x)
                {
                    // Apply transformation & rotation here as necessary to offset to correct x-value
                    glTranslated(v1.x, v1.y, v1.z); // Move to start point of rectangle
                    glRotated(90, 1, 0, 0); // Rotate to transform (y,z) to (x,y)
                    glRotated(90, 0, 1, 0); // Rotate to transform (y,z) to (x,y)
                    x2 = v2.y - v1.y;
                    y2 = v2.z - v1.z;
                }
                // Rectangles use alpha value (e.g. for translucent wetting front)
                glColor4fv(c->values);
                // Draw the rectangle
                glRectd(x1, y1, x2, y2);
                // Draw again as an opaque wireframe
                glColor4f(c->values[0], c->values[1], c->values[2], 1.0);
                glBegin(GL_LINE_STRIP);
                glVertex2d(x1, y1);
                glVertex2d(x1, y2);
                glVertex2d(x2, y2);
                glVertex2d(x2, y1);
                glEnd();
                glPopMatrix();
                //glClear(GL_DEPTH_BUFFER_BIT);
            }
        }

        // Draw cones
        if (!m_cones.empty())
        {
            if (m_antialias)
            {
                // Antialiasing for nice-looking cones
                // MSA TODO actually make this work
                glEnable(GL_POLYGON_SMOOTH);
                glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
            }

            // Sort the vector of cones so that the furthest from the camera is first and the closest to the camera is last.
            std::sort(m_cones.begin(), m_cones.end(), FurtherFromCamera);

            const ConeArray::const_iterator end = m_cones.end();
            for (ConeArray::const_iterator iter = m_cones.begin();
                iter != end; ++iter)
            {
                // Push A
                glPushMatrix();

                const GLConeData& glcd = *iter;
                const RmGLColour* c = glcd.colour;
                glColor4fv(c->values); // Cones use alpha value (e.g. for translucent pot)

                const RmGLVertex& v1 = glcd.origin;

                // Apply translation and rotation
                glTranslated(v1.x, v1.y, v1.z);
                glRotated(glcd.angle, glcd.rx, glcd.ry, glcd.rz);
                if (glcd.stacksNSlices == 4) // We must rotate a rectangular-prism-cylinder around the Z-axis
                {
                    glRotated(45, 0, 0, 1);
                }

                if (!glcd.wireframe)
                {
                    // Push B
                    glPushMatrix();
                    // Draw first as a translucent solid
                    gluQuadricDrawStyle(m_quadric, GLU_FILL);
                    gluCylinder(m_quadric, glcd.baseradius, glcd.topradius, glcd.height, glcd.stacksNSlices, glcd.stacksNSlices);

                    // Pop B
                    glPopMatrix();
                    // Set colour to opaque iff we need to overdraw a wireframe onto the translucent solid
                    glColor4f(c->values[0], c->values[1], c->values[2], 1.0);
                }

                // Draw (possibly again) as a wireframe
                gluQuadricDrawStyle(m_quadric, GLU_SILHOUETTE);
                gluCylinder(m_quadric, glcd.baseradius, glcd.topradius, glcd.height, glcd.stacksNSlices, glcd.stacksNSlices);
                // Pop A
                glPopMatrix();
                //glClear(GL_DEPTH_BUFFER_BIT);
            }
            if (m_antialias)
            {
                // Disable antialiasing
                glDisable(GL_POLYGON_SMOOTH);
            }
        }

        m_rebuildLevel = static_cast<BufferBuildLevel>(bbl_Reslap - 1);
    }

    void VertexBuffer::Buffer_MoveTo(const DoubleCoordinate& dc)
    {
        // Assume this is the start of a linestrip
        m_currentVertex = dc;
        m_currentVertex.z *= m_zMultiplier;

        VertexArray new_line_strip;
        new_line_strip.push_back(m_currentVertex);

        m_lineStripList.push_front(new_line_strip);

        m_firstLineTo = true;
    }

    void VertexBuffer::Buffer_LineTo(const DoubleCoordinate& dc, wxColour& colour)
    {
        //add to the end of the current LineStrip array, which is conveniently
        //located at the front of the list
        m_currentVertex = dc;
        m_currentVertex.z *= m_zMultiplier;

        (*(m_lineStripList.begin())).push_back(m_currentVertex);

        RmGLColour* c = GetCanonicalColour(colour);
        if (true == m_firstLineTo)
        {
            m_lineStripListColours.push_back(c);
            m_firstLineTo = false;
        }
    }

    void VertexBuffer::Buffer_LineTo(const DoubleCoordinate& dc, RmGLColour& colour)
    {
        //add to the end of the current LineStrip array, which is conveniently
        //located at the front of the list
        m_currentVertex = dc;
        m_currentVertex.z *= m_zMultiplier;

        (*(m_lineStripList.begin())).push_back(m_currentVertex);

        RmGLColour* c = GetCanonicalColour(&colour);
        if (true == m_firstLineTo)
        {
            m_lineStripListColours.push_back(c);
            m_firstLineTo = false;
        }
    }

    void VertexBuffer::Buffer_Line(const DoubleCoordinate& dcoord1, const DoubleCoordinate& dcoord2, RmGLColour& rmcolour)
    {
        RmGLColour* c = GetCanonicalColour(&rmcolour);
        VertexArrayColourMap::iterator iter = m_linesByColour.find(c);
        if (iter == m_linesByColour.end())
        {
            m_linesByColour[c] = new VertexArray();
            m_linesByColour[c]->reserve(ESTIMATED_LINES_OF_EACH_COLOUR_MAX);
            iter = m_linesByColour.find(c);
        }
        m_currentVertex = dcoord1;
        m_currentVertex.z *= m_zMultiplier;
        iter->second->push_back(m_currentVertex);

        m_currentVertex = dcoord2;
        m_currentVertex.z *= m_zMultiplier;
        iter->second->push_back(m_currentVertex);
    }

    void VertexBuffer::Buffer_Dot(DoubleCoordinate& fc, wxColour& colour)
    {
        RmGLColour* c = GetCanonicalColour(colour);
        if (!m_dotColours.empty() && m_dotColours.back().first == c)
        {
            m_dotColours.back().second++;
        }
        else
        {
            m_dotColours.push_back(std::pair<RmGLColour*, size_t>(c, 1));
        }

        m_dotArray.push_back(&fc);
    }

    void VertexBuffer::Buffer_Dot(DoubleCoordinate& fc, RmGLColour& colour)
    {
        RmGLColour* c = GetCanonicalColour(&colour);
        if (!m_dotColours.empty() && m_dotColours.back().first == c)
        {
            m_dotColours.back().second++;
        }
        else
        {
            m_dotColours.push_back(std::pair<RmGLColour*, size_t>(c, 1));
        }

        m_dotArray.push_back(&fc);
    }

    void VertexBuffer::Buffer_Cylinder(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const double& radius, const PlantElementIdentifier& elid, RmGLColour& rmcolour)
    {
        if (!m_cylindricalRoots)
        {
            Buffer_Line(coord1, coord2, rmcolour);
        }
        else
        {
            RootCylinder rc = RootCylinder(coord1.x, coord1.y, coord1.z * m_zMultiplier,
                coord2.x, coord2.y, coord2.z * m_zMultiplier,
                radius * m_root_radius_multiplier,
                rmcolour.values[0], rmcolour.values[1], rmcolour.values[2],
                elid);
            if (m_conicalRootTips)
            {
                rc.rtop = 0.0; // Set new (root tip) RootCylinder as cone

                // Now reset the previous tip of this branch to a standard RootCylinder.
                RootCylinderPredicate rcp(rc);
                RootCylinderList::iterator prevTipIter = std::find_if(m_cylinderList.begin(), m_cylinderList.end(), rcp); // find the old root tip
                if (prevTipIter != m_cylinderList.end())
                {
                    prevTipIter->rtop = prevTipIter->rbase;
                }
            }
            m_cylinderList.push_back(rc);
        }
    }

    void VertexBuffer::Buffer_Cone(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const double& radius1, const double radius2, wxColour& colour, const size_t& stacksNSlices, const bool& wireframe)
    {
        // Credit for vector maths code goes to 
        // http://home.neo.rr.com/jparris/OpenGL%20-%20draw%20cylinder%20between%202%20pts.htm
        Vec3d axial_vec = Vec3d(coord2.x - coord1.x, coord2.y - coord1.y, (coord2.z - coord1.z) * m_zMultiplier);

        const double vx = axial_vec.x;
        const double vy = axial_vec.y;
        const double vz = axial_vec.z;
        const double height = axial_vec.Length();
        double rot_angle = RAD_TO_DEG * acos(vz / height);
        if (vz <= 0.0) rot_angle = -rot_angle;
        const double rx = -vy * vz;
        const double ry = +vx * vz;

        m_currentVertex = coord1;
        m_currentVertex.z *= m_zMultiplier;

        m_cones.push_back(GLConeData(m_currentVertex, rot_angle, rx, ry, 0, radius1, radius2, height, GetCanonicalColour(colour), stacksNSlices, wireframe));
    }

    void VertexBuffer::Buffer_Rectangle(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, RmGLColour& rmcolour)
    {
        RmGLColour* c = GetCanonicalColour(&rmcolour);
        if (!m_rectangleColourArray.empty() && m_rectangleColourArray.back().first == c)
        {
            m_rectangleColourArray.back().second++;
        }
        else
        {
            m_rectangleColourArray.push_back(std::pair<RmGLColour*, size_t>(c, 1));
        }
        m_currentVertex = coord1;
        m_currentVertex.z *= m_zMultiplier;
        m_rectangleArray.push_back(m_currentVertex); // (x,y) Lower left
        m_currentVertex = coord2;
        m_currentVertex.z *= m_zMultiplier;
        m_rectangleArray.push_back(m_currentVertex); // (x,y) Upper right
    }

    void VertexBuffer::Unbuffer_Rectangle(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2)
    {
        // In theory, there will be far fewer rectangles (wetting front signifiers) than dots (nitrate packets),
        // so a straightforward iteration of the vector should be OK...
        m_currentVertex = coord1;
        m_currentVertex.z *= m_zMultiplier;

        const VertexArray::iterator end = m_rectangleArray.end();
        for (VertexArray::iterator iter = m_rectangleArray.begin(); iter != end; ++iter)
        {
            if (*iter == m_currentVertex)
            {
                RmGLVertex second = coord2;
                second.z *= m_zMultiplier;
                if (*(iter + 1) == second)
                {
                    const size_t pos = distance(m_rectangleArray.begin(), iter);
                    size_t counter = 0;
                    size_t i = 0;
                    for (i = 0; counter < pos; ++i)
                    {
                        counter += m_rectangleColourArray[i].second;
                    }
                    if (m_rectangleColourArray[i].second == 0)
                    {
                        m_rectangleColourArray.erase(m_rectangleColourArray.begin() + i);
                    }
                    else
                    {
                        (m_rectangleColourArray[i].second)--;
                    }

                    m_rectangleArray.erase(iter, iter + 2); // Remember range is [,)

                    return;
                }
            }
        }
    }

    RmGLColour VertexBuffer::SetColour(const wxColour& colour)
    {
        // copy construct a backup
        RmGLColour tmp_colour;
        glGetFloatv(GL_CURRENT_COLOR, tmp_colour.values);

        // set current
        glColor3f(static_cast<GLfloat>(colour.Red()) / 255.0
            , static_cast<GLfloat>(colour.Green()) / 255.0
            , static_cast<GLfloat>(colour.Blue()) / 255.0);
        m_defaultColour = colour;

        return (tmp_colour);
    }

    RmGLColour VertexBuffer::SetColour(const RmGLColour& colour)
    {
        // copy construct a backup
        RmGLColour save_colour;
        glGetFloatv(GL_CURRENT_COLOR, save_colour.values);
        // set current
        glColor3f(colour.values[0], colour.values[1], colour.values[2]);
        m_defaultColour = colour;

        return (save_colour);
    }
} /* namespace rootmap */
