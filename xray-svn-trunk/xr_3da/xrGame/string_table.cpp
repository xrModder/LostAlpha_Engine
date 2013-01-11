#include "stdafx.h"
#include "string_table.h"

#include "ui/xrUIXmlParser.h"
#include "xr_level_controller.h"
#include "MainMenu.h"

STRING_TABLE_DATA* CStringTable::pData = NULL;
BOOL CStringTable::m_bWriteErrorsToLog = FALSE;

extern shared_str			g_language;

CStringTable::CStringTable	()
{
	Init();
}

void CStringTable::Destroy	()
{
	xr_delete(pData);
}


void CStringTable::Init		()
{
	if(NULL != pData) return;

	pData				= xr_new<STRING_TABLE_DATA>();

	R_ASSERT2	(pSettings->section_exist("languages"),"Section [languages] is not exist!");
	R_ASSERT2	(pSettings->line_count("languages"),"Can't find languages in the section [languages]!");


	if (*g_language && pSettings->line_exist("languages",*g_language))
		pData->m_sLanguage	= g_language;
	else {
		LPCSTR name;
		LPCSTR value;
		pSettings->r_line("languages", 0, &name, &value);

		pData->m_sLanguage	= g_language =	name; 
	}

	LPCSTR S			= pSettings->r_string("string_table", "files");
	if (S && S[0]) 
	{
		string128	xml_file;
		int			count = _GetItemCount	(S);
		for (int it=0; it<count; ++it)	
		{
			_GetItem	(S,it, xml_file);
			Load		(xml_file);
		}
	}
}

void CStringTable::Load	(LPCSTR xml_file)
{
	CUIXml						uiXml;
	string_path					xml_file_full;
	strconcat					(sizeof(xml_file_full),xml_file_full, xml_file, ".xml");
	string_path					_s;
	strconcat					(sizeof(_s),_s, STRING_TABLE_PATH, "\\", *(pData->m_sLanguage) );

	bool xml_result				= uiXml.Init(CONFIG_PATH, _s, xml_file_full);
	if(!xml_result)
		Debug.fatal(DEBUG_INFO,"string table xml file not found %s, for language %s", xml_file_full, *(pData->m_sLanguage));

	//����� ������ ���� ������� ������� � �����
	int string_num = uiXml.GetNodesNum		(uiXml.GetRoot(), "string");

	for(int i=0; i<string_num; ++i)
	{
		LPCSTR string_name = uiXml.ReadAttrib(uiXml.GetRoot(), "string", i, "id", NULL);

		VERIFY3					(pData->m_StringTable.find(string_name) == pData->m_StringTable.end(), "duplicate string table id", string_name);

		LPCSTR string_text		= uiXml.Read(uiXml.GetRoot(), "string:text", i,  NULL);

		if(m_bWriteErrorsToLog && string_text)
			Msg("[string table] '%s' no translation in '%s'", string_name, *(pData->m_sLanguage));
		
		VERIFY3						(string_text, "string table entry does not has a text", string_name);
		
		STRING_VALUE str_val		= ParseLine(string_text, string_name, true);
		
		pData->m_StringTable[string_name] = str_val;
	}
}
void CStringTable::ReparseKeyBindings()
{
	if(!pData)					return;
	STRING_TABLE_MAP_IT it		= pData->m_string_key_binding.begin();
	STRING_TABLE_MAP_IT it_e	= pData->m_string_key_binding.end();

	for(;it!=it_e;++it)
	{
		pData->m_StringTable[it->first]			= ParseLine(*it->second, *it->first, false);
	}
}

void CStringTable::ReloadLanguage()
{
	if (0==xr_strcmp(g_language.c_str(), *(pData->m_sLanguage)))	return;

	//destroy
	xr_delete(pData);

	//init
	Init();

	//reload language in menu
	if (MainMenu()->IsActive())
	{
		MainMenu()->Activate(FALSE);
		MainMenu()->Activate(TRUE);
	}
}


STRING_VALUE CStringTable::ParseLine(LPCSTR str, LPCSTR skey, bool bFirst)
{
//	LPCSTR str = "1 $$action_left$$ 2 $$action_right$$ 3 $$action_left$$ 4";
	xr_string			res;
	int k = 0;
	const char*			b;
	#define ACTION_STR "$$ACTION_"

//.	int LEN				= (int)xr_strlen(ACTION_STR);
	#define LEN			9

	string256				buff;
	string256				srcbuff;
	bool	b_hit			= false;

	while( (b = strstr( str+k,ACTION_STR)) !=0 )
	{
		buff[0]				= 0;
		srcbuff[0]			= 0;
		res.append			(str+k, b-str-k);
		const char* e		= strstr( b+LEN,"$$" );

		int len				= (int)(e-b-LEN);

		strncpy				(srcbuff,b+LEN, len);
		srcbuff[len]		= 0;
		GetActionAllBinding	(srcbuff, buff, sizeof(buff) );
		res.append			(buff, xr_strlen(buff) );

		k					= (int)(b-str);
		k					+= len;
		k					+= LEN;
		k					+= 2;
		b_hit				= true;
	};

	if(k<(int)xr_strlen(str)){
		res.append(str+k);
	}

	if(b_hit&&bFirst) pData->m_string_key_binding[skey] = str;

	return STRING_VALUE(res.c_str());
}

STRING_VALUE CStringTable::translate (const STRING_ID& str_id) const
{
	VERIFY					(pData);

	STRING_VALUE res =  pData->m_StringTable[str_id];

	if(!res)
	{
		if(m_bWriteErrorsToLog && *str_id != NULL && xr_strlen(*str_id)>0)
			Msg("[string table] '%s' has no entry", *str_id);
		return str_id;
	}
	return					pData->m_StringTable[str_id];
}