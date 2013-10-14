#pragma once

#include "inventory_item.h"

class CPhysicItem;
class CEntityAlive;
class CActor;

class CEatableItem : public CInventoryItem {
private:
	typedef CInventoryItem	inherited;

protected:
	CPhysicItem		*m_physic_item;

public:
							CEatableItem				();
	virtual					~CEatableItem				();
	virtual	DLL_Pure*		_construct					();
	virtual CEatableItem	*cast_eatable_item			()	{return this;}

	virtual void			Load						(LPCSTR section);
	virtual bool			Useful						() const;

	virtual BOOL			net_Spawn					(CSE_Abstract* DC);

	virtual void			OnH_B_Independent			(bool just_before_destroy);
	virtual	void			UseBy						(CEntityAlive* npc);
	virtual void			save					(NET_Packet &output_packet);
	virtual void			load					(IReader &input_packet);

			bool			Empty						()	const				{return m_iPortionsNum==0;};

	IC	u32				GetPortionsNum		()	const		{	return m_iPortionsNum;}
		void				SetPortionsNum	(u32 value)			{m_iPortionsNum = value; }

protected:	
	//вли€ние при поедании вещи на параметры игрока
	float					m_fHealthInfluence;
	float					m_fPowerInfluence;
	float					m_fSatietyInfluence;
	float					m_fRadiationInfluence;
	float					m_fMaxPowerUpInfluence;
	//заживление ран на кол-во процентов
	float					m_fWoundsHealPerc;
	// lost alpha start
//	BOOL					m_bIsBattery;

	//количество порций еды, 
	//-1 - бесконечно количество порций. 1 - одна.
	u32						m_iPortionsNum;
};

