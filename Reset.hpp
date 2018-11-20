#pragma once

using ResetFn = long(__stdcall *)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

namespace original
{
	ResetFn reset = nullptr;
}

long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	auto hr = original::reset(device, pPresentationParameters);

	if (hr >= 0) {
		ImGui_ImplDX9_CreateDeviceObjects();
		drawmanager::SetupFonts();
		Particles = new particle_network(globals::screenweight, globals::screenheight, 60);
	}

	return hr;
}

