#ifndef __CHARBA_H__
#define __CHARBA_H__

#define HAS_HIT 0
#define HAS_BLOCKED 1
#define HAS_DODGED 2
#define HAS_DIED 3

#define HOSTILE 0
#define NEUTRAL 1
#define FRIEND 2

#define NUMBER_OF_HUMAN_ARMS	7
#define NUMBER_OF_HUMAN_HEADS	12
#define NUMBER_OF_HUMAN_LEGS	6
#define NUMBER_OF_HUMAN_TORSOS	9

#define HUNGERLEVEL 		200 
#define CRITICALHUNGERLEVEL 	60

#define OVERLOADED		0
#define STRESSED		1
#define BURDENED		2
#define UNBURDENED		3

#include "game.h"
#include "object.h"
#include "typedef.h"
#include "vector2d.h"
#include "igraph.h"

class square;
class bitmap;
class character;
class item;
class stack;
class material;
class levelsquare;
class worldmapsquare;
class outputfile;
class inputfile;

/* Presentation of the character class */

class character : public object
{
public:
	character(bool, bool, bool, bool = true);
	virtual ~character();
	virtual character* Clone(bool = true, bool = true, bool = true) const = 0;
	virtual void Save(outputfile&) const;
	virtual void Load(inputfile&);
	virtual bool CanRead() const { return true; }
	virtual bool CanWear() const { return false; }
	virtual bool CanWield() const { return false; }
	virtual bool Charmable() const { return true; }
	virtual bool Catches(item*, float) { return false; }
	virtual bool CheckBulimia() const;
	virtual bool CheckDeath(std::string);
	virtual bool CheckIfConsumable(ushort) const;
	virtual bool ConsumeItem(int, stack*);
	virtual bool ConsumeItemType(uchar) const;
	virtual bool DodgesFlyingItem(item*, float);
	virtual bool Hit(character*);
	virtual bool OpenItem();
	virtual bool OpenPos(vector2d);
	virtual bool ReadItem(int, stack*);
	virtual bool TestForPickup(item*) const;
	virtual bool ThrowItem(uchar, item*);
	virtual bool TryMove(vector2d);
	virtual bool HasHeadOfElpuri() const;
	virtual bool HasMaakotkaShirt() const;
	virtual bool HasPerttusNut() const;
	virtual bool GetFainted() const { return Fainted; }
	virtual bool GetIsPlayer() const { return IsPlayer; }
	virtual bool Apply();
	virtual bool Close();
	virtual bool Consume();
	virtual bool DecreaseGamma();
	virtual bool DecreaseSoftGamma();
	virtual bool Dip();
	virtual bool DrawMessageHistory();
	virtual bool Drop();
	virtual bool Engrave(std::string);
	virtual bool ForceVomit();
	virtual bool GainAllItems();
	virtual bool GoDown();
	virtual bool GoUp();
	virtual bool IncreaseGamma();
	virtual bool IncreaseSoftGamma();
	virtual bool Kick();
	virtual bool Look();
	virtual bool LowerStats();
	virtual bool NOP();
	virtual bool Offer();
	virtual bool Open();
	virtual bool PickUp();
	virtual bool Pray();
	virtual bool Quit();
	virtual bool RaiseStats();
	virtual bool Read();
	virtual bool Save();
	virtual bool ScreenShot();
	virtual bool SeeWholeMap();
	virtual bool ShowInventory();
	virtual bool ShowKeyLayout();
	virtual bool Talk();
	virtual bool Throw();
	virtual bool WalkThroughWalls();
	virtual bool WearArmor();
	virtual bool WhatToEngrave();
	virtual bool Wield();
	virtual bool WizardMode();
	virtual long AddScoreEntry(std::string, float = 1) const;
	virtual long Score() const;
	virtual float GetAttackStrength() const;
	virtual float GetDifficulty() const;
	virtual item* GetTorsoArmor() const				{ return 0; }
	virtual item* GetWielded() const				{ return Wielded; }
	virtual levelsquare* GetLevelSquareUnder() const;
	virtual worldmapsquare* GetWorldMapSquareUnder() const;
	virtual long GetAgilityExperience() const { return AgilityExperience; }
	virtual long GetAP() const { return AP; }
	virtual long GetAPsToBeEaten() const { return APsToBeEaten; }
	virtual long GetEnduranceExperience() const { return EnduranceExperience; }
	virtual long GetNP() const					{ return NP; }
	virtual long GetPerceptionExperience() const { return PerceptionExperience; }
	virtual long GetStrengthExperience() const { return StrengthExperience; }
	virtual short GetHP() const					{ return HP; }
	virtual stack* GetStack() const				{ return Stack; }
	virtual uchar GetBurdenState(ulong = 0) const;
	virtual uchar GetRelations() const { return Relations; }
	virtual uchar GetSex() const { return UNDEFINED; }
	virtual uchar TakeHit(ushort, short, float, character*);
	virtual ulong Danger() const = 0;
	virtual ulong GetBloodColor() const;
	virtual ushort CalculateArmorModifier() const;
	virtual ushort CRegenerationCounter() const { return RegenerationCounter; }
	virtual ushort GetAgility() const					{ return Agility; }
	virtual ushort GetConsumingCurrently() const { return EatingCurrently; }
	virtual ushort GetEmitation() const;
	virtual ushort GetEndurance() const					{ return Endurance; }
	virtual ushort GetPerception() const				{ return Perception; }
	virtual ushort GetStrength() const					{ return Strength; }
	virtual ushort LOSRange() const { return GetPerception() / 3; }
	virtual ushort LOSRangeLevelSquare() const { return GetPerception() * GetPerception() / 9; }
	virtual ushort Possibility() const = 0;
	virtual vector2d GetPos() const;
	virtual void AddBlockMessage(character*) const;
	virtual void AddDodgeMessage(character*) const;
	virtual void AddHitMessage(character*, const bool = false) const;
	virtual void ApplyExperience();
	virtual void BeTalkedTo(character*);
	virtual void ContinueEating();
	virtual void Darkness(long);
	virtual void Die();
	virtual void DrawToTileBuffer() const;
	virtual void HasBeenHitByItem(item*, float);
	virtual void HealFully(character*) {}
	virtual void Hunger(ushort = 1);
	virtual void Move(vector2d, bool = false);
	virtual void MoveRandomly();
	virtual void ReceiveBulimiaDamage();
	virtual void ReceiveFireDamage(long);
	virtual void ReceiveNutrition(long);
	virtual void ReceiveOmleUrineEffect(long);
	virtual void ReceivePepsiEffect(long);
	virtual void ReceiveSchoolFoodEffect(long);
	virtual void ReceiveSound(char*, short, float);
	virtual void Regenerate(ushort = 1);
	virtual void SetAgility(ushort What) { Agility = What; if(short(Agility) < 1) Agility = 1; }
	virtual void SetAgilityExperience(long What) { AgilityExperience = What; }
	virtual void SetAP(long What) { AP = What; }
	virtual void SetAPsToBeEaten(long What) { APsToBeEaten = What; }
	virtual void SetConsumingCurrently(ushort What) { EatingCurrently = What; }
	virtual void SetEndurance(ushort What) { Endurance = What; if(short(Endurance) < 1) Endurance = 1; }
	virtual void SetEnduranceExperience(long What) { EnduranceExperience = What; }
	virtual void SetFainted(bool To) { Fainted = To; }
	virtual void SetHP(short What) { HP = What; }
	virtual void SetIsPlayer(bool What) { IsPlayer = What; }
	virtual void SetNP(long What) { NP = What; }
	virtual void SetPerception(ushort What) { Perception = What; if(short(Perception) < 1) Perception = 1; }
	virtual void SetPerceptionExperience(long What) { PerceptionExperience = What; }
	virtual void SetRegenerationCounter(long What) { RegenerationCounter = What; }
	virtual void SetRelations(uchar What) { Relations = What; }
	virtual void SetSquareUnder(square* Square);
	virtual void SetStrength(ushort What) { Strength = What; if(short(Strength) < 1) Strength = 1; }
	virtual void SetStrengthExperience(long What) { StrengthExperience = What; }
	virtual void SetWielded(item* Something) { Wielded = Something; }
	virtual void SpillBlood(uchar);
	virtual void StopEating();
	virtual void Vomit(ushort);
	virtual void Be();
	virtual bool Zap();
	virtual bool Polymorph();
	virtual bool SetTorsoArmor(item* What) const RET(false)
	virtual void ChangeBackToPlayer();
	virtual bool CanKick() const RET(false)
	virtual void BeKicked(ushort, bool, uchar, character*);
	virtual void FallTo(vector2d, bool);
	virtual bool CheckCannibalism(ushort);
	virtual uchar GetGraphicsContainerIndex() const { return GCHARACTER; }
protected:
	virtual void CreateCorpse();
	virtual std::string DeathMessage() { return Name(DEFINITE) + " dies screaming."; }
	virtual void CreateInitialEquipment() {}
	virtual void SetDefaultStats() = 0;
	virtual void GetPlayerCommand();
	virtual void GetAICommand();
	virtual void Charge(character*);
	virtual float GetMeleeStrength() const				{ return 0; }
	virtual void HostileAICommand();
	virtual void NeutralAICommand();
	virtual void FriendAICommand();
	virtual std::string ThirdPersonWeaponHitVerb(bool Critical) const	{ return NormalThirdPersonHitVerb(Critical); }
	virtual std::string ThirdPersonMeleeHitVerb(bool Critical) const	{ return NormalThirdPersonHitVerb(Critical); }
	virtual std::string FirstPersonHitVerb(character*, bool Critical) const	{ return NormalFirstPersonHitVerb(Critical); }
	virtual std::string AICombatHitVerb(character*, bool Critical) const	{ return NormalThirdPersonHitVerb(Critical); }
	virtual std::string NormalFirstPersonHitVerb(bool Critical) const	{ return Critical ? "critically hit" : "hit"; }
	virtual std::string NormalThirdPersonHitVerb(bool Critical) const	{ return Critical ? "critically hits" : "hits"; }
	virtual std::string FirstPersonBiteVerb(bool Critical) const		{ return Critical ? "critically bite" : "bite"; }
	virtual std::string ThirdPersonBiteVerb(bool Critical) const		{ return Critical ? "critically bites" : "bites"; }
	virtual std::string FirstPersonPSIVerb(bool Critical) const		{ return Critical ? "emit very powerful psychic waves at" : "emit psychic waves at"; }
	virtual std::string ThirdPersonPSIVerb(bool Critical) const		{ return Critical ? "emits very powerful psychic waves at" : "emits psychic waves at"; }
	virtual std::string FirstPersonBrownSlimeVerb(bool Critical) const	{ return Critical ? "vomit extremely acidous brown slime at" : "vomit brown slime at"; }
	virtual std::string ThirdPersonBrownSlimeVerb(bool Critical) const	{ return Critical ? "vomits extremely acidous brown slime at" : "vomits brown slime at"; }
	virtual std::string FirstPersonPepsiVerb(bool Critical) const		{ return Critical ? "vomit extremely stale pepsi at" : "vomit pepsi at"; }
	virtual std::string ThirdPersonPepsiVerb(bool Critical) const		{ return Critical ? "vomits extremely stale pepsi at" : "vomits pepsi at"; }
	stack* Stack;
	item* Wielded;
	ushort Strength, Endurance, Agility, Perception, RegenerationCounter;
	short HP;
	long NP, AP;
	long StrengthExperience, EnduranceExperience, AgilityExperience, PerceptionExperience;
	uchar Relations;
	bool Fainted;
	ushort EatingCurrently;
	long APsToBeEaten;
	bool IsPlayer;
};

#ifdef __FILE_OF_STATIC_PROTOTYPE_DECLARATIONS__

	#define CHARACTER_PROTOINSTALLER(name, base, initmaterials, setstats)\
	\
	class name##_protoinstaller\
	{\
	public:\
		name##_protoinstaller() : Index(protocontainer<character>::Add(new name(false, false, false, false))) {}\
		ushort GetIndex() const { return Index; }\
	private:\
		ushort Index;\
	} static name##_ProtoInstaller;\
	\
	name::name(bool CreateMaterials, bool SetStats, bool CreateEquipment, bool AddToPool) : base(false, false, false, AddToPool) { if(CreateMaterials) initmaterials ; if(SetStats) { SetDefaultStats(); SetHP(GetEndurance() * 2); } if(CreateEquipment) CreateInitialEquipment(); }\
	character* name::Clone(bool CreateMaterials, bool SetStats, bool CreateEquipment) const { return new name(CreateMaterials, SetStats, CreateEquipment); }\
	typeable* name::CloneAndLoad(inputfile& SaveFile) const { character* Char = new name(false, false, false); Char->Load(SaveFile); return Char; }\
	void name::SetDefaultStats() { setstats }\
	ushort name::StaticType() { return name##_ProtoInstaller.GetIndex(); }\
	const character* const name::GetPrototype() { return protocontainer<character>::GetProto(StaticType()); }\
	ushort name::Type() const { return name##_ProtoInstaller.GetIndex(); }

#else

	#define CHARACTER_PROTOINSTALLER(name, base, initmaterials, setstats)

#endif

#define CHARACTER(name, base, initmaterials, setstats, data)\
\
name : public base\
{\
public:\
	name(bool = true, bool = true, bool = true, bool = true);\
	name(material* Material, bool SetStats = true, bool CreateEquipment = true) : base(false, false, false) { InitMaterials(Material); if(SetStats) { SetDefaultStats(); SetHP(GetEndurance() * 2); } if(CreateEquipment) CreateInitialEquipment(); }\
	virtual character* Clone(bool = true, bool = true, bool = true) const;\
	virtual typeable* CloneAndLoad(inputfile&) const;\
	static ushort StaticType();\
	static const character* const GetPrototype();\
	virtual std::string ClassName() const { return #name; }\
protected:\
	virtual void SetDefaultStats();\
	virtual ushort Type() const;\
	data\
}; CHARACTER_PROTOINSTALLER(name, base, initmaterials, setstats)

#define ABSTRACT_CHARACTER(name, base, data)\
\
name : public base\
{\
public:\
	name(bool CreateMaterials, bool SetStats, bool CreateEquipment, bool AddToPool = true) : base(CreateMaterials, SetStats, CreateEquipment, AddToPool) {}\
	data\
};

#endif
