#ifndef AppTypes_H
#define AppTypes_H

/////////////////////////////////////////////////////////////////////////////
// Name:        AppTypes.h
// Purpose:     Provides type definitions, structures and enumerations used by
//              application-level code
// Created:     06/05/2006
// Author:      RvH
// $Date: 2008-06-16 02:07:36 +0800 (Mon, 16 Jun 2008) $
// $Revision: 1 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
namespace rootmap
{
    enum ValidationReply
    {
        vrNONE = 0,
        vrRestrict = 1,
        vrAllow = 2,
        vrDontChange = 3,
        vrCancel = vrDontChange,
        vrTryAgain = 4
    };

    enum MoveDirection
    {
        mdNONE = 0,
        mdUp,
        mdDown,
        mdLeft,
        mdRight
    };

    ///
    /// view = normal View window whose contents can be reconstructed from current data.
    /// table = normal table window whose contents    "    "    "    "    "    "    "    "
    /// status = normal status window whose contents        "    "    "    "    "    "
    /// lockedview = View whose contents depend on a previous state of the scoreboard and processs.
    ///             Thus, the info is stored as a PICT/BMP, and cannot be fiddled with. The PICT info
    ///             might yet be embedded as data in the same file, or in another file altogether. */
    enum WindowType
    {
        windowNONE = 0,
        windowView,
        windowLockedview,
        windowTable,
        windowStatus,
        windowQuickStatus,
        windowPlantBrowser,
        windowPlantTypeBrowser,
        windowPlantSummaryBrowser,
        windowIOBrowser,
        windowCollectedErrors,
        window3DIsometric,
        window3DOpenGL,
    };

    ///
    /// anonymous enumeration to hold the index numbers of menus
    /// In actual fact this is a hack - by the power of xrc the order of menus
    /// in the menubar can theoretically be changed, however we'll just have to
    /// prevent that. The user/internationaliser can still rename the menus, but
    /// not shift their position.
    enum
    {
        RootMapMenuIndex_File = 0,
        RootMapMenuIndex_Edit = 1,
        RootMapMenuIndex_Run = 2,
        RootMapMenuIndex_View = 3,
        RootMapMenuIndex_Table = 4,
        RootMapMenuIndex_Window = 5,
        RootMapMenuIndex_Help = 6,

        RootMapMenus_NumberOf = 6 // for some reason, ignore the Window menu which is placed by wxWidgets
    };

    enum
    {
        TableMenuItemIndex_NewSubmenu = 1,
        TableMenuItemIndex_LayerSubmenu = 2,
        TableMenuItemIndex_PreviousLayer = 3,
        TableMenuItemIndex_NextLayer = 4,
        TableMenuItemIndex_Front = 5,
        TableMenuItemIndex_Side = 6,
        TableMenuItemIndex_Top = 7,
        TableMenuItemIndex_Characteristic = 8,
        TableMenuItemIndex_Details = 9
    };
} /* namespace rootmap */

#endif // #ifndef AppTypes_H
