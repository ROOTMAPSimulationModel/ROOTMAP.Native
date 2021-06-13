#ifndef ViewCaptor_H
#define ViewCaptor_H
/////////////////////////////////////////////////////////////////////////////
// Name:        ViewCaptor.h
// Purpose:     Declaration of the ViewCaptor class
// Created:     22/09/2009
// Author:      MSA
// $Date: 2009-09-22 02:07:36 +0800 (Tue, 22 Sep 2009) $
// $Revision: 1 $
// Copyright:   ©2009 University of Tasmania/University of Western Australia
/////////////////////////////////////////////////////////////////////////////

#include "simulation/process/common/Process.h"
#include "app/gui/ViewWindow/RmViewWindow.h"
#include "app/gui/View3DWindow/RmView3D.h"
#include "wx/bitmap.h"

/**
 *    Helper Process for ViewCaptureOutputStrategy
 */

namespace rootmap
{
    class ViewCaptor : public Process
    {
    public:

        ViewCaptor(RmViewWindow* view2D, RmView3D* view3D);
        virtual ~ViewCaptor();
        const wxBitmap* CaptureView(unsigned char twoOrThreeD);

    private:
        // Default constructor for dynamic class stuff - do not use
        ViewCaptor();
        RootMapLoggerDeclaration();

        RmViewWindow* m_2DView;
        RmView3D* m_3DView;
    }; // class ViewCaptor
} /* namespace rootmap */

#endif // #ifndef ViewCaptor_H
