#ifndef __BITMAP_H__
#define __BITMAP_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include <string>

#include "vector2d.h"
#include "felibdef.h"

class outputfile;
class inputfile;

class bitmap
{
 public:
  friend class graphics;
  friend class colorizablebitmap;
  bitmap(const std::string&);
  bitmap(bitmap*, uchar = 0);
  bitmap(ushort, ushort);
  bitmap(vector2d);
  bitmap(ushort, ushort, ushort);
  bitmap(vector2d, ushort);
  bitmap(bitmap*, ushort, ushort, ushort, ushort);
  bitmap(bitmap*, vector2d, vector2d);
  ~bitmap();
  void Save(outputfile&) const;
  void Load(inputfile&);
  void Save(const std::string&) const;
  void ResetAlpha(ushort, ushort);
  void PutPixel(ushort, ushort, ushort);
  void PutPixel(vector2d Pos, ushort Color) { PutPixel(Pos.X, Pos.Y, Color); }
  void PutPixelAndResetAlpha(ushort X, ushort Y, ushort Color) { PutPixel(X, Y, Color); ResetAlpha(X, Y); }
  void PutPixelAndResetAlpha(vector2d Pos, ushort Color) { PutPixel(Pos.X, Pos.Y, Color); ResetAlpha(Pos.X, Pos.Y); }
  void SafePutPixel(ushort X, ushort Y, ushort Color) { if(X < XSize && Y < YSize) PutPixel(X, Y, Color); }
  void SafePutPixel(vector2d Pos, ushort Color) { if(Pos.X < XSize && Pos.Y < YSize) PutPixel(Pos.X, Pos.Y, Color); }
  void SafePutPixelAndResetAlpha(ushort X, ushort Y, ushort Color) { if(X < XSize && Y < YSize) PutPixelAndResetAlpha(X, Y, Color); }
  void SafePutPixelAndResetAlpha(vector2d Pos, ushort Color) { if(Pos.X < XSize && Pos.Y < YSize) PutPixelAndResetAlpha(Pos.X, Pos.Y, Color); }
  ushort GetPixel(ushort, ushort) const;
  ushort GetPixel(vector2d Pos) const { return GetPixel(Pos.X, Pos.Y); }

  void Fill(ushort, ushort, ushort, ushort, ushort);
  void Fill(vector2d TopLeft, ushort Width, ushort Height, ushort Color) { Fill(TopLeft.X, TopLeft.Y, Width, Height, Color); }
  void Fill(ushort X, ushort Y, vector2d FillSize, ushort Color) { Fill(X, Y, FillSize.X, FillSize.Y, Color); }
  void Fill(vector2d TopLeft, vector2d FillSize, ushort Color) { Fill(TopLeft.X, TopLeft.Y, FillSize.X, FillSize.Y, Color); }
  void Fill(ushort Color) { Fill(0, 0, XSize, YSize, Color); }

  void Blit(bitmap*, ushort, ushort, ushort, ushort, ushort, ushort, uchar = 0) const;
  void Blit(bitmap* Bitmap, vector2d Source, ushort DestX, ushort DestY, ushort Width, ushort Height, uchar Flags = 0) const { Blit(Bitmap, Source.X, Source.Y, DestX, DestY, Width, Height, Flags); }
  void Blit(bitmap* Bitmap, ushort SourceX, ushort SourceY, vector2d Dest, ushort Width, ushort Height, uchar Flags = 0) const { Blit(Bitmap, SourceX, SourceY, Dest.X, Dest.Y, Width, Height, Flags); }
  void Blit(bitmap* Bitmap, ushort SourceX, ushort SourceY, ushort DestX, ushort DestY, vector2d BlitSize, uchar Flags = 0) const { Blit(Bitmap, SourceX, SourceY, DestX, DestY, BlitSize.X, BlitSize.Y, Flags); }
  void Blit(bitmap* Bitmap, vector2d Source, vector2d Dest, ushort Width, ushort Height, uchar Flags = 0) const { Blit(Bitmap, Source.X, Source.Y, Dest.X, Dest.Y, Width, Height, Flags); }
  void Blit(bitmap* Bitmap, vector2d Source, ushort DestX, ushort DestY, vector2d BlitSize, uchar Flags = 0) const { Blit(Bitmap, Source.X, Source.Y, DestX, DestY, BlitSize.X, BlitSize.Y, Flags); }
  void Blit(bitmap* Bitmap, ushort SourceX, ushort SourceY, vector2d Dest, vector2d BlitSize, uchar Flags = 0) const { Blit(Bitmap, SourceX, SourceY, Dest.X, Dest.Y, BlitSize.X, BlitSize.Y, Flags); }
  void Blit(bitmap* Bitmap, vector2d Source, vector2d Dest, vector2d BlitSize, uchar Flags = 0) const  { Blit(Bitmap, Source.X, Source.Y, Dest.X, Dest.Y, BlitSize.X, BlitSize.Y, Flags); }
  void Blit(bitmap* Bitmap, uchar Flags = 0) const { Blit(Bitmap, 0, 0, 0, 0, XSize, YSize, Flags); }

  void Blit(bitmap*, ushort, ushort, ushort, ushort, ushort, ushort, ulong) const;
  void Blit(bitmap* Bitmap, vector2d Source, ushort DestX, ushort DestY, ushort Width, ushort Height, ulong Luminance) const { Blit(Bitmap, Source.X, Source.Y, DestX, DestY, Width, Height, Luminance); }
  void Blit(bitmap* Bitmap, ushort SourceX, ushort SourceY, vector2d Dest, ushort Width, ushort Height, ulong Luminance) const { Blit(Bitmap, SourceX, SourceY, Dest.X, Dest.Y, Width, Height, Luminance); }
  void Blit(bitmap* Bitmap, ushort SourceX, ushort SourceY, ushort DestX, ushort DestY, vector2d BlitSize, ulong Luminance) const { Blit(Bitmap, SourceX, SourceY, DestX, DestY, BlitSize.X, BlitSize.Y, Luminance); }
  void Blit(bitmap* Bitmap, vector2d Source, vector2d Dest, ushort Width, ushort Height, ulong Luminance) const { Blit(Bitmap, Source.X, Source.Y, Dest.X, Dest.Y, Width, Height, Luminance); }
  void Blit(bitmap* Bitmap, vector2d Source, ushort DestX, ushort DestY, vector2d BlitSize, ulong Luminance) const { Blit(Bitmap, Source.X, Source.Y, DestX, DestY, BlitSize.X, BlitSize.Y, Luminance); }
  void Blit(bitmap* Bitmap, ushort SourceX, ushort SourceY, vector2d Dest, vector2d BlitSize, ulong Luminance) const { Blit(Bitmap, SourceX, SourceY, Dest.X, Dest.Y, BlitSize.X, BlitSize.Y, Luminance); }
  void Blit(bitmap* Bitmap, vector2d Source, vector2d Dest, vector2d BlitSize, ulong Luminance) const  { Blit(Bitmap, Source.X, Source.Y, Dest.X, Dest.Y, BlitSize.X, BlitSize.Y, Luminance); }
  void Blit(bitmap* Bitmap, ulong Luminance) const { Blit(Bitmap, 0, 0, 0, 0, XSize, YSize, Luminance); }

  void MaskedBlit(bitmap*, ushort, ushort, ushort, ushort, ushort, ushort, uchar = 0, ushort = TRANSPARENT_COLOR) const;
  void MaskedBlit(bitmap* Bitmap, vector2d Source, ushort DestX, ushort DestY, ushort Width, ushort Height, uchar Flags = 0, ushort MaskColor = TRANSPARENT_COLOR) const { MaskedBlit(Bitmap, Source.X, Source.Y, DestX, DestY, Width, Height, Flags, MaskColor); }
  void MaskedBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, vector2d Dest, ushort Width, ushort Height, uchar Flags = 0, ushort MaskColor = TRANSPARENT_COLOR) const { MaskedBlit(Bitmap, SourceX, SourceY, Dest.X, Dest.Y, Width, Height, Flags, MaskColor); }
  void MaskedBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, ushort DestX, ushort DestY, vector2d BlitSize, uchar Flags = 0, ushort MaskColor = TRANSPARENT_COLOR) const { MaskedBlit(Bitmap, SourceX, SourceY, DestX, DestY, BlitSize.X, BlitSize.Y, Flags, MaskColor); }
  void MaskedBlit(bitmap* Bitmap, vector2d Source, vector2d Dest, ushort Width, ushort Height, uchar Flags = 0, ushort MaskColor = TRANSPARENT_COLOR) const { MaskedBlit(Bitmap, Source.X, Source.Y, Dest.X, Dest.Y, Width, Height, Flags, MaskColor); }
  void MaskedBlit(bitmap* Bitmap, vector2d Source, ushort DestX, ushort DestY, vector2d BlitSize, uchar Flags = 0, ushort MaskColor = TRANSPARENT_COLOR) const { MaskedBlit(Bitmap, Source.X, Source.Y, DestX, DestY, BlitSize.X, BlitSize.Y, Flags, MaskColor); }
  void MaskedBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, vector2d Dest, vector2d BlitSize, uchar Flags = 0, ushort MaskColor = TRANSPARENT_COLOR) const { MaskedBlit(Bitmap, SourceX, SourceY, Dest.X, Dest.Y, BlitSize.X, BlitSize.Y, Flags, MaskColor); }
  void MaskedBlit(bitmap* Bitmap, vector2d Source, vector2d Dest, vector2d BlitSize, uchar Flags = 0, ushort MaskColor = TRANSPARENT_COLOR) const  { MaskedBlit(Bitmap, Source.X, Source.Y, Dest.X, Dest.Y, BlitSize.X, BlitSize.Y, Flags, MaskColor); }
  void MaskedBlit(bitmap* Bitmap, uchar Flags = 0, ushort MaskColor = TRANSPARENT_COLOR) const { MaskedBlit(Bitmap, 0, 0, 0, 0, XSize, YSize, Flags, MaskColor); }

  void MaskedBlit(bitmap*, ushort, ushort, ushort, ushort, ushort, ushort, ulong, ushort = TRANSPARENT_COLOR) const;
  void MaskedBlit(bitmap* Bitmap, vector2d Source, ushort DestX, ushort DestY, ushort Width, ushort Height, ulong Luminance, ushort MaskColor = TRANSPARENT_COLOR) const { MaskedBlit(Bitmap, Source.X, Source.Y, DestX, DestY, Width, Height, Luminance, MaskColor); }
  void MaskedBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, vector2d Dest, ushort Width, ushort Height, ulong Luminance, ushort MaskColor = TRANSPARENT_COLOR) const { MaskedBlit(Bitmap, SourceX, SourceY, Dest.X, Dest.Y, Width, Height, Luminance, MaskColor); }
  void MaskedBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, ushort DestX, ushort DestY, vector2d BlitSize, ulong Luminance, ushort MaskColor = TRANSPARENT_COLOR) const { MaskedBlit(Bitmap, SourceX, SourceY, DestX, DestY, BlitSize.X, BlitSize.Y, Luminance, MaskColor); }
  void MaskedBlit(bitmap* Bitmap, vector2d Source, vector2d Dest, ushort Width, ushort Height, ulong Luminance, ushort MaskColor = TRANSPARENT_COLOR) const { MaskedBlit(Bitmap, Source.X, Source.Y, Dest.X, Dest.Y, Width, Height, Luminance, MaskColor); }
  void MaskedBlit(bitmap* Bitmap, vector2d Source, ushort DestX, ushort DestY, vector2d BlitSize, ulong Luminance, ushort MaskColor = TRANSPARENT_COLOR) const { MaskedBlit(Bitmap, Source.X, Source.Y, DestX, DestY, BlitSize.X, BlitSize.Y, Luminance, MaskColor); }
  void MaskedBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, vector2d Dest, vector2d BlitSize, ulong Luminance, ushort MaskColor = TRANSPARENT_COLOR) const { MaskedBlit(Bitmap, SourceX, SourceY, Dest.X, Dest.Y, BlitSize.X, BlitSize.Y, Luminance, MaskColor); }
  void MaskedBlit(bitmap* Bitmap, vector2d Source, vector2d Dest, vector2d BlitSize, ulong Luminance, ushort MaskColor = TRANSPARENT_COLOR) const  { MaskedBlit(Bitmap, Source.X, Source.Y, Dest.X, Dest.Y, BlitSize.X, BlitSize.Y, Luminance, MaskColor); }
  void MaskedBlit(bitmap* Bitmap, ulong Luminance, ushort MaskColor = TRANSPARENT_COLOR) const { MaskedBlit(Bitmap, 0, 0, 0, 0, XSize, YSize, Luminance, MaskColor); }

  void SimpleAlphaBlit(bitmap*, ushort, ushort, ushort, ushort, ushort, ushort, uchar, ushort = TRANSPARENT_COLOR) const;
  void SimpleAlphaBlit(bitmap* Bitmap, vector2d Source, ushort DestX, ushort DestY, ushort Width, ushort Height, uchar Alpha, ushort MaskColor = TRANSPARENT_COLOR) const { SimpleAlphaBlit(Bitmap, Source.X, Source.Y, DestX, DestY, Width, Height, Alpha, MaskColor); }
  void SimpleAlphaBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, vector2d Dest, ushort Width, ushort Height, uchar Alpha, ushort MaskColor = TRANSPARENT_COLOR) const { SimpleAlphaBlit(Bitmap, SourceX, SourceY, Dest.X, Dest.Y, Width, Height, Alpha, MaskColor); }
  void SimpleAlphaBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, ushort DestX, ushort DestY, vector2d BlitSize, uchar Alpha, ushort MaskColor = TRANSPARENT_COLOR) const { SimpleAlphaBlit(Bitmap, SourceX, SourceY, DestX, DestY, BlitSize.X, BlitSize.Y, Alpha, MaskColor); }
  void SimpleAlphaBlit(bitmap* Bitmap, vector2d Source, vector2d Dest, ushort Width, ushort Height, uchar Alpha, ushort MaskColor = TRANSPARENT_COLOR) const { SimpleAlphaBlit(Bitmap, Source.X, Source.Y, Dest.X, Dest.Y, Width, Height, Alpha, MaskColor); }
  void SimpleAlphaBlit(bitmap* Bitmap, vector2d Source, ushort DestX, ushort DestY, vector2d BlitSize, uchar Alpha, ushort MaskColor = TRANSPARENT_COLOR) const { SimpleAlphaBlit(Bitmap, Source.X, Source.Y, DestX, DestY, BlitSize.X, BlitSize.Y, Alpha, MaskColor); }
  void SimpleAlphaBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, vector2d Dest, vector2d BlitSize, uchar Alpha, ushort MaskColor = TRANSPARENT_COLOR) const { SimpleAlphaBlit(Bitmap, SourceX, SourceY, Dest.X, Dest.Y, BlitSize.X, BlitSize.Y, Alpha, MaskColor); }
  void SimpleAlphaBlit(bitmap* Bitmap, vector2d Source, vector2d Dest, vector2d BlitSize, uchar Alpha, ushort MaskColor = TRANSPARENT_COLOR) const  { SimpleAlphaBlit(Bitmap, Source.X, Source.Y, Dest.X, Dest.Y, BlitSize.X, BlitSize.Y, Alpha, MaskColor); }
  void SimpleAlphaBlit(bitmap* Bitmap, uchar Alpha, ushort MaskColor = TRANSPARENT_COLOR) const { SimpleAlphaBlit(Bitmap, 0, 0, 0, 0, XSize, YSize, Alpha, MaskColor); }

  void AlphaBlit(bitmap*, ushort, ushort, ushort, ushort, ushort, ushort, uchar = 0, ushort = TRANSPARENT_COLOR) const;
  void AlphaBlit(bitmap* Bitmap, vector2d Source, ushort DestX, ushort DestY, ushort Width, ushort Height, uchar Flags = 0, ushort MaskColor = TRANSPARENT_COLOR) const { AlphaBlit(Bitmap, Source.X, Source.Y, DestX, DestY, Width, Height, Flags, MaskColor); }
  void AlphaBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, vector2d Dest, ushort Width, ushort Height, uchar Flags = 0, ushort MaskColor = TRANSPARENT_COLOR) const { AlphaBlit(Bitmap, SourceX, SourceY, Dest.X, Dest.Y, Width, Height, Flags, MaskColor); }
  void AlphaBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, ushort DestX, ushort DestY, vector2d BlitSize, uchar Flags = 0, ushort MaskColor = TRANSPARENT_COLOR) const { AlphaBlit(Bitmap, SourceX, SourceY, DestX, DestY, BlitSize.X, BlitSize.Y, Flags, MaskColor); }
  void AlphaBlit(bitmap* Bitmap, vector2d Source, vector2d Dest, ushort Width, ushort Height, uchar Flags = 0, ushort MaskColor = TRANSPARENT_COLOR) const { AlphaBlit(Bitmap, Source.X, Source.Y, Dest.X, Dest.Y, Width, Height, Flags, MaskColor); }
  void AlphaBlit(bitmap* Bitmap, vector2d Source, ushort DestX, ushort DestY, vector2d BlitSize, uchar Flags = 0, ushort MaskColor = TRANSPARENT_COLOR) const { AlphaBlit(Bitmap, Source.X, Source.Y, DestX, DestY, BlitSize.X, BlitSize.Y, Flags, MaskColor); }
  void AlphaBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, vector2d Dest, vector2d BlitSize, uchar Flags = 0, ushort MaskColor = TRANSPARENT_COLOR) const { AlphaBlit(Bitmap, SourceX, SourceY, Dest.X, Dest.Y, BlitSize.X, BlitSize.Y, Flags, MaskColor); }
  void AlphaBlit(bitmap* Bitmap, vector2d Source, vector2d Dest, vector2d BlitSize, uchar Flags = 0, ushort MaskColor = TRANSPARENT_COLOR) const  { AlphaBlit(Bitmap, Source.X, Source.Y, Dest.X, Dest.Y, BlitSize.X, BlitSize.Y, Flags, MaskColor); }
  void AlphaBlit(bitmap* Bitmap, uchar Flags = 0, ushort MaskColor = TRANSPARENT_COLOR) const { AlphaBlit(Bitmap, 0, 0, 0, 0, XSize, YSize, Flags, MaskColor); }

  void DrawLine(ushort, ushort, ushort, ushort, ushort, bool = false);
  void DrawLine(vector2d From, ushort ToX, ushort ToY, ushort Color, bool Wide = false) { DrawLine(From.X, From.Y, ToX, ToY, Color, Wide); }
  void DrawLine(ushort FromX, ushort FromY, vector2d To, ushort Color, bool Wide = false) { DrawLine(FromX, FromY, To.X, To.Y, Color, Wide); }
  void DrawLine(vector2d From, vector2d To, ushort Color, bool Wide = false) { DrawLine(From.X, From.Y, To.X, To.Y, Color, Wide); }

  void DrawVerticalLine(ushort, ushort, ushort, ushort, bool = false);
  void DrawHorizontalLine(ushort, ushort, ushort, ushort, bool = false);

  void StretchBlit(bitmap*, ushort, ushort, ushort, ushort, ushort, ushort, char) const;
  void StretchBlit(bitmap* Bitmap, vector2d Source, ushort DestX, ushort DestY, ushort Width, ushort Height, char Stretch) const { StretchBlit(Bitmap, Source.X, Source.Y, DestX, DestY, Width, Height, Stretch); }
  void StretchBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, vector2d Dest, ushort Width, ushort Height, char Stretch) const { StretchBlit(Bitmap, SourceX, SourceY, Dest.X, Dest.Y, Width, Height, Stretch); }
  void StretchBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, ushort DestX, ushort DestY, vector2d BlitSize, char Stretch) const { StretchBlit(Bitmap, SourceX, SourceY, DestX, DestY, BlitSize.X, BlitSize.Y, Stretch); }
  void StretchBlit(bitmap* Bitmap, vector2d Source, vector2d Dest, ushort Width, ushort Height, char Stretch) const { StretchBlit(Bitmap, Source.X, Source.Y, Dest.X, Dest.Y, Width, Height, Stretch); }
  void StretchBlit(bitmap* Bitmap, vector2d Source, ushort DestX, ushort DestY, vector2d BlitSize, char Stretch) const { StretchBlit(Bitmap, Source.X, Source.Y, DestX, DestY, BlitSize.X, BlitSize.Y, Stretch); }
  void StretchBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, vector2d Dest, vector2d BlitSize, char Stretch) const { StretchBlit(Bitmap, SourceX, SourceY, Dest.X, Dest.Y, BlitSize.X, BlitSize.Y, Stretch); }
  void StretchBlit(bitmap* Bitmap, vector2d Source, vector2d Dest, vector2d BlitSize, char Stretch) const  { StretchBlit(Bitmap, Source.X, Source.Y, Dest.X, Dest.Y, BlitSize.X, BlitSize.Y, Stretch); }
  void StretchBlit(bitmap* Bitmap, char Stretch) const { StretchBlit(Bitmap, 0, 0, 0, 0, XSize, YSize, Stretch); }

  void PowerBlit(bitmap*, ushort, ushort, ushort, ushort, ushort, ushort, ulong, ushort = TRANSPARENT_COLOR) const;
  void PowerBlit(bitmap* Bitmap, vector2d Source, ushort DestX, ushort DestY, ushort Width, ushort Height, ulong Luminance, ushort MaskColor = TRANSPARENT_COLOR) const { PowerBlit(Bitmap, Source.X, Source.Y, DestX, DestY, Width, Height, Luminance, MaskColor); }
  void PowerBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, vector2d Dest, ushort Width, ushort Height, ulong Luminance, ushort MaskColor = TRANSPARENT_COLOR) const { PowerBlit(Bitmap, SourceX, SourceY, Dest.X, Dest.Y, Width, Height, Luminance, MaskColor); }
  void PowerBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, ushort DestX, ushort DestY, vector2d BlitSize, ulong Luminance, ushort MaskColor = TRANSPARENT_COLOR) const { PowerBlit(Bitmap, SourceX, SourceY, DestX, DestY, BlitSize.X, BlitSize.Y, Luminance, MaskColor); }
  void PowerBlit(bitmap* Bitmap, vector2d Source, vector2d Dest, ushort Width, ushort Height, ulong Luminance, ushort MaskColor = TRANSPARENT_COLOR) const { PowerBlit(Bitmap, Source.X, Source.Y, Dest.X, Dest.Y, Width, Height, Luminance, MaskColor); }
  void PowerBlit(bitmap* Bitmap, vector2d Source, ushort DestX, ushort DestY, vector2d BlitSize, ulong Luminance, ushort MaskColor = TRANSPARENT_COLOR) const { PowerBlit(Bitmap, Source.X, Source.Y, DestX, DestY, BlitSize.X, BlitSize.Y, Luminance, MaskColor); }
  void PowerBlit(bitmap* Bitmap, ushort SourceX, ushort SourceY, vector2d Dest, vector2d BlitSize, ulong Luminance, ushort MaskColor = TRANSPARENT_COLOR) const { PowerBlit(Bitmap, SourceX, SourceY, Dest.X, Dest.Y, BlitSize.X, BlitSize.Y, Luminance, MaskColor); }
  void PowerBlit(bitmap* Bitmap, vector2d Source, vector2d Dest, vector2d BlitSize, ulong Luminance, ushort MaskColor = TRANSPARENT_COLOR) const  { PowerBlit(Bitmap, Source.X, Source.Y, Dest.X, Dest.Y, BlitSize.X, BlitSize.Y, Luminance, MaskColor); }
  void PowerBlit(bitmap* Bitmap, ulong Luminance, ushort MaskColor = TRANSPARENT_COLOR) const { PowerBlit(Bitmap, 0, 0, 0, 0, XSize, YSize, Luminance, MaskColor); }

  void DrawRectangle(ushort, ushort, ushort, ushort, ushort, bool = false);
  void DrawRectangle(vector2d TopLeft, ushort Right, ushort Bottom, ushort Color, bool Wide = false) { DrawRectangle(TopLeft.X, TopLeft.Y, Right, Bottom, Color, Wide); }
  void DrawRectangle(ushort Left, ushort Top, vector2d BottomRight, ushort Color, bool Wide = false) { DrawRectangle(Left, Top, BottomRight.X, BottomRight.Y, Color, Wide); }
  void DrawRectangle(vector2d TopLeft, vector2d BottomRight, ushort Color, bool Wide = false) { DrawRectangle(TopLeft.X, TopLeft.Y, BottomRight.X, BottomRight.Y, Color, Wide); }

  ushort GetXSize() const { return XSize; }
  ushort GetYSize() const { return YSize; }
  vector2d GetSize() const { return vector2d(XSize, YSize); }
  void DrawPolygon(vector2d, ushort, ushort, ushort, bool = true, bool = false, double = 0);
  void CreateAlphaMap(uchar);
  bool ChangeAlpha(char);
  void SetAlpha(ushort, ushort, uchar);
  void SetAlpha(vector2d Pos, uchar Alpha) { SetAlpha(Pos.X, Pos.Y, Alpha); }
  uchar GetAlpha(ushort, ushort) const;
  uchar GetAlpha(vector2d Pos) const { return GetAlpha(Pos.X, Pos.Y); }
  void Outline(ushort);
  void CreateOutlineBitmap(bitmap*, ushort);
  void FadeToScreen(void (*)(bitmap*) = 0);
  void CreateFlames(ushort, ushort = TRANSPARENT_COLOR);
  bool IsValidPos(vector2d What) const { return What.X >= 0 && What.Y >= 0 && What.X < XSize && What.Y < YSize; }
  bool IsValidPos(short X, short Y) const { return X >= 0 && Y >= 0 && X < XSize && Y < YSize; }
  void CreateSparkle(vector2d, ushort);
  void CreateFlies(ulonglong, ushort, uchar);
  void CreateLightning(ulonglong, ushort);
  bool CreateLightning(vector2d, vector2d, ushort, ushort);
  static bool PixelVectorHandler(long, long);
 protected:
  ushort XSize, YSize;
  bool IsIndependent;
  ushort** GetImage() const { return Data.Base.Image; }
  void SetImage(ushort** What) { Data.Base.Image = What; }
  uchar** GetAlphaMap() const { return Data.Base.AlphaMap; }
  void SetAlphaMap(uchar** What) { Data.Base.AlphaMap = What; }
  bitmap* GetMotherBitmap() const { return Data.Child.MotherBitmap; }
  void SetMotherBitmap(bitmap* What) { Data.Child.MotherBitmap = What; }
  ushort GetXPos() const { return Data.Child.XPos; }
  void SetXPos(ushort What) { Data.Child.XPos = What; }
  ushort GetYPos() const { return Data.Child.YPos; }
  void SetYPos(ushort What) { Data.Child.YPos = What; }
  union data
  {
    struct base
    {
      ushort** Image;
      uchar** AlphaMap;
    } Base;
    struct child
    {
      bitmap* MotherBitmap;
      ushort XPos, YPos;
    } Child;
  } Data;
};

outputfile& operator<<(outputfile&, bitmap*);
inputfile& operator>>(inputfile&, bitmap*&);

#endif
