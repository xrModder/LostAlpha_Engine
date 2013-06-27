//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "UI_ParticleTools.h"
#include "UI_ParticleMain.h"
#include "../xrEProps/TextForm.h"
#include "../xrEProps/ItemList.h"

void CParticleTools::OnParticleItemRename(LPCSTR old_name, LPCSTR new_name, EItemType type)
{              
	Rename			(old_name,new_name);
	Modified		();
}

void CParticleTools::OnParticleItemRemove(LPCSTR name, EItemType type, bool& res)
{
	Remove			(name);
    Modified		();
    res				= true;
}

void  CParticleTools::OnControlClick(ButtonValue* sender, bool& bDataModified, bool& bSafe)
{
	m_Transform.identity();
    bDataModified	= false;
}

void  CParticleTools::OnFileClick(ButtonValue* sender, bool& bDataModified, bool& bSafe)
{
    switch (sender->btn_num){
	case 0: // import
	{
		xr_string fn;
		if (EFS.GetOpenName("$sparticles$",fn))
		{
			LPCSTR ext	= strext(fn.c_str());
			if (0==stricmp(ext,".pe"))
			{
				PS::CPEDef* PE=xr_new<PS::CPEDef>();

				IReader* R = FS.r_open(fn.c_str());
				PE->Load(*R);
				FS.r_close(R);

				PTools->AppendPE(PE);
				PTools->Modified();
			} else if (0==stricmp(ext,".pg")) {
				PS::CPGDef* PG=xr_new<PS::CPGDef>();

				IReader* R = FS.r_open(fn.c_str());
				PG->Load(*R);
				FS.r_close(R);

				PTools->AppendPG(PG);
				PTools->Modified();
			}

			bDataModified = false;
		} else
			bDataModified = false;
	} break;
	case 1:  // export
	{

		TElTreeItem* pNode = m_PList->GetSelected(); //get current item
		if (pNode&&FHelper.IsObject(pNode))
		{
			AnsiString full_name;
			FHelper.MakeName(pNode,0,full_name,false);

			xr_string fn;

			PS::CPEDef* PE=FindPE(full_name.c_str());
			if (PE)
			{
				if (EFS.GetSaveName("$sparticlespe$",fn))
				{
					IWriter* W = FS.w_open(fn.c_str());
					if (W)
					{
						PE->Save	(*W);
						FS.w_close	(W);
					} else
						Log			("!Can't export particle:",fn.c_str());
				}
			}else{
				PS::CPGDef* PG=FindPG(full_name.c_str());
				if (PG)
				{
					if (EFS.GetSaveName("$sparticlespg$",fn))
					{
						IWriter* W = FS.w_open(fn.c_str());
						if (W)
						{
							PG->Save	(*W);
							FS.w_close	(W);
						} else
							Log			("!Can't export particle:",fn.c_str());
					}
				}
			}

			bDataModified = false;
		}else{
			ELog.DlgMsg(mtInformation, "At first select object.");
		}
	} break;
    }
}

void CParticleTools::OnParticleItemFocused(ListItemsVec& items)
{
	PropItemVec props;
	m_EditMode	= emEffect;

    ButtonValue* B;
	B=PHelper().CreateButton	(props,"Transform\\Edit",	"Reset",	ButtonValue::flFirstOnly);
    B->OnBtnClickEvent.bind		(this,&CParticleTools::OnControlClick);
    PHelper().CreateFlag32		(props,"Transform\\Type",	&m_Flags,	flSetXFORM,"Update","Set");

	B=PHelper().CreateButton	(props,"File\\Edit",	"Add, Export",	ButtonValue::flFirstOnly);
    B->OnBtnClickEvent.bind		(this,&CParticleTools::OnFileClick);
    
    // reset to default
    ResetCurrent	();
    
	if (!items.empty()){
	    for (ListItemsIt it=items.begin(); it!=items.end(); it++){
            ListItem* item = *it;
            if (item){         
                m_EditMode 			= EEditMode(item->Type());
                switch(m_EditMode){
                case emEffect:{
                    PS::CPEDef* def	= ((PS::CPEDef*)item->m_Object);
                	SetCurrentPE	(def);
                    def->FillProp	(EFFECT_PREFIX,props,item);
                }break;
                case emGroup:{
                    PS::CPGDef* def	= ((PS::CPGDef*)item->m_Object);
                	SetCurrentPG	(def);
                    def->FillProp	(GROUP_PREFIX,props,item);
                }break;
                default: THROW;
                }
            }
        }
    }

    //play current
    PlayCurrent();

	m_ItemProps->AssignItems(props);
    UI->RedrawScene();
}
//------------------------------------------------------------------------------

void CParticleTools::RealUpdateProperties()
{
	m_Flags.set(flRefreshProps,FALSE); 

	ListItemsVec items;
    {
/*
		PS::PSIt Ps = ::Render->PSystems.FirstPS();
		PS::PSIt Es = ::Render->PSystems.LastPS();
		for (; Ps!=Es; Ps++){
        	ListItem* I=LHelper.CreateItem(items,Ps->m_Name,emSystem,0,Ps);
            I->SetIcon(0);
		}
*/
	}
    {
        PS::PEDIt Pe = ::Render->PSLibrary.FirstPED();
        PS::PEDIt Ee = ::Render->PSLibrary.LastPED();
        for (; Pe!=Ee; Pe++){
            ListItem* I=LHelper().CreateItem(items,*(*Pe)->m_Name,emEffect,0,*Pe);
            I->SetIcon(1);
        }
	}
    {
        PS::PGDIt Pg = ::Render->PSLibrary.FirstPGD();
        PS::PGDIt Eg = ::Render->PSLibrary.LastPGD();
        for (; Pg!=Eg; Pg++){
            ListItem* I=LHelper().CreateItem(items,*(*Pg)->m_Name,emGroup,0,*Pg);
            I->SetIcon(2);
        }
	}
	m_PList->AssignItems(items,false,true);
}

 