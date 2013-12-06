#include "pch_script.h"
#include "UIStatic.h"
#include "UIAnimatedStatic.h"

using namespace luabind;

#pragma optimize("s",on)

void CUIStatic::script_register(lua_State *L)
{
	module(L)
	[
		class_<CUILines>("CUILines")
		.def("SetFont",				&CUILines::SetFont)
		.def("SetText",				&CUILines::SetText)
		.def("SetTextST",			&CUILines::SetTextST)
		.def("GetText",				&CUILines::GetText)
		.def("SetElipsis",			&CUILines::SetEllipsis)
		.def("SetTextColor",		&CUILines::SetTextColor),


		class_<CUIStatic, CUIWindow>("CUIStatic")
		.def(						constructor<>())
		.def("TextControl",			&CUIStatic::TextItemControl)
		.def("SetText",				&CUIStatic::SetText)
		.def("SetTextST",			&CUIStatic::SetTextST)
		.def("GetText",				&CUIStatic::GetText)
		.def("SetFont",				&CUIStatic::SetFont)
		.def("GetFont",				&CUIStatic::GetFont)
		.def("SetColor",			&CUIStatic::SetColor)
		.def("GetColor",			&CUIStatic::GetColor)
		.def("SetTextColor",		(void(CUIStatic::*)(u32))&CUIStatic::SetTextColor)
		.def("SetTextColor",		(void(CUIStatic::*)(u32,u32,u32,u32))&CUIStatic::SetTextColor)
		.def("GetTextColor",		&CUIStatic::GetTextColor)
		.def("SetTextAlignment",	&CUIStatic::SetTextAlignment)
		.def("SetVTextAlignment",	&CUIStatic::SetVTextAlignment)
		.def("SetEllipsis",			&CUIStatic::SetEllipsis)
		.def("SetTextOffset",		&CUIStatic::SetTextOffset)
		.def("SetTextureColor",			&CUIStatic::SetTextureColor)
		.def("GetTextureColor",			&CUIStatic::GetTextureColor)
		.def("InitTexture",			&CUIStatic::InitTexture )
		.def("InitTextureNewRect",			&CUIStatic::InitTexture )
		.def("SetTextureOffset",	&CUIStatic::SetTextureOffset )
		.def("SetTextureRect",		&CUIStatic::SetTextureRect_script)
		.def("SetOriginalRect",		(void(CUIStatic::*)(float,float,float,float))&CUIStatic::SetTextureRect)
		.def("SetStretchTexture",	&CUIStatic::SetStretchTexture)
		.def("GetStretchTexture",	&CUIStatic::GetStretchTexture)
		.def("SetTextAlign",		&CUIStatic::SetTextAlign_script)
		.def("GetTextAlign",		&CUIStatic::GetTextAlign_script)
		.def("GetTextureRect",		&CUIStatic::GetTextureRect_script)
		.def("SetComplexMode",	&CUIStatic::SetTextComplexMode)
		.def("IsComplexMode",	&CUIStatic::IsTextComplexMode)
	];
}