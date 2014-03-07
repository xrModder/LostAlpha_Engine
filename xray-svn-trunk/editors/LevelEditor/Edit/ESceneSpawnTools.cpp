#include "stdafx.h"
#pragma hdrstop

#include "ESceneSpawnTools.h"
#include "ui_leveltools.h"
#include "ESceneSpawnControls.h"
#include "FrameSpawn.h"
#include "Scene.h"
#include "SceneObject.h"
#include "spawnpoint.h"
#include "builder.h"
#include "levelgamedef.h"
#include "EditLibrary.h"

static HMODULE hXRSE_FACTORY = 0;
static LPCSTR xrse_factory_library	= "xrSE_Factory.dll";
static LPCSTR create_entity_func 	= "_create_entity@4"; 
static LPCSTR destroy_entity_func 	= "_destroy_entity@4";
Tcreate_entity 	create_entity;
Tdestroy_entity destroy_entity;

CEditableObject* ESceneSpawnTools::get_draw_visual(u8 _RP_TeamID, u8 _RP_Type, const u8& _GameType)
{
	CEditableObject* ret = NULL;
	if(m_draw_RP_visuals.empty())
    {
        m_draw_RP_visuals.push_back(Lib.CreateEditObject("editor\\artefakt_ah"));     		//0
        m_draw_RP_visuals.push_back(Lib.CreateEditObject("editor\\artefakt_cta_blue"));     //1
        m_draw_RP_visuals.push_back(Lib.CreateEditObject("editor\\artefakt_cta_green"));    //2
        m_draw_RP_visuals.push_back(Lib.CreateEditObject("editor\\telo_ah_cta_blue"));      //3
        m_draw_RP_visuals.push_back(Lib.CreateEditObject("editor\\telo_ah_cta_green"));     //4
        m_draw_RP_visuals.push_back(Lib.CreateEditObject("editor\\telo_dm"));               //5
        m_draw_RP_visuals.push_back(Lib.CreateEditObject("editor\\telo_tdm_blue"));         //6
        m_draw_RP_visuals.push_back(Lib.CreateEditObject("editor\\telo_tdm_green"));        //7
        m_draw_RP_visuals.push_back(Lib.CreateEditObject("editor\\spectator"));        		//8
        m_draw_RP_visuals.push_back(Lib.CreateEditObject("editor\\item_spawn"));       		//9
    }
    switch (_RP_Type)
    {
    	case rptActorSpawn: //actor spawn
        {
	    if(_GameType == rpgtGameDeathmatch)
            {
            	if(_RP_TeamID==0)
            		ret = m_draw_RP_visuals[5];
            };
	    if(_GameType == rpgtGameTeamDeathmatch)
            {
            	if(_RP_TeamID==2)
            		ret = m_draw_RP_visuals[7];
                else
            	if(_RP_TeamID==1)
            		ret = m_draw_RP_visuals[6];
            };

	   if(_GameType == rpgtGameArtefactHunt)
            {
            	if(_RP_TeamID==0)
            		ret = m_draw_RP_visuals[8]; //spactator
                else
            	if(_RP_TeamID==1)
            		ret = m_draw_RP_visuals[4];
                else
            	if(_RP_TeamID==2)
            		ret = m_draw_RP_visuals[3];
            };
            break;
        }
    	case rptArtefactSpawn: //AF spawn
        {
		
	    if(_GameType == rpgtGameArtefactHunt)
            {
           	ret = m_draw_RP_visuals[0];
            }
            break;
        }

    }
    return ret;
}

void __stdcall  FillSpawnItems	(ChooseItemVec& lst, void* param)
{
	LPCSTR gcs					= (LPCSTR)param;
    ObjectList objects;
    Scene->GetQueryObjects		(objects,OBJCLASS_SPAWNPOINT,-1,-1,-1);
    
    xr_string itm;
    int cnt 					= _GetItemCount(gcs);
    for (int k=0; k<cnt; k++){
        _GetItem				(gcs,k,itm);
        for (ObjectIt it=objects.begin(); it!=objects.end(); it++)
            if ((*it)->OnChooseQuery(itm.c_str()))	lst.push_back(SChooseItem((*it)->Name,""));
    }
}

ESceneSpawnTools::ESceneSpawnTools	():ESceneCustomOTools(OBJCLASS_SPAWNPOINT)
{
	m_Flags.zero();
    TfrmChoseItem::AppendEvents	(smSpawnItem,		"Select Spawn Item",		FillSpawnItems,		0,0,0,0);

    hXRSE_FACTORY	= LoadLibrary(xrse_factory_library);									VERIFY3(hXRSE_FACTORY,"Can't load library:",xrse_factory_library);
    create_entity 	= (Tcreate_entity)	GetProcAddress(hXRSE_FACTORY,create_entity_func);  	VERIFY3(create_entity,"Can't find func:",create_entity_func);
    destroy_entity 	= (Tdestroy_entity)	GetProcAddress(hXRSE_FACTORY,destroy_entity_func);	VERIFY3(destroy_entity,"Can't find func:",destroy_entity_func);

    m_Classes.clear			();
    CInifile::Root const& data 	= pSettings->sections();
    for (CInifile::RootCIt it=data.begin(); it!=data.end(); it++){
    	LPCSTR val;
    	if ((*it)->line_exist	("$spawn",&val)){
        	CLASS_ID cls_id	= pSettings->r_clsid((*it)->Name,"class");
        	shared_str v	= pSettings->r_string_wb((*it)->Name,"$spawn");
        	m_Classes[cls_id].push_back(SChooseItem(*v,*(*it)->Name));
        }
    }
    
}

ESceneSpawnTools::~ESceneSpawnTools()
{
	FreeLibrary		(hXRSE_FACTORY);
    m_Icons.clear	();

	xr_vector<CEditableObject*>::iterator it 	= m_draw_RP_visuals.begin();
	xr_vector<CEditableObject*>::iterator it_e 	= m_draw_RP_visuals.end();
    for(;it!=it_e;++it)
    {
		Lib.RemoveEditObject(*it);
    }
    m_draw_RP_visuals.clear();
}

void ESceneSpawnTools::CreateControls()
{
	inherited::CreateDefaultControls(estDefault);
    AddControl		(xr_new<TUI_ControlSpawnAdd>(estDefault,etaAdd,		this));
	// frame
    pFrame 			= xr_new<TfraSpawn>((TComponent*)0);
}
//----------------------------------------------------
 
void ESceneSpawnTools::RemoveControls()
{
	inherited::RemoveControls();
}
//----------------------------------------------------

void ESceneSpawnTools::FillProp(LPCSTR pref, PropItemVec& items)
{            
//.	PHelper().CreateFlag32(items, PrepareKey(pref,"Common\\Show Spawn Type"),	&m_Flags,		flShowSpawnType);
//.	PHelper().CreateFlag32(items, PrepareKey(pref,"Common\\Trace Visibility"),	&m_Flags,		flPickSpawnType);
	inherited::FillProp	(pref, items);
}
//------------------------------------------------------------------------------

ref_shader ESceneSpawnTools::CreateIcon(shared_str name)
{
    ref_shader S;
    if (pSettings->line_exist(name,"$ed_icon")){
	    LPCSTR tex_name = pSettings->r_string(name,"$ed_icon");
    	S.create("editor\\spawn_icon",tex_name);
        m_Icons[name] = S;
    }else{
        S = 0;
    }
    return S;
}

ref_shader ESceneSpawnTools::GetIcon(shared_str name)
{
	ShaderPairIt it = m_Icons.find(name);
	if (it==m_Icons.end())	return CreateIcon(name);
	else					return it->second;
}
//----------------------------------------------------
#include "EShape.h"

CCustomObject* ESceneSpawnTools::CreateObject(LPVOID data, LPCSTR name)
{
	CCustomObject* O	= xr_new<CSpawnPoint>(data,name);
    O->ParentTools		= this;
	if(data && name)
    {

    }
    return O;
}
//----------------------------------------------------

int ESceneSpawnTools::MultiRenameObjects()
{
	int cnt			= 0;
    for (ObjectIt o_it=m_Objects.begin(); o_it!=m_Objects.end(); o_it++){
    	CCustomObject* obj	= *o_it;
    	if (obj->Selected()){
        	string256			pref;
            strconcat			(sizeof(pref),pref,Scene->LevelPrefix().c_str(),"_",obj->RefName());
            string256 			buf;
        	Scene->GenObjectName(obj->ClassID,buf,pref);
            if (obj->Name!=buf){
	            obj->Name		= buf;
                cnt++; 
            }
        }
    }
    return cnt;
}


