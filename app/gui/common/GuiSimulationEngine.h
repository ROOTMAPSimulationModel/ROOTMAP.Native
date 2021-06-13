#ifndef GuiSimulationEngine_H
#define GuiSimulationEngine_H
/////////////////////////////////////////////////////////////////////////////
// Name:        GuiSimulationEngine.h
// Purpose:     Declaration of the GuiSimulationEngine class
// Created:     DD/MM/YYYY
// Author:      RvH
// $Date: 2009-07-07 02:40:29 +0800 (Tue, 07 Jul 2009) $
// $Revision: 67 $
// Copyright:   ©2006 University of Tasmania
//
// The GuiSimulationEngine "is a" SimulationEngine that understands GUI
// requirements like ViewInformation, TableInformation and all that.
/////////////////////////////////////////////////////////////////////////////
#include "simulation/common/SimulationEngine.h"
#include "app/gui/common/IProgressCoordinator.h"
#include "app/gui/common/ITableCoordinator.h"
#include "app/gui/common/IViewCoordinator.h"
#include "app/gui/common/IView3DCoordinator.h"
#include "simulation/data_access/common/ITableDataUser.h"
#include "simulation/data_access/common/IViewDataUser.h"
#include "simulation/data_access/common/IView3DDataUser.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <map>
#include <vector>

class RmParentFrame;
class RmProgressWindow;

namespace rootmap
{
    class ScoreboardStratum;
    class TableInformation;
    class ViewInformation;
    class View3DInformation;

    class GuiSimulationEngine
        : public SimulationEngine
        , public ITableDataUser
        , public IViewDataUser
        , public IView3DDataUser
        , public ITableCoordinator
        , public IViewCoordinator
        , public IView3DCoordinator
        , public IProgressCoordinator
    {
    public:
        GuiSimulationEngine(DataAccessManager& dam, RmParentFrame* parent);
        virtual ~GuiSimulationEngine();

        /**
         * Called by RmParentFrame::OnMenuOpen, deferring to here for table,
         * view and other menu updating
         */
        void OnMenuOpen(wxMenuEvent& event);

        virtual void Initialise();

        void NewSimulationWindows();

        void NewTableWindow(const ScoreboardStratum& stratum);

        void NewViewWindow();

        void NewView3DWindow();

        void AssistTableInformationDefaults(TableInformation* tableInfo);

        void AssistViewInformationDefaults(ViewInformation* viewInfo);

        virtual void Run();

        void ShowProgressWindow();

        /**
         *  MSA 21.09.09 Horrible kludge to make 2D and 3D views
         *  available to ViewCaptureOutputStrategy
         */
        std::vector<RmViewWindow*>& Get2DViews();
        std::vector<RmView3D*>& Get3DViews();

    protected:
        /**
         * ITableDataUser implementation
         */
        virtual void createTable(TableDAI& data);

        /**
         * IViewDataUser implementation
         */
        virtual void createView(ViewDAI& data);

        /**
         * IView3DDataUser implementation
         */
        virtual void createView3D(ViewDAI& data);


        /**
         * IWindowCoordinator implementation
         */
        virtual void populateCharacteristicStringArray(wxArrayString& characteristics);
        virtual void populateProcessStringArray(wxArrayString& processes, std::vector<ProcessIdentifier>& processIds, PopulateProcessStringFlags flags);
        virtual Process* findProcessCharacteristicNumber(CharacteristicIndex characteristic_index, long& characteristic_number);
        virtual Process* findProcessCharacteristicNumber(const wxString& characteristic_name, long& characteristic_number);
        virtual void findProcessesByProcessId(const ProcessIdentifierSet& processIds, ProcessArray& processes);

        /**
         * ITableCoordinator implementation
         */
        virtual void updateTableInformation(const TableInformation& tableInfo);

        virtual void removeTableWindow(RmTableWindow* tableWin);

        /**
         * IViewCoordinator implementation
         */
        virtual void updateViewInformation(const ViewInformation& viewInfo);

        virtual void removeViewWindow(RmViewWindow* viewWin);


        /**
         * IView3DCoordinator implementation
         */
        virtual void updateView3DInformation(const ViewInformation& viewInfo);

        virtual void removeView3DWindow(RmView3D* viewWin);

        /**
         * IProgressCoordinator implementation
         */
        virtual void removeProgressWindow(RmProgressWindow* progressWin);

        /**
         *
         */
        virtual void NotifyPreTimestamp();

        /**
         *
         */
        virtual void NotifyPostTimestamp();

        virtual void CloseSimulation();

    private:

        /**
         */
        RmParentFrame* m_parentMDI;

        /**
         */
        typedef std::map<long, TableInformation *> TableInformationCollection;

        /**
         */
        TableInformationCollection m_defaultTableInformation;

        /**
         */
        ViewInformation* m_defaultViewInformation;

        /**
         */
        View3DInformation* m_defaultView3DInformation;

        /**
         *
         */
        RmProgressWindow* m_progressWindow;

        /**
         *  MSA 09.09.21 Horrible kludge to make 2D and 3D views
         *  available to ViewCaptor and thus ViewCaptureOutputStrategy
         */
        std::vector<RmViewWindow*> m_2DViews;
        std::vector<RmView3D*> m_3DViews;

        /// Hidden copy constructor
        GuiSimulationEngine(const GuiSimulationEngine&);
        /// Hidden assignment operator
        GuiSimulationEngine& operator=(const GuiSimulationEngine&);
    }; // class GuiSimulationEngine


    inline std::vector<RmViewWindow*>& GuiSimulationEngine::Get2DViews()
    {
        return m_2DViews;
    }

    inline std::vector<RmView3D*>& GuiSimulationEngine::Get3DViews()
    {
        return m_3DViews;
    }
} /* namespace rootmap */

#endif // #ifndef GuiSimulationEngine_H
