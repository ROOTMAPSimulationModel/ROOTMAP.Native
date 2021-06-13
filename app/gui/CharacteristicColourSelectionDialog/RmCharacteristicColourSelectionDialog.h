/////////////////////////////////////////////////////////////////////////////
// Name:        RmCharacteristicColourSelectionDialog.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     24/06/2006 16:16:14
// RCS-ID:      
// Copyright:   ©2006 University of Tasmania
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _RMCHARACTERISTICCOLOURSELECTIONDIALOG_H_
#define _RMCHARACTERISTICCOLOURSELECTIONDIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "RmCharacteristicColourSelectionDialog.h"
#endif

/*!
 * Includes
 */

 ////@begin includes
#include "wx/xrc/xmlres.h"
#include "wx/valtext.h"
////@end includes

#include "app/gui/ViewWindow/RmViewWindow.h"
#include "simulation/common/Types.h"
#include "core/log/Logger.h"

/*!
 * Forward declarations
 */

 ////@begin forward declarations
 ////@end forward declarations
class RmViewWindow;

namespace rootmap
{
    class Process;
}

/*!
 * Control identifiers
 */

 ////@begin control identifiers
#define rmID_CCSD_DIALOG 10023
#define SYMBOL_RMCHARACTERISTICCOLOURSELECTIONDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxCLOSE_BOX
#define SYMBOL_RMCHARACTERISTICCOLOURSELECTIONDIALOG_TITLE _("Characteristic Colour Selection")
#define SYMBOL_RMCHARACTERISTICCOLOURSELECTIONDIALOG_IDNAME rmID_CCSD_DIALOG
#define SYMBOL_RMCHARACTERISTICCOLOURSELECTIONDIALOG_SIZE wxSize(600, 500)
#define SYMBOL_RMCHARACTERISTICCOLOURSELECTIONDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

 /*!
  * RmCharacteristicColourSelectionDialog class declaration
  */

class RmCharacteristicColourSelectionDialog : public wxDialog
{
    DECLARE_DYNAMIC_CLASS(RmCharacteristicColourSelectionDialog)
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    RmCharacteristicColourSelectionDialog();
    RmCharacteristicColourSelectionDialog(wxWindow* parent, wxWindowID id = SYMBOL_RMCHARACTERISTICCOLOURSELECTIONDIALOG_IDNAME, const wxString& caption = SYMBOL_RMCHARACTERISTICCOLOURSELECTIONDIALOG_TITLE, const wxPoint& pos = SYMBOL_RMCHARACTERISTICCOLOURSELECTIONDIALOG_POSITION, const wxSize& size = SYMBOL_RMCHARACTERISTICCOLOURSELECTIONDIALOG_SIZE, long style = SYMBOL_RMCHARACTERISTICCOLOURSELECTIONDIALOG_STYLE);

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = SYMBOL_RMCHARACTERISTICCOLOURSELECTIONDIALOG_IDNAME, const wxString& caption = SYMBOL_RMCHARACTERISTICCOLOURSELECTIONDIALOG_TITLE, const wxPoint& pos = SYMBOL_RMCHARACTERISTICCOLOURSELECTIONDIALOG_POSITION, const wxSize& size = SYMBOL_RMCHARACTERISTICCOLOURSELECTIONDIALOG_SIZE, long style = SYMBOL_RMCHARACTERISTICCOLOURSELECTIONDIALOG_STYLE);

    /// Creates the controls and sizers
    void CreateControls();

    ////@begin RmCharacteristicColourSelectionDialog event handler declarations

    /// wxEVT_COMMAND_CHOICE_SELECTED event handler for rmID_CCSD_COLOUR_CHOICE
    void OnRmidCcsdColourChoiceSelected(wxCommandEvent& event);

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for rmID_CCSD_CHARACTERISTIC_LISTBOX
    void OnRmidCcsdCharacteristicListboxSelected(wxCommandEvent& event);

    /// wxEVT_COMMAND_LISTBOX_DOUBLECLICKED event handler for rmID_CCSD_CHARACTERISTIC_LISTBOX
    void OnRmidCcsdCharacteristicListboxDoubleClicked(wxCommandEvent& event);

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for rmID_CCSD_MINIMUMVALUE_TEXTCTRL
    void OnRmidCcsdMinimumValueTextCtrlUpdated(wxCommandEvent& event);

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for rmID_CCSD_MAXIMUMVALUE_TEXTCTRL
    void OnRmidCcsdMaximumValueTextCtrlUpdated(wxCommandEvent& event);

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for rmID_CCSD_COLOURATMINIMUM_TEXTCTRL
    void OnRmidCcsdColourAtMinimumTextCtrlUpdated(wxCommandEvent& event);

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for rmID_CCSD_COLOURATMAXIMUM_TEXTCTRL
    void OnRmidCcsdColourAtMaximumTextCtrlUpdated(wxCommandEvent& event);

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for rmID_CCSD_APPLY_BUTTON
    void OnRmidCcsdApplyButtonClick(wxCommandEvent& event);

    ////@end RmCharacteristicColourSelectionDialog event handler declarations

    ////@begin RmCharacteristicColourSelectionDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource(const wxString& name);

    /// Retrieves icon resources
    wxIcon GetIconResource(const wxString& name);
    ////@end RmCharacteristicColourSelectionDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

    void Setup(RmViewWindow* viewParent, const rootmap::CharacteristicColours_t& colourInfo, rootmap::IViewCoordinator* coordinator);
    void UpdateColour();
    void UpdateCharacteristic();
    void UpdateOtherInformation();

    ////@begin RmCharacteristicColourSelectionDialog member variables
    wxChoice* m_colourChoiceCtrl;
    wxListBox* m_characteristicListBoxCtrl;
    wxTextCtrl* m_characteristicInformationTextCtrl;
    wxTextCtrl* m_characteristicMinimumValueTextCtrl;
    wxTextCtrl* m_characteristicMaximumValueTextCtrl;
    wxTextCtrl* m_colourMinimumTextCtrl;
    wxStaticBitmap* m_colourMinimumStaticBitmap;
    wxTextCtrl* m_colourMaximumTextCtrl;
    wxStaticBitmap* m_colourMaximumStaticBitmap;
    wxStaticText* m_otherInformationStaticTextCtrl;
private:
    rootmap::CharacteristicColours_t m_colourInfo;
    rootmap::IViewCoordinator* m_coordinator;
    RmViewWindow* m_viewParent;
    wxString m_colourMinimumValidatorText;
    wxString m_colourMaximumValidatorText;
    wxString m_minimumValueValidatorText;
    wxString m_maximumValueValidatorText;
    long m_internalUpdate;
    ////@end RmCharacteristicColourSelectionDialog member variables

    bool TransferDataFromWindow();
    void TransferToParent();

    RootMapLoggerDeclaration();

    // MSA 09.11.03 Converted to vector, as with m_colourInfo
    /*struct
    {
        rootmap::Process * process;
        long int characteristicNumber;
    } m_processInfo[rootmap::colourElementsMax];*/
    std::vector<std::pair<rootmap::Process*, long>> m_processInfo;
    rootmap::CMYSelect m_cmySelection;
};

#endif
// _RMCHARACTERISTICCOLOURSELECTIONDIALOG_H_
