#include "stdafx.h"
#include "UIActorStateIcons.h"
#include "UI/UIXmlInit.h"
#include "hudmanager.h"

CUIActorStateIcons::CUIActorStateIcons()
{
	
}

CUIActorStateIcons::~CUIActorStateIcons()
{
	
}

#define ATTACH_AND_INIT(icon_state, name)														\
	AttachChild										(&m_icons[icon_state]);						\
	xml_init.InitStatic								(xml, name, 0, &m_icons[icon_state]);		\
	m_icons[icon_state].Show						(false);									

void CUIActorStateIcons::Init()
{
	CUIXml		xml;

	string128		STATE_ICONS_XML;
	sprintf_s		(STATE_ICONS_XML, "actor_state_icon_%d.xml", ui_hud_type);

	bool		result			= xml.Init			(CONFIG_PATH, UI_PATH, STATE_ICONS_XML);
	R_ASSERT3										(result, "xml file not found", STATE_ICONS_XML);

	CUIXmlInit	xml_init;
	
	ATTACH_AND_INIT									(eHungerRed,		"hunger_icon:red");
	ATTACH_AND_INIT									(eHungerYellow,		"hunger_icon:yellow");
	ATTACH_AND_INIT									(eHungerGreen,		"hunger_icon:green");
	ATTACH_AND_INIT									(eThirstRed,		"thirst_icon:red");
	ATTACH_AND_INIT									(eThirstYellow,		"thirst_icon:yellow");
	ATTACH_AND_INIT									(eThirstGreen,		"thirst_icon:green");
	ATTACH_AND_INIT									(eSleepRed,			"sleep_icon:red");
	ATTACH_AND_INIT									(eSleepYellow,		"sleep_icon:yellow");
	ATTACH_AND_INIT									(eSleepGreen,		"sleep_icon:green");

}

void CUIActorStateIcons::SetIcons(Flags16 set)
{
	for (u16 i = 0; i < MAX_ICONS; i++)
	{
		m_icons[i].Show								(set.test(1 << i) ? true : false);
	}
}