#ifndef IViewProcessSelectionCallback_H
#define IViewProcessSelectionCallback_H
/////////////////////////////////////////////////////////////////////////////
// Name:        IViewProcessSelectionCallback.h
// Purpose:     Declaration of the IViewProcessSelectionCallback class
// Created:     24-06-2006 02:15:33
// Author:      RvH
// $Date: 2008-11-18 12:45:09 +0900 (Tue, 18 Nov 2008) $
// $Revision: 31 $
// Copyright:   ©2002-2008 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "simulation/common/Types.h"
#include <set>

namespace rootmap
{
    class IViewProcessSelectionCallback
    {
    public:
        virtual ~IViewProcessSelectionCallback()
        {
        }

        /**
         * Stuff about the what the function does relative to the scheme of things.
         * A good temporary place for design-level documentation.
         *
         * @param
         * @return
         */
        virtual void UpdateProcesses(const ProcessIdentifierSet& processes) = 0;

    protected:
        /**
         * Since this is an abstract class, only should be able to instantiate
         * via derived class constructor
         */
        IViewProcessSelectionCallback()
        {
        }

    private:

        ///
        /// member declaration
    }; // class IViewProcessSelectionCallback
} /* namespace rootmap */

#endif // #ifndef IViewProcessSelectionCallback_H
