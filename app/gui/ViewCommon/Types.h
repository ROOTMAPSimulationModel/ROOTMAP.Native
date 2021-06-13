#ifndef ViewCommonTypes_H
#define ViewCommonTypes_H

/////////////////////////////////////////////////////////////////////////////
// Name:        Types.h
// Purpose:     Declaration of the Types class
// Created:     21-03-2008 18:23:29
// Author:      RvH
// $Date: 2009-07-07 02:40:29 +0800 (Tue, 07 Jul 2009) $
// $Revision: 67 $
// Copyright:   ©2002-2008 University of Tasmania
/////////////////////////////////////////////////////////////////////////////

namespace rootmap
{
    /**
     * Determines how much of a buffer will be rebuilt
     */
    enum BufferBuildLevel
    {
        bbl_NONE = 0,
        bbl_Reslap = 1,
        bbl_Redraw = 2,
        bbl_Complete = 3,
        bbl_MinimumRedrawLevel = bbl_Reslap
    };


    /**
     * Used for indicating to client routines the success or otherwise of
     * changing a view buffer
     */
    enum BufferChangeResponse
    {
        bcr_NONE = 0L,
        bcr_OK,
        bcr_Cancel,
        bcr_TryAgain,
        bcr_PartialFailure,
        bcr_Failure
    };

    /**
     * Enumeration to indicate routine used to
     */
    enum RmColouration
    {
        RM_COLOUR_STATIC = 0,
        RM_COLOUR_BY_BRANCH_ORDER_SHADES = 1,
        RM_COLOUR_BY_BRANCH_ORDER_HIGH_CONTRAST = 2,

        //RvH: new unwritten-coding-standards-compliant enum names,
        // with transitional equivalence
        rc_ColourStatic = RM_COLOUR_STATIC,
        rc_ColourByBranchOrderShades = RM_COLOUR_BY_BRANCH_ORDER_SHADES,
        rc_ColourByBranchOrderHighContrast = RM_COLOUR_BY_BRANCH_ORDER_HIGH_CONTRAST
        //rc_Colour = ,
    };
} /* namespace rootmap */

#endif // #ifndef ViewCommonTypes_H
