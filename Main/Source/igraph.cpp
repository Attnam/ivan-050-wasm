/*
 *
 *  Iter Vehemens ad Necem 
 *  Copyright (C) Timo Kiviluoto
 *  Released under GNU General Public License
 *
 *  See LICENSING which should included with 
 *  this file for more details
 *
 */

#include "igraph.h"
#include "felist.h"
#include "bitmap.h"
#include "graphics.h"
#include "iconf.h"
#include "rawbit.h"
#include "game.h"
#include "save.h"
#include "object.h"
#include "allocate.h"
#include "whandler.h"

rawbitmap* igraph::RawGraphic[RAW_TYPES];
bitmap* igraph::Graphic[GRAPHIC_TYPES];
bitmap* igraph::TileBuffer;
bitmap* igraph::FlagBuffer;
const char* igraph::RawGraphicFileName[] = { "Graphics/GLTerra.pcx", "Graphics/OLTerra.pcx", "Graphics/Item.pcx", "Graphics/Char.pcx", "Graphics/Humanoid.pcx", "Graphics/Effect.pcx", "Graphics/Cursor.pcx" };
const char* igraph::GraphicFileName[] = { "Graphics/WTerra.pcx", "Graphics/FOW.pcx", "Graphics/Symbol.pcx" };
tilemap igraph::TileMap;
uchar igraph::RollBuffer[256];
int** igraph::BodyBitmapValidityMap;
bitmap* igraph::Menu;
bitmap* igraph::SilhouetteCache[HUMANOID_BODYPARTS][CONDITION_COLORS][SILHOUETTE_TYPES];
rawbitmap* igraph::ColorizeBuffer[2] = { new rawbitmap(16, 16), new rawbitmap(16, 16) };
bitmap* igraph::Cursor[CURSOR_TYPES];
color16 igraph::CursorColor[CURSOR_TYPES] = { MakeRGB16(40, 40, 40),
					      MakeRGB16(255, 0, 0),
					      MakeRGB16(100, 100, 255),
					      MakeRGB16(200, 200, 0) };
bitmap* igraph::BackGround;
int igraph::CurrentColorType = -1;

void igraph::Init()
{
  static bool AlreadyInstalled = false;

  if(!AlreadyInstalled)
    {
      AlreadyInstalled = true;
      graphics::Init();
      graphics::SetMode("IVAN " IVAN_VERSION, festring(game::GetGameDir() + "Graphics/Icon.bmp").CStr(), 800, 600, ivanconfig::GetFullScreenMode());
      DOUBLE_BUFFER->ClearToColor(0);
      graphics::BlitDBToScreen();
#ifndef __DJGPP__
      graphics::SetSwitchModeHandler(ivanconfig::SwitchModeHandler);
#endif
      graphics::LoadDefaultFont(game::GetGameDir() + "Graphics/Font.pcx");
      FONT->CreateFontCache(WHITE);
      FONT->CreateFontCache(LIGHT_GRAY);
      felist::CreateQuickDrawFontCaches(FONT, WHITE, 8);
      felist::CreateQuickDrawFontCaches(FONT, LIGHT_GRAY, 8);
      object::InitSparkleValidityArrays();
      int c;

      for(c = 0; c < RAW_TYPES; ++c)
	RawGraphic[c] = new rawbitmap(game::GetGameDir() + RawGraphicFileName[c]);

      for(c = 0; c < GRAPHIC_TYPES; ++c)
	{
	  Graphic[c] = new bitmap(game::GetGameDir() + GraphicFileName[c]);
	  Graphic[c]->ActivateFastFlag();
	}

      ColorizeBuffer[0]->CopyPaletteFrom(RawGraphic[0]);
      ColorizeBuffer[1]->CopyPaletteFrom(RawGraphic[0]);
      TileBuffer = new bitmap(16, 16);
      TileBuffer->ActivateFastFlag();
      TileBuffer->CreatePriorityMap(0);
      FlagBuffer = new bitmap(16, 16);
      FlagBuffer->ActivateFastFlag();
      FlagBuffer->CreateAlphaMap(0);
      Alloc2D(BodyBitmapValidityMap, 8, 16);
      CreateBodyBitmapValidityMaps();
      CreateSilhouetteCaches();

      for(c = 0; c < CURSOR_TYPES; ++c)
	{
	  Cursor[c] = new bitmap(48, 16, TRANSPARENT_COLOR);
	  Cursor[c]->CreateAlphaMap(255);
	  packedcolor16 Color = CursorColor[c];
	  GetCursorRawGraphic()->MaskedBlit(Cursor[c], 0, 0, 0, 0, 48, 16, &Color);
	}
    }
}

void igraph::DeInit()
{
  int c;

  for(c = 0; c < RAW_TYPES; ++c)
    delete RawGraphic[c];

  for(c = 0; c < GRAPHIC_TYPES; ++c)
    delete Graphic[c];

  delete TileBuffer;
  delete FlagBuffer;
  delete [] BodyBitmapValidityMap;
  delete BackGround;
}

void igraph::DrawCursor(vector2d Pos, int CursorData)
{
  /* Inefficient gum solution */

  color24 Luminance = ivanconfig::GetContrastLuminance();
  bitmap* CursorBitmap = Cursor[CursorData&~(TARGET|FLASH)];

  if(!(CursorData & (TARGET|FLASH)))
    {
      CursorBitmap->LuminanceMaskedBlit(DOUBLE_BUFFER, 0, 0, Pos, 16, 16, Luminance);
      return;
    }

  if(!(CursorData & TARGET))
    {
      int Tick = GET_TICK() % 32;
      CursorBitmap->FillAlpha(95 + 10 * abs(Tick - 16));
      CursorBitmap->AlphaBlit(DOUBLE_BUFFER, 0, 0, Pos, 16, 16, Luminance);
      return;
    }

  int Tick = (GET_TICK() << 2) / 3;
  int Frame = (Tick >> 4) % 3;
  int X1 = Frame << 4;
  CursorBitmap->FillAlpha(255 - (Tick & 0xF) * 16);
  CursorBitmap->AlphaBlit(DOUBLE_BUFFER, X1, 0, Pos, 16, 16, Luminance);
  int X2 = ((Frame + 1) % 3) << 4;
  CursorBitmap->FillAlpha((Tick & 0xF) * 16);
  CursorBitmap->AlphaBlit(DOUBLE_BUFFER, X2, 0, Pos, 16, 16, Luminance);
}

const vector2d SizeVect = vector2d(16, 16);

tilemap::iterator igraph::AddUser(const graphicid& GI)
{
  tilemap::iterator Iterator = TileMap.find(GI);

  if(Iterator != TileMap.end())
    {
      tile& Tile = Iterator->second;
      ++Tile.Users;
      return Iterator;
    }
  else
    {
      const int SpecialFlags = GI.SpecialFlags;
      const int BodyPartFlags = SpecialFlags & 0x38;
      const int RotateFlags = SpecialFlags & 0x7;
      const int Frame = GI.Frame;
      vector2d SparklePos = vector2d(GI.SparklePosX, GI.SparklePosY);
      rawbitmap* RawBitmap = RawGraphic[GI.FileIndex];
      vector2d RawPos = vector2d(GI.BitmapPosX, GI.BitmapPosY);

      if(BodyPartFlags && BodyPartFlags < ST_OTHER_BODYPART)
	{
	  ColorizeBuffer[0]->Clear();
	  EditBodyPartTile(RawBitmap, ColorizeBuffer[0], RawPos, BodyPartFlags);
	  RawBitmap = ColorizeBuffer[0];
	  RawPos.X = RawPos.Y = 0;

	  if(RotateFlags)
	    {
	      ColorizeBuffer[1]->Clear();
	      RotateTile(RawBitmap, ColorizeBuffer[1], RawPos, SparklePos, RotateFlags);
	      RawBitmap = ColorizeBuffer[1];
	    }
	}
      else if(RotateFlags)
	{
	  ColorizeBuffer[0]->Clear();
	  RotateTile(RawBitmap, ColorizeBuffer[0], RawPos, SparklePos, RotateFlags);
	  RawBitmap = ColorizeBuffer[0];
	  RawPos.X = RawPos.Y = 0;
	}

      bitmap* Bitmap = RawBitmap->Colorize(RawPos, SizeVect, GI.Position, GI.Color, GI.BaseAlpha, GI.Alpha, GI.RustData, !(GI.SpecialFlags & ST_DISALLOW_R_COLORS));
      Bitmap->ActivateFastFlag();

      if(BodyPartFlags)
	Bitmap->CreatePriorityMap(SpecialFlags & ST_CLOAK ? CLOAK_PRIORITY : AVERAGE_PRIORITY);

      if(GI.OutlineColor != TRANSPARENT_COLOR)
	Bitmap->Outline(GI.OutlineColor, GI.OutlineAlpha, BodyPartFlags != ST_WIELDED ? ARMOR_OUTLINE_PRIORITY : AVERAGE_PRIORITY);

      if(SparklePos.X != SPARKLE_POS_X_ERROR)
	Bitmap->CreateSparkle(SparklePos + GI.Position, GI.SparkleFrame);

      if(GI.FlyAmount)
	Bitmap->CreateFlies(GI.Seed, Frame, GI.FlyAmount);

      const int WobbleData = GI.WobbleData;

      if(WobbleData & WOBBLE)
	{
	  int Speed = (WobbleData & WOBBLE_SPEED_RANGE) >> WOBBLE_SPEED_SHIFT;
	  int Freq = (WobbleData & WOBBLE_FREQ_RANGE) >> WOBBLE_FREQ_SHIFT;
	  int WobbleMask = 7 >> Freq << (6 - Speed);

	  if(!(Frame & WobbleMask))
	    Bitmap->Wobble(Frame & ((1 << (6 - Speed)) - 1), Speed, !!(WobbleData & WOBBLE_HORIZONTALLY_BIT));
	}

      if(SpecialFlags & ST_FLAMES)
	{
	  ulong SeedNFlags = SpecialFlags >> ST_FLAME_SHIFT & 3 | GI.Seed << 4;
	  Bitmap->CreateFlames(RawBitmap, RawPos - GI.Position, SeedNFlags, Frame);
	}

      if(SpecialFlags & ST_LIGHTNING && !((Frame + 1) & 7))
	Bitmap->CreateLightning(GI.Seed + Frame, WHITE);

      return TileMap.insert(std::pair<graphicid, tile>(GI, tile(Bitmap))).first;
    }
}

void igraph::EditBodyPartTile(rawbitmap* Source, rawbitmap* Dest, vector2d Pos, int BodyPartFlags)
{
  if(BodyPartFlags == ST_RIGHT_ARM)
    Source->NormalBlit(Dest, Pos.X, Pos.Y, 0, 0, 8, 16);
  else if(BodyPartFlags == ST_LEFT_ARM)
    Source->NormalBlit(Dest, Pos.X + 8, Pos.Y, 8, 0, 8, 16);
  else if(BodyPartFlags == ST_GROIN)
    {
      Source->NormalBlit(Dest, Pos.X, Pos.Y + 8, 0, 8, 16, 2);
      int y, i;

      for(y = 10, i = 0; y < 13; ++y)
	for(int x = y - 5; x < 20 - y; ++x)
	  Dest->PutPixel(x, y, Source->GetPixel(Pos.X + x, Pos.Y + y));
    }
  else if(BodyPartFlags == ST_RIGHT_LEG)
    {
      /* Right leg from the character's, NOT the player's point of view */

      Source->NormalBlit(Dest, Pos.X, Pos.Y + 10, 0, 10, 8, 6);
      Dest->PutPixel(5, 10, TRANSPARENT_PALETTE_INDEX);
      Dest->PutPixel(6, 10, TRANSPARENT_PALETTE_INDEX);
      Dest->PutPixel(7, 10, TRANSPARENT_PALETTE_INDEX);
      Dest->PutPixel(6, 11, TRANSPARENT_PALETTE_INDEX);
      Dest->PutPixel(7, 11, TRANSPARENT_PALETTE_INDEX);
      Dest->PutPixel(7, 12, TRANSPARENT_PALETTE_INDEX);
    }
  else if(BodyPartFlags == ST_LEFT_LEG)
    {
      /* Left leg from the character's, NOT the player's point of view */

      Source->NormalBlit(Dest, Pos.X + 8, Pos.Y + 10, 8, 10, 8, 6);
      Dest->PutPixel(8, 10, TRANSPARENT_PALETTE_INDEX);
      Dest->PutPixel(9, 10, TRANSPARENT_PALETTE_INDEX);
      Dest->PutPixel(8, 11, TRANSPARENT_PALETTE_INDEX);
    }
}

vector2d igraph::RotateTile(rawbitmap* Source, rawbitmap* Dest, vector2d Pos, vector2d SparklePos, int RotateFlags)
{
  Source->NormalBlit(Dest, Pos.X, Pos.Y, 0, 0, 16, 16, RotateFlags);

  if(SparklePos.X != SPARKLE_POS_X_ERROR)
    {
      if(RotateFlags & ROTATE)
	{
	  const int T = SparklePos.X;
	  SparklePos.X = 15 - SparklePos.Y;
	  SparklePos.Y = T;
	}

      if(RotateFlags & MIRROR)
	SparklePos.X = 15 - SparklePos.X;

      if(RotateFlags & FLIP)
	SparklePos.Y = 15 - SparklePos.Y;
    }

  return SparklePos;
}

void igraph::RemoveUser(tilemap::iterator Iterator)
{
  tile& Tile = Iterator->second;

  if(!--Tile.Users)
    {
      delete Tile.Bitmap;
      TileMap.erase(Iterator);
    }
}

outputfile& operator<<(outputfile& SaveFile, const graphicid& Value)
{
  SaveFile.Write(reinterpret_cast<const char*>(&Value), sizeof(Value));
  return SaveFile;
}

inputfile& operator>>(inputfile& SaveFile, graphicid& Value)
{
  SaveFile.Read(reinterpret_cast<char*>(&Value), sizeof(Value));
  return SaveFile;
}

graphicdata::~graphicdata()
{
  if(AnimationFrames)
    {
      for(int c = 0; c < AnimationFrames; ++c)
	igraph::RemoveUser(GraphicIterator[c]);

      delete [] Picture;
      delete [] GraphicIterator;
    }
}

void graphicdata::Save(outputfile& SaveFile) const
{
  SaveFile << (int)AnimationFrames;

  for(int c = 0; c < AnimationFrames; ++c)
    SaveFile << GraphicIterator[c]->first;
}

void graphicdata::Load(inputfile& SaveFile)
{
  SaveFile >> (int&)AnimationFrames;

  if(AnimationFrames)
    {
      Picture = new bitmap*[AnimationFrames];
      GraphicIterator = new tilemap::iterator[AnimationFrames];
      graphicid GraphicID;

      for(int c = 0; c < AnimationFrames; ++c)
	{
	  SaveFile >> GraphicID;
	  Picture[c] = (GraphicIterator[c] = igraph::AddUser(GraphicID))->second.Bitmap;
	}
    }
}

outputfile& operator<<(outputfile& SaveFile, const graphicdata& Data)
{
  Data.Save(SaveFile);
  return SaveFile;
}

inputfile& operator>>(inputfile& SaveFile, graphicdata& Data)
{
  Data.Load(SaveFile);
  return SaveFile;
}

void graphicdata::Retire()
{
  if(AnimationFrames)
    {
      for(int c = 0; c < AnimationFrames; ++c)
	igraph::RemoveUser(GraphicIterator[c]);

      AnimationFrames = 0;
      delete [] Picture;
      delete [] GraphicIterator;
    }
}

const int* igraph::GetBodyBitmapValidityMap(int SpecialFlags)
{
  return BodyBitmapValidityMap[(SpecialFlags & 0x38) >> 3];
}

void igraph::CreateBodyBitmapValidityMaps()
{
  memset(BodyBitmapValidityMap[0], 0xFF, 128 * sizeof(int));
  int* Map = BodyBitmapValidityMap[ST_RIGHT_ARM >> 3];
  int x, y;

  for(x = 8; x < 16; ++x)
    Map[x] = 0;

  Map = BodyBitmapValidityMap[ST_LEFT_ARM >> 3];

  for(x = 0; x < 8; ++x)
    Map[x] = 0;

  Map = BodyBitmapValidityMap[ST_GROIN >> 3];
  memset(Map, 0, 16 * sizeof(int));

  for(y = 10; y < 13; ++y)
    for(x = y - 5; x < 20 - y; ++x)
      Map[x] |= 1 << y;

  Map = BodyBitmapValidityMap[ST_RIGHT_LEG >> 3];

  for(x = 8; x < 16; ++x)
    Map[x] = 0;

  Map[5] &= ~(1 << 10);
  Map[6] &= ~(1 << 10);
  Map[7] &= ~(1 << 10);
  Map[6] &= ~(1 << 11);
  Map[7] &= ~(1 << 11);
  Map[7] &= ~(1 << 12);

  Map = BodyBitmapValidityMap[ST_LEFT_LEG >> 3];

  for(x = 0; x < 8; ++x)
    Map[x] = 0;

  Map[8] &= ~(1 << 10);
  Map[9] &= ~(1 << 10);
  Map[8] &= ~(1 << 11);
}

void igraph::LoadMenu()
{
  Menu = new bitmap(game::GetGameDir() + "Graphics/Menu.pcx");
}

void igraph::UnLoadMenu()
{
  delete Menu;
}

void igraph::CreateSilhouetteCaches()
{
  int BodyPartSilhouetteMColorIndex[HUMANOID_BODYPARTS] = { 3, 0, 1, 2, 1, 2, 3 };
  color24 ConditionColor[CONDITION_COLORS] = { MakeRGB16(48, 48, 48),
					       MakeRGB16(120, 0, 0),
					       MakeRGB16(180, 0, 0),
					       MakeRGB16(180, 120, 120),
					       MakeRGB16(180, 180, 180) };
  int X = 8, Y = 64;

  for(int c1 = 0; c1 < HUMANOID_BODYPARTS; ++c1)
    {
      if(c1 == 4)
	X = 72;

      for(int c2 = 0; c2 < CONDITION_COLORS; ++c2)
	{
	  packedcolor16 Color[4] = { 0, 0, 0, 0 };
	  Color[BodyPartSilhouetteMColorIndex[c1]] = ConditionColor[c2];

	  for(int c3 = 0; c3 < SILHOUETTE_TYPES; ++c3)
	    {
	      SilhouetteCache[c1][c2][c3] = new bitmap(SILHOUETTE_X_SIZE,
						       SILHOUETTE_Y_SIZE, 0);
	      RawGraphic[GR_CHARACTER]->MaskedBlit(SilhouetteCache[c1][c2][c3],
						   X, Y, 0, 0,
						   SILHOUETTE_X_SIZE,
						   SILHOUETTE_Y_SIZE, Color);
	    }

	  SilhouetteCache[c1][c2][SILHOUETTE_INTER_LACED]->InterLace();
	}
    }
}

void igraph::CreateBackGround(int ColorType)
{
  if(CurrentColorType == ColorType)
    return;

  CurrentColorType = ColorType;
  delete BackGround;
  BackGround = new bitmap(RES_X, RES_Y);
  int Side = 1025;
  int** Map;
  Alloc2D(Map, Side, Side);
  femath::GenerateFractalMap(Map, Side, Side - 1, 1000);
  int XPlus = (1025 - RES_X) >> 1;
  int YPlus = (1025 - RES_Y) >> 1;

  for(int x = 0; x < RES_X; ++x)
    for(int y = 0; y < RES_Y; ++y)
      {
	int E = Limit<int>(abs(Map[1024 - x][1024 - (RES_Y - y)]) / 30, 0, 75);
	BackGround->PutPixel(x, y, GetBackGroundColor(E));
     }

  delete [] Map;
}

color16 igraph::GetBackGroundColor(int Element)
{
  switch(CurrentColorType)
    {
    case GRAY_FRACTAL: return MakeRGB16(Element, Element, Element);
    case RED_FRACTAL: return MakeRGB16(Element, Element / 3, Element / 3);
    case GREEN_FRACTAL: return MakeRGB16(Element / 3, Element, Element / 3);
    case BLUE_FRACTAL: return MakeRGB16(Element / 3, Element / 3, Element);
    case YELLOW_FRACTAL: return MakeRGB16(Element, Element, Element / 3);
    }

  return 0;
}

void igraph::BlitBackGround(int X, int Y, int Width, int Height)
{
  BackGround->NormalBlit(DOUBLE_BUFFER, X, Y, X, Y, Width, Height);
}
