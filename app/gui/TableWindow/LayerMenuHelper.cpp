/////////////////////////////////////////////////////////////////////////////
// Name:        LayerMenuHelper.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     24/06/2006 20:16:57
// RCS-ID:      
// Copyright:   ©2006 University of Tasmania
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "LayerMenuHelper.h"
#endif

#include "app/gui/common/GuiIdentifierUtility.h"
#include "app/gui/TableWindow/LayerMenuHelper.h"
#include "app/gui/TableWindow/ScoreboardTableHelper.h"
#include "app/gui/TableWindow/RmTableWindow.h"
#include "app/gui/TableWindow/TableInformation.h"
#include "simulation/common/SprintUtility.h"
#include "simulation/scoreboard/Scoreboard.h"
#include "core/scoreboard/Dimension.h"
#include "core/common/RmAssert.h"
#include "core/utility/Utility.h"

/*!
 * LayerMenuHelper type definition
 */

namespace rootmap
{
    RootMapLoggerDefinition(LayerMenuHelper);

    /*!
     * LayerMenuHelper constructors
     */

    LayerMenuHelper::LayerMenuHelper
    (RmTableWindow& tableWindow,
        ScoreboardTableHelper& scoreboardtablehelper)
        : m_helper(scoreboardtablehelper)
        , m_parent(tableWindow)
        , m_labelId(0)
        , m_layerMenu(__nullptr)
        , m_layerMenuItem(__nullptr)
    {
        RootMapLoggerInitialisation("rootmap.LayerMenuHelper");
    }


    LayerMenuHelper::~LayerMenuHelper()
    {
    }

    /*!
     * Control creation for rmTableWindow
     */


    const rootmap::TableInformation& LayerMenuHelper::GetTableInformation() const
    {
        return m_helper.GetTableInformation();
    }


    void LayerMenuHelper::RebuildLayerStrings()
    {
        LOG_DEBUG << "LayerMenuHelper::RebuildLayerStrings {this:" << this << "}";
        // Clearing the array is no trouble, so we don't have a separate function for it
        m_layerStrings.Empty();

        const TableInformation& table_info = GetTableInformation();

        Dimension layers_dim = rootmap::Y; // works for viewdirection==vFront
        switch (table_info.GetViewDirection())
        {
        case rootmap::vSide:
            layers_dim = rootmap::X;
            break;
        case rootmap::vTop:
            layers_dim = rootmap::Z;
            break;
        default:
            layers_dim = rootmap::Y;
        }

        const Scoreboard& scoreboard = m_helper.getScoreboard();
        const BoundaryArray& boundaries = scoreboard.GetBoundaryArray(layers_dim);
        size_t num_layers = boundaries.GetNumLayers();
        for (unsigned long layer_num = 1; layer_num <= num_layers; ++layer_num)
        {
            double layer_start = boundaries.GetLayerStart(layer_num);
            double layer_end = boundaries.GetLayerEnd(layer_num);

            std::string s = Utility::ToString(layer_start, 2, true);
            s += " -> ";
            s += Utility::ToString(layer_end, 2, true);

            m_layerStrings.Add(s.c_str());
        }
    }


    void LayerMenuHelper::RebuildLayerMenu()
    {
        RebuildLayerStrings();

        ClearLayerMenu();
        BuildLayerMenu();
    }

    void LayerMenuHelper::ClearLayerMenu()
    {
        LOG_DEBUG << "LayerMenuHelper::ClearLayerMenu {this:" << this << "}";
        wxMenu* layer_submenu = GetLayerSubMenu();

        // de-allocate all the current items one at a time
        wxMenuItemList layer_list = layer_submenu->GetMenuItems();
        for (wxMenuItemList::iterator list_iter = layer_list.begin();
            list_iter != layer_list.end(); ++list_iter)
        {
            // MSA 09.12.09 Must use Destroy() not Delete() if menu item is a submenu
            wxMenu* menuCheck = (wxMenu*)(*list_iter);
            if (menuCheck)
            {
                layer_submenu->Destroy(*list_iter);
            }
            else // Is not a submenu
            {
                layer_submenu->Delete(*list_iter);
            }
        }
        // clear the ids
        // TODO: also free ids in the GuiIdentifierUtility
        for (std::vector<long>::iterator iter(m_layerMenuItemIds.begin());
            iter != m_layerMenuItemIds.end(); ++iter)
        {
        }
        m_layerMenuItemIds.clear();
    }


    void LayerMenuHelper::BuildLayerMenu()
    {
        LOG_DEBUG << "LayerMenuHelper::BuildLayerMenu {this:" << this << "}";
        wxMenu* layer_submenu = GetLayerSubMenu();

        // if there are no items in the list, adding a radio item will cause a
        // crash - thanks, wxWidgets. So we start by adding a useful (disabled)
        // indicator
        rootmap::ViewDirection along_viewdir = rootmap::vSide;
        switch (GetTableInformation().GetViewDirection())
        {
        case rootmap::vFront: along_viewdir = rootmap::vSide;
            break;
        case rootmap::vSide: along_viewdir = rootmap::vFront;
            break;
        case rootmap::vTop: along_viewdir = rootmap::vTop;
            break;
        default: RmAssert(false, "Invalid ViewDirection");
            break;
        }
        std::string label = SprintUtility::StringOfTableAxis(along_viewdir);
        label.insert(0, "Along ");
        m_labelId = GuiIdentifierUtility::instance().useNextLayerMenuItemIdentifier();

        // MSA 09.12.09 This pointer is to a wxMenuItem on the heap (see menuitem.cpp)
        // It is part of the wxWidgets framework. So why does it leak?
        wxMenuItem* item = layer_submenu->Append(m_labelId, label.c_str(), "", wxITEM_NORMAL);
        item->Enable(false);
        LOG_INFO << "Created SubMenu Item {Id:" << m_labelId
            << ", Label:" << label << "}";


        for (wxArrayString::iterator iter = m_layerStrings.begin();
            iter != m_layerStrings.end(); ++iter)
        {
            int item_id = GuiIdentifierUtility::instance().useNextLayerMenuItemIdentifier();
            /* wxMenuItem * item = */
            layer_submenu->AppendRadioItem(item_id, (*iter));
            m_layerMenuItemIds.push_back(item_id);

            LOG_INFO << "Created SubMenu Item {Id:" << item_id
                << ", Label:" << (*iter) << "}";
        }
    }


    void LayerMenuHelper::UpdateLayer()
    {
        // menu Radio items are managed by wx so that all but the checked item will be unchecked

        int layer = GetTableInformation().GetLayer();
        int id_to_check = m_layerMenuItemIds[layer - 1];

        GetLayerSubMenu()->Check(id_to_check, true);
    }


    void LayerMenuHelper::ReplaceLayerMenu()
    {
        LOG_DEBUG << "LayerMenuHelper::ReplaceLayerMenu {this:" << this << "}";

        // If the previous submenu is required, use wxMenuItem::GetSubMenu()
        wxMenuItem* old_layer_menuitem = GetLayerSubMenuItem();
        wxMenu* our_layer_menu = GetLayerSubMenu();
        wxMenu* current_submenu = old_layer_menuitem->GetSubMenu();

        if (current_submenu != our_layer_menu)
        {
            wxMenu* table_menu = GetTableMenu();

            // Clear current entry
            // MSA 09.12.09 Must use Destroy() not Delete() if menu item is a submenu
            try
            {
                wxMenu* menuCheck = dynamic_cast<wxMenu*>(old_layer_menuitem);
                if (menuCheck)
                {
                    table_menu->Destroy(old_layer_menuitem);
                }
                else // Is not a submenu
                {
                    table_menu->Delete(old_layer_menuitem);
                }
            }
            catch (std::bad_cast& bc)
            {
                LOG_ALERT << bc.what();
                table_menu->Delete(old_layer_menuitem);
            }
            catch (std::bad_typeid& bti)
            {
                // This exception is sometimes thrown (from wxWidgets code)
                // when the user is rapidly switching between Tables.
                // The best way I can think of to deal with it
                // is to simply allow the improperly-cast old_layer_menuitem to leak
                // and continue without interruptions.
                LOG_ALERT << bti.what();
                // MSA 11.03.09 Attempting to delete this results in an assert failure
                // in wxWidgets code.
                // N.B. Will this just result in a small memory leak, or will the side-effects be worse?
                //table_menu->Delete(old_layer_menuitem);
            }
            catch (std::exception& exc)
            {
                LOG_ALERT << exc.what();
                table_menu->Delete(old_layer_menuitem);
            }

            int layer_item_id = XRCID("rmID_TABLEMENU_LAYERSUBMENU");

            m_layerMenuItem = new wxMenuItem(table_menu,
                layer_item_id,
                _("&Layer"),
                _(""),
                wxITEM_NORMAL,
                our_layer_menu);
            m_layerMenuItem = table_menu->Insert(
                rootmap::TableMenuItemIndex_LayerSubmenu,
                m_layerMenuItem);

            //m_layerMenuItem->SetSubMenu( layer_menu );

            LOG_DEBUG << "Created submenu item {ID:" << m_layerMenuItem->GetId()
                << ", Text:" << m_layerMenuItem->GetItemLabel()
                << ", SubMenu:" << m_layerMenuItem->GetSubMenu()
                << "}";
        }
    }


    wxString LayerMenuHelper::GetActiveItemString()
    {
        if (!m_layerStrings.empty())
        {
            return (m_layerStrings.Item(GetTableInformation().GetLayer() - 1));
        }
        return "";
    }

    wxMenu* LayerMenuHelper::GetTableMenu()
    {
        wxWindow* parent_win = m_parent.GetParent();
        wxMDIParentFrame* parent_frame = wxDynamicCast(parent_win, wxMDIParentFrame);
        RmAssert(__nullptr != parent_frame, "TableWindow parent is not wxMDIParentFrame");

        wxMenuBar* menu_bar = parent_frame->GetMenuBar();
        RmAssert(__nullptr != menu_bar, "wxMDIParentFrame returned NULL MenuBar");

        return (menu_bar->GetMenu(rootmap::RootMapMenuIndex_Table));
    }

    wxMenuItem* LayerMenuHelper::GetLayerSubMenuItem()
    {
        if (__nullptr == m_layerMenuItem)
        {
            wxMenu* table_menu = GetTableMenu();

            m_layerMenuItem = table_menu->FindItemByPosition(rootmap::TableMenuItemIndex_LayerSubmenu);
            RmAssert(__nullptr != m_layerMenuItem, "Layer submenu item not found at expected position on table menu");
        }

        return m_layerMenuItem;
    }

    wxMenu* LayerMenuHelper::GetLayerSubMenu()
    {
        if (__nullptr == m_layerMenu)
        {
            wxMenu* layer_submenu = new wxMenu(_("Layer"));
            RmAssert(__nullptr != layer_submenu, "Could not create layer submenu");

            delete m_layerMenu; // Out with old, in with new?
            m_layerMenu = layer_submenu;
        }

        return m_layerMenu;
    }

    int LayerMenuHelper::FindIdPosition(int id)
    {
        size_t vec_size = m_layerMenuItemIds.size();
        for (size_t idx = 0; idx < vec_size; ++idx)
        {
            if (m_layerMenuItemIds[idx] == id)
            {
                return idx + 1;
            }
        }

        return (-1);
    }
} /* namespace rootmap */
