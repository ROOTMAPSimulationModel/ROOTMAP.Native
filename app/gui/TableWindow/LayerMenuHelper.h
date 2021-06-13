#ifndef LayerMenuHelper_H
#define LayerMenuHelper_H
/////////////////////////////////////////////////////////////////////////////
// Name:        LayerMenuHelper.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     24/06/2006 20:16:57
// RCS-ID:      
// Copyright:   �2006 University of Tasmania
// Licence:     
/////////////////////////////////////////////////////////////////////////////
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "LayerMenuHelper.h"
#endif

/*!
 * Includes
 */
#include "app/common/Types.h"
#include "simulation/common/Types.h"
#include "core/common/Structures.h"
#include "core/log/Logger.h"

#include <vector>

class RmTableWindow;

namespace rootmap
{
    /*!
     * Forward declarations
     */
    class ScoreboardTableHelper;
    class TableInformation;

    class LayerMenuHelper
    {
    public:
        /// Constructors

        LayerMenuHelper(RmTableWindow& tableWindow,
            ScoreboardTableHelper& scoreboardtablehelper);

        virtual ~LayerMenuHelper();
        /**
         *
         */
        const rootmap::TableInformation& GetTableInformation() const;

        /**
         * Deletes the previous layers strings and rebuilds array
         */
        void RebuildLayerStrings();

        /**
         *
         */
        void RebuildLayerMenu();

        void UpdateLayer();

        void ReplaceLayerMenu();

        size_t GetNumLayers() const;

        wxString GetActiveItemString();

        wxMenu* GetTableMenu();

        /**
         * Thus named to avoid any possibility of confusion with a wx function name
         */
        wxMenu* GetLayerSubMenu();

        wxMenuItem* GetLayerSubMenuItem();

        int FindIdPosition(int id);

    private:
        RootMapLoggerDeclaration();

        void ClearLayerMenu();

        /**
         * Builds up the layer menu with m_layerStrings.
         *
         * Does NOT clear the menu, use ClearLayerMenu first.
         */
        void BuildLayerMenu();

        /**
         * Reference to the owner of TableInformation
         */
        rootmap::ScoreboardTableHelper& m_helper;

        /**
         * The table window is the primary contact with window stuff like menus
         */
        RmTableWindow& m_parent;

        /**
         * Container of strings placed into the menu (EXCEPT: the initial disabled item)
         */
        wxArrayString m_layerStrings;

        /**
         * Container of identifiers corresponding to the layer string menu items
         */
        std::vector<long> m_layerMenuItemIds;

        /**
         * The menu item ID of the initial disabled label item
         */
        long m_labelId;

        /**
         * The submenu we manage and own.
         */
        wxMenu* m_layerMenu;

        /**
         * The item on the Table menu that contains our submenu.
         * This is required for replacing any previous entry with ours.
         */
        wxMenuItem* m_layerMenuItem;
    };

    inline size_t LayerMenuHelper::GetNumLayers() const
    {
        return m_layerStrings.GetCount();
    }
#endif
} /* namespace rootmap */
