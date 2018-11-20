#pragma once
#include "sdk.hpp"
#include "drawmanager.hpp"
#include "esp.hpp"

typedef void(__thiscall *tPaintTraverse)(void*, unsigned int, bool, bool);

namespace original
{
	tPaintTraverse PaintTraverse = nullptr;
}

void __fastcall hkPaintTraverse(void *thisptr, void *_EDX, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	original::PaintTraverse(thisptr, vguiPanel, forceRepaint, allowForce);
	globals::engine->get_screen_size(globals::screenweight, globals::screenheight);

	static unsigned int OverlayPopupPanel;
	if (OverlayPopupPanel == NULL)
	{
		const char* szName = globals::panel->GetName(vguiPanel);
		printf("Panel name: %s\n", szName);
		if (szName[0] == 'O' && szName[7] == 'P')
		{
			drawmanager::SetupFonts();
			OverlayPopupPanel = vguiPanel;
		}
	}

	if (OverlayPopupPanel == vguiPanel)
	{
		globals::surface->PushMakeCurrent(vguiPanel, false);
		if (settings::esp::enabled) renderesp();
		globals::panel->SetMouseInputEnabled(vguiPanel, false);

		if (IsKeyPressed(VK_INSERT))
			globals::menuopened = !globals::menuopened;
		if (globals::menuopened) {
			globals::panel->SetMouseInputEnabled(vguiPanel, true);
			Particles->draw(255);
		}
		drawmanager::DrawString(drawmanager::fonts::Watermark, 4, 4, Color::Aqua(), " Nixware - Garry's Mod");

		globals::surface->PopMakeCurrent(vguiPanel);
	}
}
