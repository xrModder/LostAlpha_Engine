#include "stdafx.h"
#include "WeaponAutomaticShotgun.h"
#include "entity.h"
#include "ParticlesObject.h"
#include "xr_level_controller.h"
#include "inventory.h"
#include "level.h"
#include "actor.h"

CWeaponAutomaticShotgun::CWeaponAutomaticShotgun()
{
	m_eSoundClose			= ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING);
	m_eSoundAddCartridge	= ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING);
}

CWeaponAutomaticShotgun::~CWeaponAutomaticShotgun()
{
}

void CWeaponAutomaticShotgun::Load(LPCSTR section)
{
	inherited::Load(section);

	if(pSettings->line_exist(section, "tri_state_reload")){
		m_bTriStateReload = !!pSettings->r_bool(section, "tri_state_reload");
	};
	if(m_bTriStateReload){
		HUD_SOUND::LoadSound(section, "snd_open_weapon", m_sndOpen, m_eSoundOpen);
		animGet	(mhud_open,	pSettings->r_string(*hud_sect,"anim_open_weapon"));

		HUD_SOUND::LoadSound(section, "snd_add_cartridge", m_sndAddCartridge, m_eSoundAddCartridge);
		animGet	(mhud_add_cartridge,	pSettings->r_string(*hud_sect,"anim_add_cartridge"));

		HUD_SOUND::LoadSound(section, "snd_close_weapon", m_sndClose, m_eSoundClose);
		animGet	(mhud_close,	pSettings->r_string(*hud_sect,"anim_close_weapon"));
	};

}

bool CWeaponAutomaticShotgun::Action(u16 cmd, u32 flags) 
{
	if(inherited::Action(cmd, flags)) return true;

	if(	m_bTriStateReload && GetState()==eReload &&
		cmd==kWPN_FIRE && flags&CMD_START &&
		m_sub_state==eSubstateReloadInProcess		)//остановить перезагрузку
	{
		AddCartridge(1);
		m_sub_state = eSubstateReloadEnd;
		return true;
	}
	return false;
}

void CWeaponAutomaticShotgun::OnAnimationEnd(u32 state) 
{
	if(!m_bTriStateReload || state != eReload)
		return inherited::OnAnimationEnd(state);

	switch(m_sub_state){
		case eSubstateReloadBegin:{
			m_sub_state = eSubstateReloadInProcess;
			SwitchState(eReload);
		}break;

		case eSubstateReloadInProcess:{
			if( 0 != AddCartridge(1) ){
				m_sub_state = eSubstateReloadEnd;
			}
			SwitchState(eReload);
		}break;

		case eSubstateReloadEnd:{
			m_sub_state = eSubstateReloadBegin;
			SwitchState(eIdle);
		}break;
		
	};
}

void CWeaponAutomaticShotgun::Reload() 
{
	if(m_bTriStateReload){
		TriStateReload();
	}else
		inherited::Reload();
}

void CWeaponAutomaticShotgun::TriStateReload()
{
	if( m_magazine.size() == (u32)iMagazineSize || !HaveCartridgeInInventory(1) )return;
	CWeapon::Reload		();
	m_sub_state			= eSubstateReloadBegin;
	SwitchState			(eReload);
}

void CWeaponAutomaticShotgun::OnStateSwitch	(u32 S)
{
	if(!m_bTriStateReload || S != eReload){
		inherited::OnStateSwitch(S);
		return;
	}

	CWeapon::OnStateSwitch(S);

	if( m_magazine.size() == (u32)iMagazineSize || !HaveCartridgeInInventory(1) ){
			switch2_EndReload		();
			m_sub_state = eSubstateReloadEnd;
			return;
	};

	switch (m_sub_state)
	{
	case eSubstateReloadBegin:
		if( HaveCartridgeInInventory(1) )
			switch2_StartReload	();
		break;
	case eSubstateReloadInProcess:
			if( HaveCartridgeInInventory(1) )
				switch2_AddCartgidge	();
		break;
	case eSubstateReloadEnd:
			switch2_EndReload		();
		break;
	};
}

void CWeaponAutomaticShotgun::switch2_StartReload()
{
	PlaySound			(m_sndOpen, get_LastFP());
	PlayAnimOpenWeapon	();
	m_bPending			= true;
}

void CWeaponAutomaticShotgun::switch2_AddCartgidge	()
{
	PlaySound						(m_sndAddCartridge, get_LastFP());
	PlayAnimAddOneCartridgeWeapon	();
	m_bPending						= true;
}

void CWeaponAutomaticShotgun::switch2_EndReload	()
{
	m_bPending			= false;
	PlaySound			(m_sndClose, get_LastFP());
	PlayAnimCloseWeapon	();
}

void CWeaponAutomaticShotgun::PlayAnimOpenWeapon()
{
	VERIFY(GetState()==eReload);
	m_pHUD->animPlay(random_anim(mhud_open), TRUE, this, GetState());
}
void CWeaponAutomaticShotgun::PlayAnimAddOneCartridgeWeapon()
{
	VERIFY(GetState()==eReload);
	m_pHUD->animPlay(random_anim(mhud_add_cartridge), TRUE, this, GetState());
	LPCSTR AnimName = "_reload";
	CWeapon::WeaponCamEffector(AnimName);
}
void CWeaponAutomaticShotgun::PlayAnimCloseWeapon()
{
	VERIFY(GetState()==eReload);

	m_pHUD->animPlay(random_anim(mhud_close), TRUE, this, GetState());
}

bool CWeaponAutomaticShotgun::HaveCartridgeInInventory(u8 cnt)
{
	if (unlimited_ammo()) return true;
	m_pAmmo = NULL;
	if (m_pCurrentInventory)
	{
		//попытаться найти в инвентаре патроны текущего типа 
		m_pAmmo = smart_cast<CWeaponAmmo*>(m_pCurrentInventory->GetAny(*m_ammoTypes[m_ammoType]));

		if (!m_pAmmo)
		{
			for (u32 i = 0; i < m_ammoTypes.size(); ++i)
			{
				//проверить патроны всех подходящих типов
				m_pAmmo = smart_cast<CWeaponAmmo*>(m_pCurrentInventory->GetAny(*m_ammoTypes[i]));
				if (m_pAmmo)
				{
					m_ammoType = i;
					break;
				}
			}
		}
	}
	return (m_pAmmo != NULL) && (m_pAmmo->m_boxCurr >= cnt);
}


u8 CWeaponAutomaticShotgun::AddCartridge		(u8 cnt)
{
	if (IsMisfire())	bMisfire = false;

	if (m_set_next_ammoType_on_reload != u8(-1)){
		m_ammoType = m_set_next_ammoType_on_reload;
		m_set_next_ammoType_on_reload = u8(-1);

	}

	if (!HaveCartridgeInInventory(1))
		return 0;

	VERIFY((u32)iAmmoElapsed == m_magazine.size());


	if (m_DefaultCartridge.m_LocalAmmoType != m_ammoType)
		m_DefaultCartridge.Load(*m_ammoTypes[m_ammoType], u8(m_ammoType));
	CCartridge l_cartridge = m_DefaultCartridge;
	while (cnt)// && m_pAmmo->Get(l_cartridge)) 
	{
		if (!unlimited_ammo())
		{
			if (!m_pAmmo->Get(l_cartridge)) break;
		}
		--cnt;
		++iAmmoElapsed;
		l_cartridge.m_LocalAmmoType = u8(m_ammoType);
		m_magazine.push_back(l_cartridge);
		//		m_fCurrentCartirdgeDisp = l_cartridge.m_kDisp;
	}
	m_ammoName = (m_pAmmo) ? m_pAmmo->m_nameShort : NULL;

	VERIFY((u32)iAmmoElapsed == m_magazine.size());

	//выкинуть коробку патронов, если она пустая
	if (m_pAmmo && !m_pAmmo->m_boxCurr && OnServer())
		m_pAmmo->SetDropManual(TRUE);

	return cnt;
}

void	CWeaponAutomaticShotgun::net_Export	(NET_Packet& P)
{
	inherited::net_Export(P);	
	P.w_u8(u8(m_magazine.size()));	
	for (u32 i=0; i<m_magazine.size(); i++)
	{
		CCartridge& l_cartridge = *(m_magazine.begin()+i);
		P.w_u8(l_cartridge.m_LocalAmmoType);
	}
}

void	CWeaponAutomaticShotgun::net_Import	(NET_Packet& P)
{
	inherited::net_Import(P);	
	u8 AmmoCount = P.r_u8();
	for (u32 i=0; i<AmmoCount; i++)
	{
		u8 LocalAmmoType = P.r_u8();
		if (i>=m_magazine.size()) continue;
		CCartridge& l_cartridge = *(m_magazine.begin()+i);
		if (LocalAmmoType == l_cartridge.m_LocalAmmoType) continue;
#ifdef DEBUG
		Msg("! %s reload to %s", *l_cartridge.m_ammoSect, m_ammoTypes[LocalAmmoType].c_str());
#endif
		l_cartridge.Load( m_ammoTypes[LocalAmmoType].c_str(), LocalAmmoType );
	}
}
