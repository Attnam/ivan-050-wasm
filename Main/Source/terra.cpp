#include "terra.h"
#include "charba.h"

bool groundterrain::GetIsWalkable(character* ByWho) const
{
  return !ByWho || ByWho->CanWalk() || ByWho->CanFly();
}