#include "fluid.h"
#include "bitmap.h"
#include "save.h"
#include "game.h"
#include "femath.h"

fluid::fluid() : entity(HAS_BE) { }

fluid::fluid(square* SquareUnder) : entity(HAS_BE), Picture(0), Material(0), SquareUnder(SquareUnder)
{
  Picture = new bitmap(16, 16);
  Picture->Fill(TRANSPARENT_COLOR);
  Picture->CreateAlphaMap(0);
}

fluid::~fluid()
{
  delete Picture;
}

void fluid::SpillFluid(uchar Amount, ulong Color, ushort Lumpiness, ushort Variation)
{
  for(ushort c = 0; c < Amount; ++c)
    {
      vector2d Cords(1 + RAND() % 14, 1 + RAND() % 14);
      GetPicture()->PutPixel(Cords, Color);
      GetPicture()->SetAlpha(Cords, 150 + RAND() % 106);

      for(ushort d = 0; d < 8; ++d)
	if(RAND() % Lumpiness)
	  {
	    char Change[3];

	    for(ushort x = 0; x < 3; ++x)
	      Change[x] = RAND() % Variation - RAND() % Variation;

	    if(short(GetRed16(Color) + Change[0]) < 0) Change[0] = -GetRed16(Color);
	    if(short(GetGreen16(Color) + Change[1]) < 0) Change[1] = -GetGreen16(Color);
	    if(short(GetBlue16(Color) + Change[2]) < 0) Change[2] = -GetBlue16(Color);

	    if(short(GetRed16(Color) + Change[0]) > 255) Change[0] = 255 - GetRed16(Color);
	    if(short(GetGreen16(Color) + Change[1]) > 255) Change[1] = 255 - GetGreen16(Color);
	    if(short(GetBlue16(Color) + Change[2]) > 255) Change[2] = 255 - GetBlue16(Color);

	    GetPicture()->PutPixel(Cords + game::GetMoveVector(d), MakeRGB16(GetRed16(Color) + Change[0], GetGreen16(Color) + Change[1], GetBlue16(Color) + Change[2]));
	    GetPicture()->SetAlpha(Cords + game::GetMoveVector(d), 50 + RAND() % 206);
	  }
    }
}

void fluid::Be()
{
  if(!(RAND() % 25))
    {
      if(!GetPicture()->ChangeAlpha(-1))
	{
	  GetLSquareUnder()->RemoveFluid();
	  SendToHell();
	}

      GetLSquareUnder()->SendNewDrawRequest();
      GetLSquareUnder()->SendMemorizedUpdateRequest();
    }
}

void fluid::Save(outputfile& SaveFile) const
{
  SaveFile << Picture;
}

void fluid::Load(inputfile& SaveFile)
{
  SquareUnder = game::GetSquareInLoad();
  SaveFile >> Picture;
}

void fluid::Draw(bitmap* Bitmap, vector2d Pos, ulong Luminance, bool) const
{
  Picture->AlphaBlit(Bitmap, 0, 0, Pos, 16, 16, Luminance);
}

outputfile& operator<<(outputfile& SaveFile, const fluid* Fluid)
{
  if(Fluid)
    {
      SaveFile.Put(1);
      Fluid->Save(SaveFile);
    }
  else
    SaveFile.Put(0);

  return SaveFile;
}

inputfile& operator>>(inputfile& SaveFile, fluid*& Fluid)
{
  if(SaveFile.Get())
    {
      Fluid = new fluid;
      Fluid->Load(SaveFile);
    }

  return SaveFile;
}
