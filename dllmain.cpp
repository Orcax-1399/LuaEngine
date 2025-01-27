#include <fstream>
#include <queue>
#include <functional>

#include <random>
#include <iostream>
#include <cmath>

#include <windows.h>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include "minhook/include/MinHook.h"
#include "json/json.hpp"
#include "sol/sol.hpp"
#include "loader.h"
#include "ghidra_export.h"
#include "util.h"
#include <thread>

#include "game_utils.h"
#include "lua_core.h"

#include "hook_monster.h"
#include "hook_animals.h"
#include "hook_camera.h"
#include "hook_hit.h"
#include "hook_shlp.h"
#include "hook_frame.h"
#include "hook_screen.h"

#include "hook_ui.h"

using namespace loader;

#pragma region Basic data processing program
static void MassageCommand() {
	void* MassagePlot = *(undefined**)MH::World::Message;
	if (MassagePlot != nullptr) {
		string Massage = offsetPtr<char>(MassagePlot, 0xC0);
		string::size_type idx;
		//执行实时命令
		idx = Massage.find("luac:");
		if (idx != string::npos) {
			string command = Massage.substr(Massage.find("luac:") + 5);
			framework_logger->info("执行游戏内发出的Lua实时指令: {}", command);
			int err = 0;
			err = luaL_dostring(LuaHandle::Lc, command.c_str());
			if (err != 0)
			{
				int type = lua_type(LuaHandle::Lc, -1);
				if (type == 4) {
					string error = lua_tostring(LuaHandle::Lc, -1);
					LuaCore::LuaErrorRecord(error);
				}
			}
			*offsetPtr<char>(MassagePlot, 0xC0) = *"";
		}
		//重载虚拟机
		idx = Massage.find("reload ");
		if (idx != string::npos) {
			string luae = Massage.substr(Massage.find("reload ") + 7);
			std::map<string, LuaHandle::LuaScriptData>::iterator it;
			it = LuaHandle::LuaScript.find(luae);
			if (it != LuaHandle::LuaScript.end()) {
				framework_logger->info("重载脚本{}的Lua虚拟机", luae);
				lua_close(LuaHandle::LuaScript[luae].L);
				LuaHandle::LuaScript[luae].L = luaL_newstate();
				luaL_openlibs(LuaHandle::LuaScript[luae].L);
				registerFunc(LuaHandle::LuaScript[luae].L);
				hook_monster::Registe(LuaHandle::LuaScript[luae].L);
				hook_animals::Registe(LuaHandle::LuaScript[luae].L);
				hook_camera::Registe(LuaHandle::LuaScript[luae].L);
				hook_hit::Registe(LuaHandle::LuaScript[luae].L);
				hook_frame::Registe(LuaHandle::LuaScript[luae].L);
				hook_shlp::Registe(LuaHandle::LuaScript[luae].L);
				//hook_screen::Registe(LuaHandle::LuaScript[luae].L);
				hook_ui::LuaRegister(LuaHandle::LuaScript[luae].L);
				if (LuaCore::Lua_Run(LuaHandle::LuaScript[luae].L, luae) != 1) {
					engine_logger->warn("脚本{}重载后出现异常，已停止该脚本继续运行", luae);
					LuaHandle::LuaScript[luae].start = false;
				}
				else {
					engine_logger->info("脚本{}已完成重载操作，代码运行正常", luae);
					string message = "脚本" + luae + "已完成重载操作";
					MH::Chat::ShowGameMessage(*(undefined**)MH::Chat::MainPtr, (undefined*)&utils::string_To_UTF8(message)[0], -1, -1, 0);
					LuaHandle::LuaScript[luae].start = true;
					LuaCore::run("on_init", LuaHandle::LuaScript[luae].L);
				}
			}
			*offsetPtr<char>(MassagePlot, 0xC0) = *"";
		}
	}
}
//数据更新程序
static void updata() {
	//地图更新时清理数据
	void* TimePlot = utils::GetPlot(*(undefined**)MH::Player::PlayerBasePlot, { 0x50, 0x7D20 });
	if (TimePlot != nullptr && Chronoscope::NowTime > *offsetPtr<float>(TimePlot, 0xC24)) {
		framework_logger->info("游戏内发生场景变换，更新框架缓存数据");
		//清除计时器数据
		Chronoscope::ChronoscopeList.clear();
		//清除按键数据
		Keyboard::TempData::t_KeyCount.clear();
		Keyboard::TempData::t_KeyDown.clear();
		//清除Xbox手柄数据
		XboxPad::TempData::t_KeyCount.clear();
		XboxPad::TempData::t_KeyDown.clear();
		//钩子数据
		hook_frame::SpeedList.clear();
		engine_logger->info("运行脚本on_switch_scenes场景切换代码");
		LuaCore::run("on_switch_scenes");
	}
	//更新计时器
	Chronoscope::chronoscope();
	//更新手柄数据
	XboxPad::Updata();
	//执行玩家消息指令
	MassageCommand();
}
#pragma endregion

__declspec(dllexport) extern bool Load()
{
	engine_logger->set_level(spdlog::level::info);
	engine_logger->flush_on(spdlog::level::trace);
	framework_logger->set_level(spdlog::level::info);
	framework_logger->flush_on(spdlog::level::trace);
	lua_logger->set_level(spdlog::level::info);
	lua_logger->flush_on(spdlog::level::trace);
	imgui_logger->set_level(spdlog::level::info);
	imgui_logger->flush_on(spdlog::level::trace);
	//加载lua文件
	LuaCore::Lua_Load("Lua\\", LuaHandle::LuaFiles);
	//脚本实时环境
	framework_logger->info("创建并运行实时脚本环境");
	LuaHandle::Lc = luaL_newstate();
	luaL_openlibs(LuaHandle::Lc);
	registerFunc(LuaHandle::Lc);
	//加载并运行脚本
	for (string file_name : LuaHandle::LuaFiles) {
		if (LuaHandle::LuaScript[file_name].start && LuaHandle::LuaScript[file_name].L != nullptr) {
			lua_State* L = LuaHandle::LuaScript[file_name].L;
			luaL_openlibs(L);
			engine_logger->info("为{}脚本注册引擎初始化函数", file_name);
			registerFunc(L);
			hook_monster::Registe(L);
			hook_animals::Registe(L);
			hook_camera::Registe(L);
			hook_hit::Registe(L);
			hook_frame::Registe(L);
			hook_shlp::Registe(L);
			//hook_screen::Registe(L);
			hook_ui::LuaRegister(L);
			if (LuaCore::Lua_Run(L, file_name) != 1) {
				engine_logger->warn("脚本{}运行过程中出现异常，已停止该脚本继续运行", file_name);
				LuaHandle::LuaScript[file_name].start = false;
			}
		}
	}
	//其他绑定操作
	framework_logger->info("开始执行创建各功能钩子代码");
	hook_monster::Hook();
	hook_animals::Hook();
	hook_camera::Hook();
	hook_hit::Hook();
	hook_shlp::Hook();
	hook_frame::Hook();
	//hook_screen::Hook();
	//运行lua脚本中初始化代码
	engine_logger->info("运行脚本on_init初始化代码");
	LuaCore::run("on_init");
	//初始化钩子
	framework_logger->info("创建on_time钩子");
	MH_Initialize();
	HookLambda(MH::World::MapClockLocal,
		[](auto clock, auto clock2) {
			auto ret = original(clock, clock2);
			//更新基础数据
			updata();
			//运行lua虚拟机
			LuaCore::run("on_time");
			hook_ui::init();
			//启动绘制更新
			if(!LuaCore::luaframe)
				LuaCore::luaframe = true;
			return ret;
		});
	MH_ApplyQueued();
	return true;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
		DisableThreadLibraryCalls(hModule);
		hook_ui::hMod = hModule;
		return Load();
	return TRUE;
}

