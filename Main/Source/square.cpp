#include "charba.h"
#include "square.h"
#include "terra.h"
#include "area.h"
#include "materba.h"
#include "strover.h"
#include "config.h"
#include "femath.h"
#include "bitmap.h"
#include "graphics.h"
#include "proto.h"
#include "game.h"
#include "save.h"

square::square(area* AreaUnder, vector2d Pos) : AreaUnder(AreaUnder), Character(0), Pos(Pos), NewDrawRequested(true), MemorizedUpdateRequested(true), Memorized(0), LastSeen(0), DescriptionChanged(true), AnimatedEntities(0)
{
}

square::~square()
{
  delete Character;
  delete Memorized;
}

void square::Save(outputfile& SaveFile) const
{
  SaveFile << Character << LastSeen << DescriptionChanged  << AnimatedEntities;

  if(LastSeen)
    GetMemorized()->Save(SaveFile);

  SaveFile << MemorizedDescription;
}

void square::Load(inputfile& SaveFile)
{
  game::SetSquareInLoad(this);

  SaveFile >> Character >> LastSeen >> DescriptionChanged >> AnimatedEntities;

  if(LastSeen)
    {
      Memorized = new bitmap(16, 16);
      GetMemorized()->Load(SaveFile);
    }

  SaveFile >> MemorizedDescription;
}

void square::AddCharacter(character* Guy)
{
  Character = Guy;
  Guy->SetSquareUnder(this);
  NewDrawRequested = true;

  if(Guy->IsAnimated())
    IncAnimatedEntities();
}

void square::DrawMemorized()
{
  if(GetLastSeen() && (NewDrawRequested || GetLastSeen() == game::GetLOSTurns() - 1))
    {	
      GetMemorized()->Blit(DOUBLEBUFFER, 0, 0, game::CalculateScreenCoordinates(GetPos()), vector2d(16, 16), ushort(256.0f * configuration::GetContrast() / 100));
      NewDrawRequested = false;
    }
}

void square::RemoveCharacter()
{
  if(Character && Character->IsAnimated())
    DecAnimatedEntities();

  SetCharacter(0);
  NewDrawRequested = true;
}

bool square::CanBeSeen(bool) const
{
  if(GetLastSeen() == game::GetLOSTurns())
    return true;
  else
    return false;
}

bool square::CanBeSeenFrom(vector2d FromPos, ulong MaxDistance, bool IgnoreDarkness) const
{
  ulong Distance = (GetPos() - FromPos).Length();

  if(Distance > MaxDistance)
    return false;
  else
    {
      character* Char = GetCharacter();

      if(Char && Char->IsPlayer() && Distance < Char->LOSRangeSquare())
	return GetAreaUnder()->GetSquare(FromPos)->CanBeSeen(IgnoreDarkness);
      else
	return femath::DoLine(FromPos.X, FromPos.Y, GetPos().X, GetPos().Y, game::EyeHandler);
    }
}

void square::SetLastSeen(ulong What)
{
  if(!GetLastSeen())
    Memorized = new bitmap(16, 16);

  if(GetLastSeen() < What - 1 || !GetOTerrain()->IsWalkable())
    SendNewDrawRequest();

  UpdateMemorized();
  UpdateMemorizedDescription();

  LastSeen = What;
}

void square::KickAnyoneStandingHereAway()
{
  if(Character)
    {
      GetAreaUnder()->AddCharacter(GetAreaUnder()->GetNearestFreeSquare(Character, GetPos()), Character);
      RemoveCharacter();
    }
}

bool square::IsWalkable(character* Char) const
{
  return GetOTerrain()->IsWalkable() && GetGTerrain()->IsWalkable(Char);
}

std::string square::SurviveMessage(character*) const
{
  if(!GetOTerrain()->IsWalkable())
    return GetOTerrain()->SurviveMessage();
  else
    return GetGTerrain()->SurviveMessage();
}

std::string square::DeathMessage(character*) const
{
  if(!GetOTerrain()->IsWalkable())
    return GetOTerrain()->DeathMessage();
  else
    return GetGTerrain()->DeathMessage();
}

std::string square::MonsterDeathVerb(character*) const
{
  if(!GetOTerrain()->IsWalkable())
    return GetOTerrain()->MonsterDeathVerb();
  else
    return GetGTerrain()->MonsterDeathVerb();
}

std::string square::ScoreEntry(character*) const
{
  if(!GetOTerrain()->IsWalkable())
    return GetOTerrain()->ScoreEntry();
  else
    return GetGTerrain()->ScoreEntry();
}

ushort square::GetEntryAPRequirement() const
{
  return GetGTerrain()->GetEntryAPRequirement();
}

uchar square::RestModifier() const
{
  return GetOTerrain()->RestModifier();
}

