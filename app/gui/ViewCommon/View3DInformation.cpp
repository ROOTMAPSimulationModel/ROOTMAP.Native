/////////////////////////////////////////////////////////////////////////////
// Name:        View3DInformation.cpp
// Purpose:     Implementation of the View3DInformation class
// Created:     05-07-2009 22:57:43
// Author:      RvH
// $Date$
// $Revision$
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/ViewCommon/View3DInformation.h"
#include "simulation/data_access/interface/ViewDAI.h"

#include "core/common/Constants.h"

//#include <stdlib.h>

namespace rootmap
{
    View3DInformation::View3DInformation()
        : ViewInformation()
        , m_camera_position(DoubleCoordinate(0, 18, -40))
        , m_scale(1.0)
        , m_root_radius_multiplier(1.0)
        , m_view_cylinders(true)
        , m_view_cones(true)
        , m_colour_mode(RM_COLOUR_BY_BRANCH_ORDER_SHADES)
        , m_base_root_red(0.0)
        , m_base_root_green(0.0)
        , m_base_root_blue(0.0)
    {
    }

    View3DInformation::~View3DInformation()
    {
    }


    View3DInformation::View3DInformation(const View3DInformation& original)
    {
        // uses the assignment operator for this class !!
        *this = original;
    }

    View3DInformation& View3DInformation::operator=(const ViewDAI& data)
    {
        ViewInformation::operator=(data);

        m_camera_position = data.GetReferenceIndex();
        m_scale = data.GetScale();
        m_root_radius_multiplier = data.GetRootRadiusMultiplier();
        m_view_cylinders = data.DoesCylinders();
        m_view_spheres = data.DoesSpheres();
        m_view_cones = data.DoesCones();
        m_stacks_and_slices = data.GetStacksAndSlices();
        m_colour_mode = (RmColouration)data.GetRootColourationMode();
        m_base_root_red = data.GetBaseRootRf();
        m_base_root_green = data.GetBaseRootGf();
        m_base_root_blue = data.GetBaseRootBf();

        return *this;
    }

    View3DInformation& View3DInformation::operator=(const View3DInformation& rhs)
    {
        if (&rhs != this)
        {
            ViewInformation::operator=(rhs);

            m_camera_position = rhs.m_camera_position;
            m_scale = rhs.m_scale;
            m_root_radius_multiplier = rhs.m_root_radius_multiplier;
            m_view_cylinders = rhs.m_view_cylinders;
            m_view_spheres = rhs.m_view_spheres;
            m_view_cones = rhs.m_view_cones;
            m_stacks_and_slices = rhs.m_stacks_and_slices;
            m_colour_mode = rhs.m_colour_mode;
            m_base_root_red = rhs.m_base_root_red;
            m_base_root_green = rhs.m_base_root_green;
            m_base_root_blue = rhs.m_base_root_blue;
        }

        return (*this);
    }
} /* namespace rootmap */
