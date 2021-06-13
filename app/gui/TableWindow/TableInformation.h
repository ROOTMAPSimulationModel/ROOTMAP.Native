#ifndef TableInformation_H
#define TableInformation_H
/////////////////////////////////////////////////////////////////////////////
// Name:        TableInformation.h
// Purpose:     Declaration of the TableInformation class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2008-06-16 02:07:36 +0800 (Mon, 16 Jun 2008) $
// $Revision: 1 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "core/common/Types.h"
#include "core/common/structures.h"
#include "core/scoreboard/ScoreboardStratum.h"

namespace rootmap
{
    class Process;

    class TableInformation
    {
    public:
        // TableInformation();
        TableInformation(Process* p,
            long characteristic_number,
            ViewDirection viewdir,
            long layer,
            const ScoreboardStratum& stratum,
            long menuID);
        virtual ~TableInformation();

        /**
         * ThisFunctionName
         *
         * Stuff about the what the function does relative to the scheme of things.
         * A good temporary place for design-level documentation.
         *
         * @param
         * @return
         */
        Process* GetProcess() const;
        long GetCharacteristicNumber() const;
        ViewDirection GetViewDirection() const;
        long GetLayer() const;
        const ScoreboardStratum& GetStratum() const;
        long GetMenuID() const;
        bool NeedsAssistanceWithDefaults() const;

        void SetProcess(Process* p);
        void SetCharacteristicNumber(long characteristic_number);
        void SetViewDirection(ViewDirection viewdir);
        void SetLayer(long layer);
        void SetStratum(const ScoreboardStratum& stratum);
        void SetMenuID(long menuID);

        void SetDefaulted();
        void SetUsed();
    private:
        ///
        /// an internally used state that tracks how this piece of information
        /// has so far been utilised.
        /// 
        /// Initialised   - created with zero-ish values. In this state, the
        ///                 information should first be "assisted with defaults"
        /// Defaulted     - the information has been set to default values that
        ///                 are considered useful to the current simulation.
        ///                 The information can safely be used.
        /// Used          - at which stage the information has also been
        ///                 utilised to populate a TableWindow, so should be
        ///                 "saved" - there is a better word for this and i know it damnit.
        /// 
        enum PhaseOfUse
        {
            Initialised,
            Defaulted,
            Used
        };


        ///
        /// default = PTemperature (all volumes)
        Process* m_process;

        ///
        /// default = -1
        long m_characteristicnumber;

        ///
        /// default = vFront
        ViewDirection m_viewdirection;
        // default = vFront

        ///
        long m_layer;
        // default = 1

        ///
        /// default = NONE
        ScoreboardStratum m_stratum;

        ///
        /// default = 0
        long m_menuID;


        ///
        /// default = Initialising
        PhaseOfUse m_phase;
    }; // class TableInformation

    inline TableInformation::TableInformation(Process* p, long characteristic_number, ViewDirection viewdir, long layer, const ScoreboardStratum& stratum, long menuID)
        : m_process(p)
        , m_characteristicnumber(characteristic_number)
        , m_viewdirection(viewdir)
        , m_layer(layer)
        , m_stratum(stratum)
        , m_menuID(menuID)
        , m_phase(TableInformation::Initialised)
    {
    }

    inline TableInformation::~TableInformation()
    {
    }

    inline Process* TableInformation::GetProcess() const
    {
        return m_process;
    }

    inline long TableInformation::GetCharacteristicNumber() const
    {
        return m_characteristicnumber;
    }

    inline ViewDirection TableInformation::GetViewDirection() const
    {
        return m_viewdirection;
    }

    inline long TableInformation::GetLayer() const
    {
        return m_layer;
    }

    inline const ScoreboardStratum& TableInformation::GetStratum() const
    {
        return m_stratum;
    }

    inline long TableInformation::GetMenuID() const
    {
        return m_menuID;
    }

    inline bool TableInformation::NeedsAssistanceWithDefaults() const
    {
        return (m_phase < Defaulted);
    }

    inline void TableInformation::SetProcess(Process* p)
    {
        m_process = p;
    }

    inline void TableInformation::SetCharacteristicNumber(long characteristic_number)
    {
        m_characteristicnumber = characteristic_number;
    }

    inline void TableInformation::SetViewDirection(ViewDirection viewdir)
    {
        m_viewdirection = viewdir;
    }

    inline void TableInformation::SetLayer(long layer)
    {
        m_layer = layer;
    }

    inline void TableInformation::SetStratum(const ScoreboardStratum& stratum)
    {
        m_stratum = stratum;
    }

    inline void TableInformation::SetMenuID(long menuID)
    {
        m_menuID = menuID;
    }

    inline void TableInformation::SetDefaulted()
    {
        m_phase = Defaulted;
    }

    inline void TableInformation::SetUsed()
    {
        m_phase = Used;
    }
} /* namespace rootmap */

#endif // #ifndef TableInformation_H
