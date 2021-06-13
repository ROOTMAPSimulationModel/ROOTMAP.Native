#ifndef OpenGLCanvas_H
#define OpenGLCanvas_H
/////////////////////////////////////////////////////////////////////////////
// Name:        OpenGLCanvas.h
// Purpose:     Declaration of the OpenGLCanvas class
// Created:     14/03/2008
// Author:      RvH
// $Date: 2008-06-22 22:24:30 +0800 (Sun, 22 Jun 2008) $
// $Revision: 10 $
// Copyright:   ©2002-2008 University of Tasmania
// 
// Found at "Andy's Game Programming Resources" as part of wxOpenGL
// http://www.soe.ucsc.edu/~agames/wxogl.htm
//
// UPDATE: Link is broken as at 09.12.10
// Similar documentation can be found here:
// http://www.cse.ohio-state.edu/~crawfis/Graphics/VirtualTrackball.html
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "OpenGLCanvas.h"
#endif

#include "wx/glcanvas.h"
#include "core/log/Logger.h"
#include "core/utility/Vec3d.h"


// MSA 09.12.10 Temporary class to help me get a better conceptual hold on this stuff

class VisuMatrix
{
    char buf[512];
    double data[16];


public:
    VisuMatrix(const double* thedata, const size_t& sz)
    {
        if (sz > 16) return;
        memcpy(data, thedata, sz * sizeof(double));
        size_t dim = sqrtf((float)sz);
        if (dim == 4)
        {
            sprintf(buf, "% f\t, % f\t, % f\t, % f\t\t\tm[0]\t, m[4]\t, m[8]\t, m[12]\n% f\t, % f\t, % f\t, % f\t\t\tm[1]\t, m[5]\t, m[9]\t, m[13]\n% f\t, % f\t, % f\t, % f\t\t\tm[2]\t, m[6]\t, m[10]\t, m[14]\n% f\t, % f\t, % f\t, % f\t\t\tm[3]\t, m[7]\t, m[11]\t, m[15]\n",
                data[0], data[4], data[8], data[12], data[1], data[5], data[9], data[13], data[2], data[6], data[10], data[14], data[3], data[7], data[11], data[15]);
        }
        else if (dim == 3)
        {
            sprintf(buf, "% f\t, % f\t, % f\t\t\tm[0]\t, m[3]\t, m[6]\n% f\t, % f\t, % f\t\t\tm[1]\t, m[4]\t, m[7]\n% f\t, % f\t, % f\t\t\tm[2]\t, m[5]\t, m[8]\n",
                data[0], data[3], data[6], data[1], data[4], data[7], data[2], data[5], data[8]);
        }
    }

    char* getstr()
    {
        return buf;
    }
};

/**
 * This is a helper class that does two things:
 * 1) Hides some of the tedious details of working with the wxGLCanvas class
 * 2) provides a virtual trackball interface that can be used in applications.
 *
 * This class must be subclassed for your own application. In fact, we demonstrate
 * how to subclass it in CubeCanvas.h/cpp.
 *
 * Two abstract methods must be implemented by subclasses: DoResize() and DrawScene().
 */
class OpenGLCanvas : public wxGLCanvas
{
    DECLARE_ABSTRACT_CLASS(OpenGLCanvas)

public:

    /**
     * Constructor. Takes parent Window and two mouse sensitivity factors. The trans_scale
     * factor determines the mouse sensitivity for translating the viewpoint/scene with the
     * mouse. The rot_scale factor determines the mouse sensitivity for rotating the
     * viewpoint/scene with the mouse.
     *
     * The left mouse button performs the rotation; the middle mouse button performs
     * XZ-parallel translation; the right mouse button performs XY-parallel translation.
     */
     //OpenGLCanvas(wxWindow *parent, double trans_scale = 0.1, double rot_scale = 45.0);
    OpenGLCanvas(wxWindow* parent,
        wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        long style,
        const wxString& name,
        int* attribs,
        double trans_scale = 0.1,
        double rot_scale = 45.0);

    ~OpenGLCanvas();

    /**
     * Sets the translation values for the current viewpoint. These values are effectively
     * the centroid of rotation for the virtual trackball motion.
     *
     * The new transformation is applied and the scene redrawn.
     */
    void SetTranslation(double x, double y, double z);

    /**
     * Gets the translation values.
     */
    void GetTranslation(double& x, double& y, double& z, double& scale);

    /**
     * The new transformation is applied and the scene redrawn.
     * @param rotation : angle in radians
     */
    void SetRotation(double rotation, double x, double y, double z);

    void GetRotation(double& rotation, double& x, double& y, double& z);

    const wxString* GetExtensions();

    int GetExtensionCount();

    bool IsExtensionSupported(const wxString& extension);

protected:

    /**
     * Declaring an event table allows us to map wxWindows events to methods
     * that we want invoked in response to those events.
     */
    DECLARE_EVENT_TABLE()

    /**
     * Invoked in response to the containing Frame being activated or deactivated.
     * This is denoted by the title bar changing color. When activated, a child
     * component of the Frame has keyboard focus.
     */
    void OnActivate(wxActivateEvent& event);

    /**
     * Invoked in response to the OpenGL canvas being resized (e.g. due to re-layout
     * of the containing Frame's components).
     */
    void OnSize(wxSizeEvent& event);

    /**
     * Invoked when the OpenGL canvas needs to repaint its window.
     */
    void OnPaint(wxPaintEvent& event);

    /**
     * Invoked when the OpenGL canvas needs to repaint its background. We are
     * responding to this event in order to avoid the background color flashing
     * intermittently with the scene when repainted.
     */
    void OnEraseBackground(wxEraseEvent& event);

    /**
     * Invoked in response to a mouse event.
     */
    void OnMouse(wxMouseEvent& event);

    /**
     * Maps a 2D window coordinate generated by a mouse event to a 3D point on a sphere.
     *
     */
    rootmap::Vec3d TrackBallMapping(int x, int y);

    /**
       * Modifies the GL_MODELVIEW matrix to reflect the given rotation in the
       * rotation matrix.
     */
    void ApplyRotation(GLdouble rot_angle, const rootmap::Vec3d& rotAxis);

    /**
     * Modifies the GL_MODELVIEW matrix to reflect the current translation values and
     * rotation matrix.
     *
     * GL_MODELVIEW = T(x, y, z) * rotation
     */
    void ApplyTransformation();

    /**
     * Draws a set of coordinate axes at the local origin. Each axis is a thin, long
     * cylinder. If a non-uniform scale transformation has been applied, then the
     * axes will appear distorted.
     *
     * The x-axis is red, the y-axis is green, and the z-axis is blue (XYZ -> RGB).
     * Lighting and smooth shading are disabled, but subsequently returned to their
     * previous settings before returning to caller.
     */
    void DrawAxes();

    void EnumExtensions();

    /**
     * To be overridden by subclasses to set up OpenGL viewport and GL_PROJECTION matrix.
     * This will be invoked whenever the OpenGL canvas is resized, including when it is
     * first visible.
     */
    virtual void DoResize() = 0;

    /**
     * To be overridden by subclasses to render the OpenGL scene. It is intended that
     * any GL_MODELVIEW transformations applied should be restored before returning to
     * caller. This is because these transformations will accumulate with each invocation
     * of DrawScene(), until the OpenGL canvas is resized. When this happens, the
     * GL_MODELVIEW is reset via ApplyTransformation().
     */
    virtual void DrawScene() = 0;
    /**
     * Our GL_MODELVIEW trackball rotation matrix.
     */
    double m_rotationMatrix[16];

    /**
     * The centre point of the trackball sphere.
     */
     //double m_trackballCentroid[3];

private:
    RootMapLoggerDeclaration();

    wxGLContext* m_context;

    /**
     * Set in constructor.
     */
    const double TRANSLATION_SCALE;
    const double ROTATION_SCALE;

    /**
     * Our GL_MODELVIEW translation values (or centroid).
     */
    double m_translateX, m_translateY, m_translateZ;


    wxString* extensions;

    int extensionCount;
};

#endif // #ifndef OpenGLCanvas_H
