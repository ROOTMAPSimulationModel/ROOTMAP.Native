#ifndef RmGLVertex_H
#define RmGLVertex_H
/////////////////////////////////////////////////////////////////////////////
// Name:        RmGLVertex.h
// Purpose:     Declaration of the RmGLVertex class
// Created:     20-06-2008 04:02:35
// Author:      RvH
// $Date: 2008-06-20 10:30:16 +0800 (Fri, 20 Jun 2008) $
// $Revision: 7 $
// Copyright:   ©2002-2008 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif
#include "gl/gl.h"

#include "core/common/DoubleCoordinates.h"

namespace rootmap
{
    class RmGLVertex
    {
    public:
        // The only member variables
        GLdouble x, y, z;

        // Ctors and dtor
        RmGLVertex();
        RmGLVertex(const GLdouble& x_, const GLdouble& y_, const GLdouble& z_);
        RmGLVertex(const DoubleCoordinate& dc);
        RmGLVertex(const RmGLVertex& rhs);
        ~RmGLVertex();

        // Operators
        const bool operator==(const RmGLVertex& rhs) const;
        RmGLVertex& operator=(const RmGLVertex& rhs);

        // Mutators
        void set(GLdouble x_, GLdouble y_, GLdouble z_);
        void set(const DoubleCoordinate& dc);
    }; // class RmGLVertex

    inline RmGLVertex::~RmGLVertex()
    {
    }

    inline RmGLVertex::RmGLVertex()
        : x(0.0), y(0.0), z(0.0)
    {
    }

    inline RmGLVertex::RmGLVertex(const GLdouble& x_, const GLdouble& y_, const GLdouble& z_)
        : x(x_), y(y_), z(z_)
    {
    }

    inline RmGLVertex::RmGLVertex(const DoubleCoordinate& dc)
        : x(dc.x), y(dc.y), z(dc.z)
    {
    }

    // Copy constructor does exact copy
    inline RmGLVertex::RmGLVertex(const RmGLVertex& rhs)
        : x(rhs.x), y(rhs.y), z(rhs.z)
    {
    }

    inline const bool RmGLVertex::operator==(const RmGLVertex& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }

    inline RmGLVertex& RmGLVertex::operator=(const RmGLVertex& rhs)
    {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        return *this;
    }

    inline void RmGLVertex::set(GLdouble x_, GLdouble y_, GLdouble z_)
    {
        x = x_;
        y = y_;
        z = z_;
    }

    inline void RmGLVertex::set(const DoubleCoordinate& dc)
    {
        x = dc.x;
        y = dc.y;
        z = dc.z;
    }
} /* namespace rootmap */

#endif // #ifndef RmGLVertex_H
