#ifndef __WSKILL_H__
#define __WSKILL_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include <string>

#include "typedef.h"

class outputfile;
class inputfile;

class weaponskill
{
 public:
  weaponskill() : Level(0), Hits(0), HitCounter(0) { }
  uchar GetLevel() const { return Level; }
  ulong GetHits() const { return Hits; }
  ulong GetHitCounter() const { return HitCounter; }
  bool AddHit();
  bool AddHit(ushort);
  bool SubHit();
  bool SubHit(ushort);
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual ushort GetLevelMap(ushort) const = 0;
  virtual ulong GetUnuseTickMap(ushort) const = 0;
  virtual ushort GetUnusePenaltyMap(ushort) const = 0;
 protected:
  uchar Level;
  ushort Hits;
  ushort HitCounter;
};

class gweaponskill : public weaponskill
{
 public:
  gweaponskill(uchar Index) : Index(Index) { }
  bool Tick();
  ushort GetLevelMap(ushort Index) const { return LevelMap[Index]; }
  ulong GetUnuseTickMap(ushort Index) const { return UnuseTickMap[Index]; }
  ushort GetUnusePenaltyMap(ushort Index) const { return UnusePenaltyMap[Index]; }
  const std::string& Name() const { return SkillName[Index]; }
  float GetEffectBonus() const { return 1.0f + 0.10f * Level; }
  float GetAPBonus() const { return 1.0f - 0.05f * Level; }
  void AddLevelUpMessage() const;
  void AddLevelDownMessage() const;
 private:
  static ushort LevelMap[];
  static ulong UnuseTickMap[];
  static ushort UnusePenaltyMap[];
  static std::string SkillName[];
  uchar Index;
};

inline outputfile& operator<<(outputfile& SaveFile, gweaponskill* WeaponSkill)
{
  WeaponSkill->Save(SaveFile);
  return SaveFile;
}

inline inputfile& operator>>(inputfile& SaveFile, gweaponskill* WeaponSkill)
{
  WeaponSkill->Load(SaveFile);
  return SaveFile;
}

class sweaponskill : public weaponskill
{
 public:
  bool Tick();
  ushort GetLevelMap(ushort Index) const { return LevelMap[Index]; }
  ulong GetUnuseTickMap(ushort Index) const { return UnuseTickMap[Index]; }
  ushort GetUnusePenaltyMap(ushort Index) const { return UnusePenaltyMap[Index]; }
  float GetEffectBonus() const { return Level ? 1.15f + 0.05f * (Level - 1) : 1.0f; }
  float GetAPBonus() const { return Level ? 0.93f - 0.02f * (Level - 1) : 1.0f; }
  void AddLevelUpMessage(const std::string&) const;
  void AddLevelDownMessage(const std::string&) const;
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  void SetID(ulong What) { ID = What; }
  ulong GetID() const { return ID; }
 private:
  static ushort LevelMap[];
  static ulong UnuseTickMap[];
  static ushort UnusePenaltyMap[];
  ulong ID;
};

inline outputfile& operator<<(outputfile& SaveFile, sweaponskill* WeaponSkill)
{
  WeaponSkill->Save(SaveFile);
  return SaveFile;
}

inline inputfile& operator>>(inputfile& SaveFile, sweaponskill*& WeaponSkill)
{
  WeaponSkill = new sweaponskill;
  WeaponSkill->Load(SaveFile);
  return SaveFile;
}

#endif


