;
; S.T.A.L.K.E.R. - Lost Alpha - SDK
; Script for Inno Setup 5 compiler
; First version: 2014.08.21
; First author to blame: utak3r
; Last modification: 2014.08.22
; Last modifier: utak3r
;
; put the SDK files in the [LA_SDK_distrib] folder.
;
; This script is prepared for and meant to be used with:
; Inno Setup 5.5.4 unicode [ http://www.jrsoftware.org/isdl.php ]
; Inno Script Studio 2.1.0.20  [ https://www.kymoto.org/products/inno-script-studio/downloads ]
;
;
; [LA_SDK_distrib] (complete release of SDK to be installed)
; [installer_images] (images, icons etc.)
; [output] (the compiled installer will be put here)
;

; dirs used:
#define LASDK_files ".\LA_SDK_distrib"
#define LASDK_installer_support_files "."

; versions, names etc.:
#define LASDK_shortcut_name "S.T.A.L.K.E.R. - Lost Alpha SDK"
#define LASDK_app_name "S.T.A.L.K.E.R.: Lost Alpha SDK"
#define LASDK_directory_name "S.T.A.L.K.E.R. - Lost Alpha SDK"
#define LASDK_StartMenu_directory_name "STALKER Lost Alpha SDK"
#define LASDK_copyright "dezowave"
#define LASDK_version "2.6"
#define LASDK_version_text "2.6 beta"
#define LASDK_editor_LE "Level Editor"
#define LASDK_editor_AE "Actor Editor"
#define LASDK_editor_SE "Shader Editor"
#define LASDK_editor_PE "Particle Editor"

[Files]
Source: "{#LASDK_files}\*"; DestDir: "{app}"; Flags: ignoreversion createallsubdirs recursesubdirs skipifsourcedoesntexist

[Icons]
Name: "{userdesktop}\{#LASDK_editor_LE}"; Filename: "{app}\bins\editors\LevelEditor.exe"; WorkingDir: "{app}"; Parameters: "-editor"
Name: "{userdesktop}\{#LASDK_editor_AE}"; Filename: "{app}\bins\editors\ActorEditor.exe"; WorkingDir: "{app}"; Parameters: "-editor"
Name: "{userdesktop}\{#LASDK_editor_SE}"; Filename: "{app}\bins\editors\ShaderEditor.exe"; WorkingDir: "{app}"; Parameters: "-editor"
Name: "{userdesktop}\{#LASDK_editor_PE}"; Filename: "{app}\bins\editors\ParticleEditor.exe"; WorkingDir: "{app}"; Parameters: "-editor"

Name: "{group}\{#LASDK_editor_LE}"; Filename: "{app}\bins\editors\LevelEditor.exe"; WorkingDir: "{app}"; Parameters: "-editor"
Name: "{group}\{#LASDK_editor_AE}"; Filename: "{app}\bins\editors\ActorEditor.exe"; WorkingDir: "{app}"; Parameters: "-editor"
Name: "{group}\{#LASDK_editor_SE}"; Filename: "{app}\bins\editors\ShaderEditor.exe"; WorkingDir: "{app}"; Parameters: "-editor"
Name: "{group}\{#LASDK_editor_PE}"; Filename: "{app}\bins\editors\ParticleEditor.exe"; WorkingDir: "{app}"; Parameters: "-editor"

[Setup]
PrivilegesRequired=admin
AppName={#LASDK_app_name}
AppVersion={#LASDK_version_text}
AppCopyright={#LASDK_copyright}
DefaultDirName={pf}\{#LASDK_directory_name}
DisableProgramGroupPage=auto
DefaultGroupName={#LASDK_StartMenu_directory_name}
AppPublisher={#LASDK_copyright}
VersionInfoVersion={#LASDK_version}
VersionInfoCompany={#LASDK_copyright}
VersionInfoDescription={#LASDK_app_name}
VersionInfoTextVersion={#LASDK_version_text}
VersionInfoCopyright={#LASDK_copyright}
VersionInfoProductName={#LASDK_app_name}
VersionInfoProductVersion={#LASDK_version}
VersionInfoProductTextVersion={#LASDK_version_text}
MinVersion=0,5.01sp3
WizardImageFile={#LASDK_installer_support_files}\installer_images\LAinstallerImage.bmp
SetupIconFile={#LASDK_installer_support_files}\installer_images\stalker.ico
WizardSmallImageFile={#LASDK_installer_support_files}\installer_images\LAinstallerSmallImage.bmp
