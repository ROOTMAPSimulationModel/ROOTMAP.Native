#ifndef IViewCoordinator_H
#define IViewCoordinator_H
/////////////////////////////////////////////////////////////////////////////
// Name:        IViewCoordinator.h
// Purpose:     Declaration of the IViewCoordinator class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2008-11-18 12:45:09 +0900 (Tue, 18 Nov 2008) $
// $Revision: 31 $
// Copyright:   ©2006 University of Tasmania
//
// IViewCoordinator is the interface for a class which is able to create
// View windows from the ViewDAI
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/common/IWindowCoordinator.h"

class RmViewWindow;

namespace rootmap
{
    class ViewInformation;

    class IViewCoordinator : virtual public IWindowCoordinator
    {
    public:
        /**
         * Called by the DataAccessManager::visualiseViews for each view that
         * has configuration data
         *
         * @param
         */
        virtual void updateViewInformation(const ViewInformation& viewInfo) = 0;

        /**
         *
         */
        virtual void removeViewWindow(RmViewWindow* viewWin) = 0;

        virtual ~IViewCoordinator()
        {
        }

    protected:
        IViewCoordinator()
        {
        }
    }; // class IViewCoordinator
} /* namespace rootmap */

#endif // #ifndef IViewCoordinator_H
