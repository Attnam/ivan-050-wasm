/*
 *
 *  Iter Vehemens ad Necem 
 *  Copyright (C) Timo Kiviluoto
 *  Released under GNU General Public License
 *
 *  See LICENSING which should included with 
 *  this file for more details
 *
 */

/* Compiled through trapset.cpp */

void web::VirtualConstructor(bool)
{
  TrapData.TrapID = game::CreateNewTrapID(this);
  TrapData.VictimID = 0;
}

bool web::TryToUnStick(character* Victim, vector2d)
{
  ulong TrapID = GetTrapID();
  int Modifier = GetTrapBaseModifier() * (Max(Victim->GetAttribute(DEXTERITY), 1) + Max(Victim->GetAttribute(ARM_STRENGTH),1));

  if(Modifier <= 1 || !RAND_N(Modifier))
    {
      Victim->RemoveTrap(TrapID);
      TrapData.VictimID = 0;

      if(Victim->IsPlayer())
	ADD_MESSAGE("You manage to free yourself from the web.");
      else if(Victim->CanBeSeenByPlayer())
	ADD_MESSAGE("%s manages to free %sself from the web.", Victim->CHAR_NAME(DEFINITE), Victim->CHAR_OBJECT_PRONOUN);

      Victim->EditAP(-500);
      return true;
    }

  if(!Modifier || !RAND_N(Modifier << 1))
    {
      Victim->RemoveTrap(TrapID);
      TrapData.VictimID = 0;
      //      GetLSquareUnder()->RemoveTrap(this);

      if(Victim->IsPlayer())
	ADD_MESSAGE("You are tear the web down.");
      else if(Victim->CanBeSeenByPlayer())
	ADD_MESSAGE("%s tears the web down.", Victim->CHAR_NAME(DEFINITE));

      Victim->EditAP(-500);
      return true;
    }

  Modifier = GetTrapBaseModifier() * Max(Victim->GetAttribute(DEXTERITY) * 3 / 10, 2);

  if(Victim->CanChoke() && !RAND_N(Modifier << 2))
    {
      Victim->LoseConsciousness(250 + RAND_N(250));      
      if(Victim->IsPlayer())
	ADD_MESSAGE("You manage to choke yourself on the web.");
      else if(Victim->CanBeSeenByPlayer())
	ADD_MESSAGE("%s chokes %sself on the web.", Victim->CHAR_NAME(DEFINITE),Victim->CHAR_OBJECT_PRONOUN);
    }

  if(!RAND_N(Modifier << 1))
    {
      int VictimBodyPart = Victim->RandomizeTryToUnStickBodyPart(ALL_BODYPART_FLAGS&~TrapData.BodyParts);

      if(VictimBodyPart != NONE_INDEX)
	{
	  TrapData.BodyParts |= 1 << VictimBodyPart;
	  Victim->AddTrap(GetTrapID(), 1 << VictimBodyPart);

	  if(Victim->IsPlayer())
	    ADD_MESSAGE("You fail to free yourself from the web and your %s is stuck to it in the attempt.", Victim->GetBodyPartName(VictimBodyPart).CStr());
	  else if(Victim->CanBeSeenByPlayer())
	    ADD_MESSAGE("%s tries to free %sself from the web but is stuck more tightly to it in the attempt.", Victim->CHAR_NAME(DEFINITE), Victim->CHAR_OBJECT_PRONOUN);

	  Victim->EditAP(-1000);
	  return true;
	}
    }

  if(Victim->IsPlayer())
    ADD_MESSAGE("You are unable to escape from the web.");

  Victim->EditAP(-1000);
  return false;
}

void web::Save(outputfile& SaveFile) const
{
  trap::Save(SaveFile);
  SaveFile << Strength;
}

void web::Load(inputfile& SaveFile)
{
  trap::Load(SaveFile);
  SaveFile >> Strength;
}

void web::StepOnEffect(character* Stepper)
{
  int StepperBodyPart = Stepper->GetRandomStepperBodyPart();

  if(StepperBodyPart == NONE_INDEX)
    return;

  TrapData.VictimID = Stepper->GetID();
  TrapData.BodyParts = 1 << StepperBodyPart;
  Stepper->AddTrap(GetTrapID(), 1 << StepperBodyPart);

  if(Stepper->IsPlayer())
    ADD_MESSAGE("You try to step through the web but your %s sticks to it.", Stepper->GetBodyPartName(StepperBodyPart).CStr());
  else if(Stepper->CanBeSeenByPlayer())
    ADD_MESSAGE("%s gets stuck to the web.", Stepper->CHAR_NAME(DEFINITE));

  if(Stepper->IsPlayer())
    game::AskForKeyPress(CONST_S("Trap activated! [press any key to continue]"));

}
