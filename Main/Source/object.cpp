#include "object.h"
#include "materia.h"
#include "festring.h"
#include "whandler.h"
#include "colorbit.h"
#include "proto.h"
#include "game.h"
#include "bitmap.h"
#include "save.h"

object::object() : entity(0), MainMaterial(0), AnimationFrames(1) { }
void object::SetMainMaterial(material* NewMaterial, ushort SpecialFlags) { SetMaterial(MainMaterial, NewMaterial, GetDefaultMainVolume(), SpecialFlags); }
void object::ChangeMainMaterial(material* NewMaterial, ushort SpecialFlags) { ChangeMaterial(MainMaterial, NewMaterial, GetDefaultMainVolume(), SpecialFlags); }
void object::SetConsumeMaterial(material* NewMaterial, ushort SpecialFlags) { SetMainMaterial(NewMaterial, SpecialFlags); }
void object::ChangeConsumeMaterial(material* NewMaterial, ushort SpecialFlags) { ChangeMainMaterial(NewMaterial, SpecialFlags); }
ushort object::GetSpecialFlags() const { return ST_NORMAL; }
ushort object::GetOutlineColor(ushort) const { return TRANSPARENT_COLOR; }
const std::vector<bitmap*>& object::GetPicture() const { return Picture; }

object::object(const object& Object) : entity(Object), id(Object), Config(Object.Config), VisualEffects(Object.VisualEffects)
{
  CopyMaterial(Object.MainMaterial, MainMaterial);
}

object::~object()
{
  for(ushort c = 0; c < GraphicId.size(); ++c)
    igraph::RemoveUser(GraphicId[c]);

  delete MainMaterial;
}

void object::CopyMaterial(material* const& Source, material*& Dest)
{
  if(Source)
    {
      Dest = Source->Duplicate();
      Dest->SetMotherEntity(this);
    }
  else
    Dest = 0;
}

void object::Save(outputfile& SaveFile) const
{
  SaveFile << GraphicId << Config << VisualEffects;
  SaveFile << MainMaterial;
}

void object::Load(inputfile& SaveFile)
{
  SaveFile >> GraphicId >> Config >> VisualEffects;
  LoadMaterial(SaveFile, MainMaterial);
  AnimationFrames = GraphicId.size();
  Picture.resize(AnimationFrames);

  for(ushort c = 0; c < AnimationFrames; ++c)
    Picture[c] = igraph::AddUser(GraphicId[c]);
}

void object::InitMaterials(material* FirstMaterial, bool CallUpdatePictures)
{
  InitMaterial(MainMaterial, FirstMaterial, GetDefaultMainVolume());
  SignalVolumeAndWeightChange();

  if(CallUpdatePictures)
    UpdatePictures();
}

void object::ObjectInitMaterials(material*& FirstMaterial, material* FirstNewMaterial, ulong FirstDefaultVolume, material*& SecondMaterial, material* SecondNewMaterial, ulong SecondDefaultVolume, bool CallUpdatePictures)
{
  InitMaterial(FirstMaterial, FirstNewMaterial, FirstDefaultVolume);
  InitMaterial(SecondMaterial, SecondNewMaterial, SecondDefaultVolume);
  SignalVolumeAndWeightChange();

  if(CallUpdatePictures)
    UpdatePictures();
}

void object::ObjectInitMaterials(material*& FirstMaterial, material* FirstNewMaterial, ulong FirstDefaultVolume, material*& SecondMaterial, material* SecondNewMaterial, ulong SecondDefaultVolume, material*& ThirdMaterial, material* ThirdNewMaterial, ulong ThirdDefaultVolume, bool CallUpdatePictures)
{
  InitMaterial(FirstMaterial, FirstNewMaterial, FirstDefaultVolume);
  InitMaterial(SecondMaterial, SecondNewMaterial, SecondDefaultVolume);
  InitMaterial(ThirdMaterial, ThirdNewMaterial, ThirdDefaultVolume);
  SignalVolumeAndWeightChange();

  if(CallUpdatePictures)
    UpdatePictures();
}

void object::InitMaterial(material*& Material, material* NewMaterial, ulong DefaultVolume)
{
  Material = NewMaterial;

  if(Material)
    {
      if(Material->HasBe())
	Enable();

      if(!Material->GetVolume())
	Material->SetVolume(DefaultVolume);

      Material->SetMotherEntity(this);
      SignalEmitationIncrease(Material->GetEmitation());
    }
}

void object::ChangeMaterial(material*& Material, material* NewMaterial, ulong DefaultVolume, ushort SpecialFlags)
{
  delete SetMaterial(Material, NewMaterial, DefaultVolume, SpecialFlags);
}

material* object::SetMaterial(material*& Material, material* NewMaterial, ulong DefaultVolume, ushort SpecialFlags)
{
  material* OldMaterial = Material;
  Material = NewMaterial;

  if((!OldMaterial || !OldMaterial->HasBe()) && NewMaterial && NewMaterial->HasBe())
    Enable();
  else if(OldMaterial && OldMaterial->HasBe() && (!NewMaterial || !NewMaterial->HasBe()) && !CalculateHasBe())
    Disable();

  if(NewMaterial)
    {
      if(!NewMaterial->GetVolume())
	{
	  if(OldMaterial)
	    NewMaterial->SetVolume(OldMaterial->GetVolume());
	  else if(DefaultVolume)
	    NewMaterial->SetVolume(DefaultVolume);
	  else
	    ABORT("Singularity spawn detected!");
	}

      NewMaterial->SetMotherEntity(this);

      if(!(SpecialFlags & NO_SIGNALS))
	SignalEmitationIncrease(NewMaterial->GetEmitation());
    }

  if(!(SpecialFlags & NO_SIGNALS))
    {
      if(OldMaterial)
	SignalEmitationDecrease(OldMaterial->GetEmitation());
  
      SignalVolumeAndWeightChange();
    }

  if(!(SpecialFlags & NO_PIC_UPDATE))
    UpdatePictures();

  return OldMaterial;
}

void object::UpdatePictures()
{
  AnimationFrames = UpdatePictures(GraphicId, Picture, vector2d(0, 0), (VisualEffects & 0x7)|GetSpecialFlags(), GetMaxAlpha(), GetGraphicsContainerIndex(), &object::GetBitmapPos);
}

ushort object::UpdatePictures(std::vector<graphicid>& GraphicId, std::vector<bitmap*>& Picture, vector2d Position, uchar SpecialFlags, uchar MaxAlpha, uchar GraphicsContainerIndex, vector2d (object::*BitmapPosRetriever)(ushort) const) const
{
  ushort AnimationFrames = GetClassAnimationFrames();
  vector2d SparklePos;
  uchar SparkleTime = 0;
  ushort Seed = 0;
  uchar FlyAmount = GetSpoilLevel(); 
  bool Sparkling = false;
  bool FrameNeeded = HasSpecialAnimation();

  if(!(SpecialFlags & (ST_FLAME|ST_LIGHTNING)))
    {
      bool MColorSparkling[4] = { false, false, false, false };

      for(ushort c = 0; c < 4; ++c)
	if(IsSparkling(c))
	  Sparkling = MColorSparkling[c] = true;

      if(Sparkling)
	{
	  static ushort SeedModifier = 1;
	  femath::SaveSeed();
	  vector2d BPos = (this->*BitmapPosRetriever)(0);
	  femath::SetSeed(BPos.X + BPos.Y + GetMaterialColorA(0) + SeedModifier);

	  if(++SeedModifier > 0x10)
	    SeedModifier = 1;

	  /* These vectors should be precalculated */

	  std::vector<vector2d> ValidVector;
	  ushort y, x;

	  if((SpecialFlags & 0x38) == ST_RIGHT_ARM)
	    {
	      for(y = 0; y < 16; ++y)
		for(x = 0; x < 8; ++x)
		  ValidVector.push_back(vector2d(x, y));
	    }
	  else if((SpecialFlags & 0x38) == ST_LEFT_ARM)
	    {
	      for(y = 0; y < 16; ++y)
		for(x = 8; x < 16; ++x)
		  ValidVector.push_back(vector2d(x, y));
	    }
	  else if((SpecialFlags & 0x38) == ST_GROIN)
	    {
	      for(y = 0; y < 10; ++y)
		for(x = 0; x < 16; ++x)
		  ValidVector.push_back(vector2d(x, y));

	      for(y = 10; y < 13; ++y)
		for(x = y - 5; x < 20 - y; ++x)
		  ValidVector.push_back(vector2d(x, y));
	    }
	  else if((SpecialFlags & 0x38) == ST_RIGHT_LEG)
	    {
	      /* Right leg from the character's, NOT the player's point of view */

	      for(y = 10; y < 16; ++y)
		for(x = 0; x < 8; ++x)
		  if((y != 10 || x < 5) && (y != 11 || x < 6) && (y != 12 || x < 7))
		    ValidVector.push_back(vector2d(x, y));
	    }
	  else if((SpecialFlags & 0x38) == ST_LEFT_LEG)
	    {
	      /* Left leg from the character's, NOT the player's point of view */

	      for(y = 10; y < 16; ++y)
		for(x = 8; x < 16; ++x)
		  if((y != 10 || x > 9) && (y != 11 || x > 8))
		    ValidVector.push_back(vector2d(x, y));
	    }
	  else
	    {
	      for(y = 0; y < 16; ++y)
		for(x = 0; x < 16; ++x)
		  ValidVector.push_back(vector2d(x, y));
	    }

	  SparklePos = igraph::GetRawGraphic(GraphicsContainerIndex)->RandomizeSparklePos(ValidVector, BPos, vector2d(16, 16), MColorSparkling);
	  SparkleTime = RAND() % 241;
	  femath::LoadSeed();

	  if(AnimationFrames <= 256)
	    AnimationFrames = 256;
	}

      if(FlyAmount)
	{
	  static ushort SeedModifier = 1;
	  vector2d BPos = (this->*BitmapPosRetriever)(0);
	  Seed = BPos.X + BPos.Y + GetMaterialColorA(0) + SeedModifier;

	  if(++SeedModifier > 0x10)
	    SeedModifier = 1;

	  if(AnimationFrames <= 32)
	    AnimationFrames = 32;

	  FrameNeeded = true;
	}
    }
  else if(SpecialFlags & ST_FLAME && AnimationFrames <= 16)
    {
      AnimationFrames = 16;
      FrameNeeded = true;
    }
  else if(SpecialFlags & ST_LIGHTNING)
    {
      static ushort SeedModifier = 1;
      vector2d BPos = (this->*BitmapPosRetriever)(0);
      Seed = BPos.X + BPos.Y + GetMaterialColorA(0) + SeedModifier + 0x42;

      if(++SeedModifier > 0x10)
	SeedModifier = 1;

      if(AnimationFrames <= 128)
	AnimationFrames = 128;
    }

  ushort c;

  for(c = 0; c < GraphicId.size(); ++c)
    igraph::RemoveUser(GraphicId[c]);

  GraphicId.resize(AnimationFrames);
  Picture.resize(AnimationFrames);

  for(c = 0; c < AnimationFrames; ++c)
    {
      graphicid& GI = GraphicId[c];
      GI.Color[0] = GetMaterialColorA(c);
      GI.Color[1] = GetMaterialColorB(c);
      GI.Color[2] = GetMaterialColorC(c);
      GI.Color[3] = GetMaterialColorD(c);
      GI.BaseAlpha = GetBaseAlpha(c);

      if(GI.BaseAlpha > MaxAlpha)
	GI.BaseAlpha = MaxAlpha;

      GI.Alpha[0] = GetAlphaA(c);

      if(GI.Alpha[0] > MaxAlpha)
	GI.Alpha[0] = MaxAlpha;

      GI.Alpha[1] = GetAlphaB(c);

      if(GI.Alpha[1] > MaxAlpha)
	GI.Alpha[1] = MaxAlpha;

      GI.Alpha[2] = GetAlphaC(c);

      if(GI.Alpha[2] > MaxAlpha)
	GI.Alpha[2] = MaxAlpha;

      GI.Alpha[3] = GetAlphaD(c);

      if(GI.Alpha[3] > MaxAlpha)
	GI.Alpha[3] = MaxAlpha;

      GI.BitmapPos = (this->*BitmapPosRetriever)(c);
      GI.FileIndex = GraphicsContainerIndex;
      GI.SpecialFlags = SpecialFlags;

      bool SparkleInfoNeeded = Sparkling && c > SparkleTime && c < SparkleTime + 16;

      if(SparkleInfoNeeded)
	{
	  GI.SparklePos = SparklePos;
	  GI.SparkleFrame = c - SparkleTime;
	}
      else
	{
	  GI.SparklePos = ERROR_VECTOR;
	  GI.SparkleFrame = 0;
	}

      GI.Frame = !c || FrameNeeded || (SpecialFlags & ST_LIGHTNING && !((c + 1) & 7)) ? c : 0;
      GI.OutlineColor = GetOutlineColor(c);
      GI.OutlineAlpha = GetOutlineAlpha(c);
      GI.Seed = Seed;
      GI.FlyAmount = FlyAmount;
      GI.Position = Position;
      Picture[c] = igraph::AddUser(GI);
    }

  return AnimationFrames;
}

ushort object::GetMaterialColorA(ushort) const
{
  if(GetMainMaterial())
    return GetMainMaterial()->GetColor();
  else
    return 0;
}

bool object::AddMaterialDescription(festring& String, bool Articled) const
{
  GetMainMaterial()->AddName(String, Articled);
  String << ' ';
  return true;
}

void object::AddContainerPostFix(festring& String) const
{
  if(GetContainedMaterial())
    GetContainedMaterial()->AddName(String << " full of ");
}

void object::AddLumpyPostFix(festring& String) const
{
  if(GetMainMaterial())
    GetMainMaterial()->AddName(String << " of ");
}

void object::SetSecondaryMaterial(material*, ushort)
{
  ABORT("Illegal object::SetSecondaryMaterial call!");
}

void object::ChangeSecondaryMaterial(material*, ushort)
{
  ABORT("Illegal object::ChangeSecondaryMaterial call!");
}

void object::SetContainedMaterial(material*, ushort)
{
  ABORT("Illegal object::SetContainedMaterial call!");
}

void object::ChangeContainedMaterial(material*, ushort)
{
  ABORT("Illegal object::ChangeContainedMaterial call!");
}

uchar object::GetAlphaA(ushort) const
{
  if(GetMainMaterial())
    return GetMainMaterial()->GetAlpha();
  else
    return 255;
}

void object::RandomizeVisualEffects()
{
  uchar AcceptedFlags = GetOKVisualEffects();

  if(AcceptedFlags)
    SetVisualEffects(RAND() & 0x7 & AcceptedFlags | GetForcedVisualEffects());
  else
    SetVisualEffects(GetForcedVisualEffects());
}

void object::LoadMaterial(inputfile& SaveFile, material*& Material)
{
  SaveFile >> Material;

  if(Material)
    {
      if(Material->HasBe())
	Enable();

      Material->SetMotherEntity(this);
      game::CombineLights(Emitation, Material->GetEmitation());
    }
}

void object::Draw(bitmap* Bitmap, vector2d Pos, ulong Luminance, bool AllowAnimate) const
{
  Picture[!AllowAnimate || AnimationFrames == 1 ? 0 : globalwindowhandler::GetTick() % AnimationFrames]->AlphaBlit(Bitmap, 0, 0, Pos, 16, 16, Luminance);
}

void object::SolidDraw(bitmap* Bitmap, vector2d Pos, ulong Luminance, bool AllowAnimate) const
{
  Picture[!AllowAnimate || AnimationFrames == 1 ? 0 : globalwindowhandler::GetTick() % AnimationFrames]->MaskedBlit(Bitmap, 0, 0, Pos, 16, 16, Luminance);
}

void object::Draw(bitmap* Bitmap, vector2d Pos, ulong Luminance, bool AllowAnimate, bool AllowAlpha) const
{
  if(AllowAlpha)
    Picture[!AllowAnimate || AnimationFrames == 1 ? 0 : globalwindowhandler::GetTick() % AnimationFrames]->AlphaBlit(Bitmap, 0, 0, Pos, 16, 16, Luminance);
  else
    Picture[!AllowAnimate || AnimationFrames == 1 ? 0 : globalwindowhandler::GetTick() % AnimationFrames]->MaskedBlit(Bitmap, 0, 0, Pos, 16, 16, Luminance);
}

god* object::GetMasterGod() const
{
  return game::GetGod(GetConfig());
}

ushort object::RandomizeMaterialConfiguration()
{
  return GetMaterialConfigChances().size() > 1 ? femath::WeightedRand(GetMaterialConfigChances()) : 0;
}

void object::GenerateMaterials()
{
  InitChosenMaterial(MainMaterial, GetMainMaterialConfig(), GetDefaultMainVolume(), RandomizeMaterialConfiguration());
}

void object::InitChosenMaterial(material*& Material, const std::vector<long>& MaterialConfig, ulong DefaultVolume, ushort Chosen)
{
  if(MaterialConfig.size() == 1)
    InitMaterial(Material, MAKE_MATERIAL(MaterialConfig[0]), DefaultVolume);
  else if(MaterialConfig.size() == GetMaterialConfigChances().size())
    InitMaterial(Material, MAKE_MATERIAL(MaterialConfig[Chosen]), DefaultVolume);
  else
    ABORT("MaterialConfig array of illegal size detected!");
}

void object::SetConfig(ushort NewConfig)
{
  Config = NewConfig;
  InstallDataBase();
  CalculateAll();
  UpdatePictures();
}

bool object::AddEmptyAdjective(festring& String, bool Articled) const
{
  if(GetContainedMaterial())
    return false;
  else
    {
      String << (Articled ? "an empty " : "empty ");
      return true;
    }
}

void object::CalculateEmitation()
{
  Emitation = GetBaseEmitation();

  for(ushort c = 0; c < GetMaterials(); ++c)
    if(GetMaterial(c))
      game::CombineLights(Emitation, GetMaterial(c)->GetEmitation());
}

bool object::CalculateHasBe() const
{
  for(ushort c = 0; c < GetMaterials(); ++c)
    if(GetMaterial(c) && GetMaterial(c)->HasBe())
      return true;

  return false;
}

bool object::IsSparkling(ushort ColorIndex) const
{
  return !ColorIndex && MainMaterial->IsSparkling();
}
