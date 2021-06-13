/////////////////////////////////////////////////////////////////////////////
// Name:        RmCharacteristicColourSelectionDialog.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     24/06/2006 16:16:14
// RCS-ID:      
// Copyright:   ©2006 University of Tasmania
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "RmCharacteristicColourSelectionDialog.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "RmCharacteristicColourSelectionDialog.h"
#include "app/gui/ViewCommon/ViewInformation.h"
#include "app/gui/common/IViewCoordinator.h"
#include "simulation/process/common/Process.h"
#include "core/utility/Utility.h"
#include "simulation/common/SprintUtility.h"
#include "simulation/common/Types.h"

using rootmap::CharacteristicColours_t;
using rootmap::Process;
using rootmap::CMYSelect;
using rootmap::ColourElementValue_t;
using rootmap::Utility::ToString;

////@begin XPM images
////@end XPM images

/*!
 * RmCharacteristicColourSelectionDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS(RmCharacteristicColourSelectionDialog, wxDialog)

/*!
 * RmCharacteristicColourSelectionDialog event table definition
 */

    BEGIN_EVENT_TABLE(RmCharacteristicColourSelectionDialog, wxDialog)

    ////@begin RmCharacteristicColourSelectionDialog event table entries
    EVT_CHOICE(XRCID("rmID_CCSD_COLOUR_CHOICE"), RmCharacteristicColourSelectionDialog::OnRmidCcsdColourChoiceSelected)

    EVT_LISTBOX(XRCID("rmID_CCSD_CHARACTERISTIC_LISTBOX"), RmCharacteristicColourSelectionDialog::OnRmidCcsdCharacteristicListboxSelected)
    EVT_LISTBOX_DCLICK(XRCID("rmID_CCSD_CHARACTERISTIC_LISTBOX"), RmCharacteristicColourSelectionDialog::OnRmidCcsdCharacteristicListboxDoubleClicked)

    EVT_TEXT(XRCID("rmID_CCSD_MINIMUMVALUE_TEXTCTRL"), RmCharacteristicColourSelectionDialog::OnRmidCcsdMinimumValueTextCtrlUpdated)

    EVT_TEXT(XRCID("rmID_CCSD_MAXIMUMVALUE_TEXTCTRL"), RmCharacteristicColourSelectionDialog::OnRmidCcsdMaximumValueTextCtrlUpdated)

    EVT_TEXT(XRCID("rmID_CCSD_COLOURATMINIMUM_TEXTCTRL"), RmCharacteristicColourSelectionDialog::OnRmidCcsdColourAtMinimumTextCtrlUpdated)

    EVT_TEXT(XRCID("rmID_CCSD_COLOURATMAXIMUM_TEXTCTRL"), RmCharacteristicColourSelectionDialog::OnRmidCcsdColourAtMaximumTextCtrlUpdated)

    EVT_BUTTON(XRCID("rmID_CCSD_APPLY_BUTTON"), RmCharacteristicColourSelectionDialog::OnRmidCcsdApplyButtonClick)

    ////@end RmCharacteristicColourSelectionDialog event table entries

    END_EVENT_TABLE()

    RootMapLoggerDefinition(RmCharacteristicColourSelectionDialog);

static double sRound(double d);
static ColourElementValue_t sPercent2ColourValue(long percent);
static long sColourValue2Percent(ColourElementValue_t colourValue);

/*!
 * RmCharacteristicColourSelectionDialog constructors
 */

RmCharacteristicColourSelectionDialog::RmCharacteristicColourSelectionDialog()
    : m_viewParent(__nullptr)
    , m_colourChoiceCtrl(__nullptr)
    , m_characteristicListBoxCtrl(__nullptr)
    , m_characteristicInformationTextCtrl(__nullptr)
    , m_characteristicMinimumValueTextCtrl(__nullptr)
    , m_characteristicMaximumValueTextCtrl(__nullptr)
    , m_colourMinimumTextCtrl(__nullptr)
    , m_colourMinimumStaticBitmap(__nullptr)
    , m_colourMaximumTextCtrl(__nullptr)
    , m_colourMaximumStaticBitmap(__nullptr)
    , m_otherInformationStaticTextCtrl(__nullptr)
    , m_coordinator(__nullptr)
    , m_internalUpdate(0)
{
    RootMapLoggerInitialisation("rootmap.RmCharacteristicColourSelectionDialog");
}

RmCharacteristicColourSelectionDialog::RmCharacteristicColourSelectionDialog(wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style)
    : m_viewParent(__nullptr)
    , m_colourChoiceCtrl(__nullptr)
    , m_characteristicListBoxCtrl(__nullptr)
    , m_characteristicInformationTextCtrl(__nullptr)
    , m_characteristicMinimumValueTextCtrl(__nullptr)
    , m_characteristicMaximumValueTextCtrl(__nullptr)
    , m_colourMinimumTextCtrl(__nullptr)
    , m_colourMinimumStaticBitmap(__nullptr)
    , m_colourMaximumTextCtrl(__nullptr)
    , m_colourMaximumStaticBitmap(__nullptr)
    , m_otherInformationStaticTextCtrl(__nullptr)
    , m_coordinator(__nullptr)
    , m_internalUpdate(0)
{
    RootMapLoggerInitialisation("rootmap.RmCharacteristicColourSelectionDialog");
    Create(parent, id, caption, pos, size, style);
}

/*!
 * RmCharacteristicColourSelectionDialog creator
 */

bool RmCharacteristicColourSelectionDialog::Create(wxWindow* parent, wxWindowID /*id*/, const wxString& /*caption*/, const wxPoint& /*pos*/, const wxSize& /*size*/, long /*style*/)
{
    ////@begin RmCharacteristicColourSelectionDialog member initialisation
    m_coordinator = __nullptr;
    m_viewParent = __nullptr;
    m_colourMinimumValidatorText = _T("");
    m_colourMaximumValidatorText = _T("");
    m_minimumValueValidatorText = _T("");
    m_maximumValueValidatorText = _T("");
    m_internalUpdate = 0;
    m_colourChoiceCtrl = __nullptr;
    m_characteristicListBoxCtrl = __nullptr;
    m_characteristicInformationTextCtrl = __nullptr;
    m_characteristicMinimumValueTextCtrl = __nullptr;
    m_characteristicMaximumValueTextCtrl = __nullptr;
    m_colourMinimumTextCtrl = __nullptr;
    m_colourMinimumStaticBitmap = __nullptr;
    m_colourMaximumTextCtrl = __nullptr;
    m_colourMaximumStaticBitmap = __nullptr;
    m_otherInformationStaticTextCtrl = __nullptr;
    ////@end RmCharacteristicColourSelectionDialog member initialisation

    ////@begin RmCharacteristicColourSelectionDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    SetParent(parent);
    CreateControls();
    Centre();
    ////@end RmCharacteristicColourSelectionDialog creation

    SetAffirmativeId(XRCID("rmID_CCSD_OK_BUTTON"));
    SetEscapeId(XRCID("rmID_CCSD_CANCEL_BUTTON"));

    return true;
}

/*!
 * Control creation for RmCharacteristicColourSelectionDialog
 */

void RmCharacteristicColourSelectionDialog::CreateControls()
{
    ////@begin RmCharacteristicColourSelectionDialog content construction
    if (!wxXmlResource::Get()->LoadDialog(this, GetParent(), _T("rmID_CCSD_DIALOG")))
        wxLogError(wxT("Missing wxXmlResource::Get()->Load() in OnInit()?"));
    m_colourChoiceCtrl = XRCCTRL(*this, "rmID_CCSD_COLOUR_CHOICE", wxChoice);
    m_characteristicListBoxCtrl = XRCCTRL(*this, "rmID_CCSD_CHARACTERISTIC_LISTBOX", wxListBox);
    m_characteristicInformationTextCtrl = XRCCTRL(*this, "rmID_CCSD_CHARACTERISTICINFORMATION_TEXTCTRL", wxTextCtrl);
    m_characteristicMinimumValueTextCtrl = XRCCTRL(*this, "rmID_CCSD_MINIMUMVALUE_TEXTCTRL", wxTextCtrl);
    m_characteristicMaximumValueTextCtrl = XRCCTRL(*this, "rmID_CCSD_MAXIMUMVALUE_TEXTCTRL", wxTextCtrl);
    m_colourMinimumTextCtrl = XRCCTRL(*this, "rmID_CCSD_COLOURATMINIMUM_TEXTCTRL", wxTextCtrl);
    m_colourMinimumStaticBitmap = XRCCTRL(*this, "rmID_CCSD_COLOURATMINIMUM_STATICBITMAP", wxStaticBitmap);
    m_colourMaximumTextCtrl = XRCCTRL(*this, "rmID_CCSD_COLOURATMAXIMUM_TEXTCTRL", wxTextCtrl);
    m_colourMaximumStaticBitmap = XRCCTRL(*this, "rmID_CCSD_COLOURATMAXIMUM_STATICBITMAP", wxStaticBitmap);
    m_otherInformationStaticTextCtrl = XRCCTRL(*this, "rmID_CCSD_OTHERINFO_STATIC", wxStaticText);
    // Set validators
    if (FindWindow(XRCID("rmID_CCSD_MINIMUMVALUE_TEXTCTRL")))
        FindWindow(XRCID("rmID_CCSD_MINIMUMVALUE_TEXTCTRL"))->SetValidator(wxTextValidator(wxFILTER_NONE, &m_minimumValueValidatorText));
    if (FindWindow(XRCID("rmID_CCSD_MAXIMUMVALUE_TEXTCTRL")))
        FindWindow(XRCID("rmID_CCSD_MAXIMUMVALUE_TEXTCTRL"))->SetValidator(wxTextValidator(wxFILTER_NONE, &m_maximumValueValidatorText));
    if (FindWindow(XRCID("rmID_CCSD_COLOURATMINIMUM_TEXTCTRL")))
        FindWindow(XRCID("rmID_CCSD_COLOURATMINIMUM_TEXTCTRL"))->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_colourMinimumValidatorText));
    if (FindWindow(XRCID("rmID_CCSD_COLOURATMAXIMUM_TEXTCTRL")))
        FindWindow(XRCID("rmID_CCSD_COLOURATMAXIMUM_TEXTCTRL"))->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_colourMaximumValidatorText));
    ////@end RmCharacteristicColourSelectionDialog content construction

    // Create custom windows not generated automatically here.
    ////@begin RmCharacteristicColourSelectionDialog content initialisation
    ////@end RmCharacteristicColourSelectionDialog content initialisation
}

/*!
 * wxEVT_COMMAND_CHOICE_SELECTED event handler for rmID_CCSD_COLOUR_CHOICE
 */

void RmCharacteristicColourSelectionDialog::OnRmidCcsdColourChoiceSelected(wxCommandEvent& /*event*/)
{
    int csel = static_cast<int>(m_cmySelection);

    /*bool minval_success =*/
    m_characteristicMinimumValueTextCtrl->GetValue().ToDouble(&(m_colourInfo[csel].characteristicRangeMin));
    /*bool maxval_success =*/
    m_characteristicMaximumValueTextCtrl->GetValue().ToDouble(&(m_colourInfo[csel].characteristicRangeMax));

    long min_colour, max_colour;
    /*bool mincol_success =*/
    m_colourMinimumTextCtrl->GetValue().ToLong(&min_colour);
    m_colourInfo[csel].colourRangeMin = sPercent2ColourValue(min_colour);
    /*bool maxcol_success =*/
    m_colourMaximumTextCtrl->GetValue().ToLong(&max_colour);
    m_colourInfo[csel].colourRangeMax = sPercent2ColourValue(max_colour);

    // When the user makes a selection from the combo box, m_cmySelection can only be one of C/M/Y or Undisplayed (if applicable).
    // However, when the user makes a selection from the Characteristic list box, m_cmySelection is set to the index of that Characteristic
    // in m_colourInfo and m_processInfo. That is, if found. See OnRmidCcsdCharacteristicListboxSelected()
    // This way we can generate multiple CharacteristicColourInfo objects under the one "Undisplayed" combo box setting.
    m_cmySelection = static_cast<rootmap::CMYSelect>(m_colourChoiceCtrl->GetSelection());

    UpdateColour();
}

/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for rmID_CCSD_CHARACTERISTIC_LISTBOX
 */

void RmCharacteristicColourSelectionDialog::OnRmidCcsdCharacteristicListboxSelected(wxCommandEvent& /*event*/)
{
    wxString cname = m_characteristicListBoxCtrl->GetStringSelection();
    long int cnum = 0;
    Process* p = m_coordinator->findProcessCharacteristicNumber(cname, cnum);

    int csel = static_cast<int>(m_cmySelection);

    RmAssert(__nullptr != m_processInfo[csel].first && __nullptr != p, "Could not find process for selected characteristic");
    m_processInfo[csel].first = p;
    m_processInfo[csel].second = cnum;
    m_colourInfo[csel].characteristicRangeMin = p->GetCharacteristicMinimum(cnum);
    m_colourInfo[csel].characteristicRangeMax = p->GetCharacteristicMaximum(cnum);

    m_colourInfo[csel].colourRangeMin = std::numeric_limits<ColourElementValue_t>::min();
    m_colourInfo[csel].colourRangeMax = std::numeric_limits<ColourElementValue_t>::max();
    m_colourInfo[csel].characteristicID = p->GetCharacteristicID(cnum);
    m_colourInfo[csel].characteristicIndex = p->GetCharacteristicIndex(cnum);


    LOG_DEBUG << "Listbox Selected - cname=" << cname << " cid=" << m_colourInfo[csel].characteristicID << " colmin/max=" << static_cast<long>(m_colourInfo[csel].colourRangeMin) << "/" << static_cast<long>(m_colourInfo[csel].colourRangeMax);

    UpdateCharacteristic();
}

/*!
 * wxEVT_COMMAND_LISTBOX_DOUBLECLICKED event handler for rmID_CCSD_CHARACTERISTIC_LISTBOX
 */

void RmCharacteristicColourSelectionDialog::OnRmidCcsdCharacteristicListboxDoubleClicked(wxCommandEvent& event)
{
    OnRmidCcsdCharacteristicListboxSelected(event);
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for rmID_CCSD_APPLY_BUTTON
 */

void RmCharacteristicColourSelectionDialog::OnRmidCcsdApplyButtonClick(wxCommandEvent& /*event*/)
{
    TransferDataFromWindow();
    UpdateCharacteristic();
}

/*!
 * Should we show tooltips?
 */

bool RmCharacteristicColourSelectionDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap RmCharacteristicColourSelectionDialog::GetBitmapResource(const wxString& name)
{
    // Bitmap retrieval
    ////@begin RmCharacteristicColourSelectionDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
    ////@end RmCharacteristicColourSelectionDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon RmCharacteristicColourSelectionDialog::GetIconResource(const wxString& name)
{
    // Icon retrieval
    ////@begin RmCharacteristicColourSelectionDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
    ////@end RmCharacteristicColourSelectionDialog icon retrieval
}

void RmCharacteristicColourSelectionDialog::Setup(RmViewWindow* viewParent, const rootmap::CharacteristicColours_t& colourInfo, rootmap::IViewCoordinator* coordinator)
{
    ++m_internalUpdate;
    m_viewParent = viewParent;
    m_coordinator = coordinator;

    for (size_t i = 0; i < rootmap::colourElementsMax; ++i)
    {
        m_colourInfo[i] = colourInfo[i];

        // Find ProcessInfo to match
        if (m_processInfo.size() > i)
        {
            m_processInfo[i].first = m_coordinator->findProcessCharacteristicNumber(m_colourInfo[i].characteristicIndex, m_processInfo[i].second);
            RmAssert(__nullptr != m_processInfo[i].first, "Could not find process for initial characteristic");
        }
        else
        {
            long num;
            Process* p = m_coordinator->findProcessCharacteristicNumber(m_colourInfo[i].characteristicIndex, num);
            RmAssert(__nullptr != p, "Could not find process for initial characteristic");
            m_processInfo.push_back(std::pair<Process*, long>(p, num));
        }
    }

    // characteristic list
    wxArrayString characteristics;
    m_coordinator->populateCharacteristicStringArray(characteristics);

    m_characteristicListBoxCtrl->Set(characteristics);

    m_cmySelection = rootmap::Cyan;
    m_colourChoiceCtrl->Select(static_cast<int>(m_cmySelection));
    UpdateColour();
    --m_internalUpdate;
}

void RmCharacteristicColourSelectionDialog::UpdateColour()
{
    ++m_internalUpdate;
    int csel = static_cast<int>(m_cmySelection);

    // Cyan/Magenta/Yellow popup menu
    //m_colourChoiceCtrl->Select(csel);

    LOG_DEBUG << "UpdateColour() - m_colourInfo[" << csel << "].characteristicID=" << m_colourInfo[csel].characteristicID << " name=" << (m_processInfo[csel].first->GetCharacteristicName(m_processInfo[csel].second));
    //m_processInfo[csel].first = m_coordinator->findProcessCharacteristicNumber(m_colourInfo[csel].characteristicIndex,m_processInfo[csel].second);
    //RmAssert( 0!=m_processInfo[csel].first, "Could not find process for selected characteristic");

    wxString cname = (m_processInfo[csel].first->GetCharacteristicName(m_processInfo[csel].second)).c_str();

    m_characteristicListBoxCtrl->SetStringSelection(cname);

    UpdateCharacteristic();

    UpdateOtherInformation();
    --m_internalUpdate;
}

void RmCharacteristicColourSelectionDialog::UpdateOtherInformation()
{
    ++m_internalUpdate;
    wxString infotext = "";
    int max = rootmap::colourElementsMax; // Note we're only interested in the CMY Characteristics here
    for (int i = 0; i < max; ++i)
    {
        if (i != static_cast<int>(m_cmySelection))
        {
            //ColourElementValue_t colourRangeMin;
            //ColourElementValue_t colourRangeMax;
            //double    characteristicRangeMin;
            //double    characteristicRangeMax;

            CMYSelect csel = (CMYSelect)i;
            infotext << csel << ": "
                << m_processInfo[i].first->GetCharacteristicName(m_processInfo[csel].second).c_str() << ", "
                << m_colourInfo[i].characteristicRangeMin << "-"
                << m_colourInfo[i].characteristicRangeMax << ", "
                << sColourValue2Percent(m_colourInfo[i].colourRangeMin) << "-"
                << sColourValue2Percent(m_colourInfo[i].colourRangeMax) << "%\n";
        }
    }

    m_otherInformationStaticTextCtrl->SetLabel(infotext);
    --m_internalUpdate;
}

void RmCharacteristicColourSelectionDialog::UpdateCharacteristic()
{
    ++m_internalUpdate;
    int csel = static_cast<int>(m_cmySelection);
    Process* p = m_processInfo[csel].first;
    long int cnum = m_processInfo[csel].second;

    //m_characteristicInformationTextCtrl->SelectAll();
    // MSA 09.09.15 Added Name to this infotext, because it is 
    // often obscured in the list box
    wxString infotext = "Name: ";
    infotext << p->GetCharacteristicName(cnum) << "\nUnits:"
        << p->GetCharacteristicUnits(cnum)
        << "\nMinimum: " << p->GetCharacteristicMinimum(cnum)
        << "\nMaximum: " << p->GetCharacteristicMaximum(cnum)
        << "\nDefault: " << p->GetCharacteristicDefault(cnum);
    m_characteristicInformationTextCtrl->SetValue(infotext);

    // These may have been disabled earlier - see below
    m_colourMaximumTextCtrl->Enable();
    m_colourMinimumTextCtrl->Enable();

    if (m_minimumValueValidatorText.empty())
    {
        m_minimumValueValidatorText = ToString(m_colourInfo[csel].characteristicRangeMin).c_str();
    }
    else
    {
        m_characteristicMinimumValueTextCtrl->SetValue(ToString(m_colourInfo[csel].characteristicRangeMin).c_str());
    }

    if (m_maximumValueValidatorText.empty())
    {
        m_maximumValueValidatorText = ToString(m_colourInfo[csel].characteristicRangeMax).c_str();
    }
    else
    {
        m_characteristicMaximumValueTextCtrl->SetValue(ToString(m_colourInfo[csel].characteristicRangeMax).c_str());
    }

    LOG_DEBUG << "UpdateCharacteristic() - cname=" << p->GetCharacteristicName(cnum) << " cid=" << m_colourInfo[csel].characteristicID << " colmin/max=" << static_cast<long>(m_colourInfo[csel].colourRangeMin) << "/" << static_cast<long>(m_colourInfo[csel].colourRangeMax);

    if (m_colourMinimumValidatorText.empty())
    {
        m_colourMinimumValidatorText = ToString(sColourValue2Percent(m_colourInfo[csel].colourRangeMin)).c_str();
    }
    else
    {
        m_colourMinimumTextCtrl->SetValue(ToString(sColourValue2Percent(m_colourInfo[csel].colourRangeMin)).c_str());
    }

    if (m_colourMaximumValidatorText.empty())
    {
        m_colourMaximumValidatorText = ToString(sColourValue2Percent(m_colourInfo[csel].colourRangeMax)).c_str();
    }
    else
    {
        m_colourMaximumTextCtrl->SetValue(ToString(sColourValue2Percent(m_colourInfo[csel].colourRangeMax)).c_str());
    }

    --m_internalUpdate;
    const int minw = m_colourMinimumStaticBitmap->GetSize().GetWidth();
    const int minh = m_colourMinimumStaticBitmap->GetSize().GetHeight();
    const int maxw = m_colourMaximumStaticBitmap->GetSize().GetWidth();
    const int maxh = m_colourMaximumStaticBitmap->GetSize().GetHeight();
    wxImage minimg(minw, minh, false); // No need to clear, we are going to set image data below
    wxImage maximg(maxw, maxh, false); // No need to clear, we are going to set image data below
    switch (csel)
    {
    case rootmap::Cyan:
        minimg.SetRGB(wxRect(0, 0, minw, minh), 0xff - m_colourInfo[csel].colourRangeMin, 0xff, 0xff);
        maximg.SetRGB(wxRect(0, 0, maxw, maxh), 0xff - m_colourInfo[csel].colourRangeMax, 0xff, 0xff);
        break;
    case rootmap::Magenta:
        minimg.SetRGB(wxRect(0, 0, minw, minh), 0xff, 0xff - m_colourInfo[csel].colourRangeMin, 0xff);
        maximg.SetRGB(wxRect(0, 0, maxw, maxh), 0xff, 0xff - m_colourInfo[csel].colourRangeMax, 0xff);
        break;
    case rootmap::Yellow:
        minimg.SetRGB(wxRect(0, 0, minw, minh), 0xff, 0xff, 0xff - m_colourInfo[csel].colourRangeMin);
        maximg.SetRGB(wxRect(0, 0, maxw, maxh), 0xff, 0xff, 0xff - m_colourInfo[csel].colourRangeMax);
        break;
    default:
        RmAssert(false, "Non-Cyan/Magenta/Yellow CCI found");
        break;
    }
    m_colourMinimumStaticBitmap->SetBitmap(wxBitmap(minimg));
    m_colourMaximumStaticBitmap->SetBitmap(wxBitmap(maximg));
}


double sRound(double d)
{
    if (fmod(d, 1.0) > 0.5)
    {
        return (ceil(d));
    }

    return (floor(d));
}

ColourElementValue_t sPercent2ColourValue(long percent)
{
    double colourValue = static_cast<double>(percent * std::numeric_limits<ColourElementValue_t>::max()) / static_cast<double>(100.0);
    return static_cast<ColourElementValue_t>(sRound(colourValue));
}

long sColourValue2Percent(ColourElementValue_t colourValue)
{
    double percent = static_cast<double>(colourValue * 100) / static_cast<double>(std::numeric_limits<ColourElementValue_t>::max());
    return static_cast<long>(sRound(percent));
}


/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for rmID_CCSD_MINIMUMVALUE_TEXTCTRL
 */

void RmCharacteristicColourSelectionDialog::OnRmidCcsdMinimumValueTextCtrlUpdated(wxCommandEvent& /*event*/)
{
    //LOG_DEBUG << "Minimum Value Updated (internalUpdate=" << m_internalUpdate << ")";
    if (m_internalUpdate == 0)
    {
        LOG_DEBUG << "ctrl value=" << m_characteristicMinimumValueTextCtrl->GetValue();
    }
}


/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for rmID_CCSD_MAXIMUMVALUE_TEXTCTRL
 */

void RmCharacteristicColourSelectionDialog::OnRmidCcsdMaximumValueTextCtrlUpdated(wxCommandEvent& /*event*/)
{
    //LOG_DEBUG << "Maximum Value Updated (internalUpdate=" << m_internalUpdate << ")";
    if (m_internalUpdate == 0)
    {
        LOG_DEBUG << "ctrl value=" << m_characteristicMaximumValueTextCtrl->GetValue();
    }
}


/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for rmID_CCSD_COLOURATMINIMUM_TEXTCTRL
 */

void RmCharacteristicColourSelectionDialog::OnRmidCcsdColourAtMaximumTextCtrlUpdated(wxCommandEvent& /*event*/)
{
    //LOG_DEBUG << "Colour Maximum Updated (internalUpdate=" << m_internalUpdate << ")";
    if (m_internalUpdate == 0)
    {
        LOG_DEBUG << "ctrl value=" << m_colourMaximumTextCtrl->GetValue();
    }
}


/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for rmID_CCSD_COLOURATMINIMUM_TEXTCTRL
 */

void RmCharacteristicColourSelectionDialog::OnRmidCcsdColourAtMinimumTextCtrlUpdated(wxCommandEvent& /*event*/)
{
    //LOG_DEBUG << "Colour Minimum Updated (internalUpdate=" << m_internalUpdate << ")";
    if (m_internalUpdate == 0)
    {
        LOG_DEBUG << "ctrl value=" << m_colourMinimumTextCtrl->GetValue();
    }
}

bool RmCharacteristicColourSelectionDialog::TransferDataFromWindow()
{
    TransferToParent();

    return (wxWindowBase::TransferDataFromWindow());
}

void RmCharacteristicColourSelectionDialog::TransferToParent()
{
    // first make sure we've parsed all the currently displayed values
    // this means the text fields - the others are generally updated live
    int csel = static_cast<int>(m_cmySelection);

    /*bool minval_success =*/
    m_characteristicMinimumValueTextCtrl->GetValue().ToDouble(&(m_colourInfo[csel].characteristicRangeMin));
    /*bool maxval_success =*/
    m_characteristicMaximumValueTextCtrl->GetValue().ToDouble(&(m_colourInfo[csel].characteristicRangeMax));

    long min_colour, max_colour;
    /*bool mincol_success =*/
    m_colourMinimumTextCtrl->GetValue().ToLong(&min_colour);
    m_colourInfo[csel].colourRangeMin = sPercent2ColourValue(min_colour);
    /*bool maxcol_success =*/
    m_colourMaximumTextCtrl->GetValue().ToLong(&max_colour);
    m_colourInfo[csel].colourRangeMax = sPercent2ColourValue(max_colour);

    m_viewParent->UpdateCharacteristicColourInfo(m_colourInfo);
}
