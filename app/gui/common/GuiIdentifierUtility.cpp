#include "app/gui/common/GuiIdentifierUtility.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

namespace rootmap
{
    GuiIdentifierUtility& GuiIdentifierUtility::instance()
    {
        static GuiIdentifierUtility sGiu; // the one-and-only instance
        return sGiu;
    }


    GuiIdentifierUtility::GuiIdentifierUtility()
        : m_nextLayerMenuItemIdentifier(wxID_HIGHEST + 100)
    {
    }

    GuiIdentifierUtility::~GuiIdentifierUtility()
    {
    }

    // ///////////////////////////////
    //      Use
    // ///////////////////////////////
    int GuiIdentifierUtility::useNextLayerMenuItemIdentifier()
    {
        // get the next id
        int next_id = ++m_nextLayerMenuItemIdentifier;
        // wrap around after 1000 ids
        if (next_id > m_layerMenuItemMax) next_id = m_layerMenuItemMin;
        return next_id;
    }


    // ///////////////////////////////
    //      Peek
    // ///////////////////////////////
    int GuiIdentifierUtility::peekNextLayerMenuItemIdentifier()
    {
        return m_nextLayerMenuItemIdentifier;
    }

    void GuiIdentifierUtility::getLayerMenuItemIdentifierRange(int& min, int& max)
    {
        min = m_layerMenuItemMin;
        max = m_layerMenuItemMax;
    }

    const int GuiIdentifierUtility::m_layerMenuItemMin = wxID_HIGHEST + 100;
    const int GuiIdentifierUtility::m_layerMenuItemMax = wxID_HIGHEST + 100 + 1000;
} /* namespace rootmap */
