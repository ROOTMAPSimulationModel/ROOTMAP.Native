#ifndef View3D_Types_H
#define View3D_Types_H

#include <set>
#include "wx/colour.h"
#include "app/gui/ViewCommon/Types.h"
#include "app/gui/View3DWindow/RmGLVertex.h"

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
}
#endif