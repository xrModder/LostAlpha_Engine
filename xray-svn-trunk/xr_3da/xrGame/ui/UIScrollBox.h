#pragma once
#include "uibutton.h"

class CUIScrollBox :public CUIStatic
{
	typedef	CUIStatic			inherited;
public:
								CUIScrollBox			();

			void 				SetHorizontal			();
			void 				SetVertical				();

			bool IsHorizontal		()			{return m_bIsHorizontal;}
			void SetHorizontal		(bool horiz)		{m_bIsHorizontal = horiz;}

	virtual bool				OnMouseAction					(float x, float y, EUIMessages mouse_action);
	virtual void				Draw					();
						
protected:
	bool						m_bIsHorizontal;
};
