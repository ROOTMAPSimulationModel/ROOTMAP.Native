/////////////////////////////////////////////////////////////////////////////
// Name:        RmMdiApp.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     24/06/2006 16:16:14
// RCS-ID:      
// Copyright:   ©2006 University of Tasmania
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _RMMDIAPP_H_
#define _RMMDIAPP_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "RmMdiApp.h"
#endif

#include "wx/xrc/xmlres.h"
#include "wx/image.h"
#include "RmParentFrame.h"

// MSA 09.12.01 Include file for Visual Leak Detector
//#include "vld.h"
////@end includes

class rmMdiApp : public wxApp
{
    DECLARE_CLASS(rmMdiApp)
    DECLARE_EVENT_TABLE()

public:
    /// Constructor
    rmMdiApp();

    virtual ~rmMdiApp();


    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

    ////@begin rmMdiApp event handler declarations

    /// wxEVT_IDLE event handler for ID_UNIDENTIFIED
    void OnIdle(wxIdleEvent& event);

    ////@end rmMdiApp event handler declarations

    void ForceDynamicClassUsage();

    virtual bool OnExceptionInMainLoop();
};

/*!
 * Application instance declaration
 */

DECLARE_APP(rmMdiApp)

#endif
// _RMMDIAPP_H_
