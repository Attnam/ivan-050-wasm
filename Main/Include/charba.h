#ifndef __CHARBA_H__
#define __CHARBA_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include <list>
#include <map>
#include <vector>

#include "typedef.h"
#include "vector2d.h"
#include "id.h"
#include "entity.h"
#include "ivandef.h"

#define CHARDESCRIPTION(Case) Description(Case).c_str()

class felist;
class square;
class bitmap;
class character;
class item;
class stack;
class material;
class lsquare;
class wsquare;
class outputfile;
class inputfile;
class team;
class torso;
class humanoidtorso;
class bodypart;
class characterslot;
class action;
class go;
class gweaponskill;
class stackslot;
class god;
typedef std::list<stackslot*>::iterator stackiterator;
template <class type> class database;

struct characterdatabase
{
  void InitDefaults() { IsAbstract = false; }
  ushort DefaultArmStrength;
  ushort DefaultLegStrength;
  ushort DefaultDexterity;
  ushort DefaultAgility;
  ushort DefaultEndurance;
  ushort DefaultPerception;
  ushort DefaultIntelligence;
  ushort DefaultWisdom;
  ushort DefaultCharisma;
  ushort DefaultMana;
  ulong DefaultMoney;
  ushort TotalSize;
  bool CanRead;
  bool IsCharmable;
  uchar Sex;
  ulong BloodColor;
  bool CanBeGenerated;
  uchar CriticalModifier;
  std::string StandVerb;
  bool CanOpen;
  bool CanBeDisplaced;
  ushort Frequency;
  bool CanWalk;
  bool CanSwim;
  bool CanFly;
  ushort PhysicalDamageResistance;
  ushort SoundResistance;
  ushort EnergyResistance;
  ushort AcidResistance;
  ushort FireResistance;
  ushort PoisonResistance;
  ushort BulimiaResistance;
  bool IsUnique;
  ushort ConsumeFlags;
  ulong TotalVolume;
  std::string TalkVerb;
  vector2d HeadBitmapPos;
  vector2d TorsoBitmapPos;
  vector2d ArmBitmapPos;
  vector2d LegBitmapPos;
  vector2d RightArmBitmapPos;
  vector2d LeftArmBitmapPos;
  vector2d RightLegBitmapPos;
  vector2d LeftLegBitmapPos;
  vector2d GroinBitmapPos;
  ushort ClothColor;
  ushort SkinColor;
  ushort CapColor;
  ushort HairColor;
  ushort EyeColor;
  ushort TorsoMainColor;
  ushort BeltColor;
  ushort TorsoSpecialColor;
  ushort ArmMainColor;
  ushort ArmSpecialColor;
  ushort LegMainColor;
  ushort LegSpecialColor;
  uchar HeadBonePercentile;
  uchar TorsoBonePercentile;
  uchar ArmBonePercentile;
  uchar RightArmBonePercentile;
  uchar LeftArmBonePercentile;
  uchar GroinBonePercentile;
  uchar LegBonePercentile;
  uchar RightLegBonePercentile;
  uchar LeftLegBonePercentile;
  bool IsNameable;
  ushort BaseEmitation;
  std::string Article;
  std::string Adjective;
  std::string AdjectiveArticle;
  std::string NameSingular;
  std::string NamePlural;
  std::string PostFix;
  uchar ArticleMode;
  bool IsAbstract;
  bool IsPolymorphable;
  ulong UnarmedStrength;
  ulong BiteStrength;
  ulong KickStrength;
  uchar AttackStyle;
  bool CanUseEquipment;
  bool CanKick;
  bool CanTalk;
  ushort ClassStates;
  bool CanBeWished;
  std::vector<std::string> Alias;
  bool CreateDivineConfigurations;
};

class characterprototype
{
 public:
  friend class database<character>;
  characterprototype(characterprototype*, character* (*)(ushort, bool, bool), const std::string&);
  character* Clone(ushort Config = 0, bool CreateEquipment = true) const { return Cloner(Config, CreateEquipment, false); }
  character* CloneAndLoad(inputfile&) const;
  ushort GetIndex() const { return Index; }
  const characterdatabase* GetDataBase() const { return &DataBase; }
  const characterprototype* GetBase() const { return Base; }
  PROTODATABASEVALUE(ushort, Frequency);
  PROTODATABASEBOOL(CanBeGenerated);
  PROTODATABASEBOOL(IsAbstract);
  PROTODATABASEBOOL(CanBeWished);
  PROTODATABASEVALUE(const std::vector<std::string>&, Alias);
  const std::map<ushort, characterdatabase>& GetConfig() const { return Config; }
  const std::string& GetClassId() const { return ClassId; }
 protected:
  ushort Index;
  characterdatabase DataBase;
  characterprototype* Base;
  std::map<ushort, characterdatabase> Config;
  character* (*Cloner)(ushort, bool, bool);
  std::string ClassId;
};

class character : public entity, public id
{
 public:
  friend class database<character>;
  friend class corpse;
  typedef characterprototype prototype;
  typedef characterdatabase database;
  typedef std::map<ushort, characterdatabase> databasemap;
  character(donothing);
  virtual ~character();
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual bool CanWield() const { return false; }
  virtual bool Catches(item*, float) { return false; }
  virtual bool CheckBulimia() const;
  virtual bool CheckDeath(const std::string&, bool = false);
  virtual bool DodgesFlyingItem(item*, float);
  virtual bool Hit(character*) = 0;
  virtual bool OpenItem();
  virtual bool OpenPos(vector2d);
  virtual bool ReadItem(item*);
  virtual bool TestForPickup(item*) const;
  virtual bool ThrowItem(uchar, item*);
  virtual bool TryMove(vector2d, bool = true);
  virtual bool HasHeadOfElpuri() const;
  virtual bool HasGoldenEagleShirt() const;
  virtual bool HasPetrussNut() const;
  virtual bool IsPlayer() const { return Player; }
  virtual bool Apply();
  virtual bool Close();
  virtual bool Eat();
  virtual bool Drink();
  virtual bool Dip();
  virtual bool DrawMessageHistory();
  virtual bool Drop();
  virtual bool Engrave(const std::string&);
  virtual bool ForceVomit();
  virtual bool GainAllItems();
  virtual bool GoDown();
  virtual bool GoUp();
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
  virtual bool SeeWholeMap();
  virtual bool ShowInventory();
  virtual bool ShowKeyLayout();
  virtual bool ShowWeaponSkills();
  virtual bool Talk();
  virtual bool Throw();
  virtual bool WalkThroughWalls();
  virtual bool EquipmentScreen();
  virtual bool WhatToEngrave();
  virtual bool WizardMode();
  virtual void AddScoreEntry(const std::string&, float = 1, bool = true) const;
  virtual long GetScore() const;
  virtual long GetAP() const { return AP; }
  virtual long GetNP() const { return NP; }
  virtual short GetHP() const;
  virtual stack* GetStack() const { return Stack; }
  virtual uchar GetBurdenState(ulong = 0) const;
  virtual uchar TakeHit(character*, item*, float, float, short, uchar, bool);
  virtual ulong CurrentDanger() const;
  virtual ulong MaxDanger() const;
  virtual ushort GetEmitation() const;
  virtual ushort LOSRange() const;
  virtual ushort LOSRangeSquare() const;
  virtual ushort ESPRange() const;
  virtual ushort ESPRangeSquare() const;
  virtual vector2d GetPos() const;
  virtual void AddMissMessage(character*) const;
  virtual void AddPrimitiveHitMessage(character*, const std::string&, const std::string&, uchar) const;
  virtual void AddWeaponHitMessage(character*, item*, uchar, bool = false) const;
  virtual void ApplyExperience();
  virtual void BeTalkedTo(character*);
  virtual void ReceiveDarkness(long);
  virtual void Die(bool = false);
  virtual void HasBeenHitByItem(character*, item*, float);
  virtual void HealFully(character*) { }
  virtual void Hunger(ushort = 1);
  virtual void Move(vector2d, bool = false);
  virtual bool MoveRandomly();
  virtual void ReceiveNutrition(long);
  virtual void ReceiveOmleUrine(long);
  virtual void ReceivePepsi(long);
  virtual void ReceivePoison(long);
  virtual void Regenerate(ushort = 1);
  virtual void SetAP(long What) { AP = What; }
  virtual void SetIsPlayer(bool What) { Player = What; }
  virtual void SetNP(long);
  virtual void SpillBlood(uchar);
  virtual void SpillBlood(uchar, vector2d);
  virtual void Vomit(ushort);
  virtual void Be();
  virtual bool Zap();
  virtual bool Polymorph(character*, ushort);
  virtual void BeKicked(character*, float, float, short, bool);
  virtual void FallTo(character*, vector2d);
  virtual bool CheckCannibalism(ushort) const;
  virtual void ActivateTemporaryState(ushort What) { TemporaryState |= What; }
  virtual void DeActivateTemporaryState(ushort What) { TemporaryState &= ~What; }
  virtual void ActivateEquipmentState(ushort What) { EquipmentState |= What; }
  virtual void DeActivateEquipmentState(ushort What) { EquipmentState &= ~What; }
  virtual bool TemporaryStateIsActivated(ushort What) const { return TemporaryState & What ? true : false; }	
  virtual bool EquipmentStateIsActivated(ushort What) const { return EquipmentState & What ? true : false; }
  virtual bool StateIsActivated(ushort What) const { return TemporaryState & What || EquipmentState & What; }
  virtual void Faint();
  virtual void SetTemporaryStateCounter(ushort, ushort);
  virtual void DeActivateVoluntaryAction(const std::string& = "");
  virtual void ActionAutoTermination();
  virtual team* GetTeam() const { return Team; }
  virtual void SetTeam(team*);
  virtual void ChangeTeam(team*);
  virtual bool RestUntilHealed();
  virtual bool OutlineCharacters();
  virtual bool OutlineItems();
  virtual short GetMaxHP() const;
  virtual uchar GetMoveEase() const;
  virtual float CalculateDodgeValue() const;
  virtual bool RaiseGodRelations();
  virtual bool LowerGodRelations();
  virtual bool GainDivineKnowledge();
  virtual ulong GetMoney() const { return Money; }
  virtual void SetMoney(ulong What) { Money = What; }
  virtual void EditMoney(long What) { Money += What; }
  virtual void SetHomeRoom(uchar What) { HomeRoom = What; }
  virtual uchar GetHomeRoom() const { return HomeRoom; }
  virtual bool Displace(character*);
  virtual bool Sit();
  virtual void AddSpecialItemInfo(std::string&, item*) { }
  virtual void AddSpecialItemInfoDescription(std::string&) { }
  virtual long GetStatScore() const;
  virtual bool CheckStarvationDeath(const std::string&);
  virtual void ShowNewPosInfo() const;
  virtual void Hostility(character*);
  virtual stack* GetGiftStack() const;
  virtual bool MoveRandomlyInRoom();
  virtual bool Go();
  virtual bool ShowConfigScreen();
  virtual std::list<character*>::iterator GetTeamIterator() { return TeamIterator; }
  virtual void SetTeamIterator(std::list<character*>::iterator What) { TeamIterator = What; }
  virtual void ReceiveKoboldFlesh(long);
  virtual bool ChangeRandomStat(short);
  virtual uchar RandomizeReply(uchar, bool*);
  virtual ushort DangerLevel() const;
  virtual void CreateInitialEquipment() { }
  virtual void DisplayInfo(std::string&);
  virtual bool SpecialEnemySightedReaction(character*) { return false; }
  virtual void TestWalkability();
  virtual void EditAP(long What) { AP += What; if(AP > 1200) AP = 1200; }
  virtual void EditNP(long What) { NP += What; }
  virtual void SetSize(ushort);
  virtual ushort GetSize() const;
  virtual torso* GetTorso() const;
  virtual humanoidtorso* GetHumanoidTorso() const;
  virtual void SetTorso(torso* What);
  virtual bodypart* GetBodyPart(ushort) const;
  virtual void SetBodyPart(ushort, bodypart*);
  virtual void SetMainMaterial(material*);
  virtual void ChangeMainMaterial(material*);
  virtual void SetSecondaryMaterial(material*);
  virtual void ChangeSecondaryMaterial(material*);
  virtual void SetContainedMaterial(material*);
  virtual void ChangeContainedMaterial(material*);
  virtual void Teleport(vector2d);
  virtual bool SecretKnowledge();
  virtual void RestoreHP();
  virtual bool ReceiveDamage(character*, short, uchar, uchar = ALL, uchar = 8, bool = false, bool = false, bool = false);
  virtual bool ReceiveBodyPartDamage(character*, short, uchar, uchar, uchar = 8, bool = false, bool = false);
  virtual bool BodyPartVital(ushort) const { return true; }
  virtual void RestoreBodyParts();
  virtual bool AssignName();
  virtual std::string GetAssignedName() const { return AssignedName; }
  virtual void SetAssignedName(const std::string& What) { AssignedName = What; }
  virtual std::string Description(uchar) const;
  virtual std::string PersonalPronoun() const;
  virtual std::string PossessivePronoun() const;
  virtual std::string ObjectPronoun() const;
  virtual bool BodyPartCanBeSevered(ushort) const;
  //virtual std::string GetName(uchar) const;
  virtual void AddName(std::string&, uchar) const;
  virtual void ReceiveHeal(long);
  virtual item* GetMainWielded() const { return 0; }
  virtual item* GetSecondaryWielded() const { return 0; }
  virtual item* GetBodyArmor() const { return 0; }
  virtual void SetMainWielded(item*) { }
  virtual void SetSecondaryWielded(item*) { }
  virtual void SetBodyArmor(item*) { }
  virtual uchar GetHungerState() const;
  virtual characterslot* GetTorsoSlot() const { return GetBodyPartSlot(0); }
  virtual characterslot* GetBodyPartSlot(ushort) const;
  virtual bool ConsumeItem(item*);
  virtual bool CanConsume(material*) const;
  virtual action* GetAction() const { return Action; }
  virtual void SetAction(action* What) { Action = What; }
  virtual void SwitchToDig(item*, vector2d) { }
  virtual void SetRightWielded(item*) { }
  virtual void SetLeftWielded(item*) { }
  virtual void GoOn(go*);
  virtual bool CheckKick() const;
  virtual uchar OpenMultiplier() const { return 2; }
  virtual uchar CloseMultiplier() const { return 2; }
  virtual bool CheckThrow() const { return true; }  
  virtual bool CheckOffer() const { return true; }
  virtual ushort GetTemporaryStateCounter(ushort) const;
  virtual void EditTemporaryStateCounter(ushort, short);
  virtual void BlockDamageType(uchar);
  virtual bool AllowDamageTypeBloodSpill(uchar) const;
  virtual bool DamageTypeCanSeverBodyPart(uchar) const;
  virtual bool DrawSilhouette(bitmap*, vector2d) const { return false; }
  virtual void TalkTo(character*);
  virtual bool ClosePos(vector2d);
  virtual ushort GetResistance(uchar) const;
  virtual ushort GlobalResistance(uchar Type) const { return GetResistance(Type); }
  virtual std::string EquipmentName(ushort) const { return ""; }
  virtual bodypart* GetBodyPartOfEquipment(ushort) const { return 0; }
  virtual item* GetEquipment(ushort) const { return 0; }
  virtual ushort EquipmentSlots() const { return 0; }
  virtual bool (*EquipmentSorter(ushort) const)(item*, character*) { return 0; }
  virtual void SetEquipment(ushort, item*) { }
  virtual bool ScrollMessagesDown();
  virtual bool ScrollMessagesUp();
  virtual void AddHealingLiquidConsumeEndMessage() const;
  virtual void AddSchoolFoodConsumeEndMessage() const;
  virtual void AddSchoolFoodHitMessage() const;
  virtual void AddOmleUrineConsumeEndMessage() const;
  virtual void AddPepsiConsumeEndMessage() const;
  virtual void AddFrogFleshConsumeEndMessage() const;
  virtual void AddKoboldFleshConsumeEndMessage() const;
  virtual void AddKoboldFleshHitMessage() const;
  virtual void AddBoneConsumeEndMessage() const;
  virtual void AddInfo(felist&) const = 0;
  virtual void PrintInfo() const;
  virtual bodypart* SevereBodyPart(ushort);
  virtual bool IsAnimated() const { return false; }
  virtual void CompleteRiseFromTheDead();
  virtual bool RaiseTheDead(character*);
  virtual void CreateBodyPart(ushort);
  virtual bool CanUseEquipment(ushort Index) const { return CanUseEquipment() && Index < EquipmentSlots() && GetBodyPartOfEquipment(Index); }
  virtual const prototype* GetProtoType() const { return &character_ProtoType; }
  const database* GetDataBase() const { return DataBase; }
  virtual void SetParameters(uchar) { }
  DATABASEVALUE(ushort, DefaultArmStrength);
  DATABASEVALUE(ushort, DefaultLegStrength);
  DATABASEVALUE(ushort, DefaultDexterity);
  DATABASEVALUE(ushort, DefaultAgility);
  DATABASEVALUE(ushort, DefaultEndurance);
  DATABASEVALUE(ushort, DefaultPerception);
  DATABASEVALUE(ushort, DefaultIntelligence);
  DATABASEVALUE(ushort, DefaultWisdom);
  DATABASEVALUE(ushort, DefaultCharisma);
  DATABASEVALUE(ushort, DefaultMana);
  DATABASEVALUE(ulong, DefaultMoney);
  DATABASEVALUE(ushort, TotalSize);
  DATABASEBOOL(CanRead);
  DATABASEBOOL(IsCharmable);
  DATABASEVALUE(uchar, Sex);
  DATABASEVALUE(ulong, BloodColor);
  DATABASEBOOL(CanBeGenerated);
  DATABASEVALUE(uchar, CriticalModifier);
  DATABASEVALUE(const std::string&, StandVerb);
  DATABASEBOOL(CanOpen);
  DATABASEBOOL(CanBeDisplaced);
  DATABASEVALUE(ushort, Frequency);
  DATABASEBOOL(CanWalk);
  DATABASEBOOL(CanSwim);
  DATABASEBOOL(CanFly);
  DATABASEVALUE(ushort, PhysicalDamageResistance);
  DATABASEVALUE(ushort, SoundResistance);
  DATABASEVALUE(ushort, EnergyResistance);
  DATABASEVALUE(ushort, AcidResistance);
  DATABASEVALUE(ushort, FireResistance);
  DATABASEVALUE(ushort, PoisonResistance);
  DATABASEVALUE(ushort, BulimiaResistance);
  DATABASEBOOL(IsUnique);
  DATABASEVALUE(ushort, ConsumeFlags);
  DATABASEVALUE(ulong, TotalVolume);
  DATABASEVALUE(const std::string&, TalkVerb);
  DATABASEVALUEWITHPARAMETER(vector2d, HeadBitmapPos, ushort);
  DATABASEVALUEWITHPARAMETER(vector2d, TorsoBitmapPos, ushort);
  DATABASEVALUEWITHPARAMETER(vector2d, ArmBitmapPos, ushort);
  DATABASEVALUEWITHPARAMETER(vector2d, LegBitmapPos, ushort);
  DATABASEVALUEWITHPARAMETER(vector2d, RightArmBitmapPos, ushort);
  DATABASEVALUEWITHPARAMETER(vector2d, LeftArmBitmapPos, ushort);
  DATABASEVALUEWITHPARAMETER(vector2d, RightLegBitmapPos, ushort);
  DATABASEVALUEWITHPARAMETER(vector2d, LeftLegBitmapPos, ushort);
  DATABASEVALUEWITHPARAMETER(vector2d, GroinBitmapPos, ushort);
  DATABASEVALUEWITHPARAMETER(ushort, ClothColor, ushort);
  DATABASEVALUEWITHPARAMETER(ushort, SkinColor, ushort);
  DATABASEVALUEWITHPARAMETER(ushort, CapColor, ushort);
  DATABASEVALUEWITHPARAMETER(ushort, HairColor, ushort);
  DATABASEVALUEWITHPARAMETER(ushort, EyeColor, ushort);
  DATABASEVALUEWITHPARAMETER(ushort, TorsoMainColor, ushort);
  DATABASEVALUEWITHPARAMETER(ushort, BeltColor, ushort);
  DATABASEVALUEWITHPARAMETER(ushort, TorsoSpecialColor, ushort);
  DATABASEVALUEWITHPARAMETER(ushort, ArmMainColor, ushort);
  DATABASEVALUEWITHPARAMETER(ushort, ArmSpecialColor, ushort);
  DATABASEVALUEWITHPARAMETER(ushort, LegMainColor, ushort);
  DATABASEVALUEWITHPARAMETER(ushort, LegSpecialColor, ushort);
  DATABASEVALUE(uchar, HeadBonePercentile);
  DATABASEVALUE(uchar, TorsoBonePercentile);
  DATABASEVALUE(uchar, ArmBonePercentile);
  DATABASEVALUE(uchar, RightArmBonePercentile);
  DATABASEVALUE(uchar, LeftArmBonePercentile);
  DATABASEVALUE(uchar, GroinBonePercentile);
  DATABASEVALUE(uchar, LegBonePercentile);
  DATABASEVALUE(uchar, RightLegBonePercentile);
  DATABASEVALUE(uchar, LeftLegBonePercentile);
  DATABASEBOOL(IsNameable);
  DATABASEVALUE(ushort, BaseEmitation);
  DATABASEVALUE(const std::string&, Article);
  DATABASEVALUE(const std::string&, Adjective);
  DATABASEVALUE(const std::string&, AdjectiveArticle);
  DATABASEVALUE(const std::string&, NameSingular);
  DATABASEVALUE(const std::string&, NamePlural);
  DATABASEVALUE(const std::string&, PostFix);
  DATABASEVALUE(uchar, ArticleMode);
  DATABASEBOOL(IsPolymorphable);
  DATABASEVALUE(ulong, UnarmedStrength);
  DATABASEVALUE(ulong, BiteStrength);
  DATABASEVALUE(ulong, KickStrength);
  DATABASEVALUE(uchar, AttackStyle);
  DATABASEBOOL(CanUseEquipment);
  DATABASEBOOL(CanKick);
  DATABASEBOOL(CanTalk);
  DATABASEVALUE(ushort, ClassStates);
  DATABASEBOOL(CanBeWished);
  DATABASEVALUE(const std::vector<std::string>&, Alias);
  ushort GetType() const { return GetProtoType()->GetIndex(); }
  virtual void TeleportRandomly();
  virtual bool TeleportNear(character*);
  static character* Clone(ushort, bool, bool) { return 0; }
  virtual bool IsStuck() const;
  virtual void InitSpecialAttributes() { }
  virtual void Kick(lsquare*) = 0;
  virtual void SpecialBiteEffect(character*) { }
  virtual float GetAttackStrengthDanger() const = 0;
  virtual ushort GetAttribute(ushort Identifier) const { return BaseAttribute[Identifier]; }
  virtual bool EditAttribute(ushort Identifier, short Value) { return RawEditAttribute(BaseAttribute[Identifier], Value); }
  virtual void EditExperience(ushort Identifier, long Value) { BaseExperience[Identifier] += Value; }
  virtual bool CheckForAttributeIncrease(ushort&, long&, bool = false);
  virtual bool CheckForAttributeDecrease(ushort&, long&, bool = false);
  virtual bool RawEditAttribute(ushort&, short&, bool = false);
  virtual void DrawPanel() const;
  virtual ushort DrawStats() const = 0;
  virtual ushort GetCarryingStrength() const = 0;
  virtual ulong GetOriginalBodyPartID(ushort Index) const { return OriginalBodyPartID[Index]; }
  virtual void SetOriginalBodyPartID(ushort Index, ulong ID) { OriginalBodyPartID[Index] = ID; }
  virtual bool DamageTypeAffectsInventory(uchar) const;
  virtual void SetStuckTo(item* What) {  StuckTo = What; }
  virtual item* GetStuckTo() const { return StuckTo; }
  virtual void SetStuckToBodyPart(ushort What) { StuckToBodyPart = What; }
  virtual ushort GetStuckToBodyPart() const { return StuckToBodyPart; }
  virtual bool TryToUnstuck(vector2d);
  virtual ushort GetRandomStepperBodyPart() const { return TORSOINDEX; }
  entity* GetMotherEntity() const { return MotherEntity; }
  void SetMotherEntity(entity* What) { MotherEntity = What; }
  virtual void EditVolume(long);
  virtual void EditWeight(long);
  virtual ushort CheckForBlock(character*, item*, float, ushort Damage, short, uchar) { return Damage; }
  virtual ushort CheckForBlockWithItem(character*, item*, item*, float, float, ushort, short, uchar);
  virtual void AddBlockMessage(character*, item*, const std::string&, bool) const;
  virtual character* GetPolymorphBackup() const { return PolymorphBackup; }
  virtual void SetPolymorphBackup(character* What) { PolymorphBackup = What; }
  virtual gweaponskill* GetCategoryWeaponSkill(ushort Index) const { return CategoryWeaponSkill[Index]; }
  virtual bool AddSpecialSkillInfo(felist&) const { return false; }
  virtual bool CheckBalance(float);
  virtual long CalculateStateAPGain(long) const;
  virtual long CalculateMoveAPRequirement(uchar Difficulty) const { return (long(GetAttribute(AGILITY)) - 200) * Difficulty * GetMoveEase() / 20; }
  virtual bool EquipmentHasNoPairProblems(ushort) const { return true; }
  virtual void SignalEquipmentAdd(ushort);
  virtual void SignalEquipmentRemoval(ushort);
  ushort GetConfig() const { return Config; }
  virtual void BeginTemporaryState(ushort, ushort);
  virtual void GainIntrinsic(ushort What) { BeginTemporaryState(What, 0); }
  virtual void LoseIntrinsic(ushort);
  virtual void HandleStates();
  virtual void PrintBeginPolymorphControlMessage() const;
  virtual void PrintEndPolymorphControlMessage() const;
  virtual void PrintBeginLifeSaveMessage() const;
  virtual void PrintEndLifeSaveMessage() const;
  virtual void PrintBeginLycanthropyMessage() const;
  virtual void PrintEndLycanthropyMessage() const;
  virtual void PrintBeginHasteMessage() const;
  virtual void PrintEndHasteMessage() const;
  virtual void PrintBeginSlowMessage() const;
  virtual void PrintEndSlowMessage() const;
  virtual void EndPolymorph();
  virtual void LycanthropyHandler();
  virtual void SaveLife();
  virtual void BeginInvisibility();
  virtual void BeginInfraVision();
  virtual void BeginESP();
  virtual void EndInvisibility();
  virtual void EndInfraVision();
  virtual void EndESP();
  virtual void PolymorphRandomly(ushort);
  virtual bool EquipmentEasilyRecognized(ushort) const { return true; }
  virtual void StartReading(item*, ulong);
  virtual void DexterityAction(ushort);
  virtual void CharacterSpeciality() { }
  virtual void PrintBeginInvisibilityMessage() const;
  virtual void PrintEndInvisibilityMessage() const;
  virtual void PrintBeginInfraVisionMessage() const;
  virtual void PrintEndInfraVisionMessage() const;
  virtual void PrintBeginESPMessage() const;
  virtual void PrintEndESPMessage() const;
  virtual bool CanBeSeenByPlayer() const;
  virtual bool CanBeSeenBy(character*) const;
  virtual bool DetachBodyPart();
  virtual bodypart* MakeBodyPart(ushort);
  virtual void AttachBodyPart(bodypart*, ushort);
  virtual uchar GetBodyParts() const { return 1; }
  virtual bool HasAllBodyParts() const;
  virtual stackiterator FindRandomOwnBodyPart();
  virtual bodypart* GenerateRandomBodyPart();
  virtual std::vector<character*> GetFriendsAround() const;
  virtual bodypart* TryAttachRandomOldBodyPart();
  virtual bodypart* AttachOldBodyPartFromStack(stackiterator, stack*);
  virtual void PrintBeginPoisonedMessage() const;
  virtual void PrintEndPoisonedMessage() const;
  virtual bool IsWarm() const;
  virtual void CalculateEquipmentState();
  virtual void Draw(bitmap*, vector2d, ushort, bool, bool) const;
  virtual void DrawBodyParts(bitmap*, vector2d, ushort, bool, bool) const;
  void SetConfig(ushort);
  virtual god* GetMasterGod() const;
  virtual square* GetSquareUnder() const { return SquareUnder; }
  void SetSquareUnder(square* What) { SquareUnder = What; }
  lsquare* GetLSquareUnder() const;
  wsquare* GetWSquareUnder() const;
  virtual void PoisonedHandler();
 protected:
  virtual void SpecialTurnHandler() { }
  virtual uchar AllowedWeaponSkillCategories() const { return MARTIAL_SKILL_CATEGORIES; }
  void Initialize(uchar, bool, bool);
  virtual void VirtualConstructor(bool);
  virtual void LoadDataBaseStats();
  void InstallDataBase();
  virtual vector2d GetBodyPartBitmapPos(ushort, ushort);
  virtual ushort GetBodyPartColorA(ushort, ushort);
  virtual ushort GetBodyPartColorB(ushort, ushort);
  virtual ushort GetBodyPartColorC(ushort, ushort);
  virtual ushort GetBodyPartColorD(ushort, ushort);
  virtual ushort GetBodyPartAnimationFrames(ushort) const { return 1; }
  virtual ulong GetBodyPartSize(ushort, ushort);
  virtual ulong GetBodyPartVolume(ushort);
  virtual uchar GetBodyPartBonePercentile(ushort);
  virtual void UpdateBodyPartPicture(ushort);
  virtual void UpdateBodyPartPictures();
  virtual uchar ChooseBodyPartToReceiveHit(float, float);
  virtual void CreateBodyParts();
  virtual material* CreateBodyPartFlesh(ushort, ulong) const = 0;
  virtual material* CreateBodyPartBone(ushort, ulong) const;
  virtual bool AddMaterialDescription(std::string&, bool) const;
  virtual bool ShowClassDescription() const { return true; }
  virtual void SeekLeader();
  virtual bool CheckForUsefulItemsOnGround();
  virtual bool CheckForDoors();
  virtual bool CheckForEnemies(bool);
  virtual bool FollowLeader();
  virtual void StandIdleAI();
  virtual void CreateCorpse();
  virtual std::string GetDeathMessage() { return GetName(DEFINITE) + " is slain."; }
  virtual void GetPlayerCommand();
  virtual void GetAICommand();
  virtual bool MoveTowards(vector2d);
  virtual std::string FirstPersonUnarmedHitVerb() const { return "hit"; }
  virtual std::string FirstPersonCriticalUnarmedHitVerb() const { return "critically hit"; }
  virtual std::string ThirdPersonUnarmedHitVerb() const { return "hits"; }
  virtual std::string ThirdPersonCriticalUnarmedHitVerb() const { return "critically hits"; }
  virtual std::string FirstPersonKickVerb() const { return "kick"; }
  virtual std::string FirstPersonCriticalKickVerb() const { return "critically kick"; }
  virtual std::string ThirdPersonKickVerb() const { return "kicks"; }
  virtual std::string ThirdPersonCriticalKickVerb() const { return "critically kicks"; }
  virtual std::string FirstPersonBiteVerb() const { return "bite"; }
  virtual std::string FirstPersonCriticalBiteVerb() const { return "critically bite"; }
  virtual std::string ThirdPersonBiteVerb() const { return "bites"; }
  virtual std::string ThirdPersonCriticalBiteVerb() const { return "critically bites"; }
  virtual std::string UnarmedHitNoun() const { return "hit"; }
  virtual std::string KickNoun() const { return "kick"; }
  virtual std::string BiteNoun() const { return "attack"; }
  virtual bool AttackIsBlockable(uchar) const { return true; }
  virtual uchar GetSpecialBodyPartFlags(ushort, ushort) const { return STNORMAL; }
  stack* Stack;
  long NP, AP;
  bool Player;
  ushort TemporaryState;
  short TemporaryStateCounter[STATES];
  team* Team;
  vector2d WayPoint;
  ulong Money;
  uchar HomeRoom;
  std::list<character*>::iterator TeamIterator;
  characterslot* BodyPartSlot;
  std::string AssignedName;
  action* Action;
  ushort Config;
  const database* DataBase;
  ushort StuckToBodyPart;
  item* StuckTo; // Bad naming. Sorry.
  ushort BaseAttribute[BASEATTRIBUTES];
  long BaseExperience[BASEATTRIBUTES];
  ulong* OriginalBodyPartID;
  entity* MotherEntity;
  character* PolymorphBackup;
  gweaponskill** CategoryWeaponSkill;
  ushort EquipmentState;
  static void (character::*PrintBeginStateMessage[])() const;
  static void (character::*PrintEndStateMessage[])() const;
  static void (character::*BeginStateHandler[])();
  static void (character::*EndStateHandler[])();
  static void (character::*StateHandler[])();
  static std::string StateDescription[];
  static bool StateIsSecret[];
  square* SquareUnder;
  static prototype character_ProtoType;
};

#ifdef __FILE_OF_STATIC_CHARACTER_PROTOTYPE_DEFINITIONS__
#define CHARACTER_PROTOTYPE(name, baseproto) characterprototype name::name##_ProtoType(baseproto, &name::Clone, #name);
#else
#define CHARACTER_PROTOTYPE(name, baseproto)
#endif

#define CHARACTER(name, base, data)\
\
name : public base\
{\
 public:\
  name(ushort Config = 0, bool CreateEquipment = true, bool Load = false) : base(donothing()) { Initialize(Config, CreateEquipment, Load); }\
  name(donothing D) : base(D) { }\
  virtual const prototype* GetProtoType() const { return &name##_ProtoType; }\
  static character* Clone(ushort Config, bool CreateEquipment, bool Load) { return new name(Config, CreateEquipment, Load); }\
 protected:\
  static prototype name##_ProtoType;\
  data\
}; CHARACTER_PROTOTYPE(name, &base##_ProtoType);

#define ABSTRACT_CHARACTER(name, base, data)\
\
name : public base\
{\
 public:\
  name(donothing D) : base(D) { }\
  virtual const prototype* GetProtoType() const { return &name##_ProtoType; }\
  static character* Clone(ushort, bool, bool) { return 0; }\
 protected:\
  static prototype name##_ProtoType;\
  data\
}; CHARACTER_PROTOTYPE(name, &base##_ProtoType);

#endif

