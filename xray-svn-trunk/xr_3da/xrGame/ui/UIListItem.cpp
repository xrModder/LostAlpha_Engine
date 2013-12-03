//////////////////////////////////////////////////////////////////////
// UIListItem.cpp: ������� ���� ������ CListWnd
//////////////////////////////////////////////////////////////////////

#include"stdafx.h"

#include "UIlistitem.h"

CUIListItem::CUIListItem(void)
{
	m_eButtonState = BUTTON_NORMAL;
	SetButtonAsSwitch	(true);

	m_bIsSwitch		= false;

	m_pData = NULL;

	m_iIndex = -1;
	m_iValue = 0;
	m_bHighlightText = false;
	m_iGroupID = -1;
	SetAutoDelete(true);
	SetTextAlignment(CGameFont::alLeft);
}

CUIListItem::~CUIListItem(void)
{
}

void CUIListItem::Init(float x, float y, float width, float height)
{
	inherited::SetWndPos(Fvector2().set(x, y));
	inherited::SetWndSize(Fvector2().set(width, height));
	SetButtonState(BUTTON_PUSHED);
//	SetPushOffset(Fvector2().set(0.0f,0.0f));
}

void CUIListItem::InitTexture(LPCSTR tex_name)
{
	CUIButton::InitTexture(tex_name);
	TextItemControl()->m_TextOffset.x = (m_UIStaticItem.GetTextureRect().width());
}


void CUIListItem::Init(const char* str, float x, float y, float width, float height)
{
	Init(x,y,width, height);
	SetTextST(str);	
}
