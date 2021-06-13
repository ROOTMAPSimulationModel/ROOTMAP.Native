#ifndef ViewBufferStructures_H
#define ViewBufferStructures_H

#include "app/gui/ViewCommon/Types.h"
#include "core/macos_compatibility/CCollaborator.h"
#include "core/common/DoubleCoordinates.h"
#include "core/common/Types.h"
// wxRect
#include "wx/gdicmn.h"

namespace rootmap
{
    /**
     *
     */
    struct ViewBufferChangeInfo : public CCollaboratorInfo
    {
        wxRect bci_BufferSize;
        DoublePt bci_ReferencePt;
        double bci_Scale;

        virtual void toStream(std::ostream& ostr) const;
    };

    std::ostream& operator<<(std::ostream& ostr, const ViewBufferChangeInfo& info);

    /**
     *
     *
     */
    struct ScoreboardBufferChangeInfo : public ViewBufferChangeInfo
    {
        //wxRect      bci_WindowBounds;
        DoubleRect bci_ScoreboardBounds;
        DoubleRect bci_SoilFrame;

        virtual void toStream(std::ostream& ostr) const;
    };

    std::ostream& operator<<(std::ostream& ostr, const ScoreboardBufferChangeInfo& info);

    /**
     *
     */
    struct ResizeBufferChangeInfo : public ScoreboardBufferChangeInfo
    {
    };


    /**
     *
     */
    struct WrapRepeatBufferChangeInfo : public ScoreboardBufferChangeInfo
    {
    };


    /**
     * Additional stuff that secondary buffers might need to change View Direction.
     */
    class BoundaryArray;

    struct ViewDirectionBufferChangeInfo : public ScoreboardBufferChangeInfo
    {
        BoundaryArray* bci_VLayers;
        BoundaryArray* bci_HLayers;

        virtual void toStream(std::ostream& ostr) const;
    };

    std::ostream& operator<<(std::ostream& ostr, const ViewDirectionBufferChangeInfo& info);
} /* namespace rootmap */
#endif // #ifndef ViewBufferStructures_H
