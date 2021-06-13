/////////////////////////////////////////////////////////////////////////////
// Name:        ViewBufferStructure.cpp
// Purpose:     Implementation of the ViewBufferStructure class
// Created:     09-07-2009 20:52:04
// Author:      RvH
// $Date$
// $Revision$
// Copyright:   ©2002-2009 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/ViewWindow/ViewBufferStructures.h"

namespace rootmap
{
    void ViewBufferChangeInfo::toStream(std::ostream& /*ostr*/) const
    {
        /*TODO ostr << "{BufferSize:" << ToString(bci_BufferSize)
              << ", Scale:" << bci_Scale
              << ", ReferencePt:" << ToString(bci_ReferencePt) << "}";*/
    }

    void ScoreboardBufferChangeInfo::toStream(std::ostream& /*ostr*/) const
    {
        /*TODO ostr << "{ ";
         ViewBufferChangeInfo::toStream(ostr);
         ostr << ", ScoreboardBounds:" << ToString(bci_ScoreboardBounds)
              << ", SoilFrame:" << ToString(bci_SoilFrame) << "}";*/
    }

    void ViewDirectionBufferChangeInfo::toStream(std::ostream& ostr) const
    {
        //ostr << "{ ";
        ScoreboardBufferChangeInfo::toStream(ostr);
        //ostr << ", VLayers:" << *bci_VLayers
        //     << ", HLayers:" << *bci_HLayers << "}";
    }

    std::ostream& operator<<(std::ostream& ostr, const ViewBufferChangeInfo& info)
    {
        info.toStream(ostr);
        return ostr;
    }

    std::ostream& operator<<(std::ostream& ostr, const ScoreboardBufferChangeInfo& info)
    {
        info.toStream(ostr);
        return ostr;
    }

    std::ostream& operator<<(std::ostream& ostr, const ViewDirectionBufferChangeInfo& info)
    {
        info.toStream(ostr);
        return ostr;
    }
} /* namespace rootmap */
