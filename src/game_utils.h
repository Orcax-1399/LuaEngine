#pragma once
#include <io.h>
#include <filesystem>
#include <MMSystem.h>
#pragma comment(lib,"Winmm.lib")

using namespace loader;
using namespace std;

#pragma region Lua Handle
namespace LuaHandle {
	struct LuaScriptData {
		lua_State* L;
		string name;
		string file;
		bool start;
		LuaScriptData(
			lua_State* L = nullptr,
			string name = "",
			string file = "",
			bool start = true
		) :L(L), name(name), file(file), start(start) { };
	};
	vector<string> LuaFiles;
	map<string, LuaScriptData> LuaScript;
	lua_State* Lc;
}
#pragma endregion

#pragma region General tools
namespace utils {
	//获取偏移地址
	static void* GetPlot(void* plot, const std::vector<int>& bytes) {
		void* Plot = plot;
		//处理基址
		if ((long long)plot > 0x140000000 && (long long)plot < 0x14579b000) {
			Plot = *(undefined**)plot;
		}
		for (int i : bytes) {
			if (Plot != nullptr) {
				Plot = *offsetPtr<undefined**>((undefined(*)())Plot, i);
			}
			else {
				return nullptr;
			}
		}
		return Plot;
	}
	//获取随机数
	static float GetRandom(float min, float max)
	{
		std::random_device rd;
		std::mt19937 eng(rd());
		std::uniform_real_distribution<float> dist(min, max);
		return dist(eng);
	}
	//转utf8
	static string string_To_UTF8(const std::string& str)
	{
		int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
		wchar_t* pwBuf = new wchar_t[nwLen + 1];
		ZeroMemory(pwBuf, nwLen * 2 + 2);
		::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);
		int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
		char* pBuf = new char[nLen + 1];
		ZeroMemory(pBuf, nLen + 1);
		::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
		std::string retStr(pBuf);
		delete[]pwBuf;
		delete[]pBuf;
		pwBuf = NULL;
		pBuf = NULL;
		return retStr;
	}
	//转unicode
	std::string UTF8_To_string(const std::string& str)
	{
		int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
		wchar_t* pwBuf = new wchar_t[nwLen + 1];
		memset(pwBuf, 0, nwLen * 2 + 2);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);
		int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
		char* pBuf = new char[nLen + 1];
		memset(pBuf, 0, nLen + 1);
		WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
		std::string retStr = pBuf;
		delete[]pBuf;
		delete[]pwBuf;
		pBuf = NULL;
		pwBuf = NULL;
		return retStr;
	}
}
#pragma endregion

#pragma region Timer
namespace Chronoscope {
	struct ChronoscopeData {
		float StartTime = 0;
		float EndTime = 0;
	};

	//现在的时间
	float NowTime = 0;
	//计时器列表
	map<string, ChronoscopeData> ChronoscopeList;
	//添加计时器(时长，计时器名称，是否覆盖)
	static bool AddChronoscope(float duration, string name, bool Overlay = false) {
		if (ChronoscopeList.find(name) == ChronoscopeList.end() || Overlay) {
			ChronoscopeList[name].EndTime = NowTime + duration;
			ChronoscopeList[name].StartTime = NowTime;
			return true;
		}
		else
			return false;
	}
	//删除计时器
	static void DelChronoscope(string name) {
		if (ChronoscopeList.find(name) != ChronoscopeList.end()) {
			ChronoscopeList.erase(name);
		}
	}
	//检查计时器是否存在
	static bool CheckPresenceChronoscope(string name) {
		if (ChronoscopeList.find(name) != ChronoscopeList.end()) {
			return true;
		}
		return false;
	}
	//检查计时器是否结束
	static bool CheckChronoscope(string name) {
		if (ChronoscopeList.find(name) != ChronoscopeList.end()) {
			if (ChronoscopeList[name].EndTime < NowTime) {
				DelChronoscope(name);
				return true;
			}
			else
				return false;
		}
		return false;
	}
	//计时器更新程序
	static void chronoscope() {
		void* TimePlot = utils::GetPlot(*(undefined**)MH::Player::PlayerBasePlot, { 0x50, 0x7D20 });
		if(TimePlot != nullptr)
			NowTime = *offsetPtr<float>(TimePlot, 0xC24);
	}
}
#pragma endregion

#pragma region Key board
namespace Keyboard {
	namespace TempData {
		map<int, bool> t_KeyDown;
		map<int, int> t_KeyCount;
	}
	//检查窗口
	static bool CheckWindows() {
		HWND wnd = GetForegroundWindow();
		HWND mhd = FindWindow(L"MT FRAMEWORK", L"MONSTER HUNTER: WORLD(421631)");
		if (wnd == mhd)
			return true;
		else
			return false;
	}
	//按键检查
	static bool CheckKey(int vk, int ComboClick = 1, float Duration = 0.3) {
		if (!CheckWindows())
			return false;
		//建立按键档案
		if (TempData::t_KeyDown.find(vk) == TempData::t_KeyDown.end()) {
			TempData::t_KeyDown[vk] = false;
		}
		//按键检查
		if (GetKeyState(vk) < 0 and !TempData::t_KeyDown[vk]) {
			TempData::t_KeyDown[vk] = true;
			//连击检查
			if (TempData::t_KeyCount.find(vk) != TempData::t_KeyCount.end()) {
				//计时器检查
				if (TempData::t_KeyCount[vk] == 1)
					Chronoscope::AddChronoscope(Duration, "KEY_" + to_string(vk), true);
				if (Chronoscope::CheckChronoscope("KEY_" + to_string(vk))) {
					TempData::t_KeyCount[vk] = 0;
				}
				TempData::t_KeyCount[vk]++;
			}
			else {
				Chronoscope::AddChronoscope(Duration, "KEY_" + to_string(vk), true);
				TempData::t_KeyCount[vk] = 1;
			}

			//检查结果
			if (TempData::t_KeyCount[vk] == ComboClick)
				return true;
			else
				return false;
		}
		else if (GetKeyState(vk) >= 0)
			TempData::t_KeyDown[vk] = false;
		return false;
	}
}
#pragma endregion

#pragma region Xbox Pad
namespace XboxPad {
	namespace TempData {
		map<int, bool> t_KeyDown;
		map<int, int> t_KeyCount;
	}
	namespace KeyState {
		bool LJoystickUp = false;
		bool LJoystickRight = false;
		bool LJoystickDown = false;
		bool LJoystickLeft = false;
		bool LJoystickPress = false;
		bool RJoystickUp = false;
		bool RJoystickRight = false;
		bool RJoystickDown = false;
		bool RJoystickLeft = false;
		bool RJoystickPress = false;
		bool LT = false;
		bool RT = false;
		bool LB = false;
		bool RB = false;
		bool Up = false;
		bool Right = false;
		bool Down = false;
		bool Left = false;
		bool Y = false;
		bool B = false;
		bool A = false;
		bool X = false;
		bool Window = false;
		bool Menu = false;
	}
	static bool KeyIdHandle(int Id) {
		switch (Id)
		{
		case 0:
			return KeyState::LJoystickUp;
		case 1:
			return KeyState::LJoystickRight;
		case 2:
			return KeyState::LJoystickDown;
		case 3:
			return KeyState::LJoystickLeft;
		case 4:
			return KeyState::LJoystickPress;
		case 5:
			return KeyState::RJoystickUp;
		case 6:
			return KeyState::RJoystickRight;
		case 7:
			return KeyState::RJoystickDown;
		case 8:
			return KeyState::RJoystickLeft;
		case 9:
			return KeyState::RJoystickPress;
		case 10:
			return KeyState::LT;
		case 11:
			return KeyState::RT;
		case 12:
			return KeyState::LB;
		case 13:
			return KeyState::RB;
		case 14:
			return KeyState::Up;
		case 15:
			return KeyState::Right;
		case 16:
			return KeyState::Down;
		case 17:
			return KeyState::Left;
		case 18:
			return KeyState::Y;
		case 19:
			return KeyState::B;
		case 20:
			return KeyState::A;
		case 21:
			return KeyState::X;
		case 22:
			return KeyState::Window;
		case 23:
			return KeyState::Menu;
		default:
			return false;
		}
	}
	//按键检查
	static bool CheckKey(int Key, int ComboClick = 1, float Duration = 0.3) {
		//建立按键档案
		if (TempData::t_KeyDown.find(Key) == TempData::t_KeyDown.end()) {
			TempData::t_KeyDown[Key] = false;
		}
		//按键检查
		if (KeyIdHandle(Key) and !TempData::t_KeyDown[Key]) {
			TempData::t_KeyDown[Key] = true;
			//连击检查
			if (TempData::t_KeyCount.find(Key) != TempData::t_KeyCount.end()) {
				//计时器检查
				if (TempData::t_KeyCount[Key] == 1)
					Chronoscope::AddChronoscope(Duration, "XKEY_" + to_string(Key), true);
				if (Chronoscope::CheckChronoscope("XKEY_" + to_string(Key))) {
					TempData::t_KeyCount[Key] = 0;
				}
				TempData::t_KeyCount[Key]++;
			}
			else {
				Chronoscope::AddChronoscope(Duration, "XKEY_" + to_string(Key), true);
				TempData::t_KeyCount[Key] = 1;
			}

			//检查结果
			if (TempData::t_KeyCount[Key] == ComboClick)
				return true;
			else
				return false;
		}
		else
			TempData::t_KeyDown[Key] = false;
		return false;
	}
	static void Updata() {
		JOYINFO joyinfo;
		JOYINFOEX joyinfoex;
		joyinfoex.dwSize = sizeof(JOYINFOEX);
		joyinfoex.dwFlags = JOY_RETURNALL;
			UINT joy_nums;
			joy_nums = joyGetNumDevs();
			if (joy_nums > 0) {
				MMRESULT joy_ret = joyGetPosEx(JOYSTICKID1, &joyinfoex);
				if (joy_ret == JOYERR_NOERROR) {
					if (joyinfoex.dwButtons == 1)
						KeyState::A = KeyState::A + 0.0166;
					if (joyinfoex.dwButtons == 2)
						KeyState::B = KeyState::B + 0.0166;
					if (joyinfoex.dwButtons == 8)
						KeyState::Y = KeyState::Y + 0.0166;
					if (joyinfoex.dwButtons == 32)
						KeyState::RB = KeyState::RB + 0.0166;
					if (joyinfoex.dwZpos < 32767)
						KeyState::RT = KeyState::RT + 0.0166;
					if (joyinfoex.dwZpos > 32767)
						KeyState::LT = KeyState::LT + 0.0166;
				}
			}
			if (GetAsyncKeyState(VK_LBUTTON) < 0)
				KeyState::Y = KeyState::Y + 0.0166;
			if (GetAsyncKeyState(VK_RBUTTON) < 0)
				KeyState::B = KeyState::B + 0.0166;
			if (GetAsyncKeyState(VK_SPACE) < 0)
				KeyState::A = KeyState::A + 0.0166;
			if (GetAsyncKeyState(67) < 0)
				KeyState::LT = KeyState::LT + 0.0166;
			if (GetAsyncKeyState(VK_LSHIFT) < 0)
				KeyState::RB = KeyState::RB + 0.0166;
			if (GetAsyncKeyState(VK_XBUTTON2) < 0)
				KeyState::RT = KeyState::RT + 0.0166;
			if (joyinfoex.dwButtons == 0 && GetAsyncKeyState(VK_LBUTTON) == 0)
				KeyState::Y = 0;
			if (joyinfoex.dwButtons == 0 && GetAsyncKeyState(VK_RBUTTON) == 0)
				KeyState::B = 0;
			if (joyinfoex.dwButtons == 0 && GetAsyncKeyState(VK_SPACE) == 0)
				KeyState::A = 0;
			if (joyinfoex.dwButtons == 0 && GetAsyncKeyState(VK_LSHIFT) == 0)
				KeyState::RB = 0;
			if (joyinfoex.dwZpos == 32767 && GetAsyncKeyState(VK_XBUTTON2) == 0)
				KeyState::RT = 0;
			if (joyinfoex.dwZpos == 32767 && GetAsyncKeyState(67) == 0)
				KeyState::LT = 0;
		/*
		KeyState::LJoystickUp = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC44) > 0.0;
		KeyState::LJoystickRight = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC40) > 0.0;
		KeyState::LJoystickDown = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC44) < 0.0;
		KeyState::LJoystickLeft = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC40) < 0.0;
		KeyState::RJoystickUp = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC48) > 0.0;
		KeyState::RJoystickRight = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC4C) > 0.0;
		KeyState::RJoystickDown = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC48) < 0.0;
		KeyState::RJoystickLeft = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC4C) < 0.0;
		KeyState::LJoystickPress = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC64) != 0.0;
		KeyState::RJoystickPress = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC68) != 0.0;
		KeyState::LT = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC88) != 0.0;
		KeyState::RT = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC8C) != 0.0;
		KeyState::LB = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC80) != 0.0;
		KeyState::RB = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC84) != 0.0;
		KeyState::Up = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC70) != 0.0;
		KeyState::Right = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC74) != 0.0;
		KeyState::Down = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC78) != 0.0;
		KeyState::Left = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC7C) != 0.0;
		KeyState::Y = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC90) != 0.0;
		KeyState::B = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC94) != 0.0;
		KeyState::A = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC98) != 0.0;
		KeyState::X = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC9C) != 0.0;
		KeyState::Window = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC60) != 0.0;
		KeyState::Menu = *offsetPtr<float>(*(undefined**)MH::GamePad::XboxPadPtr, 0xC6C) != 0.0;
		*/
	}
}
#pragma endregion