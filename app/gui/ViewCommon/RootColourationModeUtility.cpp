#include "app/gui/ViewCommon/RootColourationModeUtility.h"

// MSA TODO doc this class

namespace rootmap
{
    // Returns 0 if colours left unchanged
    int RootColourUtility::setColours(const RmColouration& mode, const long branch_order,
        float& r, float& g, float& b)
    {
        switch (mode)
        {
        case RM_COLOUR_BY_BRANCH_ORDER_SHADES:
        {
            float r_incr = (1.0F - r) / 6.0F;
            float g_incr = (1.0F - g) / 6.0F;
            float b_incr = (1.0F - b) / 6.0F;

            if (branch_order != 0)
            {
                if (branch_order < 4)
                {
                    r += (branch_order + 1) * r_incr;
                    g += (branch_order + 1) * g_incr;
                    b += (branch_order + 1) * b_incr;
                }
                else if (branch_order >= 4)
                {
                    r += 5 * r_incr;
                    g += 5 * g_incr;
                    b += 5 * b_incr;
                }
            }
        }
        break;
        case RM_COLOUR_BY_BRANCH_ORDER_HIGH_CONTRAST:
            switch (branch_order)
            {
            case 0:
                // Red
                r = 1.0F;
                g = 0.0F;
                b = 0.0F;
                break;
            case 1:
                // Green
                r = 0.0F;
                g = 1.0F;
                b = 0.0F;
                break;
            case 2:
                // Blue
                r = 0.0F;
                g = 0.0F;
                b = 1.0F;
                break;
            default: // Branch order 3 and above
                // Yellow
                r = 1.0F;
                g = 1.0F;
                b = 0.0F;
                break;
            }
            break;
        case RM_COLOUR_STATIC: // INTENTIONAL FALLTHROUGH
        default:
            // Leave RGB unchanged
            return 0;
        }
        // Doesn't support alpha at present
        return 1;
    }

    // Returns 0 if colours left as default
    int RootColourUtility::setColours(const RmColouration& mode, const long branch_order,
        float& r, float& g, float& b, const float defaultColour[4])
    {
        r = defaultColour[0];
        g = defaultColour[1];
        b = defaultColour[2];
        return setColours(mode, branch_order, r, g, b);
    }
} /* namespace rootmap */
