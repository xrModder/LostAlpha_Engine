#include "stdafx.h"
#include "level_editor.h"


int stack_overflow_exception_filter	(int exception_code)
{
   if (exception_code == EXCEPTION_STACK_OVERFLOW)
   {
       // Do not call _resetstkoflw here, because
       // at this point, the stack is not yet unwound.
       // Instead, signal that the handler (the __except block)
       // is to be executed.
       return EXCEPTION_EXECUTE_HANDLER;
   }
   else
       return EXCEPTION_CONTINUE_SEARCH;
}

INT APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     INT       nCmdShow)
{
	__try 
	{
		
	}
	__except(stack_overflow_exception_filter(GetExceptionCode()))
	{
		_resetstkoflw		();
		FATAL				("stack overflow");
	}

	return					(0);
}