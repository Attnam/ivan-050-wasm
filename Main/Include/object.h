#ifndef __OBJECT_H__
#define __OBJECT_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include <vector>

#include "typedef.h"
#include "vector2d.h"
#include "igraph.h"
#include "entity.h"
#include "id.h"
#include "bitmap.h"

class material;
class outputfile;
class inputfile;
class bitmap;
class lsquare;
class square;
class god;

class object : public entity, public id
{
 public:
  object() : entity(false), MainMaterial(0), AnimationFrames(1), Emitation(0) { }
  virtual ~object();
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  void InitMaterials(material*, bool = true);
  void UpdatePictures();
  virtual material* GetMainMaterial() const { return MainMaterial; }
  virtual material* GetSecondaryMaterial() const { return 0; }
  virtual material* GetContainedMaterial() const { return 0; }
  virtual material* GetConsumeMaterial() const { return MainMaterial; }
  virtual void SetMainMaterial(material* NewMaterial) { SetMaterial(MainMaterial, NewMaterial, GetDefaultMainVolume()); }
  virtual void ChangeMainMaterial(material* NewMaterial) { ChangeMaterial(MainMaterial, NewMaterial, GetDefaultMainVolume()); }
  virtual void SetConsumeMaterial(material* NewMaterial) { SetMainMaterial(NewMaterial); }
  virtual void ChangeConsumeMaterial(material* NewMaterial) { ChangeMainMaterial(NewMaterial); }
  virtual void SetSecondaryMaterial(material*);
  virtual void ChangeSecondaryMaterial(material*);
  virtual void SetContainedMaterial(material*);
  virtual void ChangeContainedMaterial(material*);
  virtual uchar GetMaterials() const { return 1; }
  virtual material* GetMaterial(ushort Index) const { return Index ? 0 : MainMaterial; }
  const std::vector<bitmap*>& GetPicture() const { return Picture; }
  bitmap* GetPicture(ushort Index) const { return Picture[Index]; }
  virtual ushort GetBaseEmitation() const { return 0; }
  virtual void SetParameters(uchar) { }
  virtual uchar GetOKVisualEffects() const { return 0; }
  virtual uchar GetVisualEffects() const { return VisualEffects; }
  virtual void SetVisualEffects(uchar What) { VisualEffects = What; }
  virtual uchar GetForcedVisualEffects() const { return 0; }
  void SetAnimationFrames(ushort What) { AnimationFrames = What; }
  bool IsAnimated() const { return AnimationFrames > 1; }
  virtual void CalculateEmitation();
  virtual ushort GetEmitation() const { return Emitation; }
  void SetEmitation(ushort What) { Emitation = What; }
  void LoadMaterial(inputfile&, material*&);
  ushort GetConfig() const { return Config; }
  virtual void Draw(bitmap*, vector2d, ushort, bool, bool) const;
  virtual god* GetMasterGod() const;
  virtual const std::vector<long>& GetMainMaterialConfig() const = 0;
  virtual const std::vector<long>& GetMaterialConfigChances() const = 0;
  void SetConfig(ushort);
 protected:
  void ObjectInitMaterials(material*&, material*, ulong, material*&, material*, ulong, bool);
  void ObjectInitMaterials(material*&, material*, ulong, material*&, material*, ulong, material*&, material*, ulong, bool);
  virtual ulong GetDefaultMainVolume() const { return 0; }
  virtual ulong GetDefaultSecondaryVolume() const { return 0; }
  virtual ulong GetDefaultContainedVolume() const { return 0; }
  void InitMaterial(material*&, material*, ulong);
  material* SetMaterial(material*&, material*, ulong);
  void ChangeMaterial(material*&, material*, ulong);
  bool CalculateHasBe() const;
  virtual uchar GetSpecialFlags(ushort) const { return STNORMAL; }
  virtual uchar GetGraphicsContainerIndex(ushort) const = 0;
  virtual ushort GetMaterialColorA(ushort) const;
  virtual ushort GetMaterialColorB(ushort) const { return 0; }
  virtual ushort GetMaterialColorC(ushort) const { return 0; }
  virtual ushort GetMaterialColorD(ushort) const { return 0; }
  virtual uchar GetMaxAlpha(ushort) const { return 255; }
  virtual uchar GetBaseAlpha(ushort) const { return 255; }
  virtual uchar GetAlphaA(ushort) const;
  virtual uchar GetAlphaB(ushort) const { return 255; }
  virtual uchar GetAlphaC(ushort) const { return 255; }
  virtual uchar GetAlphaD(ushort) const { return 255; }
  virtual std::string GetMaterialDescription(bool) const;
  virtual ushort RandomizeMaterialConfiguration();
  virtual void GenerateMaterials();
  virtual void InitChosenMaterial(material*&, const std::vector<long>&, ulong, ushort);
  virtual void InstallDataBase() = 0;
  std::string ContainerPostFix() const;
  std::string LumpyPostFix() const;
  virtual vector2d GetBitmapPos(ushort) const = 0;
  void RandomizeVisualEffects();
  material* MainMaterial;
  std::vector<graphic_id> GraphicId;
  std::vector<bitmap*> Picture;
  ushort Config;
  uchar VisualEffects;
  ushort AnimationFrames;
  ushort Emitation;
};

#endif

