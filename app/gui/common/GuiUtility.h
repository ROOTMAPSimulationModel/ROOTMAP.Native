#ifndef GuiUtility_H
#define GuiUtility_H
/////////////////////////////////////////////////////////////////////////////
// Name:        GuiUtility.h
// Purpose:     Declaration of the GuiUtility class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2008-06-16 02:07:36 +0800 (Mon, 16 Jun 2008) $
// $Revision: 1 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "core/common/Structures.h"
#include "core/scoreboard/ScoreboardStratum.h"
#include "core/scoreboard/Dimension.h"

namespace rootmap
{
    class GuiUtility
    {
    public:

        /**
         * getDimensionForRows
         *
         * Stuff about the what the function does relative to the scheme of things.
         * A good temporary place for design-level documentation.
         *
         * @param viewDir the direction the table or view window is viewed from
         * @return the dimension along that axis, to use in
         */
        static Dimension getDimensionForRows(ViewDirection viewDir);

        static Dimension getDimensionForColumns(ViewDirection viewDir);

    private:
        // Hidden (==disabled)default constructor, destructor, copy constructor, assignment operator
        GuiUtility();
        ~GuiUtility();
        GuiUtility(const GuiUtility&);
        GuiUtility& operator=(const GuiUtility&);
    }; // class GuiUtility
} /* namespace rootmap */

#endif // #ifndef GuiUtility_H
