#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#define DOUBLEBUFFER	graphics::GetDoubleBuffer()
#define XRES		graphics::GetXRes()
#define YRES		graphics::GetYRes()

#define MAKE_RGB(Red, Green, Blue) ((Red) << 8 & 0xF800) | ((Green) << 3 & 0x7E0) | ((Blue) >> 3 & 0x1F)

#define GET_RED(Color)		((Color >> 8) & 0xF8)
#define GET_GREEN(Color)	((Color >> 3) & 0xFC)
#define GET_BLUE(Color)		((Color << 3) & 0xF8)

#define PINK 0xF81F

#include <windows.h>
#include <vector>

#include "typedef.h"

class bitmap;
class CDisplay;

class graphics
{
public:
	friend class bitmap;
	static void Init(void);
	static void DeInit(void);
	static void SetMode(HINSTANCE, HWND*, const char*, ushort, ushort, uchar, bool);
	static void BlitDBToScreen(void);
	static void ClearDBToColor(ushort = 0);
	static void ClearDBToColor(ushort, ushort, ushort, ushort, ushort = 0);
	static ushort GetXRes(void) { return XRes; }
	static ushort GetYRes(void) { return YRes; }
	static bitmap* GetDoubleBuffer(void) { return DoubleBuffer; }
	static CDisplay* GetDXDisplay(void) { return DXDisplay; }
	static void UpdateBounds(void);
	static void SwitchMode(void);
private:
	static HWND hWnd;
	static bool FullScreen;
	static CDisplay* DXDisplay;
	static bitmap* DoubleBuffer;
	static ushort XRes;
	static ushort YRes;
	static uchar ColorDepth;
	static std::vector<bitmap*> BitmapContainer;
};

#endif
