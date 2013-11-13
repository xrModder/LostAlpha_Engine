// File:		UIMessagesWindow.h
// Description:	Window with MP chat and Game Log ( with PDA messages in single and Kill Messages in MP)
// Created:		22.04.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World

#include "StdAfx.h"

#include "UIMessagesWindow.h"
#include "../level.h"
#include "UIGameLog.h"
#include "xrUIXmlParser.h"
#include "UIXmlInit.h"
#include "UIChatWnd.h"
#include "UIPdaMsgListItem.h"
#include "UIColorAnimatorWrapper.h"
#include "../InfoPortion.h"
#include "../string_table.h"
#include "../game_cl_artefacthunt.h"

CUIMessagesWindow::CUIMessagesWindow(){
	m_pChatLog = NULL;
	m_pChatWnd = NULL;
	m_pGameLog = NULL;
	Init(0, 0, UI_BASE_WIDTH, UI_BASE_HEIGHT);
}

CUIMessagesWindow::~CUIMessagesWindow(){
	
}

void CUIMessagesWindow::AddLogMessage(KillMessageStruct& msg){
	m_pGameLog->AddLogMessage(msg);
}

void CUIMessagesWindow::AddLogMessage(const shared_str& msg){
	m_pGameLog->AddLogMessage(*msg);
}

void CUIMessagesWindow::PendingMode(bool const is_pending_mode)
{
	if (is_pending_mode)
	{
		if (m_in_pending_mode)
			return;
		
		m_pChatWnd->PendingMode	(is_pending_mode);
		m_pChatLog->SetWndRect	(m_pending_chat_log_rect);
		m_in_pending_mode		= true;
		return;
	}
	if (!m_in_pending_mode)
		return;
	
	m_pChatWnd->PendingMode		(is_pending_mode);
	m_pChatLog->SetWndRect		(m_inprogress_chat_log_rect);
	m_in_pending_mode			= false;
}

void CUIMessagesWindow::Init(float x, float y, float width, float height){

	CUIXml		 xml;
	u32			color;
	CGameFont*	pFont;

	xml.Load	(CONFIG_PATH, UI_PATH, "messages_window.xml");

	m_pGameLog = xr_new<CUIGameLog>();m_pGameLog->SetAutoDelete(true);
	m_pGameLog->Show(true);
	AttachChild(m_pGameLog);
	if ( IsGameTypeSingle() )
	{
		CUIXmlInit::InitScrollView(xml, "sp_log_list", 0, m_pGameLog);
	}
	else
	{
		m_pChatLog			= xr_new<CUIGameLog>(); m_pChatLog->SetAutoDelete(true);
		m_pChatLog->Show	(true);
		AttachChild			(m_pChatLog);
		m_pChatWnd			= xr_new<CUIChatWnd>(); m_pChatWnd->SetAutoDelete(true);
		AttachChild			(m_pChatWnd);

		CUIXmlInit::InitScrollView(xml, "mp_log_list", 0, m_pGameLog);
		CUIXmlInit::InitFont(xml, "mp_log_list:font", 0, color, pFont);
		m_pGameLog->SetTextAtrib(pFont, color);

		CUIXmlInit::InitScrollView(xml, "chat_log_list", 0, m_pChatLog);
		CUIXmlInit::InitFont(xml, "chat_log_list:font", 0, color, pFont);
		m_pChatLog->SetTextAtrib(pFont, color);

		m_inprogress_chat_log_rect			= m_pChatLog->GetWndRect();
		m_in_pending_mode					= false;

		LPCSTR LogListName = "chat_log_list_pending";
		XML_NODE* pending_chat_list			= xml.NavigateToNode(LogListName);

		if (pending_chat_list)
		{
			m_pending_chat_log_rect.x1		= xml.ReadAttribFlt(LogListName, 0, "x");
			m_pending_chat_log_rect.y1		= xml.ReadAttribFlt(LogListName, 0, "y");
			m_pending_chat_log_rect.x2		= xml.ReadAttribFlt(LogListName, 0, "width");
			m_pending_chat_log_rect.y2		= xml.ReadAttribFlt(LogListName, 0, "height");
			m_pending_chat_log_rect.rb.add	(m_pending_chat_log_rect.lt);

		}else
			m_pending_chat_log_rect			= m_inprogress_chat_log_rect;
		
		m_pChatWnd->Init	(xml);
	}	

}

void CUIMessagesWindow::AddIconedPdaMessage(LPCSTR textureName, Frect originalRect, LPCSTR message, int iDelay)
{
	CUIPdaMsgListItem *pItem			= m_pGameLog->AddPdaMessage(message, iDelay);
	pItem->UIMsgText.SetTextComplexMode			(true);
	pItem->UIIcon.InitTexture			(textureName);

	Frect texture_rect;
	texture_rect.lt.set					(originalRect.x1,	originalRect.y1);
	texture_rect.rb.set					(originalRect.x2,	originalRect.y2);
	texture_rect.rb.add					(texture_rect.lt);
	pItem->UIIcon.GetUIStaticItem().SetTextureRect		(texture_rect);

	pItem->UIMsgText.SetWndPos			(Fvector2().set(pItem->UIIcon.GetWidth(), pItem->UIMsgText.GetWndPos().y));
	pItem->UIMsgText.AdjustHeightToText	();

	if (pItem->UIIcon.GetHeight() > pItem->UIMsgText.GetHeight())
		pItem->SetHeight				(pItem->UIIcon.GetHeight());
	else
		pItem->SetHeight				(pItem->UIMsgText.GetHeight());

	m_pGameLog->SendMessage				(pItem, CHILD_CHANGED_SIZE);
}

void CUIMessagesWindow::AddChatMessage(shared_str msg, shared_str author)
{
	 m_pChatLog->AddChatMessage(*msg, *author);
}

void CUIMessagesWindow::SetChatOwner(game_cl_GameState* owner)
{
//	if (m_pChatWnd)
//		m_pChatWnd->SetOwner(owner);
}

void CUIMessagesWindow::Update()
{
	CUIWindow::Update();
}