#pragma once
#include "vars.hpp"
#include "sdk.hpp"

#define TABWIDTH 72
#define TABWHEIGHT 27

#define AIMBOT ("A")
#define TRIGGER ("K")
#define VISUALS ("D")
#define SKINS ("B")
#define COLORS ("H")
#define MISC ("G")
ImFont*  bigFont = nullptr;
ImFont*  forTabs = nullptr;

void ImDrawRectRainbow(int x, int y, int width, int height, float flSpeed, float &flRainbow)
{
	ImDrawList* windowDrawList = ImGui::GetWindowDrawList();

	Color colColor(0, 0, 0);

	flRainbow += flSpeed;
	if (flRainbow > 1.f) flRainbow = 0.f;

	for (int i = 0; i < width; i++)
	{
		float hue = (1.f / (float)width) * i;
		hue -= flRainbow;
		if (hue < 0.f) hue += 1.f;

		Color colRainbow = colColor.FromHSB(hue, 1.f, 1.f);
		windowDrawList->AddRectFilled(ImVec2(x + i, y), ImVec2(width, height), colRainbow.GetU32());
	}
}

int tab = 0;

enum TABS
{
	AIMBOT_TAB,
	TRIGGER_TAB,
	VISUALS_TAB,
	SKINS_TAB,
	COLORS_TAB,
	MISC_TAB
};

void RenderMenu()
{
	if (!globals::menuopened)
		return;

	if (ImGui::Begin(("Nixware By N1x33r"), &globals::menuopened, ImVec2(676, 326), 1.0f, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar))
	{
		ImGui::BeginGroup();
		{
			ImGui::PushFont(bigFont);
			ImGui::TextColored(ImColor(1.f, 1.f, 1.f, 1.f), (u8"nixware          "));
			ImGui::PopFont();
		}
		ImGui::EndGroup();

		ImGui::SameLine();

		ImGui::BeginGroup();
		{
			ImGui::PushFont(forTabs);

			if (ImGui::Button(AIMBOT, ImVec2(TABWIDTH, TABWHEIGHT))) tab = AIMBOT_TAB;
			ImGui::SameLine();
			if (ImGui::Button(TRIGGER, ImVec2(TABWIDTH, TABWHEIGHT))) tab = TRIGGER_TAB;
			ImGui::SameLine();
			if (ImGui::Button(VISUALS, ImVec2(TABWIDTH, TABWHEIGHT))) tab = VISUALS_TAB;
			ImGui::SameLine();
			if (ImGui::Button(SKINS, ImVec2(TABWIDTH, TABWHEIGHT))) tab = SKINS_TAB;
			ImGui::SameLine();
			if (ImGui::Button(COLORS, ImVec2(TABWIDTH, TABWHEIGHT))) tab = COLORS_TAB;
			ImGui::SameLine();
			if (ImGui::Button(MISC, ImVec2(TABWIDTH, TABWHEIGHT))) tab = MISC_TAB;
			ImGui::PopFont();
		}
		ImGui::EndGroup();

		ImGui::Spacing();
		ImGui::Spacing();


		ImGui::BeginGroup();
		static float flRainbow;
		float flSpeed = 0.001f;
		int height, width;

		ImVec2 curPos = ImGui::GetCursorPos();
		ImVec2 curWindowPos = ImGui::GetWindowPos();
		curPos.x += curWindowPos.x;
		curPos.y += curWindowPos.y;
		ImDrawList* windowDrawList = ImGui::GetWindowDrawList();
		ImDrawRectRainbow(curPos.x - 6, curPos.y - 6, globals::screenweight, curPos.y - 2, flSpeed, flRainbow);
		ImGui::EndGroup();

		ImGui::BeginChild(("##TEST")); //Âñå ôóíêöèè
		{
			ImGui::BeginGroup();
			ImGui::Spacing();

			if (tab == AIMBOT_TAB)
			{

			}
			else if (tab == TRIGGER_TAB)
			{
			}
			else if (tab == VISUALS_TAB)
			{
				ImGui::Checkbox("Enable", &settings::esp::enabled);
				ImGui::Checkbox("Box", &settings::esp::box);
				ImGui::Checkbox("Healthbar", &settings::esp::healthbar);
				ImGui::Checkbox("Name", &settings::esp::name);

				ImGui::SliderFloat("aaa", &settings::esp::unf, 0.f, 15.f);

			}
			else if (tab == SKINS_TAB)
			{
			}
			else if (tab == COLORS_TAB)
			{
			}
			else if (tab == MISC_TAB)
			{
			}
			ImGui::EndGroup();

		}
		ImGui::EndChild();
	}
	ImGui::End();
}
