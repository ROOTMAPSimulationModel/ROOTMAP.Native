#ifndef ITableCoordinator_H
#define ITableCoordinator_H
/////////////////////////////////////////////////////////////////////////////
// Name:        ITableCoordinator.h
// Purpose:     Declaration of the ITableCoordinator class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2008-06-16 02:07:36 +0800 (Mon, 16 Jun 2008) $
// $Revision: 1 $
// Copyright:   ©2006 University of Tasmania
//
// ITableCoordinator is the interface for a class which is able to create
// Table windows from the TableDAI
/////////////////////////////////////////////////////////////////////////////
#include "wx/arrstr.h"
#include "app/gui/common/IWindowCoordinator.h"

class RmTableWindow;

namespace rootmap
{
    class TableInformation;

    class ITableCoordinator : virtual public IWindowCoordinator
    {
    public:
        /**
         * Used to update the table coordinator's current default table information to
         * that used by this latest change in the frontmost table window
         *
         * @param
         */
        virtual void updateTableInformation(const TableInformation& tableInfo) = 0;

        virtual void removeTableWindow(RmTableWindow* tableWin) = 0;

        virtual ~ITableCoordinator()
        {
        }

    protected:
        ITableCoordinator()
        {
        }
    }; // class ITableCoordinator
} /* namespace rootmap */

#endif // #ifndef ITableCoordinator_H
