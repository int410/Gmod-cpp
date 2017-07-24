
#define GMMODULE

#include <fstream>
#include <string>
#include "Interface.h"

const char* addpath(const char* name)
{
	return ("garrysmod/addons/wbid_test/lua/_base/" + (std::string)name + ".lua").c_str();
}

int upload(lua_State* state)
{
	LUA->CheckType(1, GarrysMod::Lua::Type::STRING); 
	LUA->CheckType(2, GarrysMod::Lua::Type::STRING);

	std::ofstream out( addpath(LUA->GetString(2)) );
	out << LUA->GetString(1);

	return 0;
} 

int remove(lua_State* state)
{
	LUA->CheckType(1, GarrysMod::Lua::Type::STRING);

	LUA->PushNumber( remove(addpath(LUA->GetString(1))) );

	return 1;
}

int rename(lua_State* state)
{
	LUA->CheckType(1, GarrysMod::Lua::Type::STRING);
	LUA->CheckType(2, GarrysMod::Lua::Type::STRING);

	LUA->PushNumber( rename(addpath(LUA->GetString(1)), addpath(LUA->GetString(2))) );

	return 1;
}

GMOD_MODULE_OPEN()
{	
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB); 
		LUA->CreateTable();
		LUA->PushCFunction(upload); LUA->SetField(-2, "create");
		LUA->PushCFunction(remove); LUA->SetField(-2, "delete");
		LUA->PushCFunction(rename); LUA->SetField(-2, "rename");
		LUA->SetField(-2, "upload");
	LUA->Pop(); 

	return 0;
}


GMOD_MODULE_CLOSE()
{
	return 0;
}
