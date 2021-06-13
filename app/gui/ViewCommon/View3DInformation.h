#ifndef View3DInformation_H
#define View3DInformation_H
/////////////////////////////////////////////////////////////////////////////
// Name:        View3DInformation.h
// Purpose:     Declaration of the View3DInformation class
// Created:     05-07-2009 22:57:43
// Author:      RvH
// $Date$
// $Revision$
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/ViewCommon/ViewInformation.h"
#include "app/gui/ViewCommon/RootColourationModeUtility.h"

#include <vector>

namespace rootmap
{
    class View3DInformation : public ViewInformation
    {
    public:
        // default constructor and destructor
        View3DInformation();
        ~View3DInformation();

        // copy constructor
        View3DInformation(const View3DInformation& original);
        // assignment from View DAI
        View3DInformation& operator=(const ViewDAI& data);
        // assignment operator (standard)
        View3DInformation& operator=(const View3DInformation& rhs);

        // Accessors
        const DoubleCoordinate GetCameraPosition() const;
        const double GetScale() const;
        const double GetRootRadiusMultiplier() const;
        bool DoesCylinders() const;
        bool DoesSpheres() const;
        bool DoesCones() const;
        const size_t GetStacksAndSlices() const;
        const RmColouration& GetRootColourMode() const;
        const float GetBaseRootRf() const;
        const float GetBaseRootGf() const;
        const float GetBaseRootBf() const;

        // Mutators
        void SetCameraPosition(const DoubleCoordinate& newCameraPosition);
        void SetRootRadiusMultiplier(double multiplier);
        void SetCylinders(bool v);
        void SetSpheres(bool v);
        void SetCones(bool v);
        void SetStacksAndSlices(const size_t sz);
        void SetRootColourMode(const RmColouration& mode);
        void SetBaseRootRf(const float red);
        void SetBaseRootGf(const float green);
        void SetBaseRootBf(const float blue);


    private:

        DoubleCoordinate m_camera_position; // default = (0.0, 18.0, -40.0)
        double m_scale; // default = 1.0;
        double m_root_radius_multiplier; // default = 1.0
        bool m_view_cylinders; // default = true
        bool m_view_spheres; // default = false
        bool m_view_cones; // default = false
        size_t m_stacks_and_slices; // default = 4
        RmColouration m_colour_mode; // default = RM_COLOUR_BY_BRANCH_ORDER
        float m_base_root_red; // default = 0.0
        float m_base_root_green; // default = 0.0
        float m_base_root_blue; // default = 0.0
    };


    inline const DoubleCoordinate View3DInformation::GetCameraPosition() const { return m_camera_position; }
    inline const double View3DInformation::GetScale() const { return m_scale; }
    inline const double View3DInformation::GetRootRadiusMultiplier() const { return m_root_radius_multiplier; }
    inline bool View3DInformation::DoesCylinders() const { return m_view_cylinders; }
    inline bool View3DInformation::DoesSpheres() const { return m_view_spheres; }
    inline bool View3DInformation::DoesCones() const { return m_view_cones; }
    inline const RmColouration& View3DInformation::GetRootColourMode() const { return m_colour_mode; }
    inline const size_t View3DInformation::GetStacksAndSlices() const { return m_stacks_and_slices; }
    inline const float View3DInformation::GetBaseRootRf() const { return m_base_root_red; }
    inline const float View3DInformation::GetBaseRootGf() const { return m_base_root_green; }
    inline const float View3DInformation::GetBaseRootBf() const { return m_base_root_blue; }

    inline void View3DInformation::SetRootRadiusMultiplier(double multiplier) { m_root_radius_multiplier = multiplier; }
    inline void View3DInformation::SetCylinders(bool v) { m_view_cylinders = v; }
    inline void View3DInformation::SetSpheres(bool v) { m_view_spheres = v; }
    inline void View3DInformation::SetCones(bool v) { m_view_cones = v; }
    inline void View3DInformation::SetStacksAndSlices(const size_t sz) { m_stacks_and_slices = sz; }
    inline void View3DInformation::SetRootColourMode(const RmColouration& mode) { m_colour_mode = mode; }
    inline void View3DInformation::SetBaseRootRf(const float red) { m_base_root_red = red; }
    inline void View3DInformation::SetBaseRootGf(const float green) { m_base_root_green = green; }
    inline void View3DInformation::SetBaseRootBf(const float blue) { m_base_root_blue = blue; }
} /* namespace rootmap */

#endif // #ifndef View3DInformation_H
