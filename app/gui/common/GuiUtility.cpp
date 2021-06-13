/////////////////////////////////////////////////////////////////////////////
// Name:        GuiUtility.cpp
// Purpose:     Implementation of the GuiUtility class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2008-06-16 02:07:36 +0800 (Mon, 16 Jun 2008) $
// $Revision: 1 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/common/GuiUtility.h"

namespace rootmap
{
    Dimension GuiUtility::getDimensionForRows(ViewDirection viewDir)
    {
        rootmap::Dimension dim = Z; // works for ViewDirection = vFront and vSide
        if (viewDir == vTop)
        {
            dim = Y;
        }
        return dim;
    }


    Dimension GuiUtility::getDimensionForColumns(ViewDirection viewDir)
    {
        rootmap::Dimension dim = X; // works for ViewDirection = vFront and vTop
        if (viewDir == vSide)
        {
            dim = Y;
        }
        return dim;
    }
} /* namespace rootmap */
