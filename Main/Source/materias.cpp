/* Compiled through materset.cpp */

void organicsubstance::ResetSpoiling() { SpoilCounter = SpoilLevel = 0; }

const char* liquid::GetConsumeVerb() const { return "drinking"; }

bool powder::IsExplosive() const { return !Wetness && material::IsExplosive(); }

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
  SaveFile << SkinColor;
}

void flesh::Load(inputfile& SaveFile)
{
  organicsubstance::Load(SaveFile);
  SaveFile >> SkinColor;
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
