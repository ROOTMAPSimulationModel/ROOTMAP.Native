#ifndef SimulationActivityWindow_H
#define SimulationActivityWindow_H
/////////////////////////////////////////////////////////////////////////////
// Name:        SimulationActivityWindow.h
// Purpose:     Declaration of the SimulationActivityWindow class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2008-06-16 02:07:36 +0800 (Mon, 16 Jun 2008) $
// $Revision: 1 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "simulation/common/ISimulationActivityObserver.h"

namespace rootmap
{
    class SimulationActivityWindow : public ISimulationActivityObserver
    {
    public:
        SimulationActivityWindow();

        virtual ~SimulationActivityWindow();

        /**
         * Sent to each observer by the SimulationEngine just before the processes
         * are sent the Start message. The simulation is marked as being in
         * "starting" phase at this point
         *
         * @param postOffice reference to the postOffice.
         */
        virtual void PreRunNotification(const PostOffice& postOffice);

        /**
         * Sent to each observer by the SimulationEngine before each timestamp
         * is run.
         *
         * @param postOffice reference to the postOffice.
         */
        virtual void PreTimestampNotification(const PostOffice& postOffice);

        /**
         * May be sent to each observer by the SimulationEngine after each
         * PostOffice message is processed.
         *
         * @param postOffice reference to the postOffice.
         */
        virtual void MidTimestampNotification(const PostOffice& postOffice);

        /**
         * Sent to each observer by the SimulationEngine after each timestamp
         * is complete.  This is the ideal place for window updates to be
         * triggered.
         *
         * Note that "Post" in this sense is not the verb but the prefix,
         * meaning "after" or "later"
         *
         * NOTE that we don't override this method, so that derived classes
         * must override this at least.
         *
         * @param postOffice reference to the postOffice.
         */
         // virtual void PostTimestampNotification(const PostOffice & postOffice);

         /**
          * Sent to each observer by the SimulationEngine just after the processes
          * are sent the Ending message (but before control is returned to the
          * user). The simulation is in "stopping" phase at this point.
          *
          * @param postOffice reference to the postOffice.
          */
        virtual void PostRunNotification(const PostOffice& postOffice);

    private:

        ///
        /// member declaration
    }; // class SimulationActivityWindow
} /* namespace rootmap */

#endif // #ifndef SimulationActivityWindow_H
