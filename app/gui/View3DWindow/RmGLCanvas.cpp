/////////////////////////////////////////////////////////////////////////////
// Name:        RmGLCanvas.cpp
// Purpose:     Implementation of the RmGLCanvas class
// Created:     14/03/2008
// Author:      RvH
// $Date: 2009-07-07 02:40:29 +0800 (Tue, 07 Jul 2009) $
// $Revision: 67 $
// Copyright:   ©2002-2008 University of Tasmania
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "RmGLCanvas.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "app/gui/View3DWindow/RmGLCanvas.h"
#include "app/gui/View3DWindow/ProcessVertexBuffer.h"
#include "app/gui/View3DWindow/ScoreboardBoxBoundaryVertices.h"
#include "app/gui/ViewCommon/View3DInformation.h"
#include "simulation/process/common/ProcessDrawing.h"
#include "simulation/process/common/Process.h"
#include "simulation/common/Types.h"
#include "core/utility/Utility.h"
#include "core/common/ExtraStuff.h"
#include "gl/gl.h"
#include "gl/glu.h"

////@begin XPM images

////@end XPM images


/*!
 * RmGLCanvas type definition
 */

IMPLEMENT_CLASS(RmGLCanvas, OpenGLCanvas)


/*!
 * RmGLCanvas event table definition
 */

    BEGIN_EVENT_TABLE(RmGLCanvas, OpenGLCanvas)

    ////@begin RmGLCanvas event table entries
    EVT_MOUSEWHEEL(RmGLCanvas::OnMouseWheel)
    EVT_CHAR(RmGLCanvas::OnChar)

    ////@end RmGLCanvas event table entries

    END_EVENT_TABLE()

    RootMapLoggerDefinition(RmGLCanvas);

using rootmap::View3DInformation;
using rootmap::ViewInformation;
using rootmap::ProcessVertexBuffer;
using rootmap::ScoreboardStratum;
using rootmap::ScoreboardCoordinator;
using rootmap::BufferChangeResponse;
using rootmap::Process;
using rootmap::Utility::CSMin;
using rootmap::Utility::CSMax;
using rootmap::ProcessArray;

/*!
 * RmGLCanvas constructors
 */
RmGLCanvas::RmGLCanvas(wxWindow* parent, rootmap::IView3DCoordinator* viewCoordinator, rootmap::ScoreboardCoordinator& scoreboardcoordinator, const rootmap::View3DInformation& viewInfo, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name, int* attribs)
    : OpenGLCanvas(parent, id, pos, size, style, name, attribs)
    , m_viewInformation(__nullptr)
    , m_scoreboardCoordinator(scoreboardcoordinator)
    , m_boxBoundaries(__nullptr)
{
    RootMapLoggerInitialisation("rootmap.RmGLCanvas");
    Init();
    Create(parent, viewCoordinator, scoreboardcoordinator, viewInfo, id, pos, size, style, name, attribs);
}


/*!
 * RmGLCanvas creator
 */

bool RmGLCanvas::Create(wxWindow* parent, rootmap::IView3DCoordinator* viewCoordinator, rootmap::ScoreboardCoordinator& scoreboardcoordinator, const rootmap::View3DInformation& viewInfo, wxWindowID /* id */, const wxPoint& /* pos */, const wxSize& /* size */, long /* style */, const wxString& /* name */, int* /* attribs */)
{
    ////@begin RmGLCanvas creation
    SetParent(parent);
    CreateControls(viewCoordinator, scoreboardcoordinator, viewInfo);
    ////@end RmGLCanvas creation
    return true;
}


/*!
 * RmGLCanvas destructor
 */

RmGLCanvas::~RmGLCanvas()
{
    ////@begin RmGLCanvas destruction
    for (ProcessBufferContainer::iterator iter = m_processBuffers.begin();
        iter != m_processBuffers.end();
        ++iter)
    {
        delete (*iter);
    }
    delete m_boxBoundaries;
    delete m_viewInformation;
    ////@end RmGLCanvas destruction
}


/*!
 * Member initialisation
 */

void RmGLCanvas::Init()
{
    ////@begin RmGLCanvas member initialisation
    m_scoreboardCoordinator.GetTotalScoreboardBounds(m_scoreboardBounds);
    ////@end RmGLCanvas member initialisation
}


/*!
 * Control creation for RmGLCanvas
 */

void RmGLCanvas::CreateControls(rootmap::IView3DCoordinator* /* viewCoordinator */, rootmap::ScoreboardCoordinator& scoreboardcoordinator, const rootmap::View3DInformation& viewInfo)
{
    ////@begin RmGLCanvas content construction
    ////@end RmGLCanvas content construction

    //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    glClearDepth(1.0f); // Depth Buffer Setup
    glEnable(GL_DEPTH_TEST); // Enables Depth Testing
    glDepthFunc(GL_LEQUAL); // The Type Of Depth Test To Do

    SetRotation(30.0, 0.0, 0.0, 1.0);
    SetRotation(-60.0, 1.0, 0.0, 0.0);
    SetRotation(0.0, 0.0, 1.0, 0.0);

    // determines shift in horizontal (X), vertical (Y) and depth (Z) planes of the zero point
    // -100<=X<=100 (default:centred:0.0)
    // -100<=Y<=100 (default:centred:0.0)
    // -100<=Z<=0   (default:centred:-50.0)
    DoubleCoordinate cameraPosition = viewInfo.GetCameraPosition();
    SetTranslation(cameraPosition.x, cameraPosition.y, cameraPosition.z);


    // Create custom windows not generated automatically here.
    //glEnableClientState(GL_VERTEX_ARRAY);
    //glVertexPointer(    3,   //3 components per vertex (x,y,z)
    //                    GL_FLOAT,
    //                    sizeof(SVertex),
    //                    Vertices);
    ////@begin RmGLCanvas content initialisation
    ////@end RmGLCanvas content initialisation
    m_viewInformation = new View3DInformation(viewInfo);

    int width, height;
    GetClientSize(&width, &height);
    m_viewInformation->SetBounds(wxRect(0, 0, width, height));

    m_boxBoundaries = new rootmap::ScoreboardBoxBoundaryVertices(scoreboardcoordinator, viewInfo);

    CreateProcessBuffers(viewInfo);
}


BufferChangeResponse RmGLCanvas::CreateProcessBuffers(const View3DInformation& viewInfo)
{
    BufferChangeResponse current_response;
    BufferChangeResponse best_response = rootmap::bcr_Failure;
    BufferChangeResponse worst_response = rootmap::bcr_OK;
    const ProcessArray& processes = viewInfo.GetDisplayProcesses();

    // iterate over the processes, making a buffer for each
    for (ProcessArray::const_iterator iter = processes.begin();
        iter != processes.end(); ++iter)
    {
        current_response = CreateProcessBuffer(*iter);

        best_response = (BufferChangeResponse)CSMin(best_response, current_response);
        worst_response = (BufferChangeResponse)CSMax(worst_response, current_response);

        // assumption: if creating this process buffer failed, the rest will
        if (current_response > rootmap::bcr_OK)
        {
            break;
        }
    }

    return worst_response;
}

BufferChangeResponse RmGLCanvas::CreateProcessBuffer(Process* process)
{
    ProcessVertexBuffer* buffer = new ProcessVertexBuffer(&(process->getDrawing()), m_scoreboardCoordinator, GetViewInformation());

    m_processBuffers.push_back(buffer);

    //process->getDrawing().AddDrawingBuffer(buffer); Now performed in buffer constructor
    return rootmap::bcr_OK;
}

void RmGLCanvas::RemoveProcessBuffer(Process* process)
{
    for (ProcessBufferContainer::iterator iter = m_processBuffers.begin();
        iter != m_processBuffers.end();
        ++iter)
    {
        if ((*iter)->GetParent()->GetProcess() == process)
        {
            delete (*iter);
            m_processBuffers.erase(iter);

            // don't have a rebuild level ourselves!
            //SetRebuildLevel(rootmap::bbl_Reslap);

            break;
        }
    }
}


/*!
 * Should we show tooltips?
 */

bool RmGLCanvas::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap RmGLCanvas::GetBitmapResource(const wxString& name)
{
    // Bitmap retrieval
    ////@begin RmGLCanvas bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
    ////@end RmGLCanvas bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon RmGLCanvas::GetIconResource(const wxString& name)
{
    // Icon retrieval
    ////@begin RmGLCanvas icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
    ////@end RmGLCanvas icon retrieval
}


void RmGLCanvas::DoResize()
{
    int w, h;
    GetClientSize(&w, &h);

    m_viewInformation->SetBounds(wxRect(0, 0, w, h));
    glViewport(0, 0, (GLint)w, (GLint)h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // fov = 54.0 is approx. as per 35mm camera lens effect
    gluPerspective(54.0, static_cast<GLdouble>(w) / static_cast<GLdouble>(h), 0.6, 600.0);
    LOG_DEBUG << LINE_HERE << "DoResize to " << w << " by " << h;

    //const GLdouble aspect = ((GLdouble)w / (GLdouble)h);
    //const GLdouble y_clip = 12.0;
    //const GLdouble x_clip = y_clip*aspect;
    //glOrtho( -(x_clip), x_clip, -(y_clip), y_clip, 8.0, 500.0 );
    //LOG_INFO << LINE_HERE << "DoResize to " << w << " by " << h << "y_clip=" << y_clip;

    glMatrixMode(GL_MODELVIEW);
}

/**
 *    MSA 09.11.27 New convenience method for setting the 3D view to vTop/vFront/vSide
 *    (equivalent to the 2D view's selectable viewpoints)
 */
void RmGLCanvas::SetViewpoint(const rootmap::ViewDirection& vDir)
{
    double tx, ty, tz;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    switch (vDir)
    {
    case rootmap::vTop:
        LOG_INFO << "TOP VIEW";
        tx = (m_scoreboardBounds.right - m_scoreboardBounds.left) / 2.0;
        ty = (m_scoreboardBounds.back - m_scoreboardBounds.front) / 2.0;
        tz = 0;
        gluLookAt(tx, ty, 100, // Camera 'location'
            tx, ty, 0, // Point to look at (centre of top plane of Scoreboard)
            0, 1, 0); // Sky vector ('up' = +ve Y)
        break;
    case rootmap::vFront:
        LOG_INFO << "FRONT VIEW";
        tx = (m_scoreboardBounds.right - m_scoreboardBounds.left) / 2.0;
        ty = 0;
        tz = ((m_scoreboardBounds.bottom - m_scoreboardBounds.top) / 2.0) * -1; // Remember that our Scoreboard is in the Z<0 "underground"
        gluLookAt(tx, -100, tz, // Camera 'location'
            tx, 0, tz, // Point to look at (centre of front plane of Scoreboard)
            0, 0, 1); // Sky vector ('up' = +ve Z)
        break;
    case rootmap::vSide:
        LOG_INFO << "SIDE VIEW";
        tx = 0;
        ty = (m_scoreboardBounds.back - m_scoreboardBounds.front) / 2.0;
        tz = ((m_scoreboardBounds.bottom - m_scoreboardBounds.top) / 2.0) * -1; // Remember that our Scoreboard is in the Z<0 "underground"
        gluLookAt(-100, ty, tz, // Camera 'location'
            0, ty, tz, // Point to look at (centre of side plane of Scoreboard)
            0, 0, 1); // Sky vector ('up' = +ve Z)
        break;
    default:
        return;
    }

    /*
    m_trackballCentroid[0] = tx;
    m_trackballCentroid[1] = ty;
    m_trackballCentroid[2] = tz;
    */

    // Store the rotation result back in base class's rotation field.
    glGetDoublev(GL_MODELVIEW_MATRIX, m_rotationMatrix);
    //LOG_INFO << "OPENGL MODELVIEW MATRIX:\n" << VisuMatrix(m_rotationMatrix).getstr();
    // Must zero out the OpenGLCanvas translation variables.
    SetTranslation(0, 0, 0);

    // The problem, now, is that the camera is stuck rotating around the gluLookAt-determined point

    DrawScene();
    SwapBuffers();
}

void RmGLCanvas::DrawScene()
{
    // clear color and depth buffers.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // MSA 09.12.24 Offset everything so that the centre of the Scoreboard is the origin.
    const double offx = -(m_scoreboardBounds.right - m_scoreboardBounds.left) / 2.0;
    const double offy = -(m_scoreboardBounds.back - m_scoreboardBounds.front) / 2.0;
    //const double offz = -((m_scoreboardBounds.bottom - m_scoreboardBounds.top) / 2.0) * -1; // Remember that our Scoreboard is in the Z<0 "underground"
    const double offz = 10; //vmd
    glTranslated(offx, offy, offz);

    // Uncomment these lines to draw a point at the origin.

    //glBegin(GL_POINTS);
    //glVertex3f(-offx,-offy,-offz);
    //glEnd();
    //glColor3f(1,1,0);


    // Draw coordinate axes.
    //DrawAxes();
    //DrawCube(0.5);

    m_boxBoundaries->DrawScene();

    for (ProcessBufferContainer::iterator iter(m_processBuffers.begin());
        iter != m_processBuffers.end(); ++iter)
    {
        (*iter)->DrawScene();
    }
    // Must flush the pipeline.
    glFlush();

    // Undo the offset so future DrawScene()s don't have a cumulative effect
    glTranslated(-offx, -offy, -offz);
}

void RmGLCanvas::DrawCube(GLdouble size)
{
    glColor3f(1.0, 0.0, 0.0);

    GLdouble cz = size / 2.0;

    // Draw cube.
    glBegin(GL_QUADS);
    glVertex3f(cz, cz, cz);
    glVertex3f(cz, cz, -(cz));
    glVertex3f(-(cz), cz, -(cz));
    glVertex3f(-(cz), cz, cz);

    glVertex3f(cz, -(cz), cz);
    glVertex3f(-(cz), -(cz), cz);
    glVertex3f(-(cz), -(cz), -(cz));
    glVertex3f(cz, -(cz), -(cz));

    glVertex3f(cz, cz, cz);
    glVertex3f(cz, -(cz), cz);
    glVertex3f(cz, -(cz), -(cz));
    glVertex3f(cz, cz, -(cz));

    glVertex3f(-(cz), cz, cz);
    glVertex3f(-(cz), cz, -(cz));
    glVertex3f(-(cz), -(cz), -(cz));
    glVertex3f(-(cz), -(cz), cz);

    glVertex3f(cz, cz, cz);
    glVertex3f(-(cz), cz, cz);
    glVertex3f(-(cz), -(cz), cz);
    glVertex3f(cz, -(cz), cz);

    glVertex3f(cz, cz, -(cz));
    glVertex3f(cz, -(cz), -(cz));
    glVertex3f(-(cz), -(cz), -(cz));
    glVertex3f(-(cz), cz, -(cz));
    glEnd();

    /*
    // Temp
    glColor3f(0.0,1.0,0.0);
    cz = 5;
    glBegin(GL_QUADS);
        // Lesser YZ quad
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, cz);
        glVertex3f(0, cz, cz);
        glVertex3f(0, cz, 0);

        // Greater YZ quad
        glVertex3f(cz/2, 0, 0);
        glVertex3f(cz/2, 0, cz);
        glVertex3f(cz/2, cz, cz);
        glVertex3f(cz/2, cz, 0);

        // Lesser XZ quad
        glVertex3f(0, 0, 0);
        glVertex3f(cz/2, 0, 0);
        glVertex3f(cz/2, 0, cz);
        glVertex3f(0, 0, cz);

        // Greater XZ quad
        glVertex3f(0, cz, 0);
        glVertex3f(cz/2, cz, 0);
        glVertex3f(cz/2, cz, cz);
        glVertex3f(0, cz, cz);

        // Lesser XY quad
        glVertex3f(0, 0, 0);
        glVertex3f(cz/2, 0, 0);
        glVertex3f(cz/2, cz, 0);
        glVertex3f(0, cz, 0);

        // Greater XY quad
        glVertex3f(0, 0, cz);
        glVertex3f(cz/2, 0, cz);
        glVertex3f(cz/2, cz, cz);
        glVertex3f(0, cz, cz);

    glEnd();
    //glClear(GL_COLOR_BUFFER_BIT);
    */
}

const View3DInformation& RmGLCanvas::GetViewInformation() const
{
    return (*m_viewInformation);
}

View3DInformation& RmGLCanvas::GetViewInformation()
{
    return (*m_viewInformation);
}

bool RmGLCanvas::DoesRepeat() const
{
    return GetViewInformation().DoesRepeat();
}

bool RmGLCanvas::DoesWrap() const
{
    return GetViewInformation().DoesWrap();
}

bool RmGLCanvas::DoesBoxes() const
{
    return GetViewInformation().DoesBoxes();
}

bool RmGLCanvas::DoesBoxColours() const
{
    return GetViewInformation().DoesBoxColours();
}

bool RmGLCanvas::DoesBoundaries() const
{
    return GetViewInformation().DoesBoundaries();
}

bool RmGLCanvas::IsScoreboardVisible(const ScoreboardStratum& stratum) const
{
    return GetViewInformation().DoesScoreboard(stratum);
}

ScoreboardCoordinator& RmGLCanvas::GetScoreboardCoordinator()
{
    return m_scoreboardCoordinator;
}

const ScoreboardCoordinator& RmGLCanvas::GetScoreboardCoordinator() const
{
    return m_scoreboardCoordinator;
}

BufferChangeResponse RmGLCanvas::SetBoxes(bool fBoxes)
{
    //RootMapLogTrace("RmGLCanvas::SetBoxes");
    if (DoesBoxes() != fBoxes)
    {
        m_boxBoundaries->SetBoxes(fBoxes);
        m_viewInformation->SetBoxes(fBoxes);
        Refresh();
    }

    return (rootmap::bcr_OK);
}


BufferChangeResponse RmGLCanvas::SetBoundaries(bool fBoundaries)
{
    //RootMapLogTrace("RmGLCanvas::SetBoundaries");
    if (DoesBoundaries() != fBoundaries)
    {
        m_boxBoundaries->SetBoundaries(fBoundaries);
        m_viewInformation->SetBoundaries(fBoundaries);
        Refresh();
    }

    return (rootmap::bcr_OK);
}


BufferChangeResponse RmGLCanvas::SetBoxColours(bool fBoxColours)
{
    //RootMapLogTrace("RmGLCanvas::SetBoundaries");
    if (DoesBoxes() != fBoxColours)
    {
        m_boxBoundaries->SetInnerBoxes(fBoxColours);
        m_viewInformation->SetBoxColours(fBoxColours);
        Refresh();
    }

    return (rootmap::bcr_OK);
}

BufferChangeResponse RmGLCanvas::SetProcesses(const ProcessArray& new_processes)
{
    BufferChangeResponse return_val = rootmap::bcr_OK;

    // iterate over current processes, comparing processes which are added and deleted
    ProcessArray current_processes = GetViewInformation().GetDisplayProcesses();
    std::set<Process *> current_process_set(current_processes.begin(), current_processes.end());

    // iterate over the new processes. If we find a process not in the current
    // set, we should add it
    for (ProcessArray::const_iterator npiter(new_processes.begin());
        new_processes.end() != npiter; ++npiter)
    {
        if (current_process_set.end() == current_process_set.find(*npiter))
        {
            LOG_INFO << "Creating ProcessBuffer for " << (*npiter)->GetProcessName();
            CreateProcessBuffer(*npiter);
        }
    }

    // similarly, iterate over the new processes. If we find a process in the
    // current which is not in the new set, we should remove it
    std::set<Process *> new_process_set(new_processes.begin(), new_processes.end());
    for (ProcessArray::const_iterator curriter(current_processes.begin());
        current_processes.end() != curriter; ++curriter)
    {
        if (new_process_set.end() == new_process_set.find(*curriter))
        {
            LOG_INFO << "Removing ProcessBuffer for " << (*curriter)->GetProcessName();
            RemoveProcessBuffer(*curriter);
        }
    }

    // Inform ViewInformation of new display processes and update view
    GetViewInformation().SetDisplayProcesses(new_processes);
    Refresh();
    return return_val;
}


void RmGLCanvas::PostTimestampNotification()
{
    DrawScene();
    SwapBuffers();
}

/*!
 * wxEVT_MOUSEWHEEL event handler for rmID_VIEW_GLCANVAS
 */

void RmGLCanvas::OnMouseWheel(wxMouseEvent& event)
{
    int units_to_scroll = event.GetWheelRotation() / 3; // / event.GetWheelDelta();

    double x, y, z, scale;
    GetTranslation(x, y, z, scale);

    z += (scale * units_to_scroll);
    SetTranslation(x, y, z);
}


/*!
 * wxEVT_CHAR event handler for rmID_VIEW_GLCANVAS
 */

void RmGLCanvas::OnChar(wxKeyEvent& event)
{
    ////@begin wxEVT_CHAR event handler for rmID_VIEW_GLCANVAS in RmGLCanvas.
    // Before editing this code, remove the block markers.
    event.Skip();
    ////@end wxEVT_CHAR event handler for rmID_VIEW_GLCANVAS in RmGLCanvas. 
}
