#ifndef __BITMAP_H__
#define __BITMAP_H__

#pragma warning(disable : 4786)

#define MIRROR 1
#define FLIP 2
#define ROTATE_90 4

#include <string>
#include <list>

#include "typedef.h"
#include "vector2d.h"

class CSurface;
struct IDirectDrawSurface7;
struct _DDSURFACEDESC2;
typedef _DDSURFACEDESC2 DDSURFACEDESC2;

class outputfile;
class inputfile;

class bitmap
{
public:
	friend class graphics;
	friend class colorizablebitmap;
	bitmap(std::string);
	bitmap(ushort, ushort);
	~bitmap();
	void Save(outputfile&) const;
	void Load(inputfile&);
	void Save(std::string) const;
	void PutPixel(ushort X, ushort Y, ushort Color) { Data[Y][X] = Color; }
	ushort GetPixel(ushort X, ushort Y) const { return Data[Y][X]; }
	void Fill(ushort Color) { Fill(0, 0, XSize, YSize, Color); }
	void Fill(ushort, ushort, ushort, ushort, ushort);
	void Blit(bitmap*, ushort, ushort, ushort, ushort, ushort, ushort, uchar = 0) const;
	void Blit(bitmap*, ushort, ushort, ushort, ushort, ushort, ushort, ushort) const;
	void MaskedBlit(bitmap*, ushort, ushort, ushort, ushort, ushort, ushort, uchar = 0) const;
	void MaskedBlit(bitmap*, ushort, ushort, ushort, ushort, ushort, ushort, ushort) const;
	void AlphaBlit(bitmap*, ushort, ushort, ushort, ushort, ushort, ushort, uchar) const;
	void AlphaBlit(bitmap*, ushort, ushort) const;
	void DrawLine(ushort, ushort, ushort, ushort, ushort = 0xFFFF, bool = false);
	ushort GetXSize() const { return XSize; }
	ushort GetYSize() const { return YSize; }
	void DrawPolygon(vector2d, ushort, ushort, ushort, bool = false, double = 0);
	void CreateAlphaMap(uchar);
	bool FadeAlpha(uchar);
	void SetAlpha(ushort X, ushort Y, uchar Alpha) { AlphaMap[Y][X] = Alpha; }
	uchar GetAlpha(ushort X, ushort Y) const { return AlphaMap[Y][X]; }
	void Outline(ushort);
	void CreateOutlineBitmap(bitmap*, ushort);
protected:
	ushort** Data;
	ushort XSize, YSize;
	uchar** AlphaMap;
};

#endif
