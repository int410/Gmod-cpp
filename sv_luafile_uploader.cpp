
#define GMMODULE

#include <fstream>
#include <string>

#include "Interface.h"

#define CLua( _function ) int _function( lua_State* state )
#define PushCLua( _function, _name ) LUA->PushCFunction(_function); LUA->SetField(-2, _name);

const char* addpath(const char* name)
{
	return ("garrysmod/addons/wbid_test/lua/_base/" + (std::string)name + ".lua").c_str();
}

CLua (f_upload)
{
	LUA->CheckType(1, GarrysMod::Lua::Type::STRING); 
	LUA->CheckType(2, GarrysMod::Lua::Type::STRING);

	std::ofstream out( addpath(LUA->GetString(2)) );
	out << LUA->GetString(1);
	out.close();

	return 0;
} 

CLua (f_remove)
{
	LUA->CheckType(1, GarrysMod::Lua::Type::STRING);

	LUA->PushNumber( std::remove(addpath(LUA->GetString(1))) );

	return 1;
}

CLua (f_rename)
{
	LUA->CheckType(1, GarrysMod::Lua::Type::STRING);
	LUA->CheckType(2, GarrysMod::Lua::Type::STRING);

	LUA->PushNumber( std::rename(addpath(LUA->GetString(1)), addpath(LUA->GetString(2))) );

	return 1;
}


GMOD_MODULE_OPEN()
{	
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB); 

		LUA->CreateTable();
			PushCLua(f_upload, "create");
			PushCLua(f_remove, "delete");
			PushCLua(f_rename, "rename");
		LUA->SetField(-2, "upload");

	LUA->Pop(); 

	return 0;
}


GMOD_MODULE_CLOSE()
{
	return 0;
}
