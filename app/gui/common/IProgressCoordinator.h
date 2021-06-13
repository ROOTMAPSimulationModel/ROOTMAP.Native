#ifndef IProgressCoordinator_H
#define IProgressCoordinator_H
/////////////////////////////////////////////////////////////////////////////
// Name:        IProgressCoordinator.h
// Purpose:     Declaration of the IProgressCoordinator class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2008-06-16 02:07:36 +0800 (Mon, 16 Jun 2008) $
// $Revision: 1 $
// Copyright:   ©2006 University of Tasmania
//
// IProgressCoordinator is the interface for a class which is coordinates
// the progress window[s]
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/common/IWindowCoordinator.h"

class RmProgressWindow;

namespace rootmap
{
    class IProgressCoordinator : virtual public IWindowCoordinator
    {
    public:
        /**
         * Used to update the progress coordinator's current default progress information to
         * that used by this latest change in the frontmost progress window
         *
         * @param
         */
        virtual void removeProgressWindow(RmProgressWindow* progressWin) = 0;

        virtual ~IProgressCoordinator()
        {
        }

    protected:
        IProgressCoordinator()
        {
        }
    }; // class IProgressCoordinator
} /* namespace rootmap */

#endif // #ifndef IProgressCoordinator_H
