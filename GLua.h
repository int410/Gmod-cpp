////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////    GLUA HEADER BY 410      ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <Windows.h>
#include <string>

#define lua_pop(L,n) lua_settop(L, -(n)-1) 
#define RUNSTRINGEX 111 // hook all lua
#define RUNSTRING 92 // hook only LuaCmd
#define FINDANDRUN 97 // hook openfile
#define INIT 55 // post state created

class ILuaShared;
class IThreadedCall;
class ILuaGameCallback;
class ILuaObject;
class lua_Debug;
class CCommand;
class ILuaBase;

namespace Bootil
{
	class Buffer;
}

struct UserData
{
	void*         data;
	unsigned char type;
};

struct lua_State
{
	unsigned char _ignore_this_common_lua_header_[69];
	ILuaBase* luabase;
};

#ifndef Color
class Color;
#endif

// ------------------------------------ VT FUNCS ----------------------------------- //

//typedef void * (*lua_Alloc) (void *ud, void *ptr, size_t osize, size_t nsize);   --------   уже не надо
//typedef lua_State* (*luanewstate)(lua_Alloc f, void *ud);   --------   уже не надо
typedef int (*CFunc)(lua_State* L);
typedef void* (__thiscall *RunStringEx)(void*, char const*, char const*, char const*, bool, bool, bool, bool);
typedef void* (__thiscall *RunString)(void*, char const*, char const*, char const*, bool, bool);
typedef void* (__thiscall *CreateLuaInterface)(void*, unsigned char, bool);
typedef void* (__thiscall *CloseLuaInterface)(void*, void*);
typedef void* (__thiscall *Init)(void*, ILuaGameCallback*, bool);
typedef void* (__thiscall *FindAndRunScript)(void*, const char* filename, bool run, bool showErrors, const char* a, bool b);

// --------------------------------------------------------------------------------- //



// ------------------------------------ LUA_ FUNCS --------------------------------- //

HMODULE lua_shared = GetModuleHandleA("lua_shared.dll");

auto luaL_loadbuffer = (int(*)(lua_State *state, const char *buff, size_t sz, const char* name))GetProcAddress(lua_shared, "luaL_loadbuffer");
auto lua_pcall = (int(*)(lua_State* L, int args, int results, int errfunc))GetProcAddress(lua_shared, "lua_pcall");
auto lua_settop = (int(*)(lua_State* L, int a))GetProcAddress(lua_shared, "lua_settop");
auto lua_tolstring = (const char*(*)(lua_State* L, int a, int len))GetProcAddress(lua_shared, "lua_tolstring");
auto luaL_loadfile = (int(*)(lua_State* L, const char* filename))GetProcAddress(lua_shared, "luaL_loadfile");

// --------------------------------------------------------------------------------- //

class ILuaBase
{
public:
	virtual int         Top(void) = 0;

	// Pushes a copy of the value at iStackPos to the top of the stack
	virtual void        Push(int iStackPos) = 0;

	// Pops iAmt values from the top of the stack
	virtual void        Pop(int iAmt = 1) = 0;

	// Pushes table[key] on to the stack
	// table = value at iStackPos
	// key   = value at top of the stack
	virtual void        GetTable(int iStackPos) = 0;

	// Pushes table[key] on to the stack
	// table = value at iStackPos
	// key   = strName
	virtual void        GetField(int iStackPos, const char* strName) = 0;

	// Sets table[key] to the value at the top of the stack
	// table = value at iStackPos
	// key   = strName
	// Pops the value from the stack
	virtual void        SetField(int iStackPos, const char* strName) = 0;

	// Creates a new table and pushes it to the top of the stack
	virtual void        CreateTable() = 0;

	// Sets table[key] to the value at the top of the stack
	// table = value at iStackPos
	// key   = value 2nd to the top of the stack
	// Pops the key and the value from the stack
	virtual void        SetTable(int iStackPos) = 0;

	// Sets the metatable for the value at iStackPos to the value at the top of the stack
	// Pops the value off of the top of the stack
	virtual void        SetMetaTable(int iStackPos) = 0;

	// Pushes the metatable of the value at iStackPos on to the top of the stack
	// Upon failure, returns false and does not push anything
	virtual bool        GetMetaTable(int i) = 0;

	// Calls a function
	// To use it: Push the function on to the stack followed by each argument
	// Pops the function and arguments from the stack, leaves iResults values on the stack
	// If this function errors, any local C values will not have their destructors called!
	virtual void        Call(int iArgs, int iResults) = 0;

	// Similar to Call
	// See: lua_pcall( lua_State*, int, int, int )
	virtual int         PCall(int iArgs, int iResults, int iErrorFunc) = 0;

	// Returns true if the values at iA and iB are equal
	virtual int         Equal(int iA, int iB) = 0;

	// Returns true if the value at iA and iB are equal
	// Does not invoke metamethods
	virtual int         RawEqual(int iA, int iB) = 0;

	// Moves the value at the top of the stack in to iStackPos
	// Any elements above iStackPos are shifted upwards
	virtual void        Insert(int iStackPos) = 0;

	// Removes the value at iStackPos from the stack
	// Any elements above iStackPos are shifted downwards
	virtual void        Remove(int iStackPos) = 0;

	// Allows you to iterate tables similar to pairs(...)
	// See: lua_next( lua_State*, int );
	virtual int         Next(int iStackPos) = 0;

#ifdef GMOD_ALLOW_DEPRECATED
	// Deprecated: Use the UserType functions instead of this
	virtual void*       NewUserdata(unsigned int iSize) = 0;
#else
protected:
	virtual UserData*   NewUserdata(unsigned int iSize) = 0;
#endif

public:
	// Throws an error and ceases execution of the function
	// If this function is called, any local C values will not have their destructors called!
	virtual void        ThrowError(const char* strError) = 0;

	// Checks that the type of the value at iStackPos is iType
	// Throws and error and ceases execution of the function otherwise
	// If this function errors, any local C values will not have their destructors called!
	virtual void        CheckType(int iStackPos, int iType) = 0;

	// Throws a pretty error message about the given argument
	// If this function is called, any local C values will not have their destructors called!
	virtual void        ArgError(int iArgNum, const char* strMessage) = 0;

	// Pushes table[key] on to the stack
	// table = value at iStackPos
	// key   = value at top of the stack
	// Does not invoke metamethods
	virtual void        RawGet(int iStackPos) = 0;

	// Sets table[key] to the value at the top of the stack
	// table = value at iStackPos
	// key   = value 2nd to the top of the stack
	// Pops the key and the value from the stack
	// Does not invoke metamethods
	virtual void        RawSet(int iStackPos) = 0;

	// Returns the string at iStackPos. iOutLen is set to the length of the string if it is not NULL
	// If the value at iStackPos is a number, it will be converted in to a string
	// Returns NULL upon failure
	virtual const char* GetString(int iStackPos = -1, unsigned int* iOutLen = NULL) = 0;

	// Returns the number at iStackPos
	// Returns 0 upon failure
	virtual double      GetNumber(int iStackPos = -1) = 0;

	// Returns the boolean at iStackPos
	// Returns false upon failure
	virtual bool        GetBool(int iStackPos = -1) = 0;

	// Returns the C-Function at iStackPos
	// returns NULL upon failure
	virtual CFunc       GetCFunction(int iStackPos = -1) = 0;

	// You should probably be using the UserType functions instead of this
#ifdef GMOD_ALLOW_DEPRECATED
	virtual void*       GetUserdata(int iStackPos = -1) = 0;
#else
	virtual UserData*   GetUserdata(int iStackPos = -1) = 0;
#endif

	// Pushes a nil value on to the stack
	virtual void        PushNil() = 0;

	// Pushes the given string on to the stack
	// If iLen is 0, strlen will be used to determine the string's length
	virtual void        PushString(const char* val, unsigned int iLen = 0) = 0;

	// Pushes the given double on to the stack
	virtual void        PushNumber(double val) = 0;

	// Pushes the given bobolean on to the stack
	virtual void        PushBool(bool val) = 0;

	// Pushes the given C-Function on to the stack
	virtual void        PushCFunction(CFunc val) = 0;

	// Pushes the given C-Function on to the stack with upvalues
	// See: GetUpvalueIndex()
	virtual void        PushCClosure(CFunc val, int iVars) = 0;

	// Pushes the given pointer on to the stack as light-userdata
	virtual void        PushUserdata(void*) = 0;

	// Allows for values to be stored by reference for later use
	// Make sure you call ReferenceFree when you are done with a reference
	virtual int         ReferenceCreate() = 0;
	virtual void        ReferenceFree(int i) = 0;
	virtual void        ReferencePush(int i) = 0;

	// Push a special value onto the top of the stack (see SPECIAL_* enums)
	virtual void        PushSpecial(int iType) = 0;

	// Returns true if the value at iStackPos is of type iType
	// See: Types.h
	virtual bool        IsType(int iStackPos, int iType) = 0;

	// Returns the type of the value at iStackPos
	// See: Types.h
	virtual int         GetType(int iStackPos) = 0;

	// Returns the name associated with the given type ID
	// See: Types.h
	// Note: GetTypeName does not work with user-created types
	virtual const char* GetTypeName(int iType) = 0;

#ifndef GMOD_ALLOW_DEPRECATED
private:
#endif
	// Deprecated: Use CreateMetaTable
	virtual void        CreateMetaTableType(const char* strName, int iType) = 0;

public:
	// Like Get* but throws errors and returns if they're not of the expected type
	// If these functions error, any local C values will not have their destructors called!
	virtual const char* CheckString(int iStackPos = -1) = 0;
	virtual double      CheckNumber(int iStackPos = -1) = 0;

	// Returns the length of the object at iStackPos
	// Works for: strings, tables, userdata
	virtual int         ObjLen(int iStackPos = -1) = 0;

	// Returns the angle at iStackPos
	virtual const QAngle& GetAngle(int iStackPos = -1) = 0;

	// Returns the vector at iStackPos
	virtual const Vector& GetVector(int iStackPos = -1) = 0;

	// Pushes the given angle to the top of the stack
	virtual void        PushAngle(const QAngle& val) = 0;

	// Pushes the given vector to the top of the stack
	virtual void        PushVector(const Vector& val) = 0;

	// Sets the lua_State to be used by the ILuaBase implementation
	// You don't need to use this if you use the LUA_FUNCTION macro
	virtual void        SetState(lua_State *L) = 0;

	// Pushes the metatable associated with the given type name
	// Returns the type ID to use for this type
	// If the type doesn't currently exist, it will be created
	virtual int         CreateMetaTable(const char* strName) = 0;

	// Pushes the metatable associated with the given type
	virtual bool        PushMetaTable(int iType) = 0;

	// Created a new UserData of type iType that references the given data
	virtual void        PushUserType(void* data, int iType) = 0;

	// Sets the data pointer of the UserType at iStackPos
	// You can use this to invalidate a UserType by passing NULL
	virtual void        SetUserType(int iStackPos, void* data) = 0;

};

class ILuaInterface : public ILuaBase
{
public:
	virtual bool Init(ILuaGameCallback *, bool) = 0;
	virtual void Shutdown() = 0;
	virtual void Cycle() = 0;
	virtual ILuaObject *Global() = 0;
	virtual ILuaObject *GetObject(int index) = 0;
	virtual void PushLuaObject(ILuaObject *obj) = 0;
	virtual void PushLuaFunction(CFunc func) = 0;
	virtual void LuaError(const char *err, int index) = 0;
	virtual void TypeError(const char *name, int index) = 0;
	virtual void CallInternal(int args, int rets) = 0;
	virtual void CallInternalNoReturns(int args) = 0;
	virtual bool CallInternalGetBool(int args) = 0;
	virtual const char *CallInternalGetString(int args) = 0;
	virtual bool CallInternalGet(int args, ILuaObject *obj) = 0;
	virtual void NewGlobalTable(const char *name) = 0;
	virtual ILuaObject *NewTemporaryObject() = 0;
	virtual bool isUserData(int index) = 0;
	virtual ILuaObject *GetMetaTableObject(const char *name, int type) = 0;
	virtual ILuaObject *GetMetaTableObject(int index) = 0;
	virtual ILuaObject *GetReturn(int index) = 0;
	virtual bool IsServer() = 0;
	virtual bool IsClient() = 0;
	virtual bool IsMenu() = 0;
	virtual void DestroyObject(ILuaObject *obj) = 0;
	virtual ILuaObject *CreateObject() = 0;
	virtual void SetMember(ILuaObject *table, ILuaObject *key, ILuaObject *value) = 0;
	virtual void GetNewTable() = 0;
	virtual void SetMember(ILuaObject *table, float key) = 0;
	virtual void SetMember(ILuaObject *table, float key, ILuaObject *value) = 0;
	virtual void SetMember(ILuaObject *table, const char *key) = 0;
	virtual void SetMember(ILuaObject *table, const char *key, ILuaObject *value) = 0;
	virtual void SetType(unsigned char) = 0;
	virtual void PushLong(long num) = 0;
	virtual int GetFlags(int index) = 0;
	virtual bool FindOnObjectsMetaTable(int objIndex, int keyIndex) = 0;
	virtual bool FindObjectOnTable(int tableIndex, int keyIndex) = 0;
	virtual void SetMemberFast(ILuaObject *table, int keyIndex, int valueIndex) = 0;
	virtual bool RunString(const char *filename, const char *path, const char *stringToRun, bool run, bool showErrors) = 0;
	virtual bool IsEqual(ILuaObject *objA, ILuaObject *objB) = 0;
	virtual void Error(const char *err) = 0;
	virtual const char *GetStringOrError(int index) = 0;
	virtual bool RunLuaModule(const char *name) = 0;
	virtual bool FindAndRunScript(const char *filename, bool run, bool showErrors, const char *, bool) = 0;
	virtual void SetPathID(const char *pathID) = 0;
	virtual const char *GetPathID() = 0;
	virtual void ErrorNoHalt(const char *fmt, ...) = 0;
	virtual void Msg(const char *fmt, ...) = 0;
	virtual void PushPath(const char *path) = 0;
	virtual void PopPath() = 0;
	virtual const char *GetPath() = 0;
	virtual int GetColor(int index) = 0;
	virtual void PushColor(Color color) = 0;
	virtual int GetStack(int level, lua_Debug *dbg) = 0;
	virtual int GetInfo(const char *what, lua_Debug *dbg) = 0;
	virtual const char *GetLocal(lua_Debug *dbg, int n) = 0;
	virtual const char *GetUpvalue(int funcIndex, int n) = 0;
	virtual bool RunStringEx(const char *filename, const char *path, const char *stringToRun, bool run, bool printErrors, bool dontPushErrors, bool noReturns) = 0;
	virtual size_t GetDataString(int index, const char **str) = 0;
	virtual void ErrorFromLua(const char *fmt, ...) = 0;
	virtual void *GetCurrentLocation() = 0;
	virtual void MsgColour(const Color &col, const char *fmt, ...) = 0;
	virtual void GetCurrentFile(std::string &outStr) = 0;
	virtual void CompileString(Bootil::Buffer &dumper, const std::string &stringToCompile) = 0;
	virtual bool CallFunctionProtected(int, int, bool) = 0;
	virtual void Require(const char *name) = 0;
	virtual const char *GetActualTypeName(int type) = 0;
	virtual void PreCreateTable(int arrelems, int nonarrelems) = 0;
	virtual void PushPooledString(int index) = 0;
	virtual const char *GetPooledString(int index) = 0;
	virtual void *AddThreadedCall(IThreadedCall *) = 0;
	virtual void *CreateConVar(const char *, const char *, const char *, int) = 0;
	virtual void *CreateConCommand(const char *, const char *, int, void(*)(const CCommand &), int(*)(const char *, char(*)[128])) = 0;
public:
	lua_State *state;
};

enum
{
	SPECIAL_GLOB,		// Global table
	SPECIAL_ENV,		// Environment table
	SPECIAL_REG,		// Registry table
};

namespace GMTYPE
{
	enum
	{

		INVALID = -1,
		NIL,
		BOOL,
		LIGHTUSERDATA,
		NUMBER,
		STRING,
		TABLE,
		FUNCTION,
		USERDATA,
		THREAD,

		// UserData
		ENTITY,
		VECTOR,
		ANGLE,
		PHYSOBJ,
		SAVE,
		RESTORE,
		DAMAGEINFO,
		EFFECTDATA,
		MOVEDATA,
		RECIPIENTFILTER,
		USERCMD,
		SCRIPTEDVEHICLE,

		// Client Only
		MATERIAL,
		PANEL,
		PARTICLE,
		PARTICLEEMITTER,
		TEXTURE,
		USERMSG,

		CONVAR,
		IMESH,
		MATRIX,
		SOUND,
		PIXELVISHANDLE,
		DLIGHT,
		VIDEO,
		FILE,

		COUNT
	};

	static const char* Name[] =
	{
		"nil",
		"bool",
		"lightuserdata",
		"number",
		"string",
		"table",
		"function",
		"userdata",
		"thread",
		"entity",
		"vector",
		"angle",
		"physobj",
		"save",
		"restore",
		"damageinfo",
		"effectdata",
		"movedata",
		"recipientfilter",
		"usercmd",
		"vehicle",
		"material",
		"panel",
		"particle",
		"particleemitter",
		"texture",
		"usermsg",
		"convar",
		"mesh",
		"matrix",
		"sound",
		"pixelvishandle",
		"dlight",
		"video",
		"file",

		0
	};
}

#ifdef _WIN32
#define  DLL_EXPORT extern "C" __declspec( dllexport )
#else
#define DLL_EXPORT	extern "C" __attribute__((visibility("default")))	
#endif

#define GMOD_MODULE_OPEN()	DLL_EXPORT int gmod13_open( lua_State* state )
#define GMOD_MODULE_CLOSE()	DLL_EXPORT int gmod13_close( lua_State* state )

#define LUA state->luabase
