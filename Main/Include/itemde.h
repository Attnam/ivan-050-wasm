#ifndef __ITEMDE_H__
#define __ITEMDE_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include "felist.h"
#include "itemba.h"
#include "materde.h"
#include "wskill.h"
#include "slot.h"

class felist;

class ABSTRACT_ITEM
(
  materialcontainer,
  item,
 public:
  /*virtual void ReceiveHitEffect(character*, character*);
  virtual void DipInto(material*, character*);
  virtual ulong Price() const;
  virtual bool IsDippable(character*) const { return true; }*/
  virtual material* GetContainedMaterial() const { return ContainedMaterial; }
  virtual void SetContainedMaterial(material* What) { SetMaterial(ContainedMaterial, What, GetDefaultContainedVolume()); }
  virtual void ChangeContainedMaterial(material* What) { ChangeMaterial(ContainedMaterial, What, GetDefaultContainedVolume()); }
  void InitMaterials(material* M1, material* M2, bool CUP = true) { ObjectInitMaterials(MainMaterial, M1, GetDefaultMainVolume(), ContainedMaterial, M2, GetDefaultContainedVolume(), CUP); }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual material* GetConsumeMaterial() const { return ContainedMaterial; }
  virtual void SetConsumeMaterial(material* NewMaterial) { SetContainedMaterial(NewMaterial); }
  virtual void ChangeConsumeMaterial(material* NewMaterial) { ChangeContainedMaterial(NewMaterial); }
  virtual uchar GetMaterials() const { return 2; }
  virtual material* GetMaterial(ushort) const;
 protected:
  virtual void GenerateMaterials();
  virtual ushort GetMaterialColorB(ushort) const;
  virtual uchar GetAlphaB(ushort) const;
  material* ContainedMaterial;
);

class ITEM
(
  banana,
  materialcontainer,
 public:
  virtual ulong Price() const { return GetContainedMaterial()->RawPrice(); }
  virtual bool Zap(character*, vector2d, uchar);
  virtual uchar GetCharges() const { return Charges; }
  virtual void SetCharges(uchar What) { Charges = What; }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void ChargeFully(character*) { SetCharges(6); }
  virtual bool IsZappable(const character*) const { return true; }
  virtual bool IsChargeable(const character*) const { return true; }
  virtual void GenerateLeftOvers(character*);
 protected:
  virtual void VirtualConstructor(bool);
  virtual ushort GetMaterialColorA(ushort) const;
  uchar Charges;
);

class ITEM
(
  holybanana,
  banana,
  ;
);

class ITEM
(
  lantern,
  item,
 public:
  virtual void SignalSquarePositionChange(uchar);
  virtual void SetSquarePosition(uchar What) { SquarePosition = What; }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual bool ReceiveDamage(character*, short, uchar);
 protected:
  virtual uchar GetSpecialFlags(ushort) const;
  virtual void VirtualConstructor(bool);
  virtual vector2d GetBitmapPos(ushort) const { return vector2d(0, SquarePosition == CENTER ? 256 : 192); }
  uchar SquarePosition;
);

class ITEM
(
  can,
  materialcontainer,
 public:
  virtual ulong Price() const { return GetContainedMaterial() ? GetContainedMaterial()->RawPrice() : 0; }
  virtual item* BetterVersion() const;
  virtual void GenerateLeftOvers(character*);
 protected:
  virtual void AddPostFix(std::string& String) const { AddContainerPostFix(String); }
  virtual bool AddAdjective(std::string& String, bool Articled) const { return AddEmptyAdjective(String, Articled); }
  virtual vector2d GetBitmapPos(ushort) const { return vector2d(16, GetContainedMaterial() ? 288 : 304); }
);

class ITEM
(
  lump,
  item,
 public:
  virtual void ReceiveHitEffect(character*, character*);
  virtual material* CreateDipMaterial();
  virtual ulong Price() const { return GetMainMaterial()->RawPrice(); }
  virtual bool IsDipDestination(const character*) const { return true; }
 protected:
  virtual void AddPostFix(std::string& String) const { AddLumpyPostFix(String); }
  virtual bool ShowMaterial() const { return false; }
);

class ITEM
(
  meleeweapon,
  item,
 public:
  virtual void ReceiveHitEffect(character*, character*);
  virtual void DipInto(material*, character*);
  virtual ulong Price() const;
  virtual bool IsDippable(const character*) const { return true; }
  virtual material* GetSecondaryMaterial() const { return SecondaryMaterial; }
  virtual void SetSecondaryMaterial(material* What) { SetMaterial(SecondaryMaterial, What, GetDefaultSecondaryVolume()); }
  virtual void ChangeSecondaryMaterial(material* What) { ChangeMaterial(SecondaryMaterial, What, GetDefaultSecondaryVolume()); }
  virtual material* GetContainedMaterial() const { return ContainedMaterial; }
  virtual void SetContainedMaterial(material* What) { SetMaterial(ContainedMaterial, What, GetDefaultContainedVolume()); }
  virtual void ChangeContainedMaterial(material* What) { ChangeMaterial(ContainedMaterial, What, GetDefaultContainedVolume()); }
  void InitMaterials(material* M1, material* M2, material* M3, bool CUP = true) { ObjectInitMaterials(MainMaterial, M1, GetDefaultMainVolume(), SecondaryMaterial, M2, GetDefaultSecondaryVolume(), ContainedMaterial, M3, GetDefaultContainedVolume(), CUP); }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual uchar GetMaterials() const { return 3; }
  virtual material* GetMaterial(ushort) const;
 protected:
  virtual void GenerateMaterials();
  virtual ushort GetMaterialColorB(ushort) const;
  virtual uchar GetAlphaB(ushort) const;
  material* SecondaryMaterial;
  material* ContainedMaterial;
);

class ITEM
(
  valpurusjustifier,
  meleeweapon,
  ;
);

class ITEM
(
  neercseulb,
  meleeweapon,
  ;
);

class ITEM
(
  pickaxe,
  meleeweapon,
 public:
  virtual bool Apply(character*);
  virtual bool IsAppliable(const character*) const;
  ;
);

class ITEM
(
  bodyarmor,
  item,
 public:
  virtual ulong Price() const;
  virtual bool IsBodyArmor(const character*) const { return true; }
);

class ITEM
(
  platemail,
  bodyarmor,
 public:
  virtual bool ReceiveDamage(character*, short, uchar);
 protected:
  virtual const std::string& GetNameSingular() const;
);

class ITEM
(
  goldeneagleshirt,
  bodyarmor,
 public:
  virtual ushort GetStrengthValue() const { return 100; }
  virtual short GetOfferValue(char) const { return 750; }
  virtual bool IsGoldenEagleShirt() const { return true; };
  virtual bool IsConsumable(const character*) const { return false; }
);

class ITEM
(
  potion,
  materialcontainer,
 public:
  virtual ulong Price() const { return GetContainedMaterial() ? GetContainedMaterial()->RawPrice() : 0; }
  virtual item* BetterVersion() const;
  virtual void DipInto(material*, character*);
  virtual material* CreateDipMaterial();
  virtual bool IsDippable(const character*) const { return !GetContainedMaterial(); }
  virtual void GenerateLeftOvers(character*);
  virtual bool ReceiveDamage(character*, short, uchar);
 protected:
  virtual void AddPostFix(std::string& String) const { AddContainerPostFix(String); }
  virtual bool AddAdjective(std::string& String, bool Articled) const { return AddEmptyAdjective(String, Articled); }
);

class ITEM
(
  bananapeals,
  item,
 public:
  virtual item* BetterVersion() const { return new banana; }
  virtual bool StepOnEffect(character *);
);

class ITEM
(
  brokenbottle,
  item,
 public:
  virtual item* BetterVersion() const;
  virtual bool StepOnEffect(character*);
);

class ABSTRACT_ITEM
(
  scroll,
  item,
 public:
  virtual bool CanBeRead(character*) const;
  virtual bool IsReadable(const character*) const { return true; }
  virtual bool ReceiveDamage(character*, short, uchar);
);

class ITEM
(
  scrollofcreatemonster,
  scroll,
 public:
  virtual bool Read(character*);
  virtual void FinishReading(character*);
);

class ITEM
(
  scrollofteleportation,
  scroll,
 public:
  virtual bool Read(character*);
  virtual void FinishReading(character*);
);

class ITEM
(
  scrollofcharging,
  scroll,
 public:
  virtual bool Read(character*);
  virtual void FinishReading(character*);
);

class ITEM
(
  nut,
  item,
  ;
);

class ITEM
(
  leftnutofpetrus,
  nut,
 public:
  virtual bool IsPetrussNut() const { return true; }
  virtual bool IsConsumable(const character*) const { return false; }
);

class ITEM
(
  bone,
  item,
 public:
  virtual bool DogWillCatchAndConsume() const { return GetConsumeMaterial()->GetConfig() == BONE; }

);

class ITEM
(
  loaf,
  item,
 public:
  virtual ulong Price() const { return GetMainMaterial() ? GetMainMaterial()->RawPrice() : 0; }
 protected:
  virtual void AddPostFix(std::string& String) const { AddLumpyPostFix(String); }
  virtual bool ShowMaterial() const { return false; }
);

class ITEM
(
  scrollofwishing,
  scroll,
 public:
  virtual bool Read(character*);
  virtual void FinishReading(character*);
);

class ITEM
(
  cheapcopyofleftnutofpetrus,
  nut,
  ;
);

class ABSTRACT_ITEM
(
  wand,
  item,
 public:
  virtual bool Apply(character*);
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual uchar GetCharges() const { return Charges; }
  virtual void SetCharges(uchar What) { Charges = What; }
  virtual uchar GetTimesUsed() const { return TimesUsed; }
  virtual void SetTimesUsed(uchar What) { TimesUsed = What; }
  virtual void Beam(character*, const std::string&, uchar, uchar);
  virtual bool BeamEffect(character*, const std::string&, uchar, lsquare*) { return false; };
  virtual void ChargeFully(character*) { SetTimesUsed(0); }
  virtual bool IsAppliable(const character*) const { return true; }
  virtual bool IsZappable(const character*) const { return true; }
  virtual bool IsChargeable(const character*) const { return true; }
  virtual bool ReceiveDamage(character*, short, uchar);
 protected:
  virtual ushort GetBeamColor() const { return WHITE; }
  virtual void VirtualConstructor(bool);
  virtual void AddPostFix(std::string&) const;
  uchar Charges;
  uchar TimesUsed;
);

class ITEM
(
  wandofpolymorph,
  wand,
 public:
  virtual bool Zap(character*, vector2d, uchar);
  virtual bool BeamEffect(character*, const std::string&, uchar, lsquare*);
 protected:
  virtual ushort GetBeamColor() const { return BLUE; }
  virtual void VirtualConstructor(bool);
);

class ITEM
(
  brokenlantern,
  lantern,
 public:
  virtual bool ReceiveDamage(character*, short, uchar) { return false; }
 protected:
  virtual vector2d GetBitmapPos(ushort) const { return vector2d(0, SquarePosition == CENTER ? 304 : 208); }
);

class ITEM
(
  scrollofchangematerial,
  scroll,
 public:
  virtual bool Read(character*);
  virtual void FinishReading(character*);
);

class ITEM
(
  avatarofvalpurus,
  item,
 public:
  virtual bool IsTheAvatar() const { return true; }
  virtual bool IsConsumable(const character*) const { return false; }
);

class ITEM
(
  wandofstriking,
  wand,
 public:
  virtual bool Zap(character*, vector2d, uchar);
  virtual bool BeamEffect(character*, const std::string&, uchar, lsquare*);
 protected:
  virtual void VirtualConstructor(bool);
  virtual ushort GetBeamColor() const { return WHITE; }
);

class ITEM
(
  brokenplatemail,
  platemail,
 public:
  virtual bool ReceiveDamage(character*, short, uchar) { return false; }
);

class ITEM
(
  kiwi,
  item,
 public:
  virtual ulong Price() const { return GetMainMaterial()->RawPrice(); }
);

class ITEM
(
  pineapple,
  item,
 public:
  virtual ulong Price() const { return GetMainMaterial()->RawPrice(); }
);

class ITEM
(
  palmbranch,
  item,
 public:
  virtual bool IsShield(const character*) const { return true; }
);

class ITEM
(
  whip,
  meleeweapon,
 public:
  virtual bool IsWhip() const { return true; }
 protected:
  virtual ushort GetFormModifier() const;
);

class ITEM
(
  backpack,
  materialcontainer,
 public:
  virtual ulong Price() const { return GetContainedMaterial() ? GetContainedMaterial()->RawPrice() : 0; }
  virtual bool Apply(character*);
  virtual bool IsAppliable(const character*) const { return true; }
  virtual bool ReceiveDamage(character*, short, uchar);
 protected:
  virtual void AddPostFix(std::string& String) const { AddContainerPostFix(String); }
);

class ITEM
(
  holybook,
  item,
 public:
  virtual bool CanBeRead(character*) const;
  virtual bool Read(character*);
  virtual bool IsReadable(const character*) const { return true; }
  virtual bool ReceiveDamage(character*, short, uchar);
  virtual void FinishReading(character*);
 protected:
  virtual ushort GetMaterialColorA(ushort) const;
  virtual void AddPostFix(std::string& String) const { AddDivineMasterDescription(String, GetConfig()); }
  virtual bool ShowMaterial() const { return false; }
);

class ITEM
(
  fiftymillionroubles,
  item,
  ;
);

class ITEM
(
  oillamp,
  item,
 public:
  virtual ulong Price() const { return GetMainMaterial()->RawPrice(); }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual bool GetInhabitedByGenie() const { return InhabitedByGenie; }
  virtual void SetInhabitedByGenie(bool What) { InhabitedByGenie = What; }
  virtual bool Apply(character*);
  virtual bool IsAppliable(const character*) const { return true; }
 protected:
  virtual void VirtualConstructor(bool);
  bool InhabitedByGenie;
);

class ITEM
(
  stone,
  item,
 public:
  virtual ulong Price() const { return GetMainMaterial()->RawPrice() * 2; }
);

class ITEM
(
  wandoffireballs,
  wand,
 public:
  virtual bool Zap(character*, vector2d, uchar);
  virtual bool BeamEffect(character*, const std::string&, uchar, lsquare*);
 protected:
  virtual void VirtualConstructor(bool);
  virtual ushort GetBeamColor() const { return YELLOW; }
);

class ITEM
(
  scrolloftaming,
  scroll,
 public:
  virtual bool Read(character*);
  virtual void FinishReading(character*);
);

class ITEM
(
  wandofteleportation,
  wand,
 public:
  virtual bool Zap(character*, vector2d, uchar);
  virtual bool BeamEffect(character*, const std::string&, uchar, lsquare*);
 protected:
  virtual void VirtualConstructor(bool);
  virtual ushort GetBeamColor() const { return GREEN; }
);

class ITEM
(
  mine,
  materialcontainer,
 public:
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual uchar GetCharged() const { return Charged; }
  virtual void SetCharged(bool What) { Charged = What; }
  virtual bool StepOnEffect(character *);
  virtual bool IsChargeable(const character*) const { return true; }
  virtual bool ReceiveDamage(character*, short, uchar);
  virtual bool IsVisible() const { return Visible; }
  virtual void SetIsVisible(bool What) { Visible = What; }
 protected:
  virtual void VirtualConstructor(bool);
  bool Charged;
  bool Visible;
);

class ITEM
(
  wandofhaste,
  wand,
 public:
  virtual bool Zap(character*, vector2d, uchar);
  virtual bool BeamEffect(character*, const std::string&, uchar, lsquare*);
 protected:
  virtual void VirtualConstructor(bool);
  virtual ushort GetBeamColor() const { return RED; }
);

class ITEM
(
  wandofslow,
  wand,
 public:
  virtual bool Zap(character*, vector2d, uchar);
  virtual bool BeamEffect(character*, const std::string&, uchar, lsquare*);
 protected:
  virtual void VirtualConstructor(bool);
  virtual ushort GetBeamColor() const { return GREEN; }
);

class ITEM
(
  key,
  item,
 public:
  virtual bool Apply(character*);
  virtual void SetLockType(uchar What) { LockType = What; }
  virtual uchar GetLockType() const { return LockType; }
  virtual bool IsAppliable(const character*) const { return true; }
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual bool CanOpenDoors() const { return true; }
  virtual bool CanOpenLockType(uchar AnotherLockType) const { return LockType == AnotherLockType; }
 protected:
  virtual const std::string& GetAdjective() const;
  virtual void VirtualConstructor(bool);
  uchar LockType;
);

class ITEM
(
  shield,
  item,
 public:
  virtual ulong Price() const { return GetMainMaterial()->RawPrice(); } // This should be overwritten, when the effectivness of the shield can be calculated somehow
  virtual bool IsShield(const character*) const { return true; }
);

class ITEM
(
  cloak,
  item,
 public:
  virtual ulong Price() const { return GetMainMaterial()->RawPrice(); } // This should be overwritten, when the effectivness of the cloak can be calculated somehow
  virtual bool IsCloak(const character*) const { return true; }
 protected:
  virtual ushort GetMaterialColorB(ushort) const { return MAKE_RGB(111, 64, 37); }
);

class ITEM
(
  boot,
  item,
 public:
  virtual ulong Price() const { return GetMainMaterial()->RawPrice(); } // This should be overwritten, when the effectivness of the boots can be calculated someho
  virtual bool IsBoot(const character*) const { return true; }
);

class ITEM
(
  gauntlet, 
  item,
 public:
  virtual ulong Price() const { return GetMainMaterial()->RawPrice(); } // This should be overwritten, when the effectivness of the gauntlets can be calculated somehow
  virtual bool IsGauntlet(const character*) const { return true; }
);

class ITEM
(
  belt, 
  item,
 public:
  virtual ulong Price() const { return GetMainMaterial()->RawPrice(); } // This should be overwritten, when the effectivness of the belt can be calculated somehow
  virtual bool IsBelt(const character*) const { return true; }
  virtual ushort GetFormModifier() const; 
);

class ITEM
(
  ring, 
  item,
 public:
  virtual ulong Price() const { return GetMainMaterial()->RawPrice(); } // This should be overwritten, when the effectivness of the belt can be calculated somehow
  virtual bool IsRing(const character*) const { return true; }
 protected:
  virtual ushort GetMaterialColorB(ushort) const { return MAKE_RGB(200, 200, 200); }
);

class ITEM
(
  amulet, 
  item,
 public:
  virtual ulong Price() const { return GetMainMaterial()->RawPrice(); } // This should be overwritten, when the effectivness of the belt can be calculated somehow
  virtual bool IsAmulet(const character*) const { return true; }
 protected:
  virtual ushort GetMaterialColorB(ushort) const { return MAKE_RGB(111, 64, 37); }
);

class ABSTRACT_ITEM
(
  bodypart,
  materialcontainer,
 public:
  friend class corpse;
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual uchar GetGraphicsContainerIndex(ushort) const { return GRHUMANOID; }
  character* GetMaster() const { return Master; }
  humanoid* GetHumanoidMaster() const;
  void SetMaster(character* What) { Master = What; }
  virtual ushort GetStrengthValue() const;
  short GetMaxHP() const { return MaxHP; }
  void SetHP(short);
  short GetHP() const { return HP; }
  void EditHP(short);
  virtual ushort GetTotalResistance(uchar) const = 0;
  virtual bool ReceiveDamage(character*, short, uchar);
  const std::string& GetOwnerDescription() const { return OwnerDescription; }
  void SetOwnerDescription(const std::string& What) { OwnerDescription = What; }
  bool GetUnique() const { return Unique; }
  void SetUnique(bool What) { Unique = What; }
  ulong GetRegenerationCounter() const { return RegenerationCounter; }
  void SetRegenerationCounter(ulong What) { RegenerationCounter = What; }
  void EditRegenerationCounter(long What) { RegenerationCounter += What; }
  void Regenerate();
  virtual ushort DangerWeight() const = 0;
  virtual ushort Danger(ulong, bool) const;
  virtual void DropEquipment() { }
  virtual material* GetConsumeMaterial() const { return MainMaterial; }
  virtual void SetConsumeMaterial(material* NewMaterial) { SetMainMaterial(NewMaterial); }
  virtual void ChangeConsumeMaterial(material* NewMaterial) { ChangeMainMaterial(NewMaterial); }
  std::vector<vector2d>& GetBitmapPosVector() { return BitmapPos; }
  std::vector<ushort>& GetColorBVector() { return ColorB; }
  std::vector<ushort>& GetColorCVector() { return ColorC; }
  std::vector<ushort>& GetColorDVector() { return ColorD; }
  std::vector<uchar>& GetSpecialFlagsVector() { return SpecialFlags; }
  virtual bool ApplyExperience() { return false; }
  virtual void RaiseStats() { }
  virtual void LowerStats() { }
  virtual void InitSpecialAttributes() { }
  void SignalEquipmentAdd(gearslot*);
  void SignalEquipmentRemoval(gearslot*);
  virtual void Mutate();
  ulong GetBodyPartVolume() const { return BodyPartVolume; }
  ulong GetCarriedWeight() const { return CarriedWeight; }
  virtual item* GetEquipment(ushort) const { return 0; }
  virtual ushort GetEquipmentSlots() const { return 0; }
  virtual void CalculateVolumeAndWeight();
  virtual void CalculateEmitation();
  virtual void CalculateMaxHP();
  virtual void SignalVolumeAndWeightChange();
  void RestoreHP() { HP = MaxHP; }
  virtual void CalculateAttackStrength() { }
  virtual void CalculateToHitValue() { }
  virtual void CalculateAPCost() { }
  void CalculateAttackInfo();
 protected:
  virtual uchar GetMaxAlpha(ushort) const;
  virtual void GenerateMaterials() { }
  virtual void VirtualConstructor(bool);
  virtual void GetPostFix(std::string&) const;
  virtual bool ShowMaterial() const { return false; }
  virtual uchar GetArticleMode() const { return Unique ? DEFINITEARTICLE : NORMALARTICLE; }
  virtual ushort GetMaterialColorA(ushort) const;
  virtual ushort GetMaterialColorB(ushort Frame) const { return ColorB[Frame]; }
  virtual ushort GetMaterialColorC(ushort Frame) const { return ColorC[Frame]; }
  virtual ushort GetMaterialColorD(ushort Frame) const { return ColorD[Frame]; }
  virtual vector2d GetBitmapPos(ushort Frame) const { return BitmapPos[Frame]; }
  virtual uchar GetSpecialFlags(ushort Frame) const { return SpecialFlags[Frame]; }
  std::string OwnerDescription;
  std::vector<vector2d> BitmapPos;
  std::vector<ushort> ColorB;
  std::vector<ushort> ColorC;
  std::vector<ushort> ColorD;
  std::vector<uchar> SpecialFlags;
  short HP;
  short MaxHP;
  bool Unique;
  ulong RegenerationCounter;
  ulong CarriedWeight;
  ulong BodyPartVolume;
  character* Master;
);

class ITEM
(
  head,
  bodypart,
 public:
  virtual ~head();
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual ushort GetTotalResistance(uchar) const;
  void SetHelmet(item* What) { HelmetSlot.PutInItem(What); }
  item* GetHelmet() const { return *HelmetSlot; }
  void SetAmulet(item* What) { AmuletSlot.PutInItem(What); }
  item* GetAmulet() const { return *AmuletSlot; }
  virtual ushort DangerWeight() const;
  virtual void DropEquipment();
  virtual uchar GetBodyPartIndex() const { return HEADINDEX; }
  float GetBiteStrength() const { return BiteStrength; }
  float GetBiteToHitValue() const { return 1.0f; }
  long GetBiteAPCost() const { return BiteAPCost; }
  virtual void InitSpecialAttributes();
  virtual item* GetEquipment(ushort) const;
  virtual ushort GetEquipmentSlots() const { return 2; }
  ulong GetBaseBiteStrength() const { return BaseBiteStrength; }
  void SetBaseBiteStrength(ulong What) { BaseBiteStrength = What; }
  virtual void CalculateAttackStrength();
  virtual void CalculateAPCost();
 protected:
  virtual void VirtualConstructor(bool);
  gearslot HelmetSlot;
  gearslot AmuletSlot;
  ulong BaseBiteStrength;
  float BiteStrength;
  long BiteAPCost;
);

class ABSTRACT_ITEM
(
  torso,
  bodypart,
 public:
  virtual ushort DangerWeight() const;
  virtual uchar GetBodyPartIndex() const { return TORSOINDEX; }
 protected:
  virtual bool ReceiveDamage(character*, short, uchar);
);

class ITEM
(
  normaltorso,
  torso,
 public:
  virtual uchar GetGraphicsContainerIndex(ushort) const { return GRCHARACTER; }
  virtual ushort GetTotalResistance(uchar) const;
);

class ITEM
(
  humanoidtorso,
  torso,
 public:
  virtual ~humanoidtorso();
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual uchar GetGraphicsContainerIndex(ushort) const { return GRHUMANOID; }
  virtual ushort GetTotalResistance(uchar) const;
  void SetBodyArmor(item* What) { BodyArmorSlot.PutInItem(What); }
  item* GetBodyArmor() const { return *BodyArmorSlot; }
  void SetCloak(item* What) { CloakSlot.PutInItem(What); }
  item* GetCloak() const { return *CloakSlot; }
  void SetBelt(item* What) { BeltSlot.PutInItem(What); }
  item* GetBelt() const { return *BeltSlot; }
  virtual void DropEquipment();
  virtual item* GetEquipment(ushort) const;
  virtual ushort GetEquipmentSlots() const { return 3; }
 protected:
  virtual void VirtualConstructor(bool);
  gearslot BodyArmorSlot;
  gearslot CloakSlot;
  gearslot BeltSlot;
);

class ABSTRACT_ITEM
(
  arm,
  bodypart,
 public:
  virtual ~arm();
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual ushort GetTotalResistance(uchar) const;
  float GetWieldedStrength() const;
  float GetWieldedToHitValue() const;
  void SetWielded(item* What) { WieldedSlot.PutInItem(What); }
  item* GetWielded() const { return *WieldedSlot; }
  void SetGauntlet(item* What) { GauntletSlot.PutInItem(What); }
  item* GetGauntlet() const { return *GauntletSlot; }
  void SetRing(item* What) { RingSlot.PutInItem(What); }
  item* GetRing() const { return *RingSlot; }
  virtual ushort DangerWeight() const;
  virtual void DropEquipment();
  float GetUnarmedToHitValue() const;
  float GetUnarmedStrength() const;
  virtual void Hit(character*);
  ushort GetAttribute(ushort) const;
  bool EditAttribute(ushort, short);
  void EditExperience(ushort, long);
  virtual bool ApplyExperience();
  virtual void RaiseStats();
  virtual void LowerStats();
  void SetStrength(ushort What) { Strength = What; }
  void SetDexterity(ushort What) { Dexterity = What; }
  virtual void InitSpecialAttributes();
  virtual void Mutate();
  ushort GetDexterity() const { return Dexterity; }
  ushort GetStrength() const { return Strength; }
  virtual arm* GetPairArm() const = 0;
  virtual sweaponskill* GetCurrentSingleWeaponSkill() const = 0;
  long GetWieldedAPCost() const;
  long GetUnarmedAPCost() const;
  virtual item* GetEquipment(ushort) const;
  virtual ushort GetEquipmentSlots() const { return 3; }
  ulong GetBaseUnarmedStrength() const { return BaseUnarmedStrength; }
  void SetBaseUnarmedStrength(ulong What) { BaseUnarmedStrength = What; }
  virtual void CalculateAttackStrength();
  virtual void CalculateToHitValue();
  virtual void CalculateAPCost();
  float GetAttackStrength() const { return AttackStrength; }
  float GetToHitValue() const { return ToHitValue; }
  long GetAPCost() const { return APCost; }
  bool PairArmAllowsMelee() const;
  void AddAttackInfo(felist&) const;
  virtual void SignalVolumeAndWeightChange();
  bool DualWieldIsActive() const;
 protected:
  virtual void VirtualConstructor(bool);
  gearslot WieldedSlot;
  gearslot GauntletSlot;
  gearslot RingSlot;
  ushort Strength;
  ushort Dexterity;
  long StrengthExperience;
  long DexterityExperience;
  ulong BaseUnarmedStrength;
  float AttackStrength;
  float ToHitValue;
  long APCost;
);

class ITEM
(
  rightarm,
  arm,
 public:
  virtual uchar GetBodyPartIndex() const { return RIGHTARMINDEX; }
  virtual arm* GetPairArm() const;
  virtual sweaponskill* GetCurrentSingleWeaponSkill() const;
 protected:
  virtual void VirtualConstructor(bool);
  virtual uchar GetSpecialFlags(ushort) const { return STRIGHTARM; }
);

class ITEM
(
  leftarm,
  arm,
 public:
  virtual uchar GetBodyPartIndex() const { return  LEFTARMINDEX; }
  virtual arm* GetPairArm() const;
  virtual sweaponskill* GetCurrentSingleWeaponSkill() const;
 protected:
  virtual void VirtualConstructor(bool);
  virtual uchar GetSpecialFlags(ushort) const { return STLEFTARM; }
);

class ITEM
(
  groin,
  bodypart,
 public:
  virtual ushort GetTotalResistance(uchar) const;
  virtual ushort DangerWeight() const;
  virtual uchar GetBodyPartIndex() const { return GROININDEX; }
 protected:
  virtual uchar GetSpecialFlags(ushort) const { return STGROIN; }
);

class ABSTRACT_ITEM
(
  leg,
  bodypart,
 public:
  virtual ~leg();
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual ushort GetTotalResistance(uchar) const;
  void SetBoot(item* What) { BootSlot.PutInItem(What); }
  item* GetBoot() const { return *BootSlot; }
  virtual ushort DangerWeight() const;
  virtual void DropEquipment();
  float GetKickToHitValue() const { return KickToHitValue; }
  float GetKickStrength() const { return KickStrength; }
  ushort GetAttribute(ushort) const;
  bool EditAttribute(ushort, short);
  void EditExperience(ushort, long);
  virtual bool ApplyExperience();
  virtual void RaiseStats();
  virtual void LowerStats();
  void SetStrength(ushort What) { Strength = What; }
  void SetAgility(ushort What) { Agility = What; }
  ushort GetAgility() const { return Agility; }
  ushort GetStrength() const { return Strength; }
  virtual void InitSpecialAttributes();
  virtual void Mutate();
  long GetKickAPCost() const { return KickAPCost; }
  virtual item* GetEquipment(ushort) const;
  virtual ushort GetEquipmentSlots() const { return 1; }
  ulong GetBaseKickStrength() const { return BaseKickStrength; }
  void SetBaseKickStrength(ulong What) { BaseKickStrength = What; }
  virtual void CalculateAttackStrength();
  virtual void CalculateToHitValue();
  virtual void CalculateAPCost();
 protected:
  virtual void VirtualConstructor(bool);
  gearslot BootSlot;
  ushort Strength;
  ushort Agility;
  long StrengthExperience;
  long AgilityExperience;
  ulong BaseKickStrength;
  float KickToHitValue;
  float KickStrength;
  long KickAPCost;
);

class ITEM
(
  rightleg,
  leg,
 public:
  virtual uchar GetBodyPartIndex() const { return RIGHTLEGINDEX; }
 protected:
  virtual void VirtualConstructor(bool);
  virtual uchar GetSpecialFlags(ushort) const { return STRIGHTLEG; }
);

class ITEM
(
  leftleg,
  leg,
 public:
  virtual uchar GetBodyPartIndex() const { return LEFTLEGINDEX; }
 protected:
  virtual void VirtualConstructor(bool);
  virtual uchar GetSpecialFlags(ushort) const { return STLEFTLEG; }
);

class ITEM
(
  headofelpuri,
  item, // can't wear equipment, so not "head"
 public:
  virtual bool IsHeadOfElpuri() const { return true; }
  virtual bool IsConsumable(const character*) const { return false; }
);

class ITEM
(
  corpse,
  item,
 public:
  virtual ~corpse();
  virtual bool Consume(character*, long);
  virtual bool IsConsumable(const character*) const;
  virtual short GetOfferValue(char) const;
  virtual float GetWeaponStrength() const;
  virtual bool IsBadFoodForAI(character*) const;
  virtual ushort GetStrengthValue() const;
  virtual void Be() { }
  virtual bool IsDipDestination(const character*) const { return true; }
  virtual character* GetDeceased() const { return Deceased; }
  virtual void SetDeceased(character*);
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void AddConsumeEndMessage(character*) const;
  virtual void GenerateLeftOvers(character*);
  virtual long Score() const;
  virtual bool IsDestroyable() const;
  virtual ulong Price() const;
  virtual item* PrepareForConsuming(character*);
  virtual uchar GetMaterials() const { return 2; }
  virtual material* GetMaterial(ushort) const;
  virtual bool RaiseTheDead(character*);
  virtual std::string GetConsumeVerb() const;
  virtual bool IsEatable(const character* Eater) const { return IsConsumable(Eater); }
  virtual void CalculateVolumeAndWeight();
  virtual void CalculateEmitation();
 protected:
  virtual void GenerateMaterials() { }
  virtual ushort GetMaterialColorA(ushort) const;
  virtual ushort GetMaterialColorB(ushort) const;
  virtual uchar GetAlphaA(ushort) const;
  virtual bool ShowMaterial() const { return false; }
  virtual void AddPostFix(std::string&) const;
  virtual vector2d GetBitmapPos(ushort) const;
  virtual ushort GetSize() const;
  character* Deceased;
);

class ITEM
(
  wandoflocking,
  wand,
 public:
  virtual bool Zap(character*, vector2d, uchar);
  virtual bool BeamEffect(character*, const std::string&, uchar, lsquare*);
 protected:
  virtual void VirtualConstructor(bool);
  virtual ushort GetBeamColor() const { return WHITE; }
);

class ITEM
(
  wandofresurrection,
  wand,
 public:
  virtual bool Zap(character*, vector2d, uchar);
  virtual bool BeamEffect(character*, const std::string&, uchar, lsquare*);
 protected:
  virtual void VirtualConstructor(bool);
  virtual ushort GetBeamColor() const { return BLACK; }
);

class ITEM
(
  whistle,
  item,
 public:
  virtual bool Apply(character*);
  virtual bool IsAppliable(const character*) const { return true; }
  virtual void BlowEffect(character*);
);

class ITEM
(
  magicalwhistle,
  whistle,
 public:
  virtual void BlowEffect(character*);
);

class ITEM
(
  chest,
  item,
 public:
  virtual ~chest();
  virtual bool Open(character*);
  virtual bool IsOpenable(const character*) const { return true; }
  virtual bool TryKey(item*, character*);
  virtual void Lock() { Locked = true; }
  virtual uchar GetLockType() const { return LockType; }
  virtual void SetLockType(uchar What) { LockType = What; }
  virtual bool IsLocked() const { return Locked; }
  virtual void SetIsLocked(bool What) { Locked = What; }
  virtual stack* GetContained() const { return Contained; }
  virtual bool TakeSomethingFrom(character*);
  virtual bool PutSomethingIn(character*);
  virtual void Load(inputfile&);
  virtual void Save(outputfile&) const;
  virtual ulong GetStorageVolume() const { return StorageVolume; }
  virtual bool Polymorph(stack*);
  virtual bool FitsIn(item*) const;
  virtual void CalculateVolumeAndWeight();
 protected:
  virtual ushort GetMaterialColorB(ushort) const { return MAKE_RGB(80, 80, 80); }
  virtual void AddPostFix(std::string& String) const { AddLockPostFix(String, LockType); }
  virtual void VirtualConstructor(bool);
  ulong StorageVolume;
  stack* Contained;
  uchar LockType;
  bool Locked;
);

class ITEM
(
  beartrap,
  item,
 public:
  virtual void Load(inputfile&);
  virtual void Save(outputfile&) const;
  virtual bool StepOnEffect(character*);
  virtual bool TryToUnstuck(character*, ushort, vector2d);
  virtual bool CheckPickUpEffect(character*);
  virtual bool IsPickable(character*) const;
  virtual bool IsVisible() const { return Visible; }
  virtual void SetIsVisible(bool What) { Visible = What; }
 protected:
  virtual void VirtualConstructor(bool);
  bool IsActivated;
  bool Visible;
); 

class ITEM
(
  wandofdoorcreation,
  wand,
 public:
  virtual bool Zap(character*, vector2d, uchar);
 protected:
  virtual void VirtualConstructor(bool);
);

class ITEM
(
  wandofinvisibility,
  wand,
 public:
  virtual bool Zap(character*, vector2d, uchar);
  virtual bool BeamEffect(character*, const std::string&, uchar, lsquare*);
 protected:
  virtual void VirtualConstructor(bool);
  virtual ushort GetBeamColor() const { return WHITE; }
);

class ITEM
(
  helmet,
  item,
 public:
  virtual ulong Price() const { return GetMainMaterial()->RawPrice(); } // Needs improving
  virtual bool IsHelmet(const character*) const { return true; }
);

class ITEM
(
 stethoscope,
 item,
 public:
 virtual bool Apply(character*);
 virtual bool IsAppliable(const character*) const { return true; };
 virtual bool ListenTo(lsquare*,character*);
);
#endif


