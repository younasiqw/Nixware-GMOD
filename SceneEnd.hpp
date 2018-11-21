#pragma once
#include "sdk.hpp"

typedef void(__fastcall *tSceneEnd)(void*, void*);

namespace original
{
	tSceneEnd sceneend = nullptr;
}

void __fastcall hkSceneEnd(void* thisptr, void* edx)
{
	original::sceneend(thisptr, edx);
}