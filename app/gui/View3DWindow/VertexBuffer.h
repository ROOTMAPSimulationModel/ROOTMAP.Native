#ifndef VertexBuffer_H
#define VertexBuffer_H
/////////////////////////////////////////////////////////////////////////////
// Name:        VertexBuffer.h
// Purpose:     Declaration of the VertexBuffer class
// Created:     20-03-2008 00:38:09
// Author:      RvH
// $Date: 2009-07-07 02:40:29 +0800 (Tue, 07 Jul 2009) $
// $Revision: 67 $
// Copyright:   ©2002-2008 University of Tasmania
/////////////////////////////////////////////////////////////////////////////

#include "app/gui/View3DWindow/RmGLVertex.h"
#include "app/gui/View3DWindow/RootCylinder.h"
#include "app/gui/ViewCommon/Types.h"
#include "app/gui/ViewCommon/RootColourationModeUtility.h"
#include "core/log/Logger.h"
#include "core/common/RmAssert.h"
#include "core/utility/Utility.h"
#include "wx/colour.h"
#include <vector>
#include <list>
#include <cstdio>

namespace rootmap
{
    class Scoreboard;
    class ScoreboardStratum;

    struct RmGLColour
    {
        // allocate 4, on account of possible alpha storage with glGetFloat
        GLfloat values[4];

        RmGLColour();

        RmGLColour(const float r, const float g, const float b, const float a)
        {
            values[0] = r;
            values[1] = g;
            values[2] = b;
            values[3] = a;
        }

        RmGLColour(const RmGLColour& orig);
        RmGLColour(const wxColour& orig);

        ~RmGLColour()
        {
        }

        RmGLColour& operator=(const wxColour& orig);
        RmGLColour& operator=(const RmGLColour& rhs);

        bool operator==(const RmGLColour& other)
        {
            return values[0] == other.values[0]
                && values[1] == other.values[1]
                && values[2] == other.values[2]
                && values[3] == other.values[3];
        }

        bool operator!=(const RmGLColour& other) const
        {
            return values[0] != other.values[0]
                || values[1] != other.values[1]
                || values[2] != other.values[2]
                || values[3] != other.values[3];
        }

        // MSA 11.04.01 Have to implement comparison operators so this struct can be used to key a map.
        // They are internally consistent, but rather arbitrary.
        bool operator<(const RmGLColour& other) const
        {
            if (values[0] == other.values[0])
            {
                if (values[1] == other.values[1])
                {
                    if (values[2] == other.values[2])
                    {
                        return values[3] < other.values[3];
                    }
                    return values[2] < other.values[2];
                }
                return values[1] < other.values[1];
            }
            return values[0] < other.values[0];
        }

        bool operator>(const RmGLColour& other) const
        {
            if (values[0] == other.values[0])
            {
                if (values[1] == other.values[1])
                {
                    if (values[2] == other.values[2])
                    {
                        return values[3] > other.values[3];
                    }
                    return values[2] > other.values[2];
                }
                return values[1] > other.values[1];
            }
            return values[0] > other.values[0];
        }
    };

    struct GLConeData
    {
        RmGLVertex origin;
        double angle, rx, ry, rz;
        double baseradius, topradius, height;
        const RmGLColour* colour;
        GLint stacksNSlices;
        bool wireframe;

        GLConeData(const RmGLVertex& vert, const double ang,
            const double xx, const double yy, const double zz,
            const double radb, const double radt, const double h, const RmGLColour* col, const size_t& sNs, const bool& wf)
            : origin(vert), angle(ang), rx(xx), ry(yy), rz(zz)
            , baseradius(radb), topradius(radt), height(h), colour(col), stacksNSlices(sNs), wireframe(wf)
        {
        }
    };

    typedef std::set<RmGLColour*> ColourSet;

    class VertexBuffer
    {
    public:
        virtual ~VertexBuffer();


        /**
         */
        virtual void DrawScene();

        /**
         */
        virtual void DrawContents(const DoubleRect& area) = 0;

    protected:
        VertexBuffer();

        void Buffer_MoveTo(const DoubleCoordinate& dc);

        // LineTo with prevailing and specified colour
        void Buffer_LineTo(const DoubleCoordinate& dc);
        void Buffer_LineTo(const DoubleCoordinate& dc, wxColour& colour);
        void Buffer_LineTo(const DoubleCoordinate& dc, RmGLColour& colour);

        // Line with prevailing and specified colour
        void Buffer_Line(const DoubleCoordinate& dcoord1, const DoubleCoordinate& dcoord2);
        void Buffer_Line(const DoubleCoordinate& dcoord1, const DoubleCoordinate& dcoord2, wxColour& colour);
        void Buffer_Line(const DoubleCoordinate& dcoord1, const DoubleCoordinate& dcoord2, RmGLColour& rmcolour);

        // Dots with prevailing and specified colour
        void Buffer_Dot(DoubleCoordinate& fc);
        void Buffer_Dot(DoubleCoordinate& fc, wxColour& colour);
        void Buffer_Dot(DoubleCoordinate& fc, RmGLColour& colour);

        void Unbuffer_Dot(const DoubleCoordinate& fc);
        void Clear_Dots();

        // Cylinders with calculated or directly specified colour
        void Buffer_Cylinder(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const double& radius, const PlantElementIdentifier& elid);
        void Buffer_Cylinder(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const double& radius, const PlantElementIdentifier& elid, wxColour& colour);
        void Buffer_Cylinder(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const double& radius, const PlantElementIdentifier& elid, RmGLColour& rmcolour);

        // Cones with directly specified colour
        void Buffer_Cone(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const double& radius1, const double radius2, wxColour& colour, const size_t& stacksNSlices, const bool& wireframe);

        // 2D rectangles
        void Buffer_Rectangle(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, RmGLColour& rmcolour);
        void Unbuffer_Rectangle(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2);

        /**
         * Sets the colour in a way
           * MSA 09.12.01 Changed to return the previous colour by value.
           * RmGLColours aren't big and this saves us having to follow another free store pointer.
           * (RestoreColour no longer takes a RmGLColour* as a parameter and deletes it.)
         */
        RmGLColour SetColour(const wxColour& colour);
        RmGLColour SetColour(const RmGLColour& colour);
        void RestoreColour(const RmGLColour& tmpColour);

        /**
         * Sets the BufferRebuildLevel.
         * Will only have an effect if the given level is greater than the current.
         */
        void SetRebuildLevel(BufferBuildLevel bbl);

        /**
         * Retrieve the BufferBuildLevel
         */
        BufferBuildLevel GetBuildLevel() const;

        void Prepare();
        void ForceNextPrepare();

        /**
         * When drawing into a scoreboard that is below ground, currently need to
         * compensate by flipping the z axis.
         */
        void SetZMultiplier(double z_multiplier);

        /**
         *    Sets the multiplier to be applied to all root radii.
         */
        void SetRootRadiusMultiplier(const double multiplier);

        /**
         * Set cylindrical roots on or off
         */
        void SetCylindricalRoots(bool flag);

        /**
         * Set interstitial spheres on or off
         * If cylindrical roots are switched off, this will be ignored
         */
        void SetInterstitialSpheres(bool flag);

        /**
         * Set conical root tips on or off
         * If cylindrical roots are switched off, this will be ignored
         */
        void SetConicalTips(bool flag);

        /**
         * Set number of stacks and slices used by the OpenGL quadric
         * If cylindrical roots are switched off, this will be ignored
         */
        void SetStacksAndSlices(size_t sz);

    private:

        // Colour-handling helper functions.
        static RmGLColour* GetCanonicalColour(RmGLColour* colour);
        RmGLColour* GetCanonicalColour(wxColour& colour) const;

        /**
         */
        RmGLVertex m_currentVertex;

        /** type definition for an array of Vertices
         */
        typedef std::vector<RmGLVertex> VertexArray;

        /** type definition for a list of VertexArrays
         */
        typedef std::list<VertexArray> VertexArrayList;

        /**
         *    MSA 11.05.03 Map of VertexArray pointers, keyed by colour.
         *    Use for coloured lines.
         */
        typedef std::map<RmGLColour*, VertexArray*> VertexArrayColourMap;

        /** type definition for an array of Colour pointers
           *    MSA 11.04.11 Implementing a basic compression optimisation in this container:
           *    This typedef now defines an array of pairs:
           *  a Colour pointer and an unsigned integer specifying how many OpenGL primitives
           *  should be drawn in this colour.
         */
        typedef std::vector<std::pair<RmGLColour *, size_t>> ColourArray;

        /** type definition for a list of ColourArrays
         */
        typedef std::list<ColourArray> ColourArrayList;

        /** type definition for a list of Colours
         */
        typedef std::list<RmGLColour *> ColourList;

        /** type definition for a list of root cylinders
         */
        typedef std::list<RootCylinder> RootCylinderList;

        /** type definition for a vector of [data for] OpenGL cones
         */
        typedef std::vector<GLConeData> ConeArray;

        typedef std::vector<DoubleCoordinate *> DoubleCoordinatePtrArray;

        /** MSA 11.04.04 Changing dot handling to use the same method as line handling, but with pointers to non-local coordinates.
         * This is valid because the coordinates themselves are members of Nitrate,
         * and don't go out of scope until they are erased (and no longer need to be drawn).
         */
        ColourArray m_dotColours;
        DoubleCoordinatePtrArray m_dotArray;

        /** array of vertices to be drawn as lines
         */
         //VertexArray m_linesArray;

         /** Map of arrays of vertices to be drawn as lines
         */
        VertexArrayColourMap m_linesByColour;

        /** array of colours for the lines
         */
         //ColourArray m_linesArrayColours;

         /**
          * list of VertexArrays (a.k.a. Line Strips).
          * The front list is assumed to be the current destination of
          * LineTo commands
          */
        VertexArrayList m_lineStripList;

        /**
         * list of colours for the line strips. Note that we currently support
         * one colour per line strip.
         */
        ColourList m_lineStripListColours;

        /**
         * list of root cylinders
         */
        RootCylinderList m_cylinderList;

        /**
         *    Vector of rectangles
         */
        VertexArray m_rectangleArray;

        /**
         *    Colours for the rectangles
         */

        ColourArray m_rectangleColourArray;

        /**
         *  Data defining general cones
         */
        ConeArray m_cones;

        /**
         * is this the first LineTo in a line strip ?
         */
        bool m_firstLineTo;

        /**
         * black
         */
        RmGLColour m_defaultColour;

        /**
         * RGB values specified in Windows.XML
         */
        RmGLColour m_baseColour;

        /**
         */
        BufferBuildLevel m_rebuildLevel;

        /**
         */
        bool m_prepared;

        /**
         *    The multiplier to be applied to all root radii. Often roots are too fine to be clearly seen.
         *  By applying this multiplier, we exaggerate their thickness while keeping relative differences intact.
         */
        double m_root_radius_multiplier;

        /**
         * Draw roots using OpenGL cylinders? Default=true
         */
        bool m_cylindricalRoots;

        /**
         * Draw root tips as cones instead of cylinders? Default=false
         */
        bool m_conicalRootTips;

        /**
         * Draw spheres between root segments to smooth out joins? Default=false
         */
        bool m_interstitialSpheres;

        /**
         * Number of stacks and slices used by the GLUquadric. Default=16
         */
        GLint m_stacksAndSlices;

        /**
         * Antialiasing on? Default=false
         */
        bool m_antialias;

        /**
         */
        GLdouble m_zMultiplier;

        /**
         * How roots are to be coloured
         */
        RmColouration m_colouration;

        GLUquadric* m_quadric;

        /**
         * Map of all colours that are used in all instances of VertexBuffer.
         * MSA TODO Convert to a std::set    DONE
         */
        static ColourSet m_globalColoursInUse;

        static const size_t ESTIMATED_LINE_MAX = 10000;
        static const size_t ESTIMATED_LINES_OF_EACH_COLOUR_MAX = 5000;
        static const size_t ESTIMATED_DOT_MAX = 50000;
        static const size_t ESTIMATED_RECTANGLE_MAX = 100;
        static const size_t ESTIMATED_COLOUR_MAX = 50; // Unlikely to be more than this many colours in a simulation
    }; // class VertexBuffer

    inline void VertexBuffer::ForceNextPrepare() { m_prepared = false; }
    inline BufferBuildLevel VertexBuffer::GetBuildLevel() const { return m_rebuildLevel; }
    inline RmGLColour::RmGLColour() { values[0] = values[1] = values[2] = values[3] = 0.0; }

    inline RmGLColour::RmGLColour(const RmGLColour& orig)
    {
        values[0] = orig.values[0];
        values[1] = orig.values[1];
        values[2] = orig.values[2];
        values[3] = orig.values[3];
    }

    inline RmGLColour::RmGLColour(const wxColour& orig)
    {
        values[0] = static_cast<GLfloat>(orig.Red()) / 255.0;
        values[1] = static_cast<GLfloat>(orig.Green()) / 255.0;
        values[2] = static_cast<GLfloat>(orig.Blue()) / 255.0;
        values[3] = static_cast<GLfloat>(orig.Alpha()) / 255.0;
    }

    inline RmGLColour& RmGLColour::operator=(const wxColour& orig)
    {
        values[0] = static_cast<GLfloat>(orig.Red()) / 255.0;
        values[1] = static_cast<GLfloat>(orig.Green()) / 255.0;
        values[2] = static_cast<GLfloat>(orig.Blue()) / 255.0;
        values[3] = static_cast<GLfloat>(orig.Alpha()) / 255.0;

        return (*this);
    }

    inline RmGLColour& RmGLColour::operator=(const RmGLColour& rhs)
    {
        values[0] = rhs.values[0];
        values[1] = rhs.values[1];
        values[2] = rhs.values[2];
        values[3] = rhs.values[3];
        return (*this);
    }

    inline void VertexBuffer::SetRootRadiusMultiplier(const double multiplier) { m_root_radius_multiplier = multiplier; }
    inline void VertexBuffer::SetCylindricalRoots(bool flag) { m_cylindricalRoots = flag; }
    inline void VertexBuffer::SetInterstitialSpheres(bool flag) { m_interstitialSpheres = flag; }
    inline void VertexBuffer::SetConicalTips(bool flag) { m_conicalRootTips = flag; }
    inline void VertexBuffer::SetStacksAndSlices(size_t sz) { m_stacksAndSlices = static_cast<GLint>(sz); }

    // MSA 11.04.20 Newly inlined functions

    inline RmGLColour* VertexBuffer::GetCanonicalColour(RmGLColour* colour)
    {
        ColourSet::iterator iter = m_globalColoursInUse.find(colour);
        if (iter == m_globalColoursInUse.end())
        {
            m_globalColoursInUse.insert(colour);

            return *(m_globalColoursInUse.find(colour));
        }
        return *iter;
    }

    inline RmGLColour* VertexBuffer::GetCanonicalColour(wxColour& colour) const
    {
        RmGLColour rmcolour(colour);
        return VertexBuffer::GetCanonicalColour(&rmcolour);
    }

    inline void VertexBuffer::Buffer_LineTo(const DoubleCoordinate& dc)
    {
        Buffer_LineTo(dc, m_defaultColour);
    }

    inline void VertexBuffer::Buffer_Line(const DoubleCoordinate& dcoord1, const DoubleCoordinate& dcoord2)
    {
        Buffer_Line(dcoord1, dcoord2, m_defaultColour);
    }

    inline void VertexBuffer::Buffer_Line(const DoubleCoordinate& dcoord1, const DoubleCoordinate& dcoord2, wxColour& colour)
    {
        RmGLColour c(colour);
        Buffer_Line(dcoord1, dcoord2, c);
    }

    inline void VertexBuffer::Buffer_Dot(DoubleCoordinate& fc)
    {
        Buffer_Dot(fc, m_defaultColour);
    }

    inline void VertexBuffer::Unbuffer_Dot(const DoubleCoordinate& /* fc */)
    {
        RmAssert(false, "Deprecated. Use Clear_Dots()");
        //m_dotMap.erase(DotKey(fc.x,fc.y,fc.z));        // Don't delete the colour!
    }

    inline void VertexBuffer::Clear_Dots()
    {
        m_dotArray.clear();
        m_dotColours.clear(); // Remember, doesn't delete the canonical colours, just erases pointers to them
    }

    inline void VertexBuffer::Buffer_Cylinder(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const double& radius, const PlantElementIdentifier& elid)
    {
        // MSA This method will not likely be called. If it is, it will call the RmGLColour Buffer_Cylinder method
        // with the default colour as a parameter.
        Buffer_Cylinder(coord1, coord2, radius, elid, m_defaultColour);
    }

    inline void VertexBuffer::Buffer_Cylinder(const DoubleCoordinate& coord1, const DoubleCoordinate& coord2, const double& radius, const PlantElementIdentifier& elid, wxColour& colour)
    {
        RmGLColour c(colour);
        Buffer_Cylinder(coord1, coord2, radius, elid, c);
    }

    inline void VertexBuffer::RestoreColour(const RmGLColour& prev_colour)
    {
        m_defaultColour = prev_colour;

        // restore previous
        glColor3f(prev_colour.values[0], prev_colour.values[1], prev_colour.values[2]);
    }

    inline void VertexBuffer::SetZMultiplier(double z_multiplier)
    {
        m_zMultiplier = z_multiplier;
    }

    inline void VertexBuffer::SetRebuildLevel(BufferBuildLevel bbl)
    {
        m_rebuildLevel = Utility::CSMax(m_rebuildLevel, bbl);

        if (bbl >= bbl_Redraw)
        {
            ForceNextPrepare();
        }
    }

    RootMapLoggerDeclaration();
} /* namespace rootmap */

#endif // #ifndef VertexBuffer_H
