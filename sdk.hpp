#pragma once

bool bKeyPress[256];
bool IsKeyPressed(int iKey)
{
	if (GetAsyncKeyState(iKey))
	{
		if (!bKeyPress[iKey])
		{
			bKeyPress[iKey] = true;
			return true;
		}
	}
	else
		bKeyPress[iKey] = false;
	return false;
}

#include "color.hpp"
#include "ISurface.hpp"
#include "IPanel.hpp"
#include "Entities.hpp"
#include "lua.hpp"
#include "Net.hpp"
#include "Entities.hpp"
#include "Engine.hpp"
#include "IBaseClient.hpp"
#include "IClientMode.hpp"
#include "CInput.hpp"
#include "IEngineTrace.hpp"
#include "ModelInfo.hpp"
#include "Singleton.hpp"
