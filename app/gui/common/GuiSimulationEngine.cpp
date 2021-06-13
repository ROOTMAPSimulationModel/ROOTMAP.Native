/////////////////////////////////////////////////////////////////////////////
// Name:        GuiSimulationEngine.cpp
// Purpose:     Implementation of the GuiSimulationEngine class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2009-07-07 02:40:29 +0800 (Tue, 07 Jul 2009) $
// $Revision: 67 $
// Copyright:   ©2006 University of Tasmania
/////////////////////////////////////////////////////////////////////////////
#include "app/gui/common/GuiSimulationEngine.h"
#include "app/gui/TableWindow/RmTableWindow.h"
#include "app/gui/TableWindow/TableInformation.h"
#include "app/gui/ViewWindow/RmViewWindow.h"
#include "app/gui/View3DWindow/RmView3D.h"
#include "app/gui/ViewCommon/ViewInformation.h"
#include "app/gui/ViewCommon/View3DInformation.h"
#include "app/gui/MDI/RmParentFrame.h"
#include "app/gui/ProgressDialog/RmProgressWindow.h"

#include "simulation/common/Types.h"
#include "simulation/data_access/common/DataAccessManager.h"
#include "simulation/data_access/interface/TableDAI.h"
#include "simulation/data_access/interface/ViewDAI.h"
#include "simulation/process/common/ProcessCoordinator.h"
#include "simulation/process/common/Process.h"
#include "simulation/process/raytrace/ScoreboardColourInfo.h"
#include "simulation/scoreboard/ScoreboardCoordinator.h"
#include "simulation/scoreboard/Scoreboard.h"

#include "core/common/ExtraStuff.h"
#include "core/common/RmAssert.h"

#include <algorithm>

namespace rootmap
{
    GuiSimulationEngine::GuiSimulationEngine(DataAccessManager& dam, RmParentFrame* parent)
        : SimulationEngine(dam)
        , m_parentMDI(parent)
        , m_defaultViewInformation(__nullptr)
        , m_defaultView3DInformation(__nullptr)
        , m_progressWindow(__nullptr)
    {
        //eDefaultViewInformation->SetScoreboard(stratum.value(), true);
        //UpdateDefaultViewInformationTopLeft(stratum);
    }

    GuiSimulationEngine::~GuiSimulationEngine()
    {
        for (TableInformationCollection::iterator titer = m_defaultTableInformation.begin();
            titer != m_defaultTableInformation.end(); ++titer)
        {
            TableInformation* table_info = (*titer).second;
            delete table_info;
        }

        delete m_defaultViewInformation;
        delete m_defaultView3DInformation;

        // MSA 09.12.08 Ugly iteration of ISimulationActivityObservers to account for the fact that
        // some implementations of DisconnectNotification() call SimulationEngine::RemoveSimulationActivityObserver()
        if (!m_simulationActivityObservers.empty())
        {
            ISimulationActivityObserver* prevptr = m_simulationActivityObservers.front();
            while (true)
            {
                m_simulationActivityObservers.front()->DisconnectNotification();
                // If our call to DisconnectNotification didn't remove the pointer at front(), we will have to.
                if (prevptr == m_simulationActivityObservers.front()) { m_simulationActivityObservers.pop_front(); }

                if (m_simulationActivityObservers.empty()) { break; }
                prevptr = m_simulationActivityObservers.front();
            }
        }
    }

    void GuiSimulationEngine::OnMenuOpen(wxMenuEvent& /* event */)
    {
    }

    void GuiSimulationEngine::Initialise()
    {
        // performs the construction and initialisation phases
        SimulationEngine::Initialise();

        // performs the visualisation phase
        NewSimulationWindows();
    }

    void GuiSimulationEngine::NewSimulationWindows()
    {
        GetDataAccessManager().visualiseTables(this);
        GetDataAccessManager().visualiseViews(this);
        GetDataAccessManager().visualiseView3D(this);

        ShowProgressWindow();
    }

    void GuiSimulationEngine::NewTableWindow(const ScoreboardStratum& stratum)
    {
        ScoreboardCoordinator& sbcoord = GetScoreboardCoordinator();
        TableInformationCollection::iterator fnd = m_defaultTableInformation.find(stratum.value());
        if (fnd == m_defaultTableInformation.end())
        {
            wxLogError(wxT(HERE "Could not find table information for stratum %d"), stratum.value());
            return;
        }
        TableInformation* table_info = (*fnd).second;
        table_info->SetUsed();

        RmTableWindow* window = new RmTableWindow(m_parentMDI, this, sbcoord, *table_info);
        window->Raise();
        window->Show(true);
        RegisterSimulationActivityObserver(window);
    }

    void GuiSimulationEngine::NewViewWindow()
    {
        if (__nullptr == m_defaultViewInformation)
        {
            m_defaultViewInformation = new ViewInformation;
            AssistViewInformationDefaults(m_defaultViewInformation);
        }

        RmViewWindow* window = new RmViewWindow(m_parentMDI, this, GetScoreboardCoordinator(), *m_defaultViewInformation);
        m_2DViews.push_back(window);
        window->Show(true);
        // MSA 09.12.21 Workaround to ensure view window is visible if MDI is located on a secondary screen
        wxSize sz = m_parentMDI->GetSize();
        window->SetPosition(wxPoint(sz.GetWidth() / 6, sz.GetHeight() / 6));
        RegisterSimulationActivityObserver(window);
    }

    void GuiSimulationEngine::NewView3DWindow()
    {
        if (__nullptr == m_defaultView3DInformation)
        {
            m_defaultView3DInformation = new View3DInformation;
            AssistViewInformationDefaults(m_defaultView3DInformation);
        }

        RmView3D* window = new RmView3D(m_parentMDI, this, GetScoreboardCoordinator(), *m_defaultView3DInformation);
        m_3DViews.push_back(window);
        window->Show(true);
        // MSA 09.12.21 Workaround to ensure view window is visible if MDI is located on a secondary screen
        wxSize sz = m_parentMDI->GetSize();
        window->SetPosition(wxPoint(sz.GetWidth() / 4, sz.GetHeight() / 4));
        RegisterSimulationActivityObserver(window);
    }

    void GuiSimulationEngine::ShowProgressWindow()
    {
        if (__nullptr == m_progressWindow)
        {
            m_progressWindow = new RmProgressWindow(m_parentMDI); // , rmID_PROGRESS_DIALOG, _("Progress"));
            m_progressWindow->Initialise(GetPostOffice(), this);
            m_progressWindow->Show(true);
            RegisterSimulationActivityObserver(m_progressWindow);
        }

        //window->Activate();
        m_progressWindow->Raise(); // Show(true);
    }

    void GuiSimulationEngine::createTable(TableDAI& data)
    {
        wxString sanitisedCharacteristicName = data.getCharacteristicName();
        sanitisedCharacteristicName.Replace(" VolumeObject [none]", "");

        long stratum_value = data.getScoreboardStratum().value();
        TableInformationCollection::iterator fnd = m_defaultTableInformation.find(stratum_value);

        ProcessCoordinator& pcoord = GetProcessCoordinator();
        long characteristic_number = 0;
        Process* process = pcoord.FindProcessByCharacteristicName(std::string(sanitisedCharacteristicName), &characteristic_number);

        // MSA 10.12.22 Allowing selection of Process by variant Characteristic name 
        if (__nullptr == process)
        {
            process = pcoord.FindProcessByVariantCharacteristicName(std::string(sanitisedCharacteristicName), &characteristic_number);
        }

        wxString msg = "Process required for TableInformation is NULL. Characteristic " + data.getCharacteristicName() + " appears to be unavailable.";
        RmAssert(__nullptr != process, msg.c_str());

        if (fnd == m_defaultTableInformation.end())
        {
            TableInformation* table_info = new TableInformation(process,
                characteristic_number,
                data.getViewDirection(),
                data.getLayer(),
                data.getScoreboardStratum(),
                0);
            m_defaultTableInformation[stratum_value] = table_info;
        }
        else
        {
            TableInformation* table_info = (*fnd).second;
            table_info->SetProcess(process);
            table_info->SetCharacteristicNumber(characteristic_number);
            table_info->SetViewDirection(data.getViewDirection());
            table_info->SetLayer(data.getLayer());
            table_info->SetStratum(data.getScoreboardStratum());
            table_info->SetMenuID(0);
        }

        NewTableWindow(data.getScoreboardStratum());
    }

    void GuiSimulationEngine::createView(ViewDAI& data)
    {
        if (__nullptr == m_defaultViewInformation)
        {
            m_defaultViewInformation = new ViewInformation;
        }
        // doesn't copy the CharacteristicColourInfo or Processes
        *m_defaultViewInformation = data;

        // make a Process-ified array from the names
        ProcessArray process_array;
        std::vector<std::string> process_names = data.GetProcesses();
        for (std::vector<std::string>::iterator piter = process_names.begin();
            piter != process_names.end(); ++piter)
        {
            Process* p = GetProcessCoordinator().FindProcessByProcessName(*piter);
            if (__nullptr != p)
            {
                process_array.push_back(p);
            }
        }
        m_defaultViewInformation->SetDisplayProcesses(process_array);

        // make a CharacteristicColourInfo_t-ified version of the data
        // MSA 09.10.30 Now supports >=3 colourElements
        ViewDAI::CharacteristicColourDataArray colour_data = data.GetCharacteristicColourInfo();
        CharacteristicColours_t characteristicColours_t;
        for (size_t i = 0; i < colourElementsMax; ++i)
        {
            CharacteristicColourInfo cci;
            cci.characteristicIndex = 0;
            cci.characteristicID = 0;

            cci.colourRangeMin = colour_data[i].colourRangeMin;
            cci.colourRangeMax = colour_data[i].colourRangeMax;
            cci.characteristicRangeMin = colour_data[i].characteristicRangeMin;
            cci.characteristicRangeMax = colour_data[i].characteristicRangeMax;

            long characteristic_number = -1;
            Process* p = GetProcessCoordinator().FindProcessByCharacteristicName(colour_data[i].characteristicName, &characteristic_number);
            if (__nullptr == p)
            {
                p = GetProcessCoordinator().FindProcessByVariantCharacteristicName(colour_data[i].characteristicName, &characteristic_number);
            }
            if (__nullptr == p)
            {
                // If process not found, last chance is to try to find the VO-ignorant equivalent.
                wxString name = colour_data[i].characteristicName;
                int idx = name.Find(" VolumeObject ");
                if (idx > 0)
                {
                    name = name.SubString(0, idx - 1);
                    p = GetProcessCoordinator().FindProcessByVariantCharacteristicName(std::string(name), &characteristic_number);
                }
                if (__nullptr == p)
                {
                    wxString excStr = "Could not find a process containing characteristic " + colour_data[i].characteristicName;
                    throw new RmException(excStr);
                }
            }
            RmAssert(p->GetProcessName() == colour_data[i].processName, "Process Names do not match");
            cci.characteristicIndex = p->GetCharacteristicIndex(characteristic_number);
            cci.characteristicID = p->GetCharacteristicID(characteristic_number);
            characteristicColours_t[i] = cci;
        }

        m_defaultViewInformation->SetCharacteristicColourInfo(characteristicColours_t);


        //
        NewViewWindow();
    }


    void GuiSimulationEngine::createView3D(ViewDAI& data)
    {
        if (__nullptr == m_defaultView3DInformation)
        {
            m_defaultView3DInformation = new View3DInformation;
        }
        // doesn't copy the CharacteristicColourInfo or Processes
        *m_defaultView3DInformation = data;

        // make a Process-ified array from the names
        ProcessArray process_array;
        std::vector<std::string> process_names = data.GetProcesses();
        for (std::vector<std::string>::iterator piter = process_names.begin();
            piter != process_names.end(); ++piter)
        {
            Process* p = GetProcessCoordinator().FindProcessByProcessName(*piter);
            if (__nullptr != p && p->DoesDrawing()) // Ensure each Process actually draws before adding it
            {
                process_array.push_back(p);
            }
        }
        m_defaultView3DInformation->SetDisplayProcesses(process_array);

        NewView3DWindow();
    }


    void GuiSimulationEngine::updateTableInformation(const TableInformation& tableInfo)
    {
        long stratum_value = tableInfo.GetStratum().value();

        TableInformationCollection::iterator fnd = m_defaultTableInformation.find(stratum_value);

        RmAssert(fnd != m_defaultTableInformation.end(), "TableInformation does not exist for updated information");

        *((*fnd).second) = tableInfo;
    }

    void GuiSimulationEngine::removeTableWindow(RmTableWindow* tableWin)
    {
        RemoveSimulationActivityObserver(tableWin);
    }


    void GuiSimulationEngine::populateCharacteristicStringArray(wxArrayString& characteristics)
    {
        GetProcessCoordinator().populateCharacteristicStringArray(characteristics);
    }

    void GuiSimulationEngine::populateProcessStringArray(wxArrayString& processes, std::vector<ProcessIdentifier>& processIds, PopulateProcessStringFlags flags)
    {
        GetProcessCoordinator().populateProcessStringArray(processes, processIds, flags);
    }

    Process* GuiSimulationEngine::findProcessCharacteristicNumber(CharacteristicIndex characteristic_index, long& characteristic_number)
    {
        Process* p = GetProcessCoordinator().FindProcessByCharacteristicIndex(characteristic_index);
        if (__nullptr != p)
        {
            characteristic_number = p->FindCharacteristicNumberOfCharacteristicIndex(characteristic_index);
        }

        return p;
    }


    Process* GuiSimulationEngine::findProcessCharacteristicNumber(const wxString& characteristic_name, long& characteristic_number)
    {
        Process* p = GetProcessCoordinator().FindProcessByCharacteristicName(std::string(characteristic_name), &characteristic_number);
        return p;
    }

    void GuiSimulationEngine::findProcessesByProcessId(const ProcessIdentifierSet& processIds, ProcessArray& processes)
    {
        GetProcessCoordinator().findProcessesByProcessId(processIds, processes);
    }

    void GuiSimulationEngine::updateViewInformation(const ViewInformation& /* viewInfo */)
    {
#pragma message(HERE " implement GuiSimulationEngine::updateViewInformation")
    }

    void GuiSimulationEngine::removeViewWindow(RmViewWindow* viewWin)
    {
        // MSA 09.10.07    Would have preferred to use the STL remove algorithm here,
        // but it does not seem to support inclusive sets (it operates on [first, last) 
        // and attempting to use "last=vector::end()+1 iterator" throws an assert failure )
        for (size_t i = 0; i < m_2DViews.size(); ++i)
        {
            if (m_2DViews[i] == viewWin)
            {
                m_2DViews.erase(m_2DViews.begin() + i);
                i--; // Because vector size has been decremented. Note this will underflow the size_t if i==0, but it will be immediately reincremented back (down) to 0, so no problem.
            }
        }
        RemoveSimulationActivityObserver(viewWin);
    }

    void GuiSimulationEngine::updateView3DInformation(const ViewInformation& /* viewInfo */)
    {
    }

    void GuiSimulationEngine::removeView3DWindow(RmView3D* viewWin)
    {
        // MSA 09.10.07    Would have preferred to use the STL remove algorithm here,
        // but it does not seem to support inclusive sets (it operates on [first, last) 
        // and attempting to use "last=vector::end()+1 iterator" throws an assert failure )
        for (size_t i = 0; i < m_3DViews.size(); ++i)
        {
            if (m_3DViews[i] == viewWin)
            {
                m_3DViews.erase(m_3DViews.begin() + i);
                i--; // Because vector size has been decremented
            }
        }
        RemoveSimulationActivityObserver(viewWin);
    }

    void GuiSimulationEngine::removeProgressWindow(RmProgressWindow* progressWin)
    {
        RemoveSimulationActivityObserver(progressWin);
        m_progressWindow = __nullptr;
    }

    void GuiSimulationEngine::AssistTableInformationDefaults(TableInformation* tableInfo)
    {
        tableInfo->SetDefaulted();
    }

    void GuiSimulationEngine::AssistViewInformationDefaults(ViewInformation* /* viewInfo */)
    {
    }

    void GuiSimulationEngine::Run()
    {
        SimulationEngine::Run();
    }

    void GuiSimulationEngine::NotifyPreTimestamp()
    {
        wxSafeYield(m_parentMDI);
        SimulationEngine::NotifyPreTimestamp();

        if (GetNumSimulationActivityObservers() > 0)
        {
        }
    }

    void GuiSimulationEngine::NotifyPostTimestamp()
    {
        wxSafeYield(m_parentMDI);
        SimulationEngine::NotifyPostTimestamp();

        // MSA 10.10.19 Currently unused
        /*
        if (GetNumSimulationActivityObservers() > 0)
        {
        }
        */
    }

    void GuiSimulationEngine::CloseSimulation()
    {
        if (m_parentMDI)
        {
            m_parentMDI->DoCloseSimulation();
        }
    }


#if defined ROOTMAP_TODO
    void Engine::AssistViewWindowWithDefaults(ViewInformation * default_view_info)
    {
        /* we need to establish a process and characteristic_number from the currently front-most table window
        the easy thing to do is assume that we only have table and view windows open. However it is entirely possible
        we may have other windows foremost, ie. a clock or status window. Thus, we need to start looking from the
        top of the window pile Õtill we get to a table window . Goodness only knows what i might do if there are NO
        table windows open , only the status or clock windows */
        //    long num_windows = gDesktop->GetNumWindows();
        TDirector * table_director = __nullptr;
        CWindow * table_window = gDesktop->FirstSuccessWindow(IsTDirector, 0);
        if (table_window != 0)
        {
            // if we succeeded in finding a window, we shouldn't need
            // to do this safely, but gosh its good to know that we do
            table_director = TCL_DYNAMIC_CAST(TDirector, table_window->GetSupervisor());
        }

        if (table_director != 0)
        { // set the characteristicID and characteristic_index
            double c_range_min, c_range_max;
            long int c_id, c_index;
            CharacteristicColourInfo cci[kYellow + 1];

            c_range_min = table_director->tdProcess->GetCharacteristicMinimum(table_director->tdCharacteristicNumber);
            c_range_max = table_director->tdProcess->GetCharacteristicMaximum(table_director->tdCharacteristicNumber);
            c_id = table_director->tdProcess->GetCharacteristicID(table_director->tdCharacteristicNumber);
            c_index = table_director->tdProcess->GetCharacteristicIndex(table_director->tdCharacteristicNumber);

            for (long int i8 = kCyanXtic; i8 <= kYellowXtic; ++i8)
            {
                cci[i8].characteristicRangeMin = c_range_min;
                cci[i8].characteristicRangeMax = c_range_max;
                cci[i8].characteristicIndex = c_index;
                cci[i8].characteristicID = c_id;
            }
            default_view_info->SetCharacteristicColourInfo(cci);

            eViewWindowNeedsAssistanceWithDefaults = false;
        } // if (table_director != 0)
        else
        {
            const int max_cnames = 10;
            Str255 view1_cnames[max_cnames] = {
                "\pWater Content",
                "\pNitrate Amount",
                "\pTotal Root Length Density",
                "\pTotal Root Length",
                "\pRoot Conductance 0",
                "\ppH",
                "\pTemperature",
                "\pTotal Tip Count",
                "\pTotal Branch Count",
                "\p" };
            // an accompanying array of characteristic range minimums and maximums
            // note that we could use
            // process->GetCharacteristicMaximum(characteristic_number)
            // to get the mins and max's, however some of the maximums are theoretical limits,
            // ie. for Root Length and Density. So just provide a manual assignment method
            double view1_xtic_range_min_array[max_cnames] = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
            double view1_xtic_range_max_array[max_cnames] = {
                0.2, 500.0, 1.0, 6.0, 1.0, 14.0, 20.0, 15, 100, 0.0 };

            long int characteristic_number[3] = { -1,-1,-1 };
            long int characteristic_index[3] = { -1,-1,-1 };
            long int characteristic_id[3] = { -1,-1,-1 };
            Process* process[3] = { 0, 0, 0 };
            long int i; // view characteristic index
            long int j; // characteristic name array index

    // scoot through the array of names, seeing if we can use one
            j = 0;
            for (i = 0; i < 3; ++i)
            {
                process[i] = 0;
                while ((j < max_cnames) && (process[i] == 0) && (characteristic_number[i] < 0))
                {
                    process[i] = FindProcessByCharacteristicName(view1_cnames[j], &(characteristic_number[i]));
                    if ((process[i] != 0) && (characteristic_number[i] > 0))
                    {
                        characteristic_index[i] = (process[i])->GetCharacteristicIndex(characteristic_number[i]);
                        characteristic_id[i] = (process[i])->GetCharacteristicID(characteristic_number[i]);
                    }
                    ++j;
                }
            }

            // if we have got process modules to show
            if ((process[0] != 0) && (process[1] != 0) && (process[2] != 0))
            {
                CharacteristicColourInfo cci[kYellow + 1];

                cci[kCyan].colourRangeMin = 0;
                cci[kCyan].colourRangeMax = USHRT_MAX;
                cci[kCyan].characteristicRangeMin = view1_xtic_range_min_array[0];
                cci[kCyan].characteristicRangeMax = view1_xtic_range_max_array[0];
                cci[kCyan].characteristicIndex = characteristic_index[0];
                cci[kCyan].characteristicID = characteristic_id[0];

                cci[kMagenta].colourRangeMin = 0;
                cci[kMagenta].colourRangeMax = USHRT_MAX;
                cci[kMagenta].characteristicRangeMin = view1_xtic_range_min_array[1];
                cci[kMagenta].characteristicRangeMax = view1_xtic_range_max_array[1];
                cci[kMagenta].characteristicIndex = characteristic_index[1];
                cci[kMagenta].characteristicID = characteristic_id[1];

                cci[kYellow].colourRangeMin = 0;
                cci[kYellow].colourRangeMax = USHRT_MAX;
                cci[kYellow].characteristicRangeMin = view1_xtic_range_min_array[2];
                cci[kYellow].characteristicRangeMax = view1_xtic_range_max_array[2];
                cci[kYellow].characteristicIndex = characteristic_index[2];
                cci[kYellow].characteristicID = characteristic_id[2];

                eDefaultViewInformation->SetCharacteristicColourInfo(cci);

                eViewWindowNeedsAssistanceWithDefaults = false;
            }
        }
    }
#endif // #if defined ROOTMAP_TODO

#if defined ROOTMAP_TODO
    static Boolean IsVDirector(CWindow* theWindow, long /*theOtherParameter*/)
    {
        CBureaucrat * tcl_director = __nullptr;
        VDirector * view_director = __nullptr;

        tcl_director = theWindow->GetSupervisor();
        view_director = TCL_DYNAMIC_CAST(VDirector, tcl_director);

        return (view_director != 0);
    }
    /* We need to establish a tprocess and tcharacteristicnumber from the currently front-most
    view window. The easy thing to do is assume that we only have Table and View windows open.
    However it is entirely possible we may have other windows foremost, ie. a clock or status
    window. Thus, we need to start looking from the top of the window pile until we get to a view
    window . Goodness only knows what i might do if there are NO characteristic-displaying
    windows open, for example only a status or graph or filter window */
    void Engine::AssistTableWindowWithDefaults(DefaultTableInfoH default_table_info, Ground Volume Type volume)
    {
        VDirector* view_director = __nullptr;
        CWindow * view_window = gDesktop->FirstSuccessWindow(IsVDirector, 0);
        if (view_window != 0)
        {
            // if we succeeded in finding a window, we shouldn't need
            // to do this safely, but gosh its good to know that we do
            view_director = TCL_DYNAMIC_CAST(VDirector, view_window->GetSupervisor());
        }

        if (view_director != 0)
        { // to be fancy , i choose the characteristic that is using the largest colour range to be displayed
            unsigned short int this_colour_range = 0;
            unsigned short int largest_colour_range = 0;
            long int corresponding_index = kCyanXtic;
            const CharacteristicColourInfo * cci = view_director->vdPanorama->GetCharacteristicColourInfo();

            for (long int i9 = kCyanXtic; i9 <= kYellowXtic; ++i9)
            {
                this_colour_range = cci[i9].colourRangeMax - cci[i9].colourRangeMin;
                if (this_colour_range > largest_colour_range)
                {
                    largest_colour_range = this_colour_range;
                    corresponding_index = i9;
                } // if (this_colour_range > largest_colour_range)
            } // for (long int i9...

            (*default_table_info)->tprocess = FindProcessByCharacteristicID(cci[corresponding_index].characteristicID);
            (*default_table_info)->tcharacteristicnumber = ((*default_table_info)->tprocess)->FindCharacteristicNumber(cci[corresponding_index].characteristicID);

            eTableWindowNeedsAssistanceWithDefaults[volume] = false;
        } // if (view_director != 0)
#ifdef _CS_DEBUG_
        else if ((itsDirectors != 0) && (itsDirectors->GetNumItems() >= 1))
            // no view window's director was found !! Allow problem to be detected.
            Debugger();
#endif
    }
#endif // #if defined ROOTMAP_TODO
} /* namespace rootmap */
