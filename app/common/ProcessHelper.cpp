/////////////////////////////////////////////////////////////////////////////
// Name:        ProcessHelper.cpp
// Purpose:     Implementation of the ProcessHelper class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2008-06-16 02:07:36 +0800 (Mon, 16 Jun 2008) $
// $Revision: 1 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/common/ProcessHelper.h"

#include "simulation/process/common/Process.h"

#include "core/utility/Utility.h"

namespace rootmap
{
    ProcessHelper::ProcessHelper(Process* p)
        : m_process(p)
    {
    }

    ProcessHelper::~ProcessHelper()
    {
    }

    ValidationReply ProcessHelper::ValidateCharacteristicInput
    (long characteristic_number,
        double& input_value)
    {
        ValidationReply validator = vrAllow;
        // Firstly, set up the max and min values that we shall use for validation
        double min_val = m_process->GetCharacteristicMinimum(characteristic_number);
        double max_val = m_process->GetCharacteristicMaximum(characteristic_number);
        double use_val;
        std::string max_str = "";
        std::string min_str = "";
        std::string input_str = "";

        // Now, check if the input is above the maximum, or below the minimum.
        if (input_value > max_val)
        {
            use_val = max_val;
            max_str = Utility::ToString(max_val); // sprintCellText(max_val, 160, max_str);
        }
        else if (input_value < min_val)
        {
            use_val = min_val;
            min_str = Utility::ToString(min_val); // sprintCellText(min_val, 170, min_str);
        }
        else
        {
            return vrAllow;
        }

#if defined ROOTMAP_TODO
        // KLUDGE !! 120 is the width of the cells. I think.
        sprintCellText(*input_value, 120, input_str);
        ParamText(min_str, max_str, input_str, "\p");


        // The Dialog
        InitCursor();
        DialogPtr dlog = GetNewDialog(DLOGValueValidation, 0, (WindowPtr)-1L);
        PositionDialog('DLOG', DLOGValueValidation);

        /* item 6 is the user item to surround the default button */
        SetUpDefaultButton(dlog, 6);

        ShowWindow(dlog);    // since the DLOG is not initially visible

        short int response;
        while ((response < buttonConstrain) || (response > buttonCancel))
            ModalDialog(sProcess_ValidationDialogFilter, &response);

        DisposeDialog(dlog); dlog = __nullptr;

        validator = (ValidationReply)response;

        if (validator == vrRestrict)
            *input_value = use_val;
#endif // #if defined ROOTMAP_TODO

        return validator;
    }
} /* namespace rootmap */
