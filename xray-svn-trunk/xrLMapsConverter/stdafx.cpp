#include "stdafx.h"

#pragma comment(lib,"xrCore.lib")
#ifndef USE_NVIDIA_LIBS
#pragma comment(lib,"dxsdk\\Lib\\d3d10.lib")
#pragma comment(lib,"dxsdk\\Lib\\d3dcompiler.lib")
#pragma comment(lib,"dxsdk\\Lib\\dxguid.lib")
#pragma comment(lib,"dxsdk\\Lib\\D3D10_1.lib")
#pragma comment(lib,"dxsdk\\Lib\\d3dx10.lib")
#pragma comment(lib,"dxsdk\\Lib\\dxgi.lib")
#else
#pragma comment(lib,"dxt.lib")
#pragma comment(lib,"nvDXTlibMTDLL.lib")
#endif