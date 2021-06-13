/////////////////////////////////////////////////////////////////////////////
// Name:        ViewCaptor.cpp
// Purpose:     Definition of the ViewCaptor class
// Created:     22/09/2009
// Author:      MSA
// $Date: 2009-09-22 02:07:36 +0800 (Tue, 22 Sep 2009) $
// $Revision: 1 $
// Copyright:   ©2009 University of Tasmania/University of Western Australia
//
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/ViewCommon/ViewCaptor.h"

namespace rootmap
{
    RootMapLoggerDefinition(ViewCaptor);

    // Private default constructor solely for IMPLEMENT_DYNAMIC_CLASS above; do not call directly.
    ViewCaptor::ViewCaptor()
        : m_2DView(__nullptr)
        , m_3DView(__nullptr)
    {
    }

    ViewCaptor::ViewCaptor(RmViewWindow* view2D, RmView3D* view3D)
        : m_2DView(view2D)
        , m_3DView(view3D)
    {
        RootMapLoggerInitialisation("rootmap.ViewCaptor");
    }

    ViewCaptor::~ViewCaptor()
    {
    }

    const wxBitmap* ViewCaptor::CaptureView(unsigned char twoOrThreeD)
    {
        if (2 == twoOrThreeD)
        {
            RmAssert(m_2DView, "Error: 2D view for screen capture not found");
            return m_2DView->GetBitmap();
        }
        else if (3 == twoOrThreeD)
        {
            RmAssert(m_3DView, "Error: 3D view for screen capture not found");
            return m_3DView->GetBitmap();
        }
        else
        {
            RmAssert(false, "Invalid number of dimensions supplied. Can only capture 2D or 3D view");
            return __nullptr; // To shut the compiler up
        }
    }
} /* namespace rootmap */
