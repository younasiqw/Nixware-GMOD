#pragma once
#include "sdk.hpp"
#include "drawmanager.hpp"

typedef void(__thiscall *tCreateMove)(void*, int, float, bool);

namespace original
{
	tCreateMove CreateMove = nullptr;
}

void __fastcall hkCreateMove(void* thiscall, void* _EDX, int sequence_number, float input_sample_frametime, bool active)
{
	original::CreateMove(thiscall, sequence_number, input_sample_frametime, active);

	if (!globals::input)
		return;

	CUserCmd* pCommand = globals::input->GetUserCmd(sequence_number);
	if (pCommand)
	{
		CBaseEntity *localPlayer = (CBaseEntity*)globals::entitylist->get_entity(globals::engine->get_local_player());
		if (!localPlayer)
			return;

		CVerifiedUserCmd *pVerifiedCommands = *(CVerifiedUserCmd**)((DWORD)globals::input + 0xC8);
		CVerifiedUserCmd *pVerified = &pVerifiedCommands[sequence_number % MULTIPLAYER_BACKUP];

		if (pVerified)
		{
			pVerified->m_cmd = *pCommand;
			pVerified->m_crc = CRC32_ProcessSingleBuffer(pCommand, sizeof(pCommand));
		}
	}
}
