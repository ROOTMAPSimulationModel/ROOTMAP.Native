#ifndef ProcessHelper_H
#define ProcessHelper_H
/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessHelper.h
// Purpose:     Declaration of the ProcessHelper class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2008-06-16 02:07:36 +0800 (Mon, 16 Jun 2008) $
// $Revision: 1 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/common/Types.h"
#include "core/common/Types.h"

//#include "simulation/process/common/Process.h"

namespace rootmap
{
    class Process;


    class ProcessHelper
    {
    public:
        ProcessHelper(Process* p);

        virtual ~ProcessHelper();

        /**
         * ValidateCharacteristicInput
         *
         * Used to determine if the input value is valid for a particular
         * characteristic. If the value is out of the defined range, we put up
         * an alert asking the user for a choice of actions :
         * Constrain    : the value is changed to the Maximum allowed value if
         *                it was above, or the Minimum value if it was below.
         * Allow        : The value is returned unchanged.
         * Don't Change : The value is returned unchanged. The calling routine
         *                should then ensure that the value previously in that
         *                box should remain unaltered.
         * Try Again    : The value is returned unchanged. The chain of calling
         *                should then allow the user to STAY AND RE-EDIT the
         *                value.
         *
         * @param
         * @param
         * @return
         */
        ValidationReply ValidateCharacteristicInput(long characteristic_number, double& input_value);

    private:

        ///
        /// member declaration
        Process* m_process;
    }; // class ProcessHelper
} /* namespace rootmap */

#endif // #ifndef ProcessHelper_H
