#ifndef ScoreboardColourBoxes_H
#define ScoreboardColourBoxes_H
/////////////////////////////////////////////////////////////////////////////
// Name:        ScoreboardColourBoxes.h
// Purpose:     Declaration of the ClassTemplate class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2008-06-16 02:07:36 +0800 (Mon, 16 Jun 2008) $
// $Revision: 1 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/ViewWindow/ScoreboardBuffer.h"

#include "simulation/common/Types.h"

#include "core/log/Logger.h"

namespace rootmap
{
    struct EditCellInfo;
    class Scoreboard;

    //     struct CharacteristicColourUpdateInfo
    //     {
    //         bool                update[kYellow+1];
    //         wxGridCellCoords    cell;
    //         DoubleRect          soilRect;
    //     };


    class ScoreboardColourBoxes : public ScoreboardBuffer
    {
    public:

        ScoreboardColourBoxes(ScoreboardCoordinator& scoord,
            const ScoreboardFlags& scoreboardFlags);

        virtual ~ScoreboardColourBoxes();

        virtual void SetCharacteristic(CMYSelect cmy, const CharacteristicColourInfo& info);
        virtual void SetCharacteristicColourInfo(const CharacteristicColours_t& info);
        bool UpdateBoxCharacteristic(EditCellInfo* eci);
        virtual void DrawScoreboard(const DoubleRect& area, Scoreboard* scoreboard);

    private:
        RootMapLoggerDeclaration();

        CharacteristicColours_t m_colourInfo;
    };
} /* namespace rootmap */

#endif // #ifndef ScoreboardColourBoxes_H
