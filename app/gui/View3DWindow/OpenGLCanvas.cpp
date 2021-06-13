/////////////////////////////////////////////////////////////////////////////
// Name:        OpenGLCanvas.cpp
// Purpose:     
// Author:      Unknown
// Created:     14/03/2008 22:27:33
// $Date: 2009-07-07 02:40:29 +0800 (Tue, 07 Jul 2009) $
// $Revision: 67 $
// Copyright:   ©2002-2008 University of Tasmania
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "OpenGLCanvas.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "math.h"

#include "gl/gl.h"
#include "gl/glu.h"

#include "app/gui/View3DWindow/glext.h"
#include "app/gui/View3DWindow/OpenGLCanvas.h"

IMPLEMENT_CLASS(OpenGLCanvas, wxGLCanvas)

RootMapLoggerDefinition(OpenGLCanvas);

OpenGLCanvas::OpenGLCanvas
(wxWindow* parent,
    wxWindowID id,
    const wxPoint& /*pos*/,
    const wxSize& /*size*/,
    long /*style*/,
    const wxString& /*name*/,
    int* attribs,
    double trans_scale,
    double rot_scale)
    : wxGLCanvas(parent, id, attribs)
    , TRANSLATION_SCALE(trans_scale)
    , ROTATION_SCALE(rot_scale)
{
    RootMapLoggerInitialisation("rootmap.OpenGLCanvas");
    // Try a semi-modern OpenGL context:
    wxGLContextAttrs ctxAttrs;
    ctxAttrs
        .CompatibilityProfile()
        .OGLVersion(3, 0)
        .Robust()
        .ResetIsolation()
        .EndList();
    m_context = new wxGLContext(this, __nullptr, &ctxAttrs);

    // If that fails, try a more conservative context.
    if (!m_context->IsOK())
    {
        delete m_context;
        ctxAttrs.Reset();
        ctxAttrs
            .CompatibilityProfile()
            .OGLVersion(1, 0)
            .Robust()
            .ResetIsolation()
            .EndList();
        m_context = new wxGLContext(this, __nullptr, &ctxAttrs);
        if (!m_context->IsOK())
        {
            wxMessageBox("The 3D View requires at least an OpenGL 1.0 capable driver.\nROOTMAP should continue to work without the 3D View.",
                "OpenGL version error", wxOK | wxICON_INFORMATION, this);
        }
    }

    extensions = __nullptr;
    extensionCount = -1;

    // Initialize translation values so that viewpoint is at origin.
    m_translateX = m_translateY = m_translateZ = 0.0;

    // Initialize rotation matrix to identity.
    for (int i = 0; i < 16; ++i)
        m_rotationMatrix[i] = 0.0;
    m_rotationMatrix[0] = m_rotationMatrix[5] = m_rotationMatrix[10] = m_rotationMatrix[15] = 1.0;

    // Set this OpenGL canvas as the current OpenGL rendering context.
    // RvH : can't do this until parent frame is Show()n
    //SetCurrent();
}

OpenGLCanvas::~OpenGLCanvas()
{
    delete[] extensions;
    delete m_context;
}

void OpenGLCanvas::SetTranslation(double x, double y, double z)
{
    m_translateX = x;
    m_translateY = y;
    m_translateZ = z;

    ApplyTransformation();
    Refresh();
}

void OpenGLCanvas::GetTranslation(double& x, double& y, double& z, double& scale)
{
    x = m_translateX;
    y = m_translateY;
    z = m_translateZ;
    scale = TRANSLATION_SCALE;
}

void OpenGLCanvas::SetRotation(double rotation, double x, double y, double z)
{
    ApplyRotation(rotation, rootmap::Vec3d(x, y, z));
}

void OpenGLCanvas::GetRotation(double&/* rotation */, double&/* x */, double&/* y */, double&/* z */)
{
}

const wxString* OpenGLCanvas::GetExtensions()
{
    if (!extensions)
        EnumExtensions();
    return (const wxString*)extensions;
}

int OpenGLCanvas::GetExtensionCount()
{
    if (!extensions)
        EnumExtensions();
    return extensionCount;
}

bool OpenGLCanvas::IsExtensionSupported(const wxString& extension)
{
    if (!extensions)
        EnumExtensions();
    for (int i = 0; i < extensionCount; ++i)
        if (extensions[i].Cmp(extension.c_str()) == 0)
            return true;
    return false;
}

/**
 * Maps the event handler methods.
 */
BEGIN_EVENT_TABLE(OpenGLCanvas, wxGLCanvas)
EVT_SIZE(OpenGLCanvas::OnSize)
EVT_PAINT(OpenGLCanvas::OnPaint)
EVT_ERASE_BACKGROUND(OpenGLCanvas::OnEraseBackground)
EVT_MOUSE_EVENTS(OpenGLCanvas::OnMouse)
END_EVENT_TABLE()

void OpenGLCanvas::OnPaint(wxPaintEvent& /* event */)
{
    // This is a dummy, to avoid an endless succession of paint messages.
    // OnPaint handlers must always create a wxPaintDC.
    wxPaintDC dc(this);

#ifndef __WXMOTIF__
    // Checks if the OpenGL was successfully initialized. Motif doesn't use
    // an OpenGL context.
    if (!m_context)
        return;
#endif

    // Set this OpenGL canvas as the current OpenGL rendering context, in case
    // we previously lost the rights to the 3D hardware due to another OpenGL
    // window or application being active.
    SetCurrent(*m_context);

    // Calls the subclass' DrawScene() method.
    DrawScene();

    // We're using a double buffer, so display our scene.
    SwapBuffers();
}

void OpenGLCanvas::OnSize(wxSizeEvent& /*event*/)
{
    // this is also necessary to update the context on some platforms
    //TODO wxGLCanvas::OnSize(event);

#ifndef __WXMOTIF__
    // Checks if the OpenGL was successfully initialized. Motif doesn't use
    // an OpenGL context.
    if (m_context)
#endif
    {
        // Set this OpenGL canvas as the current OpenGL rendering context, in case
        // we previously lost the rights to the 3D hardware due to another OpenGL
        // window or application being active.
        SetCurrent(*m_context);

        // Compute and set the OpenGL viewport and GL_PROJECTION matrix.
        DoResize();

        // Apply translation values and rotation matrix.
        ApplyTransformation();
    }
}

void OpenGLCanvas::OnEraseBackground(wxEraseEvent& /* event */)
{
    // Do nothing, to avoid flashing.
}

void OpenGLCanvas::OnMouse(wxMouseEvent& event)
{
    static rootmap::Vec3d lastPoint, curPoint;
    static int lastX, lastY;

    if (event.LeftDown())
    { // left mouse button is pressed.
        //
        // Map the mouse position to a logical sphere location.
        // Keep it in the class variable lastPoint.
        //
        lastPoint = TrackBallMapping(event.GetX(), event.GetY());
    }
    else if (event.RightDown())
    { // right mouse button is pressed.
        lastX = event.GetX();
        lastY = event.GetY();
    }
    else if (event.MiddleDown())
    { // middle mouse button is pressed.
        lastX = event.GetX();
        lastY = event.GetY();
    }
    else if (event.Dragging())
    { // mouse is being dragged while a mouse button is held down.
        if (event.LeftIsDown())
        { // left mouse button is being dragged.
            curPoint = TrackBallMapping(event.GetX(), event.GetY()); // Map the mouse position to a logical sphere location.
            rootmap::Vec3d direction = curPoint - lastPoint;
            double velocity = direction.Length();
            if (velocity > 0.0001)
            {
                // Rotate about the axis that is perpendicular to the great circle connecting the mouse movements.
                rootmap::Vec3d rotAxis;
                rotAxis.crossProd(lastPoint, curPoint);
                double rot_angle = velocity * ROTATION_SCALE;

                ApplyRotation(rot_angle, rotAxis);
                // Finally, apply transformation from scratch, in case it has been overridden to do other
                // stuff. Then, refresh.
                ApplyTransformation();
                Refresh();

                lastPoint = curPoint;
            }
        }
        else if (event.RightIsDown())
        { // right mouse button is being dragged.
            m_translateX += (event.GetX() - lastX) * TRANSLATION_SCALE;
            m_translateY += (lastY - event.GetY()) * TRANSLATION_SCALE;

            ApplyTransformation();
            Refresh();

            lastX = event.GetX();
            lastY = event.GetY();
        }
        else if (event.MiddleIsDown())
        { // middle mouse button is being dragged.
            //m_translateX += (event.GetX() - lastX) * TRANSLATION_SCALE;
            m_translateZ += (event.GetY() - lastY) * TRANSLATION_SCALE;

            ApplyTransformation();
            Refresh();

            lastX = event.GetX();
            lastY = event.GetY();
        }
    }
}

rootmap::Vec3d OpenGLCanvas::TrackBallMapping(int x, int y)
{
    int w, h;
    GetSize(&w, &h);

    // Scale bounds to [0,0] - [2,2]
    double vx = x / (w / 2.0);
    double vy = y / (h / 2.0);
    // Translate 0,0 to the centre
    vx = vx - 1;
    // Flip so +Y is up instead of down
    vy = 1 - vy;
    // vz from the sphere equation: sqrtf(x^2+y^2+z^2) = r^2; // r==1, unit sphere
    double vz2 = 1 - vx * vx - vy * vy;

    double vz = vz2 > 0 ? sqrtf(vz2) : 0;

    rootmap::Vec3d vec(vx, vy, vz);
    vec.Normalize();

    ////

    /*
    Vec3d v;
    double d;
    v.x = (2.0*x - w) / w;
    v.y = (h - 2.0*y) / h;
    v.z = 0.0;
    d = v.Length();
    d = (d < 1.0) ? d : 1.0;  // If d is > 1, then clamp it at one.
    v.z = sqrtf(1.001 - d*d);  // project the line segment up to the surface of the sphere.

    v.Normalize();  // We forced d to be less than one, not v, so need to normalize somewhere.
    */
    return vec;
}


void OpenGLCanvas::ApplyRotation(GLdouble rot_angle, const rootmap::Vec3d& rotAxis)
{
    // MSA TODO Implement translation before and after rotation such that the centre of rotation
    // is the centre of the viewport.


    // This is the tricky part. We want to apply the rotation that just occurred to the rotation
    // that was previously in use. So, we load identity, apply new rotation, apply old rotation.
    //
    // GL_MODELVIEW = rotation * R(rot_angle, rotAxis) 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // Apply the new rotation
    glRotated(rot_angle, rotAxis.x, rotAxis.y, rotAxis.z);
    glMultMatrixd(m_rotationMatrix);

    // Then, we want to store the result back in our rotation field.
    glGetDoublev(GL_MODELVIEW_MATRIX, m_rotationMatrix);
}

void OpenGLCanvas::ApplyTransformation()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslated(m_translateX, m_translateY, m_translateZ);
    glMultMatrixd(m_rotationMatrix);
}

void OpenGLCanvas::DrawAxes()
{
    // Store current render state so we can set the values back to what they were. We
    // want this function to be used by subclasses rather transparently.

    bool lighting;
    glGetBooleanv(GL_LIGHTING, (GLboolean*)&lighting);
    glDisable(GL_LIGHTING);

    // make sure they are filled and flat
    int polygonMode[2];
    glGetIntegerv(GL_POLYGON_MODE, (GLint*)polygonMode);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    int shadeModel;
    glGetIntegerv(GL_SHADE_MODEL, (GLint*)&shadeModel);
    glShadeModel(GL_FLAT);

    int matrixMode;
    glGetIntegerv(GL_MATRIX_MODE, (GLint*)&matrixMode);
    glMatrixMode(GL_MODELVIEW);

    // the axes are simply cylinders aligned with each axis. x-axis is red,
    // y-axis is green, and z-axis is blue.

    // we're using a quadric object to draw the cylinders
    GLUquadricObj* quadric = gluNewQuadric();

    // x-axis
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, -500.0f);
    gluCylinder(quadric, 0.05f, 0.05f, 1000.0f, 10, 10);
    glPopMatrix();

    // y-axis
    glColor3f(0.0f, 1.0f, 0.0f);
    glPushMatrix();
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, -500.0f);
    gluCylinder(quadric, 0.05f, 0.05f, 1000.0f, 10, 10);
    glPopMatrix();

    // z-axis
    glColor3f(0.0f, 0.0f, 1.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -500.0f);
    gluCylinder(quadric, 0.05f, 0.05f, 1000.0f, 10, 10);
    glPopMatrix();

    // delete quadric
    gluDeleteQuadric(quadric);

    // Restore render state
    if (lighting)
        glEnable(GL_LIGHTING);

    glPolygonMode(GL_FRONT, polygonMode[0]);
    glPolygonMode(GL_BACK, polygonMode[1]);

    glShadeModel(shadeModel);

    glMatrixMode(matrixMode);
}

void OpenGLCanvas::EnumExtensions()
{
    if (!extensions)
    {
        // MSA Left this as a C-style cast because C++ casts won't do the job
        // and I don't want to mess with this code too much
        char* ext = (char*)glGetString(GL_EXTENSIONS);

        int index = 0;
        this->extensionCount = 0;
        while (ext[index])
        {
            if (ext[index] == ' ')
                this->extensionCount++;
            ++index;
        }

        this->extensions = new wxString[this->extensionCount];

        int countIndex = 0;
        index = 0;
        int stIndex = 0;
        while (ext[index])
        {
            if (ext[index] == ' ')
            {
                int length = index - stIndex;
                this->extensions[countIndex] = wxString(&ext[stIndex], length);
                ++countIndex;
                stIndex = index + 1;
            }
            ++index;
        }
    }
}
