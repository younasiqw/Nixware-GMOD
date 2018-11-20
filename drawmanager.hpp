#pragma once
#include "ISurface.hpp"
#include "particles.hpp"

particle_network* Particles = nullptr;

namespace drawmanager
{
	namespace fonts
	{
		DWORD Watermark;
		DWORD Esp;
	}

	void SetupFonts()
	{
		drawmanager::fonts::Watermark = globals::surface->FontCreate();
		drawmanager::fonts::Esp = globals::surface->FontCreate();

		globals::surface->SetFontGlyphSet(drawmanager::fonts::Esp, ("Calibri"), 14, 150, 0, 0, FONTFLAG_OUTLINE /*| FONTFLAG_ANTIALIAS*/);
		globals::surface->SetFontGlyphSet(drawmanager::fonts::Watermark, ("Arial"), 16, 800, 0, 0, FONTFLAG_OUTLINE /*| FONTFLAG_ANTIALIAS*/);
	}

	void DrawLine(int x0, int y0, int x1, int y1, Color color)
	{
		globals::surface->DrawSetColor(color);
		globals::surface->DrawLine(x0, y0, x1, y1);
	}

	void DrawRectFilled(int x0, int y0, int x1, int y1, Color color)
	{
		globals::surface->DrawSetColor(color);
		globals::surface->DrawFilledRect(x0, y0, x1, y1);
	}

	void FillRGBA(int x, int y, int w, int h, Color c)
	{
		globals::surface->DrawSetColor(c);
		globals::surface->DrawFilledRect(x, y, x + w, y + h);
	}

	void DrawRect(int x0, int y0, int x1, int y1, Color color)
	{
		globals::surface->DrawSetColor(color);
		globals::surface->DrawOutlinedRect(x0, y0, x1, y1);
	}

	void DrawOutlinedRect(int x0, int y0, int x1, int y1, Color color)
	{
		globals::surface->DrawSetColor(color);
		globals::surface->DrawOutlinedRect(x0, y0, x1, y1);
		globals::surface->DrawSetColor(Color(0, 0, 0, 255));
		globals::surface->DrawOutlinedRect(x0 - 1, y0 - 1, x1 + 1, y1 + 1);
		globals::surface->DrawOutlinedRect(x0 + 1, y0 + 1, x1 - 1, y1 - 1);
	}

	void DrawString(DWORD font, int x, int y, Color col, const char *pszText, ...)
	{
		if (pszText == NULL)
			return;

		va_list va_alist;
		char szBuffer[1024] = { '\0' };
		wchar_t szString[1024] = { '\0' };

		va_start(va_alist, pszText);
		vsprintf_s(szBuffer, pszText, va_alist);
		va_end(va_alist);

		wsprintfW(szString, L"%S", szBuffer);
		globals::surface->DrawSetTextPos(x, y);
		globals::surface->DrawSetTextFont(font);
		globals::surface->DrawSetTextColor(col.r(), col.g(), col.b(), col.a());
		globals::surface->DrawPrintText(szString, wcslen(szString));
	}
}
