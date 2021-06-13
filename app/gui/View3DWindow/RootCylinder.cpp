#include "app/gui/View3DWindow/RootCylinder.h"

namespace rootmap
{
    const static double RAD_TO_DEG = (180.0 / 3.141592653589793238);

    // Previous colour values
    static double currentR;
    static double currentG;
    static double currentB;

    /** Constructor for calculating the parameters of an OpenGL cylinder
     * from the parameters of a root segment, i.e.
     *  1) a pair of coordinates,
     *  2) radius (from branch order),
     *    3) PlantElementIdentifier, and
     *    4) colour
     */
    RootCylinder::RootCylinder(const double& sx, const double& sy, const double& sz,
        const double& ex, const double& ey, const double& ez,
        const double& ra, const double& r, const double& g, const double& b,
        const PlantElementIdentifier& elid) :
        startx(sx), starty(sy), startz(sz), endx(ex), endy(ey), endz(ez),
        radius(ra), red(r), green(g), blue(b), m_elid(elid)
    {
        rbase = radius;
        rtop = radius;

        // Credit for vector maths code goes to 
        // http://home.neo.rr.com/jparris/OpenGL%20-%20draw%20cylinder%20between%202%20pts.htm
        Vec3d axial_vec = Vec3d(ex - sx, ey - sy, ez - sz);
        height = axial_vec.Length();
        double vx = axial_vec.x;
        double vy = axial_vec.y;
        double vz = axial_vec.z;
        rot_angle = RAD_TO_DEG * acos(vz / height);
        if (vz <= 0.0) rot_angle = -rot_angle;
        rx = -vy * vz;
        ry = +vx * vz;

        must_move = true;
    }

    RootCylinder::~RootCylinder()
    {
    }

    void RootCylinder::SetCurrentRGB(const double& r, const double& g, const double& b)
    {
        currentR = r;
        currentG = g;
        currentB = b;
    }

    void RootCylinder::Draw(GLUquadricObj* quad, const GLint& stacksAndSlices)
    {
        if (must_move)
        {
            // Apply translation and rotation
            glTranslated(startx, starty, startz);
            glRotated(rot_angle, rx, ry, 0.0);
        }
        if (red != currentR || green != currentG || blue != currentB)
        {
            // Apply new colour
            glColor3f(red, green, blue);
            currentR = red;
            currentG = green;
            currentB = blue;
        }
        gluCylinder(quad, rbase, rtop, height, stacksAndSlices, stacksAndSlices);
    }
} /* namespace rootmap */
