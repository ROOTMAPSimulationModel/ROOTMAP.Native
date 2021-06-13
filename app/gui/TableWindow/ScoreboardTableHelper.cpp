/////////////////////////////////////////////////////////////////////////////
// Name:        ScoreboardTableHelper.cpp
// Purpose:     Provides an bridge to various Scoreboard properties and behaviour
//              for the RmTableWindow to use.
// Created:     06/05/2006
// Author:      RvH
// $Date: 2009-07-07 02:40:29 +0800 (Tue, 07 Jul 2009) $
// $Revision: 67 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/TableWindow/ScoreboardTableHelper.h"

#include "app/gui/common/GuiUtility.h"

#include "simulation/scoreboard/ScoreboardCoordinator.h"
#include "simulation/scoreboard/ScoreboardCollaboration.h"
#include "simulation/scoreboard/Scoreboard.h"
#include "simulation/process/common/Process.h"

#include "core/scoreboard/ScoreboardUtility.h"
#include "core/common/RmAssert.h"
#include "core/utility/Utility.h"

namespace rootmap
{
    using Utility::CSMax;
    using Utility::CSMin;

    ScoreboardTableHelper::ScoreboardTableHelper
    (ScoreboardCoordinator& scoreboardcoordinator,
        const TableInformation& tableInfo)
        : m_scoreboardcoordinator(scoreboardcoordinator)
        , m_scoreboard(__nullptr)
        , m_valueSpace(__nullptr)
        , m_initialCoordinate(0, 0, 0)
        , m_finalCoordinate(0, 0, 0)
        , m_pivotCoordinate(0, 0, 0)
        , m_viewDirection(tableInfo.GetViewDirection())
        , m_bounds(0, 0, 0, 0)
        , m_layer(tableInfo.GetLayer())
        , m_characteristicIndex(InvalidCharacteristicIndex)
        , m_tableInfo(tableInfo)
        , m_inBroadcastChange(false)
    {
        m_scoreboard = scoreboardcoordinator.GetScoreboard(tableInfo.GetStratum());
        m_characteristicIndex = (tableInfo.GetProcess())->GetCharacteristicIndex(tableInfo.GetCharacteristicNumber());
        long largest_slice = m_scoreboard->GetSizeOfLargestSlice();
        m_valueSpace = new CharacteristicValue[largest_slice];

        fillSpace(refillFull);

        ::CCollaborator::DependUpon(m_scoreboard);
        m_scoreboard->DependUpon(this);
    }


    ScoreboardTableHelper::~ScoreboardTableHelper()
    {
        // MSA 09.12.07 Cancelling dependency is now performed by
        // RmTableWindow::DisconnectNotification(), which is called
        // by SimulationEngine::~SimulationEngine().
        // By the time we get to THIS destructor, all the ROOTMAP
        // simulation classes have been destroyed and we can no longer access them.
        //::CCollaborator::CancelDependency(m_scoreboard);
        //m_scoreboard->CancelDependency(this);
        delete[] m_valueSpace;
    }


    //std::string ScoreboardTableHelper::getActiveLayerString()
    //{
    //    return "active layer string";
    //}


    /*
    ProviderChanged [override]
    We only take notice if the changed provider is our scoreboard. There are two possibilities :
    a) IF it has changed entirely due to a simulation step having been completed THEN
        Re-fill the table with values.

    We need to update the EditCell even if it isn't in the current layer. When we change to that layer, or
    pivot (which at this stage pivots around the EditCell) the EditCell isn't updated.
     */
    void ScoreboardTableHelper::ProviderChanged(CCollaborator* aProvider, long reason, CCollaboratorInfo* info)
    {
        Scoreboard* scoreboard = dynamic_cast<Scoreboard *>(aProvider);

        if ((__nullptr != scoreboard)
            && (scoreboard->GetScoreboardStratum() == m_scoreboard->GetScoreboardStratum())
            )
        {
            if (scoreboardSimulationStepComplete == reason)
            {
                fillSpace(refillMessage);
            }
            else if ((scoreboardEdittedCell == reason) && (!m_inBroadcastChange))
            {
                EditCellInfo* eci = dynamic_cast<EditCellInfo *>(info);

                RmAssert(__nullptr != eci, "CollaboratorInfo not of type EditCellInfo");

                if (inCurrentLayer(eci->box))
                {
                    wxGridCellCoords cell;
                    projectCoordinate2Cell(cell, eci->box);

                    setCellValue(cell, eci->value);
                    GetView()->ForceRefresh();
                }
            }
            else // not a message we care about
            {
            }
        }
        else // the given provider is NOT our scoreboard
        {
            // TODO: who to pass this on to ?
            // CBureaucrat::ProviderChanged(aProvider, reason, info);
        }
    } // ScoreboardTableHelper::ProviderChanged


    /* GetInitialBoxCoordinate
    Just return the m_initialCoordinate instance variable */
    const BoxCoordinate& ScoreboardTableHelper::getInitialBoxCoordinate() const
    {
        return m_initialCoordinate;
    }

    /* GetInitialProjection
    Return the projection of the m_initialCoordinate instance variable */
    wxGridCellCoords ScoreboardTableHelper::getInitialProjection() const
    {
        wxGridCellCoords initial_cell;
        projectCoordinate2Cell(initial_cell, m_initialCoordinate);

        return initial_cell;
    }


    /* SetFinalBoxCoordinate */
    void ScoreboardTableHelper::setFinalBoxCoordinate(wxGridCellCoords& aFinalCell)
    {
        projectCell2Coordinate(m_finalCoordinate, aFinalCell);
    }

    /*
    updateProcess
    By the time this class gets this message, we can be sure that itsSupervisor has sorted out whether the
    update to a new process is necessary or not. We still need to make sure the present editCell gets updated
    before the relevant things disappear out from under its feet. We then just need to get the new
    CharacteristicOffset for the new process or characteristic index, then fill the m_valueSpace.
     */
    ValidationReply ScoreboardTableHelper::updateProcess
    (Process* newProcess,
        long characteristic_number)
    {
        ValidationReply checkWasOK = updateCheck();

        if (checkWasOK < vrTryAgain)
        {
            m_tableInfo.SetProcess(newProcess);
            m_tableInfo.SetCharacteristicNumber(characteristic_number);

            m_characteristicIndex = newProcess->GetCharacteristicIndex(characteristic_number);
            fillSpace(refillUpdate);
            updateSelection();
        } // is updated with the new characteristic's values
        return checkWasOK;
    }


    /*
    updateLayer
    By the time this class gets this message, we can be sure that itsSupervisor has sorted out whether the update TO A
    NEW LAYER is necessary or not. We still need to make sure the present editCell gets updated before the relevant
    things disappear out from under its feet. We then need to update the m_layer instance and fill the m_valueSpace. We don't
    update the Initial, Final, Pivot or Edit Indexes since we like them to stay where they are until specifically moved.
     */
    ValidationReply ScoreboardTableHelper::updateLayer(long newLayer, long delta)
    {
        ValidationReply checkWasOK = updateCheck();

        if (checkWasOK < vrTryAgain)
        {
            if (delta != 0)
            {
                newLayer = m_layer + delta;
            }
            m_layer = newLayer;
            m_tableInfo.SetLayer(newLayer);
            fillSpace(refillFull);
            updateSelection();

            // update the label's cell that describes which layer in the unseen direction we are looking at
            //        wxGridCellCoords interval_cell={0,0}; table_ColumnLabels->RefreshCell(interval_cell);
        }
        return checkWasOK;
    }


    /*
    updateViewDirection
    By the time this class gets this message, we can be sure that its TDirector Supervisor has sorted out
    whether the update to a different view direction is necessary or not. We still need to make sure the
    present editCell gets updated before the relevant things disappear out from under its feet. We then merely
    need to update the m_viewDirection instance, then fill the m_valueSpace.
    When changing viewDirection, we don't necessarily pivot around the Editor (although we DO at this stage).
     */
    ValidationReply ScoreboardTableHelper::updateViewDirection(ViewDirection newDirection)
    {
        ValidationReply checkWasOK = updateCheck();

        if (checkWasOK < vrTryAgain)
        {
            m_viewDirection = newDirection;
            m_tableInfo.SetViewDirection(newDirection);
            if (m_pivotCoordinate.isValid())
            {
                switch (m_viewDirection)
                {
                case vFront: m_layer = m_pivotCoordinate.y + 1;
                    break;
                case vSide: m_layer = m_pivotCoordinate.x + 1;
                    break;
                case vTop: m_layer = m_pivotCoordinate.z + 1;
                    break;
                } // switch (m_viewDirection)
            } // if the pivot index is valid
            else
            {
                m_layer = 1;
            }

            fillSpace(refillFull);
            updateSelection();
        } // if checkWasOK
        return checkWasOK;
    }// updateViewDirection

    /*
    updateMessage
    Called by XXXXXX::MessageUpdate when a message or timestamp's worth of messages
    have run. There are two things to do here
    1. Refill the data for the visible slice. This is done simply by calling fillSpace
     */
    void ScoreboardTableHelper::updateMessage()
    {
        fillSpace(refillUpdate);
    }

    /* updateCheck
    Gets called before an attempt to update layer or process/characteristic or view direction.*/
    ValidationReply ScoreboardTableHelper::updateCheck()
    {
        return (vrAllow);
    }

    /*
    updateSelection
    Here, we pester the living daylights out of ourselves by not leaving any in-line documentation
     */
    void ScoreboardTableHelper::updateSelection()
    {
        wxRect selection_rect(0, 0, 0, 0);

        // project the 3D selection into a single rectangle. If it is actually in
        // the current layer, projectVolume2Cells returns true
        if (projectVolume2Cells(selection_rect, m_initialCoordinate, m_finalCoordinate))
        {
            // if the selected volume cuts through this layer, select its slice/profile
            GetView()->SelectBlock(selection_rect.GetTop(),
                selection_rect.GetLeft(),
                selection_rect.GetBottom(),
                selection_rect.GetRight(),
                false);
        }
        else
        {
            // otherwise, clear the selection
            GetView()->ClearSelection();
        }

        GetView()->ForceRefresh();
    }

    /*
    setCellValue
    Perform surgery on the valueSpace, placing the required value
     */
    void ScoreboardTableHelper::setCellValue(const wxGridCellCoords& aCell, double aValue)
    {
        long valueIndex = (m_bounds.GetRight() * aCell.GetRow()) + aCell.GetCol();
        m_valueSpace[valueIndex] = aValue;
        //(**m_valueSpace)[valueIndex] = aValue;
    }


    /*
    GetCellValue
    Perform an extraction of a double from the valueSpace.
    CODE CROSS REFERENCES :    LScoreboard::SetBoxFloat() {in "LScoreboard.h"}
                            LScorebaord::GetBoxFloat() {in "LScoreboard.h"}
                            this::setCellValue()
     */
    double ScoreboardTableHelper::getCellValue(const wxGridCellCoords& aCell) const
    {
        long valueIndex = (m_bounds.GetRight() * aCell.GetRow()) + aCell.GetCol();
        return m_valueSpace[valueIndex];
        //    return (**m_valueSpace)[valueIndex];
    }


    /*
    updateCellValue
    Used for ProviderChanged
    By the time this member is called, we should have already confirmed that the given value is in this layer
    and is for this characteristic. All we need to do here is calculate the box's cell projection, stuff the value
    in, and set the view of the cell to be updated.
     */
    void ScoreboardTableHelper::updateCellValue(const BoxCoordinate& coordinate, double value)
    {
        wxGridCellCoords the_cell;
        projectCoordinate2Cell(the_cell, coordinate);
        setCellValue(the_cell, value);
#if defined ROOTMAP_TODO
        what to do ? For the moment we just force a full refresh
            refreshCell(the_cell);
#endif // #if defined ROOTMAP_TODO
        GetView()->ForceRefresh();
    }

    /*
    InCurrentLayer
    Check if the Given box's coordinate is in the currentLayer.
     */
    bool ScoreboardTableHelper::inCurrentLayer(const BoxCoordinate& coordinate) const
    {
        bool in_layer = false;
        switch (m_viewDirection)
        {
        case vFront:
            in_layer = (m_layer == coordinate.y);
            break;
        case vSide:
            in_layer = (m_layer == coordinate.x);
            break;
        case vTop:
            in_layer = (m_layer == coordinate.z);
            break;
        default:
            RmAssert(false, "Invalid View Direction");
            break;
        }// switch (m_viewDirection)

        return ((in_layer) && (coordinate.isValid()));
    }

    /*
    fillSpace
    Handles the mundane and loopy process of querying the Scoreboard for the
    characteristic values of the required Process for all the boxes to be displayed
    in the table.

    NOTE:   the rowIndex and boxIndex variables must be used from range for the
            scoreboard's indices.
     */
    void ScoreboardTableHelper::fillSpace(RefillType refill)
    {
        RmAssert((m_characteristicIndex > 0), "Invalid CharacteristicIndex");
        // if (m_characteristicIndex<0) return;

        if (refill >= refillFull)
        {
            resetTable(refill);
        }

        //#define FILLSPACE_LONG_METHOD
        long rows_to_get = m_bounds.GetBottom();
        long boxes_per_row = m_bounds.GetRight();
#if defined FILLSPACE_LONG_METHOD
        double value = 0.0;
#else
        CharacteristicValue* space_ptr = m_valueSpace;
#endif // #if defined FILLSPACE_LONG_METHOD

        switch (m_viewDirection)
        {
        case vSide: // get values within a YZ layer, Yrow by Yrow
        {
            for (long row_index = 1; row_index <= rows_to_get; ++row_index)
            {
                for (long col_index = 1; col_index <= boxes_per_row; ++col_index)
                {
#if defined FILLSPACE_LONG_METHOD
                    wxGridCellCoords coord(row_index - 1, col_index - 1);
                    value = m_scoreboard->GetCharacteristicValue(m_characteristicIndex, m_layer, col_index, row_index);
                    setCellValue(coord, value);
#else
                    *space_ptr = m_scoreboard->GetCharacteristicValue(m_characteristicIndex, m_layer, col_index, row_index);
                    ++space_ptr;
#endif // #if defined FILLSPACE_LONG_METHOD
                } // col_index for loop
            } // row_index for loop
            break;
        } // vSide clause

        case vFront: // get values within an XZ layer, Xrow by Xrow 
        {
            for (long row_index = 1; row_index <= rows_to_get; ++row_index)
            {
                for (long col_index = 1; col_index <= boxes_per_row; ++col_index)
                {
#if defined FILLSPACE_LONG_METHOD
                    wxGridCellCoords coord(row_index - 1, col_index - 1);
                    value = m_scoreboard->GetCharacteristicValue(m_characteristicIndex, m_layer, col_index, row_index);
                    setCellValue(coord, value);
#else
                    *space_ptr = m_scoreboard->GetCharacteristicValue(m_characteristicIndex, col_index, m_layer, row_index);
                    ++space_ptr;
#endif // #if defined FILLSPACE_LONG_METHOD
                } // col_index for loop
            } // row_index for loop
            break;
        } // vFront clause

        case vTop: // get values within an XY layer, Xrow by Xrow, COUNTING DOWN from
        { // the back, so as to have the front of the scoreboard at the bottom of the table
            for (long row_index = 1; row_index <= rows_to_get; ++row_index)
            {
                for (long col_index = 1; col_index <= boxes_per_row; ++col_index)
                {
#if defined FILLSPACE_LONG_METHOD
                    wxGridCellCoords coord(row_index - 1, col_index - 1);
                    value = m_scoreboard->GetCharacteristicValue(m_characteristicIndex, m_layer, col_index, row_index);
                    setCellValue(coord, value);
#else
                    *space_ptr = m_scoreboard->GetCharacteristicValue(m_characteristicIndex, col_index, (rows_to_get - row_index) + 1, m_layer);
                    ++space_ptr;
#endif // #if defined FILLSPACE_LONG_METHOD
                } // col_index for loop
            } // row_index for loop
            break;
        } // vTop clause
        } // switch (m_viewDirection)
    } // fillSpace


    /* resetTable
    Used by fillSpace when it needs to delete all the rows and make all new ones again */
    void ScoreboardTableHelper::resetTable(RefillType refill)
    {
        if (refill == refillFull)
        {
            const BoundaryArray& row_boundaries = m_scoreboard->GetBoundaryArray(GuiUtility::getDimensionForRows(m_viewDirection)); //getBoundaryArray_V(m_scoreboard, m_viewDirection);
            const BoundaryArray& col_boundaries = m_scoreboard->GetBoundaryArray(GuiUtility::getDimensionForColumns(m_viewDirection)); //getBoundaryArray_H(m_scoreboard, m_viewDirection);

            long int num_rows = row_boundaries.GetNumLayers();
            long int num_cols = col_boundaries.GetNumLayers();

#if defined ROOTMAP_APPARENTLY_UNNECESSARY
            all the wxGridTableBase functions for adding and deleting rows call
                this class - which doesn't implement it, doesn't really need to.
                Therefore, just do what's necessary and refresh the parent grid 
                as required
                DeleteRows(0, m_bounds.GetRight());  // GetNumberRows()); // Delete ALL the rows 
            DeleteCols(0, m_bounds.GetBottom()); // GetNumberCols()); // Delete ALL the columns 
            AppendRows(num_rows);           // add the new rows
            AppendCols(num_cols);           // add columns similarly
#endif // #if defined ROOTMAP_APPARENTLY_UNNECESSARY

#if defined ROOTMAP_TODO
            // get the boundary layer strings and put in seperate wxArrayString data member
#endif // #if defined ROOTMAP_TODO

            m_bounds.SetRight(num_cols);
            m_bounds.SetBottom(num_rows);
        }
    }


    void ScoreboardTableHelper::projectCell2Coordinate(BoxCoordinate& coordinate, const wxGridCellCoords& aCell) const
    {
        switch (m_viewDirection)
        {
        case vFront:
            coordinate.set(aCell.GetCol() + 1, m_layer, aCell.GetRow() + 1);
            break;
        case vSide:
            coordinate.set(m_layer, aCell.GetCol() + 1, aCell.GetRow() + 1);
            break;
        case vTop:
            coordinate.set(aCell.GetCol() + 1, (m_bounds.GetBottom() - aCell.GetRow()), m_layer);
            break;
        default:
            RmAssert(false, "Invalid View Direction");
            break;
        }// switch (m_viewDirection)
    }// projectCell2Index 


    //
    // Function:
    //  projectIndex2Cell 
    // 
    // Description:
    //  
    // 
    // Working Notes:
    //  The -1's are because the indexes start at 1, and wxGridCellCoords indexes start at zero.
    //
    void ScoreboardTableHelper::projectCoordinate2Cell(wxGridCellCoords& aCell, const BoxCoordinate& coordinate) const
    {
        switch (m_viewDirection)
        {
        case vFront:
            aCell.SetCol(coordinate.x - 1);
            aCell.SetRow(coordinate.z - 1);
            break;

        case vSide:
            aCell.SetCol(coordinate.y - 1);
            aCell.SetRow(coordinate.z - 1);
            break;

        case vTop:
            aCell.SetCol(coordinate.x - 1);
            aCell.SetRow(m_bounds.GetBottom() - coordinate.y);
            break;

        default:
            RmAssert(false, "Invalid View Direction");
            break;
        }// switch (m_viewDirection) 
    }// SetCellFromBVD 


    /* projectVolume2Cells */
    bool ScoreboardTableHelper::projectVolume2Cells
    (wxRect& aRect,
        const BoxCoordinate& coordinate1,
        const BoxCoordinate& coordinate2) const
    {
        bool is_in_volume = false;
        if ((ScoreboardUtility::IsValidBox(coordinate1, coordinate2))
            && (ScoreboardUtility::PlaneIsInVolume(m_layer, m_viewDirection, coordinate1, coordinate2)))
        {
            switch (m_viewDirection)
            {
            case vFront:
                aRect.SetTop(CSMin(coordinate1.z, coordinate2.z) - 1);
                aRect.SetBottom(CSMax(coordinate1.z, coordinate2.z));
                aRect.SetLeft(CSMin(coordinate1.x, coordinate2.x) - 1);
                aRect.SetRight(CSMax(coordinate1.x, coordinate2.x));
                break; // vFront 

            case vSide:
                aRect.SetTop(CSMin(coordinate1.z, coordinate2.z) - 1);
                aRect.SetBottom(CSMax(coordinate1.z, coordinate2.z));
                aRect.SetLeft(CSMin(coordinate1.y, coordinate2.y) - 1);
                aRect.SetRight(CSMax(coordinate1.y, coordinate2.y));
                break; // vSide 

            case vTop:
                aRect.SetTop(m_bounds.GetBottom() - CSMax(coordinate1.y, coordinate2.y));
                aRect.SetBottom(m_bounds.GetBottom() - CSMin(coordinate1.y, coordinate2.y) + 1);
                aRect.SetLeft(CSMin(coordinate1.x, coordinate2.x) - 1);
                aRect.SetRight(CSMax(coordinate1.x, coordinate2.x));
                break; // vTop 
            }// switch (m_viewDirection)

            is_in_volume = true;
        }
        else
        {
            aRect.SetTopLeft(wxPoint(0, 0));
            aRect.SetSize(wxSize(0, 0));
        }

        return is_in_volume;
    }

    int ScoreboardTableHelper::GetNumberRows()
    {
        rootmap::Dimension dim = Z; // works for ViewDirection = vFront and vSide
        if (vTop == m_viewDirection)
        {
            dim = Y;
        }

        return (m_scoreboard->GetNumLayers(dim));
    }

    int ScoreboardTableHelper::GetNumberCols()
    {
        rootmap::Dimension dim = X; // works for ViewDirection = vFront and vTop
        if (vSide == m_viewDirection)
        {
            dim = Y;
        }

        return (m_scoreboard->GetNumLayers(dim));
    }

    // NOTE 10.06.17: not currently implemented
    bool ScoreboardTableHelper::IsEmptyCell(int /* row */, int /* col */)
    {
        return false;
    }

    wxString ScoreboardTableHelper::GetValue(int row, int col)
    {
        double value = getCellValue(wxGridCellCoords(row, col));
        return wxString(Utility::ToString(value).c_str());
    }

    void ScoreboardTableHelper::SetValue(int row, int col, const wxString& value)
    {
        double dvalue = Utility::StringToDouble(value.c_str());
        setCellValue(wxGridCellCoords(row, col), dvalue);
    }

    void ScoreboardTableHelper::SetRowLabelValue(int row, const wxString& s)
    {
        wxGridTableBase::SetRowLabelValue(row, s);
    }

    void ScoreboardTableHelper::SetColLabelValue(int col, const wxString& s)
    {
        wxGridTableBase::SetColLabelValue(col, s);
    }

    wxString ScoreboardTableHelper::GetRowLabelValue(int row)
    {
        RmAssert(row < static_cast<int>(m_rowLabels.GetCount()), "Invalid row specified for GetRowLabelValue");
        return m_rowLabels[row];
    }

    wxString ScoreboardTableHelper::GetColLabelValue(int col)
    {
        RmAssert(col < static_cast<int>(m_colLabels.GetCount()), "Invalid column specified for GetColLabelValue");
        return m_colLabels[col];
    }

    wxString ScoreboardTableHelper::GetTypeName(int /* row */, int /* col */)
    {
        return (wxGRID_VALUE_FLOAT);
    }

    bool ScoreboardTableHelper::CanGetValueAs(int /* row */, int /* col */, const wxString& /* typeName */)
    {
        return (wxGRID_VALUE_FLOAT);
    }

    bool ScoreboardTableHelper::CanSetValueAs(int /* row */, int /* col */, const wxString& typeName)
    {
        return (wxGRID_VALUE_FLOAT == typeName);
    }

    double ScoreboardTableHelper::GetValueAsDouble(int row, int col)
    {
        return getCellValue(wxGridCellCoords(row, col));
    }

    void ScoreboardTableHelper::SetValueAsDouble(int row, int col, double value)
    {
        wxGridCellCoords cell(row, col);
        setCellValue(cell, value);

        //
        EditCellInfo* eci = new EditCellInfo;
        eci->characteristicIndex = m_characteristicIndex;
        eci->value = value;
        projectCell2Coordinate(eci->box, cell);
        eci->stratum = m_scoreboard->GetScoreboardStratum();

        m_inBroadcastChange = true;
        BroadcastChange(scoreboardEdittedCell, eci);
        delete eci;
        m_inBroadcastChange = false;
    }

    void ScoreboardTableHelper::UpdateColumnLabels()
    {
        const BoundaryArray& col_boundaries = m_scoreboard->GetBoundaryArray(GuiUtility::getDimensionForColumns(m_viewDirection)); //getBoundaryArray_H(m_scoreboard, m_viewDirection);
        size_t num_cols = col_boundaries.GetNumLayers();
        m_colLabels.Empty();
        for (size_t col = 1; col <= num_cols; ++col)
        {
            double layer_start = col_boundaries.GetLayerStart(col);
            double layer_end = col_boundaries.GetLayerEnd(col);

            std::string s = rootmap::Utility::ToString(layer_start, 2, true);
            s += " - ";
            s += rootmap::Utility::ToString(layer_end, 2, true);

            m_colLabels.Add(s.c_str());
        }
    }

    void ScoreboardTableHelper::UpdateRowLabels()
    {
        const BoundaryArray& row_boundaries = m_scoreboard->GetBoundaryArray(GuiUtility::getDimensionForRows(m_viewDirection)); //getBoundaryArray_V(m_scoreboard, m_viewDirection);
        size_t num_rows = row_boundaries.GetNumLayers();
        m_rowLabels.Empty();
        for (size_t row = 1; row <= num_rows; ++row)
        {
            double layer_start = row_boundaries.GetLayerStart(row);
            double layer_end = row_boundaries.GetLayerEnd(row);

            std::string s = rootmap::Utility::ToString(layer_start, 2, true);
            s += " - ";
            s += rootmap::Utility::ToString(layer_end, 2, true);

            m_rowLabels.Add(s.c_str());
        }
    }
} /* namespace rootmap */
