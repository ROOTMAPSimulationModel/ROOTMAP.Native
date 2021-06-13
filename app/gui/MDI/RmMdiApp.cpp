/////////////////////////////////////////////////////////////////////////////
// Name:        RmMdiApp.cpp
// Purpose:     
// Author:      
// Modified by: 
// Created:     24/06/2006 16:16:14
// RCS-ID:      
// Copyright:   ©2006 University of Tasmania
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "RmMdiApp.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
#include "wx/fs_zip.h"
#include "wx/log.h"
#include "wx/ffile.h"
#include "wx/xrc/xh_grid.h"

#include "RmMdiApp.h"

#include "simulation/file/output/OutputFileAlarm.h"
#include "simulation/process/modules/DrawingDemonstration.h"
#include "simulation/process/modules/OrganicMatter.h"
#include "simulation/process/modules/Phosphorus.h"
#include "simulation/process/modules/Nitrate.h"
#include "simulation/process/modules/Water.h"
#include "simulation/data_access/dummyconfig/DummyProcessNothing.h"
#include "simulation/data_access/dummyconfig/DummyProcessWith2Characteristics.h"

#include "core/common/Exceptions.h"
#include "core/log/Logger.h"
#include "core/macos_compatibility/MacResourceManager.h"
#include "app/common/RmGetOpts.h"

#include <iostream>

#include <string>
#include <fstream>

#include <sstream>
////@end includes


////@begin XPM images
////@end XPM images

/*!
* Application instance implementation
*/

////@begin implement app
IMPLEMENT_APP(rmMdiApp)
////@end implement app

/*!
* rmMdiApp type definition
*/

IMPLEMENT_CLASS(rmMdiApp, wxApp)

/*!
* rmMdiApp event table definition
*/

BEGIN_EVENT_TABLE(rmMdiApp, wxApp)

////@begin rmMdiApp event table entries
EVT_IDLE(rmMdiApp::OnIdle)

////@end rmMdiApp event table entries

END_EVENT_TABLE()

/*!
* Constructor for rmMdiApp
*/

rmMdiApp::rmMdiApp()
{
    ////@begin rmMdiApp member initialisation
    ////@end rmMdiApp member initialisation
}

rmMdiApp::~rmMdiApp()
{
}

wxString GetExeDir()
{
    WCHAR ownPth[MAX_PATH];
    HMODULE hModule = GetModuleHandle(__nullptr);
    if (hModule != __nullptr)
    {
        GetModuleFileName(hModule, ownPth, (sizeof(ownPth)));
        std::string::size_type pos = wxString(ownPth).find_last_of("\\/");
        return wxString(ownPth).substr(0, pos);
    }
    // Could not find path to currently executing assembly!
    return ".";
}

/*!
* Initialisation for rmMdiApp
*/

bool rmMdiApp::OnInit()
{
    wxXmlResource::Get()->InitAllHandlers();
    wxFileSystem::AddHandler(new wxZipFSHandler);

    wxString exeDir = GetExeDir();

    wxXmlResource::Get()->Load(exeDir + "\\RootMapMDI.xrc");

#if wxUSE_XPM
    wxImage::AddHandler(new wxXPMHandler);
#endif
#if wxUSE_LIBPNG
    wxImage::AddHandler(new wxPNGHandler);
#endif
#if wxUSE_LIBJPEG
    wxImage::AddHandler(new wxJPEGHandler);
#endif
#if wxUSE_GIF
    wxImage::AddHandler(new wxGIFHandler);
#endif
    wxXmlResource::Get()->AddHandler(new wxGridXmlHandler);

    RmParentFrame* mainWindow = new RmParentFrame(__nullptr, rmID_PARENT_WINDOWFRAME);
    mainWindow->Show(true);
    ////@end rmMdiApp initialisation

    //#define ROOTMAP_LOG_TO_FILE
#if defined ROOTMAP_LOG_TO_FILE
    wxDateTime now(wxDateTime::Now());
    wxString filename = now.Format("RootMap.%Y%m%d-%H%M%S.log").c_str();
    wxFFile * logfile = new wxFFile(filename.c_str(), _T("w+"));
    wxLog * logger = new wxLogStderr(logfile->fp());
    wxLog::SetActiveTarget(logger);
#endif // #if defined ROOTMAP_LOG_TO_FILE

    rootmap::RmGetOpts::getInstance().parseCmdLine(argc, argv);
    wxString specifiedLogConfigFile = rootmap::RmGetOpts::getInstance().getOpt(RMGETOPTS_LOGFILENAME);
    wxString exeDirLogConfigFile = exeDir << "\\rootmap.log.cfg";
    wxString workingDirLogConfigFile = rootmap::RmGetOpts::getInstance().getOpt(RMGETOPTS_WORKINGDIR);
    workingDirLogConfigFile << "rootmap.log.cfg";

    if (!specifiedLogConfigFile.empty() && !specifiedLogConfigFile.Contains(":"))
    {
        // Assume to be relative if path does not contain a colon.
        // If it's relative, convert it to absolute here.
        specifiedLogConfigFile = rootmap::RmGetOpts::getInstance().getOpt(RMGETOPTS_WORKINGDIR) + specifiedLogConfigFile;
    }

    // Attempt to load config from
    // 1. Any explicitly specified file
    // 2. The current exe directory
    // 3. The base working directory
    bool success = !specifiedLogConfigFile.empty() && RootMapLoggerConfigure(specifiedLogConfigFile.c_str());
    if (!success && exeDir != ".")
    {
        success = RootMapLoggerConfigure(exeDirLogConfigFile.c_str());
    }
    if (!success)
    {
        success = RootMapLoggerConfigure(workingDirLogConfigFile.c_str());
    }

    rootmap::MacResourceManager::Initialise();

    return true;
}

/*!
* Cleanup for rmMdiApp
*/
int rmMdiApp::OnExit()
{
    return wxApp::OnExit();
}

void rmMdiApp::OnIdle(wxIdleEvent& event)
{
    event.Skip();
}

#define FORCE_DYNAMIC_CLASS_USE(cname) rootmap::cname * ForcedUseOf##cname = new rootmap::cname; delete ForcedUseOf##cname

void rmMdiApp::ForceDynamicClassUsage()
{
    // We don't actually want to run this code, however we don't want the compiler
    // optimising it away either. We just want it to have the code thinking it
    // might be executed, so that the constructors of the classes are called
    // and not stripped out of the executable.
    if (time(__nullptr) == 0)
    {
        FORCE_DYNAMIC_CLASS_USE(Water);
        FORCE_DYNAMIC_CLASS_USE(Nitrate);
        FORCE_DYNAMIC_CLASS_USE(Phosphorus);
        FORCE_DYNAMIC_CLASS_USE(OrganicMatter);
        FORCE_DYNAMIC_CLASS_USE(OutputFileAlarm);
        FORCE_DYNAMIC_CLASS_USE(DrawingDemonstration);
        FORCE_DYNAMIC_CLASS_USE(DummyProcessNothing);
        FORCE_DYNAMIC_CLASS_USE(DummyProcessWith2Characteristics);
    }
}

// Catch explosions
bool rmMdiApp::OnExceptionInMainLoop()
{
    try
    {
        throw;
    }
    catch (rootmap::RmException rmEx)
    {
        std::string what = rmEx.what();
        std::string msg = "Caught a ROOTMAP exception: " + what;
        wxMessageBox(msg);
    }
    catch (std::exception ex)
    {
        std::string what = ex.what();
        std::string msg = "Caught an exception: " + what;
        wxMessageBox(msg);
    }
    catch (std::string strEx)
    {
        std::string msg = "Caught a string exception: " + strEx;
        wxMessageBox(msg);
    }
    catch (int i)
    {
        wxLogMessage(("Caught an int \"%d\""), i);
    }
    catch (...)
    {
        wxMessageBox("Caught an unknown exception!");
    }
    return false;
}
