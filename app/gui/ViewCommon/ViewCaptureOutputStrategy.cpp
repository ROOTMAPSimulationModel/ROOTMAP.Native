/////////////////////////////////////////////////////////////////////////////
// Name:        ViewCaptureOutputStrategy.cpp
// Purpose:     Definition of the ViewCaptureOutputStrategy class
// Created:     21/09/2009
// Author:      MSA
// $Date: 2009-09-21 02:07:36 +0800 (Mon, 21 Sep 2009) $
// $Revision: 1 $
// Copyright:   ©2009 University of Tasmania/University of Western Australia
//
// NOTE: Breaks file-placement paradigm in the same manner and for
// the same reasons as ProcessDrawing.
//
/////////////////////////////////////////////////////////////////////////////

#include "simulation/file/output/ViewCaptureOutputStrategy.h"
#include "simulation/process/interprocess_communication/PostOffice.h"
#include "simulation/file/output/BinaryDataOutputFile.h"
#include "core/common/RmAssert.h"
#include "simulation/file/output/OutputRule.h"
#include "app/gui/ViewCommon/ViewCaptor.h"
#include "app/gui/common/GuiSimulationEngine.h"
#include "app/gui/View3DWindow/RmView3D.h"
#include "app/gui/ViewWindow/RmViewWindow.h"
#include "app/gui/common/SimulationActivityWindow.h"

#include <typeinfo>

namespace rootmap
{
    RootMapLoggerDefinition(ViewCaptureOutputStrategy);

    ViewCaptureOutputStrategy::ViewCaptureOutputStrategy(SimulationEngine* septr, const ViewType vt, const size_t& viewWindowIndex, const wxString& fileExt)
        : m_simulationEngine(septr)
        , m_viewType(vt)
        , m_viewWindowIndex(viewWindowIndex)
        , m_fileExt(fileExt)
        , m_viewCaptor(__nullptr)
    {
        RootMapLoggerInitialisation("rootmap.ViewCaptureOutputStrategy");
    }

    ViewCaptureOutputStrategy::~ViewCaptureOutputStrategy()
    {
        wxDELETE(m_viewCaptor);
    }

    void ViewCaptureOutputStrategy::ArrangeOutput(ProcessActionDescriptor* action, OutputRule* output_rule)
    {
        Use_PostOffice;

        GuiSimulationEngine* gse = dynamic_cast<GuiSimulationEngine*>(m_simulationEngine);
        if (!gse) { return; } // Bail out if SimulationEngine is not a GuiSimulationEngine

        // MSA 09.10.07 We must create a NEW ViewCaptor each time, as the instances of each view may be different
        // at any time (e.g. 2D View closed, a new 3D View spawned to get multiple angles on the simulation, etc etc)

        if (m_viewCaptor)
            wxDELETE(m_viewCaptor);

        if (VIEW_2D == m_viewType)
        {
            if (m_viewWindowIndex < gse->Get2DViews().size())
            {
                m_viewCaptor = new ViewCaptor(gse->Get2DViews()[m_viewWindowIndex], __nullptr);
            }
            else
            {
                LOG_ERROR << "Could not arrange ViewCapture output for time " << action->GetTime() << ": Invalid 2D View window index provided in ViewCapture output rule. 2D view window may have been closed.";
                return;
            }
        }
        else
        {
            if (m_viewWindowIndex < gse->Get3DViews().size())
            {
                m_viewCaptor = new ViewCaptor(__nullptr, gse->Get3DViews()[m_viewWindowIndex]);
            }
            else
            {
                LOG_ERROR << "Could not arrange ViewCapture output for time " << action->GetTime() << ": Invalid 3D View window index provided in ViewCapture output rule. 3D view window may have been closed.";
                return;
            }
        }

        postoffice->sendMessage(m_viewCaptor, m_viewCaptor, kSpecialOutputDataMessage, output_rule);
    }

    void ViewCaptureOutputStrategy::OutputToFile(DataOutputFile& file)
    {
        // ViewCaptor::CaptureView returns (hands over ownership of)
        // a pointer to a wxBitmap in heap memory,
        // or NULL if the view is not visible
        const wxBitmap* bitmapOnHeap = m_viewCaptor->CaptureView(m_viewType == VIEW_3D ? 3 : 2);

        if (bitmapOnHeap)
        {
            BinaryDataOutputFile bfile = file;
            bfile.WriteBitmap(bitmapOnHeap, m_fileExt);

            delete bitmapOnHeap;
        }
    }
} /* namespace rootmap */
