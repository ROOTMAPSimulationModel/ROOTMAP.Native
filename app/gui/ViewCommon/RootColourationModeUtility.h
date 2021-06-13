#ifndef RootColourationModeUtility_H
#define RootColourationModeUtility_H
#include "app/gui/ViewCommon/Types.h"

namespace rootmap
{
    /**
       * RGB params should be GLfloats, but for some reason
       * including "gl/gl.h" makes the MSVC++ include file break horribly
     */
    class RootColourUtility
    {
    public:
        static int setColours(const RmColouration& mode,
            const long branch_order,
            float& r, float& g, float& b);
        static int setColours(const RmColouration& mode,
            const long branch_order,
            float& r, float& g, float& b,
            const float defaultColour[4]);
    }; // class RootColourUtility
}
#endif // #ifndef RootColourationModeUtility_H
