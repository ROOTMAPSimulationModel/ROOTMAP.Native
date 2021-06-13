#ifndef ScoreboardTableHelper_H
#define ScoreboardTableHelper_H
/////////////////////////////////////////////////////////////////////////////
// Name:        ScoreboardTableHelper.h
// Purpose:     Provides an bridge to various Scoreboard properties and behaviour
//              for the RmTableWindow to use.
// Created:     06/05/2006
// Author:      RvH
// $Date: 2009-07-07 02:40:29 +0800 (Tue, 07 Jul 2009) $
// $Revision: 67 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "core/common/Structures.h"
#include "core/scoreboard/ScoreboardBox.h"
#include "core/scoreboard/ScoreboardStratum.h"
#include "core/macos_compatibility/CCollaborator.h"

#include "simulation/common/Types.h"

#include "app/common/Types.h"
#include "app/gui/TableWindow/TableInformation.h"

#include "wx/grid.h"

namespace rootmap
{
    class ScoreboardCoordinator;
    class Scoreboard;
    class Process;
    class BoundaryArray;

    class ScoreboardTableHelper
        : public ::CCollaborator
        , public ::wxGridTableBase
    {
    public:
        enum RefillType
        {
            refillNONE,
            refillMessage,
            refillUpdate,
            refillFull
        };

        ScoreboardTableHelper(
            ScoreboardCoordinator& scoreboardcoordinator,
            const rootmap::TableInformation& tableInfo);
        virtual ~ScoreboardTableHelper();

        virtual ValidationReply updateProcess(Process* process, long characteristic_number);
        // if delta>0 then newLayer is ignored
        virtual ValidationReply updateLayer(long newLayer, long delta = 0);
        virtual ValidationReply updateViewDirection(ViewDirection newDirection);
        virtual void updateMessage();

        const TableInformation& GetTableInformation() const;

        virtual Scoreboard& getScoreboard();

        /**
         * CCollaborator override
         * TODO: investigate if wxwidgets has a corresponding paradigm
         */
        void ProviderChanged(CCollaborator* aProvider, long reason, CCollaboratorInfo* info);

        virtual const BoxCoordinate& getInitialBoxCoordinate() const;
        virtual wxGridCellCoords getInitialProjection() const;
        virtual void setFinalBoxCoordinate(wxGridCellCoords& aFinalCell);

        // ///////////////////////////
        //
        // miscellaneous support implementations
        //
        // ///////////////////////////
        void UpdateColumnLabels();
        void UpdateRowLabels();

        // Updating
    protected:
        virtual ValidationReply updateCheck();// formerly CheckAndUpdate()
        virtual void updateSelection();// formerly UpdateEditorAndSelection()

        // Query
        virtual void setCellValue(const wxGridCellCoords& aCell, double aValue);
        virtual double getCellValue(const wxGridCellCoords& aCell) const;
        virtual void updateCellValue(const BoxCoordinate& coordinate, double value);
        virtual bool inCurrentLayer(const BoxCoordinate& coordinate) const;

        // Utility
        virtual void fillSpace(RefillType refill);
        virtual void resetTable(RefillType refill);


    protected:
        virtual void projectCell2Coordinate(BoxCoordinate& coordinate, const wxGridCellCoords& aCell) const;
        virtual void projectCoordinate2Cell(wxGridCellCoords& aCell, const BoxCoordinate& coordinate) const;
        virtual bool projectVolume2Cells(wxRect& aRect, const BoxCoordinate& coordinate1, const BoxCoordinate& coordinate2) const;

        // ///////////////////////////
        //
        // wxGridTableBase concrete implementations
        //
        // ///////////////////////////
        virtual int GetNumberRows();
        virtual int GetNumberCols();
        virtual bool IsEmptyCell(int row, int col);
        virtual wxString GetValue(int row, int col);
        virtual void SetValue(int row, int col, const wxString& value);

        // ///////////////////////////
        //
        // wxGridTableBase also-required implementations
        //
        // ///////////////////////////
        void SetRowLabelValue(int row, const wxString&);
        void SetColLabelValue(int col, const wxString&);
        wxString GetRowLabelValue(int row);
        wxString GetColLabelValue(int col);

        wxString GetTypeName(int row, int col);
        bool CanGetValueAs(int row, int col, const wxString& typeName);
        bool CanSetValueAs(int row, int col, const wxString& typeName);
        double GetValueAsDouble(int row, int col);
        void SetValueAsDouble(int row, int col, double value);


    private:
        ///
        ///
        ScoreboardCoordinator& m_scoreboardcoordinator;

        ///
        ///
        Scoreboard* m_scoreboard;

        ///
        /// a 2D array which contains one layer of summaries for one
        /// characteristic (the one being displayed). The array is sized to the
        /// largest layer of the current scoreboard
        CharacteristicValueArray m_valueSpace;

        ///
        /// first box clicked in, w.r.t. 3D selected volumes 
        BoxCoordinate m_initialCoordinate;

        ///
        /// Shift-Clicked box, w.r.t. 3D selected volumes 
        BoxCoordinate m_finalCoordinate;

        ///
        /// box about which we "pivot" when rotating 
        BoxCoordinate m_pivotCoordinate;

        ///
        /// front, side or top? 
        ViewDirection m_viewDirection;

        ///
        ///
        wxRect m_bounds;

        ///
        /// the index of the x, y or z layer in the direction we are looking
        /// (1-based)
        long m_layer;

        ///
        /// 
        CharacteristicIndex m_characteristicIndex; // the index of values in the scoreboard's boxes 

        ///
        /// 
        wxArrayString m_rowLabels;

        ///
        /// 
        wxArrayString m_colLabels;

        ///
        /// 
        TableInformation m_tableInfo;

        ///
        /// 
        bool m_inBroadcastChange;

    private:
        ScoreboardTableHelper(const ScoreboardTableHelper&);
        ScoreboardTableHelper& operator=(const ScoreboardTableHelper&);
    };

    enum
    {
        scoreboardtableChangeViewDirection = 'sbt0',
        scoreboardtableChangeLayer,
        scoreboardtableChangeCharacteristic
    };

    inline Scoreboard& ScoreboardTableHelper::getScoreboard()
    {
        return *m_scoreboard;
    }

    inline const TableInformation& ScoreboardTableHelper::GetTableInformation() const
    {
        return m_tableInfo;
    }
} /* namespace rootmap */

#endif // #ifndef ScoreboardTableHelper_H
