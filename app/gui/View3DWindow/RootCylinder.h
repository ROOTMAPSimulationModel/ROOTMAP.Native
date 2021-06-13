#ifndef RootCylinder_H
#define RootCylinder_H
#include "app/gui/View3DWindow/RmGLVertex.h"
#include "core/utility/Vec3d.h"
#include "simulation/common/Types.h"
#include "gl/glu.h"

namespace rootmap
{
    class RootCylinder
    {
    public:

        /**    Default constructor - just for container classes
         *    Sets everything to 0
         */
        RootCylinder()
            : height(0), rbase(0), rtop(0), startx(0), starty(0), startz(0), endx(0), endy(0), endz(0)
            , radius(0), red(0), green(0), blue(0), must_move(false), m_elid(0)
        {
        }

        /**
         * Copy ctor
         */
        RootCylinder(const RootCylinder& rhs);

        /** Constructor with all OpenGL parameters (except stacks and slices).
         * A cylinder constructed this way assumes no
         * translations, rotations or colour changes required.
         */
        RootCylinder(double& rb, double& rt, double& h) :
            rbase(rb), rtop(rt), height(h), must_move(false), red(0.0), green(0.0), blue(0.0)
        {
        }

        /** Constructor with all RootMap parameters */
        RootCylinder(const double& sx, const double& sy, const double& sz, const double& ex, const double& ey,
            const double& ez, const double& ra, const double& r, const double& g, const double& b, const rootmap::PlantElementIdentifier& elid);

        // Dtor
        virtual ~RootCylinder();

        static void SetCurrentRGB(const double& r, const double& g, const double& b);

        // Draw with OpenGL quadric object
        void Draw(GLUquadricObj* quad, const GLint& stacksAndSlices);

        // OpenGL parameter members
        double height, rbase, rtop;

        // RootMap parameter members
        double startx, starty, startz, endx, endy, endz;
        double radius, red, green, blue;
        bool must_move;
        PlantElementIdentifier m_elid;
    protected:

    private:
        // Values for rotation
        double rx, ry, rot_angle;

        // Declared private so as to warn us (by throwing a compiler error) if something tries to = assign a RootCylinder
        RootCylinder& operator=(const RootCylinder& rhs);
    };

    // Function object predicate class for use with std::find_if()
    class RootCylinderPredicate
        : public std::unary_function<RootCylinder, bool>
    {
    public:
        RootCylinderPredicate(const RootCylinder& rootCyl)
            : rc(rootCyl)
        {
        }

        // Returns true if the RootCylinder "checkCyl" is the root segment previous to this one
        // (on the same root branch, of course)
        const bool operator()(const RootCylinder& checkCyl) const
        {
            return rc.height > 0 && /* If this cylinder has zero height, the previous cylinder is still effectively the tip */
                checkCyl.endx == rc.startx && checkCyl.endy == rc.starty && checkCyl.endz == rc.startz;
        }

    private:
        const RootCylinder rc;
    };

    inline RootCylinder::RootCylinder(const RootCylinder& rhs)
        : height(rhs.height), rbase(rhs.rbase), rtop(rhs.rtop)
        , startx(rhs.startx), starty(rhs.starty), startz(rhs.startz)
        , endx(rhs.endx), endy(rhs.endy), endz(rhs.endz)
        , radius(rhs.radius), red(rhs.red), green(rhs.green), blue(rhs.blue)
        , must_move(rhs.must_move), m_elid(rhs.m_elid), rx(rhs.rx), ry(rhs.ry), rot_angle(rhs.rot_angle)
    {
    }

    inline RootCylinder& RootCylinder::operator=(const RootCylinder& rhs)
    {
        height = rhs.height;
        rbase = rhs.rbase;
        rtop = rhs.rtop;
        startx = rhs.startx;
        starty = rhs.starty;
        startz = rhs.startz;
        endx = rhs.endx;
        endy = rhs.endy;
        endz = rhs.endz;
        radius = rhs.radius;
        red = rhs.red;
        green = rhs.green;
        blue = rhs.blue;
        must_move = rhs.must_move;
        m_elid = rhs.m_elid;
        rx = rhs.rx;
        ry = rhs.ry;
        rot_angle = rhs.rot_angle;
        return *this;
    }
} /* namespace rootmap */

#endif // #ifndef RootCylinder_H
