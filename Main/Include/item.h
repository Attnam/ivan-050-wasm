#ifndef __ITEM_H__
#define __ITEM_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include <cmath>

#include "object.h"
#include "lsquare.h"
#include "slot.h"

class felist;
template <class type> class contentscript;
template <class type> class database;

struct itemdatabase
{
  void InitDefaults(ushort);
  ushort Possibility;
  vector2d InHandsPic;
  long Score;
  bool IsDestroyable;
  bool CanBeWished;
  bool IsMaterialChangeable;
  uchar WeaponCategory;
  bool IsPolymorphSpawnable;
  bool IsAutoInitializable;
  ulong Category;
  ushort FireResistance;
  ushort PoisonResistance;
  ushort ElectricityResistance;
  ushort StrengthModifier;
  ushort FormModifier;
  ulong NPModifier;
  ushort DefaultSize;
  ulong DefaultMainVolume;
  ulong DefaultSecondaryVolume;
  ulong DefaultContainedVolume;
  vector2d BitmapPos;
  ulong Price;
  ulong BaseEmitation;
  std::string Article;
  std::string Adjective;
  std::string AdjectiveArticle;
  std::string NameSingular;
  std::string NamePlural;
  std::string PostFix;
  uchar ArticleMode;
  std::vector<long> MainMaterialConfig;
  std::vector<long> SecondaryMaterialConfig;
  std::vector<long> ContainedMaterialConfig;
  std::vector<long> MaterialConfigChances;
  bool IsAbstract;
  bool IsPolymorphable;
  std::vector<std::string> Alias;
  uchar OKVisualEffects;
  bool CanBeGeneratedInContainer;
  uchar ForcedVisualEffects;
  uchar Roundness;
  ushort GearStates;
  bool IsTwoHanded;
  bool CreateDivineConfigurations;
  bool CanBeCloned;
  ushort BeamRange;
  bool CanBeBroken;
  vector2d WallBitmapPos;
  std::string FlexibleNameSingular;
  uchar MinCharges;
  uchar MaxCharges;
  bool CanBePiled;
  ulong StorageVolume;
  ushort MaxGeneratedContainedItems;
  bool AffectsArmStrength;
  bool AffectsLegStrength;
  bool AffectsDexterity;
  bool AffectsAgility;
  bool AffectsEndurance;
  bool AffectsPerception;
  bool AffectsIntelligence;
  bool AffectsWisdom;
  bool AffectsCharisma;
  bool AffectsMana;
  char DefaultEnchantment;
  bool PriceIsProportionalToEnchantment;
  uchar InElasticityPenaltyModifier;
  bool CanBeUsedBySmith;
  bool AffectsCarryingCapacity;
  uchar DamageDivider;
  bool HandleInPairs;
  bool CanBeEnchanted;
  ulong BeamColor;
  uchar BeamEffect;
  uchar BeamStyle;
};

class itemprototype
{
 public:
  friend class database<item>;
  itemprototype(itemprototype*, item* (*)(ushort, ushort), const std::string&);
  item* Clone(ushort Config = 0, ushort SpecialFlags = 0) const { return Cloner(Config, SpecialFlags); }
  item* CloneAndLoad(inputfile&) const;
  const std::string& GetClassId() const { return ClassId; }
  ushort GetIndex() const { return Index; }
  const itemprototype* GetBase() const { return Base; }
  const std::map<ushort, itemdatabase>& GetConfig() const { return Config; }
  void CreateSpecialConfigurations() { }
  const itemdatabase& ChooseBaseForConfig(ushort);
 private:
  ushort Index;
  itemprototype* Base;
  std::map<ushort, itemdatabase> Config;
  item* (*Cloner)(ushort, ushort);
  std::string ClassId;
};

class item : public object
{
 public:
  friend class database<item>;
  typedef itemprototype prototype;
  typedef itemdatabase database;
  typedef std::map<ushort, itemdatabase> databasemap;
  item(donothing);
  item(const item&);
  virtual float GetWeaponStrength() const;
  virtual bool Open(character*);
  virtual bool Consume(character*, long);
  virtual bool IsHeadOfElpuri() const { return false; }
  virtual bool IsPetrussNut() const { return false; }
  virtual bool IsGoldenEagleShirt() const { return false; }
  virtual bool CanBeRead(character*) const { return false; }
  virtual bool Read(character*) { return false; }
  virtual void FinishReading(character*) { }
  virtual bool HitEffect(character*, character*, uchar, uchar, bool) { return false; }
  virtual void DipInto(material*, character*) { }
  virtual material* CreateDipMaterial() { return 0; }
  virtual item* BetterVersion() const { return 0; }
  virtual short GetOfferValue(char) const;
  void Fly(character*, uchar, ushort);
  uchar HitCharacter(character*, character*, ushort, float, uchar);
  virtual bool DogWillCatchAndConsume() const { return false; }
  virtual bool Apply(character*);
  virtual bool Zap(character*, vector2d, uchar) { return false; }
  virtual bool Polymorph(stack*);
  virtual bool CheckPickUpEffect(character*) { return true; }
  virtual void StepOnEffect(character*) { }
  virtual bool IsTheAvatar() const { return false; }
  virtual void SignalSquarePositionChange(uchar) { }
  virtual bool CanBeEatenByAI(const character*) const;
  virtual std::string GetConsumeVerb() const;
  virtual bool IsExplosive() const { return false; }
  virtual bool CatWillCatchAndConsume() const { return false; }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void ChargeFully(character*) { }
  void SetSize(ushort Value) { Size = Value; }
  virtual ushort GetSize() const { return Size; }
  ulong GetID() const { return ID; }
  void SetID(ulong What) { ID = What; }
  ulong GetBackupID() const { return BackupID; }
  void SetBackupID(ulong What) { BackupID = What; }
  virtual void TeleportRandomly();
  virtual ushort GetStrengthValue() const;
  slot* GetSlot() const { return Slot; }
  void SetSlot(slot* What) { Slot = What; }
  void PlaceToSlot(slot* Slot) { Slot->PutInItem(this); }
  void RemoveFromSlot();
  void MoveTo(stack*);
  static std::string ItemCategoryName(ulong);
  static bool EatableSorter(const item* Item, const character* Char) { return Item->IsEatable(Char); }
  static bool DrinkableSorter(const item* Item, const character* Char) { return Item->IsDrinkable(Char); }
  static bool OpenableSorter(const item* Item, const character* Char) { return Item->IsOpenable(Char); }
  static bool ReadableSorter(const item* Item, const character* Char) { return Item->IsReadable(Char); }
  static bool DippableSorter(const item* Item, const character* Char) { return Item->IsDippable(Char); }
  static bool DipDestinationSorter(const item* Item, const character* Char) { return Item->IsDipDestination(Char); }
  static bool AppliableSorter(const item* Item, const character* Char) { return Item->IsAppliable(Char); }
  static bool ZappableSorter(const item* Item, const character* Char) { return Item->IsZappable(Char); }
  static bool ChargeableSorter(const item* Item, const character* Char) { return Item->IsChargeable(Char); }
  static bool HelmetSorter(const item* Item, const character* Char) { return Item->IsHelmet(Char); }
  static bool AmuletSorter(const item* Item, const character* Char) { return Item->IsAmulet(Char); }
  static bool CloakSorter(const item* Item, const character* Char) { return Item->IsCloak(Char); }
  static bool BodyArmorSorter(const item* Item, const character* Char) { return Item->IsBodyArmor(Char); }
  static bool RingSorter(const item* Item, const character* Char) { return Item->IsRing(Char); }
  static bool GauntletSorter(const item* Item, const character* Char) { return Item->IsGauntlet(Char); }
  static bool BeltSorter(const item* Item, const character* Char) { return Item->IsBelt(Char); }
  static bool BootSorter(const item* Item, const character* Char) { return Item->IsBoot(Char); }
  static bool WeaponSorter(const item* Item, const character* Char) { return Item->IsWeapon(Char); }
  static bool ArmorSorter(const item* Item, const character* Char) { return Item->IsArmor(Char); }
  static bool FixableBySmithSorter(const item* Item, const character* Char) { return Item->IsFixableBySmith(Char); }
  static bool BrokenSorter(const item* Item, const character*) { return Item->IsBroken(); }
  virtual bool IsConsumable(const character*) const;
  virtual bool IsEatable(const character*) const;
  virtual bool IsDrinkable(const character*) const;
  virtual bool IsOpenable(const character*) const { return false; }
  virtual bool IsReadable(const character*) const { return false; }
  virtual bool IsDippable(const character*) const { return false; }
  virtual bool IsDipDestination(const character*) const { return false; }
  virtual bool IsAppliable(const character*) const { return false; }
  virtual bool IsZappable(const character*) const { return false; }
  virtual bool IsChargeable(const character*) const { return false; }
  virtual bool IsHelmet(const character*) const { return false; }
  virtual bool IsAmulet(const character*) const { return false; }
  virtual bool IsCloak(const character*) const { return false; }
  virtual bool IsBodyArmor(const character*) const { return false; }
  virtual bool IsRing(const character*) const { return false; }
  virtual bool IsGauntlet(const character*) const { return false; }
  virtual bool IsBelt(const character*) const { return false; }
  virtual bool IsBoot(const character*) const { return false; }
  virtual bool IsShield(const character*) const { return false; }
  virtual bool IsWeapon(const character*) const { return false; }
  virtual bool IsArmor(const character*) const { return false; }
  virtual bool IsOnGround() const;
  ushort GetResistance(uchar) const;
  virtual void GenerateLeftOvers(character*);
  virtual void Be();
  ushort GetType() const { return GetProtoType()->GetIndex(); }
  virtual bool ReceiveDamage(character*, ushort, uchar);
  virtual void AddConsumeEndMessage(character*) const;
  virtual bool RaiseTheDead(character*) { return false; }
  virtual uchar GetBodyPartIndex() const { return 0xFF; }
  virtual const prototype* GetProtoType() const;
  const database* GetDataBase() const { return DataBase; }
  virtual bool CanOpenLockType(uchar) const { return false; }
  virtual bool IsWhip() const { return false; }
  virtual DATA_BASE_VALUE(vector2d, InHandsPic);
  virtual DATA_BASE_VALUE(long, Score);
  virtual DATA_BASE_BOOL(IsDestroyable);
  DATA_BASE_BOOL(IsMaterialChangeable);
  DATA_BASE_VALUE(uchar, WeaponCategory);
  DATA_BASE_BOOL(IsAutoInitializable);
  DATA_BASE_VALUE(ulong, Category);
  DATA_BASE_VALUE(ushort, FireResistance);
  DATA_BASE_VALUE(ushort, PoisonResistance);
  DATA_BASE_VALUE(ushort, ElectricityResistance);
  DATA_BASE_VALUE(ushort, StrengthModifier);
  virtual DATA_BASE_VALUE(ushort, FormModifier);
  DATA_BASE_VALUE(ulong, NPModifier);
  DATA_BASE_VALUE(ushort, DefaultSize);
  DATA_BASE_VALUE(ulong, DefaultMainVolume);
  DATA_BASE_VALUE(ulong, DefaultSecondaryVolume);
  DATA_BASE_VALUE(ulong, DefaultContainedVolume);
  virtual DATA_BASE_VALUE_WITH_PARAMETER(vector2d, BitmapPos, ushort);
  virtual DATA_BASE_VALUE(ulong, Price);
  virtual DATA_BASE_VALUE(ulong, BaseEmitation);
  virtual DATA_BASE_VALUE(const std::string&, Article);
  virtual DATA_BASE_VALUE(const std::string&, Adjective);
  virtual DATA_BASE_VALUE(const std::string&, AdjectiveArticle);
  virtual DATA_BASE_VALUE(const std::string&, NameSingular);
  virtual DATA_BASE_VALUE(const std::string&, NamePlural);
  virtual DATA_BASE_VALUE(const std::string&, PostFix);
  virtual DATA_BASE_VALUE(uchar, ArticleMode);
  DATA_BASE_VALUE(const std::vector<long>&, MainMaterialConfig);
  DATA_BASE_VALUE(const std::vector<long>&, SecondaryMaterialConfig);
  DATA_BASE_VALUE(const std::vector<long>&, ContainedMaterialConfig);
  DATA_BASE_VALUE(const std::vector<long>&, MaterialConfigChances);
  virtual DATA_BASE_BOOL(IsPolymorphable);
  virtual DATA_BASE_VALUE(const std::vector<std::string>&, Alias);
  virtual DATA_BASE_VALUE(uchar, OKVisualEffects);
  virtual DATA_BASE_BOOL(CanBeGeneratedInContainer);
  virtual DATA_BASE_VALUE(uchar, ForcedVisualEffects);
  DATA_BASE_VALUE(uchar, Roundness);
  DATA_BASE_VALUE(ushort, GearStates);
  DATA_BASE_BOOL(IsTwoHanded);
  DATA_BASE_BOOL(CanBeBroken);
  virtual DATA_BASE_VALUE_WITH_PARAMETER(vector2d, WallBitmapPos, ushort);
  virtual DATA_BASE_VALUE(const std::string&, FlexibleNameSingular);
  DATA_BASE_BOOL(CanBePiled);
  DATA_BASE_BOOL(AffectsArmStrength);
  DATA_BASE_BOOL(AffectsLegStrength);
  DATA_BASE_BOOL(AffectsDexterity);
  DATA_BASE_BOOL(AffectsAgility);
  DATA_BASE_BOOL(AffectsEndurance);
  DATA_BASE_BOOL(AffectsPerception);
  DATA_BASE_BOOL(AffectsIntelligence);
  DATA_BASE_BOOL(AffectsWisdom);
  DATA_BASE_BOOL(AffectsCharisma);
  DATA_BASE_BOOL(AffectsMana);
  DATA_BASE_BOOL(AffectsCarryingCapacity);
  DATA_BASE_VALUE(char, DefaultEnchantment);
  virtual DATA_BASE_BOOL(PriceIsProportionalToEnchantment);
  virtual DATA_BASE_VALUE(uchar, MaxCharges);
  virtual DATA_BASE_VALUE(uchar, MinCharges);
  DATA_BASE_VALUE(uchar, InElasticityPenaltyModifier);
  virtual DATA_BASE_VALUE(ulong, StorageVolume);
  virtual DATA_BASE_VALUE(ushort, MaxGeneratedContainedItems);
  virtual DATA_BASE_BOOL(CanBeCloned);
  virtual DATA_BASE_VALUE(ushort, BeamRange);
  virtual DATA_BASE_BOOL(CanBeUsedBySmith);
  DATA_BASE_VALUE(uchar, DamageDivider);
  DATA_BASE_BOOL(HandleInPairs);
  virtual DATA_BASE_BOOL(CanBeEnchanted);
  virtual DATA_BASE_VALUE(ulong, BeamColor);
  virtual DATA_BASE_VALUE(uchar, BeamEffect);
  virtual DATA_BASE_VALUE(uchar, BeamStyle);
  virtual bool CanBeSoldInLibrary(character* Librarian) const { return CanBeRead(Librarian); }
  virtual bool TryKey(item*, character*) { return false; }
  virtual bool TryToUnstuck(character*, vector2d) { return true; }
  virtual bool TryToUnstuck(character*, ushort, vector2d) { return false; }
  ulong GetBlockModifier() const;
  bool IsSimiliarTo(item*) const;
  virtual bool IsPickable(character*) const { return true; }
  virtual bool CanBeSeenByPlayer() const;
  virtual bool CanBeSeenBy(const character*) const;
  std::string GetDescription(uchar) const;
  virtual square* GetSquareUnderEntity() const { return GetSquareUnder(); }
  square* GetSquareUnder() const { return Slot ? Slot->GetSquareUnder() : 0; }
  lsquare* GetLSquareUnder() const { return static_cast<lsquare*>(Slot->GetSquareUnder()); }
  level* GetLevel() const { return static_cast<level*>(Slot->GetSquareUnder()->GetArea()); }
  area* GetArea() const { return Slot->GetSquareUnder()->GetArea(); }
  vector2d GetPos() const { return Slot->GetSquareUnder()->GetPos(); }
  square* GetNearSquare(vector2d Pos) const { return Slot->GetSquareUnder()->GetArea()->GetSquare(Pos); }
  square* GetNearSquare(ushort x, ushort y) const { return Slot->GetSquareUnder()->GetArea()->GetSquare(x, y); }
  lsquare* GetNearLSquare(vector2d Pos) const { return static_cast<lsquare*>(Slot->GetSquareUnder()->GetArea()->GetSquare(Pos)); }
  lsquare* GetNearLSquare(ushort x, ushort y) const { return static_cast<lsquare*>(Slot->GetSquareUnder()->GetArea()->GetSquare(x, y)); }
  virtual void SignalVolumeAndWeightChange();
  virtual void CalculateVolumeAndWeight();
  ulong GetVolume() const { return Volume; }
  ulong GetWeight() const { return Weight; }
  virtual void SignalEmitationIncrease(ulong);
  virtual void SignalEmitationDecrease(ulong);
  void CalculateAll();
  virtual void DropEquipment() { }
  virtual bool DangerousToStepOn(const character*) const { return false; } 
  void WeaponSkillHit();
  virtual void SetTeam(ushort) { }
  void SpecialGenerationHandler();
  item* Duplicate() const;
  virtual void SetIsActive(bool) { }
  ushort GetBaseMinDamage() const;
  ushort GetBaseMaxDamage() const;
  ushort GetBaseToHitValue() const;
  ushort GetBaseBlockValue() const;
  virtual void AddInventoryEntry(const character*, std::string&, ushort, bool) const;
  virtual void AddAttackInfo(felist&) const;
  virtual void AddMiscellaneousInfo(felist&) const;
  ulong GetNutritionValue() const;
  virtual void SignalSpoil(material*);
  virtual bool AllowSpoil() const;
  bool CarriedByPlayer() const;
  bool CarriedBy(const character*) const;
  item* DuplicateToStack(stack*);
  virtual bool CanBePiledWith(const item*, const character*) const;
  virtual ulong GetTotalExplosivePower() const { return 0; }
  virtual void Break();
  void Empty();
  virtual void SetEnchantment(char) { }
  virtual void EditEnchantment(char) { }
  virtual void SignalEnchantmentChange();
  virtual ushort GetBonus() const { return 100; }
  virtual void DrawContents(const character*) { }
  virtual bool IsBroken() const;
  virtual void ReceiveFluidSpill(material*) { }
  virtual char GetEnchantment() const { return 0; }
  virtual ulong GetEnchantedPrice(char) const;
  virtual item* Fix();
  ushort GetStrengthRequirement() const;
  virtual ushort GetInElasticityPenalty(ushort) const { return 0; }
  virtual bool IsFixableBySmith(const character*) const { return false; }
  virtual ulong GetFixPrice() const { return 100; } 
  void DonateSlotTo(item*);
  virtual uchar GetFlyAmount() const;
  virtual void SignalSpoilLevelChange(material*);
  virtual void ResetSpoiling();
  virtual void SetItemsInside(const std::vector<contentscript<item> >&, ushort) { }
  virtual short GetCarryingBonus() const { return 0; }
  virtual bool IsBanana() const { return false; }
  virtual bool IsEncryptedScroll() const { return false; }
  virtual const std::string& GetStrengthValueDescription() const;
  virtual const std::string& GetBaseToHitValueDescription() const;
  virtual bool IsInCorrectSlot(ushort) const;
  bool IsInCorrectSlot() const;
  ushort GetEquipmentIndex() const;
  room* GetRoom() const { return GetLSquareUnder()->GetRoom(); }
  virtual bool HasBetterVersion() const { return false; }
 protected:
  virtual item* RawDuplicate() const = 0;
  void LoadDataBaseStats();
  virtual void VirtualConstructor(bool) { }
  void Initialize(ushort, ushort);
  virtual void InstallDataBase();
  virtual uchar GetGraphicsContainerIndex() const;
  virtual bool ShowMaterial() const;
  slot* Slot;
  bool Cannibalised;
  ushort Size;
  ulong ID;
  ulong BackupID;
  const database* DataBase;
  ulong Volume;
  ulong Weight;
};

#ifdef __FILE_OF_STATIC_ITEM_PROTOTYPE_DEFINITIONS__
#define ITEM_PROTOTYPE(name, base, baseproto)\
item* name##_Clone(ushort Config, ushort SpecialFlags) { return new name(Config, SpecialFlags); }\
itemprototype name##_ProtoType(baseproto, &name##_Clone, #name);\
name::name(ushort Config, ushort SpecialFlags) : base(donothing()) { Initialize(Config, SpecialFlags); }\
name::name(donothing D) : base(D) { }\
const itemprototype* name::GetProtoType() const { return &name##_ProtoType; }\
item* name::RawDuplicate() const { return new name(*this); }
#define ABSTRACT_ITEM_PROTOTYPE(name, base, baseproto)\
itemprototype name##_ProtoType(baseproto, 0, #name);\
name::name(donothing D) : base(D) { }\
const itemprototype* name::GetProtoType() const { return &name##_ProtoType; }
#else
#define ITEM_PROTOTYPE(name, base, baseproto)
#define ABSTRACT_ITEM_PROTOTYPE(name, base, baseproto)
#endif

#define ITEM(name, base, data)\
\
name : public base\
{\
 public:\
  name(ushort = 0, ushort = 0);\
  name(donothing);\
  virtual const prototype* GetProtoType() const;\
  data\
 protected:\
  virtual item* RawDuplicate() const;\
}; ITEM_PROTOTYPE(name, base, &base##_ProtoType);

#define ABSTRACT_ITEM(name, base, data)\
\
name : public base\
{\
 public:\
  name(donothing);\
  virtual const prototype* GetProtoType() const;\
  data\
}; ABSTRACT_ITEM_PROTOTYPE(name, base, &base##_ProtoType);

#endif
