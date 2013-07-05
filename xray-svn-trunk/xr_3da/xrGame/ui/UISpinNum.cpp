// file:		UISpinNum.cpp
// description:	Spin Button with numerical data (unlike text data)
// created:		15.06.2005
// author:		Serge Vynnychenko
//

#include "StdAfx.h"
#include "UISpinNum.h"
#include "UILines.h"

CUISpinNum::CUISpinNum()
:	m_iVal(0),
	m_iMin(0),
	m_iMax(100),
	m_iStep(1)
{}

void CUISpinNum::SetMaxValueByScript(int MaxValue)
{
	R_ASSERT(m_iMin<MaxValue);
	m_iMax = MaxValue;
}

void CUISpinNum::SetMinValueByScript(int MinValue)
{
	R_ASSERT(m_iMin<MinValue);
	m_iMin = MinValue;
}

int CUISpinNum::GetValueByScript()
{
	return Value();
}
int CUISpinNum::GetStepByScript()
{
	return m_iStep;
}

void CUISpinNum::SetStepByScript(int StepValue)
{
	m_iStep = StepValue;
}

bool CUISpinNum::SetValueByScript(int value)
{
	R_ASSERT	(!((value<=m_iMax) && (value>=m_iMin)));
	
	m_iVal = value;
	SetValue();
	
	return true;
}

void CUISpinNum::SetCurrentOptValue	()	// opt->current
{
	CUIOptionsItem::SetCurrentOptValue();
	GetOptIntegerValue	(m_iVal, m_iMin, m_iMax);
	SetValue			(m_iVal);
}

void CUISpinNum::SaveBackUpOptValue	()	// current->backup
{
	CUIOptionsItem::SaveBackUpOptValue();
	m_opt_backup_value	= m_iVal;
}

void CUISpinNum::SetCurrentValue()
{
	GetOptIntegerValue(m_iVal, m_iMin, m_iMax);
	SetValue();
}

void CUISpinNum::SaveValue()
{
	CUIOptionsItem::SaveValue();
	SaveOptIntegerValue(m_iVal);
}

bool CUISpinNum::IsChanged()
{
	int val, min, max;
	GetOptIntegerValue(val, min, max);
    return m_iVal != val;
}

void CUISpinNum::Init(float x, float y, float width, float height)
{
	CUICustomSpin::Init(x,y,width,height);
	SetValue();
}

void CUISpinNum::IncVal()
{
	if (CanPressUp())
		m_iVal += m_iStep;

	SetValue();
}

void CUISpinNum::DecVal()
{
	if (CanPressDown())
		m_iVal -= m_iStep;

	SetValue();
}

void CUISpinNum::OnBtnUpClick()
{
	IncVal						();
	CUICustomSpin::OnBtnUpClick	();
}

void CUISpinNum::OnBtnDownClick()
{
	DecVal						();
	CUICustomSpin::OnBtnDownClick();
}

void CUISpinNum::SetValue()
{
	string16	buff;
	m_pLines->SetText(itoa(m_iVal, buff, 10)); 
}

bool CUISpinNum::CanPressUp()
{
	return m_iVal + m_iStep <= m_iMax;

}

bool CUISpinNum::CanPressDown()
{
	return m_iVal - m_iStep >= m_iMin;
}

CUISpinFlt::CUISpinFlt()
:	m_fVal(0),
	m_fMin(0),
	m_fMax(100),
	m_fStep(0.1f)
{}

void CUISpinFlt::SaveBackUpOptValue()
{
	CUIOptionsItem::SaveBackUpOptValue();
	m_opt_backup_value	= m_fVal;
}

void CUISpinFlt::UndoOptValue()
{
	m_fVal			= m_opt_backup_value;
	SetValue		(m_fVal);
	CUIOptionsItem::UndoOptValue();
}

void CUISpinFlt::SetCurrentOptValue()
{
	CUIOptionsItem::SetCurrentOptValue();
	GetOptFloatValue	(m_fVal, m_fMin, m_fMax);
	SetValue			(m_fVal);
}

void CUISpinFlt::SaveOptValue()
{
	CUIOptionsItem::SaveOptValue();
	SaveOptFloatValue(m_fVal);
}

bool CUISpinFlt::IsChangedOptValue() const
{
    return !fsimilar		(m_fVal, m_opt_backup_value);
}

void CUISpinFlt::InitSpin(Fvector2 pos, Fvector2 size)
{
	CUICustomSpin::InitSpin	(pos, size);
	SetValue				(m_fVal);
}

void CUISpinFlt::IncVal()
{
	m_fVal		+= m_fStep;
	clamp		(m_fVal, m_fMin, m_fMax);
	SetValue	(m_fVal);
}

void CUISpinFlt::DecVal()
{
	m_fVal		-= m_fStep;
	clamp		(m_fVal, m_fMin, m_fMax);
	SetValue	(m_fVal);
}

void CUISpinFlt::OnBtnUpClick()
{
	IncVal		();

	CUICustomSpin::OnBtnUpClick();
}

void CUISpinFlt::OnBtnDownClick()
{
	DecVal			();

	CUICustomSpin::OnBtnDownClick();
}

void CUISpinFlt::SetValue(float v)
{
	string16				buff;
	xr_sprintf				(buff, "%.1f", v);
	m_pLines->SetText		(buff); 
}

bool CUISpinFlt::CanPressUp()
{
	return m_fVal + m_fStep <= m_fMax;
}

bool CUISpinFlt::CanPressDown()
{
	return (m_fVal - m_fStep > m_fMin) || fsimilar(m_fVal-m_fStep, m_fMin);
}

void CUISpinFlt::SetMaxValueByScript(float MaxValue)
{
	R_ASSERT(MaxValue>m_fMin);
	m_fMax = MaxValue;
}

void CUISpinFlt::SetMinValueByScript(float MinValue)
{
	R_ASSERT(MinValue<m_fMax);
	m_fMin = MinValue;
}

int CUISpinFlt::GetValueByScript()
{
	return m_fVal;
}
int CUISpinFlt::GetStepByScript()
{
	return m_fStep;
}

void CUISpinFlt::SetStepByScript(float StepValue)
{
	m_fStep = StepValue;
}

bool CUISpinFlt::SetValueByScript(float value)
{
	R_ASSERT	(!((value<=m_fMax) && (value>=m_fMin)));
	
	m_fVal = value;
	SetValue();
	
	return true;
}
