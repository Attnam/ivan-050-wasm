#include "bitmap.h"
#include "graphics.h"
#include "igraph.h"
#include "game.h"
#include "error.h"
#include "colorbit.h"

colorizablebitmap* igraph::RawGraphic[RAW_TYPES];
bitmap* igraph::Graphic[GRAPHIC_TYPES];
bitmap* igraph::TileBuffer;
bitmap* igraph::OutlineBuffer;
char* igraph::RawGraphicFileName[] = { "Graphics/LTerrain.pcx", "Graphics/Item.pcx", "Graphics/Char.pcx" };
char* igraph::GraphicFileName[] = { "Graphics/Human.pcx", "Graphics/WTerrain.pcx", "Graphics/FOW.pcx", "Graphics/Cursor.pcx", "Graphics/Symbol.pcx" };
tilemap igraph::TileMap;

void igraph::Init(HINSTANCE hInst, HWND* hWnd)
{
	static bool AlreadyInstalled = false;

	if(!AlreadyInstalled)
	{
		AlreadyInstalled = true;

		graphics::SetMode(hInst, hWnd, "IVAN 0.240a", 800, 600, 16, false);
		graphics::LoadDefaultFont("Graphics/Font.pcx");

		uchar c;

		for(c = 0; c < RAW_TYPES; ++c)
			RawGraphic[c] = new colorizablebitmap(RawGraphicFileName[c]);

		for(c = 0; c < GRAPHIC_TYPES; ++c)
			Graphic[c] = new bitmap(GraphicFileName[c]);

		TileBuffer = new bitmap(16, 16);
		OutlineBuffer = new bitmap(16, 16);
		OutlineBuffer->ClearToColor(0xF81F);

		atexit(igraph::DeInit);
	}
}

void igraph::DeInit()
{
	uchar c;

	for(c = 0; c < RAW_TYPES; ++c)
		delete RawGraphic[c];

	for(c = 0; c < GRAPHIC_TYPES; ++c)
		delete Graphic[c];
}

void igraph::BlitTileBuffer(vector2d Pos, ushort Luminance)
{
	Luminance = ushort(Luminance * game::GetSoftGamma());

	if(Luminance == 256)
		TileBuffer->Blit(DOUBLEBUFFER, 0, 0, Pos.X, Pos.Y, 16, 16);
	else
		TileBuffer->Blit(DOUBLEBUFFER, 0, 0, Pos.X, Pos.Y, 16, 16, Luminance);
}

void igraph::DrawCursor(vector2d Pos)
{
	ushort Luminance = ushort(256 * game::GetSoftGamma());

	igraph::GetCursorGraphic()->MaskedBlit(DOUBLEBUFFER, 0, 0, Pos.X, Pos.Y, 16, 16, Luminance);
}

tile igraph::GetTile(graphic_id GI)
{
	tilemap::iterator Iterator = TileMap.find(GI);

	if(Iterator == TileMap.end())
		ABORT("Art stolen!");

	return Iterator->second;
}

tile igraph::AddUser(graphic_id GI)
{
	tilemap::iterator Iterator = TileMap.find(GI);

	if(Iterator != TileMap.end())
		++Iterator->second.Users;
	else
	{
		bitmap* Bitmap = RawGraphic[GI.FileIndex]->Colorize(GI.BitmapPos, vector2d(16, 16), GI.Color);

		tile Tile(Bitmap, 1);

		graphic_id NewGI(GI.BitmapPos, new ushort[4], GI.FileIndex);

		for(uchar c = 0; c < 4; ++c)
			NewGI.Color[c] = GI.Color[c];

		TileMap[NewGI] = Tile;

		return Tile;
	}

	return Iterator->second;
}

void igraph::RemoveUser(graphic_id GI)
{
	tilemap::iterator Iterator = TileMap.find(GI);

	if(Iterator != TileMap.end())
		if(!--Iterator->second.Users)
		{
			delete Iterator->second.Bitmap;
			delete [] Iterator->first.Color;
			TileMap.erase(Iterator);
		}
}
