// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:11:06 , by user : Oles , from computer : OLES
// Tracer.cpp: implementation of the CTracer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tracer.h"
#include "../render.h"

const u32	MAX_TRACERS	= (1024*5);
const float TRACER_SIZE = 0.13f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define TRACERS_COLOR_TABLE "tracers_color_table"
CTracer::CTracer()
{
	sh_Tracer->create("effects\\bullet_tracer","fx\\fx_tracer");
	m_aColors.clear();
	for (u8 i=0; i<255; i++)
	{
		shared_str LineName;
		LineName.printf("color_%d", i);
		if (!pSettings->line_exist(TRACERS_COLOR_TABLE, LineName)) break;
		float r, g, b;
		sscanf(pSettings->r_string(TRACERS_COLOR_TABLE, *LineName),	"%f,%f,%f", &r, &g, &b);		
		m_aColors.push_back(color_argb_f(1.0f, r, g, b));
	};
}

CTracer::~CTracer()
{
}

IC void FillSprite_Circle      (const Fvector& pos, const float r1, const float r2, u32 color)
{
	const Fvector& T        = Device.vCameraTop;
	const Fvector& R        = Device.vCameraRight;
	Fvector Vr, Vt;
	Vr.x            = R.x*r1;
	Vr.y            = R.y*r1;
	Vr.z            = R.z*r1;
	Vt.x            = T.x*r2;
	Vt.y            = T.y*r2;
	Vt.z            = T.z*r2;

	Fvector         a,b,c,d;
	a.sub           (Vt,Vr);
	b.add           (Vt,Vr);
	c.invert        (a);
	d.invert        (b);
	//	TODO: return code back to indexed rendering since we use quads
	//	Tri 1
	UIRender->PushPoint(d.x+pos.x, d.y+pos.y, d.z+pos.z, color, 0.f,1.f);
	UIRender->PushPoint(a.x+pos.x, a.y+pos.y, a.z+pos.z, color, 0.f,0.f);
	UIRender->PushPoint(c.x+pos.x, c.y+pos.y, c.z+pos.z, color, 1.f,1.f);
	//	Tri 2																					
	UIRender->PushPoint(c.x+pos.x, c.y+pos.y, c.z+pos.z, color, 1.f,1.f);
	UIRender->PushPoint(a.x+pos.x, a.y+pos.y, a.z+pos.z, color, 0.f,0.f);
	UIRender->PushPoint(b.x+pos.x, b.y+pos.y, b.z+pos.z, color, 1.f,0.f);


}

IC void FillSprite_Line	(const Fvector& pos, const Fvector& dir, const float r1, const float r2, u32 color)
{
    const Fvector& T        = dir;
    Fvector        R;     
	R.crossproduct(T,Device.vCameraDirection).normalize_safe();
	
    Fvector Vr, Vt;
    Vr.x            = R.x*r1;
    Vr.y            = R.y*r1;
    Vr.z            = R.z*r1;
    Vt.x            = T.x*r2;
    Vt.y            = T.y*r2;
    Vt.z            = T.z*r2;

    Fvector         a,b,c,d;
    a.sub           (Vt,Vr);
    b.add           (Vt,Vr);
    c.invert        (a);
    d.invert        (b);
	//	TODO: return code back to indexed rendering since we use quads
	//	Tri 1
	UIRender->PushPoint(d.x+pos.x, d.y+pos.y, d.z+pos.z, color, 0.f,1.f);
	UIRender->PushPoint(a.x+pos.x, a.y+pos.y, a.z+pos.z, color, 0.f,0.5f);
	UIRender->PushPoint(c.x+pos.x, c.y+pos.y, c.z+pos.z, color, 1.f,1.f);
	//	Tri 2																					
	UIRender->PushPoint(c.x+pos.x, c.y+pos.y, c.z+pos.z, color, 1.f,1.f);
	UIRender->PushPoint(a.x+pos.x, a.y+pos.y, a.z+pos.z, color, 0.f,0.5f);
	UIRender->PushPoint(b.x+pos.x, b.y+pos.y, b.z+pos.z, color,  1.f,0.5f);
}

void  CTracer::Render	(const Fvector& pos, const Fvector& center, const Fvector& dir, float length,  float width, u8 colorID, bool bActor)
{
	if (::Render->ViewBase.testSphere_dirty((Fvector&)center,length*.5f)){
		if (colorID >= m_aColors.size()) colorID = 0;
		FillSprite_Circle	(pos,width*.5f,width*.5f, m_aColors[colorID]);
		FillSprite_Line	(center,dir,width*.5f,length*.5f, m_aColors[colorID]);
	}
}