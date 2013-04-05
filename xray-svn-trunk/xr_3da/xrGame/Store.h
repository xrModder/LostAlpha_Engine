#pragma once
#include "object_interfaces.h"
#include "script_export_space.h"
#include "alife_space.h"

class xrTime;
class NET_Packet;

enum TypeOfData 
{
	lua_nil,
	lua_bool,
	lua_string,
	lua_number,
	lua_vector,
	lua_table,
	lua_u32,
	lua_u16,
	lua_u8,
	lua_s32,
	lua_s16,
	lua_s8,
	lua_float,
	lua_ctime
};

namespace luabind 
{
	class object;
};


class CStoreHouse : public IPureSerializeObject<IReader, IWriter>
{
	public:
								CStoreHouse		();
		virtual					~CStoreHouse	();
		virtual void			load			(IReader& stream);
		virtual void			save			(IWriter& stream);
	
	private:
				void			OnSave			();
				void			OnLoad			();
				void			SerializeTable	(luabind::object& tbl);

	public:
		DECLARE_SCRIPT_REGISTER_FUNCTION;

	private:
		struct SStorageHelper
		{
			template <typename T> static void	Write	(NET_Packet *P, shared_str str, T& val);
												   static void*	Read	(NET_Packet *P, shared_str* str);
		};

		NET_Packet*		m_buffer;

};


add_to_type_list(CStoreHouse)
#undef script_type_list
#define script_type_list save_type_list(CStoreHouse)