#ifndef __CHAR_H__
#define __CHAR_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include "bodypart.h"
#include "script.h"

#define CHAR_PERSONAL_PRONOUN GetPersonalPronoun(true).c_str()
#define CHAR_POSSESSIVE_PRONOUN GetPossessivePronoun(true).c_str()
#define CHAR_OBJECT_PRONOUN GetObjectPronoun(true).c_str()
#define CHAR_PERSONAL_PRONOUN_THIRD_PERSON_VIEW GetPersonalPronoun(false).c_str()
#define CHAR_POSSESSIVE_PRONOUN_THIRD_PERSON_VIEW GetPossessivePronoun(false).c_str()
#define CHAR_OBJECT_PRONOUN_THIRD_PERSON_VIEW GetObjectPronoun(false).c_str()

class go;
class team;
class wsquare;
class cweaponskill;
struct homedata;

typedef std::vector<std::pair<float, ushort> > blockvector;

inline long APBonus(long Attribute) { return Attribute >= 10 ? 90 + Attribute : 50 + Attribute * 5; }

struct characterdatabase
{
  void InitDefaults(ushort) { IsAbstract = false; }
  bool AllowRandomInstantiation() const { return CanBeGenerated && !IsUnique; }
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
  ushort FireResistance;
  ushort PoisonResistance;
  ushort ElectricityResistance;
  bool IsUnique;
  ushort ConsumeFlags;
  ulong TotalVolume;
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
  ushort BootColor;
  ushort TorsoSpecialColor;
  ushort ArmMainColor;
  ushort ArmSpecialColor;
  ushort LegMainColor;
  ushort LegSpecialColor;
  bool IsNameable;
  ulong BaseEmitation;
  std::string Article;
  std::string Adjective;
  std::string AdjectiveArticle;
  std::string NameSingular;
  std::string NamePlural;
  std::string PostFix;
  uchar ArticleMode;
  bool IsAbstract;
  bool IsPolymorphable;
  ulong BaseUnarmedStrength;
  ulong BaseBiteStrength;
  ulong BaseKickStrength;
  uchar AttackStyle;
  bool CanUseEquipment;
  bool CanKick;
  bool CanTalk;
  ulong ClassStates;
  bool CanBeWished;
  std::vector<std::string> Alias;
  bool CreateDivineConfigurations;
  bool CreateGolemMaterialConfigurations;
  contentscript<item> Helmet;
  contentscript<item> Amulet;
  contentscript<item> Cloak;
  contentscript<item> BodyArmor;
  contentscript<item> Belt;
  contentscript<item> RightWielded;
  contentscript<item> LeftWielded;
  contentscript<item> RightRing;
  contentscript<item> LeftRing;
  contentscript<item> RightGauntlet;
  contentscript<item> LeftGauntlet;
  contentscript<item> RightBoot;
  contentscript<item> LeftBoot;
  short AttributeBonus;
  std::vector<long> KnownCWeaponSkills;
  std::vector<long> CWeaponSkillHits;
  ushort RightSWeaponSkillHits;
  ushort LeftSWeaponSkillHits;
  uchar PanicLevel;
  bool CanBeCloned;
  std::list<contentscript<item> > Inventory;
  ushort DangerModifier;
  std::string DefaultName;
  std::vector<std::string> FriendlyReplies;
  std::vector<std::string> HostileReplies;
  bool CanZap;
  ushort FleshMaterial;
  bool HasFeet;
  std::string DeathMessage;
  bool IgnoreDanger;
  ushort HPRequirementForGeneration;
  bool IsExtraCoward;
  bool SpillsBlood;
  bool HasEyes;
  bool HasHead;
  bool CanThrow;
  bool UsesNutrition;
  ushort AttackWisdomLimit;
  bool CanWalkThroughWalls;
  uchar AttachedGod;
  bool BodyPartsDisappearWhenSevered;
};

class characterprototype
{
 public:
  friend class database<character>;
  characterprototype(characterprototype*, character* (*)(ushort, ushort), const std::string&);
  character* Clone(ushort Config = 0, ushort SpecialFlags = 0) const { return Cloner(Config, SpecialFlags); }
  character* CloneAndLoad(inputfile&) const;
  ushort GetIndex() const { return Index; }
  const characterprototype* GetBase() const { return Base; }
  const std::map<ushort, characterdatabase>& GetConfig() const { return Config; }
  const std::string& GetClassId() const { return ClassId; }
  void CreateSpecialConfigurations();
  const characterdatabase& ChooseBaseForConfig(ushort);
 private:
  ushort Index;
  characterprototype* Base;
  std::map<ushort, characterdatabase> Config;
  character* (*Cloner)(ushort, ushort);
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
  character(const character&);
  virtual ~character();
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual bool CanWield() const { return false; }
  virtual bool Catches(item*) { return false; }
  bool CheckDeath(const std::string&, character*, bool = false);
  bool DodgesFlyingItem(item*, float);
  virtual bool Hit(character*, bool = false) = 0;
  bool OpenPos(vector2d);
  bool ReadItem(item*);
  bool TestForPickup(item*) const;
  void ThrowItem(uchar, item*);
  bool TryMove(vector2d, bool = true);
  bool HasHeadOfElpuri() const;
  bool HasGoldenEagleShirt() const;
  bool HasPetrussNut() const;
  bool HasEncryptedScroll() const;
  bool IsPlayer() const { return Player; }
  bool Engrave(const std::string&);
  void AddScoreEntry(const std::string&, float = 1, bool = true) const;
  long GetScore() const;
  long GetAP() const { return AP; }
  long GetNP() const { return NP; }
  stack* GetStack() const { return Stack; }
  uchar GetBurdenState() const { return BurdenState; }
  bool MakesBurdened(ulong What) const { return ulong(GetCarryingStrength()) * 2500 < What; }
  virtual ushort TakeHit(character*, item*, float, float, short, uchar, bool, bool);
  ushort GetLOSRange() const;
  ushort GetLOSRangeSquare() const { return GetLOSRange() * GetLOSRange(); }
  ushort GetESPRange() const { return GetAttribute(INTELLIGENCE) / 3; }
  ushort GetESPRangeSquare() const { return GetESPRange() * GetESPRange(); }
  void AddMissMessage(const character*) const;
  void AddPrimitiveHitMessage(const character*, const std::string&, const std::string&, ushort) const;
  void AddWeaponHitMessage(const character*, const item*, ushort, bool = false) const;
  virtual void ApplyExperience(bool = false);
  virtual void BeTalkedTo();
  void ReceiveDarkness(long);
  void Die(const std::string& = "", bool = false);
  void HasBeenHitByItem(character*, item*, ushort, float, uchar);
  void Hunger();
  void Move(vector2d, bool = false);
  virtual bool MoveRandomly();
  void ReceiveNutrition(long);
  void ReceiveOmmelUrine(long);
  void ReceivePepsi(long);
  void ReceiveSchoolFood(long);
  void Regenerate();
  void SetAP(long What) { AP = What; }
  void SetIsPlayer(bool What) { Player = What; }
  void SetNP(long);
  void Vomit(ushort);
  virtual void Be();
  bool Polymorph(character*, ushort);
  void BeKicked(character*, item*, float, float, short, bool, bool);
  void FallTo(character*, vector2d);
  bool CheckCannibalism(const material*) const;
  void ActivateTemporaryState(ulong What) { TemporaryState |= What; }
  void DeActivateTemporaryState(ulong What) { TemporaryState &= ~What; }
  void ActivateEquipmentState(ulong What) { EquipmentState |= What; }
  void DeActivateEquipmentState(ulong What) { EquipmentState &= ~What; }
  bool TemporaryStateIsActivated(ulong What) const { return (TemporaryState & What) != 0; }	
  bool EquipmentStateIsActivated(ulong What) const { return (EquipmentState & What) != 0; }
  bool StateIsActivated(ulong What) const { return TemporaryState & What || EquipmentState & What; }
  virtual bool Faint(ushort, bool = false);
  void SetTemporaryStateCounter(ulong, ushort);
  void DeActivateVoluntaryAction(const std::string&);
  void ActionAutoTermination();
  team* GetTeam() const { return Team; }
  void SetTeam(team*);
  void ChangeTeam(team*);
  virtual ushort GetMoveEase() const;
  float GetDodgeValue() const { return DodgeValue; }
  ulong GetMoney() const { return Money; }
  void SetMoney(ulong What) { Money = What; }
  void EditMoney(long What) { Money += What; }
  bool Displace(character*, bool = false);
  long GetStatScore() const;
  bool CheckStarvationDeath(const std::string&);
  void ShowNewPosInfo() const;
  void Hostility(character*);
  stack* GetGiftStack() const;
  bool MoveRandomlyInRoom();
  std::list<character*>::iterator GetTeamIterator();
  void SetTeamIterator(std::list<character*>::iterator);
  void ReceiveKoboldFlesh(long);
  bool ChangeRandomStat(short);
  ushort RandomizeReply(ulong, ushort);
  virtual void CreateInitialEquipment(ushort);
  void DisplayInfo(std::string&);
  virtual bool SpecialEnemySightedReaction(character*) { return false; }
  void TestWalkability();
  void EditAP(long);
  void EditNP(long What) { NP += What; }
  void SetSize(ushort);
  virtual ushort GetSize() const;
  torso* GetTorso() const { return static_cast<torso*>(*BodyPartSlot[TORSO_INDEX]); }
  humanoidtorso* GetHumanoidTorso() const { return static_cast<humanoidtorso*>(*BodyPartSlot[TORSO_INDEX]); }
  void SetTorso(torso* What) { SetBodyPart(TORSO_INDEX, What); }
  bodypart* GetBodyPart(ushort Index) const { return static_cast<bodypart*>(*BodyPartSlot[Index]); }
  void SetBodyPart(ushort, bodypart*);
  void SetMainMaterial(material*, ushort = 0);
  void ChangeMainMaterial(material*, ushort = 0);
  void SetSecondaryMaterial(material*, ushort = 0);
  void ChangeSecondaryMaterial(material*, ushort = 0);
  void SetContainedMaterial(material*, ushort = 0);
  void ChangeContainedMaterial(material*, ushort = 0);
  void Teleport(vector2d);
  void RestoreHP();
  void RestoreLivingHP();
  virtual bool ReceiveDamage(character*, ushort, uchar, uchar = ALL, uchar = 8, bool = false, bool = false, bool = false, bool = true);
  virtual ushort ReceiveBodyPartDamage(character*, ushort, uchar, uchar, uchar = 8, bool = false, bool = false, bool = true);
  virtual bool BodyPartIsVital(ushort) const { return true; }
  void RestoreBodyParts();
  std::string GetAssignedName() const { return AssignedName; }
  void SetAssignedName(const std::string& What) { AssignedName = What; }
  std::string GetDescription(uchar) const;
  std::string GetPersonalPronoun(bool = true) const;
  std::string GetPossessivePronoun(bool = true) const;
  std::string GetObjectPronoun(bool = true) const;
  virtual bool BodyPartCanBeSevered(ushort) const;
  void AddName(std::string&, uchar) const;
  void ReceiveHeal(long);
  virtual item* GetMainWielded() const { return 0; }
  virtual item* GetSecondaryWielded() const { return 0; }
  virtual void SetMainWielded(item*) { }
  virtual void SetSecondaryWielded(item*) { }
  uchar GetHungerState() const;
  bool ConsumeItem(item*);
  bool CanConsume(material*) const;
  action* GetAction() const { return Action; }
  void SetAction(action* What) { Action = What; }
  virtual void SwitchToDig(item*, vector2d) { }
  virtual void SetRightWielded(item*) { }
  virtual void SetLeftWielded(item*) { }
  void GoOn(go*);
  virtual bool CheckKick() const;
  virtual uchar OpenMultiplier() const { return 2; }
  virtual uchar CloseMultiplier() const { return 2; }
  virtual bool CheckThrow() const;
  virtual bool CheckOffer() const { return true; }
  ushort GetTemporaryStateCounter(ulong) const;
  void EditTemporaryStateCounter(ulong, short);
  static bool AllowDamageTypeBloodSpill(uchar);
  bool ClosePos(vector2d);
  ushort GetResistance(uchar) const;
  virtual ushort GlobalResistance(uchar Type) const { return GetResistance(Type); }
  virtual std::string GetEquipmentName(ushort) const;
  virtual bodypart* GetBodyPartOfEquipment(ushort) const { return 0; }
  virtual item* GetEquipment(ushort) const { return 0; }
  virtual ushort GetEquipmentSlots() const { return 0; }
  virtual bool (*EquipmentSorter(ushort) const)(const item*, const character*) { return 0; }
  virtual void SetEquipment(ushort, item*) { }
  void AddHealingLiquidConsumeEndMessage() const;
  void AddSchoolFoodConsumeEndMessage() const;
  void AddSchoolFoodHitMessage() const;
  void AddOmmelUrineConsumeEndMessage() const;
  void AddPepsiConsumeEndMessage() const;
  void AddFrogFleshConsumeEndMessage() const;
  void AddKoboldFleshConsumeEndMessage() const;
  void AddKoboldFleshHitMessage() const;
  void AddBoneConsumeEndMessage() const;
  void PrintInfo() const;
  virtual item* SevereBodyPart(ushort);
  virtual bool IsAnimated() const { return false; }
  virtual void CompleteRiseFromTheDead();
  bool RaiseTheDead(character*);
  bodypart* CreateBodyPart(ushort, ushort = 0);
  bool CanUseEquipment(ushort) const;
  virtual const prototype* GetProtoType() const;
  const database* GetDataBase() const { return DataBase; }
  void SetParameters(uchar) { }
  DATA_BASE_VALUE(ushort, DefaultArmStrength);
  DATA_BASE_VALUE(ushort, DefaultLegStrength);
  DATA_BASE_VALUE(ushort, DefaultDexterity);
  DATA_BASE_VALUE(ushort, DefaultAgility);
  DATA_BASE_VALUE(ushort, DefaultEndurance);
  DATA_BASE_VALUE(ushort, DefaultPerception);
  DATA_BASE_VALUE(ushort, DefaultIntelligence);
  DATA_BASE_VALUE(ushort, DefaultWisdom);
  DATA_BASE_VALUE(ushort, DefaultCharisma);
  DATA_BASE_VALUE(ushort, DefaultMana);
  DATA_BASE_VALUE(ulong, DefaultMoney);
  DATA_BASE_VALUE(ushort, TotalSize);
  DATA_BASE_BOOL(CanRead);
  DATA_BASE_BOOL(IsCharmable);
  DATA_BASE_VALUE(uchar, Sex);
  DATA_BASE_VALUE(ulong, BloodColor);
  DATA_BASE_BOOL(CanBeGenerated);
  DATA_BASE_VALUE(uchar, CriticalModifier);
  virtual DATA_BASE_VALUE(const std::string&, StandVerb);
  DATA_BASE_BOOL(CanOpen);
  DATA_BASE_BOOL(CanBeDisplaced);
  DATA_BASE_BOOL(CanWalk);
  DATA_BASE_BOOL(CanSwim);
  DATA_BASE_BOOL(CanFly);
  DATA_BASE_VALUE(ushort, FireResistance);
  DATA_BASE_VALUE(ushort, PoisonResistance);
  DATA_BASE_VALUE(ushort, ElectricityResistance);
  DATA_BASE_VALUE(ushort, ConsumeFlags);
  DATA_BASE_VALUE(ulong, TotalVolume);
  virtual DATA_BASE_VALUE(vector2d, HeadBitmapPos);
  virtual DATA_BASE_VALUE(vector2d, TorsoBitmapPos);
  virtual DATA_BASE_VALUE(vector2d, ArmBitmapPos);
  virtual DATA_BASE_VALUE(vector2d, LegBitmapPos);
  virtual DATA_BASE_VALUE(vector2d, RightArmBitmapPos);
  virtual DATA_BASE_VALUE(vector2d, LeftArmBitmapPos);
  virtual DATA_BASE_VALUE(vector2d, RightLegBitmapPos);
  virtual DATA_BASE_VALUE(vector2d, LeftLegBitmapPos);
  virtual DATA_BASE_VALUE(vector2d, GroinBitmapPos);
  virtual DATA_BASE_VALUE(ushort, ClothColor);
  virtual DATA_BASE_VALUE(ushort, SkinColor);
  virtual DATA_BASE_VALUE(ushort, CapColor);
  virtual DATA_BASE_VALUE(ushort, HairColor);
  virtual DATA_BASE_VALUE(ushort, EyeColor);
  virtual DATA_BASE_VALUE(ushort, TorsoMainColor);
  virtual DATA_BASE_VALUE(ushort, BeltColor);
  virtual DATA_BASE_VALUE(ushort, BootColor);
  virtual DATA_BASE_VALUE(ushort, TorsoSpecialColor);
  virtual DATA_BASE_VALUE(ushort, ArmMainColor);
  virtual DATA_BASE_VALUE(ushort, ArmSpecialColor);
  virtual DATA_BASE_VALUE(ushort, LegMainColor);
  virtual DATA_BASE_VALUE(ushort, LegSpecialColor);
  DATA_BASE_BOOL(IsNameable);
  virtual DATA_BASE_VALUE(ulong, BaseEmitation); // devirtualize ASAP
  DATA_BASE_VALUE(const std::string&, Article);
  DATA_BASE_VALUE(const std::string&, Adjective);
  DATA_BASE_VALUE(const std::string&, AdjectiveArticle);
  DATA_BASE_VALUE(const std::string&, NameSingular);
  DATA_BASE_VALUE(const std::string&, NamePlural);
  DATA_BASE_VALUE(const std::string&, PostFix);
  DATA_BASE_VALUE(uchar, ArticleMode);
  DATA_BASE_BOOL(CanZap);
  DATA_BASE_BOOL(IsPolymorphable);
  DATA_BASE_VALUE(ulong, BaseUnarmedStrength);
  DATA_BASE_VALUE(ulong, BaseBiteStrength);
  DATA_BASE_VALUE(ulong, BaseKickStrength);
  DATA_BASE_VALUE(uchar, AttackStyle);
  DATA_BASE_BOOL(CanUseEquipment);
  DATA_BASE_BOOL(CanKick);
  DATA_BASE_BOOL(CanTalk);
  DATA_BASE_VALUE(ulong, ClassStates);
  DATA_BASE_VALUE(const std::vector<std::string>&, Alias);
  DATA_BASE_BOOL(CreateGolemMaterialConfigurations);
  DATA_BASE_VALUE(short, AttributeBonus);
  DATA_BASE_VALUE(const std::vector<long>&, KnownCWeaponSkills);
  DATA_BASE_VALUE(const std::vector<long>&, CWeaponSkillHits);
  DATA_BASE_VALUE(ushort, RightSWeaponSkillHits);
  DATA_BASE_VALUE(ushort, LeftSWeaponSkillHits);
  DATA_BASE_VALUE(uchar, PanicLevel);
  DATA_BASE_BOOL(CanBeCloned);
  DATA_BASE_VALUE(const std::string&, DefaultName);
  DATA_BASE_VALUE(const std::vector<std::string>&, FriendlyReplies);
  DATA_BASE_VALUE(const std::vector<std::string>&, HostileReplies);
  DATA_BASE_VALUE(ushort, FleshMaterial);
  virtual DATA_BASE_BOOL(HasFeet);
  virtual DATA_BASE_VALUE(const std::string&, DeathMessage);
  DATA_BASE_BOOL(IsExtraCoward);
  DATA_BASE_BOOL(SpillsBlood);
  DATA_BASE_BOOL(HasEyes);
  virtual DATA_BASE_BOOL(HasHead);
  DATA_BASE_BOOL(CanThrow);
  DATA_BASE_BOOL(UsesNutrition);
  DATA_BASE_VALUE(ushort, AttackWisdomLimit);
  DATA_BASE_BOOL(CanWalkThroughWalls);
  DATA_BASE_BOOL(IsUnique);
  DATA_BASE_VALUE(uchar, AttachedGod);
  DATA_BASE_BOOL(BodyPartsDisappearWhenSevered);
  ushort GetType() const { return GetProtoType()->GetIndex(); }
  void TeleportRandomly();
  bool TeleportNear(character*);
  bool IsStuck() const;
  virtual void InitSpecialAttributes() { }
  virtual void Kick(lsquare*, bool = false) = 0;
  virtual ushort GetAttribute(ushort) const;
  virtual bool EditAttribute(ushort, short);
  virtual void EditExperience(ushort, long);
  bool CheckForAttributeIncrease(ushort&, long&, bool = false);
  bool CheckForAttributeDecrease(ushort&, long&, bool = false);
  bool RawEditAttribute(ushort&, short&, bool = false) const;
  void DrawPanel(bool) const;
  virtual ushort DrawStats(bool) const = 0;
  virtual ushort GetCarryingStrength() const = 0;
  static bool DamageTypeAffectsInventory(uchar);
  void SetStuckTo(item* What) { StuckTo = What; }
  item* GetStuckTo() const { return StuckTo; }
  void SetStuckToBodyPart(ushort What) { StuckToBodyPart = What; }
  ushort GetStuckToBodyPart() const { return StuckToBodyPart; }
  bool TryToUnstuck(vector2d);
  virtual ushort GetRandomStepperBodyPart() const;
  entity* GetMotherEntity() const { return MotherEntity; }
  void SetMotherEntity(entity* What) { MotherEntity = What; }
  virtual ushort CheckForBlock(character*, item*, float, ushort Damage, short, uchar) { return Damage; }
  ushort CheckForBlockWithArm(character*, item*, arm*, float, ushort, short, uchar);
  void AddBlockMessage(const character*, const item*, const std::string&, bool) const;
  character* GetPolymorphBackup() const { return PolymorphBackup; }
  void SetPolymorphBackup(character* What) { PolymorphBackup = What; }
  cweaponskill* GetCWeaponSkill(ushort Index) const { return CWeaponSkill[Index]; }
  virtual bool AddSpecialSkillInfo(felist&) const { return false; }
  virtual bool CheckBalance(float);
  long GetStateAPGain(long) const;
  virtual long GetMoveAPRequirement(uchar) const;
  virtual void SignalEquipmentAdd(ushort);
  virtual void SignalEquipmentRemoval(ushort);
  ushort GetConfig() const { return Config; }
  void BeginTemporaryState(ulong, ushort);
  void GainIntrinsic(ulong);
  void HandleStates();
  void PrintBeginPolymorphControlMessage() const;
  void PrintEndPolymorphControlMessage() const;
  void PrintBeginLifeSaveMessage() const;
  void PrintEndLifeSaveMessage() const;
  void PrintBeginLycanthropyMessage() const;
  void PrintEndLycanthropyMessage() const;
  void PrintBeginHasteMessage() const;
  void PrintEndHasteMessage() const;
  void PrintBeginSlowMessage() const;
  void PrintEndSlowMessage() const;
  void PrintBeginSearchingMessage() const;
  void PrintEndSearchingMessage() const;
  void EndPolymorph();
  void LycanthropyHandler();
  void SearchingHandler();
  void SaveLife();
  void BeginInvisibility();
  void BeginInfraVision();
  void BeginESP();
  void EndInvisibility();
  void EndInfraVision();
  void EndESP();
  character* PolymorphRandomly(ushort, ushort, ushort);
  virtual bool EquipmentEasilyRecognized(ushort) const { return true; }
  void StartReading(item*, ulong);
  void DexterityAction(ushort);
  virtual void SWeaponSkillTick() { }
  void PrintBeginInvisibilityMessage() const;
  void PrintEndInvisibilityMessage() const;
  void PrintBeginInfraVisionMessage() const;
  void PrintEndInfraVisionMessage() const;
  void PrintBeginESPMessage() const;
  void PrintEndESPMessage() const;
  bool CanBeSeenByPlayer(bool = false, bool = false) const;
  bool CanBeSeenBy(const character*, bool = false, bool = false) const;
  virtual void DetachBodyPart();
  void AttachBodyPart(bodypart*);
  bool HasAllBodyParts() const;
  bodypart* FindRandomOwnBodyPart(bool = false) const;
  bodypart* GenerateRandomBodyPart();
  void PrintBeginPoisonedMessage() const;
  void PrintEndPoisonedMessage() const;
  bool IsWarm() const;
  void CalculateEquipmentState();
  void Draw(bitmap*, vector2d, ulong, bool) const;
  virtual void DrawBodyParts(bitmap*, vector2d, ulong, bool) const;
  god* GetMasterGod() const;
  void PoisonedHandler();
  void PrintBeginTeleportMessage() const;
  void PrintEndTeleportMessage() const;
  void TeleportHandler();
  void PrintEndTeleportControlMessage() const;
  void PrintBeginTeleportControlMessage() const;
  void PolymorphHandler();
  void PrintEndPolymorphMessage() const;
  void PrintBeginPolymorphMessage() const;
  void DisplayStethoscopeInfo(character*) const;
  virtual bool CanUseStethoscope(bool) const;
  virtual bool IsUsingArms() const;
  virtual bool IsUsingLegs() const;
  virtual bool IsUsingHead() const;
  virtual void AddAttackInfo(felist&) const = 0;
  level* GetLevel() const { return static_cast<level*>(GetSquareUnder()->GetArea()); }
  area* GetArea() const { return GetSquareUnder()->GetArea(); }
  square* GetNeighbourSquare(ushort Index) const { return GetSquareUnder()->GetNeighbourSquare(Index); }
  lsquare* GetNeighbourLSquare(ushort Index) const { return static_cast<lsquare*>(GetSquareUnder())->GetNeighbourLSquare(Index); }
  wsquare* GetNeighbourWSquare(ushort) const;
  stack* GetStackUnder() const { return static_cast<lsquare*>(GetSquareUnder())->GetStack(); }
  square* GetNearSquare(vector2d Pos) const { return GetSquareUnder()->GetArea()->GetSquare(Pos); }
  square* GetNearSquare(ushort x, ushort y) const { return GetSquareUnder()->GetArea()->GetSquare(x, y); }
  lsquare* GetNearLSquare(vector2d Pos) const { return static_cast<lsquare*>(GetSquareUnder()->GetArea()->GetSquare(Pos)); }
  lsquare* GetNearLSquare(ushort x, ushort y) const { return static_cast<lsquare*>(GetSquareUnder()->GetArea()->GetSquare(x, y)); }
  vector2d GetPos() const { return GetSquareUnder()->GetPos(); }
  square* GetSquareUnder() const { return !MotherEntity ? SquareUnder : MotherEntity->GetSquareUnderEntity(); }
  virtual square* GetSquareUnderEntity() const { return GetSquareUnder(); }
  void SetSquareUnder(square* What) { SquareUnder = What; }
  lsquare* GetLSquareUnder() const { return static_cast<lsquare*>(GetSquareUnder()); }
  wsquare* GetWSquareUnder() const;
  ushort GetRandomNonVitalBodyPart();
  void TeleportSomePartsAway(ushort);
  virtual void SignalVolumeAndWeightChange();
  void CalculateVolumeAndWeight();
  ulong GetVolume() const { return Volume; }
  ulong GetBodyVolume() const { return BodyVolume; }
  ulong GetWeight() const { return Weight; }
  ulong GetCarriedWeight() const { return CarriedWeight; }
  virtual void SignalEmitationIncrease(ulong);
  virtual void SignalEmitationDecrease(ulong);
  void CalculateEmitation();
  void CalculateAll();
  void CalculateHP();
  void CalculateMaxHP();
  short GetHP() const { return HP; }
  short GetMaxHP() const { return MaxHP; }
  void CalculateBodyPartMaxHPs(bool = true);
  bool IsInitializing() const { return Initializing; }
  bool IsInNoMsgMode() const { return InNoMsgMode; }
  bool ActivateRandomState(ushort);
  ulong GetRandomNotActivatedState(bool) const;
  bool GainRandomInstric();
  virtual void CalculateBattleInfo() = 0;
  void CalculateBurdenState();
  virtual void CalculateDodgeValue();
  virtual void CalculateBodyParts() { BodyParts = 1; }
  virtual void CalculateAllowedWeaponSkillCategories();
  uchar GetBodyParts() const { return BodyParts; }
  uchar GetAllowedWeaponSkillCategories() const { return AllowedWeaponSkillCategories; }
  float GetRelativeDanger(const character*, bool = false) const;
  float GetTimeToDie(const character*, ushort, float, bool, bool) const;
  virtual float GetTimeToKill(const character*, bool) const = 0;
  float GetDangerModifier() const;
  virtual void AddSpecialEquipmentInfo(std::string&, ushort) const { }
  virtual std::string GetBodyPartName(ushort, bool = false) const;
  item* SearchForItemWithID(ulong) const;
  bool ContentsCanBeSeenBy(const character*) const;
  std::string GetBeVerb() const;
  virtual void CreateBlockPossibilityVector(blockvector&, float) const { }
  virtual bool SpecialUnarmedEffect(character*, uchar, uchar, bool) { return false; }
  virtual bool SpecialKickEffect(character*, uchar, uchar, bool) { return false; }
  virtual bool SpecialBiteEffect(character*, uchar, uchar, bool) { return false; }
  bool HitEffect(character*, item*, uchar, uchar, uchar, bool);
  void WeaponSkillHit(item*, uchar);
  virtual void AddDefenceInfo(felist&) const;
  character* Duplicate() const;
  room* GetRoom() const { return GetLSquareUnder()->GetRoom(); }
  bool TryToEquip(item*);
  bool TryToConsume(item*);
  void UpdateESPLOS() const;
  uchar GetCWeaponSkillLevel(const item*) const;
  virtual uchar GetSWeaponSkillLevel(const item*) const { return 0; }
  void PrintBeginPanicMessage() const;
  void PrintEndPanicMessage() const;
  void CheckPanic(ushort);
  character* CloneToNearestSquare(character*) const;
  void SignalSpoil();
  void SignalSpoilLevelChange();
  virtual bool IsAlive() const = 0;
  bool IsPolymorphed() const { return Polymorphed; }
  void SetPolymorphed(bool What) { Polymorphed = What; }
  bool IsInBadCondition() const { return HP * 3 < MaxHP; }
  bool IsInBadCondition(short HP) const { return HP * 3 < MaxHP; }
  ushort GetCondition() const;
  void UpdatePictures();
  bool CanHeal() const;
  vector2d GetWaypoint() const { return WayPoint; }
  void SetWayPoint(vector2d What) { WayPoint = What; }
  void ReceiveFluidSpill(material*, ushort);
  uchar GetRelation(const character*) const;
  void CalculateAttributeBonuses();
  void ApplyEquipmentAttributeBonuses(item*);
  void ReceiveAntidote(long);
  void AddAntidoteConsumeEndMessage() const;
  bool IsDead() const;
  virtual void ShowBattleInfo() = 0;
  void AddOriginalBodyPartID(ushort, ulong);
  void AddToInventory(const std::list<contentscript<item> >&, ushort);
  void ShowDodgeAndMoveInfo() const;
  virtual void AddSpecialMovePenaltyInfo(felist&) const { }
  bool HasHadBodyPart(const item*) const;
  void ProcessAndAddMessage(std::string) const;
  virtual bool CheckZap();
  void SetEndurance(ushort);
  void SetPerception(ushort);
  void SetIntelligence(ushort);
  void SetWisdom(ushort);
  void SetCharisma(ushort);
  void SetMana(ushort);
  void DamageAllItems(character*, ushort, uchar);
  bool Equips(const item*) const;
  virtual void AddAttributeInfo(std::string&) const;
  void PrintBeginConfuseMessage() const;
  void PrintEndConfuseMessage() const;
  vector2d ApplyStateModification(vector2d) const;
  void AddConfuseHitMessage() const;
  item* SelectFromPossessions(const std::string&, bool (*)(const item*, const character*) = 0);
  void SelectFromPossessions(itemvector&, const std::string&, uchar, bool (*)(const item*, const character*) = 0);
  bool EquipsSomething(bool (*)(const item*, const character*) = 0);
  bool CheckTalk();
  virtual bool CanCreateBodyPart(ushort) const { return false; }
  virtual bool HandleCharacterBlockingTheWay(character*) { return false; }
  std::string& ProcessMessage(std::string&) const;
  virtual bool IsHumanoid() const { return false; }
  long GetStuffScore() const;
  bool IsOnGround() const;
  virtual bool CheckIfEquipmentIsNotUsable(ushort) const { return false; }
  bool MoveTowardsHomePos();
  virtual void SetWayPoints(const std::vector<vector2d>&) { }
  bool TryToChangeEquipment(ushort);
  void PrintBeginParasitizedMessage() const;
  void PrintEndParasitizedMessage() const;
  void ParasitizedHandler();
  bool CanFollow() const;
  bool LeftOversAreUnique() const;
  std::string GetKillName() const;
  std::string GetPanelName() const;
  virtual void AddSpecialStethoscopeInfo(felist&) const = 0;
  virtual item* GetPairEquipment(ushort) const { return 0; }
  bool HealHitPoint();
  void CreateHomeData();
  room* GetHomeRoom() const;
  bool HomeDataIsValid() const;
  void SetHomePos(vector2d);
  void RemoveHomeData();
  ulong GetID() const { return ID; }
  void AddESPConsumeMessage() const;
  virtual void RaiseStats();
  virtual void LowerStats();
  const std::list<ulong>& GetOriginalBodyPartID(ushort) const;
  void GetHitByExplosion(const explosion&, ushort);
  bool AllowPoisoned() const { return IsAlive(); }
  bool AllowParasitized() const { return IsAlive(); }
  virtual ushort GetSpecies() const { return 0; }
  void SortAllItems(itemvector&, const character* = 0, bool (*)(const item*, const character*) = 0);
  character* GetRandomNeighbourEnemy() const;
  void Search(ushort);
  character* GetRandomNeighbour(uchar = (HOSTILE | UNCARING | FRIEND)) const;
  virtual bool IsRetreating() const { return StateIsActivated(PANIC); }
 protected:
  virtual bodypart* MakeBodyPart(ushort) const;
  virtual character* RawDuplicate() const = 0;
  virtual void SpecialTurnHandler() { }
  void Initialize(ushort, ushort);
  virtual void VirtualConstructor(bool) { }
  void LoadDataBaseStats();
  void InstallDataBase();
  virtual vector2d GetBodyPartBitmapPos(ushort);
  virtual ushort GetBodyPartColorA(ushort);
  virtual ushort GetBodyPartColorB(ushort);
  virtual ushort GetBodyPartColorC(ushort);
  virtual ushort GetBodyPartColorD(ushort);
  virtual ulong GetBodyPartSize(ushort, ushort);
  virtual ulong GetBodyPartVolume(ushort) const;
  void UpdateBodyPartPicture(ushort);
  ushort ChooseBodyPartToReceiveHit(float, float);
  virtual void CreateBodyParts(ushort);
  virtual material* CreateBodyPartMaterial(ushort, ulong) const;
  virtual bool ShowClassDescription() const { return true; }
  void SeekLeader();
  virtual bool CheckForUsefulItemsOnGround();
  bool CheckForDoors();
  bool CheckForEnemies(bool, bool, bool = true);
  bool FollowLeader();
  void StandIdleAI();
  virtual void CreateCorpse(lsquare*);
  void GetPlayerCommand();
  virtual void GetAICommand();
  bool MoveTowards(vector2d);
  virtual std::string FirstPersonUnarmedHitVerb() const;
  virtual std::string FirstPersonCriticalUnarmedHitVerb() const;
  virtual std::string ThirdPersonUnarmedHitVerb() const;
  virtual std::string ThirdPersonCriticalUnarmedHitVerb() const;
  virtual std::string FirstPersonKickVerb() const;
  virtual std::string FirstPersonCriticalKickVerb() const;
  virtual std::string ThirdPersonKickVerb() const;
  virtual std::string ThirdPersonCriticalKickVerb() const;
  virtual std::string FirstPersonBiteVerb() const;
  virtual std::string FirstPersonCriticalBiteVerb() const;
  virtual std::string ThirdPersonBiteVerb() const;
  virtual std::string ThirdPersonCriticalBiteVerb() const;
  virtual std::string UnarmedHitNoun() const;
  virtual std::string KickNoun() const;
  virtual std::string BiteNoun() const;
  virtual bool AttackIsBlockable(uchar) const { return true; }
  virtual uchar GetSpecialBodyPartFlags(ushort) const;
  stack* Stack;
  long NP, AP;
  bool Player;
  ulong TemporaryState;
  ushort TemporaryStateCounter[STATES];
  team* Team;
  vector2d WayPoint;
  ulong Money;
  std::list<character*>::iterator TeamIterator;
  characterslot* BodyPartSlot;
  std::string AssignedName;
  action* Action;
  ushort Config;
  const database* DataBase;
  ushort StuckToBodyPart;
  item* StuckTo; // Bad naming. Sorry.
  ushort BaseAttribute[BASE_ATTRIBUTES];
  long BaseExperience[BASE_ATTRIBUTES];
  std::list<ulong>* OriginalBodyPartID;
  entity* MotherEntity;
  character* PolymorphBackup;
  cweaponskill** CWeaponSkill;
  ulong EquipmentState;
  square* SquareUnder;
  ulong Volume;
  ulong Weight;
  ulong CarriedWeight;
  ulong BodyVolume;
  short HP;
  short MaxHP;
  bool Initializing;
  uchar BurdenState;
  float DodgeValue;
  uchar AllowedWeaponSkillCategories;
  uchar BodyParts;
  bool Polymorphed;
  bool InNoMsgMode;
  ulong RegenerationCounter;
  short AttributeBonus[BASE_ATTRIBUTES];
  short CarryingBonus;
  homedata* HomeData;
  ulong ID;
};

#ifdef __FILE_OF_STATIC_CHARACTER_PROTOTYPE_DEFINITIONS__
#define CHARACTER_PROTOTYPE(name, base, baseproto)\
character* name##_Clone(ushort Config, ushort SpecialFlags) { return new name(Config, SpecialFlags); }\
characterprototype name##_ProtoType(baseproto, &name##_Clone, #name);\
name::name(ushort Config, ushort SpecialFlags) : base(donothing()) { Initialize(Config, SpecialFlags); }\
name::name(donothing D) : base(D) { }\
const characterprototype* name::GetProtoType() const { return &name##_ProtoType; }\
character* name::RawDuplicate() const { return new name(*this); }
#define ABSTRACT_CHARACTER_PROTOTYPE(name, base, baseproto)\
characterprototype name##_ProtoType(baseproto, 0, #name);\
name::name(donothing D) : base(D) { }\
const characterprototype* name::GetProtoType() const { return &name##_ProtoType; }
#else
#define CHARACTER_PROTOTYPE(name, base, baseproto)
#define ABSTRACT_CHARACTER_PROTOTYPE(name, base, baseproto)
#endif

#define CHARACTER(name, base, data)\
\
name : public base\
{\
 public:\
  name(ushort = 0, ushort = 0);\
  name(donothing);\
  virtual const prototype* GetProtoType() const;\
  data\
 protected:\
  virtual character* RawDuplicate() const;\
}; CHARACTER_PROTOTYPE(name, base, &base##_ProtoType);

#define ABSTRACT_CHARACTER(name, base, data)\
\
name : public base\
{\
 public:\
  name(donothing);\
  virtual const prototype* GetProtoType() const;\
  data\
}; ABSTRACT_CHARACTER_PROTOTYPE(name, base, &base##_ProtoType);

#endif

