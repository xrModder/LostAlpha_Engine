#ifndef xrDebug_macrosH
#define xrDebug_macrosH
#pragma once

//. #define _ANONYMOUS_BUILD

#ifndef __BORLANDC__
	#ifndef _ANONYMOUS_BUILD
		#	define DEBUG_INFO					__FILE__,__LINE__,__FUNCTION__
	#else
		#	define DEBUG_INFO					"",__LINE__,""
	#endif
#else // __BORLANDC__
#	define DEBUG_INFO					__FILE__,__LINE__,__FILE__
#endif // __BORLANDC__

#ifdef _ANONYMOUS_BUILD
	#define _TRE(arg)	""
#else
	#define _TRE(arg)	arg
#endif


#	define CHECK_OR_EXIT(expr,message)	do {if (!(expr)) ::Debug.do_exit(message);} while (0)

#	define R_ASSERT(expr)				do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(_TRE(#expr),DEBUG_INFO,ignore_always);} while(0)
#	define R_ASSERT2(expr,e2)			do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(_TRE(#expr),_TRE(e2),DEBUG_INFO,ignore_always);} while(0)
#	define R_ASSERT3(expr,e2,e3)		do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(_TRE(#expr),_TRE(e2),_TRE(e3),DEBUG_INFO,ignore_always);} while(0)
#	define R_ASSERT4(expr,e2,e3,e4)		do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(_TRE(#expr),_TRE(e2),_TRE(e3),_TRE(e4),DEBUG_INFO,ignore_always);} while(0)
#	define R_CHK(expr)					do {static bool ignore_always = false; HRESULT hr = expr; if (!ignore_always && FAILED(hr)) ::Debug.error(hr,_TRE(#expr),DEBUG_INFO,ignore_always);} while(0)
#	define R_CHK2(expr,e2)				do {static bool ignore_always = false; HRESULT hr = expr; if (!ignore_always && FAILED(hr)) ::Debug.error(hr,_TRE(#expr),_TRE(e2),DEBUG_INFO,ignore_always);} while(0)
#	define FATAL(description)			Debug.fatal(DEBUG_INFO,description)

#	ifdef VERIFY
#		undef VERIFY
#	endif // VERIFY

#	ifdef DEBUG
#		define NODEFAULT				FATAL("nodefault reached")
#		define VERIFY(expr)				do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(#expr,DEBUG_INFO,ignore_always);} while(0)
#		define VERIFY2(expr, e2)		do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(#expr,e2,DEBUG_INFO,ignore_always);} while(0)
#		define VERIFY3(expr, e2, e3)	do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(#expr,e2,e3,DEBUG_INFO,ignore_always);} while(0)
#		define VERIFY4(expr, e2, e3, e4)do {static bool ignore_always = false; if (!ignore_always && !(expr)) ::Debug.fail(#expr,e2,e3,e4,DEBUG_INFO,ignore_always);} while(0)
#		define CHK_DX(expr)				do {static bool ignore_always = false; HRESULT hr = expr; if (!ignore_always && FAILED(hr)) ::Debug.error(hr,#expr,DEBUG_INFO,ignore_always);} while(0)
#   define LOG_IF_FAIL_DX(func) HRESULT inline_hresult = func; if (FAILED(inline_hresult)) { string32 str; switch (inline_hresult) { case D3D10_ERROR_FILE_NOT_FOUND: xr_sprintf(str, sizeof(str), "D3D10_ERROR_FILE_NOT_FOUND"); break; case D3D10_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS: xr_sprintf(str, sizeof(str), "D3D10_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS"); break; case D3DERR_WASSTILLDRAWING: xr_sprintf(str, sizeof(str), "D3DERR_WASSTILLDRAWING"); break; case E_INVALIDARG: xr_sprintf(str, sizeof(str), "E_INVALIDARG"); break; case E_OUTOFMEMORY: xr_sprintf(str, sizeof(str), "E_OUTOFMEMORY"); break; } Msg("%s failed with error: %s", #func, str); }
#	else // DEBUG
#		ifdef __BORLANDC__
#			define NODEFAULT
#		else
#			define NODEFAULT __assume(0)
#		endif
#		define VERIFY(expr)				do {} while (0)
#		define VERIFY2(expr, e2)		do {} while (0)
#		define VERIFY3(expr, e2, e3)	do {} while (0)
#		define VERIFY4(expr, e2, e3, e4)do {} while (0)
#		define CHK_DX(a) a
#   define LOG_IF_FAIL_DX(func) func

#	endif // DEBUG
//---------------------------------------------------------------------------------------------
// FIXMEs / TODOs / NOTE macros
//---------------------------------------------------------------------------------------------
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "

#define NOTE( x )  message( x )
#define FILE_LINE  message( __FILE__LINE__ )

#define TODO( x )  message( __FILE__LINE__"\n"           \
	" ------------------------------------------------\n" \
	"|  TODO :   " #x "\n" \
	" -------------------------------------------------\n" )
#define FIXME( x )  message(  __FILE__LINE__"\n"           \
	" ------------------------------------------------\n" \
	"|  FIXME :  " #x "\n" \
	" -------------------------------------------------\n" )
#define todo( x )  message( __FILE__LINE__" TODO :   " #x "\n" ) 
#define fixme( x )  message( __FILE__LINE__" FIXME:   " #x "\n" ) 

//--------- static assertion
template<bool>	struct CompileTimeError;
template<>		struct CompileTimeError<true>	{};
#define STATIC_CHECK(expr, msg) \
{ \
	CompileTimeError<((expr) != 0)> ERROR_##msg; \
	(void)ERROR_##msg; \
}
#endif // xrDebug_macrosH