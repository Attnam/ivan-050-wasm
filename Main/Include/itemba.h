#ifndef __ITEMBA_H__
#define __ITEMBA_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include "typedef.h"
#include "vector2d.h"
#include "object.h"
#include "lsquare.h"
#include "slot.h"

class felist;
class bitmap;
class character;
class humanoid;
class material;
class stack;
class outputfile;
class inputfile;
class slot;
class item;
class felist;
template <class type> class database;

struct itemdatabase
{
  void InitDefaults(ushort);
  ushort Possibility;
  vector2d InHandsPic;
  ulong OfferModifier;
  long Score;
  bool IsDestroyable;
  bool CanBeWished;
  bool IsMaterialChangeable;
  uchar WeaponCategory;
  bool IsPolymorphSpawnable;
  bool IsAutoInitializable;
  uchar Category;
  ushort SoundResistance;
  ushort EnergyResistance;
  ushort AcidResistance;
  ushort FireResistance;
  ushort PoisonResistance;
  ushort BulimiaResistance;
  ushort StrengthModifier;
  ushort FormModifier;
  ulong NPModifier;
  ushort DefaultSize;
  ulong DefaultMainVolume;
  ulong DefaultSecondaryVolume;
  ulong DefaultContainedVolume;
  vector2d BitmapPos;
  ulong Price;
  ushort BaseEmitation;
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
};

class itemprototype
{
 public:
  friend class database<item>;
  itemprototype(itemprototype*, item* (*)(ushort, bool, bool), const std::string&);
  item* Clone(ushort Config = 0, bool CallGenerateMaterials = true) const { return Cloner(Config, CallGenerateMaterials, false); }
  item* CloneAndLoad(inputfile&) const;
  const std::string& GetClassId() const { return ClassId; }
  ushort GetIndex() const { return Index; }
  const itemprototype* GetBase() const { return Base; }
  const std::map<ushort, itemdatabase>& GetConfig() const { return Config; }
  void CreateSpecialConfigurations() { }
  bool IsAbstract() const { return Config.begin()->second.IsAbstract; }
  const itemdatabase& ChooseBaseForConfig(ushort);
 protected:
  ushort Index;
  itemprototype* Base;
  std::map<ushort, itemdatabase> Config;
  item* (*Cloner)(ushort, bool, bool);
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
  virtual bool Fly(character*, uchar, ushort);
  virtual bool HitCharacter(character*, character*, float);
  virtual bool DogWillCatchAndConsume() const { return false; }
  virtual item* PrepareForConsuming(character*) { return this; }
  virtual bool Apply(character*);
  virtual bool Zap(character*, vector2d, uchar) { return false; }
  virtual bool Polymorph(stack*);
  virtual bool CheckPickUpEffect(character*) { return true; }
  virtual void StepOnEffect(character*) { }
  virtual bool IsTheAvatar() const { return false; }
  virtual void SignalSquarePositionChange(uchar) { }
  virtual bool IsBadFoodForAI(character*) const;
  virtual std::string GetConsumeVerb() const;
  virtual bool IsExplosive() const { return false; }
  virtual bool CatWillCatchAndConsume() const { return false; }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void ChargeFully(character*) { }
  virtual void SetSize(ushort Value) { Size = Value; }
  virtual ushort GetSize() const { return Size; }
  virtual ulong GetID() const { return ID; }
  virtual void SetID(ulong What) { ID = What; }
  virtual void TeleportRandomly();
  virtual ushort GetStrengthValue() const;
  slot* GetSlot() const { return Slot; }
  void SetSlot(slot* What) { Slot = What; }
  void PlaceToSlot(slot* Slot) { Slot->PutInItem(this); }
  void RemoveFromSlot();
  void MoveTo(stack*);
  static std::string ItemCategoryName(uchar);
  static bool EatableSorter(item* Item, const character* Char) { return Item->IsEatable(Char); }
  static bool DrinkableSorter(item* Item, const character* Char) { return Item->IsDrinkable(Char); }
  static bool OpenableSorter(item* Item, const character* Char) { return Item->IsOpenable(Char); }
  static bool ReadableSorter(item* Item, const character* Char) { return Item->IsReadable(Char); }
  static bool DippableSorter(item* Item, const character* Char) { return Item->IsDippable(Char); }
  static bool DipDestinationSorter(item* Item, const character* Char) { return Item->IsDipDestination(Char); }
  static bool AppliableSorter(item* Item, const character* Char) { return Item->IsAppliable(Char); }
  static bool ZappableSorter(item* Item, const character* Char) { return Item->IsZappable(Char); }
  static bool ChargeableSorter(item* Item, const character* Char) { return Item->IsChargeable(Char); }
  static bool HelmetSorter(item* Item, const character* Char) { return Item->IsHelmet(Char); }
  static bool AmuletSorter(item* Item, const character* Char) { return Item->IsAmulet(Char); }
  static bool CloakSorter(item* Item, const character* Char) { return Item->IsCloak(Char); }
  static bool BodyArmorSorter(item* Item, const character* Char) { return Item->IsBodyArmor(Char); }
  static bool RingSorter(item* Item, const character* Char) { return Item->IsRing(Char); }
  static bool GauntletSorter(item* Item, const character* Char) { return Item->IsGauntlet(Char); }
  static bool BeltSorter(item* Item, const character* Char) { return Item->IsBelt(Char); }
  static bool BootSorter(item* Item, const character* Char) { return Item->IsBoot(Char); }
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
  virtual bool IsOnGround() const { return GetSlot()->IsOnGround(); }
  virtual ushort GetResistance(uchar) const;
  virtual void GenerateLeftOvers(character*);
  virtual void Be();
  virtual bool RemoveMaterial(uchar) { return true; }
  ushort GetType() const { return GetProtoType()->GetIndex(); }
  virtual bool ReceiveDamage(character*, ushort, uchar);
  virtual void AddConsumeEndMessage(character*) const;
  virtual bool IsEqual(item*) const { return false; }
  virtual bool RaiseTheDead(character*) { return false; }
  virtual uchar GetBodyPartIndex() const { return 0xFF; }
  virtual const prototype* GetProtoType() const { return &item_ProtoType; }
  const database* GetDataBase() const { return DataBase; }
  virtual bool CanOpenLockType(uchar) const { return false; }
  virtual bool IsWhip() const { return false; }
  virtual DATABASEVALUE(ushort, Possibility);
  virtual DATABASEVALUE(vector2d, InHandsPic);
  virtual DATABASEVALUE(ulong, OfferModifier);
  virtual DATABASEVALUE(long, Score);
  virtual DATABASEBOOL(IsDestroyable);
  virtual DATABASEBOOL(CanBeWished);
  virtual DATABASEBOOL(IsMaterialChangeable);
  virtual DATABASEVALUE(uchar, WeaponCategory);
  virtual DATABASEBOOL(IsPolymorphSpawnable);
  virtual DATABASEBOOL(IsAutoInitializable);
  virtual DATABASEVALUE(uchar, Category);
  virtual DATABASEVALUE(ushort, SoundResistance);
  virtual DATABASEVALUE(ushort, EnergyResistance);
  virtual DATABASEVALUE(ushort, AcidResistance);
  virtual DATABASEVALUE(ushort, FireResistance);
  virtual DATABASEVALUE(ushort, PoisonResistance);
  virtual DATABASEVALUE(ushort, BulimiaResistance);
  virtual DATABASEVALUE(ushort, StrengthModifier);
  virtual DATABASEVALUE(ushort, FormModifier);
  virtual DATABASEVALUE(ulong, NPModifier);
  virtual DATABASEVALUE(ushort, DefaultSize);
  virtual DATABASEVALUE(ulong, DefaultMainVolume);
  virtual DATABASEVALUE(ulong, DefaultSecondaryVolume);
  virtual DATABASEVALUE(ulong, DefaultContainedVolume);
  virtual DATABASEVALUEWITHPARAMETER(vector2d, BitmapPos, ushort);
  virtual DATABASEVALUE(ulong, Price);
  virtual DATABASEVALUE(ushort, BaseEmitation);
  virtual DATABASEVALUE(const std::string&, Article);
  virtual DATABASEVALUE(const std::string&, Adjective);
  virtual DATABASEVALUE(const std::string&, AdjectiveArticle);
  virtual DATABASEVALUE(const std::string&, NameSingular);
  virtual DATABASEVALUE(const std::string&, NamePlural);
  virtual DATABASEVALUE(const std::string&, PostFix);
  virtual DATABASEVALUE(uchar, ArticleMode);
  virtual DATABASEVALUE(const std::vector<long>&, MainMaterialConfig);
  virtual DATABASEVALUE(const std::vector<long>&, SecondaryMaterialConfig);
  virtual DATABASEVALUE(const std::vector<long>&, ContainedMaterialConfig);
  virtual DATABASEVALUE(const std::vector<long>&, MaterialConfigChances);
  virtual DATABASEBOOL(IsPolymorphable);
  virtual DATABASEVALUE(const std::vector<std::string>&, Alias);
  virtual DATABASEVALUE(uchar, OKVisualEffects);
  virtual DATABASEBOOL(CanBeGeneratedInContainer);
  virtual DATABASEVALUE(uchar, ForcedVisualEffects);
  virtual DATABASEVALUE(uchar, Roundness);
  virtual DATABASEVALUE(ushort, GearStates);
  virtual DATABASEBOOL(IsTwoHanded);
  virtual DATABASEBOOL(CanBeBroken);
  virtual DATABASEVALUEWITHPARAMETER(vector2d, WallBitmapPos, ushort);
  virtual DATABASEVALUE(const std::string&, FlexibleNameSingular);
  virtual DATABASEBOOL(CanBePiled);
  static item* Clone(ushort, bool, bool) { return 0; }
  virtual bool CanBeSoldInLibrary(character* Librarian) const { return CanBeRead(Librarian); }
  virtual bool TryKey(item*, character*) { return false; }
  virtual bool TryToUnstuck(character*, vector2d) { return true; }
  virtual uchar GetVisualEffects() const { return VisualEffects; }
  virtual void SetVisualEffects(uchar What) { VisualEffects = What; }
  virtual bool TryToUnstuck(character*, ushort, vector2d) { return false; }
  virtual ulong GetBlockModifier(const character*) const;
  virtual bool IsSimiliarTo(item* Item) const { return Item->GetType() == GetType() && Item->GetConfig() == GetConfig(); }
  virtual bool IsPickable(character*) const { return true; }
  virtual bool CanBeSeenByPlayer() const;
  virtual bool CanBeSeenBy(const character*) const;
  virtual std::string GetDescription(uchar) const;
  virtual square* GetSquareUnder() const { return Slot ? Slot->GetSquareUnder() : 0; }
  lsquare* GetLSquareUnder() const { return static_cast<lsquare*>(Slot->GetSquareUnder()); }
  level* GetLevelUnder() const { return static_cast<level*>(Slot->GetSquareUnder()->GetAreaUnder()); }
  area* GetAreaUnder() const { return Slot->GetSquareUnder()->GetAreaUnder(); }
  vector2d GetPos() const { return Slot->GetSquareUnder()->GetPos(); }
  square* GetNearSquare(vector2d Pos) const { return Slot->GetSquareUnder()->GetAreaUnder()->GetSquare(Pos); }
  lsquare* GetNearLSquare(vector2d Pos) const { return static_cast<lsquare*>(Slot->GetSquareUnder()->GetAreaUnder()->GetSquare(Pos)); }
  virtual void SignalVolumeAndWeightChange();
  virtual void CalculateVolumeAndWeight();
  ulong GetVolume() const { return Volume; }
  ulong GetWeight() const { return Weight; }
  virtual void SignalEmitationIncrease(ushort);
  virtual void SignalEmitationDecrease(ushort);
  virtual void CalculateAll();
  virtual void DropEquipment() { }
  virtual bool DangerousToStepOn(const character*) const { return false; } 
  void WeaponSkillHit();
  virtual void SetTeam(ushort) { }
  virtual void SpecialGenerationHandler() { }
  item* Duplicate() const;
  virtual void SetIsActive(bool) { }
  ushort GetBaseMinDamage() const { return ushort(GetWeaponStrength() * 3 / 20000); }
  ushort GetBaseMaxDamage() const { return ushort(GetWeaponStrength() * 5 / 20000 + 1); }
  ushort GetBaseBlockValue(const character* Char) const { return ushort(12.5f * GetBlockModifier(Char) / (2500 + float(GetWeight() - 500))); }
  virtual void AddInventoryEntry(const character*, felist&, ushort, bool) const;
  virtual void AddMiscellaneousInfo(felist&) const;
  virtual ulong GetNutritionValue() const;
  virtual DATABASEBOOL(CanBeCloned);
  virtual DATABASEVALUE(ushort, BeamRange);
  virtual void SignalSpoil(material*);
  virtual bool AllowSpoil() const { return true; }
  bool CarriedByPlayer() const;
  bool CarriedBy(const character*) const;
  item* DuplicateToStack(stack*);
  virtual DATABASEVALUE(uchar, MaxCharges);
  virtual DATABASEVALUE(uchar, MinCharges);
  virtual bool CanBePiledWith(const item*, const character*) const;
  virtual ulong GetTotalExplosivePower() const { return 0; }
  virtual void Break();
  void Empty();
 protected:
  virtual item* RawDuplicate() const = 0;
  virtual void LoadDataBaseStats();
  virtual void VirtualConstructor(bool) { }
  void Initialize(ushort, bool, bool);
  virtual void InstallDataBase();
  virtual uchar GetGraphicsContainerIndex(ushort) const { return GRITEM; }
  virtual bool ShowMaterial() const;
  slot* Slot;
  bool Cannibalised;
  ushort Size;
  ulong ID;
  const database* DataBase;
  static prototype item_ProtoType;
  ulong Volume;
  ulong Weight;
};

#ifdef __FILE_OF_STATIC_ITEM_PROTOTYPE_DEFINITIONS__
#define ITEM_PROTOTYPE(name, baseproto) itemprototype name::name##_ProtoType(baseproto, &name::Clone, #name);
#else
#define ITEM_PROTOTYPE(name, baseproto)
#endif

#define ITEM(name, base, data)\
\
name : public base\
{\
 public:\
  name(ushort Config = 0, bool CallGenerateMaterials = true, bool Load = false) : base(donothing()) { Initialize(Config, CallGenerateMaterials, Load); }\
  name(ushort Config, material* FirstMaterial) : base(donothing()) { Initialize(Config, true, false); SetMainMaterial(FirstMaterial); }\
  name(material* FirstMaterial) : base(donothing()) { Initialize(0, true, false); SetMainMaterial(FirstMaterial); }\
  name(donothing D) : base(D) { }\
  virtual const prototype* GetProtoType() const { return &name##_ProtoType; }\
  static item* Clone(ushort Config, bool CallGenerateMaterials, bool Load) { return new name(Config, CallGenerateMaterials, Load); }\
 protected:\
  virtual item* RawDuplicate() const { return new name(*this); }\
  static itemprototype name##_ProtoType;\
  data\
}; ITEM_PROTOTYPE(name, &base##_ProtoType);

#define ABSTRACT_ITEM(name, base, data)\
\
name : public base\
{\
 public:\
  name(donothing D) : base(D) { }\
  virtual const prototype* GetProtoType() const { return &name##_ProtoType; }\
  static item* Clone(ushort, bool, bool) { return 0; }\
 protected:\
  static prototype name##_ProtoType;\
  data\
}; ITEM_PROTOTYPE(name, &base##_ProtoType);

#endif

