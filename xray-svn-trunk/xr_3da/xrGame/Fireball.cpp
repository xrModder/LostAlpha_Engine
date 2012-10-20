#include "stdafx.h"
#include "Fireball.h"
#include "xr_level_controller.h"

CFireball::CFireball() : CWeaponKnife()
{
}

CFireball::~CFireball(void)
{
	HUD_SOUND::DestroySound(m_sndShot);
}

void CFireball::Load(LPCSTR section)
{
	//inherited::Load(section);
	CWeapon::Load(section);

	fWallmarkSize = pSettings->r_float(section,"wm_size");

	// HUD :: Anims
	R_ASSERT			(m_pHUD);
	animGet				(mhud_idle,		pSettings->r_string(*hud_sect,"anim_idle"));
	animGet				(mhud_hide,		pSettings->r_string(*hud_sect,"anim_hide"));
	animGet				(mhud_show,		pSettings->r_string(*hud_sect,"anim_draw"));
	animGet				(mhud_attack,	pSettings->r_string(*hud_sect,"anim_shoot"));
	animGet				(mhud_activate,	pSettings->r_string(*hud_sect,"anim_prefire"));
	
	if(pSettings->line_exist(*hud_sect,"anim_idle_sprint"))
		animGet				(mhud_idle_sprint,		pSettings->r_string(*hud_sect,"anim_idle_sprint"));

	HUD_SOUND::LoadSound(section,"snd_shoot"		, m_sndShot		, ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING)		);
	
	knife_material_idx =  GMLib.GetMaterialIdx(pSettings->r_string(*hud_sect,"fireball_material"));
}

bool CFireball::Action(u16 cmd, u32 flags)
{
	switch(cmd){
		case kWPN_FIRE: {
			if (flags&CMD_START){
				SwitchState(ePreFire);
				return true;
			} else if (flags&CMD_STOP && GetState()==ePreFire) {
				SwitchState(eIdle);
				return true;
			}
		} return true;
		case kWPN_ZOOM: return true;

	}
	return inherited::Action(cmd, flags);
}

void CFireball::OnStateSwitch		(u32 S)
{
	if (S==eFire) {
		int t=10;
	}
	inherited::OnStateSwitch(S);
	switch (S) {
		case eFire: {
			StartFlameParticles();
			fTime			+=	fTimeToFire;
		}return;
		case eFire2: {
			Msg("eFire2 CFireball called!!! WTF???");
					 }return;
		case ePreFire: {
			switch2_PreFire	();
		}break;
	}
}



void CFireball::switch2_PreFire	()
{
	if(m_bPending)	return;

	m_pHUD->animPlay(random_anim(mhud_activate),		FALSE, this, ePreFire);
	m_bPending		= true;
}

void CFireball::OnAnimationEnd		(u32 state)
{
	switch (state)
	{
	case eFire: {
				Fvector	p1, d; 
				p1.set(get_LastFP()); 
				d.set(get_LastFD());

				KnifeStrike(p1,d);

				SwitchState(eIdle);
		}return;
		case ePreFire:  m_bPending=false; SwitchState(eFire); return;

	}
	inherited::OnAnimationEnd(state);
}


void CFireball::LoadFireParams(LPCSTR section, LPCSTR prefix)
{
	CWeapon::LoadFireParams(section, prefix);

	fvHitPower_1		= fvHitPower;
	fHitImpulse_1		= fHitImpulse;
	m_eHitType_1		= ALife::g_tfString2HitType(pSettings->r_string(section, "hit_type"));
}



void CFireball::UpdateCL			()
{
	inherited::UpdateCL	();
	float dt = Device.fTimeDelta;

	

	//когда происходит апдейт состояния оружия
	//ничего другого не делать
	if(GetNextState() == GetState())
	{
		switch (GetState())
		{
		case eShowing:
		case eHiding:
		case eIdle:
			fTime			-=	dt;
			if (fTime<0)	
				fTime = 0;
			break;
		case eFire:			
			//if(iAmmoElapsed>0)
			//	state_Fire		(dt);
			
			if(fTime<=0)	StopShooting();
			else			fTime -= dt;

			break;
		case eHidden:		break;
		}
	}
}




#include "pch_script.h"

using namespace luabind;

void CFireball::script_register	(lua_State *L)
{
	module(L)
	[
		class_<CFireball,CGameObject>("CFireball")
			.def(constructor<>())
	];
}



