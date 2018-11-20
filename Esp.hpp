#pragma once
#include "EndScene.hpp"
#include "Entities.hpp"
#include "Engine.hpp"
#include "Player.hpp"
#include <algorithm>
#include "drawmanager.hpp"
#include "vars.hpp"

#include <string>

/*
	Todo:
		- Optimize code
*/
namespace offsets
{
	constexpr std::ptrdiff_t m_rgflCoordinateFrame = 0x308;
}

namespace esp
{
	inline std::optional<ImVec2> __fastcall transform(const matrix4x4_t& matrix, const ImVec2& resolution, const Vector& position)
	{
		float w = matrix[12] * position[0] + matrix[13] * position[1] + matrix[14] * position[2] + matrix[15];

		if (w < 0.01f)
			return {};

		w = 1.f / w;
		float x = (matrix[0] * position[0] + matrix[1] * position[1] + matrix[2] * position[2] + matrix[3]) * w;
		float y = (matrix[4] * position[0] + matrix[5] * position[1] + matrix[6] * position[2] + matrix[7]) * w;

		return ImVec2
		{
			(resolution.x * 0.5f) + (0.5f * x * resolution.x + 0.5f),
			(resolution.y * 0.5f) - (0.5f * y * resolution.y + 0.5f)
		};
	}

	inline bool __fastcall transform(const matrix4x4_t& matrix, const ImVec2& resolution, const Vector& position, ImVec2& out)
	{
		float w = matrix[12] * position[0] + matrix[13] * position[1] + matrix[14] * position[2] + matrix[15];

		if (w < 0.01f)
			return false;

		w = 1.f / w;
		float x = (matrix[0] * position[0] + matrix[1] * position[1] + matrix[2] * position[2] + matrix[3]) * w;
		float y = (matrix[4] * position[0] + matrix[5] * position[1] + matrix[6] * position[2] + matrix[7]) * w;

		out.x = (resolution.x * 0.5f) + (0.5f * x * resolution.x + 0.5f);
		out.y = (resolution.y * 0.5f) - (0.5f * y * resolution.y + 0.5f);

		return true;
	}

	inline uint32_t __fastcall calculate_health_color(int32_t health, int32_t max)
	{
		if (health > max)
			return IM_COL32(32, 115, 255, 255);

		auto multiplier = 510.f / max;

		auto r = (std::min)(static_cast<uint32_t>(multiplier * (max - health)), 255u);
		auto g = (std::min)(static_cast<uint32_t>(multiplier * (health)), 255u);
		return 0xFF000000 + r + (g << 8);
	}

	inline void __fastcall eye_trace(ImDrawList* list, const matrix4x4_t& matrix, const ImVec2& res, const Vector& origin, const Angle& angles)
	{
		constexpr float len = 240.f;
		float pitch = angles[0] * radians;
		float yaw = angles[1] * radians;

		float sp = std::sin(pitch);
		float cp = std::cos(pitch);
		
		float sy = std::sin(yaw);
		float cy = std::cos(yaw);

		Vector dir(cp * cy, cp * sy, -sp);

		if (auto start = transform(matrix, res, origin))
		{
			if (auto end = transform(matrix, res, origin + dir * len))
			{
				list->AddLine(start.value(), end.value(), IM_COL32(255, 255, 255, 255));
			}
		}
	}

	bool IsVisible(CBaseEntity* pLocal, Vector Src, Vector Dst)
	{
		Ray_t ray;
		ray.Init(Src, Dst);

		trace_t tr;
		CTraceFilter filter;
		//filter.pSkip = pLocal;

		globals::trace->TraceRay(ray, 0x46004033, &filter, &tr);

		return (tr.fraction > 0.97f);
	}
	Vector headPos;

	int FindHead(CBaseEntity* pPlayer)
	{
		studiohdr_t *hdr = globals::modelinfo->GetStudioModel(pPlayer->GetModel());
		if (!hdr)
			return -1;
		mstudiobone_t* bone;
		for (int k = 0; k < hdr->numbones; k++)
		{
			bone = hdr->pBone(k);
			if ((bone && (bone->flags & (0x00000100)) && (bone->parent != -1)))
			{
				char* boneName = bone->pszName();
				std::string name(boneName);
				size_t found = name.find("Head");
				if (found != std::string::npos)
					return k;
			}
		}
		return -1;
	}

	void GetBonePositions(/*pos_t &pos,*/ CBaseEntity* pPlayer)
	{
		int head = FindHead(pPlayer);
	}

	inline void __fastcall draw_bounding_box(const matrix4x4_t& matrix/*, ImDrawList* list*/, const ImVec2& res, CBaseEntity* ply)
	{
		auto& frame   = *reinterpret_cast<matrix3x4_t*>(reinterpret_cast<uintptr_t>(ply) + offsets::m_rgflCoordinateFrame);
		auto& min     = ply->get_oob_min();
		auto& max     = ply->get_oob_max();
		auto health   = ply->get_health();
		auto capacity = ply->get_max_health();

		if (!ply->is_alive())
			return;

		Vector coords[]
		{
			Vector(min),
			Vector(min[0], max[1], min[2]),
			Vector(max[0], max[1], min[2]),
			Vector(max[0], min[1], min[2]),
			Vector(max),
			Vector(min[0], max[1], max[2]),
			Vector(min[0], min[1], max[2]),
			Vector(max[0], min[1], max[2]),
		};

		ImVec2 points[std::size(coords)];

		for (auto& coord : coords)
		{
			fmath::vector_transform(frame, coord, coord);
		}

		for (size_t i = 0; i < std::size(points); ++i)
		{
			if (!transform(matrix, res, coords[i], points[i]))
				return;
		}

		float left = points[0].x, right = points[0].x, bottom = points[0].y, top = points[0].y;

		for (const auto& point : points)
		{
			if (left > point.x)
				left = point.x;
			if (top > point.y)
				top = point.y;
			if (right < point.x)
				right = point.x;
			if (bottom < point.y)
				bottom = point.y;
		}

		auto* localplayer = (CBaseEntity*)(globals::entitylist->get_entity(globals::engine->get_local_player()));

		if (settings::esp::healthbar) 
		{
			auto offset = (std::min)(static_cast<float>(health) / capacity, 1.f);
			int green = int(health * 2.55f);
			int red = 255 - green;
			drawmanager::DrawRectFilled(left - 5.f, top - 1.f, left - 2.f, bottom + 1.f, Color::Black());
			if (health > 0)
				drawmanager::DrawRectFilled(left - 4.f, top, left - 3.f, top - offset * (top - bottom), Color(red, green, 0, 255));
		}
		CBaseEntity *localPlayer = (CBaseEntity*)globals::entitylist->get_entity(globals::engine->get_local_player());
		Vector localEyePos = localPlayer->get_eye_pos();
		//GetBonePosition(ply, headPos, FindHead(ply));
		//bool bIsVisible = (IsVisible(localPlayer, localEyePos, ply->get_abs_origin()));
		//IsVisible(localPlayer, localEyePos, headPos);
		if (settings::esp::box /*&& bIsVisible*/) {
			drawmanager::DrawOutlinedRect(left, top, right, bottom, Color::Green());
		}

		if (settings::esp::name)
		{
			std::string name = ply->get_name();
			std::wstring widestr = std::wstring(name.begin(), name.end());
			int tw, th;
			globals::surface->GetTextSize(drawmanager::fonts::Esp, std::wstring(name.begin(), name.end()).c_str(), tw, th);
			ImVec2 pos = ImVec2(left + .5f * (right - left) - tw * .5f - 1.f, top - th * 0.942);
			drawmanager::DrawString(drawmanager::fonts::Esp, pos.x, pos.y, Color::Green(), name.c_str());
		}

		if (settings::esp::name)
		{
			std::string team = ply->get_team_name();
			int tw, th;
			globals::surface->GetTextSize(drawmanager::fonts::Esp, std::wstring(team.begin(), team.end()).c_str(), tw, th);
			ImVec2 pos = ImVec2(left + .5f * (right - left) - tw * .5f - 1.f, bottom + th * -0.100);
			drawmanager::DrawString(drawmanager::fonts::Esp, pos.x, pos.y, Color::Green(), team.c_str());
		}
	}

	inline void __fastcall draw_3d_box(const matrix4x4_t& matrix, ImDrawList* list, const ImVec2& res, CBaseEntity* ply)
	{
		auto& pos = ply->get_pos();
		auto& min = ply->get_oob_min();
		auto& max = ply->get_oob_max();
		auto  yaw = ply->get_angles()[1];
		
		Vector coords[]
		{
			Vector(min),
			Vector(min[0], max[1], min[2]),
			Vector(max[0], max[1], min[2]),
			Vector(max[0], min[1], min[2]),
			Vector(min[0], min[1], max[2]),
			Vector(min[0], max[1], max[2]),
			Vector(max),
			Vector(max[0], min[1], max[2]),
		};

		ImVec2 points[std::size(coords)];

		for (auto& coord : coords)
		{
			coord.rotate(yaw);
			coord += pos;
		}

		for (size_t i = 0; i < std::size(points); ++i)
		{
			if (!transform(matrix, res, coords[i], points[i]))
				return;
		}

		auto col = ply->get_team_color().value_or(0xFFFF0000);

		for (size_t i = 1; i <= 4; ++i)
		{
			list->AddLine(points[i - 1], points[i % 4], col);
			list->AddLine(points[i - 1], points[i + 3], col);
			list->AddLine(points[i + 3], points[i % 4 + 4], col);
		}
	}
	
}

inline void renderesp()
{
	if (!globals::engine->is_ingame())
		return;

	auto glua = globals::lua->create_interface(lua::type::client);
	if (glua) {
		glua->PushSpecial(lua::special::glob);
		glua->GetField(-1, "cam");
		glua->GetField(-1, "Start3D");
		glua->Call(0, 1);
		glua->Pop();
	}
	auto& matrix = globals::engine->get_view_matrix();
	if (glua) {
		glua->GetField(-1, "End3D");
		glua->Call(0, 1);
		glua->Pop(3);
	}
	auto local = globals::engine->get_local_player();
	for (size_t i = 1; i <= globals::engine->get_max_clients(); ++i)
	{
		auto player = reinterpret_cast<CBaseEntity*>(globals::entitylist->get_entity(i));

		if (i == local || !player || player->is_dormant())
			continue;

		/*
		player_info_t info{ 0 };
		if (!globals::engine->get_player_info(i, &info))
			continue;
		*/

		esp::draw_bounding_box(matrix,/* list,*/ ImVec2(globals::screenweight, globals::screenheight), player);
	}
}
