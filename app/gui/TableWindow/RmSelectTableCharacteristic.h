/////////////////////////////////////////////////////////////////////////////
// Name:        RmSelectTableCharacteristic.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     10/02/2007 18:07:28
// RCS-ID:      
// Copyright:   ©2006 University of Tasmania
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _RMSELECTTABLECHARACTERISTIC_H_
#define _RMSELECTTABLECHARACTERISTIC_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "RmSelectTableCharacteristic.h"
#endif

/*!
 * Includes
 */

 ////@begin includes
#include "wx/xrc/xmlres.h"
////@end includes

/*!
 * Forward declarations
 */

 ////@begin forward declarations
 ////@end forward declarations

 /*!
  * Control identifiers
  */

  ////@begin control identifiers
#define rmID_SELECTTABLECHARACTERISTIC_DIALOG 10006
#define SYMBOL_RMSELECTTABLECHARACTERISTIC_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_RMSELECTTABLECHARACTERISTIC_TITLE _("Select Table Characteristic")
#define SYMBOL_RMSELECTTABLECHARACTERISTIC_IDNAME rmID_SELECTTABLECHARACTERISTIC_DIALOG
#define SYMBOL_RMSELECTTABLECHARACTERISTIC_SIZE wxSize(900, 975)
#define SYMBOL_RMSELECTTABLECHARACTERISTIC_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

 /*!
  * RmSelectTableCharacteristic class declaration
  */

class RmSelectTableCharacteristic : public wxDialog
{
    DECLARE_DYNAMIC_CLASS(RmSelectTableCharacteristic)
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    RmSelectTableCharacteristic();
    RmSelectTableCharacteristic(wxWindow* parent, wxWindowID id = SYMBOL_RMSELECTTABLECHARACTERISTIC_IDNAME, const wxString& caption = SYMBOL_RMSELECTTABLECHARACTERISTIC_TITLE, const wxPoint& pos = SYMBOL_RMSELECTTABLECHARACTERISTIC_POSITION, const wxSize& size = SYMBOL_RMSELECTTABLECHARACTERISTIC_SIZE, long style = SYMBOL_RMSELECTTABLECHARACTERISTIC_STYLE);

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = SYMBOL_RMSELECTTABLECHARACTERISTIC_IDNAME, const wxString& caption = SYMBOL_RMSELECTTABLECHARACTERISTIC_TITLE, const wxPoint& pos = SYMBOL_RMSELECTTABLECHARACTERISTIC_POSITION, const wxSize& size = SYMBOL_RMSELECTTABLECHARACTERISTIC_SIZE, long style = SYMBOL_RMSELECTTABLECHARACTERISTIC_STYLE);

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

    ////@begin RmSelectTableCharacteristic event handler declarations

    ////@end RmSelectTableCharacteristic event handler declarations

    ////@begin RmSelectTableCharacteristic member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource(const wxString& name);

    /// Retrieves icon resources
    wxIcon GetIconResource(const wxString& name);
    ////@end RmSelectTableCharacteristic member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

    ////@begin RmSelectTableCharacteristic member variables
    wxListBox* m_listBox;
    ////@end RmSelectTableCharacteristic member variables
};

#endif
// _RMSELECTTABLECHARACTERISTIC_H_
