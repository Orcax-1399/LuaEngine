#pragma once
#include "imgui/imgui.h"
#include "sol_ImGui.h"

using namespace loader;

namespace LuaData {
    map<string, int> IntVariable;
    map<string, float> FloatVariable;
    map<string, string> StringVariable;
}

#pragma region LuaFun
//存入整数变量
static int Lua_Variable_SaveIntVariable(lua_State* pL) {
    string variableName = (string)lua_tostring(pL, 1);
    int variableValue = (int)lua_tointeger(pL, 2);
    LuaData::IntVariable[variableName] = variableValue;
    return 0;
}
//存入浮点数变量
static int Lua_Variable_SaveFloatVariable(lua_State* pL) {
    string variableName = (string)lua_tostring(pL, 1);
    float variableValue = (float)lua_tonumber(pL, 2);
    LuaData::FloatVariable[variableName] = variableValue;
    return 0;
}
//存入字符串变量
static int Lua_Variable_SaveStringVariable(lua_State* pL) {
    string variableName = (string)lua_tostring(pL, 1);
    string variableValue = (string)lua_tostring(pL, 2);
    LuaData::StringVariable[variableName] = variableValue;
    return 0;
}
//读取整数变量
static int Lua_Variable_ReadIntVariable(lua_State* pL) {
    string variableName = (string)lua_tostring(pL, -1);
    int ret;
    if (LuaData::IntVariable.find(variableName) == LuaData::IntVariable.end())
        ret = 0;
    else
        ret = LuaData::IntVariable[variableName];
    lua_pushinteger(pL, ret);
    return 1;
}
//读取浮点数变量
static int Lua_Variable_ReadFloatVariable(lua_State* pL) {
    string variableName = (string)lua_tostring(pL, -1);
    float ret;
    if (LuaData::FloatVariable.find(variableName) == LuaData::FloatVariable.end())
        ret = 0;
    else
        ret = LuaData::FloatVariable[variableName];
    lua_pushnumber(pL, ret);
    return 1;
}
//读取字符串变量
static int Lua_Variable_ReadStringVariable(lua_State* pL) {
    string variableName = (string)lua_tostring(pL, -1);
    string ret;
    if (LuaData::StringVariable.find(variableName) == LuaData::StringVariable.end())
        ret = "";
    else
        ret = LuaData::StringVariable[variableName];
    lua_pushstring(pL, ret.c_str());
    return 1;
}
//销毁变量
static int Lua_Variable_DestroyVariable(lua_State* pL) {
    string variableTpye = (string)lua_tostring(pL, 1);
    string variableName = (string)lua_tostring(pL, 2);
    if (variableTpye == "Int")
        LuaData::IntVariable.erase(variableName);
    else if (variableTpye == "Float")
        LuaData::FloatVariable.erase(variableName);
    else if (variableTpye == "String")
        LuaData::StringVariable.erase(variableName);
    return 0;
}
//获取随机数
static int Lua_Math_Rander(lua_State* pL) {
    float min = (float)lua_tonumber(pL, 1);
    float max = (float)lua_tonumber(pL, 2);
    lua_pushnumber(pL, utils::GetRandom(min, max));
    return 1;
}
#pragma endregion
#pragma region SystemFun
static int System_Keyboard_CheckKey(lua_State* pL) {
    int key = (int)lua_tointeger(pL, -1);
    lua_pushboolean(pL, Keyboard::CheckKey(key));
    return 1;
}
static int System_Keyboard_CheckDoubleKey(lua_State* pL) {
    int key = (int)lua_tointeger(pL, -1);
    lua_pushboolean(pL, Keyboard::CheckKey(key, 2));
    return 1;
}
static int System_Keyboard_CheckKeyIsPressed(lua_State* pL) {
    int key = (int)lua_tointeger(pL, -1);
    bool ret = GetKeyState(key) < 0;
    if (!Keyboard::CheckWindows())
        ret = false;
    lua_pushboolean(pL, ret);
    return 1;
}
static int System_Chronoscope_AddChronoscope(lua_State* pL) {
    float time = (float)lua_tonumber(pL, 1);
    string name = (string)lua_tostring(pL, 2);
    Chronoscope::AddChronoscope(time, name, true);
    return 0;
}
static int System_Chronoscope_CheckPresenceChronoscope(lua_State* pL) {
    string name = (string)lua_tostring(pL, -1);
    lua_pushboolean(pL, Chronoscope::CheckPresenceChronoscope(name));
    return 1;
}
static int System_Chronoscope_DelChronoscope(lua_State* pL) {
    string name = (string)lua_tostring(pL, -1);
    Chronoscope::DelChronoscope(name);
    return 0;
}
static int System_Chronoscope_CheckChronoscope(lua_State* pL) {
    string name = (string)lua_tostring(pL, -1);
    lua_pushboolean(pL, Chronoscope::CheckChronoscope(name));
    return 1;
}
static int System_Message_ShowMessage(lua_State* pL) {
    string message = (string)lua_tostring(pL, -1);
    MH::Chat::ShowGameMessage(*(undefined**)MH::Chat::MainPtr, (undefined*)&message[0], -1, -1, 0);
    return 0;
}
static int System_Console_Info(lua_State* pL) {
    string message = (string)lua_tostring(pL, -1);
    LOG(INFO) << utils::UTF8_To_string(message);
    return 0;
}
static int System_Console_Error(lua_State* pL) {
    string message = (string)lua_tostring(pL, -1);
    LOG(ERR) << utils::UTF8_To_string(message);
    return 0;
}
static int System_XboxPad_CheckKey(lua_State* pL) {
    int key = (int)lua_tointeger(pL, -1);
    lua_pushboolean(pL, XboxPad::CheckKey(key));
    return 1;
}
static int System_XboxPad_CheckDoubleKey(lua_State* pL) {
    int key = (int)lua_tointeger(pL, -1);
    lua_pushboolean(pL, XboxPad::CheckKey(key, 2));
    return 1;
}
static int System_XboxPad_CheckKeyIsPressed(lua_State* pL) {
    int key = (int)lua_tointeger(pL, -1);
    bool ret = XboxPad::KeyIdHandle(key);
    lua_pushboolean(pL, ret);
    return 1;
}
#pragma endregion
#pragma region MemoryFun
static int System_Memory_GetAddress(lua_State* pL) {
    vector<int> bytes;
    long long ptr = (long long)lua_tointeger(pL, 1);
    lua_pushnil(pL);
    while (lua_next(pL, 2) != 0)
    {
        bytes.push_back((long long)lua_tointeger(pL, -1));
        
        lua_pop(pL, 1);
    }
    void* address = utils::GetPlot((void*)ptr, bytes);
    long long addr = (long long)address;
    lua_pushinteger(pL, addr);
    return 1;
}
static int System_Memory_GetAddressData(lua_State* pL) {
    long long ptr = (long long)lua_tointeger(pL, 1);
    string type = (string)lua_tostring(pL, 2);
    void* address = (void*)ptr;
    if (address != nullptr) {
        if (type == "int")
            lua_pushinteger(pL, *(int*)(ptr));
        else if (type == "float")
            lua_pushnumber(pL, *(float*)(ptr));
        else if (type == "bool")
            lua_pushboolean(pL, *(bool*)(ptr));
        else if (type == "byte")
            lua_pushinteger(pL, *(char*)(ptr));
        else if (type == "string") {
            string memory_string = (char*)(ptr);
            lua_pushstring(pL, memory_string.c_str());
        }
        else
            lua_pushinteger(pL, *(char*)(ptr));
    }
    else
        lua_pushstring(pL, "Address error");
    return 1;
}
static int System_Memory_SetAddressData(lua_State* pL) {
    long long ptr = (long long)lua_tointeger(pL, 1);
    string type = (string)lua_tostring(pL, 2);
    void* address = (void*)ptr;
    if (address != nullptr) {
        if (type == "int") {
            *(int*)(ptr) = (int)lua_tointeger(pL, 3);
            lua_pushboolean(pL, true);
        }
        else if (type == "float") {
            *(float*)(ptr) = (float)lua_tonumber(pL, 3);
            lua_pushboolean(pL, true);
        }
        else if (type == "bool") {
            *(bool*)(ptr) = (bool)lua_toboolean(pL, 3);
            lua_pushboolean(pL, true);
        }
        else if (type == "byte") {
            *(char*)(ptr) = (char)lua_tointeger(pL, 3);
            lua_pushboolean(pL, true);
        }
        else
            lua_pushboolean(pL, false);
    }
    else
        lua_pushboolean(pL, false);
    return 1;
}
#pragma endregion
#pragma region GameFun
//添加特效
static int Game_Player_AddEffect(lua_State* pL) {
    int group = (int)lua_tointeger(pL, 1);
    int record = (int)lua_tointeger(pL, 2);
    void* PlayerPlot = *(undefined**)MH::Player::PlayerBasePlot;
    PlayerPlot = *offsetPtr<undefined**>((undefined(*)())PlayerPlot, 0x50);
    void* Effects = *offsetPtr<void*>(PlayerPlot, 0x8808);
    MH::Player::Effects((undefined*)Effects, group, record);
    return 0;
}
//执行Fsm动作
static int Game_Player_RunFsmAction(lua_State* pL) {
    int type = (int)lua_tointeger(pL, 1);
    int id = (int)lua_tointeger(pL, 2);
    void* PlayerPlot = *(undefined**)MH::Player::PlayerBasePlot;
    PlayerPlot = *offsetPtr<undefined**>((undefined(*)())PlayerPlot, 0x50);
    *offsetPtr<int>(PlayerPlot, 0x6284) = type;
    *offsetPtr<int>(PlayerPlot, 0x6288) = id;
    *offsetPtr<int>(PlayerPlot, 0x628C) = type;
    *offsetPtr<int>(PlayerPlot, 0x6290) = id;
    return 0;
}
//执行Lmt动作
static int Game_Player_RunLmtAction(lua_State* pL) {
    int id = (int)lua_tointeger(pL, -1);
    void* PlayerPlot = *(undefined**)MH::Player::PlayerBasePlot;
    PlayerPlot = *offsetPtr<undefined**>((undefined(*)())PlayerPlot, 0x50);
    MH::Player::CallLmt((undefined*)PlayerPlot, id, 0);
    return 0;
}
//切换武器
static int Game_Player_Weapon_ChangeWeapons(lua_State* pL) {
    int type = (int)lua_tointeger(pL, 1);
    int id = (int)lua_tointeger(pL, 2);
    if (type <= 13 and type >= 0 and id >= 0) {
        void* PlayerPlot = *(undefined**)MH::Player::PlayerBasePlot;
        PlayerPlot = *offsetPtr<undefined**>((undefined(*)())PlayerPlot, 0x50);
        if (lua_gettop(pL) > 3) {
            *offsetPtr<int>(PlayerPlot, 0x13860) = type;
            *offsetPtr<int>(PlayerPlot, 0x13864) = id;
            MH::Weapon::CompleteChangeWeapon(PlayerPlot, 1, 0);
        }
        else
            MH::Weapon::ChangeWeapon(PlayerPlot, type, id);
    }
    return 0;
}
//发射投射物
struct Vector3 {
    float x, y, z;
    Vector3(float x = 0, float y = 0, float z = 0) :x(x), y(y), z(z) { };
};
//执行投射物生成
static bool CallProjectilesGenerate(int Id, float* Coordinate, void* FromPtr = nullptr) {
    void* ShlpRoute = MH::Shlp::GetShlp(FromPtr, Id);
    if (ShlpRoute == nullptr)
        return false;
    ShlpRoute = *offsetPtr<void*>(ShlpRoute, 0x8);
    MH::Shlp::CallShlp(ShlpRoute, *(undefined**)MH::Player::PlayerBasePlot, *(undefined**)MH::Player::PlayerBasePlot, Coordinate);
    return true;
}
//处理投射物路径数据
static void GenerateProjectilesCoordinateData(float*& CalculationCoordinates, Vector3 startPoint, Vector3 endPoint) {
    //缓存指针
    float* temp_float = CalculationCoordinates;
    //写入起始坐标
    *temp_float = startPoint.x;
    temp_float++;
    *temp_float = startPoint.y;
    temp_float++;
    *temp_float = startPoint.z;
    temp_float++;
    //起始坐标写入完成，空4个字节
    *temp_float = 0;
    temp_float++;
    //更换指针为单字节并写入1
    unsigned char* temp_byte = (unsigned char*)temp_float;
    *temp_byte = 1;

    //重设缓存指针至坐标地址40处
    temp_float = offsetPtr<float>(CalculationCoordinates, 0x40);
    //写入结束坐标
    *temp_float = endPoint.x;
    temp_float++;
    *temp_float = endPoint.y;
    temp_float++;
    *temp_float = endPoint.z;
    temp_float++;
    //结束坐标写入完成，空4个字节
    *temp_float = 0;
    temp_float++;
    //更换指针为单字节并写入1
    temp_byte = (unsigned char*)temp_float;
    *temp_byte = 1;

    //重设缓存指针至坐标地址A0处
    int* tempCoordinateTailData = offsetPtr<int>(CalculationCoordinates, 0xA0);
    //写入坐标数据尾部信息
    *tempCoordinateTailData = 0x12;
    tempCoordinateTailData++;
    longlong* tempCoordinateTailData_longlong = (longlong*)tempCoordinateTailData;
    *tempCoordinateTailData_longlong = -1;
}
//生成投射物
static bool CreateProjectiles(int Id, Vector3 startPoint, Vector3 endPoint, void* FromPtr = nullptr) {
    //创建投射物路径数据缓存指针
    float* CoordinatesData = new float[73];
    //填充缓存区数据
    memset(CoordinatesData, 0, 73 * 4);
    //处理投射物路径数据
    GenerateProjectilesCoordinateData(CoordinatesData, startPoint, endPoint);
    //执行生成投射物
    bool GenerateResults = CallProjectilesGenerate(Id, CoordinatesData, FromPtr);
    //清理缓冲区
    delete[]CoordinatesData;
    return GenerateResults;
}
static int Game_Player_CreateProjectiles(lua_State* pL) {
    int id = (int)lua_tointeger(pL, 1);
    float startx = (float)lua_tonumber(pL, 2);
    float starty = (float)lua_tonumber(pL, 3);
    float startz = (float)lua_tonumber(pL, 4);
    float endx = (float)lua_tonumber(pL, 5);
    float endy = (float)lua_tonumber(pL, 6);
    float endz = (float)lua_tonumber(pL, 7);
    long long entity = (long long)lua_tointeger(pL, 8);
    void* EntityAddress = (void*)entity;
    if (EntityAddress != nullptr) {
        lua_pushboolean(pL, CreateProjectiles(
            id, Vector3(startx, starty, startz), Vector3(endx, endy, endz), EntityAddress
        ));
    }
    return 1;
}
#pragma endregion
#pragma region UI
//ImGui::Begin
int Imgui_Bindings(lua_State* L) {
    sol::state_view lua(L);
    sol_ImGui::Init(lua);
    return 0;
}
#pragma endregion

static void registerFunc(lua_State* L) {

#pragma region LuaFun
	//存入整数变量
	lua_register(L, "setGlobalVariable_int", Lua_Variable_SaveIntVariable);
	//存入浮点数变量
	lua_register(L, "setGlobalVariable_float", Lua_Variable_SaveFloatVariable);
	//存入字符串变量
	lua_register(L, "setGlobalVariable_string", Lua_Variable_SaveStringVariable);
	//读取整数变量
	lua_register(L, "GlobalVariable_int", Lua_Variable_ReadIntVariable);
	//读取浮点数变量
	lua_register(L, "GlobalVariable_float", Lua_Variable_ReadFloatVariable);
	//读取字符串变量
	lua_register(L, "GlobalVariable_string", Lua_Variable_ReadStringVariable);
	//销毁变量
	lua_register(L, "DestroyGlobalVariable", Lua_Variable_DestroyVariable);
#pragma endregion
#pragma region System
    //检查按键
    lua_register(L, "CheckKey", System_Keyboard_CheckKey);
    //检查按键是否处于按下状态
    lua_register(L, "CheckKeyIsPressed", System_Keyboard_CheckKeyIsPressed);
    //检查Xbox按键
    lua_register(L, "XCheckKey", System_XboxPad_CheckKey);
    //检查Xbox按键是否处于按下状态
    lua_register(L, "XCheckKeyIsPressed", System_XboxPad_CheckKeyIsPressed);
    //添加计时器
    lua_register(L, "AddChronoscope", System_Chronoscope_AddChronoscope);
    //检查计时器
    lua_register(L, "CheckChronoscope", System_Chronoscope_CheckChronoscope);
    //检查计时器是否存在
    lua_register(L, "CheckPresenceChronoscope", System_Chronoscope_CheckPresenceChronoscope);
    //删除计时器
    lua_register(L, "DelChronoscope", System_Chronoscope_DelChronoscope);
    //向游戏内发送消息
    lua_register(L, "Message", System_Message_ShowMessage);
    //向控制台发送消息
    lua_register(L, "Console_Info", System_Console_Info);
    //向控制台发送错误消息
    lua_register(L, "Console_Error", System_Console_Error);
#pragma endregion
#pragma region Memory
    //获取内存地址
    lua_register(L, "GetAddress", System_Memory_GetAddress);
    //获取内存地址数据
    lua_register(L, "GetAddressData", System_Memory_GetAddressData);
    //修改内存地址数据
    lua_register(L, "SetAddressData", System_Memory_SetAddressData);
#pragma endregion
#pragma region Game
    //添加特效
    lua_register(L, "AddEffect", Game_Player_AddEffect);
    //执行Fsm动作(完全可通过修改内存实现，暂且先加上吧)
    lua_register(L, "RunFsmAction", Game_Player_RunFsmAction);
    //执行Lmt动作
    lua_register(L, "RunLmtAction", Game_Player_RunLmtAction);
    //更换玩家武器
    lua_register(L, "ChangeWeapons", Game_Player_Weapon_ChangeWeapons);
    //发射投射物
    lua_register(L, "CreateProjectiles", Game_Player_CreateProjectiles);
#pragma endregion
#pragma region UI
    lua_register(L, "Imgui_Bindings", Imgui_Bindings);
    if (luaL_dostring(L, "Imgui_Bindings()")) {
        lua_error(L);
    }

    
#pragma endregion

}