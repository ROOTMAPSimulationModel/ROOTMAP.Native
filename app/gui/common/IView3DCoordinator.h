#ifndef IView3DCoordinator_H
#define IView3DCoordinator_H
/////////////////////////////////////////////////////////////////////////////
// Name:        IView3DCoordinator.h
// Purpose:     Declaration of the IView3DCoordinator class
// Created:     18-06-2008 17:06:58
// Author:      RvH
// $Date: 2008-06-20 10:30:16 +0800 (Fri, 20 Jun 2008) $
// $Revision: 7 $
// Copyright:   ©2006 University of Tasmania
//
// IView3DCoordinator is the interface for a class which is able to create
// View windows from the ViewDAI
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/common/IWindowCoordinator.h"

class RmView3D;

namespace rootmap
{
    class ViewInformation;

    class IView3DCoordinator : virtual public IWindowCoordinator
    {
    public:
        /**
         * Called by the DataAccessManager::visualiseViews for each view that
         * has configuration data
         *
         * @param
         */
        virtual void updateView3DInformation(const ViewInformation& viewInfo) = 0;

        virtual void removeView3DWindow(RmView3D* viewWin) = 0;

        virtual ~IView3DCoordinator()
        {
        }

    protected:
        IView3DCoordinator()
        {
        }
    }; // class IView3DCoordinator
} /* namespace rootmap */

#endif // #ifndef IView3DCoordinator_H
