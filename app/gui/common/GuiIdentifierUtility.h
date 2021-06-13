//
// File:
//  GuiIdentifierUtility.h
//
// Description:
//  contains constants and functions for maintaining various gui ids, such as -
//  * the next layer menu item id
//
//
#ifndef GuiIdentifierUtility_H
#define GuiIdentifierUtility_H

namespace rootmap
{
    class GuiIdentifierUtility
    {
    public:
        ///
        int useNextLayerMenuItemIdentifier();
        int peekNextLayerMenuItemIdentifier();

        void getLayerMenuItemIdentifierRange(int& min, int& max);

        static GuiIdentifierUtility& instance();

    private:

        GuiIdentifierUtility();
        virtual ~GuiIdentifierUtility();

        ///
        /// the next available layer menu item identifier
        int m_nextLayerMenuItemIdentifier;

        static const int m_layerMenuItemMin;
        static const int m_layerMenuItemMax;
    }; // class GuiIdentifierUtility
} /* namespace rootmap */

#endif // #ifndef GuiIdentifierUtility_H
