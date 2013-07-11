#include "stdafx.h"
#pragma hdrstop

#include "sh_atomic.h"
#include "ResourceManager.h"

// Atomic

//	SVS
SVS::SVS() :
	vs(0)
{
	;
}

SVS::~SVS()
{
	Device.Resources->_DeleteVS(this);
	_RELEASE(vs);
}

///////////////////////////////////////////////////////////////////////
//	SPS
SPS::~SPS								()			{	_RELEASE(ps);		Device.Resources->_DeletePS			(this);	}

///////////////////////////////////////////////////////////////////////
//	SState
SState::~SState							()			{	_RELEASE(state);	Device.Resources->_DeleteState		(this);	}

///////////////////////////////////////////////////////////////////////
//	SDeclaration
SDeclaration::~SDeclaration()
{	
	Device.Resources->_DeleteDecl(this);	
}
