#pragma once
#ifndef __CONSTANT_DEBUG_H__
#define __CONSTANT_DEBUG_H__

class CGameFont;

class ENGINE_API CConstantsDebug 
{
	DEFINE_MAP(shared_str, Fvector3, VEC3_STORAGE, VEC3_STORAGE_IT);
	DEFINE_MAP(shared_str, Fvector4, VEC4_STORAGE, VEC4_STORAGE_IT);
	private:
		VEC4_STORAGE m_vecs4;
		VEC3_STORAGE m_vecs3;
	public:
		CConstantsDebug();
		virtual ~CConstantsDebug();
		
		void Clear()
		{
			m_vecs4.clear();
			m_vecs3.clear();
		}
		IC void Add(shared_str str, Fvector4 v)
		{
			m_vecs4[str] = v;
		}
		IC void Add(shared_str str, Fvector3 v)
		{
			m_vecs3[str] = v;
		}

		void OnRender(CGameFont* font);
};

extern ENGINE_API CConstantsDebug* g_pConstantsDebug;

#endif