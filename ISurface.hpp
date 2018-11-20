#pragma once
#include "color.hpp"
#include "interface.hpp"

enum FontDrawType_t
{
	// Use the "additive" value from the scheme file
	FONT_DRAW_DEFAULT = 0,

	// Overrides
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,

	FONT_DRAW_TYPE_COUNT = 2,
};

enum EFontFlags
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,
	FONTFLAG_BITMAP = 0x800,
};

struct IntRect
{
	int x0;
	int y0;
	int x1;
	int y1;
};

typedef unsigned long HFont;
typedef unsigned int VPANEL;

enum class FontDrawType {
	FONT_DRAW_DEFAULT = 0,
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,
	FONT_DRAW_TYPE_COUNT = 2,
};

class ISurface
{
public:
	void DrawSetColor(Color col)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, Color);
		return GetVFunc<OriginalFn>(this, 10)(this, col);
	}

	void UnlockCursor()
	{
		typedef void(__thiscall* OriginalFn)(PVOID);
		GetVFunc< OriginalFn >(this, 61)(this);
	}

	void LockCursor()
	{
		typedef void(__thiscall* OriginalFn)(PVOID);
		GetVFunc< OriginalFn >(this, 62)(this);
	}

	void PushMakeCurrent(VPANEL panel, bool useInsets)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, VPANEL, bool);
		return GetVFunc<OriginalFn>(this, 8)(this, panel, useInsets);
	}

	void PopMakeCurrent(VPANEL panel)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, VPANEL);
		return GetVFunc<OriginalFn>(this, 9)(this, panel);
	}

	void DrawOutlinedRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		GetVFunc<OriginalFn>(this, 14)(this, x0, y0, x1, y1);
	}

	void DrawFilledRect(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		GetVFunc<OriginalFn>(this, 12)(this, x0, y0, x1, y1);
	}

	void SurfaceGetCursorPos(int &x, int &y)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int&, int&);
		GetVFunc<OriginalFn>(this, 97)(this, x, y);
	}

	void DrawOutlinedCircle(int x, int y, int radius, int segments)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		GetVFunc<OriginalFn>(this, 100)(this, x, y, radius, segments);
	}

	unsigned long FontCreate()
	{
		typedef unsigned int(__thiscall* OriginalFn)(PVOID);
		return GetVFunc<OriginalFn>(this, 66)(this);
	}

	void SetFontGlyphSet(unsigned long font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, unsigned long, const char*, int, int, int, int, int, int, int);
		GetVFunc<OriginalFn>(this, 67)(this, font, windowsFontName, tall, weight, blur, scanlines, flags, 0, 0);
	}

	void DrawLine(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		GetVFunc<OriginalFn>(this, 15)(this, x0, y0, x1, y1);
	}

	void DrawSetTextFont(unsigned long font)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, unsigned long);
		GetVFunc<OriginalFn>(this, 17)(this, font); // 21 mb
	}

	void DrawSetTextColor(int r, int g, int b, int a)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int, int, int);
		GetVFunc<OriginalFn>(this, 19)(this, r, g, b, a); // 19
	}

	void DrawSetTextColor(Color col)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, Color);
		return GetVFunc<OriginalFn>(this, 18)(this, col);
	}

	void DrawSetTextPos(int x, int y) // maybe
	{
		typedef void(__thiscall* OriginalFn)(PVOID, int, int);
		GetVFunc<OriginalFn>(this, 20)(this, x, y);
	}

	void GetTextSize(HFont font, const wchar_t *text, int &wide, int &tall)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, HFont, const wchar_t*, int&, int&);
		GetVFunc<OriginalFn>(this, 76)(this, font, text, wide, tall);
	}

	void DrawPrintText(const wchar_t* text, int textLen)
	{
		typedef void(__thiscall* OriginalFn)(PVOID, const wchar_t*, int, int);
		GetVFunc<OriginalFn>(this, 22)(this, text, textLen, 0);
	}
}; 

namespace globals
{
	inline ISurface* surface = nullptr;
}
