#include <algorithm>

#include "wsquare.h"
#include "bitmap.h"
#include "worldmap.h"
#include "message.h"
#include "charba.h"
#include "femath.h"
#include "wterraba.h"
#include "game.h"
#include "festring.h"
#include "igraph.h"
#include "save.h"
#include "proto.h"
#include "whandler.h"

void wterrain::AddName(std::string& String, uchar Case) const
{
  if(!(Case & PLURAL))
    if(!(Case & ARTICLEBIT))
      String << GetNameStem();
    else
      if(!(Case & INDEFINEBIT))
	String << "the " << GetNameStem();
      else
	String << (LongerArticle() ? "an " : "a ") << GetNameStem();
  else
    if(!(Case & ARTICLEBIT))
      String << GetNameStem() << " terrains";
    else
      if(!(Case & INDEFINEBIT))
	String << "the " << GetNameStem() << " terrains";
      else
	String << GetNameStem() << " terrains";
}

std::string wterrain::GetName(uchar Case) const
{
  std::string Name;
  AddName(Name, Case);
  return Name;
}

vector2d wterrain::GetPos() const
{
  return GetWSquareUnder()->GetPos();
}

void gwterrain::Draw(bitmap* Bitmap, vector2d Pos, ushort Luminance, bool, bool AllowAnimate) const
{
  igraph::GetWTerrainGraphic()->Blit(Bitmap, GetBitmapPos(!AllowAnimate || AnimationFrames == 1 ? 0 : globalwindowhandler::GetTick() % AnimationFrames), Pos, 16, 16, Luminance);

  for(ushort c = 0; c < Neighbour.size(); ++c)
    igraph::GetWTerrainGraphic()->MaskedBlit(Bitmap, Neighbour[c].first, Pos, 16, 16, Luminance);
}

void owterrain::Draw(bitmap* Bitmap, vector2d Pos, ushort Luminance, bool, bool AllowAnimate) const
{
  igraph::GetWTerrainGraphic()->MaskedBlit(Bitmap, GetBitmapPos(!AllowAnimate || AnimationFrames == 1 ? 0 : globalwindowhandler::GetTick() % AnimationFrames), Pos, 16, 16, Luminance);
}

worldmap* wterrain::GetWorldMapUnder() const
{
  return GetWSquareUnder()->GetWorldMapUnder();
}

bool owterrain::GoUp(character*) const
{
  ADD_MESSAGE("You jump into the air. For some reason you don't get too far above.");
  return false;
}

bool owterrain::GoDown(character*) const
{
  ADD_MESSAGE("There seems to be nothing of interest here.");
  return false;
}

void wterrain::Load(inputfile&)
{
  WSquareUnder = (wsquare*)game::GetSquareInLoad();
}

void gwterrain::Save(outputfile& SaveFile) const
{
  SaveFile << GetType();
}

void owterrain::Save(outputfile& SaveFile) const
{
  SaveFile << GetType();
}

gwterrain* gwterrainprototype::CloneAndLoad(inputfile& SaveFile) const
{
  gwterrain* Terrain = Cloner(true);
  Terrain->Load(SaveFile);
  return Terrain;
}

owterrain* owterrainprototype::CloneAndLoad(inputfile& SaveFile) const
{
  owterrain* Terrain = Cloner(true);
  Terrain->Load(SaveFile);
  return Terrain;
}

gwterrainprototype::gwterrainprototype(gwterrain* (*Cloner)(bool), const std::string& ClassId) : Cloner(Cloner), ClassId(ClassId)
{
  Index = protocontainer<gwterrain>::Add(this);
}

owterrainprototype::owterrainprototype(owterrain* (*Cloner)(bool), const std::string& ClassId) : Cloner(Cloner), ClassId(ClassId)
{
  Index = protocontainer<owterrain>::Add(this);
}

bool DrawOrderer(const std::pair<vector2d, uchar>& Pair1, const std::pair<vector2d, uchar>& Pair2)
{
  return Pair1.second < Pair2.second;
}

void gwterrain::CalculateNeighbourBitmapPoses()
{
  Neighbour.clear();

  for(ushort d = 0; d < 8; ++d)
    {
      wsquare* NeighbourSquare = GetWorldMapUnder()->GetNeighbourWSquare(GetPos(), d);

      if(NeighbourSquare)
	{
	  gwterrain* DoNeighbour = NeighbourSquare->GetGWTerrain();

	  if(DoNeighbour->Priority() > Priority())
	    Neighbour.push_back(std::pair<vector2d, uchar>(DoNeighbour->GetBitmapPos(0) - (game::GetMoveVector(d) << 4), DoNeighbour->Priority()));
	}
    }

  std::sort(Neighbour.begin(), Neighbour.end(), DrawOrderer);
}

