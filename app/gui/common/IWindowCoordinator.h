#ifndef IWindowCoordinator_H
#define IWindowCoordinator_H
/////////////////////////////////////////////////////////////////////////////
// Name:        IWindowCoordinator.h
// Purpose:     Declaration of the IWindowCoordinator class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2008-11-18 12:45:09 +0900 (Tue, 18 Nov 2008) $
// $Revision: 31 $
// Copyright:   ©2006 University of Tasmania
//
// IWindowCoordinator is the base interface for the coordinator of window
// classes.
/////////////////////////////////////////////////////////////////////////////
#include "simulation/common/Types.h"
#include "core/scoreboard/ScoreboardBox.h"

#include "wx/arrstr.h"

#include <set>
#include <vector>

namespace rootmap
{
    class Process;

    class IWindowCoordinator
    {
    public:
        /**
         * Used to retrieve the list of characteristics, ordered by characteristic index,
         * for displaying in the characteristic selection dialog
         *
         * @param
         */
        virtual void populateCharacteristicStringArray(wxArrayString& characteristics) = 0;

        /**
         * Used by the ProcessSelection dialog to populate its list.
         */
        virtual void populateProcessStringArray(wxArrayString& processes, std::vector<ProcessIdentifier>& processIds, PopulateProcessStringFlags flags = ppsf_All) = 0;

        /**
         * Used to determine the process and characteristic number of the newly selected
         * characteristic
         *
         * @param
         */
        virtual Process* findProcessCharacteristicNumber(CharacteristicIndex characteristic_index, long& characteristic_number) = 0;

        /**
         * Used to determine the process and characteristic number of the newly selected
         * characteristic
         *
         * @param
         */
        virtual Process* findProcessCharacteristicNumber(const wxString& characteristic_name, long& characteristic_number) = 0;

        /**
         * Essentially converts a set of ProcessIdentifiers to a vector of Process pointers
         *
         * Used by the View Window when the Process Selection dialog updates according to
         * currently selected processes in the dialog
         */
        virtual void findProcessesByProcessId(const ProcessIdentifierSet& processIds, ProcessArray& displayProcesses) = 0;

        virtual ~IWindowCoordinator()
        {
        }

    protected:
        IWindowCoordinator()
        {
        }
    }; // class IWindowCoordinator
} /* namespace rootmap */

#endif // #ifndef IWindowCoordinator_H
