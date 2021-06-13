#ifndef RmGLCanvas_H
#define RmGLCanvas_H
/////////////////////////////////////////////////////////////////////////////
// Name:        RmGLCanvas.h
// Purpose:     Declaration of the RmGLCanvas class
// Created:     14/03/2008
// Author:      RvH
// $Date: 2009-07-07 02:40:29 +0800 (Tue, 07 Jul 2009) $
// $Revision: 67 $
// Copyright:   ©2002-2008 University of Tasmania
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "RmGLCanvas.h"
#endif

/*!
 * Includes
 */

 ////@begin includes
#include "wx/xrc/xmlres.h"
#include "wx/glcanvas.h"
////@end includes

/*!
 * Forward declarations
 */

 ////@begin forward declarations
class RmGLCanvas;

////@end forward declarations

namespace rootmap
{
    class Process;
    class IView3DCoordinator;
    class ProcessVertexBuffer;
    class ScoreboardStratum;
    class ScoreboardCoordinator;
    class ScoreboardBoxBoundaryVertices;
}

/*!
 * Control identifiers
 */

 ////@begin control identifiers
#define rmID_VIEW_GLCANVAS 10013
#define SYMBOL_RMGLCANVAS_STYLE wxNO_BORDER
#define SYMBOL_RMGLCANVAS_IDNAME rmID_VIEW_GLCANVAS
#define SYMBOL_RMGLCANVAS_SIZE wxDefaultSize
#define SYMBOL_RMGLCANVAS_POSITION wxDefaultPosition
////@end control identifiers

#include "app/gui/View3DWindow/RmView3D.h"
#include "app/gui/View3DWindow/OpenGLCanvas.h"
#include "app/gui/ViewCommon/Types.h"
#include "core/common/structures.h"
#include "core/log/Logger.h"
#include "core/common/DoubleCoordinates.h"
#include <list>


/*!
 * RmGLCanvas class declaration
 */

class RmGLCanvas : public OpenGLCanvas
{
    DECLARE_CLASS(RmGLCanvas)
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    RmGLCanvas(wxWindow* parent, rootmap::IView3DCoordinator* viewCoordinator, rootmap::ScoreboardCoordinator& scoreboardcoordinator, const rootmap::View3DInformation& viewInfo, wxWindowID id = SYMBOL_RMGLCANVAS_IDNAME, const wxPoint& pos = SYMBOL_RMGLCANVAS_POSITION, const wxSize& size = SYMBOL_RMGLCANVAS_SIZE, long style = SYMBOL_RMGLCANVAS_STYLE, const wxString& name = wxT("GLCanvas"), int* attribs = __nullptr);

    /// Creation
    bool Create(wxWindow* parent, rootmap::IView3DCoordinator* viewCoordinator, rootmap::ScoreboardCoordinator& scoreboardcoordinator, const rootmap::View3DInformation& viewInfo, wxWindowID id = SYMBOL_RMGLCANVAS_IDNAME, const wxPoint& pos = SYMBOL_RMGLCANVAS_POSITION, const wxSize& size = SYMBOL_RMGLCANVAS_SIZE, long style = SYMBOL_RMGLCANVAS_STYLE, const wxString& name = wxT("GLCanvas"), int* attribs = __nullptr);

    /// Destructor
    ~RmGLCanvas();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls(rootmap::IView3DCoordinator* viewCoordinator, rootmap::ScoreboardCoordinator& scoreboardcoordinator, const rootmap::View3DInformation& viewInfo);

    ////@begin RmGLCanvas event handler declarations

    /// wxEVT_MOUSEWHEEL event handler for rmID_VIEW_GLCANVAS
    void OnMouseWheel(wxMouseEvent& event);

    /// wxEVT_CHAR event handler for rmID_VIEW_GLCANVAS
    void OnChar(wxKeyEvent& event);

    ////@end RmGLCanvas event handler declarations

    ////@begin RmGLCanvas member function declarations
        /// Retrieves bitmap resources
    wxBitmap GetBitmapResource(const wxString& name);

    /// Retrieves icon resources
    wxIcon GetIconResource(const wxString& name);
    ////@end RmGLCanvas member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

    /**
     * Sets up OpenGL viewport and GL_PROJECTION matrix. This will be invoked whenever the
     * OpenGL canvas is resized, including when it is first visible.
     */
    virtual void DoResize();

    /**
     *    MSA 09.11.27 New convenience method for setting the 3D view to vTop/vFront/vSide
     *    (equivalent to the 2D view's selectable viewpoints)
     */
    void SetViewpoint(const rootmap::ViewDirection& vDir);

    const rootmap::View3DInformation& GetViewInformation() const;
    rootmap::View3DInformation& GetViewInformation();

    bool DoesRepeat() const;
    bool DoesWrap() const;
    bool DoesBoxes() const;
    bool DoesBoxColours() const;
    bool DoesBoundaries() const;
    bool IsScoreboardVisible(const rootmap::ScoreboardStratum& stratum) const;

    virtual rootmap::BufferChangeResponse SetBoxes(bool fBoxes);
    virtual rootmap::BufferChangeResponse SetBoundaries(bool fBoundaries);
    virtual rootmap::BufferChangeResponse SetBoxColours(bool fBoxColours);
    virtual rootmap::BufferChangeResponse SetProcesses(const rootmap::ProcessArray& new_processes);
    //virtual rootmap::BufferChangeResponse SetScoreboard(const rootmap::ScoreboardStratum & stratum, bool fTurnOn);
    //virtual rootmap::BufferChangeResponse SetScoreboards(const rootmap::ScoreboardFlags & flags);


    rootmap::ScoreboardCoordinator& GetScoreboardCoordinator();
    const rootmap::ScoreboardCoordinator& GetScoreboardCoordinator() const;

    virtual rootmap::BufferChangeResponse CreateProcessBuffers(const rootmap::View3DInformation& viewInfo);
    virtual rootmap::BufferChangeResponse CreateProcessBuffer(rootmap::Process* process);
    virtual void RemoveProcessBuffer(rootmap::Process* process);

    void PostTimestampNotification();

private:
    /**
     * Renders the OpenGL scene.
     */
    virtual void DrawScene();

    /**
     * Test object
     */
    void DrawCube(GLdouble size);

    RootMapLoggerDeclaration();

    /**
     *
     */
    rootmap::View3DInformation* m_viewInformation;

    /**
     *
     */
    rootmap::ScoreboardCoordinator& m_scoreboardCoordinator;

    /**
     * Scoreboard Box Boundary Buffer
     */
    rootmap::ScoreboardBoxBoundaryVertices* m_boxBoundaries;

    /**
     * type definition for a list of Process Buffers
     */
    typedef std::list<rootmap::ProcessVertexBuffer *> ProcessBufferContainer;

    /**
     * the drawing buffers
     */
    ProcessBufferContainer m_processBuffers;

    rootmap::DoubleBox m_scoreboardBounds;

    ////@begin RmGLCanvas member variables
    ////@end RmGLCanvas member variables
};

#endif // #ifndef RmGLCanvas_H
