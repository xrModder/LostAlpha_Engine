#include "stdafx.h"
#include "UIMpTradeWnd.h"

#include "UIXmlInit.h"
#include "UIMpItemsStoreWnd.h"
#include "UITabControl.h"
#include "UITabButtonMP.h"
#include "UIDragDropListEx.h"
#include "UIItemInfo.h"
#include "UIHelper.h"

#include "../object_broker.h"

LPCSTR _list_names[]= {
		"lst_pistol",
		"lst_pistol_ammo",
		"lst_rifle",
		"lst_rifle_ammo",
		"lst_outfit",
		"lst_medkit",
		"lst_granade",
		"lst_others",
		"lst_player_bag",
		"lst_shop",
};
CUIMpTradeWnd::CUIMpTradeWnd()
{
	m_money								= 0;
	g_mp_restrictions.InitGroups		();
	m_bIgnoreMoneyAndRank				= false;
}

CUIMpTradeWnd::~CUIMpTradeWnd()
{
	m_root_tab_control->RemoveAll		();
	delete_data							(m_store_hierarchy);
	delete_data							(m_list[e_shop]);
	delete_data							(m_all_items);
	delete_data							(m_item_mngr);
}

void CUIMpTradeWnd::Init(const shared_str& sectionName, const shared_str& sectionPrice)
{
	m_sectionName						= sectionName;
	m_sectionPrice						= sectionPrice;

	CUIXml								xml_doc;
	xml_doc.Load							(CONFIG_PATH, UI_PATH, "mp_buy_menu.xml");

	m_store_hierarchy					= xr_new<CStoreHierarchy>();
 	m_store_hierarchy->Init				(xml_doc, "items_hierarchy");
	m_store_hierarchy->InitItemsInGroup	(m_sectionName);

	CUIXmlInit::InitWindow				(xml_doc, "main",						0, this);

	m_root_tab_control					= xr_new<CUITabControl>(); AttachChild(m_root_tab_control); m_root_tab_control->SetAutoDelete(true);
	CUIXmlInit::InitTabControl			(xml_doc, "tab_control",				0, m_root_tab_control);
	Register							(m_root_tab_control);
	AddCallback							(m_root_tab_control,	TAB_CHANGED,		CUIWndCallback::void_function	(this, &CUIMpTradeWnd::OnRootTabChanged));

	u32 root_cnt						= m_store_hierarchy->GetRoot().ChildCount();
	for(u32 i=0; i<root_cnt; ++i)
	{
		const CStoreHierarchy::item& it	= m_store_hierarchy->GetRoot().ChildAt(i);

		CUITabButtonMP* btn				= it.m_button;	
		m_root_tab_control->AddItem		(btn);
		btn->SetAutoDelete				(false);
	}
	m_root_tab_control->ResetTab		();

	m_shop_wnd							= xr_new<CUIWindow>();	AttachChild(m_shop_wnd);		m_shop_wnd->SetAutoDelete(true);
	CUIXmlInit::InitWindow				(xml_doc, "shop_wnd",	0, m_shop_wnd);

	m_btn_ok							= xr_new<CUI3tButton>();AttachChild(m_btn_ok);					m_btn_ok->SetAutoDelete			(true);
	m_btn_cancel						= xr_new<CUI3tButton>();AttachChild(m_btn_cancel);				m_btn_cancel->SetAutoDelete		(true);
	m_btn_shop_back						= xr_new<CUI3tButton>();AttachChild(m_btn_shop_back);			m_btn_shop_back->SetAutoDelete	(true);
	m_btns_preset[0]					= xr_new<CUI3tButton>();AttachChild(m_btns_preset[0]	);		m_btns_preset[0]->SetAutoDelete	(true);
	m_btns_preset[1]					= xr_new<CUI3tButton>();AttachChild(m_btns_preset[1]	);		m_btns_preset[1]->SetAutoDelete	(true);
	m_btns_preset[2]					= xr_new<CUI3tButton>();AttachChild(m_btns_preset[2]	);		m_btns_preset[2]->SetAutoDelete	(true);
	m_btns_preset[3]					= xr_new<CUI3tButton>();AttachChild(m_btns_preset[3]	);		m_btns_preset[3]->SetAutoDelete	(true);
	m_btns_preset[4]					= xr_new<CUI3tButton>();AttachChild(m_btns_preset[4]	);		m_btns_preset[4]->SetAutoDelete	(true);
	m_btns_save_preset[0]				= xr_new<CUI3tButton>();AttachChild(m_btns_save_preset[0]);		m_btns_save_preset[0]->SetAutoDelete(true);
	m_btns_save_preset[1]				= xr_new<CUI3tButton>();AttachChild(m_btns_save_preset[1]);		m_btns_save_preset[1]->SetAutoDelete(true);
	m_btns_save_preset[2]				= xr_new<CUI3tButton>();AttachChild(m_btns_save_preset[2]);		m_btns_save_preset[2]->SetAutoDelete(true);
	m_btn_reset							= xr_new<CUI3tButton>();AttachChild(m_btn_reset		);			m_btn_reset->SetAutoDelete		(true);
	m_btn_sell							= xr_new<CUI3tButton>();AttachChild(m_btn_sell		);			m_btn_sell->SetAutoDelete		(true);

	m_btn_pistol_ammo					= xr_new<CUI3tButton>();AttachChild(m_btn_pistol_ammo	);		m_btn_pistol_ammo->SetAutoDelete		(true);
	m_btn_pistol_silencer				= xr_new<CUI3tButton>();AttachChild(m_btn_pistol_silencer);		m_btn_pistol_silencer->SetAutoDelete	(true);
	m_btn_rifle_ammo					= xr_new<CUI3tButton>();AttachChild(m_btn_rifle_ammo	);		m_btn_rifle_ammo->SetAutoDelete			(true);
	m_btn_rifle_silencer				= xr_new<CUI3tButton>();AttachChild(m_btn_rifle_silencer);		m_btn_rifle_silencer->SetAutoDelete		(true);
	m_btn_rifle_scope					= xr_new<CUI3tButton>();AttachChild(m_btn_rifle_scope	);		m_btn_rifle_scope->SetAutoDelete		(true);
	m_btn_rifle_glauncher				= xr_new<CUI3tButton>();AttachChild(m_btn_rifle_glauncher);		m_btn_rifle_glauncher->SetAutoDelete	(true);
	m_btn_rifle_ammo2					= xr_new<CUI3tButton>();AttachChild(m_btn_rifle_ammo2	);		m_btn_rifle_ammo2->SetAutoDelete		(true);


	CUIXmlInit::Init3tButton			(xml_doc, "btn_ok",				0, m_btn_ok				);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_cancel",			0, m_btn_cancel			);
	CUIXmlInit::Init3tButton			(xml_doc, "shop_back_btn",		0, m_btn_shop_back		);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_preset_1",		0, m_btns_preset[1]		);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_preset_2",		0, m_btns_preset[2]		);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_preset_3",		0, m_btns_preset[3]		);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_last_set",		0, m_btns_preset[0]		);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_preset_def",		0, m_btns_preset[4]		);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_save_preset_1",	0, m_btns_save_preset[0]);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_save_preset_2",	0, m_btns_save_preset[1]);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_save_preset_3",	0, m_btns_save_preset[2]);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_reset",			0, m_btn_reset			);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_sell",			0, m_btn_sell			);

	CUIXmlInit::Init3tButton			(xml_doc, "btn_pistol_ammo",	0, m_btn_pistol_ammo	);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_pistol_silencer",0, m_btn_pistol_silencer);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_rifle_ammo",		0, m_btn_rifle_ammo		);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_rifle_silencer",	0, m_btn_rifle_silencer	);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_rifle_scope",	0, m_btn_rifle_scope	);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_rifle_glauncher",0, m_btn_rifle_glauncher);
	CUIXmlInit::Init3tButton			(xml_doc, "btn_rifle_ammo2",	0, m_btn_rifle_ammo2	);



	Register							(m_btn_ok				);
	Register							(m_btn_cancel			);
	Register							(m_btn_shop_back		);
	Register							(m_btns_preset[0]		);
	Register							(m_btns_preset[1]		);
	Register							(m_btns_preset[2]		);
	Register							(m_btns_preset[3]		);
	Register							(m_btns_preset[4]		);
	Register							(m_btns_save_preset[0]	);
	Register							(m_btns_save_preset[1]	);
	Register							(m_btns_save_preset[2]	);
	Register							(m_btn_reset			);
	Register							(m_btn_sell				);
	Register							(m_btn_pistol_ammo		);
	Register							(m_btn_pistol_silencer	);
	Register							(m_btn_rifle_ammo		);
	Register							(m_btn_rifle_silencer	);
	Register							(m_btn_rifle_scope		);
	Register							(m_btn_rifle_glauncher	);
	Register							(m_btn_rifle_ammo2		);


	AddCallback							(m_btn_ok,			BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnOkClicked));
	AddCallback							(m_btn_cancel,		BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnCancelClicked));
	AddCallback							(m_btn_shop_back,	BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnShopBackClicked));
	AddCallbackStr						("sub_btn",			TAB_CHANGED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnSubLevelBtnClicked));
	AddCallbackStr						("sub_btn",			BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnSubLevelBtnClicked));
	
	AddCallback							(m_btns_preset[1],	BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnPreset1Clicked		));
	AddCallback							(m_btns_preset[2],	BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnPreset2Clicked		));
	AddCallback							(m_btns_preset[3],	BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnPreset3Clicked		));
	AddCallback							(m_btns_preset[4],	BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnPresetDefaultClicked	));
	AddCallback							(m_btns_preset[0],	BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnLastSetClicked		));
	
	AddCallback							(m_btns_save_preset[0],BUTTON_CLICKED,	CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnSave1PresetClicked	));
	AddCallback							(m_btns_save_preset[1],BUTTON_CLICKED,	CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnSave2PresetClicked	));
	AddCallback							(m_btns_save_preset[2],BUTTON_CLICKED,	CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnSave3PresetClicked	));
	AddCallback							(m_btn_reset,		BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnResetClicked		));
	AddCallback							(m_btn_sell,		BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnSellClicked		));

	AddCallback							(m_btn_pistol_silencer,	BUTTON_CLICKED,	CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnPistolSilencerClicked	));
	AddCallback							(m_btn_rifle_silencer,BUTTON_CLICKED,	CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnRifleSilencerClicked	));
	AddCallback							(m_btn_rifle_scope,	BUTTON_CLICKED,		CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnRifleScopeClicked		));
	AddCallback							(m_btn_rifle_glauncher,BUTTON_CLICKED,	CUIWndCallback::void_function	(this,	&CUIMpTradeWnd::OnBtnRifleGLClicked			));

	for(int idx = e_first; idx<e_total_lists; ++idx)
	{
		CUIDragDropListEx* lst			= xr_new<CUIDragDropListEx>();
		m_list[idx]						= lst;
		if(idx!=e_shop)
		{
			AttachChild					(lst);
			lst->SetAutoDelete			(true);
		}
		CUIXmlInit::InitDragDropListEx	(xml_doc, _list_names[idx], 0, lst);
		BindDragDropListEvents			(lst, true);
	}

	m_static_player_money				= UIHelper::CreateTextWnd( xml_doc, "static_player_money",		this );
	m_static_curr_items_money			= UIHelper::CreateTextWnd( xml_doc, "static_curr_items_money",	this );
	
	m_static_preset_money[0]			= UIHelper::CreateTextWnd( xml_doc, "static_preset_money_last",	this );
	m_static_preset_money[1]			= UIHelper::CreateTextWnd( xml_doc, "static_preset_money_1",		this );
	m_static_preset_money[2]			= UIHelper::CreateTextWnd( xml_doc, "static_preset_money_2",		this );
	m_static_preset_money[3]			= UIHelper::CreateTextWnd( xml_doc, "static_preset_money_3",		this );
	m_static_preset_money[4]			= UIHelper::CreateTextWnd( xml_doc, "static_preset_money_def",	this );

// preset money indicators

	m_item_color_restr_rank					= CUIXmlInit::GetColor	(xml_doc, "item_color_restr_rank",	0, color_rgba(255,255,255,255));
	m_item_color_restr_money				= CUIXmlInit::GetColor	(xml_doc, "item_color_restr_money",	0, color_rgba(255,255,255,255));
	m_item_color_normal						= CUIXmlInit::GetColor	(xml_doc, "item_color_normal",		0, color_rgba(255,255,255,255));
	
	m_text_color_money_positive				= CUIXmlInit::GetColor	(xml_doc, "money_color_positive",	0, color_rgba(255,255,255,255));
	m_text_color_money_negative				= CUIXmlInit::GetColor	(xml_doc, "money_color_negative",	0, color_rgba(255,255,255,255));

	m_static_player_rank				= UIHelper::CreateStatic( xml_doc, "static_player_rank",	this );
	m_static_item_rank					= UIHelper::CreateStatic( xml_doc, "static_item_rank",		this );
	m_static_information				= UIHelper::CreateTextWnd( xml_doc, "static_info",			this );
	m_static_money_change				= UIHelper::CreateTextWnd( xml_doc, "static_money_change",	this );

	
	m_item_info							= xr_new<CUIItemInfo>();
	AttachChild							(m_item_info); m_item_info->SetAutoDelete(true);
	m_item_info->InitItemInfo			(Fvector2().set(0,0), Fvector2().set(100,100), "buy_menu_item.xml");

	m_item_mngr							= xr_new<CItemMgr>();
	m_item_mngr->Load					(sectionPrice);
#ifndef MASTER_GOLD
	m_item_mngr->Dump					();
#endif // #ifndef MASTER_GOLD

	SetRank								(0);
	UpdateShop							();
	SetCurrentItem						(NULL);
}