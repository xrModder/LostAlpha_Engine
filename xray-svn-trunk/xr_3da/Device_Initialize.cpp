#include "stdafx.h"
#include "resource.h"
#include "dedicated_server_only.h"

extern LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

PROTECT_API void CRenderDevice::Initialize			()
{
	Log("Initializing Engine...");
	TimerGlobal.Start			();
	TimerMM.Start				();

	// Unless a substitute hWnd has been specified, create a window to render into
    if( m_hWnd == NULL)
    {
		LPCSTR	wndclass ="_XRAY_1.5";

        // Register the windows class
		HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(0);
		WNDCLASS wndClass = { CS_HREDRAW | CS_VREDRAW | CS_OWNDC, WndProc, 0, 0, hInstance,
                              LoadIcon( hInstance, MAKEINTRESOURCE(IDI_ICON1) ),
                              LoadCursor( NULL, IDC_ARROW ),
                              (HBRUSH)GetStockObject(BLACK_BRUSH),
                              NULL, wndclass };
        RegisterClass( &wndClass );

        // Set the window's initial style
		m_dwWindowStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP; // WS_BORDER | WS_DLGFRAME;
		/*
        // Set the window's initial width
        RECT rc;
        SetRect			( &rc, 0, 0, 640, 480 );
        AdjustWindowRect( &rc, m_dwWindowStyle, FALSE );
		*/

		u32 screen_width = GetSystemMetrics(SM_CXSCREEN);
		u32 screen_height = GetSystemMetrics(SM_CYSCREEN);

		DEVMODE screen_settings;
		memset(&screen_settings, 0, sizeof(screen_settings));
		screen_settings.dmSize = sizeof(screen_settings);
		screen_settings.dmPelsWidth = (unsigned long) screen_width;
		screen_settings.dmPelsHeight = (unsigned long) screen_height;
		screen_settings.dmBitsPerPel = 32;
		screen_settings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&screen_settings, CDS_FULLSCREEN);


        // Create the render window
		m_hWnd = CreateWindowEx( WS_EX_TOPMOST, 
								wndclass, "S.T.A.L.K.E.R.: Lost Alpha", m_dwWindowStyle,
                               0, 0, // /*rc.left, rc.top, */CW_USEDEFAULT, CW_USEDEFAULT,
							   screen_width, screen_height, //(rc.right-rc.left), (rc.bottom-rc.top), 
							   0L, 0, hInstance, 0L);
    }

    // Save window properties
    m_dwWindowStyle = GetWindowLong( m_hWnd, GWL_STYLE );
    GetWindowRect	( m_hWnd, &m_rcWindowBounds );
    GetClientRect	( m_hWnd, &m_rcWindowClient );

	/*
	if (strstr(lpCmdLine,"-gpu_sw")!=NULL)		HW.Caps.bForceGPU_SW		= TRUE;
	else										HW.Caps.bForceGPU_SW		= FALSE;
	if (strstr(lpCmdLine,"-gpu_nopure")!=NULL)	HW.Caps.bForceGPU_NonPure	= TRUE;
	else										HW.Caps.bForceGPU_NonPure	= FALSE;
	if (strstr(lpCmdLine,"-gpu_ref")!=NULL)		HW.Caps.bForceGPU_REF		= TRUE;
	else										HW.Caps.bForceGPU_REF		= FALSE;
	*/
}

