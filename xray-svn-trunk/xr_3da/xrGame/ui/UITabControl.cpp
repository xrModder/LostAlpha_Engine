#include "StdAfx.h"
#include "UITabControl.h"
#include "UITabButton.h"

CUITabControl::CUITabControl()
	 :m_cGlobalTextColor	(0xFFFFFFFF),
	  m_cActiveTextColor	(0xFFFFFFFF),
	  m_cActiveButtonColor	(0xFFFFFFFF),
	  m_cGlobalButtonColor	(0xFFFFFFFF),
	  m_bAcceleratorsEnable	(true)
{}

CUITabControl::~CUITabControl()
{
	RemoveAll();
}

void CUITabControl::SetCurrentOptValue()
{
	CUIOptionsItem::SetCurrentOptValue();
	int v			= GetOptIntegerValue();
	CUITabButton* b			= GetButtonById(v);
	if(NULL==b)
	{
#ifndef MASTER_GOLD
		Msg("! tab named [%s] doesnt exist", v);
#endif // #ifndef MASTER_GOLD
		v					= m_TabsArr[0]->m_btn_id;
	}
	SetActiveTab			(v);
}

void CUITabControl::SaveOptValue()
{
	CUIOptionsItem::SaveOptValue();
	SaveOptIntegerValue			(GetActiveIndex());
}

void CUITabControl::UndoOptValue()
{
	SetActiveTab		(m_opt_backup_value);
	CUIOptionsItem::UndoOptValue();
}

void CUITabControl::SaveBackUpOptValue()
{
	CUIOptionsItem::SaveBackUpOptValue();
	m_opt_backup_value	= GetActiveIndex();
}

bool CUITabControl::IsChangedOptValue() const
{
	return GetActiveIndex() != m_opt_backup_value;
}

// добавление кнопки-закладки в список закладок контрола
bool CUITabControl::AddItem(LPCSTR pItemName, LPCSTR pTexName, Fvector2 pos, Fvector2 size)
{
	CUITabButton *pNewButton = xr_new<CUITabButton>();
	pNewButton->SetAutoDelete	(true);
	pNewButton->InitButton		(pos, size);
	pNewButton->InitTexture		(pTexName);
	pNewButton->TextItemControl()->SetText(pItemName);
	pNewButton->TextItemControl()->SetTextColor	(m_cGlobalTextColor);
	pNewButton->SetTextureColor	(m_cGlobalButtonColor);

	return AddItem				(pNewButton);
}

bool CUITabControl::AddItem(CUITabButton *pButton)
{
	pButton->SetAutoDelete		(true);
	pButton->Show				(true);
	pButton->Enable				(true);
	pButton->SetButtonAsSwitch	(true);

	AttachChild					(pButton);
	m_TabsArr.push_back			(pButton);
//	R_ASSERT					(pButton->m_btn_id.size());
	return						true;
}

void CUITabControl::RemoveAll()
{
	TABS_VECTOR_it it = m_TabsArr.begin();
	for (; it != m_TabsArr.end(); ++it)
	{
		DetachChild(*it);
	}
	m_TabsArr.clear();
}

void CUITabControl::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if (TAB_CHANGED == msg)
	{
		for (u32 i = 0; i < m_TabsArr.size(); ++i)
		{
			if (m_TabsArr[i] == pWnd)
			{
				m_iPushedIndex = m_TabsArr[i]->m_btn_id;
				if (m_iPrevPushedIndex == m_iPushedIndex)
					return;
                
				OnTabChange(m_iPushedIndex, m_iPrevPushedIndex);
				m_iPrevPushedIndex = m_iPushedIndex;							
				break;
			}
		}
	}

	else if (WINDOW_FOCUS_RECEIVED	== msg	||
			 WINDOW_FOCUS_LOST		== msg)
	{
		for (u8 i = 0; i < m_TabsArr.size(); ++i)
		{
			if (pWnd == m_TabsArr[i])
			{				
				if (msg == WINDOW_FOCUS_RECEIVED)
                    OnStaticFocusReceive(pWnd);
				else
					OnStaticFocusLost(pWnd);
			}
		}
	}
	else
	{
		inherited::SendMessage(pWnd, msg, pData);
	}
}

void CUITabControl::OnStaticFocusReceive(CUIWindow* pWnd)
{
	GetMessageTarget()->SendMessage			(this, WINDOW_FOCUS_RECEIVED, static_cast<void*>(pWnd));
}

void CUITabControl::OnStaticFocusLost(CUIWindow* pWnd)
{
	GetMessageTarget()->SendMessage			(this, WINDOW_FOCUS_LOST, static_cast<void*>(pWnd));
}

void CUITabControl::OnTabChange(const int iCur, const int iPrev)
{
	CUITabButton* tb_cur					= GetButtonById			(iCur);
	CUITabButton* tb_prev					= GetButtonById			(iPrev);
	if(tb_prev)	
		tb_prev->SendMessage				(tb_cur, TAB_CHANGED, NULL);

	tb_cur->SendMessage						(tb_cur, TAB_CHANGED, NULL);	

	GetMessageTarget()->SendMessage			(this, TAB_CHANGED, NULL);
}

void CUITabControl::SetActiveTab(const int iNewTab)
{
	if (m_iPushedIndex == iNewTab)
		return;
    
	m_iPushedIndex			= iNewTab;
	OnTabChange			(m_iPushedIndex, m_iPrevPushedIndex);
	
	m_iPrevPushedIndex		= m_iPushedIndex;
}

bool CUITabControl::OnKeyboardAction(int dik, EUIMessages keyboard_action)
{

	if (GetAcceleratorsMode() && WINDOW_KEY_PRESSED == keyboard_action)
	{
		for (u32 i = 0; i < m_TabsArr.size(); ++i)
		{
			if (m_TabsArr[i]->IsAccelerator(dik) )
			{
				SetActiveTab(m_TabsArr[i]->m_btn_id);
				return	true;
			}
		}
	}
	return false;
}
struct STabsArrayPred
{
public:
	int m_id;
	STabsArrayPred(int id) : m_id(id) { }
	bool operator () (const CUITabButton* btn)
	{
		return (btn->m_btn_id==m_id);
	}
};

CUITabButton* CUITabControl::GetButtonById(const int id)
{ 
	TABS_VECTOR::const_iterator it = std::find_if(m_TabsArr.begin(), m_TabsArr.end(), STabsArrayPred(id));
	if(it!=m_TabsArr.end())
		return *it;
	else
		return NULL;
}
/*
const shared_str CUITabControl::GetCommandName(const shared_str& id)
{ 
	CUITabButton* tb			= GetButtonById(id);
	R_ASSERT2					(tb, id.c_str());

	return (GetButtonByIndex(i))->WindowName();
};

CUIButton* CUITabControl::GetButtonByCommand(const shared_str& n)
{
	for(u32 i = 0; i<m_TabsArr.size(); ++i)
		if(m_TabsArr[i]->WindowName() == n)
			return m_TabsArr[i];

	return NULL;
}*/

void CUITabControl::ResetTab()
{
	for (u32 i = 0; i < m_TabsArr.size(); ++i)
	{
		m_TabsArr[i]->SetButtonState(CUIButton::BUTTON_NORMAL);
	}
	m_iPushedIndex		= -1;
	m_iPrevPushedIndex	= -1;
}

void CUITabControl::Enable(bool status)
{
	for(u32 i=0; i<m_TabsArr.size(); ++i)
		m_TabsArr[i]->Enable(status);

	inherited::Enable(status);
}
