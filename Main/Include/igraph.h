#ifndef __IGRAPH_H__
#define __IGRAPH_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include <map>

#include "ivandef.h"
#include "femath.h"

class bitmap;
class colorizablebitmap;
class outputfile;
class inputfile;
class festring;

/* memcmp doesn't like alignment of structure members */

#ifdef VC
#pragma pack(1)
#endif

struct graphicid
{
  graphicid() { }
  bool operator<(const graphicid& GI) const { return memcmp(this, &GI, sizeof(graphicid)) < 0; }
  ushort BitmapPosX NO_ALIGNMENT;
  ushort BitmapPosY NO_ALIGNMENT;
  packedcolor16 Color[4] NO_ALIGNMENT;
  uchar Frame NO_ALIGNMENT;
  uchar FileIndex NO_ALIGNMENT;
  ushort SpecialFlags NO_ALIGNMENT;
  packedalpha BaseAlpha NO_ALIGNMENT;
  packedalpha Alpha[4] NO_ALIGNMENT;
  uchar SparkleFrame NO_ALIGNMENT;
  uchar SparklePosX NO_ALIGNMENT;
  uchar SparklePosY NO_ALIGNMENT;
  packedcolor16 OutlineColor NO_ALIGNMENT;
  packedalpha OutlineAlpha NO_ALIGNMENT;
  ushort Seed NO_ALIGNMENT;
  uchar FlyAmount NO_ALIGNMENT;
  vector2d Position NO_ALIGNMENT;
  uchar RustData[4] NO_ALIGNMENT;
};

#ifdef VC
#pragma pack()
#endif

outputfile& operator<<(outputfile&, const graphicid&);
inputfile& operator>>(inputfile&, graphicid&);

struct tile
{
  tile() { }
  tile(bitmap* Bitmap) : Bitmap(Bitmap), Users(1) { }
  bitmap* Bitmap;
  long Users;
};

typedef std::map<graphicid, tile> tilemap;

struct graphicdata
{
  graphicdata() : AnimationFrames(0) { }
  ~graphicdata();
  void Save(outputfile&) const;
  void Load(inputfile&);
  void Retire();
  int AnimationFrames;
  bitmap** Picture;
  tilemap::iterator* GraphicIterator;
};

outputfile& operator<<(outputfile&, const graphicdata&);
inputfile& operator>>(inputfile&, graphicdata&);

class igraph
{
 public:
  static void Init();
  static void DeInit();
  static const bitmap* GetWTerrainGraphic() { return Graphic[GR_WTERRAIN]; }
  static const bitmap* GetFOWGraphic() { return Graphic[GR_FOW]; }
  static const bitmap* GetCursorGraphic() { return Graphic[GR_CURSOR]; }
  static const bitmap* GetSymbolGraphic() { return Graphic[GR_SYMBOL]; }
  static bitmap* GetTileBuffer() { return TileBuffer; }
  static void DrawCursor(vector2d);
  static tilemap::iterator AddUser(const graphicid&);
  static void RemoveUser(tilemap::iterator);
  static const colorizablebitmap* GetHumanoidRawGraphic() { return RawGraphic[GR_HUMANOID]; }
  static const colorizablebitmap* GetCharacterRawGraphic() { return RawGraphic[GR_CHARACTER]; }
  static const colorizablebitmap* GetEffectRawGraphic() { return RawGraphic[GR_EFFECT]; }
  static const colorizablebitmap* GetRawGraphic(int I) { return RawGraphic[I]; }
  static const int* GetBodyBitmapValidityMap(int);
  static bitmap* GetFlagBuffer() { return FlagBuffer; }
  static const bitmap* GetMenuGraphic() { return Menu; }
  static void LoadMenu();
  static void UnLoadMenu();
  static bitmap* GetSilhouetteCache(int I1, int I2) { return SilhouetteCache[I1][I2]; }
 private:
  static void EditBodyPartTile(bitmap*, int);
  static vector2d RotateTile(bitmap*, vector2d, int);
  static void CreateBodyBitmapValidityMaps();
  static void CreateSilhouetteCaches();
  static colorizablebitmap* RawGraphic[RAW_TYPES];
  static bitmap* Graphic[GRAPHIC_TYPES];
  static bitmap* TileBuffer;
  static const char* RawGraphicFileName[];
  static const char* GraphicFileName[];
  static tilemap TileMap;
  static uchar RollBuffer[256];
  static bitmap* FlagBuffer;
  static int** BodyBitmapValidityMap;
  static bitmap* Menu;
  static bitmap* SilhouetteCache[HUMANOID_BODYPARTS][CONDITION_COLORS];
};

#endif
