// dx10HW.cpp: implementation of the DX10 specialisation of CHW.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)
#include <d3dx10.h>
#include "../xrRender/HW.h"
#include "../XR_IOConsole.h"
#include "../../Include/xrAPI/xrAPI.h"

#include "StateManager\dx10SamplerStateCache.h"
#include "StateManager\dx10StateCache.h"

#ifndef _EDITOR
void	fill_vid_mode_list			(CHW* _hw);
void	free_vid_mode_list			();

void	fill_render_mode_list		();
void	free_render_mode_list		();
#else
void	fill_vid_mode_list			(CHW* _hw)	{}
void	free_vid_mode_list			()			{}
void	fill_render_mode_list		()			{}
void	free_render_mode_list		()			{}
#endif

#pragma comment (lib, "d3dx10.lib")

CHW			HW;

//	DX10: Don't neeed this?
/*
#ifdef DEBUG
IDirect3DStateBlock9*	dwDebugSB = 0;
#endif
*/

CHW::CHW() : 
//	hD3D(NULL),
	//pD3D(NULL),
	m_pAdapter(0),
	pDevice(NULL),
	m_move_window(true)
	//pBaseRT(NULL),
	//pBaseZB(NULL)
{
	Device.seqAppActivate.Add(this);
	Device.seqAppDeactivate.Add(this);
}

CHW::~CHW()
{
	Device.seqAppActivate.Remove(this);
	Device.seqAppDeactivate.Remove(this);
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CHW::CreateD3D()
{
	IDXGIFactory * pFactory;
	R_CHK( CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory)) );

	m_pAdapter = 0;
	m_bUsePerfhud = false;

#ifndef	MASTER_GOLD
	// Look for 'NVIDIA NVPerfHUD' adapter
	// If it is present, override default settings
	UINT i = 0;
	while(pFactory->EnumAdapters(i++, &m_pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		m_pAdapter->GetDesc(&desc);
		if(!wcscmp(desc.Description,L"NVIDIA PerfHUD"))
		{
			m_bUsePerfhud = true;
			break;
		}
		else
		{
			_RELEASE(m_pAdapter);
		}
	}
#endif	//	MASTER_GOLD

	if (!m_pAdapter)
	{
		HRESULT R = pFactory->EnumAdapters(0, &m_pAdapter);
		if (FAILED(R))
		{
			// Fatal error! Cannot get the primary graphics adapter AT STARTUP !!!
			Msg					("Failed to initialize graphics hardware.\n"
								 "Please try to restart the game.\n"
								 "EnumAdapters returned 0x%08x", R
								 );
			FlushLog			();
			MessageBox			(NULL,"Failed to initialize graphics hardware.\nPlease try to restart the game.","Error!",MB_OK|MB_ICONERROR);
			TerminateProcess	(GetCurrentProcess(),0);
		}
	}

	_RELEASE(pFactory);
}

void CHW::DestroyD3D()
{
	_SHOW_REF				("refCount:m_pAdapter",m_pAdapter);
	_RELEASE				(m_pAdapter);
}

void CHW::CreateDevice( HWND m_hWnd, bool move_window )
{
	m_move_window			= move_window;
	CreateD3D();

	// TODO: DX10: Create appropriate initialization

	// General - select adapter and device
	BOOL  bWindowed			= !psDeviceFlags.is(rsFullscreen);

	m_DriverType = (Caps.bForceGPU_REF || m_bUsePerfhud) ? 
		D3D10_DRIVER_TYPE_REFERENCE : D3D10_DRIVER_TYPE_HARDWARE;

	// Display the name of video board
	DXGI_ADAPTER_DESC Desc;
	R_CHK( m_pAdapter->GetDesc(&Desc) );
	//	Warning: Desc.Description is wide string
	Msg		("* GPU [vendor:%X]-[device:%X]: %S", Desc.VendorId, Desc.DeviceId, Desc.Description);
	Msg		("* GPU driver type: %s", (D3D10_DRIVER_TYPE_HARDWARE == m_DriverType) ? "hardware" : "software reference");

	Caps.id_vendor	= Desc.VendorId;
	Caps.id_device	= Desc.DeviceId;

	// Select back-buffer & depth-stencil format
	DXGI_FORMAT&	fTarget	= Caps.fTarget;
	DXGI_FORMAT&	fDepth	= Caps.fDepth;

	/*
	if (bWindowed)
	{
		fTarget = mWindowed.Format;
		R_CHK(pD3D->CheckDeviceType	(DevAdapter,DevT,fTarget,fTarget,TRUE));
		fDepth  = selectDepthStencil(fTarget);
	} else {
		switch (psCurrentBPP) {
		case 32:
			fTarget = D3DFMT_X8R8G8B8;
			if (SUCCEEDED(pD3D->CheckDeviceType(DevAdapter,DevT,fTarget,fTarget,FALSE)))
				break;
			fTarget = D3DFMT_A8R8G8B8;
			if (SUCCEEDED(pD3D->CheckDeviceType(DevAdapter,DevT,fTarget,fTarget,FALSE)))
				break;
			fTarget = D3DFMT_R8G8B8;
			if (SUCCEEDED(pD3D->CheckDeviceType(DevAdapter,DevT,fTarget,fTarget,FALSE)))
				break;
			fTarget = D3DFMT_UNKNOWN;
			break;
		case 16:
		default:
			fTarget = D3DFMT_R5G6B5;
			if (SUCCEEDED(pD3D->CheckDeviceType(DevAdapter,DevT,fTarget,fTarget,FALSE)))
				break;
			fTarget = D3DFMT_X1R5G5B5;
			if (SUCCEEDED(pD3D->CheckDeviceType(DevAdapter,DevT,fTarget,fTarget,FALSE)))
				break;
			fTarget = D3DFMT_X4R4G4B4;
			if (SUCCEEDED(pD3D->CheckDeviceType(DevAdapter,DevT,fTarget,fTarget,FALSE)))
				break;
			fTarget = D3DFMT_UNKNOWN;
			break;
		}
		fDepth  = selectDepthStencil(fTarget);
	}
	

	if ((D3DFMT_UNKNOWN==fTarget) || (D3DFMT_UNKNOWN==fTarget))	{
		Msg					("Failed to initialize graphics hardware.\nPlease try to restart the game.");
		FlushLog			();
		MessageBox			(NULL,"Failed to initialize graphics hardware.\nPlease try to restart the game.","Error!",MB_OK|MB_ICONERROR);
		TerminateProcess	(GetCurrentProcess(),0);
	}

	*/
	
	// Set up the presentation parameters
	//DXGI_SWAP_CHAIN_DESC	&sd	= m_ChainDesc;
	ZeroMemory(&m_ChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	selectResolution(m_ChainDesc.BufferDesc.Width, m_ChainDesc.BufferDesc.Height, bWindowed);
	m_ChainDesc.BufferCount = 1;
    m_ChainDesc.BufferDesc.Format = selectFormat(true);
    m_ChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    m_ChainDesc.OutputWindow = m_hWnd;
    m_ChainDesc.SampleDesc.Count = 1;
	m_ChainDesc.SampleDesc.Quality = 0;
    m_ChainDesc.Windowed = bWindowed;
    m_ChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	//m_ChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	m_ChainDesc.BufferDesc.RefreshRate = selectRefresh(m_ChainDesc.BufferDesc.Width, 
														m_ChainDesc.BufferDesc.Height, 
														m_ChainDesc.BufferDesc.Format);


	UINT createDeviceFlags = 0;
#if 1
#ifdef DEBUG
   createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif
#endif

   HRESULT R;

   bool bTryCreate101 = ps_r2_ls_flags.test((u32)R3FLAG_USE_DX10_1) > 0 ? true : false;

   pDevice = NULL;
   pDevice1 = NULL;
   pContext = NULL;

   if (bTryCreate101)
   {
	   // try to create DX10.1 device
	   R = D3D10CreateDeviceAndSwapChain1(	m_pAdapter,
											D3D_DRIVER_TYPE_HARDWARE, //m_DriverType,
											NULL,
											createDeviceFlags,
											D3D10_FEATURE_LEVEL_10_1,
											D3D10_1_SDK_VERSION,
											&m_ChainDesc,
											&m_pSwapChain,
											&pDevice1
											);
	   if (FAILED(R))
	   {
		   pDevice1 = NULL;
		   ps_r2_ls_flags.set((u32)R3FLAG_USE_DX10_1, 0);
	   }
	   else
	   {
		   pContext = pDevice1;
		   pDevice = pDevice1;
		   FeatureLevel = D3D_FEATURE_LEVEL_10_1;
	   }
   }

   if (!pDevice1)
   {
	   // try to create a DX10.0 device
	   R = D3D10CreateDeviceAndSwapChain(	m_pAdapter,
											m_DriverType,
											NULL,
											createDeviceFlags,
											D3D10_SDK_VERSION,
											&m_ChainDesc,
											&m_pSwapChain,
											&pDevice
											);
	   if (FAILED(R))
	   {
			Msg("Failed to initialize graphics hardware.\n"
				"Please try to restart the game.\n"
				"CreateDevice returned 0x%08x", R
				);
			FlushLog();
			MessageBox(NULL, "Failed to initialize graphics hardware.\nPlease try to restart the game.",
				"Error!",
				MB_OK | MB_ICONERROR);
			TerminateProcess(GetCurrentProcess(), 0);
	   }
	   else
	   {
		   pContext = pDevice;
		   FeatureLevel = D3D_FEATURE_LEVEL_10_0;
	   }
   }
   
	R_CHK(R);

	_SHOW_REF	("* CREATE: DeviceREF:", HW.pDevice);

	//	Create backbuffer and depth-stencil views here
	UpdateViews();

	//u32	memory									= pDevice->GetAvailableTextureMem	();
	size_t	memory									= Desc.DedicatedVideoMemory;
	Msg		("*     Texture memory: %d M",		memory/(1024*1024));
	//Msg		("*          DDI-level: %2.1f",		float(D3DXGetDriverLevel(pDevice))/100.f);
#ifndef _EDITOR
	updateWindowProps							(m_hWnd);
	fill_vid_mode_list							(this);
#endif
}

void CHW::DestroyDevice()
{
	//	Destroy state managers
	StateManager.Reset();
	RSManager.ClearStateArray();
	DSSManager.ClearStateArray();
	BSManager.ClearStateArray();
	SSManager.ClearStateArray();

	_SHOW_REF				("refCount:pBaseZB",pBaseZB);
	_RELEASE				(pBaseZB);

	_SHOW_REF				("refCount:pBaseRT",pBaseRT);
	_RELEASE				(pBaseRT);
//#ifdef DEBUG
//	_SHOW_REF				("refCount:dwDebugSB",dwDebugSB);
//	_RELEASE				(dwDebugSB);
//#endif

	//	Must switch to windowed mode to release swap chain
	if (!m_ChainDesc.Windowed) m_pSwapChain->SetFullscreenState( FALSE, NULL);
	_SHOW_REF				("refCount:m_pSwapChain",m_pSwapChain);
	_RELEASE				(m_pSwapChain);

#ifdef USE_DX11
	_RELEASE				(pContext);
#endif

#ifndef USE_DX11
	_RELEASE				(HW.pDevice1);
#endif
	_SHOW_REF				("DeviceREF:",HW.pDevice);
	_RELEASE				(HW.pDevice);


	DestroyD3D				();

#ifndef _EDITOR
	free_vid_mode_list		();
#endif
}

//////////////////////////////////////////////////////////////////////
// Resetting device
//////////////////////////////////////////////////////////////////////
void CHW::Reset (HWND hwnd)
{
	DXGI_SWAP_CHAIN_DESC &cd = m_ChainDesc;
	BOOL bWindowed = psDeviceFlags.is(rsFullscreen);
	cd.Windowed = !bWindowed;

	DXGI_MODE_DESC	&desc = m_ChainDesc.BufferDesc;
	selectResolution(desc.Width, desc.Height, !bWindowed);
	desc.RefreshRate = selectRefresh(desc.Width, desc.Height, desc.Format);


#ifdef DEBUG
	//	_RELEASE			(dwDebugSB);
#endif
	_SHOW_REF				("refCount:pBaseZB",pBaseZB);
	_SHOW_REF				("refCount:pBaseRT",pBaseRT);

	_RELEASE(pBaseZB);
	_RELEASE(pBaseRT);

	m_pSwapChain->SetFullscreenState(bWindowed, NULL);
	CHK_DX(m_pSwapChain->ResizeTarget(&desc));
	CHK_DX(m_pSwapChain->ResizeBuffers(
		cd.BufferCount,
		desc.Width,
		desc.Height,
		desc.Format,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	UpdateViews();

	updateWindowProps	(hwnd);

}

void CHW::safeClearState()
{
	if (S_OK == lastPresentStatus)
		pContext->ClearState();
}

bool CHW::isSupportingColorFormat(DXGI_FORMAT format, D3D10_FORMAT_SUPPORT support)
{
	UINT values = 0;
	if (!pDevice)
	  return false;
	if (FAILED(pDevice->CheckFormatSupport(format, &values)))
		return false;

	if (values && support)
		return true;

	return false;
}

DXGI_FORMAT CHW::selectFormat(bool isForOutput)
{
	DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
	
	if (isForOutput)
	{
	  switch (psCurrentBPP)
	  {
	  case 32:
	    if ((FeatureLevel >= D3D_FEATURE_LEVEL_10_1) && 
	          isSupportingColorFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D3D10_FORMAT_SUPPORT_RENDER_TARGET))
	      format = DXGI_FORMAT_B8G8R8A8_UNORM;
	    else
	      // it should be supported on every device...
	      format = DXGI_FORMAT_R8G8B8A8_UNORM;
	    break;
	  case 16:
	  default:
	    if (isSupportingColorFormat(DXGI_FORMAT_B5G6R5_UNORM, D3D10_FORMAT_SUPPORT_RENDER_TARGET))
	      format = DXGI_FORMAT_B5G6R5_UNORM;
	    else
	      // it should be supported on every device...
	      format = DXGI_FORMAT_R8G8B8A8_UNORM;
	    break;
	  }
	}
	else
	{
	  // for some reason, xray is written in a way it *REQUIRES*
	  // the depth stencil texture to be DXGI_FORMAT_D24_UNORM_S8_UINT
	  // if we select some 16-bit texture format, like DXGI_FORMAT_D16_UNORM
	  // it will render an incorrect image.
    format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	}
	
	return format;
}

DXGI_FORMAT CHW::selectDepthStencil	(DXGI_FORMAT fTarget)
{
	// this is UNUSED!
	return DXGI_FORMAT_D24_UNORM_S8_UINT;  // D3DFMT_D24S8
}

void CHW::selectResolution( u32 &dwWidth, u32 &dwHeight, BOOL bWindowed )
{
	fill_vid_mode_list			(this);

	if(bWindowed)
	{
		dwWidth		= psCurrentVidMode[0];
		dwHeight	= psCurrentVidMode[1];
	}
	else //check
	{
		string64					buff;
		xr_sprintf					(buff,sizeof(buff),"%dx%d",psCurrentVidMode[0],psCurrentVidMode[1]);

		if(_ParseItem(buff,vid_mode_token)==u32(-1)) //not found
		{ //select safe
			xr_sprintf				(buff,sizeof(buff),"vid_mode %s",vid_mode_token[0].name);
			Console->Execute		(buff);
		}

		dwWidth						= psCurrentVidMode[0];
		dwHeight					= psCurrentVidMode[1];
	}
}

DXGI_RATIONAL CHW::selectRefresh(u32 dwWidth, u32 dwHeight, DXGI_FORMAT fmt)
{
	// utak3r: when resizing target, let DXGI calculate the refresh rate for itself.
	// This is very important for performance, that this value is correct.
	DXGI_RATIONAL refresh;
	refresh.Numerator = 0;
	refresh.Denominator = 1;
	return refresh;

	/*
	DXGI_RATIONAL	res;

	res.Numerator = 60;
	res.Denominator = 1;

	float	CurrentFreq = 60.0f;

	if (psDeviceFlags.is(rsRefresh60hz))	
	{
		return res;
	}
	else
	{
		xr_vector<DXGI_MODE_DESC>	modes;

		IDXGIOutput *pOutput;
		m_pAdapter->EnumOutputs(0, &pOutput);
		VERIFY(pOutput);

		UINT num = 0;
		DXGI_FORMAT format = fmt;
		UINT flags         = DXGI_ENUM_MODES_INTERLACED;

		// Get the number of display modes available
		pOutput->GetDisplayModeList( format, flags, &num, 0);

		// Get the list of display modes
		modes.resize(num);
		pOutput->GetDisplayModeList( format, flags, &num, &modes.front());

		_RELEASE(pOutput);

		for (u32 i=0; i<num; ++i)
		{
			DXGI_MODE_DESC &desc = modes[i];

			if( (desc.Width == dwWidth) 
				&& (desc.Height == dwHeight)
				)
			{
				VERIFY(desc.RefreshRate.Denominator);
				float TempFreq = float(desc.RefreshRate.Numerator)/float(desc.RefreshRate.Denominator);
				if ( TempFreq > CurrentFreq )
				{
					CurrentFreq = TempFreq;
					res = desc.RefreshRate;
				}
			}
		}

		return res;
	}
	*/
}

void CHW::OnAppActivate()
{
	if ( m_pSwapChain && !m_ChainDesc.Windowed )
	{
		ShowWindow( m_ChainDesc.OutputWindow, SW_RESTORE );
		m_pSwapChain->SetFullscreenState( TRUE, NULL );
	}
}

void CHW::OnAppDeactivate()
{
	if ( m_pSwapChain && !m_ChainDesc.Windowed )
	{
		m_pSwapChain->SetFullscreenState( FALSE, NULL );
		ShowWindow( m_ChainDesc.OutputWindow, SW_MINIMIZE );
	}
}


BOOL CHW::support( D3DFORMAT fmt, DWORD type, DWORD usage)
{
	//	TODO: DX10: implement stub for this code.
	VERIFY(!"Implement CHW::support");
	/*
	HRESULT hr		= pD3D->CheckDeviceFormat(DevAdapter,DevT,Caps.fTarget,usage,(D3DRESOURCETYPE)type,fmt);
	if (FAILED(hr))	return FALSE;
	else			return TRUE;
	*/
	return TRUE;
}

void CHW::updateWindowProps(HWND m_hWnd)
{
	// utak3r: with DX10 we no longer want to play with window's styles and flags,
	// DXGI will do this for its own for us, if we're in a fullscreen mode.
	// Thus, we will check if the current mode is a windowed mode
	// and only then perform our actions.

	BOOL	bWindowed				= !psDeviceFlags.is	(rsFullscreen);

	if (bWindowed)		{
		if (m_move_window) {
      LONG dwWindowStyle = 0;
      
			if (strstr(Core.Params,"-no_dialog_header"))
				SetWindowLong	( m_hWnd, GWL_STYLE, dwWindowStyle=(WS_BORDER|WS_VISIBLE) );
			else
				SetWindowLong	( m_hWnd, GWL_STYLE, dwWindowStyle=(WS_BORDER|WS_DLGFRAME|WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX ) );

			RECT			m_rcWindowBounds;
			BOOL			bCenter = TRUE;
			if (strstr(Core.Params, "-no_center_screen"))	bCenter = FALSE;

			if (bCenter) {
				RECT				DesktopRect;

				GetClientRect		(GetDesktopWindow(), &DesktopRect);

				SetRect(			&m_rcWindowBounds, 
					(DesktopRect.right-m_ChainDesc.BufferDesc.Width)/2, 
					(DesktopRect.bottom-m_ChainDesc.BufferDesc.Height)/2, 
					(DesktopRect.right+m_ChainDesc.BufferDesc.Width)/2, 
					(DesktopRect.bottom+m_ChainDesc.BufferDesc.Height)/2);
			}else{
				SetRect(			&m_rcWindowBounds,
					0, 
					0, 
					m_ChainDesc.BufferDesc.Width, 
					m_ChainDesc.BufferDesc.Height);
			};

			AdjustWindowRect(&m_rcWindowBounds, dwWindowStyle, FALSE);

			SetWindowPos(m_hWnd, 
				HWND_NOTOPMOST,	
				m_rcWindowBounds.left, 
				m_rcWindowBounds.top,
				( m_rcWindowBounds.right - m_rcWindowBounds.left ),
				( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
				SWP_SHOWWINDOW|SWP_NOCOPYBITS|SWP_DRAWFRAME );
		}
	}

	ShowCursor	(FALSE);
	SetForegroundWindow( m_hWnd );
}


struct _uniq_mode
{
	_uniq_mode(LPCSTR v):_val(v){}
	LPCSTR _val;
	bool operator() (LPCSTR _other) {return !stricmp(_val,_other);}
};

#ifndef _EDITOR

void free_vid_mode_list()
{
	for( int i=0; vid_mode_token[i].name; i++ )
	{
		xr_free					(vid_mode_token[i].name);
	}
	xr_free						(vid_mode_token);
	vid_mode_token				= NULL;
}

void fill_vid_mode_list(CHW* _hw)
{
	if(vid_mode_token != NULL)		return;
	xr_vector<LPCSTR>	_tmp;
	xr_vector<DXGI_MODE_DESC>	modes;

	IDXGIOutput *pOutput;
	//_hw->m_pSwapChain->GetContainingOutput(&pOutput);
	_hw->m_pAdapter->EnumOutputs(0, &pOutput);
	VERIFY(pOutput);

	UINT num = 0;
	DXGI_FORMAT format = _hw->selectFormat(true);
	UINT flags         = 0;

	// Get the number of display modes available
	pOutput->GetDisplayModeList( format, flags, &num, 0);

	// Get the list of display modes
	modes.resize(num);
	pOutput->GetDisplayModeList( format, flags, &num, &modes.front());

	_RELEASE(pOutput);

	for (u32 i=0; i<num; ++i)
	{
		DXGI_MODE_DESC &desc = modes[i];
		string32		str;

		if(desc.Width < 800)
			continue;

		xr_sprintf(str, sizeof(str), "%dx%d", desc.Width, desc.Height);

		if(_tmp.end() != std::find_if(_tmp.begin(), _tmp.end(), _uniq_mode(str)))
			continue;

		_tmp.push_back				(NULL);
		_tmp.back()					= xr_strdup(str);
	}
	


//	_tmp.push_back				(NULL);
//	_tmp.back()					= xr_strdup("1024x768");

	u32 _cnt						= _tmp.size()+1;

	vid_mode_token					= xr_alloc<xr_token>(_cnt);

	vid_mode_token[_cnt-1].id			= -1;
	vid_mode_token[_cnt-1].name		= NULL;

#ifdef DEBUG
	Msg("Available video modes[%d]:",_tmp.size());
#endif // DEBUG
	for( u32 i=0; i<_tmp.size(); ++i )
	{
		vid_mode_token[i].id		= i;
		vid_mode_token[i].name		= _tmp[i];
#ifdef DEBUG
		Msg							("[%s]",_tmp[i]);
#endif // DEBUG
	}
}

void CHW::UpdateViews()
{
	DXGI_SWAP_CHAIN_DESC &sd = m_ChainDesc;
	HRESULT R;

	// Create a render target view
	ID3D10Texture2D *pBuffer;
	R = m_pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), reinterpret_cast<void **>(&pBuffer));
	R_CHK(R);

	R = pDevice->CreateRenderTargetView(pBuffer, NULL, &pBaseRT);
	_RELEASE(pBuffer);
	R_CHK(R);

	//	Create Depth/stencil buffer
	//	HACK: DX10: hard depth buffer format
	//R_CHK	(pDevice->GetDepthStencilSurface	(&pBaseZB));
	ID3D10Texture2D* pDepthStencil = NULL;
	D3D10_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = sd.BufferDesc.Width;
	descDepth.Height = sd.BufferDesc.Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = selectFormat(false);
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D10_USAGE_DEFAULT;
	descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	R = pDevice->CreateTexture2D( &descDepth,       // Texture desc
		NULL,                  // Initial data
		&pDepthStencil ); // [out] Texture
	R_CHK(R);

	// set format etc.
	D3D10_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = descDepth.Format; 
	depthStencilViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//	Create Depth/stencil view
	R = pDevice->CreateDepthStencilView( pDepthStencil, NULL /*&depthStencilViewDesc*/, &pBaseZB );
	R_CHK(R);

	_RELEASE(pDepthStencil);
}
#endif
