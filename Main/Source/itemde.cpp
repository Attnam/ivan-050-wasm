#define __FILE_OF_STATIC_ITEM_PROTOTYPE_DEFINITIONS__

#include "proto.h"
#include "itemba.h"

std::vector<item::prototype*> protocontainer<item>::ProtoData;
valuemap protocontainer<item>::CodeNameMap;

ITEM_PROTOTYPE(item, 0);

#include "femath.h"
#include "itemde.h"

#undef __FILE_OF_STATIC_ITEM_PROTOTYPE_DEFINITIONS__

#include <cmath>

#include "message.h"
#include "stack.h"
#include "charde.h"
#include "level.h"
#include "lsquare.h"
#include "lterraba.h"
#include "config.h"
#include "godba.h"
#include "festring.h"
#include "whandler.h"
#include "lterrade.h"
#include "actionba.h"
#include "felist.h"
#include "save.h"
#include "team.h"
#include "error.h"

void banana::GenerateLeftOvers(character* Eater)
{
  item* Peel = new bananapeels(0, NOMATERIALS);
  Peel->InitMaterials(GetMainMaterial());

  if(!game::IsInWilderness() && (!Eater->IsPlayer() || configuration::GetAutoDropLeftOvers()))
    Eater->GetStackUnder()->AddItem(Peel);
  else
    Eater->GetStack()->AddItem(Peel);

  RemoveFromSlot();
  SetMainMaterial(0, NOPICUPDATE);
  SendToHell();
}

void potion::GenerateLeftOvers(character* Eater)
{
  ChangeConsumeMaterial(0);

  if(!game::IsInWilderness() && (!Eater->IsPlayer() || configuration::GetAutoDropLeftOvers()))
    MoveTo(Eater->GetStackUnder());
  else
    MoveTo(Eater->GetStack());
}

bool scroll::CanBeRead(character* Reader) const
{
  return Reader->CanRead() || game::GetSeeWholeMapCheat();
}

bool scrollofcreatemonster::Read(character* Reader)
{
  Reader->StartReading(this, 200);
  return true;
}

void scrollofcreatemonster::FinishReading(character* Reader)
{
  vector2d TryToCreate;

  for(ushort c = 0; c < 100; ++c)
    {
      TryToCreate = Reader->GetPos() + game::GetMoveVector(RAND() % DIRECTION_COMMAND_KEYS);
      character* Monster = protosystem::CreateMonster();

      if(GetAreaUnder()->IsValidPos(TryToCreate) && GetNearSquare(TryToCreate)->IsWalkable(Monster) && GetNearSquare(TryToCreate)->GetCharacter() == 0)
	{
	  GetNearLSquare(TryToCreate)->AddCharacter(Monster);

	  if(Reader->IsPlayer())
	    {
	      if(Monster->CanBeSeenByPlayer())
		ADD_MESSAGE("%s appears.", Monster->CHARNAME(INDEFINITE));
	      else
		ADD_MESSAGE("You notice no effect.");
	    }
	  else if(Reader->CanBeSeenByPlayer())
	    {
	      if(Monster->CanBeSeenByPlayer())
		ADD_MESSAGE("%s summons %s!", Reader->CHARNAME(DEFINITE), Monster->CHARNAME(INDEFINITE));
	    }
	  else if(Monster->CanBeSeenByPlayer())
	    ADD_MESSAGE("Suddenly %s appears.", Monster->CHARNAME(INDEFINITE));

	  return;
	}
      else
	delete Monster;
    }

  ADD_MESSAGE("You feel a lost soul fly by you.");
  RemoveFromSlot();
  SendToHell();
}

bool scrollofteleportation::Read(character* Reader)
{
  Reader->StartReading(this, 200);
  return true;
}

void scrollofteleportation::FinishReading(character* Reader)
{
  if(Reader->IsPlayer())
    ADD_MESSAGE("Suddenly you realize you have teleported.");
  else if(Reader->CanBeSeenByPlayer())
    ADD_MESSAGE("%s disappears!", Reader->CHARNAME(DEFINITE));

  Reader->TeleportRandomly();
  RemoveFromSlot();
  SendToHell();
}

bool lump::HitEffect(character* Enemy, character*, uchar, uchar, bool BlockedByArmour)
{
  if(!BlockedByArmour && RAND() & 1)
    {
      if(Enemy->IsPlayer() || Enemy->CanBeSeenByPlayer())
	ADD_MESSAGE("The %s touches %s!", GetMainMaterial()->GetName(false, false).c_str(), Enemy->CHARDESCRIPTION(DEFINITE));

      bool Success = GetMainMaterial()->HitEffect(Enemy);

      if(!GetMainMaterial()->GetVolume())
	{
	  RemoveFromSlot();
	  SendToHell();
	}

      return Success;
    }
  else
    return false;
}

bool meleeweapon::HitEffect(character* Enemy, character*, uchar, uchar, bool BlockedByArmour)
{
  if(!BlockedByArmour && GetContainedMaterial())
    {
      if(Enemy->IsPlayer() || Enemy->CanBeSeenByPlayer())
	ADD_MESSAGE("The %s reacts with %s!", GetContainedMaterial()->GetName(false, false).c_str(), Enemy->CHARDESCRIPTION(DEFINITE));

      bool Success = GetContainedMaterial()->HitEffect(Enemy);

      if(!GetContainedMaterial()->GetVolume())
	Empty();

      return Success;
    }
  else
    return false;
}

void meleeweapon::DipInto(material* Material, character* Dipper)
{
  if(Dipper->IsPlayer())
    ADD_MESSAGE("%s is now covered with %s.", CHARNAME(DEFINITE), Material->GetName(false, false).c_str());

  ChangeContainedMaterial(Material);
  Dipper->DexterityAction(10);
}

material* lump::CreateDipMaterial()
{
  material* Material = GetMainMaterial()->Clone(GetMainMaterial()->TakeDipVolumeAway());

  if(!GetMainMaterial()->GetVolume())
    {
      RemoveFromSlot();
      SendToHell();
    }

  return Material;
}

bool pickaxe::Apply(character* User)
{
  if(IsBroken())
    {
      ADD_MESSAGE("%s is totally broken.",CHARNAME(DEFINITE));
      return false;
    }
  uchar Dir = game::DirectionQuestion("What direction do you want to dig? [press a direction key]", false);

  vector2d Temp = game::GetMoveVector(Dir);

  if(Dir == DIR_ERROR || !GetAreaUnder()->IsValidPos(User->GetPos() + Temp))
    return false;

  lsquare* Square = GetNearLSquare(User->GetPos() + Temp);

  if(Square->GetCharacter())
    if(User->Hit(Square->GetCharacter()))
      return true;

  if(Square->CanBeDug())
    if(Square->GetOLTerrain()->CanBeDug())
      if(Square->GetOLTerrain()->GetMainMaterial()->CanBeDug(GetMainMaterial()))
	{
	  uchar RoomNumber = Square->GetRoom();

	  if(!RoomNumber || Square->GetLevelUnder()->GetRoom(RoomNumber)->DestroyTerrain(User, Square->GetOLTerrain()))
	    {
	      User->SwitchToDig(this, User->GetPos() + Temp);
	      User->DexterityAction(5);
	      return true;
	    }
	  else
	    return false;
	}
      else
	ADD_MESSAGE("%s is too hard to dig with %s.", Square->GetOLTerrain()->CHARNAME(DEFINITE), CHARNAME(INDEFINITE));
    else
      ADD_MESSAGE(Square->GetOLTerrain()->GetDigMessage().c_str());

  return false;
}

bool wand::Apply(character* Terrorist)
{
  if(Terrorist->IsPlayer())
    ADD_MESSAGE("%s breaks in two and then explodes!", CHARNAME(DEFINITE));
  else if(Terrorist->CanBeSeenByPlayer())
    ADD_MESSAGE("%s breaks %s in two. It explodes!", Terrorist->CHARNAME(DEFINITE), CHARNAME(INDEFINITE));
  else if(GetSquareUnder()->CanBeSeenByPlayer())
    ADD_MESSAGE("Something explodes!");

  RemoveFromSlot();
  SendToHell();
  std::string DeathMsg;

  if(Terrorist->IsPlayer())
    DeathMsg = "exploded himself by breaking a wand";
  else
    DeathMsg = "kamikazed by " + Terrorist->GetName(INDEFINITE);

  Terrorist->GetLevelUnder()->Explosion(Terrorist, DeathMsg, Terrorist->GetLSquareUnder()->GetPos(), 40);
  Terrorist->DexterityAction(5);
  return true;
}

void wand::Save(outputfile& SaveFile) const
{
  item::Save(SaveFile);
  SaveFile << TimesUsed << Charges;
}

void wand::Load(inputfile& SaveFile)
{
  item::Load(SaveFile);
  SaveFile >> TimesUsed >> Charges;
}

bool scrollofwishing::Read(character* Reader)
{
  Reader->StartReading(this, 2500);
  return true;
}

void scrollofwishing::FinishReading(character* Reader)
{
  while(true)
    {
      std::string Temp = game::StringQuestion("What do you want to wish for?", vector2d(16, 6), WHITE, 0, 80, false);
      item* TempItem = protosystem::CreateItem(Temp, Reader->IsPlayer());

      if(TempItem)
	{
	  Reader->GetStack()->AddItem(TempItem);
	  ADD_MESSAGE("%s appears from nothing and the scroll burns!", TempItem->CHARNAME(INDEFINITE));
	  RemoveFromSlot();
	  SendToHell();
	  return;
	}
    }
}

bool scrollofchangematerial::Read(character* Reader)
{
  Reader->StartReading(this, 1000);
  return true;
}

void scrollofchangematerial::FinishReading(character* Reader)
{
  if(!Reader->GetStack()->GetItems())
    ADD_MESSAGE("You notice you have lost anything you wished to alter.");
  else
    {
      while(true)
	{
	  item* Item = Reader->GetStack()->DrawContents(Reader, "What item do you wish to change?");

	  if(Item)
	    {
	      if(!Item->IsMaterialChangeable())
		{
		  ADD_MESSAGE("You cast the spell, but the magic is not powerful enough to affect %s!", Item->CHARNAME(DEFINITE));
		  break;
		}

	      std::string Temp = game::StringQuestion("What material do you want to wish for?", vector2d(16, 6), WHITE, 0, 80, false);
	      material* TempMaterial = protosystem::CreateMaterial(Temp, Item->GetMainMaterial()->GetVolume());

	      if(TempMaterial)
		{
		  if(!Item->GetMainMaterial()->IsSameAs(TempMaterial))
		    {
		      ADD_MESSAGE("Suddenly your %s is consumed in roaring magical flames. As you lift it again it looks greatly altered.", Item->CHARNAME(UNARTICLED));
		      Item->ChangeMainMaterial(TempMaterial);
		    }
		  else
		    ADD_MESSAGE("Suddenly your %s is consumed in roaring magical flames. As you lift it again it looks unchanged.", Item->CHARNAME(UNARTICLED));

		  break;
		}
	      else
		game::DrawEverythingNoBlit();
	    }
	  else if(game::BoolQuestion("Really cancel read? [y/N]"))
	    return;
	}

      RemoveFromSlot();
      SendToHell();
    }
}

item* brokenbottle::BetterVersion() const
{
  material* Stuff;

  if(RAND() % 5)
    Stuff = MAKE_MATERIAL(BANANAFLESH);
  else
    Stuff = MAKE_MATERIAL(OMLEURINE);

  potion* P = new potion(0, NOMATERIALS); 
  P->InitMaterials(MAKE_MATERIAL(GLASS), Stuff); 
  return P;
}

void brokenbottle::StepOnEffect(character* Stepper)
{
  if(!(RAND() % 10))
    {
      if(Stepper->ReceiveDamage(0, 1 + (RAND() & 1), PHYSICALDAMAGE, LEGS, YOURSELF, false, false, false, false))
	{
	  if(Stepper->IsPlayer())
	    ADD_MESSAGE("Auch. You step on sharp glass splinters.");
	  else if(Stepper->CanBeSeenByPlayer())
	    ADD_MESSAGE("%s steps on sharp glass splinters and is hurt.", Stepper->CHARNAME(DEFINITE));
	}
      else
	{
	  if(Stepper->IsPlayer())
	    ADD_MESSAGE("Some glass splinters are shattered under your feet.");
	  else if(Stepper->CanBeSeenByPlayer())
	    ADD_MESSAGE("Some glass splinters are shattered under %s's feet.", Stepper->CHARNAME(DEFINITE));
	}

      Stepper->CheckDeath("stepped on a broken bottle");
    }
}

material* can::CreateDipMaterial()
{
  material* Material = GetContainedMaterial()->Clone(GetContainedMaterial()->TakeDipVolumeAway());

  if(!GetContainedMaterial()->GetVolume())
    Empty();

  return Material;
}

material* potion::CreateDipMaterial()
{
  material* Material = GetContainedMaterial()->Clone(GetContainedMaterial()->TakeDipVolumeAway());

  if(!GetContainedMaterial()->GetVolume())
    Empty();

  return Material;
}

void potion::DipInto(material* Material, character* Dipper)
{
  /* Add alchemy */

  if(Dipper->IsPlayer())
    ADD_MESSAGE("%s is now filled with %s.", CHARNAME(DEFINITE), Material->GetName(false, false).c_str());

  ChangeContainedMaterial(Material);
  Dipper->DexterityAction(10);
}

ulong meleeweapon::GetPrice() const
{
  float WeaponStrengthModifier = float(GetFormModifier()) * GetMainMaterial()->GetStrengthValue() * GetWeight();
  WeaponStrengthModifier *= WeaponStrengthModifier;
  return ulong(WeaponStrengthModifier / (5000000000000.0f * sqrt(GetWeight()))) + item::GetPrice();
}

ulong armor::GetPrice() const
{
  float StrengthValue = GetStrengthValue();
  return ulong(StrengthValue * StrengthValue * StrengthValue * 20 / sqrt(GetWeight()));
}

void lantern::SignalSquarePositionChange(uchar NewSquarePosition)
{
  if(SquarePosition != NewSquarePosition)
    {
      SquarePosition = NewSquarePosition;
      UpdatePictures();
    }
}

void lantern::Save(outputfile& SaveFile) const
{
  item::Save(SaveFile);
  SaveFile << SquarePosition;
}

void lantern::Load(inputfile& SaveFile)
{
  item::Load(SaveFile);
  SaveFile >> SquarePosition;
}

item* potion::BetterVersion() const
{
  if(!GetContainedMaterial())
    {
      material* Stuff;

      if(RAND() % 5)
	Stuff = MAKE_MATERIAL(BANANAFLESH);
      else
	Stuff = MAKE_MATERIAL(OMLEURINE);

      potion* P = new potion(0, NOMATERIALS); 
      P->InitMaterials(MAKE_MATERIAL(GLASS), Stuff);

      return P;
    }
  else
    return 0;
}

item* can::BetterVersion() const
{
  if(!GetContainedMaterial())
    {
      material* Stuff;

      if(RAND() & 1)
	Stuff = MAKE_MATERIAL(SCHOOLFOOD);
      else
	Stuff = MAKE_MATERIAL(BANANAFLESH);

      can* P = new can(0, NOMATERIALS); 
      P->InitMaterials(MAKE_MATERIAL(IRON), Stuff); 

      return P;
    }
  else
    return 0;
}

ushort whip::GetFormModifier() const
{
  return item::GetFormModifier() * GetMainMaterial()->GetFlexibility();
}

bool backpack::Apply(character* Terrorist)
{
  if(IsExplosive())
    {
      if(Terrorist->IsPlayer())
	ADD_MESSAGE("You light your %s. It explodes!", CHARNAME(UNARTICLED));
      else if(Terrorist->CanBeSeenByPlayer())
	ADD_MESSAGE("%s lights %s. It explodes!", Terrorist->CHARNAME(DEFINITE), CHARNAME(INDEFINITE));
      else if(GetSquareUnder()->CanBeSeenByPlayer())
	ADD_MESSAGE("Something explodes!");

      RemoveFromSlot();
      SendToHell();

      std::string DeathMsg;

      if(Terrorist->IsPlayer())
	DeathMsg = "exploded himself with " + GetName(INDEFINITE);
      else
	DeathMsg = "kamikazed by " + Terrorist->GetName(INDEFINITE);

      Terrorist->DexterityAction(5);
      Terrorist->GetLevelUnder()->Explosion(Terrorist, DeathMsg, Terrorist->GetLSquareUnder()->GetPos(), GetContainedMaterial()->GetTotalExplosivePower());
      return true;
    }
  else if(Terrorist->IsPlayer())
    ADD_MESSAGE("You are not able to explode yourself with this crummy %s!", CHARNAME(UNARTICLED));

  return false;
}

bool holybook::CanBeRead(character* Reader) const
{
  return Reader->CanRead() || game::GetSeeWholeMapCheat();
}

bool holybook::Read(character* Reader)
{
  Reader->StartReading(this, 1000);
  return true;
}

void holybook::FinishReading(character* Reader)
{
  if(Reader->IsPlayer())
    {
      if(GetMasterGod()->GetKnown())
	{
	  ADD_MESSAGE("The book reveals many divine secrets of %s to you.", GetMasterGod()->GOD_NAME);
	  GetMasterGod()->AdjustRelation(75);
	  game::ApplyDivineAlignmentBonuses(GetMasterGod(), true);

	  if(!(RAND() % 3))
	    {
	      ADD_MESSAGE("But then it disappears.");
	      RemoveFromSlot();
	      SendToHell();
	    }
	}
      else
	{
	  GetMasterGod()->SetKnown(true);
	  ADD_MESSAGE("You feel you master the magical rites of %s.", GetMasterGod()->GOD_NAME);
	}
    }
}

bool wand::ReceiveDamage(character* Damager, ushort, uchar Type)
{
  if((Type == FIRE && !(RAND() % 10)) || (Type == ENERGY && !(RAND() % 10)))
    {
      std::string DeathMsg = "explosion of ";
      AddName(DeathMsg, INDEFINITE);

      if(Damager)
	DeathMsg << " caused by " << Damager->GetName(INDEFINITE);

      if(GetSquareUnder()->CanBeSeenByPlayer())
	ADD_MESSAGE("%s explodes!", CHARDESCRIPTION(DEFINITE));

      lsquare* Square = GetLSquareUnder();
      RemoveFromSlot();
      SendToHell();
      Square->GetLevelUnder()->Explosion(Damager, DeathMsg, Square->GetPos(), 40);
      return true;
    }

  return false;
}

bool backpack::ReceiveDamage(character* Damager, ushort, uchar Type)
{
  if(((Type == FIRE && !(RAND() % 3)) || (Type == ENERGY && RAND() % 3)) && IsExplosive())
    {
      std::string DeathMsg = "explosion of ";
      AddName(DeathMsg, INDEFINITE);

      if(Damager)
	DeathMsg << " caused by " << Damager->GetName(INDEFINITE);

      if(GetSquareUnder()->CanBeSeenByPlayer())
	ADD_MESSAGE("%s explodes!", CHARDESCRIPTION(DEFINITE));

      lsquare* Square = GetLSquareUnder();
      RemoveFromSlot();
      SendToHell();
      Square->GetLevelUnder()->Explosion(Damager, DeathMsg, Square->GetPos(), GetContainedMaterial()->GetTotalExplosivePower());
      return true;
    }

  return false;
}

bool scroll::ReceiveDamage(character*, ushort, uchar Type)
{
  if(Type == FIRE && !(RAND() % 10) && GetMainMaterial()->IsFlammable())
    {
      if(CanBeSeenByPlayer())
	ADD_MESSAGE("%s catches fire!", CHARNAME(DEFINITE));

      RemoveFromSlot();
      SendToHell();
      return true;
    }

  return false;
}

void wand::Beam(character* Zapper, const std::string& DeathMsg, uchar Direction, uchar Range)
{
  vector2d CurrentPos = Zapper->GetPos();

  if(Direction != YOURSELF)
    {
      for(ushort Length = 0; Length < Range; ++Length)
	{
	  if(!GetAreaUnder()->IsValidPos(CurrentPos + game::GetMoveVector(Direction)))
	    break;

	  lsquare* CurrentSquare = GetNearLSquare(CurrentPos + game::GetMoveVector(Direction));

	  if(!(CurrentSquare->GetOLTerrain()->IsWalkable()))
	    {
	      BeamEffect(Zapper, DeathMsg, Direction, CurrentSquare);
	      break;
	    }
	  else
	    {	
	      CurrentPos += game::GetMoveVector(Direction);

	      if(BeamEffect(Zapper, DeathMsg, Direction, CurrentSquare))
		break;

	      if(CurrentSquare->CanBeSeenByPlayer(true))
		CurrentSquare->DrawParticles(GetBeamColor(), Direction);
	    }
	}
    }
  else
    {
      lsquare* Where = Zapper->GetLSquareUnder();
      BeamEffect(Zapper, DeathMsg, Direction, Where);

      if(Where->CanBeSeenByPlayer(true))
	Where->DrawParticles(GetBeamColor(), Direction);
    }
}

bool wandofpolymorph::BeamEffect(character* Zapper, const std::string&, uchar, lsquare* LSquare)
{
  LSquare->PolymorphEverything(Zapper);
  return false;
}

bool wandofstriking::BeamEffect(character* Who, const std::string& DeathMsg, uchar Dir, lsquare* Where) 
{ 
  Where->StrikeEverything(Who, DeathMsg, 20, Dir); 
  return false;
}

bool holybook::ReceiveDamage(character*, ushort, uchar Type)
{
  if(Type == FIRE && RAND() & 1 && GetMainMaterial()->IsFlammable())
    {
      if(CanBeSeenByPlayer())
	ADD_MESSAGE("%s catches fire!", CHARNAME(DEFINITE));

      RemoveFromSlot();
      SendToHell();
      return true;
    }

  return false;
}

bool oillamp::Apply(character* Applier)
{
  if(Applier->IsPlayer())
    ADD_MESSAGE("You rub %s.", CHARNAME(DEFINITE));

  if(GetInhabitedByGenie())
    {
      genie* Genie = new genie;
      vector2d TryToCreate;
      bool FoundPlace = false;

      /* First try to create a genie nearby (10 tries - if all of them fail then stop trying) */

      for(ushort c = 0; c < 10 && !FoundPlace; ++c)
	{	  
	  TryToCreate = Applier->GetPos() + game::GetMoveVector(RAND() % DIRECTION_COMMAND_KEYS);

	  if(GetAreaUnder()->IsValidPos(TryToCreate) && GetNearSquare(TryToCreate)->IsWalkable(Genie) && GetNearSquare(TryToCreate)->GetCharacter() == 0)
	    {
	      GetNearSquare(TryToCreate)->AddCharacter(Genie);
	      FoundPlace = true;
	      SetInhabitedByGenie(false);
	      break;
	    }
	}

      if(FoundPlace)
	{
	  if(!(RAND() % 5))
	    {
	      Genie->SetTeam(game::GetTeam(1));
	      ADD_MESSAGE("You see a puff of smoke, and %s appears. \"For centuries I have been imprisoned in this lamp. But at last you have freed me! As a reward, I will kill you.\"", Genie->CHARNAME(INDEFINITE));
	    }
	  else
	    {
	      if(Applier->IsPlayer())
		{
		  Genie->SetTeam(Applier->GetTeam());
		  ADD_MESSAGE("You see a puff of smoke, and %s appears. \"For centuries I have been imprisoned in this lamp. But at last you have freed me! I am deeply grateful. You deserve a generous reward. I may serve you for 1001 nights or grant you a wish. It's your choice.\"", Genie->CHARNAME(INDEFINITE));
		  game::DrawEverything();
		  GETKEY();

		  if(game::BoolQuestion("Do you want to wish? [Y/n]", YES))
		    {
		      ADD_MESSAGE("You may wish for an item.");

		      while(true)
			{
			  std::string Temp = game::StringQuestion("What do you want to wish for?", vector2d(16, 6), WHITE, 0, 80, false);
			  item* TempItem = protosystem::CreateItem(Temp, Applier->IsPlayer());

			  if(TempItem)
			    {
			      Applier->GetStack()->AddItem(TempItem);
			      ADD_MESSAGE("%s appears from nothing and the genie flies happily away!", TempItem->CHARNAME(INDEFINITE));
			      break;
			    }
			}

		      GetLevelUnder()->RemoveCharacter(TryToCreate);
		      delete Genie;
		      Applier->EditAP(-1000);
		      return true;
		    }
		}
	    }

	    ADD_MESSAGE("%s wishes for %s. Suddenly %s appears from nothing and %s wields it.", Genie->CHARNAME(DEFINITE), Genie->GetMainWielded()->CHARNAME(INDEFINITE), Genie->GetMainWielded()->CHARNAME(INDEFINITE), Genie->CHARNAME(DEFINITE));
	}
      else
	{
	  if(Applier->IsPlayer())
	    ADD_MESSAGE("You feel that it is warmer.");

	  delete Genie;
	}
    }
  else
    if(Applier->IsPlayer())
      ADD_MESSAGE("Nothing happens.");

  Applier->EditAP(-1000);
  return true;
}

void oillamp::Save(outputfile& SaveFile) const
{
  item::Save(SaveFile);
  SaveFile << InhabitedByGenie;
}

void oillamp::Load(inputfile& SaveFile)
{
  item::Load(SaveFile);
  SaveFile >> InhabitedByGenie;
}

ushort holybook::GetMaterialColorA(ushort) const
{
  return GetMasterGod()->GetColor();
}

bool scrollofcharging::Read(character* Reader)
{
  if(Reader->GetStack()->GetItems() > 1) // that 1 is the scroll itself
    {
      ADD_MESSAGE("You have nothing to charge.");
      return false;
    }
  else
    {
      Reader->StartReading(this, 500);
      return true;
    }
}

void scrollofcharging::FinishReading(character* Reader)
{
  if(!Reader->GetStack()->SortedItems(Reader, &item::ChargeableSorter))
    ADD_MESSAGE("You have lost whatever you wished to charge.");
  else
    while(true)
      {
	item* Item = Reader->GetStack()->DrawContents(Reader, "What item do you wish to charge?", 0, &item::ChargeableSorter);

	if(Item)
	  {
	    game::DrawEverythingNoBlit();
	    Item->ChargeFully(Reader);
	    ADD_MESSAGE("You charge %s and the scroll burns.", Item->CHARNAME(DEFINITE));
	    break;
	  }
      }

  RemoveFromSlot();
  SendToHell();
}

void banana::Save(outputfile& SaveFile) const
{
  meleeweapon::Save(SaveFile);
  SaveFile << Charges;
}

void banana::Load(inputfile& SaveFile)
{
  meleeweapon::Load(SaveFile);
  SaveFile >> Charges;
}

bool banana::Zap(character*, vector2d, uchar)
{
  if(Charges)
    {
      ADD_MESSAGE("BANG! You zap %s!", CHARNAME(DEFINITE));
      --Charges;
    }
  else
    ADD_MESSAGE("Click!");

  return true;
}

void bananapeels::StepOnEffect(character* Stepper)
{
  if(Stepper->HasFeet() && !(RAND() % 3))
    {
      if(Stepper->IsPlayer())
	ADD_MESSAGE("Auch. Your feet slip on %s and you fall down.", CHARNAME(INDEFINITE));
      else if(Stepper->CanBeSeenByPlayer())
	ADD_MESSAGE("%s steps on %s and falls down.", Stepper->CHARNAME(DEFINITE), CHARNAME(INDEFINITE));

      /* Do damage against any random bodypart except legs */

      Stepper->ReceiveDamage(0, 2 + (RAND() & 1), PHYSICALDAMAGE, ALL&~LEGS);
      Stepper->CheckDeath("slipped on a banana peel.");
      Stepper->EditAP(-1000);
    }
}

bool wandoffireballs::BeamEffect(character* Who, const std::string& DeathMsg, uchar, lsquare* Where) 
{ 
  if(!Where->GetOTerrain()->IsWalkable() || Where->GetCharacter())
    {
      Where->GetLevelUnder()->Explosion(Who, DeathMsg, Where->GetPos(), 20 + RAND() % 5 - RAND() % 5);
      return true;
    }

  return false;
}

bool scrolloftaming::Read(character* Reader)
{
  Reader->StartReading(this, 100);
  return true;
}

void scrolloftaming::FinishReading(character* Reader)
{
  // First find all characters in the squares around Reader

  std::vector<character*> CharactersNearBy;

  for(ushort c = 0; c < 8; ++c)
    {
      vector2d Test = Reader->GetPos() + game::GetMoveVector(c);

      if(GetAreaUnder()->IsValidPos(Test))
	{
	  character* CharacterInSquare = GetNearSquare(Test)->GetCharacter();

	  if(CharacterInSquare && CharacterInSquare->IsCharmable() && CharacterInSquare->GetTeam() != Reader->GetTeam())
	    CharactersNearBy.push_back(CharacterInSquare);
	}
    }
  
  // Then pick one of the characters and set it to the same team as Reader

  if(CharactersNearBy.empty())
    {
      if(Reader->IsPlayer() || Reader->CanBeSeenByPlayer())
	ADD_MESSAGE("The scroll burns, but nothing happens.");
    }
  else
    {
      character* ToBeTamed = CharactersNearBy[RAND() % CharactersNearBy.size()];
      ToBeTamed->ChangeTeam(Reader->GetTeam());

      if(Reader->IsPlayer())
	{
	  if(ToBeTamed->CanBeSeenByPlayer())
	    ADD_MESSAGE("%s looks much friendlier.", ToBeTamed->CHARNAME(INDEFINITE));
	  else
	    ADD_MESSAGE("You notice no effect.");
	}
      else if(Reader->CanBeSeenByPlayer())
	{
	  if(ToBeTamed->CanBeSeenByPlayer())
	    ADD_MESSAGE("%s seems to like %s far more.", ToBeTamed->CHARNAME(INDEFINITE), Reader->CHARNAME(DEFINITE));
	}
      else if(ToBeTamed->CanBeSeenByPlayer())
	ADD_MESSAGE("Suddenly %s looks like having fallen in love with something nearby.", ToBeTamed->CHARNAME(INDEFINITE));
    }

  RemoveFromSlot();
  SendToHell();
}

void bodypart::Save(outputfile& SaveFile) const
{
  materialcontainer::Save(SaveFile);
  SaveFile << BitmapPos << ColorB << ColorC << ColorD << SpecialFlags << HP << OwnerDescription << Unique << BloodColor;
}

void bodypart::Load(inputfile& SaveFile)
{
  materialcontainer::Load(SaveFile);
  SaveFile >> BitmapPos >> ColorB >> ColorC >> ColorD >> SpecialFlags  >> HP >> OwnerDescription >> Unique >> BloodColor;
}

bool wandofteleportation::BeamEffect(character* Who, const std::string&, uchar, lsquare* Where) 
{ 
  Where->TeleportEverything(Who);
  return false;
}

ushort bodypart::GetStrengthValue() const
{
  if(GetMaster() && IsAlive())
    return ulong(GetStrengthModifier()) * GetMaster()->GetAttribute(ENDURANCE) / 1000;
  else
    return ulong(GetStrengthModifier()) * GetMainMaterial()->GetStrengthValue() / 1000;
}

ushort head::GetTotalResistance(uchar Type) const
{
  if(GetMaster())
    {
      ushort Resistance = GetResistance(Type) + GetMaster()->GlobalResistance(Type);

      if(GetHelmet())
	Resistance += GetHelmet()->GetResistance(Type);

      return Resistance;
    }
  else
    return GetResistance(Type);
}

ushort normaltorso::GetTotalResistance(uchar Type) const
{
  if(GetMaster())
    return GetResistance(Type) + GetMaster()->GlobalResistance(Type);
  else
    return GetResistance(Type);
}

ushort humanoidtorso::GetTotalResistance(uchar Type) const
{
  if(GetMaster())
    {
      ushort Resistance = GetResistance(Type) + GetMaster()->GlobalResistance(Type);

      if(GetBodyArmor())
	Resistance += GetBodyArmor()->GetResistance(Type);

      if(GetBelt())
	Resistance += GetBelt()->GetResistance(Type);

      return Resistance;
    }
  else
    return GetResistance(Type);
}

ushort arm::GetTotalResistance(uchar Type) const
{
  if(GetMaster())
    {
      ushort Resistance = GetResistance(Type) + GetMaster()->GlobalResistance(Type);

      if(GetHumanoidMaster()->GetBodyArmor())
	Resistance += GetHumanoidMaster()->GetBodyArmor()->GetResistance(Type) / 2;

      if(GetGauntlet())
	Resistance += GetGauntlet()->GetResistance(Type);

      return Resistance;
    }
  else
    return GetResistance(Type);
}

ushort groin::GetTotalResistance(uchar Type) const
{
  if(GetMaster())
    {
      ushort Resistance = GetResistance(Type) + GetMaster()->GlobalResistance(Type);

      if(GetHumanoidMaster()->GetBodyArmor())
	Resistance += GetHumanoidMaster()->GetBodyArmor()->GetResistance(Type);

      if(GetHumanoidMaster()->GetBelt())
	Resistance += GetHumanoidMaster()->GetBelt()->GetResistance(Type);

      return Resistance;
    }
  else
    return GetResistance(Type);
}

ushort leg::GetTotalResistance(uchar Type) const
{
  if(GetMaster())
    {
      ushort Resistance = GetResistance(Type) + GetMaster()->GlobalResistance(Type);

      if(GetHumanoidMaster()->GetBodyArmor())
	Resistance += GetHumanoidMaster()->GetBodyArmor()->GetResistance(Type) / 2;

      if(GetBoot())
	Resistance += GetBoot()->GetResistance(Type);

      return Resistance;
    }
  else
    return GetResistance(Type);
}

void head::Save(outputfile& SaveFile) const
{
  bodypart::Save(SaveFile);
  SaveFile << BaseBiteStrength;
  SaveFile << HelmetSlot << AmuletSlot;
}

void head::Load(inputfile& SaveFile)
{
  bodypart::Load(SaveFile);
  SaveFile >> BaseBiteStrength;
  SaveFile >> HelmetSlot >> AmuletSlot;
}

void humanoidtorso::Save(outputfile& SaveFile) const
{
  bodypart::Save(SaveFile);
  SaveFile << BodyArmorSlot << CloakSlot << BeltSlot;
}

void humanoidtorso::Load(inputfile& SaveFile)
{
  bodypart::Load(SaveFile);
  SaveFile >> BodyArmorSlot >> CloakSlot >> BeltSlot;
}

void arm::Save(outputfile& SaveFile) const
{
  bodypart::Save(SaveFile);
  SaveFile << BaseUnarmedStrength;
  SaveFile << Strength << StrengthExperience << Dexterity << DexterityExperience;
  SaveFile << WieldedSlot << GauntletSlot << RingSlot;
}

void arm::Load(inputfile& SaveFile)
{
  bodypart::Load(SaveFile);
  SaveFile >> BaseUnarmedStrength;
  SaveFile >> Strength >> StrengthExperience >> Dexterity >> DexterityExperience;
  SaveFile >> WieldedSlot >> GauntletSlot >> RingSlot;
}

void leg::Save(outputfile& SaveFile) const
{
  bodypart::Save(SaveFile);
  SaveFile << BaseKickStrength << Strength << StrengthExperience << Agility << AgilityExperience;
  SaveFile << BootSlot;
}

void leg::Load(inputfile& SaveFile)
{
  bodypart::Load(SaveFile);
  SaveFile >> BaseKickStrength >> Strength >> StrengthExperience >> Agility >> AgilityExperience;
  SaveFile >> BootSlot;
}

bool bodypart::ReceiveDamage(character*, ushort Damage, uchar)
{
  if(GetMaster())
    {
      ushort BHP = GetHP();

      if(GetHP() <= Damage && GetHP() == GetMaxHP() && GetHP() != 1 && GetMaster()->BodyPartVital(GetBodyPartIndex()))
	Damage = GetHP() - 1;

      EditHP(-Damage);

      if(GetHP() <= 0)
	return true;
      else if(GetMaster()->IsPlayer())
	if(GetHP() == 1 && BHP > 1)
	  {
	    game::Beep();

	    if(IsAlive())
	      ADD_MESSAGE("Your %s bleeds very badly.", GetBodyPartName().c_str());
	    else
	      ADD_MESSAGE("Your %s is in very bad condition.", GetBodyPartName().c_str());
	  }
	else if(IsInBadCondition() && !IsInBadCondition(BHP))
	  {
	    game::Beep();

	    if(IsAlive())
	      ADD_MESSAGE("Your %s bleeds.", GetBodyPartName().c_str());
	    else
	      ADD_MESSAGE("Your %s is in bad condition.", GetBodyPartName().c_str());
	  }
    }

  return false;
}

void mine::Load(inputfile& SaveFile)
{
  materialcontainer::Load(SaveFile);
  SaveFile >> Active >> Team;
}

void mine::Save(outputfile& SaveFile) const
{
  materialcontainer::Save(SaveFile);
  SaveFile << Active << Team;
}

bool mine::ReceiveDamage(character* Damager, ushort, uchar Type)
{
  if((Type == FIRE && RAND() & 1) || (Type == ENERGY && RAND() & 1) || ((Type == PHYSICALDAMAGE) && WillExplode(0)))
    {
      std::string DeathMsg = "killed by an explosion of ";
      AddName(DeathMsg, INDEFINITE);

      if(Damager)
	{
	  DeathMsg << " caused by ";
	  Damager->AddName(DeathMsg, INDEFINITE);
	}

      if(GetSquareUnder()->CanBeSeenByPlayer())
	ADD_MESSAGE("%s explodes!", CHARDESCRIPTION(DEFINITE));

      lsquare* Square = GetLSquareUnder();
      RemoveFromSlot();
      SendToHell();
      Square->GetLevelUnder()->Explosion(Damager, DeathMsg, Square->GetPos(), GetContainedMaterial()->GetTotalExplosivePower());
      return true;
    }

  return false;
}

void mine::StepOnEffect(character* Stepper)
{
  if(!WillExplode(Stepper))
    return;

  if(Stepper->IsPlayer())
    ADD_MESSAGE("You hear a faint thumb. You look down. You see %s.", CHARNAME(INDEFINITE));
  else if(Stepper->CanBeSeenByPlayer())
    ADD_MESSAGE("%s steps on %s.", Stepper->CHARNAME(DEFINITE), CHARNAME(INDEFINITE));
  else if(GetSquareUnder()->CanBeSeenByPlayer())
    ADD_MESSAGE("Something explodes!");

  lsquare* Square = GetLSquareUnder();
  RemoveFromSlot();
  SendToHell();
  Square->GetLevelUnder()->Explosion(0, "killed by a land mine", Square->GetPos(), GetContainedMaterial()->GetTotalExplosivePower());
}

bool wandofhaste::BeamEffect(character*, const std::string&, uchar, lsquare* LSquare)
{
  character* Dude = LSquare->GetCharacter();

  if(Dude)
    Dude->BeginTemporaryState(HASTE, 500 + RAND() % 1000);

  return false;
}

bool wandofslow::BeamEffect(character*, const std::string&, uchar, lsquare* LSquare)
{
  character* Dude = LSquare->GetCharacter();

  if(Dude)
    Dude->BeginTemporaryState(SLOW, 500 + RAND() % 1000);

  return false;
}

bool key::Apply(character* User)
{
  if(User->IsPlayer())
    {
      uchar Dir = game::DirectionQuestion("What door do you wish to lock or unlock? [press a direction key]", true);

      if(Dir == DIR_ERROR)
	return false;

      vector2d ApplyPos = User->GetPos() + game::GetMoveVector(Dir);

      if(GetAreaUnder()->IsValidPos(ApplyPos))
	{
	  GetNearLSquare(ApplyPos)->TryKey(this, User);
	  User->DexterityAction(5);
	}
      else
	return false;
    }

  return true;
}

float arm::GetUnarmedDamage() const
{
  float WeaponStrength = GetBaseUnarmedStrength();

  if(GetGauntlet())
    WeaponStrength += GetGauntlet()->GetWeaponStrength();

  return WeaponStrength * GetAttribute(ARMSTRENGTH) * GetHumanoidMaster()->GetCWeaponSkill(UNARMED)->GetEffectBonus() / 5000000;
}

float arm::GetUnarmedToHitValue() const
{
  return float((GetAttribute(DEXTERITY) << 2) + GetMaster()->GetAttribute(PERCEPTION)) * GetHumanoidMaster()->GetCWeaponSkill(UNARMED)->GetEffectBonus() * GetMaster()->GetMoveEase() / 10000;
}

long arm::GetUnarmedAPCost() const
{
  return long(float(GetMaster()->GetCWeaponSkill(UNARMED)->GetAPBonus()) * (200 - GetAttribute(DEXTERITY)) * 5 / GetMaster()->GetMoveEase());
}

void arm::CalculateDamage()
{
  if(!Master)
    return;

  if(GetWielded())
    Damage = GetWieldedDamage();
  else if(PairArmAllowsMelee())
    Damage = GetUnarmedDamage();
  else
    Damage = 0;
}

void arm::CalculateToHitValue()
{
  if(!Master)
    return;

  if(GetWielded())
    ToHitValue = GetWieldedToHitValue();
  else if(PairArmAllowsMelee())
    ToHitValue = GetUnarmedToHitValue();
  else
    ToHitValue = 0;
}

void arm::CalculateAPCost()
{
  if(!Master)
    return;

  if(GetWielded())
    APCost = GetWieldedAPCost();
  else if(PairArmAllowsMelee())
    APCost = GetUnarmedAPCost();
  else
    APCost = 100;

  if(APCost < 100)
    APCost = 100;
}

bool arm::PairArmAllowsMelee() const
{
  arm* PairArm = GetPairArm();
  return !PairArm || !PairArm->GetWielded() || PairArm->GetWielded()->IsShield(Master);
}

float arm::GetWieldedDamage() const
{
  if(GetWielded()->IsShield(Master))
    return 0;

  float HitStrength = GetAttribute(ARMSTRENGTH);

  if(TwoHandWieldIsActive())
    HitStrength += GetPairArm()->GetAttribute(ARMSTRENGTH);

  return HitStrength * GetWielded()->GetWeaponStrength() * GetCurrentSWeaponSkill()->GetEffectBonus() * GetHumanoidMaster()->GetCWeaponSkill(GetWielded()->GetWeaponCategory())->GetEffectBonus() / 500000000;
}

float arm::GetWieldedToHitValue() const
{
  arm* PairArm = GetPairArm();
  float Weight = GetWielded()->GetWeight();

  if(Weight < 500)
    Weight = 500;

  float Bonus = float(GetHumanoidMaster()->GetCWeaponSkill(GetWielded()->GetWeaponCategory())->GetEffectBonus()) * GetCurrentSWeaponSkill()->GetEffectBonus() * GetMaster()->GetMoveEase() * GetWielded()->GetToHitValueBonus() / (1000000 + (Weight - 500) * 400);
  float ThisToHit = ((GetAttribute(DEXTERITY) << 2) + GetMaster()->GetAttribute(PERCEPTION));

  if(PairArm)
    {
      if(!PairArm->GetWielded())
	{
	  if(GetWielded()->IsTwoHanded())
	    return Bonus * (ThisToHit + ((PairArm->GetAttribute(DEXTERITY) << 2) + GetMaster()->GetAttribute(PERCEPTION))) / 2;
	}
      else
	{
	  float Weight = GetPairArm()->GetWielded()->GetWeight();

	  if(Weight < 500)
	    Weight = 500;

	  return Bonus * ThisToHit / (1 + (Weight - 500) / 10000);
	}
    }

  return Bonus * ThisToHit;
}

long arm::GetWieldedAPCost() const
{
  arm* PairArm = GetPairArm();
  float SkillPenalty = 0.01f * GetHumanoidMaster()->GetCWeaponSkill(GetWielded()->GetWeaponCategory())->GetAPBonus() * GetCurrentSWeaponSkill()->GetAPBonus() * GetWielded()->GetAPBonus() / GetMaster()->GetMoveEase();

  if(PairArm)
    {
      if(!PairArm->GetWielded())
	{
	  if(GetWielded()->IsTwoHanded())
	    return long(SkillPenalty * ((400 - (GetAttribute(DEXTERITY) + PairArm->GetAttribute(DEXTERITY))) * 2.5f + GetWielded()->GetWeight() / 50));
	}
      else
	return long(SkillPenalty * ((200 - GetAttribute(DEXTERITY)) * 5 + (GetWielded()->GetWeight() + GetPairArm()->GetWielded()->GetWeight()) / 50));
    }

  return long(SkillPenalty * ((200 - GetAttribute(DEXTERITY)) * 5 + GetWielded()->GetWeight() / 50));
}

void head::CalculateDamage()
{
  if(!Master)
    return;

  BiteDamage = float(GetBaseBiteStrength()) * GetHumanoidMaster()->GetCWeaponSkill(BITE)->GetEffectBonus() / 500000;
}

void head::CalculateToHitValue()
{
  if(!Master)
    return;

  BiteToHitValue = float((Master->GetAttribute(AGILITY) << 2) + GetMaster()->GetAttribute(PERCEPTION)) * GetHumanoidMaster()->GetCWeaponSkill(KICK)->GetEffectBonus() * GetMaster()->GetMoveEase() / 20000;
}

void head::CalculateAPCost()
{
  if(!Master)
    return;

  BiteAPCost = long(float(GetMaster()->GetCWeaponSkill(BITE)->GetAPBonus()) * (200 - GetMaster()->GetAttribute(AGILITY)) * 5 / GetMaster()->GetMoveEase());

  if(BiteAPCost < 100)
    BiteAPCost = 100;
}

void leg::CalculateDamage()
{
  if(!Master)
    return;

  float WeaponStrength = GetBaseKickStrength();

  if(GetBoot())
    WeaponStrength += GetBoot()->GetWeaponStrength();

  KickDamage = WeaponStrength * GetAttribute(LEGSTRENGTH) * GetHumanoidMaster()->GetCWeaponSkill(KICK)->GetEffectBonus() / 5000000;
}

void leg::CalculateToHitValue()
{
  if(!Master)
    return;

  KickToHitValue = float((GetAttribute(AGILITY) << 2) + GetMaster()->GetAttribute(PERCEPTION)) * GetHumanoidMaster()->GetCWeaponSkill(KICK)->GetEffectBonus() * GetMaster()->GetMoveEase() / 20000;
}

void leg::CalculateAPCost()
{
  if(!Master)
    return;

  KickAPCost = long(float(GetMaster()->GetCWeaponSkill(KICK)->GetAPBonus()) * (200 - GetAttribute(AGILITY)) * 10 / GetMaster()->GetMoveEase());

  if(KickAPCost < 100)
    KickAPCost = 100;
}

humanoid* bodypart::GetHumanoidMaster() const
{
  return static_cast<humanoid*>(Master);
}

ushort belt::GetFormModifier() const
{
  return item::GetFormModifier() * GetMainMaterial()->GetFlexibility();
}

bool pickaxe::IsAppliable(const character* Who) const
{
  return Who->CanWield();
}

void key::Save(outputfile& SaveFile) const
{
  item::Save(SaveFile);
  SaveFile << LockType;
}

void key::Load(inputfile& SaveFile)
{
  item::Load(SaveFile);
  SaveFile >> LockType;
}

void corpse::Save(outputfile& SaveFile) const
{
  item::Save(SaveFile);
  SaveFile << Deceased;
}

void corpse::Load(inputfile& SaveFile)
{
  item::Load(SaveFile);
  SaveFile >> Deceased;
  Deceased->SetMotherEntity(this);
}

void corpse::AddPostFix(std::string& String) const
{
  String << " of ";
  GetDeceased()->AddName(String, INDEFINITE);
}

bool corpse::Consume(character* Eater, long Amount)
{
  return GetDeceased()->GetTorso()->Consume(Eater, Amount);
}

void corpse::GenerateLeftOvers(character* Eater)
{
  GetDeceased()->GetTorso()->GenerateLeftOvers(Eater);
  RemoveFromSlot();
  SendToHell();
}

bool corpse::IsConsumable(const character* Eater) const
{
  for(ushort c = 0; c < GetDeceased()->GetBodyParts(); ++c)
    if(GetDeceased()->GetBodyPart(c) && GetDeceased()->GetBodyPart(c)->IsConsumable(Eater))
      return true;

  return false;
}

short corpse::GetOfferValue(char GodAlignment) const
{
  short OfferValue = 0;

  for(ushort c = 0; c < GetDeceased()->GetBodyParts(); ++c)
    if(GetDeceased()->GetBodyPart(c))
      OfferValue += GetDeceased()->GetBodyPart(c)->GetOfferValue(GodAlignment);

  return OfferValue;
}

float corpse::GetWeaponStrength() const
{
  return sqrt(float(GetFormModifier()) * GetDeceased()->GetTorso()->GetMainMaterial()->GetStrengthValue() * GetWeight());
}

bool corpse::IsBadFoodForAI(character* Eater) const
{
  for(ushort c = 0; c < GetDeceased()->GetBodyParts(); ++c)
    if(GetDeceased()->GetBodyPart(c) && GetDeceased()->GetBodyPart(c)->IsBadFoodForAI(Eater))
      return true;

  return false;
}

ushort corpse::GetStrengthValue() const
{
  return ulong(GetStrengthModifier()) * GetDeceased()->GetTorso()->GetMainMaterial()->GetStrengthValue() / 1000;
}

corpse::~corpse()
{
  delete Deceased;
}

ushort corpse::GetMaterialColorA(ushort) const
{
  return GetDeceased()->GetTorso()->GetMainMaterial()->GetColor();
}

uchar corpse::GetAlphaA(ushort) const
{
  return GetDeceased()->GetTorso()->GetMainMaterial()->GetAlpha();
}

ushort corpse::GetMaterialColorB(ushort) const
{
  return GetDeceased()->GetTorso()->IsAlive() ? GetDeceased()->GetBloodColor() : GetDeceased()->GetTorso()->GetMainMaterial()->GetColor();
}

vector2d corpse::GetBitmapPos(ushort) const
{
  if(GetDeceased()->GetSize() < 50)
    return vector2d(32, 64);
  else if(GetDeceased()->GetSize() < 150)
    return vector2d(16, 192);
  else
    return vector2d(16, 272);
}

ushort corpse::GetSize() const
{
  return GetDeceased()->GetSize();
}

void corpse::SetDeceased(character* What)
{
  Deceased = What;
  Deceased->SetMotherEntity(this);
  SignalVolumeAndWeightChange();
  UpdatePictures();
}

void head::DropEquipment()
{
  if(GetHelmet())
    GetSlot()->AddFriendItem(GetHelmet());

  if(GetAmulet())
    GetSlot()->AddFriendItem(GetAmulet());
}

void humanoidtorso::DropEquipment()
{
  if(GetBodyArmor())
    GetSlot()->AddFriendItem(GetBodyArmor());

  if(GetCloak())
    GetSlot()->AddFriendItem(GetCloak());

  if(GetBelt())
    GetSlot()->AddFriendItem(GetBelt());
}

void arm::DropEquipment()
{
  if(GetWielded())
    GetSlot()->AddFriendItem(GetWielded());

  if(GetGauntlet())
    GetSlot()->AddFriendItem(GetGauntlet());

  if(GetRing())
    GetSlot()->AddFriendItem(GetRing());
}

void leg::DropEquipment()
{
  if(GetBoot())
    GetSlot()->AddFriendItem(GetBoot());
}

void corpse::AddConsumeEndMessage(character* Eater) const
{
  GetDeceased()->GetTorso()->AddConsumeEndMessage(Eater);
}

head::~head()
{
  delete GetHelmet();
  delete GetAmulet();
}

humanoidtorso::~humanoidtorso()
{
  delete GetBodyArmor();
  delete GetCloak();
  delete GetBelt();
}

arm::~arm()
{
  delete GetWielded();
  delete GetGauntlet();
  delete GetRing();
}

leg::~leg()
{
  delete GetBoot();
}

long corpse::Score() const
{
  long Score = 0;

  for(ushort c = 0; c < GetDeceased()->GetBodyParts(); ++c)
    if(GetDeceased()->GetBodyPart(c))
      Score += GetDeceased()->GetBodyPart(c)->GetScore();

  return Score;
}

bool corpse::IsDestroyable() const
{
  for(ushort c = 0; c < GetDeceased()->GetBodyParts(); ++c)
    if(GetDeceased()->GetBodyPart(c) && !GetDeceased()->GetBodyPart(c)->IsDestroyable())
      return false;

  return true;
}

ulong corpse::GetPrice() const
{
  ulong Price = 0;

  for(ushort c = 0; c < GetDeceased()->GetBodyParts(); ++c)
    if(GetDeceased()->GetBodyPart(c))
      Price += GetDeceased()->GetBodyPart(c)->GetPrice();

  return Price;
}

item* corpse::PrepareForConsuming(character*)
{
  for(ushort c = GetDeceased()->GetBodyParts() - 1; c != 0; --c)
    {
      bodypart* BPart = GetDeceased()->GetBodyPart(c);

      if(BPart)
	{
	  GetDeceased()->SevereBodyPart(c);
	  GetSlot()->AddFriendItem(BPart);
	  return BPart;
	}
    }

  return this;
}

bool wandoflocking::BeamEffect(character* Who, const std::string&, uchar, lsquare* Where) 
{ 
  return Where->LockEverything(Who); 
}

void materialcontainer::Save(outputfile& SaveFile) const
{
  item::Save(SaveFile);
  SaveFile << ContainedMaterial;
}

void materialcontainer::Load(inputfile& SaveFile)
{
  item::Load(SaveFile);
  LoadMaterial(SaveFile, ContainedMaterial);
}

void meleeweapon::Save(outputfile& SaveFile) const
{
  item::Save(SaveFile);
  SaveFile << Enchantment;
  SaveFile << SecondaryMaterial;
  SaveFile << ContainedMaterial;
}

void meleeweapon::Load(inputfile& SaveFile)
{
  item::Load(SaveFile);
  SaveFile >> Enchantment;
  LoadMaterial(SaveFile, SecondaryMaterial);
  LoadMaterial(SaveFile, ContainedMaterial);
}

material*& materialcontainer::GetMaterialReference(ushort Index)
{
  switch(Index)
    {
    case 0: return MainMaterial;
    default: return ContainedMaterial;
    }
}

material*& meleeweapon::GetMaterialReference(ushort Index)
{
  switch(Index)
    {
    case 0: return MainMaterial;
    case 1: return SecondaryMaterial;
    default: return ContainedMaterial;
    }
}

material*& corpse::GetMaterialReference(ushort Index)
{
  return GetDeceased()->GetTorso()->GetMaterialReference(Index);
}

ushort materialcontainer::GetMaterialColorB(ushort Frame) const
{
  if(GetContainedMaterial())
    return GetContainedMaterial()->GetColor();
  else
    return GetMaterialColorA(Frame);
}

uchar materialcontainer::GetAlphaB(ushort Frame) const
{
  if(GetContainedMaterial() && GetContainedMaterial()->GetAlpha() > GetAlphaA(Frame))
    return GetContainedMaterial()->GetAlpha();
  else
    return GetAlphaA(Frame);
}

ushort meleeweapon::GetMaterialColorB(ushort) const
{
  if(GetSecondaryMaterial())
    return GetSecondaryMaterial()->GetColor();
  else
    return 0;
}

uchar meleeweapon::GetAlphaB(ushort) const
{
  if(GetSecondaryMaterial())
    return GetSecondaryMaterial()->GetAlpha();
  else
    return 0;
}

ushort bodypart::GetMaterialColorA(ushort) const
{
  if(GetMainMaterial())
    return GetMainMaterial()->GetSkinColor();
  else
    return 0;
}

bool wandofresurrection::BeamEffect(character* Zapper, const std::string&, uchar, lsquare* LSquare)
{
  return LSquare->RaiseTheDead(Zapper);
}

bool corpse::RaiseTheDead(character* Summoner)
{
  if(Summoner->IsPlayer())
    game::DoEvilDeed(50);

  GetLSquareUnder()->AddCharacter(GetDeceased());
  RemoveFromSlot();
  GetDeceased()->SetHasBe(true);
  GetDeceased()->CompleteRiseFromTheDead();
  GetDeceased()->SetMotherEntity(0);
  Deceased = 0;
  SendToHell();
  return true;
}

void banana::VirtualConstructor(bool Load)
{
  meleeweapon::VirtualConstructor(Load);

  if(!Load)
    SetCharges(GetMinCharges() + RAND() % (GetMaxCharges() - GetMinCharges() + 1));
}

void lantern::VirtualConstructor(bool)
{
  SetSquarePosition(CENTER);
}

void wand::VirtualConstructor(bool Load)
{
  if(!Load)
    {
      SetCharges(GetMinCharges() + RAND() % (GetMaxCharges() - GetMinCharges() + 1));
      SetTimesUsed(0);
    }
}

void oillamp::VirtualConstructor(bool Load)
{
  if(!Load)
    SetInhabitedByGenie(RAND() & 1);
}

void mine::VirtualConstructor(bool)
{
  Active = false; /* this must be false */
}

void key::VirtualConstructor(bool Load)
{ 
  if(!Load)
    SetLockType(RAND() % NUMBER_OF_LOCK_TYPES);
}

void bodypart::VirtualConstructor(bool Load)
{
  materialcontainer::VirtualConstructor(Load);
  SetUnique(false);
  SetMaster(0);
}

void head::VirtualConstructor(bool Load)
{
  bodypart::VirtualConstructor(Load);
  HelmetSlot.Init(this, HELMETINDEX);
  AmuletSlot.Init(this, AMULETINDEX);
}

void humanoidtorso::VirtualConstructor(bool Load)
{
  torso::VirtualConstructor(Load);
  BodyArmorSlot.Init(this, BODYARMORINDEX);
  CloakSlot.Init(this, CLOAKINDEX);
  BeltSlot.Init(this, BELTINDEX);
}

void arm::VirtualConstructor(bool Load)
{
  bodypart::VirtualConstructor(Load);

  if(!Load)
    StrengthBonus = DexterityBonus = StrengthExperience = DexterityExperience = 0;
}

void rightarm::VirtualConstructor(bool Load)
{
  arm::VirtualConstructor(Load);
  WieldedSlot.Init(this, RIGHTWIELDEDINDEX);
  GauntletSlot.Init(this, RIGHTGAUNTLETINDEX);
  RingSlot.Init(this, RIGHTRINGINDEX);
}

void leftarm::VirtualConstructor(bool Load)
{
  arm::VirtualConstructor(Load);
  WieldedSlot.Init(this, LEFTWIELDEDINDEX);
  GauntletSlot.Init(this, LEFTGAUNTLETINDEX);
  RingSlot.Init(this, LEFTRINGINDEX);
}

void leg::VirtualConstructor(bool Load)
{
  bodypart::VirtualConstructor(Load);

  if(!Load)
    StrengthBonus = AgilityBonus = StrengthExperience = AgilityExperience = 0;
}

void rightleg::VirtualConstructor(bool Load)
{
  leg::VirtualConstructor(Load);
  BootSlot.Init(this, RIGHTBOOTINDEX);
}

void leftleg::VirtualConstructor(bool Load)
{
  leg::VirtualConstructor(Load);
  BootSlot.Init(this, LEFTBOOTINDEX);
}

bool whistle::Apply(character* Whistler) 
{
  BlowEffect(Whistler);
  Whistler->EditAP(-1000);
  return true;
} 

void whistle::BlowEffect(character* Whistler)
{
  if(Whistler->IsPlayer())
    ADD_MESSAGE("You produce an interesting sound.");
  else if(Whistler->CanBeSeenByPlayer())
    ADD_MESSAGE("%s blows %s and produces an interesting sound.", Whistler->CHARNAME(DEFINITE));
  else 
    ADD_MESSAGE("You hear a whistle playing.");

  for(ushort c = 0; c < game::GetTeams(); ++c)
    {
      if(game::GetTeam(c)->HasEnemy())
	for(std::list<character*>::const_iterator i = game::GetTeam(c)->GetMember().begin(); i != game::GetTeam(c)->GetMember().end(); ++i)
	  if((*i)->IsEnabled())
	    {
	      ulong ThisDistance = GetHypotSquare(long((*i)->GetPos().X) - GetPos().X, long((*i)->GetPos().Y) - GetPos().Y);

	      if(ThisDistance <= GetRange())
		(*i)->SetWayPoint(GetPos());
	    }
    }
}

void magicalwhistle::BlowEffect(character* Whistler)
{
  if(Whistler->IsPlayer())
    ADD_MESSAGE("You produce a peculiar sound.");
  else if(Whistler->CanBeSeenByPlayer())
    ADD_MESSAGE("%s blows %s and produces a peculiar sound.", Whistler->CHARNAME(DEFINITE));
  else 
    ADD_MESSAGE("You hear a strange tune playing.");

  const std::list<character*>& Member = Whistler->GetTeam()->GetMember();

  for(std::list<character*>::const_iterator i = Member.begin(); i != Member.end(); ++i)
    if(Whistler != *i)
      (*i)->TeleportNear(Whistler);
}

void chest::VirtualConstructor(bool Load)
{
  Contained = new stack(0, this, HIDDEN, true);

  if(!Load)
    {
      SetLockType(RAND() % NUMBER_OF_LOCK_TYPES);
      SetIsLocked(RAND() % 3 != 0);
      ulong ItemNumber = RAND() % (GetMaxGeneratedContainedItems() + 1);

      for(ushort c = 0; c < ItemNumber; ++c)
	{
	  item* NewItem = protosystem::BalancedCreateItem();

	  if(NewItem->CanBeGeneratedInContainer() && HowManyFits(NewItem))
	    {
	      GetContained()->AddItem(NewItem);
	      NewItem->SpecialGenerationHandler();
	    }
	  else
	    delete NewItem;
	}
    }
}

bool chest::TryKey(item* Key, character* Applier)
{
  if(GetLockType() == DAMAGED)
    {
      ADD_MESSAGE("The lock is broken.");
      return true;
    }

  if(Key->CanOpenLockType(GetLockType()))
    {
      if(IsLocked())
	{
	  if(Applier->IsPlayer())
	    ADD_MESSAGE("You unlock %s.", CHARDESCRIPTION(DEFINITE));
	  else if(Applier->CanBeSeenByPlayer())
	    ADD_MESSAGE("%s unlocks %s.", Applier->CHARNAME(DEFINITE), CHARDESCRIPTION(DEFINITE));
	}
      else
	{
	  if(Applier->IsPlayer())
	    ADD_MESSAGE("You lock %s.", CHARDESCRIPTION(DEFINITE));
	  else if(Applier->CanBeSeenByPlayer())
	    ADD_MESSAGE("%s locks %s.", Applier->CHARNAME(DEFINITE), CHARDESCRIPTION(DEFINITE));
	}

      SetIsLocked(!IsLocked());
    }
  else
    {
      if(Applier->IsPlayer())
	ADD_MESSAGE("%s doesn't fit in the lock.", Key->CHARNAME(DEFINITE));
      else if(Applier->CanBeSeenByPlayer())
	ADD_MESSAGE("%s tries to fit %s in the lock, but fails.", Applier->CHARNAME(DEFINITE), Key->CHARNAME(DEFINITE));
    }

  return true;
}

void materialcontainer::GenerateMaterials()
{
  ushort Chosen = RandomizeMaterialConfiguration();
  InitChosenMaterial(MainMaterial, GetMainMaterialConfig(), GetDefaultMainVolume(), Chosen);
  InitChosenMaterial(ContainedMaterial, GetContainedMaterialConfig(), GetDefaultContainedVolume(), Chosen);
}

void meleeweapon::GenerateMaterials()
{
  ushort Chosen = RandomizeMaterialConfiguration();
  InitChosenMaterial(MainMaterial, GetMainMaterialConfig(), GetDefaultMainVolume(), Chosen);
  InitChosenMaterial(SecondaryMaterial, GetSecondaryMaterialConfig(), GetDefaultSecondaryVolume(), Chosen);
  InitChosenMaterial(ContainedMaterial, GetContainedMaterialConfig(), GetDefaultContainedVolume(), Chosen);
}

bool arm::ApplyExperience()
{
  if(!IsAlive())
    return false;

  bool Edited = false;

  if(GetMaster()->CheckForAttributeIncrease(Strength, StrengthExperience))
    {
      if(GetMaster()->IsPlayer())
	ADD_MESSAGE("Your %s feels stronger!", GetBodyPartName().c_str());
      else if(CanBeSeenByPlayer())
	ADD_MESSAGE("Suddenly %s looks stronger.", GetMaster()->CHARNAME(DEFINITE));

      Edited = true;
    }
  else if(GetMaster()->CheckForAttributeDecrease(Strength, StrengthExperience))
    {
      if(GetMaster()->IsPlayer())
	ADD_MESSAGE("Your %s feels weaker.", GetBodyPartName().c_str());
      else if(CanBeSeenByPlayer())
	ADD_MESSAGE("Suddenly %s looks weaker.", GetMaster()->CHARNAME(DEFINITE));

      Edited = true;
    }

  if(GetMaster()->CheckForAttributeIncrease(Dexterity, DexterityExperience))
    {
      if(GetMaster()->IsPlayer())
	ADD_MESSAGE("Your %s feels quite dextrous.", GetBodyPartName().c_str());
      else if(CanBeSeenByPlayer())
	ADD_MESSAGE("Suddenly %s looks quite dextrous.", GetMaster()->CHARNAME(DEFINITE));

      Edited = true;
    }
  else if(GetMaster()->CheckForAttributeDecrease(Dexterity, DexterityExperience))
    {
      if(GetMaster()->IsPlayer())
	ADD_MESSAGE("Your %s feels clumsy.", GetBodyPartName().c_str());
      else if(CanBeSeenByPlayer())
	ADD_MESSAGE("Suddenly %s looks clumsy.", GetMaster()->CHARNAME(DEFINITE));

      Edited = true;
    }

  return Edited;
}

bool leg::ApplyExperience()
{
  if(!IsAlive())
    return false;

  bool Edited = false;

  if(GetMaster()->CheckForAttributeIncrease(Strength, StrengthExperience))
    {
      if(GetMaster()->IsPlayer())
	ADD_MESSAGE("Your %s feels stronger!", GetBodyPartName().c_str());
      else if(CanBeSeenByPlayer())
	ADD_MESSAGE("Suddenly %s looks stronger.", GetMaster()->CHARNAME(DEFINITE));

      GetMaster()->CalculateBurdenState();
      Edited = true;
    }
  else if(GetMaster()->CheckForAttributeDecrease(Strength, StrengthExperience))
    {
      if(GetMaster()->IsPlayer())
	ADD_MESSAGE("Your %s feels weaker.", GetBodyPartName().c_str());
      else if(CanBeSeenByPlayer())
	ADD_MESSAGE("Suddenly %s looks weaker.", GetMaster()->CHARNAME(DEFINITE));

      GetMaster()->CalculateBurdenState();
      Edited = true;
    }

  if(GetMaster()->CheckForAttributeIncrease(Agility, AgilityExperience))
    {
      if(GetMaster()->IsPlayer())
	ADD_MESSAGE("Your %s feels very agile!", GetBodyPartName().c_str());
      else if(CanBeSeenByPlayer())
	ADD_MESSAGE("Suddenly %s looks very agile.", GetMaster()->CHARNAME(DEFINITE));

      Edited = true;
    }
  else if(GetMaster()->CheckForAttributeDecrease(Agility, AgilityExperience))
    {
      if(GetMaster()->IsPlayer())
	ADD_MESSAGE("Your %s feels slower.", GetBodyPartName().c_str());
      else if(CanBeSeenByPlayer())
	ADD_MESSAGE("Suddenly %s looks sluggish.", GetMaster()->CHARNAME(DEFINITE));

      Edited = true;
    }

  return Edited;
}

void arm::Hit(character* Enemy)
{
  switch(Enemy->TakeHit(GetMaster(), GetWielded() ? GetWielded() : GetGauntlet(), GetDamage(), GetToHitValue(), RAND() % 26 - RAND() % 26, GetWielded() ? WEAPONATTACK : UNARMEDATTACK, !(RAND() % GetMaster()->GetCriticalModifier())))
    {
    case HASHIT:
    case HASBLOCKED:
    case HASDIED:
    case DIDNODAMAGE:
      EditExperience(ARMSTRENGTH, 50);
    case HASDODGED:
      EditExperience(DEXTERITY, 25);
    }
}

ushort arm::GetAttribute(ushort Identifier) const
{
  if(Identifier == ARMSTRENGTH)
    {
      if(IsAlive())
	return Strength + StrengthBonus;
      else
	return GetMainMaterial()->GetStrengthValue() + StrengthBonus;
    }
  else if(Identifier == DEXTERITY)
    {
      if(IsAlive())
	return Dexterity + DexterityBonus;
      else
	return GetMainMaterial()->GetFlexibility() + DexterityBonus;
    }
  else
    {
      ABORT("Illegal arm attribute %d request!", Identifier);
      return 0xACCA;
    }
}

bool arm::EditAttribute(ushort Identifier, short Value)
{
  if(Identifier == ARMSTRENGTH)
    return IsAlive() && GetMaster()->RawEditAttribute(Strength, Value);
  else if(Identifier == DEXTERITY)
    return IsAlive() && GetMaster()->RawEditAttribute(Dexterity, Value);
  else
    {
      ABORT("Illegal arm attribute %d edit request!", Identifier);
      return false;
    }
}

void arm::EditExperience(ushort Identifier, long Value)
{
  if(Identifier == ARMSTRENGTH)
    {
      if(IsAlive())
	StrengthExperience += Value << 1;
    }
  else if(Identifier == DEXTERITY)
    {
      if(IsAlive())
	DexterityExperience += Value << 1;
    }
  else
    ABORT("Illegal arm attribute %d experience edit request!", Identifier);
}

ushort leg::GetAttribute(ushort Identifier) const
{
  if(Identifier == LEGSTRENGTH)
    {
      if(IsAlive())
	return Strength + StrengthBonus;
      else
	return GetMainMaterial()->GetStrengthValue() + StrengthBonus;
    }
  else if(Identifier == AGILITY)
    {
      if(IsAlive())
	return Agility + AgilityBonus;
      else
	return GetMainMaterial()->GetFlexibility() + AgilityBonus;
    }
  else
    {
      ABORT("Illegal leg attribute %d request!", Identifier);
      return 0xECCE;
    }
}

bool leg::EditAttribute(ushort Identifier, short Value)
{
  if(Identifier == LEGSTRENGTH)
    return IsAlive() && GetMaster()->RawEditAttribute(Strength, Value);
  else if(Identifier == AGILITY)
    return IsAlive() && GetMaster()->RawEditAttribute(Agility, Value);
  else
    {
      ABORT("Illegal leg attribute %d edit request!", Identifier);
      return false;
    }
}

void leg::EditExperience(ushort Identifier, long Value)
{
  if(Identifier == LEGSTRENGTH)
    {
      if(IsAlive())
	StrengthExperience += Value << 1;
    }
  else if(Identifier == AGILITY)
    {
      if(IsAlive())
	AgilityExperience += Value << 1;
    }
  else
    ABORT("Illegal leg attribute %d experience edit request!", Identifier);
}

void arm::RaiseStats()
{
  Strength += 10;
  Dexterity += 10;
}

void arm::LowerStats()
{
  Strength -= 10;
  Dexterity -= 10;
}

void leg::RaiseStats()
{
  Strength += 10;
  Agility += 10;
}

void leg::LowerStats()
{
  Strength -= 10;
  Agility -= 10;
}

/* Returns true if chest opens fine else false */

bool chest::Open(character* Opener)
{
  if(IsLocked())
    {
      ADD_MESSAGE("The chest seems to be locked.");
      return false;
    }

  std::string Question = "Do you want to (t)ake something from or (p)ut something in this chest? [t,p]";

  switch(game::KeyQuestion(Question, KEYESC, 3, 't', 'p', KEYESC))
    {
    case 't':
      return TakeSomethingFrom(Opener);
    case 'p':
      return PutSomethingIn(Opener);
    default:
      return false;
    }
}

bool chest::TakeSomethingFrom(character* Opener)
{
  if(!GetContained()->GetItems())
    {
      ADD_MESSAGE("There is nothing in %s.", CHARNAME(DEFINITE));
      return false;
    }

  bool Success = false;
  uchar RoomNumber = GetLSquareUnder()->GetRoom();
  stack::SetSelected(0);

  for(;;)
    {
      std::vector<item*> ToTake;
      game::DrawEverythingNoBlit();
      GetContained()->DrawContents(ToTake, Opener, "What do you want to take from " + GetName(DEFINITE) + "?", REMEMBER_SELECTED);

      if(ToTake.empty())
	break;

      if(!IsOnGround() || !RoomNumber || GetLevelUnder()->GetRoom(RoomNumber)->PickupItem(Opener, ToTake[0], ToTake.size()))
	{
	  for(ushort c = 0; c < ToTake.size(); ++c)
	    ToTake[c]->MoveTo(Opener->GetStack());

	  ADD_MESSAGE("You take %s from %s.", ToTake[0]->GetName(DEFINITE, ToTake.size()).c_str(), CHARNAME(DEFINITE));
	  Success = true;
	}
    }

  if(Success)
    {
      Opener->DexterityAction(Opener->OpenMultiplier() * 5);
      return true;
    }
  else
    return false;
}

bool chest::PutSomethingIn(character* Opener)
{
  if(!Opener->GetStack()->GetItems())
    {
      ADD_MESSAGE("You have nothing to put in %s.", CHARNAME(DEFINITE));
      return false;
    }

  bool Success = false;
  uchar RoomNumber = GetLSquareUnder()->GetRoom();
  stack::SetSelected(0);

  for(;;)
    {
      std::vector<item*> ToPut;
      game::DrawEverythingNoBlit();
      Opener->GetStack()->DrawContents(ToPut, Opener, "What do you want to put in " + GetName(DEFINITE) + "?", REMEMBER_SELECTED);

      if(ToPut.empty())
	break;

      if(ToPut[0]->GetID() == GetID())
	{
	  ADD_MESSAGE("You can't put %s inside itself!", CHARNAME(DEFINITE));
	  continue;
	}

      ushort Amount = Min<ushort>(HowManyFits(ToPut[0]), ToPut.size());

      if(!Amount)
	{
	  if(ToPut.size() == 1)
	    ADD_MESSAGE("%s doesn't fit in %s.", ToPut[0]->CHARNAME(DEFINITE), CHARNAME(DEFINITE));
	  else
	    ADD_MESSAGE("None of the %d %s fits in %s.", ToPut.size(), ToPut[0]->CHARNAME(PLURAL), CHARNAME(DEFINITE));

	  continue;
	}

      if(Amount != ToPut.size())
	ADD_MESSAGE("Only %d of the %d %s fit%s in %s.", Amount, ToPut.size(), ToPut[0]->CHARNAME(PLURAL), Amount == 1 ? "s" : "", CHARNAME(DEFINITE));

      if(!IsOnGround() || !RoomNumber || GetLevelUnder()->GetRoom(RoomNumber)->DropItem(Opener, ToPut[0], Amount))
	{
	  for(ushort c = 0; c < Amount; ++c)
	    ToPut[c]->MoveTo(GetContained());

	  ADD_MESSAGE("You put %s in %s.", ToPut[0]->GetName(DEFINITE, Amount).c_str(), CHARNAME(DEFINITE));
	  Success = true;
	}
    }

  if(Success)
    {
      Opener->DexterityAction(Opener->OpenMultiplier() * 5);
      return true;
    }
  else
    return false;
}

void chest::Save(outputfile& SaveFile) const
{
  item::Save(SaveFile);
  GetContained()->Save(SaveFile);
  SaveFile << LockType << Locked;
}

void chest::Load(inputfile& SaveFile)
{
  item::Load(SaveFile);
  GetContained()->Load(SaveFile);
  SaveFile >> LockType >> Locked;
}

bool chest::Polymorph(stack* CurrentStack)
{
  GetContained()->MoveItemsTo(CurrentStack);
  item::Polymorph(CurrentStack);
  return true;
}

ushort chest::HowManyFits(item* ToBePut) const
{
  return (GetStorageVolume() - GetContained()->GetVolume()) / ToBePut->GetVolume();
}

chest::~chest()
{
  delete Contained;
}

const std::string& key::GetAdjective() const
{
  return game::GetLockDescription(LockType);
}

void head::InitSpecialAttributes()
{
  bodypart::InitSpecialAttributes();
  BaseBiteStrength = Master->GetBaseBiteStrength();
}

void arm::InitSpecialAttributes()
{
  bodypart::InitSpecialAttributes();
  Strength = Master->GetDefaultArmStrength() * (100 + Master->GetAttributeBonus()) / 100;
  Dexterity = Master->GetDefaultDexterity() * (100 + Master->GetAttributeBonus()) / 100;
  BaseUnarmedStrength = Master->GetBaseUnarmedStrength();
}

void leg::InitSpecialAttributes()
{
  bodypart::InitSpecialAttributes();
  Strength = Master->GetDefaultLegStrength() * (100 + Master->GetAttributeBonus()) / 100;
  Agility = Master->GetDefaultAgility() * (100 + Master->GetAttributeBonus()) / 100;
  BaseKickStrength = Master->GetBaseKickStrength();
}

std::string corpse::GetConsumeVerb() const
{
  return GetDeceased()->GetTorso()->GetConsumeVerb();
}

/* Victim is of course the stuck person, Bodypart is the index of the bodypart that the trap is stuck to and the last vector2d is just a direction vector that may - or may not - be used in the future. This function returns true if the character manages to escape */

bool beartrap::TryToUnstuck(character* Victim, ushort BodyPart, vector2d)
{
  if(!(RAND() % 3))
    {
      if(Victim->IsPlayer())
	ADD_MESSAGE("You manage to hurt your %s even more.", Victim->GetBodyPartName(BodyPart).c_str());
      else if(Victim->CanBeSeenByPlayer())
	ADD_MESSAGE("%s hurts %s %s more with %s.", Victim->CHARNAME(DEFINITE), Victim->GetPossessivePronoun().c_str(), Victim->GetBodyPartName(BodyPart).c_str(), CHARNAME(DEFINITE));

      Victim->ReceiveBodyPartDamage(0, 2 + RAND() % 2, PHYSICALDAMAGE, BodyPart, YOURSELF, false, false, false);
      Victim->CheckDeath("died while trying to escape from " + GetName(DEFINITE));
      return false;
    }

  if(!(RAND() % 5))
    {
      Victim->SetStuckTo(0);
      Victim->SetStuckToBodyPart(NONEINDEX);

      if(Victim->IsPlayer())
	ADD_MESSAGE("You manage to free yourself from %s.", CHARNAME(DEFINITE));
      else if(Victim->CanBeSeenByPlayer())
	ADD_MESSAGE("%s manages to free %sself from %s.", Victim->CHARNAME(DEFINITE), Victim->CHARPERSONALPRONOUN, CHARNAME(DEFINITE));

      return true;
    }

  if(Victim->IsPlayer())
    ADD_MESSAGE("You are unable to escape from %s.", CHARNAME(DEFINITE));

  return false;
}

void beartrap::Load(inputfile& SaveFile)
{
  item::Load(SaveFile);
  SaveFile >> Active >> Team;
}

void beartrap::Save(outputfile& SaveFile) const
{
  item::Save(SaveFile);
  SaveFile << Active << Team;
}

void beartrap::VirtualConstructor(bool)
{
  Active = false; /* this must be false */
}

void beartrap::StepOnEffect(character* Stepper)
{
  if(IsActive() && !Stepper->IsStuck())
    {
      ushort StepperBodyPart = Stepper->GetRandomStepperBodyPart();

      if(StepperBodyPart == NONEINDEX)
	return;

      Stepper->SetStuckTo(this);
      Stepper->SetStuckToBodyPart(StepperBodyPart);

      if(Stepper->IsPlayer())
	ADD_MESSAGE("You step in %s and it traps your %s.", CHARNAME(INDEFINITE), Stepper->GetBodyPartName(StepperBodyPart).c_str());
      else if(Stepper->CanBeSeenByPlayer())
	ADD_MESSAGE("%s is trapped in %s.", Stepper->CHARNAME(DEFINITE), CHARNAME(INDEFINITE));

      SetIsActive(false);
      Stepper->ReceiveBodyPartDamage(0, 3 + RAND() % 3, PHYSICALDAMAGE, Stepper->GetStuckToBodyPart(), YOURSELF, false, false, false);
      Stepper->CheckDeath("died stepping to " + GetName(INDEFINITE));
      GetLSquareUnder()->SendMemorizedUpdateRequest();
      GetLSquareUnder()->SendNewDrawRequest();
    }
}

bool beartrap::CheckPickUpEffect(character* Picker)
{
  if(Picker->IsStuck() && Picker->GetStuckTo()->GetID() == GetID())
    {
      if(Picker->IsPlayer())
	ADD_MESSAGE("%s is tightly stuck to your %s.", CHARNAME(DEFINITE), Picker->GetBodyPartName(Picker->GetStuckToBodyPart()).c_str());

      return false;
    }

  SetIsActive(false);
  return true;
}

bool wandofdoorcreation::Zap(character* Zapper, vector2d, uchar Direction)
{
  if(GetCharges() <= GetTimesUsed())
    {
      ADD_MESSAGE("Nothing happens.");
      return true;
    }

  vector2d Pos[3];
  Pos[0] = game::GetMoveVector(Direction);

  switch(Direction)
    {
    case 0:
      Pos[1] = vector2d(-1, 0);
      Pos[2] = vector2d(0, -1);
      break;
    case 1:
      Pos[1] = vector2d(-1, -1);
      Pos[2] = vector2d(1, -1);
      break;
    case 2:
      Pos[1] = vector2d(0, -1);
      Pos[2] = vector2d(1, 0);
      break;
    case 3:
      Pos[1] = vector2d(-1, -1);
      Pos[2] = vector2d(-1, 1);
      break;
    case 4:
      Pos[1] = vector2d(1, -1);
      Pos[2] = vector2d(1, 1);
      break;
    case 5:
      Pos[1] = vector2d(-1, 0);
      Pos[2] = vector2d(0, 1);
      break;
    case 6:
      Pos[1] = vector2d(-1, 1);
      Pos[2] = vector2d(1, 1);
      break;
    case 7:
      Pos[1] = vector2d(0, 1);
      Pos[2] = vector2d(1, 0);
      break;
    case 8:
      ADD_MESSAGE("You suddenly feel like a door.");
      return false;
    }

  bool Succeeded = false;

  for(ushort c = 0; c < 3; ++c)
    if(GetAreaUnder()->IsValidPos(Zapper->GetPos() + Pos[c]))
      {
	lsquare* Square = GetNearLSquare(Zapper->GetPos() + Pos[c]);

	if(Square->GetOLTerrain()->IsSafeToDestroy() && !Square->GetCharacter())
	  {
	    door* Door = new door(0, NOMATERIALS);
	    Door->InitMaterials(MAKE_MATERIAL(IRON));
	    Door->Lock();
	    Square->ChangeOLTerrainAndUpdateLights(Door);
	    Succeeded = true;
	  }
      }

  if(!Succeeded)
    {
      ADD_MESSAGE("The spell is wasted.");
      return true;
    }

  SetTimesUsed(GetTimesUsed() + 1);
  return true;
}

void bodypart::SignalEquipmentAdd(gearslot* Slot)
{
  ApplyEquipmentAttributeBonuses(Slot->GetItem());

  if(GetMaster())
    GetMaster()->SignalEquipmentAdd(Slot->GetEquipmentIndex());
}

void bodypart::SignalEquipmentRemoval(gearslot* Slot)
{
  CalculateAttributeBonuses();

  if(GetMaster())
    GetMaster()->SignalEquipmentRemoval(Slot->GetEquipmentIndex());
}

bool beartrap::IsPickable(character* Picker) const
{
  return !IsActive() && (!Picker->GetStuckTo() || Picker->GetStuckTo()->GetID() != GetID());
}

void bodypart::Mutate()
{
  GetMainMaterial()->SetVolume(ulong(GetVolume() * (1.5f - float(RAND() % 1000) / 1000)));
}

void leg::Mutate()
{
  bodypart::Mutate();
  SetAgility(ushort(GetAgility() * (1.5f - float(RAND() % 1000) / 1000)));
  SetStrength(ushort(GetStrength() * (1.5f - float(RAND() % 1000) / 1000)));
}

void arm::Mutate()
{
  bodypart::Mutate();
  SetStrength(ulong(GetStrength() * (1.5f - float(RAND() % 1000) / 1000)));
  SetDexterity(ulong(GetDexterity() * (1.5f - float(RAND() % 1000) / 1000)));
}

arm* rightarm::GetPairArm() const
{
  if(GetHumanoidMaster())
    return GetHumanoidMaster()->GetLeftArm();
  else
    return 0;
}

arm* leftarm::GetPairArm() const
{
  if(GetHumanoidMaster())
    return GetHumanoidMaster()->GetRightArm();
  else
    return 0;
}

sweaponskill* rightarm::GetCurrentSWeaponSkill() const
{
  return GetHumanoidMaster()->GetCurrentRightSWeaponSkill();
}

sweaponskill* leftarm::GetCurrentSWeaponSkill() const
{
  return GetHumanoidMaster()->GetCurrentLeftSWeaponSkill();
}

void can::GenerateLeftOvers(character* Eater)
{
  ChangeConsumeMaterial(0);

  if(!game::IsInWilderness() && (!Eater->IsPlayer() || configuration::GetAutoDropLeftOvers()))
    MoveTo(Eater->GetStackUnder());
  else
    MoveTo(Eater->GetStack());
}

uchar bodypart::GetMaxAlpha(ushort) const
{
  if(GetMaster() && GetMaster()->StateIsActivated(INVISIBLE))
    return 150;
  else
    return 255;
}

bool wandofinvisibility::BeamEffect(character*, const std::string&, uchar, lsquare* Where) 
{
  if(Where->GetCharacter())
    Where->GetCharacter()->BeginTemporaryState(INVISIBLE, 1000 + RAND() % 1001);

  return false;
}

uchar lantern::GetSpecialFlags() const
{
  switch(SquarePosition)
    {
    case RIGHT:
      return ROTATE;
    case DOWN:
      return FLIP;
    case LEFT:
      return ROTATE|MIRROR;
    default:
      return 0;
    }
}

void bodypart::AddPostFix(std::string& String) const
{
  if(OwnerDescription.length())
    String << ' ' << OwnerDescription;
}

bool stethoscope::Apply(character* Doctor) 
{
  if(!Doctor->IsPlayer())
    ABORT("Doctor is not here, man, but these pills taste just as good anyway.");

  uchar Dir = game::DirectionQuestion("What do you want to inspect? [press a direction key]", false,true);  

  if(Dir == DIR_ERROR)
    return false;

  Doctor->DexterityAction(2);
  return ListenTo(GetNearLSquare(GetPos() + game::GetMoveVector(Dir)), Doctor);
} 

/* Returns true if successful else false */

bool stethoscope::ListenTo(lsquare* Square, character* Listener)
{
  if(!Listener->CanUseStethoscope(true))
    return false;

  character* Char = Square->GetCharacter();
  if(!Char)
    {
      if(Listener->IsPlayer())
	ADD_MESSAGE("There's no-one here.");
      return false;
    }


  Char->DisplayStethoscopeInfo(Listener);
  return true;
}

void chest::CalculateVolumeAndWeight()
{
  item::CalculateVolumeAndWeight();
  Volume += Contained->GetVolume();
  Weight += Contained->GetWeight();
}

void corpse::CalculateVolumeAndWeight()
{
  Volume = Deceased->GetVolume();
  Weight = Deceased->GetWeight();
}

item* head::GetEquipment(ushort Index) const
{
  switch(Index)
    {
    case 0: return GetHelmet();
    case 1: return GetAmulet();
    default: return 0;
    }
}

item* humanoidtorso::GetEquipment(ushort Index) const
{
  switch(Index)
    {
    case 0: return GetBodyArmor();
    case 1: return GetCloak();
    case 2: return GetBelt();
    default: return 0;
    }
}

item* arm::GetEquipment(ushort Index) const
{
  switch(Index)
    {
    case 0: return GetWielded();
    case 1: return GetGauntlet();
    case 2: return GetRing();
    default: return 0;
    }
}

item* leg::GetEquipment(ushort Index) const
{
  if(!Index)
    return GetBoot();
  else
    return 0;
}

void bodypart::CalculateVolumeAndWeight()
{
  item::CalculateVolumeAndWeight();
  CarriedWeight = 0;
  BodyPartVolume = Volume;

  for(ushort c = 0; c < GetEquipmentSlots(); ++c)
    if(GetEquipment(c))
      {
	Volume += GetEquipment(c)->GetVolume();
	CarriedWeight += GetEquipment(c)->GetWeight();
      }

  Weight += CarriedWeight;
}

void corpse::CalculateEmitation()
{
  Emitation = Deceased->GetEmitation();
}

void bodypart::CalculateEmitation()
{
  item::CalculateEmitation();

  for(ushort c = 0; c < GetEquipmentSlots(); ++c)
    if(GetEquipment(c))
      game::CombineLights(Emitation, GetEquipment(c)->GetEmitation());
}

void bodypart::CalculateMaxHP()
{
  MaxHP = 0;

  if(GetMaster())
    {
      if(IsAlive())
	MaxHP = GetBodyPartVolume() * GetMaster()->GetAttribute(ENDURANCE) / 10000;
      else
	MaxHP = GetBodyPartVolume() * GetMainMaterial()->GetStrengthValue() / 10000;

      if(MaxHP < 1)
	MaxHP = 1;
    }
}

void bodypart::SignalVolumeAndWeightChange()
{
  item::SignalVolumeAndWeightChange();

  if(GetMaster() && !GetMaster()->IsInitializing())
    {
      CalculateMaxHP();
      GetMaster()->CalculateMaxHP();
    }
}

void bodypart::SetHP(short What)
{
  HP = What;

  if(GetMaster())
    GetMaster()->CalculateHP();
}

void bodypart::EditHP(short What)
{
  HP += What;

  if(GetMaster())
    GetMaster()->CalculateHP();
}

void arm::AddAttackInfo(felist& List) const
{
  if(GetDamage())
    {
      std::string Entry = "   ";

      if(GetWielded())
	{
	  GetWielded()->AddName(Entry, UNARTICLED);

	  if(TwoHandWieldIsActive())
	    Entry << " (b)";
	}
      else
	Entry << "melee attack";

      Entry.resize(50, ' ');
      Entry << GetMinDamage() << '-' << GetMaxDamage();
      Entry.resize(60, ' ');
      Entry << int(GetToHitValue());
      Entry.resize(70, ' ');
      Entry << GetAPCost();
      List.AddEntry(Entry, LIGHTGRAY);
    }
}

void arm::AddDefenceInfo(felist& List) const
{
  if(GetWielded())
    {
      std::string Entry = "   ";
      GetWielded()->AddName(Entry, UNARTICLED);
      Entry.resize(50, ' ');
      Entry << int(GetBlockValue());
      Entry.resize(70, ' ');
      Entry << GetBlockCapability();
      List.AddEntry(Entry, LIGHTGRAY);
    }
}

void arm::SignalVolumeAndWeightChange()
{
  bodypart::SignalVolumeAndWeightChange();

  if(GetMaster() && !GetMaster()->IsInitializing())
    CalculateAttackInfo();
}

void bodypart::CalculateAttackInfo()
{
  CalculateDamage();
  CalculateToHitValue();
  CalculateAPCost();
}

bool flamingsword::HitEffect(character* Enemy, character* Hitter, uchar BodyPartIndex, uchar Direction, bool BlockedByArmour)
{
  bool BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, BodyPartIndex, Direction, BlockedByArmour);

  if(Enemy->IsEnabled() && RAND() & 1)
    {
      if(Enemy->IsPlayer() || Hitter->IsPlayer() || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer())
	ADD_MESSAGE("%s sword burns %s.", Hitter->CHARPOSSESSIVEPRONOUN, Enemy->CHARDESCRIPTION(DEFINITE));

      return Enemy->ReceiveBodyPartDamage(Hitter, 2 + (RAND() & 3), FIRE, BodyPartIndex, Direction) != 0 || BaseSuccess;
    }
  else
    return BaseSuccess;
}

bool arm::TwoHandWieldIsActive() const
{
  if(GetWielded()->IsTwoHanded())
    {
      arm* PairArm = GetPairArm();
      return PairArm && !PairArm->GetWielded();
    }
  else
    return false;
}

float bodypart::GetTimeToDie(ushort Damage, float ToHitValue, float DodgeValue, bool AttackIsBlockable, bool UseMaxHP) const
{
  float Durability;
  short TrueDamage = Damage - GetTotalResistance(PHYSICALDAMAGE);
  short HP = UseMaxHP ? GetMaxHP() : GetHP();

  if(TrueDamage > 0)
    {
      float AverageDamage;

      if(AttackIsBlockable)
	{
	  blockvector Block;
	  Master->CreateBlockPossibilityVector(Block, ToHitValue);

	  if(Block.size())
	    {
	      float ChanceForNoBlock = 1.0f;
	      AverageDamage = 0;

	      for(ushort c = 0; c < Block.size(); ++c)
		{
		  ChanceForNoBlock -= Block[c].first;

		  if(TrueDamage - Block[c].second > 0)
		    AverageDamage += Block[c].first * (TrueDamage - Block[c].second);
		}

	      AverageDamage += ChanceForNoBlock * TrueDamage;
	    }
	  else
	    AverageDamage = TrueDamage;
	}
      else
	AverageDamage = TrueDamage;

      Durability = HP / (AverageDamage * GetRoughChanceToHit(ToHitValue, DodgeValue));

      if(Durability < 1)
	Durability = 1;

      if(Durability > 1000)
	Durability = 1000;
    }
  else
    Durability = 1000;

  return Durability;
}

float bodypart::GetRoughChanceToHit(float ToHitValue, float DodgeValue) const
{
  return GLOBAL_WEAK_BODYPART_HIT_MODIFIER * ToHitValue * GetBodyPartVolume() / ((DodgeValue / ToHitValue + 1) * DodgeValue * Master->GetBodyVolume() * 100);
}

float torso::GetRoughChanceToHit(float ToHitValue, float DodgeValue) const
{
  return 1 / (DodgeValue / ToHitValue + 1);
}

materialcontainer::~materialcontainer()
{
  delete ContainedMaterial;
}

meleeweapon::~meleeweapon()
{
  delete SecondaryMaterial;
  delete ContainedMaterial;
}

bool mjolak::HitEffect(character* Enemy, character* Hitter, uchar BodyPartIndex, uchar Direction, bool BlockedByArmour)
{
  bool BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, BodyPartIndex, Direction, BlockedByArmour);

  if(!(IsBroken()) && Enemy->IsEnabled() && !(RAND() % 5))
    {
      if(Enemy->IsPlayer() || Hitter->IsPlayer() || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer())
	ADD_MESSAGE("A burst of %s Mjolak's unholy energy fries %s.", Hitter->CHARPOSSESSIVEPRONOUN, Enemy->CHARDESCRIPTION(DEFINITE));

      return Enemy->ReceiveBodyPartDamage(Hitter, 5 + (RAND() % 6), ENERGY, BodyPartIndex, Direction) != 0 || BaseSuccess;
    }
  else
    return BaseSuccess;
}

bool vermis::HitEffect(character* Enemy, character* Hitter, uchar BodyPartIndex, uchar Direction, bool BlockedByArmour)
{
  bool BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, BodyPartIndex, Direction, BlockedByArmour);

  if(!(IsBroken()) && Enemy->IsEnabled() && !(RAND() % 5))
    {
      if(Enemy->IsPlayer() || Enemy->CanBeSeenByPlayer())
	ADD_MESSAGE("%s Vermis sends %s on a sudden journey.", Hitter->CHARPOSSESSIVEPRONOUN, Enemy->CHARDESCRIPTION(DEFINITE));

      Enemy->TeleportRandomly();
      return true;
    }
  else
    return BaseSuccess;
}

bool turox::HitEffect(character* Enemy, character* Hitter, uchar BodyPartIndex, uchar Direction, bool BlockedByArmour)
{
  bool BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, BodyPartIndex, Direction, BlockedByArmour);

  if(!(IsBroken()) && Enemy->IsEnabled() && !(RAND() % 5))
    {
      if(Enemy->IsPlayer() || Hitter->IsPlayer() || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer())
	ADD_MESSAGE("%s smash%s %s with the full force of Turox.", Hitter->CHARPERSONALPRONOUN, Hitter->IsPlayer() ? "" : "es", Enemy->CHARDESCRIPTION(DEFINITE));

      std::string DeathMSG = "killed by " + Enemy->GetName(DEFINITE); 
      Enemy->GetLevelUnder()->Explosion(Hitter, DeathMSG, Enemy->GetPos(), 20 + RAND() % 5 - RAND() % 5);
      return true;
    }
  else
    return BaseSuccess;
}

bool whipofcleptia::HitEffect(character* Enemy, character* Hitter, uchar BodyPartIndex, uchar Direction, bool BlockedByArmour)
{
  bool BaseSuccess = meleeweapon::HitEffect(Enemy, Hitter, BodyPartIndex, Direction, BlockedByArmour);

  if(!(IsBroken()) && Enemy->IsEnabled() && CleptiaHelps(Enemy, Hitter))
    {
      if(Enemy->IsPlayer() || Hitter->IsPlayer() || Enemy->CanBeSeenByPlayer() || Hitter->CanBeSeenByPlayer())
	ADD_MESSAGE("%s whip asks for the help of Cleptia as it steals %s %s.", Hitter->CHARPOSSESSIVEPRONOUN, Enemy->CHARPOSSESSIVEPRONOUN, Enemy->GetMainWielded()->CHARNAME(UNARTICLED));

      if(Hitter->IsPlayer())
	{
	  game::DoEvilDeed(10);
	  game::GetGod(10)->AdjustRelation(10);
	}

      Enemy->GetMainWielded()->MoveTo(Hitter->GetStackUnder());
      return true;
    }
  else
    return BaseSuccess;
}

void bodypart::RandomizePosition()
{
  SpecialFlags |= 1 + RAND() % 7;
  UpdatePictures();
}

void bodypart::ResetPosition()
{
  SpecialFlags &= ~0x7;
  UpdatePictures();
}

bool chest::ContentsCanBeSeenBy(const character* Viewer) const
{
  return GetMainMaterial()->IsTransparent() && CanBeSeenBy(Viewer);
}

bool mine::CanBeSeenBy(const character* Viewer) const 
{ 
  return (!IsActive() || (Viewer->GetTeam()->GetID() == Team)) && materialcontainer::CanBeSeenBy(Viewer); 
}

bool beartrap::CanBeSeenBy(const character* Viewer) const 
{ 
  return (!IsActive() || (Viewer->GetTeam()->GetID() == Team)) && item::CanBeSeenBy(Viewer); 
}

bool mine::Apply(character* User)
{
  if(User->IsPlayer())
    ADD_MESSAGE("%s is now %sactive.", CHARNAME(DEFINITE), IsActive() ? "in" : "");

  SetIsActive(!IsActive());
  User->DexterityAction(10);

  if(IsActive())
    {
      Team = User->GetTeam()->GetID();
      RemoveFromSlot();
      User->GetStackUnder()->AddItem(this);
    }

  return true;
}

bool beartrap::Apply(character* User)
{
  if(User->IsPlayer())
    ADD_MESSAGE("%s is now %sactive.", CHARNAME(DEFINITE), IsActive() ? "in" : "");

  SetIsActive(!IsActive());
  User->DexterityAction(10);

  if(IsActive())
    {
      Team = User->GetTeam()->GetID();
      RemoveFromSlot();
      User->GetStackUnder()->AddItem(this);
    }

  return true;
}

float arm::GetBlockChance(float EnemyToHitValue) const
{
  return GetWielded() ? Min(1.0f / (1 + EnemyToHitValue / (GetToHitValue() * GetWielded()->GetBlockModifier()) * 10000), 1.0f) : 0;
}

ushort arm::GetBlockCapability() const
{
  return GetWielded() ? GetWielded()->GetStrengthValue() * GetAttribute(ARMSTRENGTH) / 10 : 0;
}

void arm::WieldedSkillHit()
{
  if(Master->GetCWeaponSkill(GetWielded()->GetWeaponCategory())->AddHit())
    {
      CalculateAttackInfo();

      if(Master->IsPlayer())
	GetHumanoidMaster()->GetCWeaponSkill(GetWielded()->GetWeaponCategory())->AddLevelUpMessage();
    }

  if(GetCurrentSWeaponSkill()->AddHit())
    {
      CalculateAttackInfo();

      if(Master->IsPlayer())
	GetCurrentSWeaponSkill()->AddLevelUpMessage(GetWielded()->GetName(UNARTICLED));
    }
}

vector2d beartrap::GetBitmapPos(ushort) const
{
  if(IsActive())
    return vector2d(32, 304);
  else
    return vector2d(32, 320);
}

bool mine::WillExplode(const character* Stepper) const
{
  return IsActive() && GetContainedMaterial() && GetContainedMaterial()->IsExplosive() && (!Stepper || Stepper->GetWeight() > 5000);
}

materialcontainer::materialcontainer(const materialcontainer& MC) : item(MC)
{
  CopyMaterial(MC.ContainedMaterial, ContainedMaterial);
}

meleeweapon::meleeweapon(const meleeweapon& MW) : item(MW), Enchantment(MW.Enchantment)
{
  CopyMaterial(MW.SecondaryMaterial, SecondaryMaterial);
  CopyMaterial(MW.ContainedMaterial, ContainedMaterial);
}

head::head(const head& Head) : bodypart(Head), BaseBiteStrength(Head.BaseBiteStrength)
{
  HelmetSlot.Init(this, HELMETINDEX);
  AmuletSlot.Init(this, AMULETINDEX);
}

humanoidtorso::humanoidtorso(const humanoidtorso& Torso) : torso(Torso)
{
  BodyArmorSlot.Init(this, BODYARMORINDEX);
  CloakSlot.Init(this, CLOAKINDEX);
  BeltSlot.Init(this, BELTINDEX);
}

arm::arm(const arm& Arm) : bodypart(Arm), Strength(Arm.Strength), Dexterity(Arm.Dexterity), StrengthExperience(Arm.StrengthExperience), DexterityExperience(Arm.DexterityExperience), BaseUnarmedStrength(Arm.BaseUnarmedStrength)
{
}

rightarm::rightarm(const rightarm& Arm) : arm(Arm)
{
  WieldedSlot.Init(this, RIGHTWIELDEDINDEX);
  GauntletSlot.Init(this, RIGHTGAUNTLETINDEX);
  RingSlot.Init(this, RIGHTRINGINDEX);
}

leftarm::leftarm(const leftarm& Arm) : arm(Arm)
{
  WieldedSlot.Init(this, LEFTWIELDEDINDEX);
  GauntletSlot.Init(this, LEFTGAUNTLETINDEX);
  RingSlot.Init(this, LEFTRINGINDEX);
}

leg::leg(const leg& Leg) : bodypart(Leg), Strength(Leg.Strength), Agility(Leg.Agility), StrengthExperience(Leg.StrengthExperience), AgilityExperience(Leg.AgilityExperience), BaseKickStrength(Leg.BaseKickStrength)
{
}

rightleg::rightleg(const rightleg& Leg) : leg(Leg)
{
  BootSlot.Init(this, RIGHTBOOTINDEX);
}

leftleg::leftleg(const leftleg& Leg) : leg(Leg)
{
  BootSlot.Init(this, LEFTBOOTINDEX);
}

corpse::corpse(const corpse& Corpse) : item(Corpse)
{
  Deceased = Corpse.Deceased->Duplicate();
  Deceased->SetMotherEntity(this);
}

chest::chest(const chest& Chest) : item(Chest), LockType(Chest.LockType), Locked(Chest.Locked)
{
  Contained = new stack(0, this, HIDDEN, true);
}

oillamp::oillamp(const oillamp& Lamp) : item(Lamp), InhabitedByGenie(false)
{
}

bool whipofcleptia::CleptiaHelps(const character* Enemy, const character* Hitter) const
{
  if(!Enemy->GetMainWielded() || GetMainMaterial()->GetFlexibility() <= 5)
    return false;

  if(Hitter->IsPlayer())
    {
      if(game::GetGod(10)->GetRelation() < 0)
	return false;
      else
	return !(RAND() % (7 - game::GetGod(10)->GetRelation() / 200));
    }
  else
    return !(RAND() % 5);
}

bool wand::Zap(character* Zapper, vector2d, uchar Direction)
{
  if(GetCharges() <= GetTimesUsed())
    {
      ADD_MESSAGE("Nothing happens.");
      return true;
    }
  std::string DeathMSG = "killed by " + GetName(INDEFINITE);
  Beam(Zapper, DeathMSG, Direction, GetBeamRange());
  SetTimesUsed(GetTimesUsed() + 1);
  return true;
}

bool wandofcloning::BeamEffect(character* Zapper, const std::string&, uchar, lsquare* Where)
{
  return Where->CloneEverything(Zapper);
}

void meleeweapon::AddInventoryEntry(const character* Viewer, std::string& Entry, ushort, bool ShowSpecialInfo) const // never piled
{
  AddName(Entry, INDEFINITE);

  if(ShowSpecialInfo)
    {
      Entry << " [" << GetWeight() << "g, DAM " << GetBaseMinDamage() << "-" << GetBaseMaxDamage() << ", BV " << GetBaseBlockValue() << ", SV " << GetStrengthValue();

      uchar CWeaponSkillLevel = Viewer->GetCWeaponSkillLevel(this);
      uchar SWeaponSkillLevel = Viewer->GetSWeaponSkillLevel(this);

      if(CWeaponSkillLevel)
	Entry << ", CS " << CWeaponSkillLevel;

      if(SWeaponSkillLevel)
	Entry << ", SS " << SWeaponSkillLevel;

      Entry << "]";
    }
}

void banana::AddInventoryEntry(const character* Viewer, std::string& Entry, ushort Amount, bool ShowSpecialInfo) const
{
  item::AddInventoryEntry(Viewer, Entry, Amount, ShowSpecialInfo);
}

void armor::AddInventoryEntry(const character*, std::string& Entry, ushort Amount, bool ShowSpecialInfo) const
{
  if(Amount == 1)
    AddName(Entry, INDEFINITE);
  else
    {
      Entry << Amount << " ";
      AddName(Entry, PLURAL);
    }

  if(ShowSpecialInfo)
    Entry << " [" << GetWeight() * Amount << "g, AV " << GetStrengthValue() << "]";
}

void shield::AddInventoryEntry(const character* Viewer, std::string& Entry, ushort, bool ShowSpecialInfo) const // never piled
{
  AddName(Entry, INDEFINITE);

  if(ShowSpecialInfo)
    {
      Entry << " [" << GetWeight() << "g, BV " << GetBaseBlockValue() << ", SV " << GetStrengthValue();

      uchar CWeaponSkillLevel = Viewer->GetCWeaponSkillLevel(this);
      uchar SWeaponSkillLevel = Viewer->GetSWeaponSkillLevel(this);

      if(CWeaponSkillLevel)
	Entry << ", CS " << CWeaponSkillLevel;

      if(SWeaponSkillLevel)
	Entry << ", SS " << SWeaponSkillLevel;

      Entry << "]";
    }
}

void wand::AddInventoryEntry(const character*, std::string& Entry, ushort, bool ShowSpecialInfo) const // never piled
{
  AddName(Entry, INDEFINITE);

  if(ShowSpecialInfo)
    {
      Entry << " [" << GetWeight();

      if(TimesUsed == 1)
	Entry << "g, used 1 time]";
      else if(TimesUsed)
	Entry << "g, used " << TimesUsed << " times]";
      else
	Entry << "g]";
    }
}

void materialcontainer::SignalSpoil(material* Material)
{
  if(!Exists())
    return;

  if(Material == GetMainMaterial())
    {
      if(CanBeSeenByPlayer())
	ADD_MESSAGE("%s becomes so spoiled that it cannot hold its contents anymore.", CHARNAME(DEFINITE));

      if(GetContainedMaterial()->IsLiquid())
	{
	  if(!game::IsInWilderness())
	    GetLSquareUnder()->SpillFluid(5, GetContainedMaterial()->GetColor());

	  RemoveFromSlot();
	}
      else
	{
	  lump* Lump = new lump(0, NOMATERIALS);
	  Lump->InitMaterials(GetContainedMaterial());
	  SetContainedMaterial(0, NOPICUPDATE);
	  GetSlot()->DonateTo(Lump);
	}

      SendToHell();
    }
  else if(Material == GetContainedMaterial())
    {
      if(CanBeSeenByPlayer())
	ADD_MESSAGE("The contents of %s spoil completely.", CHARNAME(DEFINITE));

      Empty();
    }
}

void banana::SignalSpoil(material* Material)
{
  if(!Exists())
    return;

  if(Material == GetSecondaryMaterial() && !GetMainMaterial()->IsVeryCloseToSpoiling())
    {
      if(CanBeSeenByPlayer())
	ADD_MESSAGE("The inside of %s spoils completely.", CHARNAME(DEFINITE));

      item* Peel = new bananapeels(0, NOMATERIALS);
      Peel->InitMaterials(GetMainMaterial());
      GetSlot()->DonateTo(Peel);
      SetMainMaterial(0, NOPICUPDATE);
      SendToHell();
    }
  else
    item::SignalSpoil(Material); // this should spill potential poison liquid to the ground!
}

void meleeweapon::SignalSpoil(material* Material)
{
  if(!Exists())
    return;

  if(Material == GetContainedMaterial())
    {
      Empty();

      if(CanBeSeenByPlayer())
	ADD_MESSAGE("%s seems cleaner now.", CHARNAME(DEFINITE));
    }
  else
    item::SignalSpoil(Material); // this should spill potential poison liquid to the ground!
}

void meleeweapon::AddPostFix(std::string& String) const
{
  item::AddPostFix(String);

  if(GetContainedMaterial())
    {
      String << " covered with ";
      GetContainedMaterial()->AddName(String, false, false);
    }

  if(Enchantment > 0)
    String << " +" << short(Enchantment);
  else if(Enchantment < 0)
    String << ' ' << short(Enchantment);
}

void bodypart::SignalSpoil(material* Material)
{
  if(Material == GetContainedMaterial())
    {
      /* Bug: this isn't shown if the bpart is part of a player-carried corpse */

      if(CarriedByPlayer())
	ADD_MESSAGE("%s feels lighter.", CHARNAME(DEFINITE));

      ChangeContainedMaterial(0);
    }
  else if(GetMaster())
    GetMaster()->SignalSpoil();
  else
    item::SignalSpoil(Material);
}

bool bodypart::IsVeryCloseToSpoiling() const
{
  return GetMainMaterial()->IsVeryCloseToSpoiling();
}

void corpse::SignalSpoil(material*)
{
  bool TorsoSpoiled = false;

  if(GetDeceased()->GetTorso()->IsVeryCloseToSpoiling())
    {
      if(CanBeSeenByPlayer())
	ADD_MESSAGE("%s spoils.", CHARNAME(DEFINITE));

      TorsoSpoiled = true;
    }

  for(ushort c = 1; c < GetDeceased()->GetBodyParts(); ++c)
    {
      bodypart* BodyPart = GetDeceased()->GetBodyPart(c);

      if(BodyPart)
	if(BodyPart->IsVeryCloseToSpoiling())
	  {
	    if(!TorsoSpoiled && CanBeSeenByPlayer())
	      ADD_MESSAGE("The %s of %s spoils.", GetDeceased()->GetBodyPartName(c).c_str(), GetDeceased()->CHARNAME(DEFINITE));

	    GetDeceased()->SevereBodyPart(c)->SendToHell();
	  }
	else if(TorsoSpoiled)
	  GetSlot()->AddFriendItem(GetDeceased()->SevereBodyPart(c));
    }

  if(TorsoSpoiled)
    {
      RemoveFromSlot();
      SendToHell();
    }
}

bool materialcontainer::CanBePiledWith(const item* Item, const character* Viewer) const
{
  if(!item::CanBePiledWith(Item, Viewer))
    return false;

  const materialcontainer* Weapon = static_cast<const materialcontainer*>(Item);

  if(ContainedMaterial == 0 && Weapon->ContainedMaterial == 0)
    return true;

  return ContainedMaterial != 0 && Weapon->ContainedMaterial != 0 && ContainedMaterial->IsSameAs(Weapon->ContainedMaterial);
}

bool meleeweapon::CanBePiledWith(const item* Item, const character* Viewer) const
{
  if(!item::CanBePiledWith(Item, Viewer))
    return false;

  const meleeweapon* Weapon = static_cast<const meleeweapon*>(Item);

  if(Enchantment != Weapon->Enchantment || !SecondaryMaterial->IsSameAs(Weapon->SecondaryMaterial))
    return false;

  if(ContainedMaterial == 0 && Weapon->ContainedMaterial == 0)
    return true;

  return ContainedMaterial != 0 && Weapon->ContainedMaterial != 0 && ContainedMaterial->IsSameAs(Weapon->ContainedMaterial);
}

bool armor::CanBePiledWith(const item* Item, const character* Viewer) const
{
  return item::CanBePiledWith(Item, Viewer) && Enchantment == static_cast<const armor*>(Item)->Enchantment;
}

bool key::CanBePiledWith(const item* Item, const character* Viewer) const
{
  return item::CanBePiledWith(Item, Viewer) && LockType == static_cast<const key*>(Item)->LockType;
}

bool bodypart::CanBePiledWith(const item* Item, const character* Viewer) const
{
  return materialcontainer::CanBePiledWith(Item, Viewer) && OwnerDescription == static_cast<const bodypart*>(Item)->OwnerDescription;
}

bool corpse::CanBePiledWith(const item* Item, const character* Viewer) const
{
  if(GetType() != Item->GetType()
  || GetConfig() != Item->GetConfig()
  || GetWeight() != Item->GetWeight()
  || Viewer->GetCWeaponSkillLevel(this) != Viewer->GetCWeaponSkillLevel(Item)
  || Viewer->GetSWeaponSkillLevel(this) != Viewer->GetSWeaponSkillLevel(Item))
    return false;

  const corpse* Corpse = static_cast<const corpse*>(Item);

  if(Deceased->GetBodyParts() != Corpse->Deceased->GetBodyParts())
    return false;

  for(ushort c = 0; c < Deceased->GetBodyParts(); ++c)
    {
      bodypart* BodyPart1 = Deceased->GetBodyPart(c);
      bodypart* BodyPart2 = Corpse->Deceased->GetBodyPart(c);

      if(BodyPart1 == 0 && BodyPart2 == 0)
	continue;

      if(BodyPart1 == 0 || BodyPart2 == 0 || !BodyPart1->CanBePiledWith(BodyPart2, Viewer))
	return false;
    }

  if(Deceased->GetName(UNARTICLED) != Corpse->Deceased->GetName(UNARTICLED))
    return false;

  return true;
}

ulong mine::GetPrice() const
{
  if(GetContainedMaterial())
    {
      if(GetContainedMaterial()->IsExplosive())
	return (GetContainedMaterial()->GetRawPrice() << 1) + item::GetPrice();
      else
	return GetContainedMaterial()->GetRawPrice() + item::GetPrice();
    }
  else
    return item::GetPrice();
}

ulong chest::GetPrice() const
{
  return GetContained()->GetPrice() + item::GetPrice();
}

void potion::Break()
{
  if(CanBeSeenByPlayer())
    ADD_MESSAGE("The %s shatters to pieces.", CHARNAME(DEFINITE));

  if(GetContainedMaterial()) 
    GetLSquareUnder()->SpillFluid(GetContainedMaterial()->Clone(), 70, 0);

  item* Remains = new brokenbottle(0, NOMATERIALS);
  Remains->InitMaterials(GetMainMaterial()->Clone());
  GetSlot()->DonateTo(Remains);
  SendToHell();
}

void bodypart::Be()
{
  if(Master)
    {
      if(Master->IsPolymorphed())
	return;

      if(Master->IsEnabled())
	{
	  if(IsInBadCondition() && !(RAND() & 0xF))
	    SpillBlood(1);

	  return;
	}
    }

  if(HP < MaxHP && !(RAND() & 0xF))
    {
      SpillBlood(1);
      HP += Max(((MaxHP - HP) >> 2), 1);
    }

  materialcontainer::Be();
}

void bodypart::SpillBlood(ushort HowMuch, vector2d GetPos)
{
  if(!HowMuch || !IsAlive())
    return;

  if(!game::IsInWilderness()) 
    GetNearLSquare(GetPos)->SpillFluid(HowMuch, GetBloodColor(), 5, 60);
}

void bodypart::SpillBlood(ushort HowMuch)
{
  if(!HowMuch || !IsAlive())
    return;

  if(!game::IsInWilderness()) 
    GetLSquareUnder()->SpillFluid(HowMuch, GetBloodColor(), 5, 60);
}

void materialcontainer::Be()
{
  MainMaterial->Be();

  if(ContainedMaterial)
    ContainedMaterial->Be();
}

void meleeweapon::Be()
{
  MainMaterial->Be();
  SecondaryMaterial->Be();

  if(ContainedMaterial)
    ContainedMaterial->Be();
}

void bodypart::InitSpecialAttributes()
{
  BloodColor = Master->GetBloodColor();
}

ulong shield::GetPrice() const /* temporary... */
{
  float StrengthValue = GetStrengthValue();
  return ulong(GetBaseBlockValue() * StrengthValue * StrengthValue * StrengthValue / 750) + item::GetPrice();
}

ulong whipofcleptia::GetPrice() const
{
  /* If not broken but not flexible enough to work, special thievery bonus must be removed */

  return GetMainMaterial()->GetFlexibility() > 5 || IsBroken() ? whip::GetPrice() : whip::GetPrice() - item::GetPrice();
}

bool materialcontainer::IsSparkling(ushort ColorIndex) const
{
  return (ColorIndex == 0 && MainMaterial->IsSparkling()) || (ColorIndex == 1 && ContainedMaterial && ContainedMaterial->IsSparkling());
}

bool meleeweapon::IsSparkling(ushort ColorIndex) const
{
  return (ColorIndex == 0 && MainMaterial->IsSparkling()) || (ColorIndex == 1 && SecondaryMaterial->IsSparkling()) || (ColorIndex == 2 && ContainedMaterial && ContainedMaterial->IsSparkling());
}

ushort justifier::GetOutlineColor(ushort Frame) const
{
  Frame &= 31;
  return MakeRGB16(0, 135 + (Frame * (31 - Frame) >> 1), 0);
}

ushort neercseulb::GetOutlineColor(ushort Frame) const
{
  Frame &= 31;
  return MakeRGB16(135 + (Frame * (31 - Frame) >> 1), 0, 0);
}

ushort goldeneagleshirt::GetOutlineColor(ushort Frame) const
{
  Frame &= 31;
  ushort Element = 135 + (Frame * (31 - Frame) >> 2);
  return MakeRGB16(0, Element, Element);
}

void armor::Save(outputfile& SaveFile) const
{
  item::Save(SaveFile);
  SaveFile << Enchantment;
}

void armor::Load(inputfile& SaveFile)
{
  item::Load(SaveFile);
  SaveFile >> Enchantment;
}

void armor::AddPostFix(std::string& String) const
{
  item::AddPostFix(String);

  if(Enchantment > 0)
    String << " +" << short(Enchantment);
  else if(Enchantment < 0)
    String << ' ' << short(Enchantment);
}

bool scrollofenchantweapon::Read(character* Reader)
{
  Reader->StartReading(this, 1000);
  return true;
}

void scrollofenchantweapon::FinishReading(character* Reader)
{
  if(!Reader->GetStack()->SortedItems(Reader, item::WeaponSorter))
    ADD_MESSAGE("You notice you have lost anything you wished to enchant.");
  else
    {
      while(true)
	{
	  item* Item = Reader->GetStack()->DrawContents(Reader, "Choose a weapon to enchant:", 0, item::WeaponSorter);

	  if(Item)
	    {
	      ADD_MESSAGE("Your %s glows briefly red. It feels very warm now.", Item->CHARNAME(UNARTICLED));
	      Item->EditEnchantment(1);
	      break;
	    }
	  else if(game::BoolQuestion("Really cancel read? [y/N]"))
	    return;
	}

      RemoveFromSlot();
      SendToHell();
    }
}

bool scrollofenchantarmor::Read(character* Reader)
{
  Reader->StartReading(this, 1000);
  return true;
}

void scrollofenchantarmor::FinishReading(character* Reader)
{
  if(!Reader->GetStack()->SortedItems(Reader, item::ArmorSorter))
    ADD_MESSAGE("You notice you have lost anything you wished to enchant.");
  else
    {
      while(true)
	{
	  item* Item = Reader->GetStack()->DrawContents(Reader, "Choose an armor to enchant:", 0, item::ArmorSorter);

	  if(Item)
	    {
	      ADD_MESSAGE("Your %s glows briefly blue. It feels very warm now.", Item->CHARNAME(UNARTICLED));
	      Item->EditEnchantment(1);
	      break;
	    }
	  else if(game::BoolQuestion("Really cancel read? [y/N]"))
	    return;
	}

      RemoveFromSlot();
      SendToHell();
    }
}

void meleeweapon::SetEnchantment(char Amount)
{
  Enchantment = Amount;
  SignalEnchantmentChange();
}

void armor::SetEnchantment(char Amount)
{
  Enchantment = Amount;
  SignalEnchantmentChange();
}

void meleeweapon::EditEnchantment(char Amount)
{
  Enchantment += Amount;
  SignalEnchantmentChange();
}

void armor::EditEnchantment(char Amount)
{
  Enchantment += Amount;
  SignalEnchantmentChange();
}

float meleeweapon::GetWeaponStrength() const
{
  return sqrt(float(GetFormModifier()) * GetMainMaterial()->GetStrengthValue() * GetWeight()) * (20 + Enchantment) / 20;
}

ushort meleeweapon::GetStrengthValue() const
{
  return ulong(GetStrengthModifier()) * GetMainMaterial()->GetStrengthValue() * (20 + Enchantment) / 20000;
}

ushort armor::GetStrengthValue() const
{
  return ulong(GetStrengthModifier()) * GetMainMaterial()->GetStrengthValue() * (10 + Enchantment) / 10000;
}

void bodypart::SignalEnchantmentChange()
{
  CalculateAttributeBonuses();

  if(GetMaster() && !GetMaster()->IsInitializing())
    {
      GetMaster()->CalculateAttributeBonuses();
      GetMaster()->CalculateBattleInfo();
    }
}

bool chest::ReceiveDamage(character* Damager, ushort Damage, uchar Type)
{
  if(Type == PHYSICALDAMAGE)
    {
      Contained->ReceiveDamage(Damager, Damage / 2, PHYSICALDAMAGE);

      if(IsLocked() && Damage > RAND() % 6)
	{
	  SetIsLocked(false);
	  SetLockType(DAMAGED);

	  if(CanBeSeenByPlayer())
	    {
	      ADD_MESSAGE("%s's lock shatters to pieces.", GetNameSingular().c_str());
	    }

	  return true;
	}
      else
	{
	  if(Damager->IsPlayer())
	    ADD_MESSAGE("THUMB!");
	}
    }

  return false;
}

void chest::DrawContents(const character* Char)
{
  std::string Topic = "Contents of your " + GetName(UNARTICLED);
  GetContained()->DrawContents(Char, Topic, NO_SELECT);
}

void backpack::ReceiveFluidSpill(material* Liquid)
{
  if(!Liquid->IsExplosive())
    {
      ulong Amount = Liquid->GetVolume() * (30 + (RAND() % 71)) / 100;
      GetContainedMaterial()->SetWetness(Amount);
      if(CanBeSeenByPlayer())
	ADD_MESSAGE("%s gets wet.", CHARNAME(DEFINITE));
    }
}

void arm::ApplyEquipmentAttributeBonuses(item* Item)
{
  if(Item->AffectsArmStrength())
    StrengthBonus += Item->GetEnchantment();

  if(Item->AffectsDexterity())
    DexterityBonus += Item->GetEnchantment();
}

void leg::ApplyEquipmentAttributeBonuses(item* Item)
{
  if(Item->AffectsLegStrength())
    {
      StrengthBonus += Item->GetEnchantment();

      if(GetMaster())
	GetMaster()->CalculateBurdenState();
    }

  if(Item->AffectsAgility())
    AgilityBonus += Item->GetEnchantment();
}

void arm::CalculateAttributeBonuses()
{
  StrengthBonus = DexterityBonus = 0;

  for(ushort c = 0; c < GetEquipmentSlots(); ++c)
    {
      item* Equipment = GetEquipment(c);

      if(Equipment)
	ApplyEquipmentAttributeBonuses(Equipment);
    }
}

void leg::CalculateAttributeBonuses()
{
  StrengthBonus = AgilityBonus = 0;

  for(ushort c = 0; c < GetEquipmentSlots(); ++c)
    {
      item* Equipment = GetEquipment(c);

      if(Equipment)
	ApplyEquipmentAttributeBonuses(Equipment);
    }
}

void arm::CalculateAll()
{
  item::CalculateAll();
  CalculateAttributeBonuses();
}

void leg::CalculateAll()
{
  item::CalculateAll();
  CalculateAttributeBonuses();
}

void meleeweapon::VirtualConstructor(bool Load)
{
  if(!Load)
    Enchantment = GetDefaultEnchantment();
}

void armor::VirtualConstructor(bool Load)
{
  if(!Load)
    Enchantment = GetDefaultEnchantment();
}
