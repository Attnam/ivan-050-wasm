#include <cstdarg>

#include "colorbit.h"
#include "bitmap.h"
#include "save.h"
#include "femath.h"

void colorizablebitmap::MaskedBlit(bitmap* Bitmap, vector2d Source, ushort DestX, ushort DestY, ushort Width, ushort Height, ushort* Color) const { MaskedBlit(Bitmap, Source.X, Source.Y, DestX, DestY, Width, Height, Color); }
void colorizablebitmap::MaskedBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, vector2d Dest, ushort Width, ushort Height, ushort* Color) const { MaskedBlit(Bitmap, SourceX, SourceY, Dest.X, Dest.Y, Width, Height, Color); }
void colorizablebitmap::MaskedBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, ushort DestX, ushort DestY, vector2d BlitSize, ushort* Color) const { MaskedBlit(Bitmap, SourceX, SourceY, DestX, DestY, BlitSize.X, BlitSize.Y, Color); }
void colorizablebitmap::MaskedBlit(bitmap* Bitmap, vector2d Source, vector2d Dest, ushort Width, ushort Height, ushort* Color) const { MaskedBlit(Bitmap, Source.X, Source.Y, Dest.X, Dest.Y, Width, Height, Color); }
void colorizablebitmap::MaskedBlit(bitmap* Bitmap, vector2d Source, ushort DestX, ushort DestY, vector2d BlitSize, ushort* Color) const { MaskedBlit(Bitmap, Source.X, Source.Y, DestX, DestY, BlitSize.X, BlitSize.Y, Color); }
void colorizablebitmap::MaskedBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, vector2d Dest, vector2d BlitSize, ushort* Color) const { MaskedBlit(Bitmap, SourceX, SourceY, Dest.X, Dest.Y, BlitSize.X, BlitSize.Y, Color); }
void colorizablebitmap::MaskedBlit(bitmap* Bitmap, vector2d Source, vector2d Dest, vector2d BlitSize, ushort* Color) const  { MaskedBlit(Bitmap, Source.X, Source.Y, Dest.X, Dest.Y, BlitSize.X, BlitSize.Y, Color); }
void colorizablebitmap::MaskedBlit(bitmap* Bitmap, ushort* Color) const { MaskedBlit(Bitmap, 0, 0, 0, 0, XSize, YSize, Color); }

void colorizablebitmap::AlterGradient(vector2d Pos, ushort Width, ushort Height, uchar MColor, char Amount, bool Clip) { AlterGradient(Pos.X, Pos.Y, Width, Height, MColor, Amount, Clip); }
void colorizablebitmap::AlterGradient(ushort X, ushort Y, vector2d AlterSize, uchar MColor, char Amount, bool Clip) { AlterGradient(X, Y, AlterSize.X, AlterSize.Y, MColor, Amount, Clip); }
void colorizablebitmap::AlterGradient(vector2d Pos, vector2d AlterSize, uchar MColor, char Amount, bool Clip) { AlterGradient(Pos.X, Pos.Y, AlterSize.X, AlterSize.Y, MColor, Amount, Clip); }

void colorizablebitmap::SwapColors(vector2d Pos, ushort Width, ushort Height, uchar Color1, uchar Color2) { SwapColors(Pos.X, Pos.Y, Width, Height, Color1, Color2); }
void colorizablebitmap::SwapColors(ushort X, ushort Y, vector2d AlterSize, uchar Color1, uchar Color2) { SwapColors(X, Y, AlterSize.X, AlterSize.Y, Color1, Color2); }
void colorizablebitmap::SwapColors(vector2d Pos, vector2d AlterSize, uchar Color1, uchar Color2) { SwapColors(Pos.X, Pos.Y, AlterSize.X, AlterSize.Y, Color1, Color2); }

void colorizablebitmap::Roll(vector2d Pos, ushort Width, ushort Height, short XMove, short YMove) { Roll(Pos.X, Pos.Y, Width, Height, XMove, YMove); }
void colorizablebitmap::Roll(ushort X, ushort Y, vector2d BlitSize, short XMove, short YMove) { Roll(X, Y, BlitSize.X, BlitSize.Y, XMove, YMove); }
void colorizablebitmap::Roll(ushort X, ushort Y, ushort Width, ushort Height, vector2d Move) { Roll(X, Y, Width, Height, Move.X, Move.Y); }
void colorizablebitmap::Roll(vector2d Pos, vector2d BlitSize, short XMove, short YMove) { Roll(Pos.X, Pos.Y, BlitSize.X, BlitSize.Y, XMove, YMove); }
void colorizablebitmap::Roll(vector2d Pos, ushort Width, ushort Height, vector2d Move) { Roll(Pos.X, Pos.Y, Width, Height, Move.X, Move.Y); }
void colorizablebitmap::Roll(ushort X, ushort Y, vector2d BlitSize, vector2d Move) { Roll(X, Y, BlitSize.X, BlitSize.Y, Move.X, Move.Y); }
void colorizablebitmap::Roll(vector2d Pos, vector2d BlitSize, vector2d Move) { Roll(Pos.X, Pos.Y, BlitSize.X, BlitSize.Y, Move.X, Move.Y); }

colorizablebitmap::colorizablebitmap(const festring& FileName)
{
  inputfile File(FileName.CStr(), 0, false);

  if(!File.IsOpen())
    ABORT("Bitmap %s not found!", FileName.CStr());

  File.SeekPosEnd(-768);
  Palette = new uchar[768];
  File.Read(reinterpret_cast<char*>(Palette), 768);
  File.SeekPosBegin(8);
  XSize  =  File.Get();
  XSize += (File.Get() << 8) + 1;
  YSize  =  File.Get();
  YSize += (File.Get() << 8) + 1;
  File.SeekPosBegin(128);
  PaletteBuffer = new uchar[XSize * YSize];
  uchar* Buffer = PaletteBuffer;

  while(ulong(Buffer) != ulong(PaletteBuffer) + XSize * YSize)
    {
      int Char1 = File.Get();

      if(Char1 > 192)
	{
	  int Char2 = File.Get();

	  for(; Char1 > 192; Char1--)
	    *(Buffer++) = Char2;
	}
      else
	*(Buffer++) = Char1;
    }
}

colorizablebitmap::~colorizablebitmap()
{
  delete [] Palette;
  delete [] PaletteBuffer;

  for(fontcache::iterator i = FontCache.begin(); i != FontCache.end(); ++i)
    {
      delete i->second.first;
      delete i->second.second;
    }
}

/* A lousy bitmap saver that uses the pcx format but doesn't do any compression. */

void colorizablebitmap::Save(const festring& FileName)
{
  char PCXHeader[128];
  memset(PCXHeader, 0, 128);
  *((ulong*)PCXHeader) = 0x0801050A;
  PCXHeader[65] = 0x01;
  PCXHeader[66] = XSize & 0xFF;
  PCXHeader[67] = (XSize >> 8) & 0xFF;
  PCXHeader[0x08] = (XSize - 1) & 0xFF;
  PCXHeader[0x09] = ((XSize - 1) >> 8) & 0xFF;
  PCXHeader[0x0A] = (YSize - 1) & 0xFF;
  PCXHeader[0x0B] = ((YSize - 1) >> 8) & 0xFF;
  outputfile SaveFile(FileName);
  SaveFile.Write(PCXHeader, 128);
  uchar* Buffer = PaletteBuffer;

  while(ulong(Buffer) != ulong(PaletteBuffer) + XSize * YSize)
    {
      uchar Char = *(Buffer++);

      if(Char >= 192)
	SaveFile.Put(uchar(193));

      SaveFile.Put(Char);
    }

  SaveFile.Write(reinterpret_cast<char*>(Palette), 768);
}

void colorizablebitmap::MaskedBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, ushort DestX, ushort DestY, ushort Width, ushort Height, ushort* Color) const
{
  if(!femath::Clip(SourceX, SourceY, DestX, DestY, Width, Height, XSize, YSize, Bitmap->GetXSize(), Bitmap->GetYSize()))
    return;

  uchar* Buffer = reinterpret_cast<uchar*>(ulong(PaletteBuffer) + ulong(SourceY) * XSize);
  ulong DestBuffer = ulong(Bitmap->GetImage()[DestY]);

  for(ushort y = 0; y < Height; ++y)
    {
      for(ushort x = 0; x < Width; ++x)
	{
	  uchar PaletteElement = Buffer[SourceX + x];

	  if(PaletteElement >= 192)
	    {
	      ushort ThisColor = Color[(PaletteElement - 192) >> 4];
	      ushort Index = PaletteElement & 15;
	      ushort Red = (GetRed16(ThisColor) * Index) >> 3;
	      ushort Blue = (GetBlue16(ThisColor) * Index) >> 3;
	      ushort Green = (GetGreen16(ThisColor) * Index) >> 3;
	      reinterpret_cast<ushort*>(DestBuffer)[DestX + x] = MakeRGB16(Red < 256 ? Red : 255, Green < 256 ? Green : 255, Blue < 256 ? Blue : 255);
	    }
	  else
	    {
	      ushort ThisColor = ((Palette[PaletteElement + (PaletteElement << 1)] >> 3) << 11) | ((Palette[PaletteElement + (PaletteElement << 1) + 1] >> 2) << 5) | (Palette[PaletteElement + (PaletteElement << 1) + 2] >> 3);

	      if(ThisColor != TRANSPARENT_COLOR)
		reinterpret_cast<ushort*>(DestBuffer)[DestX + x] = ThisColor;
	    }
	}

      DestBuffer += Bitmap->GetXSize() << 1;
      Buffer = reinterpret_cast<uchar*>(ulong(Buffer) + XSize);
    }
}

bitmap* colorizablebitmap::Colorize(const ushort* Color, uchar BaseAlpha, const uchar* Alpha) const
{
  bitmap* Bitmap = new bitmap(XSize, YSize);
  uchar* Buffer = PaletteBuffer;
  ulong DestBuffer = ulong(Bitmap->GetImage()[0]);
  ulong AlphaMap;
  bool UseAlpha;

  if(BaseAlpha != 255 || (Alpha && (Alpha[0] != 255 || Alpha[1] != 255 || Alpha[2] != 255 || Alpha[3] != 255)))
    {
      Bitmap->CreateAlphaMap(BaseAlpha);
      AlphaMap = ulong(Bitmap->GetAlphaMap()[0]);
      UseAlpha = true;
    }
  else
    {
      AlphaMap = 0;
      UseAlpha = false;
    }

  for(ushort y = 0; y < YSize; ++y)
    {
      for(ushort x = 0; x < XSize; ++x)
	{
	  if(Buffer[x] >= 192)
	    {
	      ushort ThisColor = Color[(Buffer[x] - 192) >> 4];

	      if(ThisColor != TRANSPARENT_COLOR)
		{
		  ushort Index = Buffer[x] & 15;
		  ushort Red = (GetRed16(ThisColor) * Index) >> 3;
		  ushort Blue = (GetBlue16(ThisColor) * Index) >> 3;
		  ushort Green = (GetGreen16(ThisColor) * Index) >> 3;
		  reinterpret_cast<ushort*>(DestBuffer)[x] = MakeRGB16(Red < 256 ? Red : 255, Green < 256 ? Green : 255, Blue < 256 ? Blue : 255);

		  if(UseAlpha)
		    reinterpret_cast<uchar*>(AlphaMap)[x] = Alpha[(Buffer[x] - 192) >> 4];
		}
	      else
		reinterpret_cast<ushort*>(DestBuffer)[x] = TRANSPARENT_COLOR;
	    }
	  else
	    reinterpret_cast<ushort*>(DestBuffer)[x] = ((Palette[Buffer[x] + (Buffer[x] << 1)] >> 3) << 11) | ((Palette[Buffer[x] + (Buffer[x] << 1) + 1] >> 2) << 5) | (Palette[Buffer[x] + (Buffer[x] << 1) + 2] >> 3);
	}

      DestBuffer += (Bitmap->GetXSize() << 1);
      AlphaMap += Bitmap->GetXSize();
      Buffer = reinterpret_cast<uchar*>(ulong(Buffer) + XSize);
    }

  return Bitmap;
}

bitmap* colorizablebitmap::Colorize(vector2d Pos, vector2d Size, const ushort* Color, uchar BaseAlpha, const uchar* Alpha) const
{
  bitmap* Bitmap = new bitmap(Size.X, Size.Y);
  uchar* Buffer = reinterpret_cast<uchar*>(ulong(PaletteBuffer) + ulong(Pos.Y) * XSize);
  ulong DestBuffer = ulong(Bitmap->GetImage()[0]);
  ulong AlphaMap;
  bool UseAlpha;

  if(BaseAlpha != 255 || (Alpha && (Alpha[0] != 255 || Alpha[1] != 255 || Alpha[2] != 255 || Alpha[3] != 255)))
    {
      Bitmap->CreateAlphaMap(BaseAlpha);
      AlphaMap = ulong(Bitmap->GetAlphaMap()[0]);
      UseAlpha = true;
    }
  else
    {
      AlphaMap = 0;
      UseAlpha = false;
    }

  for(ushort y = 0; y < Size.Y; ++y)
    {
      for(ushort x = 0; x < Size.X; ++x)
	{
	  uchar PaletteElement = Buffer[Pos.X + x];

	  if(PaletteElement >= 192)
	    {
	      ushort ThisColor = Color[(PaletteElement - 192) >> 4];

	      if(ThisColor != TRANSPARENT_COLOR)
		{
		  ushort Index = PaletteElement & 15;
		  ushort Red = (GetRed16(ThisColor) * Index) >> 3;
		  ushort Blue = (GetBlue16(ThisColor) * Index) >> 3;
		  ushort Green = (GetGreen16(ThisColor) * Index) >> 3;
		  reinterpret_cast<ushort*>(DestBuffer)[x] = MakeRGB16(Red < 256 ? Red : 255, Green < 256 ? Green : 255, Blue < 256 ? Blue : 255);

		  if(UseAlpha)
		    reinterpret_cast<uchar*>(AlphaMap)[x] = Alpha[(PaletteElement - 192) >> 4];
		}
	      else
		reinterpret_cast<ushort*>(DestBuffer)[x] = TRANSPARENT_COLOR;
	    }
	  else
	    reinterpret_cast<ushort*>(DestBuffer)[x] = ((Palette[PaletteElement + (PaletteElement << 1)] >> 3) << 11) | ((Palette[PaletteElement + (PaletteElement << 1) + 1] >> 2) << 5) | (Palette[PaletteElement + (PaletteElement << 1) + 2] >> 3);
	}

      DestBuffer += (Bitmap->GetXSize() << 1);
      AlphaMap += Bitmap->GetXSize();
      Buffer = reinterpret_cast<uchar*>(ulong(Buffer) + XSize);
    }
  return Bitmap;
}

void colorizablebitmap::Printf(bitmap* Bitmap, ushort X, ushort Y, ushort Color, const char* Format, ...) const
{
  char Buffer[256];

  va_list AP;
  va_start(AP, Format);
  vsprintf(Buffer, Format, AP);
  va_end(AP);

  fontcache::const_iterator Iterator = FontCache.find(Color);

  if(Iterator == FontCache.end())
    {
      ushort ShadeCol = MakeShadeColor(Color);

      for(ushort c = 0; c < strlen(Buffer); ++c)
	{
	  ushort FX = ((Buffer[c] - 0x20) & 0xF) << 4, FY = (Buffer[c] - 0x20) & 0xF0;
	  MaskedBlit(Bitmap, FX, FY, X + (c << 3) + 1, Y + 1, 8, 8, &ShadeCol);
	  MaskedBlit(Bitmap, FX, FY, X + (c << 3), Y, 8, 8, &Color);
	}
    }
  else
    {
      for(ushort c = 0; c < strlen(Buffer); ++c)
	Iterator->second.first->MaskedBlit(Bitmap, ((Buffer[c] - 0x20) & 0xF) << 4, (Buffer[c] - 0x20) & 0xF0, X + (c << 3), Y, 9, 9);
    }
}

void colorizablebitmap::PrintfShade(bitmap* Bitmap, ushort X, ushort Y, ushort Color, const char* Format, ...) const
{
  ++X; ++Y;
  char Buffer[256];

  va_list AP;
  va_start(AP, Format);
  vsprintf(Buffer, Format, AP);
  va_end(AP);

  fontcache::const_iterator Iterator = FontCache.find(Color);

  if(Iterator == FontCache.end())
    {
      Color = MakeShadeColor(Color);

      for(ushort c = 0; c < strlen(Buffer); ++c)
	MaskedBlit(Bitmap, ((Buffer[c] - 0x20) & 0xF) << 4, (Buffer[c] - 0x20) & 0xF0, X + (c << 3), Y, 8, 8, &Color);
    }
  else
    {
      for(ushort c = 0; c < strlen(Buffer); ++c)
	Iterator->second.second->MaskedBlit(Bitmap, ((Buffer[c] - 0x20) & 0xF) << 4, (Buffer[c] - 0x20) & 0xF0, X + (c << 3), Y, 8, 8);
    }
}

void colorizablebitmap::AlterGradient(ushort X, ushort Y, ushort Width, ushort Height, uchar MColor, char Amount, bool Clip)
{
  uchar ColorMin = 192 + (MColor << 4);
  uchar ColorMax = 207 + (MColor << 4);

  if(Clip)
    {
      for(ushort x = X; x < X + Width; ++x)
	for(ushort y = Y; y < Y + Height; ++y)
	  {
	    uchar Pixel = PaletteBuffer[y * XSize + x];

	    if(Pixel >= ColorMin && Pixel <= ColorMax)
	      {
		ushort NewPixel = ushort(Pixel) + Amount;

		if(NewPixel < ColorMin)
		  NewPixel = ColorMin;

		if(NewPixel > ColorMax)
		  NewPixel = ColorMax;

		PaletteBuffer[y * XSize + x] = NewPixel;
	      }
	  }
    }
  else
    {
      ushort x;

      for(x = X; x < X + Width; ++x)
	for(ushort y = Y; y < Y + Height; ++y)
	  {
	    uchar Pixel = PaletteBuffer[y * XSize + x];

	    if(Pixel >= ColorMin && Pixel <= ColorMax)
	      {
		ushort NewPixel = ushort(Pixel) + Amount;

		if(NewPixel < ColorMin)
		  return;

		if(NewPixel > ColorMax)
		  return;
	      }
	  }

      for(x = X; x < X + Width; ++x)
	for(ushort y = Y; y < Y + Height; ++y)
	  {
	    uchar Pixel = PaletteBuffer[y * XSize + x];

	    if(Pixel >= ColorMin && Pixel <= ColorMax)
	      PaletteBuffer[y * XSize + x] = Pixel + Amount;
	  }
    }
}

void colorizablebitmap::SwapColors(ushort X, ushort Y, ushort Width, ushort Height, uchar Color1, uchar Color2)
{
  if(Color1 > 3 || Color2 > 3)
    ABORT("Illgal color swap!");

  for(ushort x = X; x < X + Width; ++x)
    for(ushort y = Y; y < Y + Height; ++y)
      {
	uchar& Pixel = PaletteBuffer[y * XSize + x];

	if(Pixel >= 192 + (Color1 << 4) && Pixel <= 207 + (Color1 << 4))
	  Pixel += (Color2 - Color1) << 4;
	else if(Pixel >= 192 + (Color2 << 4) && Pixel <= 207 + (Color2 << 4))
	  Pixel += (Color1 - Color2) << 4;
      }
}

void colorizablebitmap::Roll(ushort X, ushort Y, ushort Width, ushort Height, short XMove, short YMove)
{
  if(!XMove && !YMove)
    return;

  ushort x, y;
  uchar* TempBuffer = new uchar[Width * Height];

  for(x = X; x < X + Width; ++x)
    for(y = Y; y < Y + Height; ++y)
      {
	short XPos = x + XMove, YPos = y + YMove;

	while(XPos < X)
	  XPos += Width;

	while(YPos < Y)
	  YPos += Height;

	while(XPos >= X + Width)
	  XPos -= Width;

	while(YPos >= Y + Height)
	  YPos -= Height;

	TempBuffer[(YPos - Y) * Width + XPos - X] = PaletteBuffer[y * XSize + x];
      }

  for(x = X; x < X + Width; ++x)
    for(y = Y; y < Y + Height; ++y)
      PaletteBuffer[y * XSize + x] = TempBuffer[(y - Y) * Width + x - X];

  delete [] TempBuffer;
}

void colorizablebitmap::CreateFontCache(ushort Color)
{
  if(FontCache.find(Color) != FontCache.end())
    return;

  ushort ShadeColor = MakeShadeColor(Color);
  bitmap* Font = new bitmap(XSize, YSize);
  Font->Fill(0, 0, 1, YSize, TRANSPARENT_COLOR);
  Font->Fill(0, 0, XSize, 1, TRANSPARENT_COLOR);
  bitmap* ShadeFont = Colorize(&ShadeColor);
  ShadeFont->Blit(Font, 0, 0, 1, 1, XSize - 1, YSize - 1);
  MaskedBlit(Font, &Color);
  FontCache[Color] = std::pair<bitmap*, bitmap*>(Font, ShadeFont);
}

/* returns ERROR_VECTOR if fails find Pos else returns pos */

vector2d colorizablebitmap::RandomizeSparklePos(vector2d Pos, vector2d Size, bool* Sparkling) const
{
  bool SparklingReally = false;

  for(ushort c = 0; c < 4; ++c)
    if(Sparkling[c])
      SparklingReally = true;

  if(!SparklingReally)
    return ERROR_VECTOR;

  vector2d* PreferredPossible = new vector2d[(Size.X - 8) * (Size.Y - 8)];
  vector2d* BadPossible = new vector2d[((Size.X + Size.Y) << 3) - 64];
  ushort Preferred = 0;
  ushort Bad = 0;
  ushort XMax = Pos.X + Size.X;
  ushort YMax = Pos.Y + Size.Y;

  for(ushort x = Pos.X; x < XMax; ++x)
    for(ushort y = Pos.Y; y < YMax; ++y)
      {
	uchar Entry = GetPaletteEntry(x, y);

	if(IsMaterialColor(Entry) && Sparkling[GetMaterialColorIndex(Entry)])
	  if(x >= Pos.X + 4 && x < XMax - 4 && y >= Pos.Y + 4 && y < YMax - 4)
	    PreferredPossible[Preferred++] = vector2d(x, y);
	  else
	    BadPossible[Bad++] = vector2d(x, y);
      }

  vector2d Return;

  if(Preferred)
    Return = PreferredPossible[RAND() % Preferred] - Pos;
  else if(Bad)
    Return = BadPossible[RAND() % Bad] - Pos;
  else
    Return = ERROR_VECTOR;

  delete [] PreferredPossible;
  delete [] BadPossible;
  return Return;
}
