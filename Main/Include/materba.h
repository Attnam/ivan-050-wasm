#ifndef __MATERBA_H__
#define __MATERBA_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include <string>
#include <map>

#include "typedef.h"
#include "ivandef.h"

#define MAKE_MATERIAL material::MakeMaterial

class character;
class item;
class entity;
class outputfile;
class inputfile;
class material;
template <class type> class database;

struct materialdatabase
{
  void InitDefaults() { }
  ushort StrengthValue;
  ushort ConsumeType;
  ushort Density;
  ushort OfferValue;
  ushort Color;
  ulong PriceModifier;
  bool IsSolid;
  ushort Emitation;
  bool CanBeWished;
  uchar Alignment;
  ushort NutritionValue;
  bool IsAlive;
  bool IsBadFoodForAI;
  bool IsFlammable;
  bool IsExplosive;
  std::string NameStem;
  std::string AdjectiveStem;
  std::string Article;
  uchar Effect;
  uchar ConsumeEndMessage;
  uchar HitMessage;
  ulong ExplosivePower;
  uchar Alpha;
  bool CreateDivineConfigurations;
  uchar Flexibility;
};

class materialprototype
{
 public:
  friend class database<material>;
  materialprototype(materialprototype*, material* (*)(ushort, ulong, bool), const std::string&);
  material* Clone(ushort Config, ulong Volume) const { return Cloner(Config, Volume, false); }
  material* CloneAndLoad(inputfile&) const;
  const std::string& GetClassId() const { return ClassId; }
  ushort GetIndex() const { return Index; }
  const materialdatabase* GetDataBase() const { return &DataBase; }
  const materialprototype* GetBase() const { return Base; }
  const std::map<ushort, materialdatabase>& GetConfig() const { return Config; }
 protected:
  ushort Index;
  materialdatabase DataBase;
  materialprototype* Base;
  std::map<ushort, materialdatabase> Config;
  material* (*Cloner)(ushort, ulong, bool);
  std::string ClassId;
};

class material
{
 public:
  typedef materialprototype prototype;
  typedef materialdatabase database;
  typedef std::map<ushort, materialdatabase> databasemap;
  material(ushort NewConfig, ulong InitVolume, bool Load = false) : MotherEntity(0) { Initialize(NewConfig, InitVolume, Load); }
  material(donothing) : MotherEntity(0) { }
  virtual ~material() { }
  void AddName(std::string&, bool = false, bool = true) const;
  std::string GetName(bool = false, bool = true) const;
  ulong GetVolume() const { return Volume; }
  ushort TakeDipVolumeAway();
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  void SetVolume(ulong);
  void Effect(character*, long);
  void EatEffect(character*, ulong, float = 1.0);
  void HitEffect(character*);
  virtual ushort GetSkinColor(ushort) const { return GetColor(); }
  ulong RawPrice() const { return 0; }
  bool CanBeDug(material* ShovelMaterial) const { return ShovelMaterial->GetStrengthValue() > GetStrengthValue(); }
  bool HasBe() const { return false; }
  virtual bool Be() { return true; }
  ushort GetType() const { return GetProtoType()->GetIndex(); }
  void AddConsumeEndMessage(character*) const;
  long CalculateOfferValue(char GodAlignment) const;
  DATABASEVALUE(ushort, StrengthValue);
  DATABASEVALUE(ushort, ConsumeType);
  DATABASEVALUE(ushort, Density);
  DATABASEVALUE(ushort, OfferValue);
  DATABASEVALUE(ushort, Color);
  DATABASEVALUE(ulong, PriceModifier);
  DATABASEBOOL(IsSolid);
  DATABASEVALUE(ushort, Emitation);
  DATABASEBOOL(CanBeWished);
  DATABASEVALUE(uchar, Alignment);
  DATABASEVALUE(ushort, NutritionValue);
  DATABASEBOOL(IsAlive);
  DATABASEBOOL(IsBadFoodForAI);
  DATABASEBOOL(IsFlammable);
  DATABASEBOOL(IsExplosive);
  DATABASEVALUE(const std::string&, NameStem);
  DATABASEVALUE(const std::string&, AdjectiveStem);
  DATABASEVALUE(const std::string&, Article);
  DATABASEVALUE(uchar, Effect);
  DATABASEVALUE(uchar, ConsumeEndMessage);
  DATABASEVALUE(uchar, HitMessage);
  DATABASEVALUE(ulong, ExplosivePower);
  DATABASEVALUE(uchar, Alpha);
  DATABASEVALUE(uchar, Flexibility);
  virtual const prototype* GetProtoType() const { return &material_ProtoType; }
  const database* GetDataBase() const { return DataBase; }
  material* Clone(ulong Volume) const { return GetProtoType()->Clone(Config, Volume); }
  ulong GetTotalExplosivePower() const { return ulong(float(Volume) * GetExplosivePower() / 1000000); }
  ushort GetConfig() const { return Config; }
  static material* MakeMaterial(ushort);
  static material* MakeMaterial(ushort, ulong);
  virtual bool IsFlesh() const { return false; }
  virtual bool IsLiquid() const { return false; }
  virtual std::string GetConsumeVerb() const { return "eating"; }
  ulong GetWeight() const { return Weight; }
  void CalculateWeight() { Weight = ulong(float(Volume) * GetDensity() / 1000); }
  entity* GetMotherEntity() const { return MotherEntity; }
  void SetMotherEntity(entity* What) { MotherEntity = What; }
  bool IsSameAs(material* What) const { return What->GetConfig() == GetConfig(); }
  void SetConfig(ushort);
  static material* Clone(ushort Config, ulong Volume, bool Load) { return new material(Config, Volume, Load); }
 protected:
  void Initialize(ushort, ulong, bool);
  void InstallDataBase();
  const database* DataBase;
  entity* MotherEntity;
  ulong Volume;
  ulong Weight;
  ushort Config;
  static prototype material_ProtoType;
};

#ifdef __FILE_OF_STATIC_MATERIAL_PROTOTYPE_DEFINITIONS__
#define MATERIAL_PROTOTYPE(name, baseproto) materialprototype name::name##_ProtoType(baseproto, &name::Clone, #name);
#else
#define MATERIAL_PROTOTYPE(name, baseproto)
#endif

#define MATERIAL(name, base, data)\
\
name : public base\
{\
 public:\
  name(ushort NewConfig, ulong InitVolume, bool Load = false) : base(donothing()) { Initialize(NewConfig, InitVolume, Load); }\
  name(donothing D) : base(D) { }\
  virtual const prototype* GetProtoType() const { return &name##_ProtoType; }\
  static material* Clone(ushort NewConfig, ulong Volume, bool Load) { return new name(NewConfig, Volume, Load); }\
 protected:\
  static prototype name##_ProtoType;\
  data\
}; MATERIAL_PROTOTYPE(name, &base##_ProtoType);

#endif

