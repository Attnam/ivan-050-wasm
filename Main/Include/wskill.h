#ifndef __WSKILL_H__
#define __WSKILL_H__

#define WEAPON_SKILL_GATEGORIES	12

#define UNCATEGORIZED	0
#define UNARMED		1
#define DAGGERS		2
#define SMALL_SWORDS	3
#define LARGE_SWORDS	4
#define CLUBS		5
#define HAMMERS		6
#define MACES		7
#define FLAILS		8
#define AXES		9
#define HALBERDS	10
#define SPEARS		11

#include <string>

#include "typedef.h"

class outputfile;
class inputfile;

class weaponskill
{
public:
	weaponskill();
	void Turn(bool = false);
	uchar GetLevel() const { return Level; }
	ulong GetHits() const { return Hits; }
	ulong GetHitCounter() const { return HitCounter; }
	void AddHit(bool = false);
	void AddHit(ulong, bool = false);
	void SubHit(bool = false);
	void SubHit(ulong, bool = false);
	virtual ushort GetLevelMap(ushort Index) const = 0;
	virtual ushort GetUnuseTurnMap(ushort Index) const = 0;
	virtual ushort GetUnusePenaltyMap(ushort Index) const = 0;
	virtual void Save(outputfile&) const;
	virtual void Load(inputfile&);
	void SetHitMultiplier(float What) { HitMultiplier = What; }
	float GetHitMultiplier() const { return HitMultiplier; }
	virtual void AddLevelUpMessage() const = 0;
	virtual void AddLevelDownMessage() const = 0;
protected:
	uchar Level;
	ushort Hits, HitCounter;
	float HitMultiplier;
};

class gweaponskill : public weaponskill
{
public:
	gweaponskill(uchar Index) : Index(Index) {}
	ushort GetLevelMap(ushort Index) const { return LevelMap[Index]; }
	ushort GetUnuseTurnMap(ushort Index) const { return UnuseTurnMap[Index]; }
	ushort GetUnusePenaltyMap(ushort Index) const { return UnusePenaltyMap[Index]; }
	std::string Name() const { return SkillName[Index]; }
	float GetBonus() const { return 1.0f + 0.15f * Level; }
	void AddLevelUpMessage() const;
	void AddLevelDownMessage() const;
private:
	static ushort LevelMap[];
	static ushort UnuseTurnMap[];
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
	sweaponskill() {}
	sweaponskill(std::string WeaponName) : WeaponName(WeaponName) {}
	ushort GetLevelMap(ushort Index) const { return LevelMap[Index]; }
	ushort GetUnuseTurnMap(ushort Index) const { return UnuseTurnMap[Index]; }
	ushort GetUnusePenaltyMap(ushort Index) const { return UnusePenaltyMap[Index]; }
	float GetBonus() const { return Level ? 1.2f + 0.05f * (Level - 1) : 1.0f; }
	void AddLevelUpMessage() const;
	void AddLevelDownMessage() const;
	void Save(outputfile&) const;
	void Load(inputfile&);
	void SetID(ulong What) { ID = What; }
	ulong GetID() const { return ID; }
	std::string Name() const { return WeaponName; }
private:
	static ushort LevelMap[];
	static ushort UnuseTurnMap[];
	static ushort UnusePenaltyMap[];
	std::string WeaponName;
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

