#include <Windows.h>
#include "fmath.hpp"
#include <thread>
#include <chrono>
#include <iostream>
#include "Esp.hpp"
#include "sdk.hpp"
#include <fstream>
#include "vmthook.hpp"
#include "drawmanager.hpp"
#include "pattern.hpp"
#include "PaintTraverse.hpp"
#include "EndScene.hpp"
#include "Reset.hpp"
#include "CreateMove.hpp"

vfunc_hook ppanel;
vfunc_hook direct3d_hook;
vfunc_hook client;

uint64_t FindSignature(const char* szModule, const char* szSignature)
{
	MODULEINFO modInfo;
	GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(szModule), &modInfo, sizeof(MODULEINFO));
	DWORD startAddress = (DWORD)modInfo.lpBaseOfDll;
	DWORD endAddress = startAddress + modInfo.SizeOfImage;
	const char* pat = szSignature;
	DWORD firstMatch = 0;
	for (DWORD pCur = startAddress; pCur < endAddress; pCur++) {
		if (!*pat) return firstMatch;
		if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
			if (!firstMatch) firstMatch = pCur;
			if (!pat[2]) return firstMatch;
			if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?') pat += 3;
			else pat += 2;    //one ?
		}
		else {
			pat = szSignature;
			firstMatch = 0;
		}
	}
	return NULL;
}

DWORD FindPattern(DWORD start_offset, DWORD size, BYTE* pattern, char mask[])
{
	DWORD pos = 0;
	int searchLen = strlen(mask) - 1;

	for (DWORD retAddress = start_offset; retAddress < start_offset + size; retAddress++)
	{
		if (*(BYTE*)retAddress == pattern[pos] || mask[pos] == '?') {
			if (mask[pos + 1] == '\0')
				return (retAddress - searchLen);
			pos++;
		}
		else
			pos = 0;
	}
	return NULL;
}

using color_t = uint8_t[4];
void(__cdecl* datapaths)(void) = nullptr;

unsigned long __stdcall init(void* dll)
{
	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);

	globals::engine = interface::get<Engine>(GetModuleHandleW(L"engine.dll"), "VEngineClient");
	globals::entitylist = interface::get<EntityList>(GetModuleHandleW(L"client.dll"), "VClientEntityList");
	globals::surface = interface::get<ISurface>(GetModuleHandleW(L"vguimatsurface.dll"), "VGUI_Surface030");
	globals::panel = interface::get<IPanel>(GetModuleHandleW(L"vgui2.dll"), "VGUI_Panel009");
	globals::lua = interface::get<lua::Shared>(GetModuleHandleW(L"lua_shared.dll"), "LUASHARED");
	globals::client = interface::get<IBaseClientDLL>(GetModuleHandleW(L"client.dll"), "VClient017");
	globals::trace = interface::get<IEngineTrace>(GetModuleHandleW(L"engine.dll"), "EngineTraceClient003");
	globals::modelinfo = interface::get<IVModelInfo>(GetModuleHandleW(L"engine.dll"), "VModelInfoClient006");

	globals::pDevice = **(IDirect3DDevice9***)(FindSignature("shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);

	printf("globals::engine: 0x%X\n", (DWORD)globals::engine);
	printf("globals::entitylist: 0x%X\n", (DWORD)globals::entitylist);
	printf("globals::panel: 0x%X\n", (DWORD)globals::panel);
	printf("globals::lua: 0x%X\n", (DWORD)globals::lua);
	printf("globals::client: 0x%X\n", (DWORD)globals::client);
	printf("globals::trace: 0x%X\n", (DWORD)globals::trace);
	printf("globals::modelinfo: 0x%X\n", (DWORD)globals::modelinfo);
	printf("globals::pDevice: 0x%X\n", (DWORD)globals::pDevice);
	globals::engine->get_screen_size(globals::screenweight, globals::screenheight);

	using datapack_paths_t = void(__cdecl*)();
	datapaths = reinterpret_cast<datapack_paths_t>(signature::search(GetModuleHandleW(L"client.dll"), signature::detail::convert("55 8B EC 8B 0D ?? ?? ?? ?? 83 EC 7C")));
	// datapaths();

	auto chl = interface::get<void>(GetModuleHandleW(L"client.dll"), "VClient");
	auto classes = method<get_all_classes_t>(8, chl)(chl);

	for (; classes; classes = classes->next)
		netvars::store(classes->table->table, classes->table);

	// PaintTraverse Hook
	if (ppanel.setup(globals::panel)) {
		original::PaintTraverse = ppanel.get_original<tPaintTraverse>(41);
		printf("Original PaintTraverse founded \n");
		ppanel.hook_index(41, hkPaintTraverse);
		printf("PaintTraverse founded \n");
	}

	// EndScene & Reset Hook
	if (direct3d_hook.setup(globals::pDevice)) {
		original::endscene = direct3d_hook.get_original<EndSceneFn>(42);
		printf("Original EndScene founded \n");
		direct3d_hook.hook_index(42, hkEndScene);
		printf("EndScene hooked \n");

		original::reset = direct3d_hook.get_original<ResetFn>(16);
		printf("Original Reset founded \n");
		direct3d_hook.hook_index(16, hkReset);
		printf("Reset hooked \n");
	}

	// CreateMove Hook
	if (client.setup(globals::client)) {
		original::CreateMove = client.get_original<tCreateMove>(21);
		DWORD dwInputPointer = FindPattern((DWORD)original::CreateMove, 0x100, (byte*)"\x8B\x0D", "xx");
		if (dwInputPointer != NULL)
		{
			dwInputPointer += 0x2;
			globals::input = **(CInput***)dwInputPointer;
			printf("globals::input: 0x%X\n", (DWORD)globals::input);

		}
		client.hook_index(21, hkCreateMove);
	}

	Particles = new particle_network(globals::screenweight, globals::screenheight, 60);

	for (; !(GetAsyncKeyState(VK_HOME) & 1); std::this_thread::sleep_for(std::chrono::milliseconds(25)));

	SetWindowLongPtrW(FindWindowW(L"Valve001", nullptr), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(original::proc));

	direct3d_hook.unhook_all();
	ppanel.unhook_all();
	client.unhook_all();

	ImGui_ImplDX9_Shutdown();

	FreeLibraryAndExitThread(HMODULE(dll), 0);
}

int __stdcall DllMain(HINSTANCE dll, DWORD reason, LPVOID)
{
	DisableThreadLibraryCalls(dll);
	if (reason == DLL_PROCESS_ATTACH)
	{
		CreateThread(nullptr, 0, init, dll, 0, nullptr);
	}

	return 1;
}
