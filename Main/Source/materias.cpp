/* Compiled through materset.cpp */

void organicsubstance::ResetSpoiling() { SpoilCounter = SpoilLevel = 0; }

const char* liquid::GetConsumeVerb() const { return "drinking"; }

bool powder::IsExplosive() const { return !Wetness && material::IsExplosive(); }

bool ironalloy::IsSparkling() const { return material::IsSparkling() && GetRustLevel() == NOT_RUSTED; }

void organicsubstance::Be()
{
  if(MotherEntity->AllowSpoil())
    {
      if(++SpoilCounter < GetSpoilModifier())
	{
	  if(SpoilCounter << 1 >= GetSpoilModifier())
	    {
	      uchar NewSpoilLevel = ((SpoilCounter << 4) / GetSpoilModifier()) - 7;

	      if(NewSpoilLevel != SpoilLevel)
		{
		  SpoilLevel = NewSpoilLevel;
		  MotherEntity->SignalSpoilLevelChange(this);
		}
	    }
	}
      else
	MotherEntity->SignalSpoil(this);
    }
}

void organicsubstance::Save(outputfile& SaveFile) const
{
  material::Save(SaveFile);
  SaveFile << SpoilCounter << SpoilLevel;
}

void organicsubstance::Load(inputfile& SaveFile)
{
  material::Load(SaveFile);
  SaveFile >> SpoilCounter >> SpoilLevel;
}

void organicsubstance::VirtualConstructor(bool Load)
{
  if(!Load)
    {
      SpoilCounter = (RAND() % GetSpoilModifier()) >> 5;
      SpoilLevel = 0;
    }
}

void flesh::Save(outputfile& SaveFile) const
{
  organicsubstance::Save(SaveFile);
  SaveFile << SkinColor << SkinColorSparkling;
}

void flesh::Load(inputfile& SaveFile)
{
  organicsubstance::Load(SaveFile);
  SaveFile >> SkinColor >> SkinColorSparkling;
}

void powder::Be()
{
  if(Wetness > 0)
    --Wetness;
}

void powder::Save(outputfile& SaveFile) const
{
  material::Save(SaveFile);
  SaveFile << Wetness;
}

void powder::Load(inputfile& SaveFile)
{
  material::Load(SaveFile);
  SaveFile >> Wetness;
}

void organicsubstance::EatEffect(character* Eater, ulong Amount)
{
  Amount = Volume > Amount ? Amount : Volume;
  Effect(Eater, Amount);
  Eater->ReceiveNutrition(GetNutritionValue() * Amount * 15 / (1000 * (GetSpoilLevel() + 1)));

  if(GetSpoilLevel() > 0)
    {
      Eater->BeginTemporaryState(CONFUSED, ushort(Amount * GetSpoilLevel() * sqrt(GetNutritionValue()) / 1000));

      if(CanHaveParasite() && !(RAND() % (1000 / GetSpoilLevel())))
	Eater->GainIntrinsic(PARASITIZED);
    }

  if(GetSpoilLevel() > 4)
    Eater->BeginTemporaryState(POISONED, ushort(Amount * (GetSpoilLevel() - 4) * sqrt(GetNutritionValue()) / 1000));

  SetVolume(Volume - Amount);
}

void organicsubstance::AddConsumeEndMessage(character* Eater) const
{
  if(Eater->IsPlayer())
    {
      if(GetSpoilLevel() > 0 && GetSpoilLevel() <= 4)
	ADD_MESSAGE("Ugh. This stuff was slightly spoiled.");
      else if(GetSpoilLevel() > 4)
	ADD_MESSAGE("Ugh. This stuff was terribly spoiled!");
    }

  material::AddConsumeEndMessage(Eater);
}

void organicsubstance::SetSpoilCounter(ushort What)
{
  SpoilCounter = What;

  if(SpoilCounter < GetSpoilModifier())
    {
      if(SpoilCounter << 1 >= GetSpoilModifier())
	{
	  uchar NewSpoilLevel = ((SpoilCounter << 4) / GetSpoilModifier()) - 7;

	  if(NewSpoilLevel != SpoilLevel)
	    {
	      SpoilLevel = NewSpoilLevel;
	      MotherEntity->SignalSpoilLevelChange(this);
	    }
	}
    }
  else
    MotherEntity->SignalSpoil(this);
}

void ironalloy::SetRustLevel(uchar What)
{
  if(GetRustLevel() != What)
    {
      if(!RustData)
	RustData = RAND() & 0xFC | What;
      else if(!What)
	RustData = 0;
      else
	RustData = RustData & 0xFC | What;

      if(MotherEntity)
	MotherEntity->SignalRustLevelChange();
    }
}

ushort ironalloy::GetStrengthValue() const
{
  ushort Base = material::GetStrengthValue();

  switch(GetRustLevel())
    {
    case NOT_RUSTED: return Base;
    case SLIGHTLY_RUSTED: return ((Base << 3) + Base) / 10;
    case RUSTED: return ((Base << 1) + Base) >> 2;
    case VERY_RUSTED: return Base >> 1;
    }

  return 0; /* not possible */
}

void ironalloy::AddName(festring& Name, bool Articled, bool Adjective) const
{
  if(Articled)
    if(GetRustLevel() == NOT_RUSTED)
      Name << GetArticle() << ' ';
    else
      Name << "a ";

  switch(GetRustLevel())
    {
    case SLIGHTLY_RUSTED:
      Name << "sligthly rusted ";
      break;
    case RUSTED:
      Name << "rusted ";
      break;
    case VERY_RUSTED:
      Name << "very rusted ";
      break;
    }

  Name << (Adjective ? GetAdjectiveStem() : GetNameStem());
}

void ironalloy::Save(outputfile& SaveFile) const
{
  material::Save(SaveFile);
  SaveFile << RustData;
}

void ironalloy::Load(inputfile& SaveFile)
{
  material::Load(SaveFile);
  SaveFile >> RustData;
}

void liquid::SpillEffect(item* Item)
{
}

void liquid::SpillEffect(character* Char, ushort BodyPartIndex)
{  
}

void liquid::ConstantEffect(item* Item)
{
  if(GetRustModifier())
    Item->TryToRust(GetRustModifier() * GetVolume());

  if(GetAcidicity())
    Item->ReceiveAcid(this, Volume * GetAcidicity());
}

void liquid::ConstantEffect(lterrain* Terrain)
{
  if(GetRustModifier())
    Terrain->TryToRust(GetRustModifier() * GetVolume());

  if(GetAcidicity())
    Terrain->ReceiveAcid(this, Volume * GetAcidicity());
}

void liquid::ConstantEffect(character* Char, ushort BodyPartIndex)
{
  if(GetAcidicity())
    Char->GetBodyPart(BodyPartIndex)->ReceiveAcid(this, Volume * GetAcidicity() >> 1);
}

/* Doesn't do the actual rusting */

bool ironalloy::TryToRust(ulong Modifier)
{
  if(GetRustLevel() != VERY_RUSTED)
    {
      ulong Chance = 5000000. * sqrt(GetVolume()) / (Modifier * GetRustModifier());

      if(Chance <= 1 || !(RAND() % Chance))
	return true;
    }

  return false;
}
