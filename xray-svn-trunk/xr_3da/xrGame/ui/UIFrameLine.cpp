#include "stdafx.h"
#include "UIFrameLine.h"
#include "../hudmanager.h"
#include "UITextureMaster.h"
#include "../ui_base.h"

CUIFrameLine::CUIFrameLine()
	:	uFlags					(0),
		iSize					(0),
		bHorizontalOrientation	(true),
		bStretchTexture			(false)
{
	iPos.set(0, 0);
	m_parent_wnd_size.set(0.0f, 0.0f);
}

void CUIFrameLine::InitFrameLine(Fvector2 pos, float size, bool horizontal, EUIItemAlign align)
{
	SetPos			(pos);
	SetSize			(size);
	SetAlign		(align);
	SetOrientation	(horizontal);
}

void CUIFrameLine::InitTexture(LPCSTR texture, LPCSTR sh_name)
{
	string256		buf;

	CUITextureMaster::InitTexture(strconcat(sizeof(buf),buf,texture,"_back"), &elements[flBack]);
	CUITextureMaster::InitTexture(strconcat(sizeof(buf),buf,texture,"_b"), &elements[flFirst]);
	CUITextureMaster::InitTexture(strconcat(sizeof(buf),buf,texture,"_e"), &elements[flSecond]);
}

void CUIFrameLine::SetTextureColor(u32 cl)
{
	for (int i = 0; i < flMax; ++i)
		elements[i].SetTextureColor(cl);
}

void CUIFrameLine::UpdateSize()
{
	VERIFY(g_bRendering);

	float f_width		= elements[flFirst].GetTextureRect().width();
	float f_height		= elements[flFirst].GetTextureRect().height();
	elements[flFirst].SetPos(iPos.x, iPos.y);

	// Right or bottom texture
	float s_width		= elements[flSecond].GetTextureRect().width();
	float s_height		= elements[flSecond].GetTextureRect().height();
	
	if(bHorizontalOrientation && UI().is_widescreen())
		s_width			/= 1.2f;

	if(bHorizontalOrientation)
		elements[flSecond].SetPos(iPos.x + iSize - s_width, iPos.y);
	else
		elements[flSecond].SetPos(iPos.x, iPos.y + iSize - s_height);

	// Dimentions of element textures must be the same
	if (bHorizontalOrientation)
		R_ASSERT(s_height == f_height);
	else
		R_ASSERT(f_width == s_width);


	// Now stretch back texture to remaining space
	float back_width, back_height;

	if (bHorizontalOrientation)
	{
		back_width	= iSize - f_width - s_width;
		back_height	= f_height;

		// Size of frameline must be equal or greater than sum of size of two side textures
		R_ASSERT(back_width > 0);
	}
	else
	{
		back_width	= f_width;
		back_height	= iSize - f_height - s_height;

		// Size of frameline must be equal or greater than sum of size of two side textures
		R_ASSERT(back_height > 0);
	}

	// Now resize back texture
	float b_width		= elements[flBack].GetTextureRect().width();
	float b_height		= elements[flBack].GetTextureRect().height();

	if (bHorizontalOrientation)
	{
		elements[flBack].SetPos(iPos.x + f_width, iPos.y);
		elements[flBack].SetSize(Fvector2().set(back_width, b_height)); 
	} else {
		elements[flBack].SetPos(iPos.x, iPos.y + f_height);
		elements[flBack].SetSize(Fvector2().set(b_width, back_height)); 
	}

	uFlags |= flValidSize;
}

void CUIFrameLine::Render()
{
	// If size changed - update size
	if ( !(uFlags & flValidSize) )
	{
		UpdateSize();
	}
	// Now render all statics
	for (int i = 0; i < flMax; ++i)
	{
		elements[i].Render();
	}
}