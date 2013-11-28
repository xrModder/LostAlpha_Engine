#include "stdafx.h"
#include "../pch_script.h"

#include "UIListBox.h"
#include "UIListBoxItem.h"
#include "UIListBoxItemEx.h"
#include "UIListBoxItemMsgChain.h"
#include "ServerList.h"
#include "UIMapList.h"
#include "UISpinText.h"
#include "UIMapInfo.h"
#include "UIComboBox.h"
#include "../../device.h"

using namespace luabind;


struct CUIListBoxItemWrapper : public CUIListBoxItem, public luabind::wrap_base 
{
	CUIListBoxItemWrapper():CUIListBoxItem(){}
};

struct CUIListBoxItemExWrapper : public CUIListBoxItemEx, public luabind::wrap_base
{
	CUIListBoxItemExWrapper():CUIListBoxItemEx(){}
};

struct CUIListBoxItemMsgChainWrapper : public CUIListBoxItemMsgChain, public luabind::wrap_base
{
	CUIListBoxItemMsgChainWrapper() : CUIListBoxItemMsgChain() {}
};

bool xrRender_test_hw_script()
{
	return !!Device.m_pRender->Render_test_hw();
}

#pragma optimize("s",on)
void CUIListBox::script_register(lua_State *L)
{

	module(L)
	[

		class_<CUIListBox, CUIScrollView>("CUIListWnd")
		.def(							constructor<>())
		.def("ShowSelectedItem",		&CUIListBox::Show)
		.def("RemoveAll",				&CUIListBox::Clear)
		.def("GetSize",					&CUIListBox::GetSize)
		.def("GetSelectedItem",			&CUIListBox::GetSelectedItem)
		.def("GetSelectedIndex",		&CUIListBox::GetSelectedIDX)		

		.def("GetItemByTag",			&CUIListBox::GetItemByTAG)
		.def("GetItemByText",			&CUIListBox::GetItemByText)			
		.def("GetItem",					&CUIListBox::GetItemByIDX)		
		.def("RemoveItem",				&CUIListBox::RemoveItem)
		.def("Remove",				&CUIListBox::Remove)
		.def("AddTextItem",				&CUIListBox::AddTextItem)

		.def("SetSelectionTexture",				&CUIListBox::SetSelectionTexture)
		.def("SetItemHeight",				&CUIListBox::SetItemHeight)
		.def("GetItemHeight",				&CUIListBox::GetItemHeight)
		.def("GetLongestLength",				&CUIListBox::GetLongestLength)
		.def("SetSelected",				&CUIListBox::SetSelectedIDX)
		.def("SetSelectedTag",				&CUIListBox::SetSelectedTAG)
		.def("SetSelectedText",				&CUIListBox::SetSelectedText)
		.def("SetImmediateSelection",				&CUIListBox::SetImmediateSelection)
		.def("SetTextColor",				&CUIListBox::SetTextColor)
		.def("GetTextColor",				&CUIListBox::GetTextColor)

		.def("AddItem",         &CUIListBox::AddExistingItem, adopt(_2)),

		class_<CUIListBoxItem, CUIFrameLineWnd, CUIListBoxItemWrapper>("CUIListItem")
		.def(							constructor<>())
		.def("GetTextItem",             &CUIListBoxItem::GetTextItem)
		.def("AddTextField",            &CUIListBoxItem::AddTextField)
		.def("AddIconField",            &CUIListBoxItem::AddIconField)
		.def("SetTextColor",			&CUIListBoxItem::SetTextColor),

		class_<CUIListBoxItemEx, CUIListBoxItem, CUIListBoxItemExWrapper>("CUIListItemEx")
		.def(							constructor<>())
		.def("SetSelectionColor",		&CUIListBoxItemEx::SetSelectionColor),

		class_<CUIMapList, CUIWindow>("CUIMapList")
		.def(							constructor<>())
		.def("SetWeatherSelector",		&CUIMapList::SetWeatherSelector)
		.def("SetModeSelector",			&CUIMapList::SetModeSelector)
		.def("OnModeChange",			&CUIMapList::OnModeChange)
		.def("LoadMapList",				&CUIMapList::LoadMapList)
		.def("SaveMapList",				&CUIMapList::SaveMapList)
		.def("GetCommandLine",			&CUIMapList::GetCommandLine)
		.def("SetServerParams",			&CUIMapList::SetServerParams)
		.def("GetCurGameType",			&CUIMapList::GetCurGameType)
		.def("StartDedicatedServer",	&CUIMapList::StartDedicatedServer)
		.def("SetMapPic",				&CUIMapList::SetMapPic)
		.def("SetMapInfo",				&CUIMapList::SetMapInfo)
		.def("IsEmpty",					&CUIMapList::IsEmpty),

		class_<enum_exporter<EGameTypes> >("GAME_TYPE")
		.enum_("gametype")
		[
			value("GAME_UNKNOWN",			int(GAME_ANY)),
			value("GAME_DEATHMATCH",		int(GAME_DEATHMATCH)),
			value("GAME_TEAMDEATHMATCH",	int(GAME_TEAMDEATHMATCH)),
			value("GAME_ARTEFACTHUNT",		int(GAME_ARTEFACTHUNT))
		],
		def("xrRender_test_r2_hw",			&xrRender_test_hw_script)
	];
}