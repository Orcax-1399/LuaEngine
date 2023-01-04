#include <fstream>
#include <queue>
#include <functional>

#include <random>
#include <iostream>
#include <cmath>

#include <windows.h>

#include "minhook/include/MinHook.h"
#include "json/json.hpp"
#include "sol/sol.hpp"
#include "loader.h"
#include "ghidra_export.h"
#include "util.h"
#include <thread>

#include "game_utils.h"

using namespace loader;

#pragma region Basic data processing program
//计时器更新程序
static void chronoscope() {
	void* TimePlot = utils::GetPlot(*(undefined**)MH::Player::PlayerBasePlot, { 0x50, 0x88, 0x1B0, 0x308, 0x10, 0x10 });
	Chronoscope::NowTime = *offsetPtr<float>(TimePlot, 0xC24);
}
//数据更新程序
static void updata() {
	//更新计时器
	chronoscope();
	//更新手柄数据
	XboxPad::Updata();
}
#pragma endregion

//主程序
static void run() {
	
}

__declspec(dllexport) extern bool Load()
{
	//初始化钩子
	MH_Initialize();
	HookLambda(MH::World::MapClockLocal,
		[](auto clock, auto clock2) {
			auto ret = original(clock, clock2);
			updata();
			run();
			return ret;
		});
	
	MH_ApplyQueued();
	return true;
}

// lua线程
void startup_thread(HMODULE handle_module) {
	sol::state lua;
	lua.open_libraries();
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)startup_thread, hModule, 0, nullptr);
		return Load();
	return TRUE;
}

