/*
 *
 *  Iter Vehemens ad Necem 
 *  Copyright (C) Timo Kiviluoto
 *  See LICENSING which should included with this file
 *
 */

#include "wskill.h"
#include "message.h"
#include "save.h"
#include "item.h"

/*int CWeaponSkillLevelMap[] = { 0, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000, 65535 };
ulong CWeaponSkillUnuseTickMap[] = { 500000, 250000, 200000, 150000, 50000, 30000, 25000, 20000, 15000, 12500, 10000 };
int CWeaponSkillUnusePenaltyMap[] = { 10, 15, 25, 50, 75, 100, 200, 600, 1000, 2500, 3000 };
const char* CWeaponSkillName[WEAPON_SKILL_CATEGORIES] = { "unarmed combat", "kicking", "biting", "uncategorized", "small swords", "large swords", "blunt weapons", "axes", "polearms", "whips", "shields" };

int SWeaponSkillLevelMap[] = { 0, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 65535 };
ulong SWeaponSkillUnuseTickMap[] = { 100000, 100000, 40000, 30000, 20000, 15000, 10000, 7500, 5000, 2500, 2000 };
int SWeaponSkillUnusePenaltyMap[] = { 5, 5, 5, 15, 25, 50, 150, 250, 500, 1000, 1500 };*/

int CWeaponSkillLevelMap[] = { 0, 1500, 2000, 3000, 5000, 7500, 10000, 15000,
			       20000, 30000, 50000, 75000, 100000, 150000,
			       200000, 300000, 500000, 750000, 1000000,
			       1500000, 2000000 };
ulong CWeaponSkillUnuseTickMap[] = { 500000, 400000, 300000, 250000, 200000,
				     150000, 125000, 100000, 80000, 62500,
				     50000, 40000, 30000, 25000, 20000,
				     15000, 12500, 10000, 8000, 6250, 5000 };
int CWeaponSkillUnusePenaltyMap[] = { 100, 125, 150, 200, 250, 300, 400, 500,
				      625, 800, 1000, 1250, 1500, 2000, 2500,
				      3000, 4000, 5000, 6250, 8000, 10000 };
const char* CWeaponSkillName[WEAPON_SKILL_CATEGORIES]
= { "unarmed combat",
    "kicking",
    "biting",
    "uncategorized",
    "small swords",
    "large swords",
    "blunt weapons",
    "axes",
    "polearms",
    "whips",
    "shields"
  };

int SWeaponSkillLevelMap[] = { 0, 500, 750, 1000, 1500, 2000, 3000, 5000,
			       7500, 10000, 15000, 20000, 30000, 50000,
			       75000, 100000, 150000, 200000, 300000,
			       500000, 750000 };
ulong SWeaponSkillUnuseTickMap[] = { 250000, 200000, 150000, 125000, 100000,
				     80000, 62500, 50000, 40000, 30000,
				     25000, 20000, 15000, 12500, 10000,
				     8000, 6250, 5000, 4000, 3000, 2500 };
int SWeaponSkillUnusePenaltyMap[] = { 250, 300, 400, 500, 625, 800, 1000,
				      1250, 1500, 2000, 2500, 3000, 4000,
				      5000, 6250, 8000, 10000, 12500, 15000,
				      20000, 25000 };

int cweaponskill::GetLevelMap(int I) const { return CWeaponSkillLevelMap[I]; }
ulong cweaponskill::GetUnuseTickMap(int I) const { return CWeaponSkillUnuseTickMap[I]; }
int cweaponskill::GetUnusePenaltyMap(int I) const { return CWeaponSkillUnusePenaltyMap[I]; }
const char* cweaponskill::GetName(int Category) const { return CWeaponSkillName[Category]; }

sweaponskill::sweaponskill(const item* Item) : ID(Item->GetID()), Weight(Item->GetWeight()), Config(Item->GetConfig()) { }
int sweaponskill::GetLevelMap(int I) const { return SWeaponSkillLevelMap[I]; }
ulong sweaponskill::GetUnuseTickMap(int I) const { return SWeaponSkillUnuseTickMap[I]; }
int sweaponskill::GetUnusePenaltyMap(int I) const { return SWeaponSkillUnusePenaltyMap[I]; }
bool sweaponskill::IsSkillOf(const item* Item) const { return ID == Item->GetID() && Weight == Item->GetWeight() && Config == Item->GetConfig(); }
bool sweaponskill::IsSkillOfCloneMother(const item* Item, ulong CMID) const { return ID == CMID && Weight == Item->GetWeight() && Config == Item->GetConfig(); }

void weaponskill::Save(outputfile& SaveFile) const
{
  SaveFile << (int)Level << (int)Hits << (int)HitCounter;
}

void weaponskill::Load(inputfile& SaveFile)
{
  SaveFile >> (int&)Level >> (int&)Hits >> (int&)HitCounter;
}

/*bool weaponskill::AddHit()
{
  HitCounter = 0;

  if(Hits != 50000)
    if(++Hits == GetLevelMap(Level + 1))
      {
	++Level;
	return true;
      }

  return false;
}*/

bool weaponskill::AddHit(int AddHits)
{
  if(!AddHits)
    return false;

  HitCounter = 0;

  if(Hits <= 5000000 - AddHits)
    Hits += AddHits;
  else
    Hits = 5000000;

  int OldLevel = Level;

  while(Hits >= GetLevelMap(Level + 1))
    ++Level;

  return Level != OldLevel;
}

/*bool weaponskill::SubHit()
{
  if(Hits)
    {
      --Hits;

      if(Level && Hits < GetLevelMap(Level))
	{
	  --Level;
	  return true;
	}
    }

  return false;
}*/

bool weaponskill::SubHit(int SubHits)
{
  if(!SubHits)
    return false;

  if(Hits >= SubHits)
    Hits -= SubHits;
  else
    Hits = 0;

  int OldLevel = Level;

  while(Level && Hits < GetLevelMap(Level))
    --Level;

  return Level != OldLevel;
}

void cweaponskill::AddLevelUpMessage(int Category) const
{
  ADD_MESSAGE("You advance to skill level %d with %s!", Level, CWeaponSkillName[Category]);
}

void cweaponskill::AddLevelDownMessage(int Category) const
{
  ADD_MESSAGE("You have not practised enough with %s lately. Your skill level is reduced to %d!", CWeaponSkillName[Category], Level);
}

void sweaponskill::AddLevelUpMessage(const char* WeaponName) const
{
  ADD_MESSAGE("You advance to skill level %d with your %s!", Level, WeaponName);
}

void sweaponskill::AddLevelDownMessage(const char* WeaponName) const
{
  ADD_MESSAGE("You have not practised enough with your %s lately. Your skill level is reduced to %d!", WeaponName, Level);
}

void sweaponskill::Save(outputfile& SaveFile) const
{
  weaponskill::Save(SaveFile);
  SaveFile << ID << Weight << (int)Config;
}

void sweaponskill::Load(inputfile& SaveFile)
{
  weaponskill::Load(SaveFile);
  SaveFile >> ID >> Weight >> (int&)Config;
}

bool weaponskill::Tick()
{
  if(Hits && HitCounter++ >= GetUnuseTickMap(Level))
    {
      HitCounter -= GetUnuseTickMap(Level);

      if(SubHit(GetUnusePenaltyMap(Level)))
	return true;
    }

  return false;
}
