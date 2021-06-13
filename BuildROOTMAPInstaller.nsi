!include FileFunc.nsh
!include LogicLib.nsh
!insertmacro GetTime
!include "WordFunc.nsh"

!ifndef SEMVER
    !error "SEMVER not set. Please provide a valid semantic versioning string (without 'v' prefix)."
!endif
!ifndef PRODUCTVERSION
    !error "PRODUCTVERSION not set. Please provide a version string in major.minor.patch format, without prerelease tags."
!endif

Name ROOTMAP
OutFile "InstallROOTMAP-v${SEMVER}.exe"
ShowInstDetails "nevershow"
ShowUninstDetails "nevershow"
RequestExecutionLevel admin ; Admin is required to install VC++ redistributable, add ROOTMAP to Start menu etc.

VIProductVersion                 "${PRODUCTVERSION}.0" ; Note ROOTMAP's versioning scheme is only major.minor.patch. NSIS requires a 4-part version number, so we add .0 to the end.
VIAddVersionKey ProductName      "ROOTMAP"
VIAddVersionKey Comments         ""
VIAddVersionKey CompanyName      "University of Western Australia"
VIAddVersionKey LegalCopyright   "© 2020 University of Western Australia"
VIAddVersionKey FileDescription  "ROOTMAP is a simulation for modelling root systems."
VIAddVersionKey FileVersion      "${SEMVER}"
VIAddVersionKey ProductVersion   "${SEMVER}"
VIAddVersionKey InternalName     "ROOTMAP"

;--------------------------------
; Folder selection page

InstallDir "$APPDATA\ROOTMAP\v${SEMVER}"

;--------------------------------
; Checks to see if a Visual Studio 2017 C++ Redistributable is installed. If not, downloads and installs the latest. Adapted from CheckAndDownloadDotNet45 below.
Function TryInstallVisualCppRedistributable
; Could refactor this and the .NET framework installer function; low priority.
Var /GLOBAL EXITCODE
Var /GLOBAL REDIST_INSTALLED

; Tested on 64-bit Windows 10. TODO (important): test on other platforms.
ReadRegStr $REDIST_INSTALLED HKLM "Software\Wow6432Node\Microsoft\VisualStudio\14.0\VC\Runtimes\x86" "Installed"

StrCmp $REDIST_INSTALLED 1 already_installed installation_required

already_installed:
    Goto done

installation_required:
    # Save redownloading the redistributable, e.g. if we are reinstalling
    IfFileExists "$TEMP\VC_redist.x86.exe" do_local_install do_network_install

do_local_install:
    # Redistributable installer found on the local disk.  Use this copy
    ExecWait '"$TEMP\VC_redist.x86.exe" /passive /norestart' $EXITCODE
    Goto is_reboot_requested

do_network_install:
    Var /GLOBAL vcRedistDidDownload
    NSISdl::download "https://aka.ms/vs/15/release/VC_redist.x86.exe" "$TEMP\VC_redist.x86.exe" vcRedistDidDownload

    StrCmp $vcRedistDidDownload success fail
    success:
        ExecWait '"$TEMP\VC_redist.x86.exe" /passive /norestart' $EXITCODE
        Goto is_reboot_requested

    fail:
        MessageBox MB_OK|MB_ICONEXCLAMATION "Unable to download Visual C++ Redistributable. Please try again. If still unsuccessful, you can try installing the Redistributable from https://aka.ms/vs/15/release/VC_redist.x86.exe separately, rebooting, then installing ROOTMAP."
        Goto done

# $EXITCODE contains the return codes.  1641 and 3010 means a Reboot has been requested
is_reboot_requested:
    ${If} $EXITCODE = 1641
    ${OrIf} $EXITCODE = 3010
        SetRebootFlag true
    ${EndIf}

#exit the function
done:

FunctionEnd


;--------------------------------
; Borrowed from https://stackoverflow.com/questions/15227634/check-for-net4-5-with-nsis.
Function CheckAndDownloadDotNet45
# Let's see if the user has the .NET Framework 4.5 installed on their system or not
# Remember: you need Vista SP2 or 7 SP1.  It is built in to Windows 8, and not needed
# In case you're wondering, running this code on Windows 8 will correctly return is_equal
# or is_greater (maybe Microsoft releases .NET 4.5 SP1 for example)

# Set up our Variables
Var /GLOBAL dotNET45IsThere
Var /GLOBAL dotNET_CMD_LINE
Var /GLOBAL EXIT_CODE

ReadRegDWORD $dotNET45IsThere HKLM "Software\Microsoft\NET Framework Setup\NDP\v4\Full" "Release"
IntCmp $dotNET45IsThere 378389 is_equal is_less is_greater

is_equal:
    Goto done_compare_not_needed
is_greater:
    # Useful if, for example, Microsoft releases .NET 4.5 SP1
    # We want to be able to simply skip install since it's not
    # needed on this system
    Goto done_compare_not_needed
is_less:
    Goto done_compare_needed

done_compare_needed:
    #.NET Framework 4.5 install is *NEEDED*

    # Microsoft Download Center EXE:
    # Web Bootstrapper: http://go.microsoft.com/fwlink/?LinkId=225704
    # Full Download: http://go.microsoft.com/fwlink/?LinkId=225702

    # Setup looks for components\dotNET45Full.exe relative to the install EXE location
    # This allows the installer to be placed on a USB stick (for computers without internet connections)
    # If the .NET Framework 4.5 installer is *NOT* found, Setup will connect to Microsoft's website
    # and download it for you

    # Reboot Required with these Exit Codes:
    # 1641 or 3010

    # Command Line Switches:
    # /showrmui /passive /norestart

    # Silent Command Line Switches:
    # /q /norestart


    # Let's see if the user is doing a Silent install or not
    IfSilent is_quiet is_not_quiet

    is_quiet:
        StrCpy $dotNET_CMD_LINE "/q /norestart"
        Goto LookForLocalFile
    is_not_quiet:
        StrCpy $dotNET_CMD_LINE "/showrmui /passive /norestart"
        Goto LookForLocalFile

    LookForLocalFile:
        # Let's see if the user stored the Full Installer
        IfFileExists "$EXEPATH\components\dotNET45Full.exe" do_local_install do_network_install

        do_local_install:
            # .NET Framework found on the local disk.  Use this copy

            ExecWait '"$EXEPATH\components\dotNET45Full.exe" $dotNET_CMD_LINE' $EXIT_CODE
            Goto is_reboot_requested

        # Now, let's Download the .NET
        do_network_install:

            Var /GLOBAL dotNetDidDownload
            NSISdl::download "http://go.microsoft.com/fwlink/?LinkId=225704" "$TEMP\dotNET45Web.exe" $dotNetDidDownload

            StrCmp $dotNetDidDownload success fail
            success:
                ExecWait '"$TEMP\dotNET45Web.exe" $dotNET_CMD_LINE' $EXIT_CODE
                Goto is_reboot_requested

            fail:
                MessageBox MB_OK|MB_ICONEXCLAMATION "Unable to download .NET Framework.  ROOTMAP Configuration Application will be installed, but will not function without the Framework. ROOTMAP itself does not use the .NET Framework and should be unaffected."
                Goto done_dotNET_function

            # $EXIT_CODE contains the return codes.  1641 and 3010 means a Reboot has been requested
            is_reboot_requested:
                ${If} $EXIT_CODE = 1641
                ${OrIf} $EXIT_CODE = 3010
                    SetRebootFlag true
                ${EndIf}

done_compare_not_needed:
    # Done dotNET Install
    Goto done_dotNET_function

#exit the function
done_dotNET_function:

FunctionEnd

;--------------------------------
; Installer Section
Section "install"
  ; Get a timestamp for making a backup directory
  ${GetTime} "" "L" $0 $1 $2 $3 $4 $5 $6

  ; Install the Visual C++ Redistributable
  Call TryInstallVisualCppRedistributable

  ; Back up the user's configuration directory, if it exists.
  IfFileExists "$INSTDIR\Configurations\*.*" 0 +3
  CreateDirectory "$INSTDIR\reinstallation_backup_$2$1$0$4$5$6\Configurations"
  CopyFiles "$INSTDIR\Configurations" "$INSTDIR\reinstallation_backup_$2$1$0$4$5$6"

  ; Back up the users's raytracing directory, if it exists.
  IfFileExists "$INSTDIR\Raytracing\*.*" 0 +3
  CreateDirectory "$INSTDIR\reinstallation_backup_$2$1$0$4$5$6\Raytracing"
  CopyFiles "$INSTDIR\Raytracing" "$INSTDIR\reinstallation_backup_$2$1$0$4$5$6"

  ; Back up the users's postprocessing directory, if it exists.
  IfFileExists "$INSTDIR\Postprocessing\*.*" 0 +3
  CreateDirectory "$INSTDIR\reinstallation_backup_$2$1$0$4$5$6\Postprocessing"
  CopyFiles "$INSTDIR\Postprocessing" "$INSTDIR\reinstallation_backup_$2$1$0$4$5$6"

  ; Set to only overwrite configuration, documentation etc. if newer.
  ; Don't want to overwrite any changes the user may have made to their config,
  ; unless we've made changes too (which need to be enforced for compatibility reasons).
  SetOverwrite ifnewer

  ; Copy configuration schemata
  SetOutPath "$INSTDIR\Configurations\Schemata"
  File /r "Configurations\Schemata\"

  ; Copy the basic config dir to user APPDATA space
  SetOutPath "$INSTDIR\Configurations\default"
  File /r "Configurations\default\"

  ; Copy log config files to base data dir
  SetOutPath "$INSTDIR"
  File "Configurations\*.cfg"

  ; Copy documentation folder,
  ; raytracing folder and postprocessing folder
  SetOutPath "$INSTDIR\Documentation\"
  File /r "Documentation\"
  SetOutPath "$INSTDIR\Postprocessing\"
  File /r "Postprocessing\"
  SetOutPath "$INSTDIR\Raytracing\"
  File /r "Raytracing\"

  ; Set to overwrite all application files.
  SetOverwrite on

  ; Now, copy all static, unmodifiable application files to app directory
  SetOutPath "$INSTDIR\application"
  File "Release\*.*"

  SetOutPath "$INSTDIR\application\resources"
  File /r "Release\resources\"


  ; Install configuration app
  ; NOT CURRENTLY IN USE
  ; Call CheckAndDownloadDotNet45
  ; SetOutPath "$INSTDIR\tools\ConfigurationApp"
  ; File /r "..\code\ConfigurationApp\MainApp\MainApp.WPF\bin\Release\"

  ; Copy tools
  ; NOT CURRENTLY IN USE
  ; SetOutPath "$INSTDIR\tools\ConfigurationImporter"
  ; File /r /x *.pdb /x *.xml "Release\ConfigurationImporter\"
  ; SetOutPath "$INSTDIR\tools\SchemaValidator"
  ; File /r /x *.pdb /x *.xml "Release\SchemaValidator\"
  ; FileOpen $9 watchconfig.bat w
  ; FileWrite $9 "RootmapSchemaValidator.exe -p ..\..\Configurations --watch --strict $\r$\n"
  ; FileClose $9

  ; Set OutPath back to the root application directory, so all shortcuts start in that directory.
  SetOutPath "$INSTDIR\application"
  ; create desktop shortcut
  ; TODO: add config-dir argument to this shortcut.
  CreateShortCut "$DESKTOP\ROOTMAP.lnk" "$INSTDIR\application\ROOTMAP.exe" ""

  ; create start-menu items
  CreateDirectory "$SMPROGRAMS\ROOTMAP"
  CreateShortCut "$SMPROGRAMS\ROOTMAP\Uninstall ROOTMAP v${SEMVER}.lnk" "$INSTDIR\application\Uninstall.exe" "" "$INSTDIR\application\Uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\ROOTMAP\ROOTMAP v${SEMVER}.lnk" "$INSTDIR\application\ROOTMAP.exe" "" "$INSTDIR\application\ROOTMAP.exe" 0
  ; TODO reenable CreateShortCut "$SMPROGRAMS\ROOTMAP\ROOTMAP Configuration App v${SEMVER}.lnk" "$INSTDIR\tools\ConfigurationApp\MainApp.WPF.exe" "" "$INSTDIR\tools\ConfigurationApp\MainApp.WPF.exe" 0

  ; write version and path information to the registry
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\App Paths\ROOTMAP.exe" "" "$APPDATA\ROOTMAP\v${SEMVER}\application\ROOTMAP.exe"

  ; write uninstall information to the registry
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ROOTMAP-v${SEMVER}" "DisplayName" "ROOTMAP (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ROOTMAP-v${SEMVER}" "UninstallString" "$INSTDIR\application\Uninstall.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ROOTMAP-v${SEMVER}" "Publisher" "University of Western Australia"

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ROOTMAP-v${SEMVER}" "DisplayVersion" "${SEMVER}"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ROOTMAP-v${SEMVER}" "NoModify" "1"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ROOTMAP-v${SEMVER}" "NoRepair" "1"

  ; ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  ; IntFmt $0 "0x%08X" $0
  ; WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ROOTMAP-${SEMVER}" "EstimatedSize" "$0"

  WriteUninstaller "$INSTDIR\application\Uninstall.exe"

  SetAutoClose true
SectionEnd

;--------------------------------
; Uninstaller Section
Section "Uninstall"

  MessageBox MB_OK "ROOTMAP will be uninstalled. Configurations, Raytracing and Postprocessing directories will remain untouched. If you don't need any of these files, please manually delete the directory $APPDATA\ROOTMAP\v${SEMVER}."

  ; Delete application files
  RMDir /r "$APPDATA\ROOTMAP\v${SEMVER}\application\*.*"
  RMDir "$APPDATA\ROOTMAP\v${SEMVER}\application"
  ; Delete tools
  RMDir /r "$APPDATA\ROOTMAP\v${SEMVER}\tools\*.*"
  RMDir "$APPDATA\ROOTMAP\v${SEMVER}\tools"
  ; Delete documentation files
  RMDir /r "$APPDATA\ROOTMAP\v${SEMVER}\Documentation\*.*"
  RMDir "$APPDATA\ROOTMAP\v${SEMVER}\Documentation"

  ; Delete Start Menu Shortcuts
  Delete "$DESKTOP\ROOTMAP v${SEMVER}.lnk"
  Delete "$SMPROGRAMS\ROOTMAP\ROOTMAP v${SEMVER}.lnk"
  Delete "$SMPROGRAMS\ROOTMAP\Uninstall ROOTMAP v${SEMVER}.lnk"
  ; TODO reenable Delete "$SMPROGRAMS\ROOTMAP\ROOTMAP Configuration App v${SEMVER}.lnk"
  RMDir  "$SMPROGRAMS\ROOTMAP"

  ; Delete Registry Entries
  DeleteRegKey HKEY_LOCAL_MACHINE "Software\ROOTMAP-v${SEMVER}"
  DeleteRegKey HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\ROOTMAP-v${SEMVER}"
  DeleteRegKey HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\App Paths\ROOTMAP.exe"

  SetAutoClose true
SectionEnd

;--------------------------------
; MessageBox Section

; Function that calls a messagebox when installation finished correctly
Function .onInstSuccess
  MessageBox MB_OK "ROOTMAP v${SEMVER} installed successfully. Use the desktop icon to start the program."
FunctionEnd

Function un.onUninstSuccess
  MessageBox MB_OK "ROOTMAP v${SEMVER} uninstalled successfully."
FunctionEnd
