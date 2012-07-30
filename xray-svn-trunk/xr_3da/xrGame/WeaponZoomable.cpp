#include "stdafx.h"
#include "WeaponZoomable.h"

CWeaponZoomable::CWeaponZoomable() : CWeaponBinoculars()
{
	m_bIsZoom		= true;
}

CWeaponZoomable::~CWeaponZoomable()
{
}

void CWeaponZoomable::Load	(LPCSTR section)
{
	inherited::Load(section);

	if(pSettings->line_exist(section,"activity_zoom"))
		m_bIsZoom = !!pSettings->r_bool(section,"activity_zoom");
}

void CWeaponZoomable::GetBriefInfo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count)
{
		CWeaponCustomPistol::GetBriefInfo		(str_name, icon_sect_name, str_count);
}

bool CWeaponZoomable::Action(s32 cmd, u32 flags) 
{
	return CWeaponCustomPistol::Action(cmd, flags);
}

void CWeaponZoomable::OnZoomIn		()
{
	if (m_bIsZoom)
		inherited::OnZoomIn();
	else
		CWeaponMagazined::OnZoomIn();
}

void CWeaponZoomable::OnZoomOut		()
{
	if (m_bIsZoom)
		inherited::OnZoomOut();
	else
		CWeaponMagazined::OnZoomOut();
}

void CWeaponZoomable::ZoomInc()
{
	if (m_bIsZoom)
	inherited::ZoomInc();
}

void CWeaponZoomable::ZoomDec()
{
	if (m_bIsZoom)
	inherited::ZoomDec();
}

void CWeaponZoomable::switch2_Fire	()
{
	CWeaponMagazined::switch2_Fire();
}


void CWeaponZoomable::FireEnd() 
{
	CWeaponMagazined::FireEnd();
}