#include <queue>
#include <cmath>

#include "charde.h"
#include "stack.h"
#include "error.h"
#include "message.h"
#include "itemde.h"
#include "lsquare.h"
#include "lterraba.h"
#include "dungeon.h"
#include "whandler.h"
#include "level.h"
#include "worldmap.h"
#include "hscore.h"
#include "feio.h"
#include "godba.h"
#include "felist.h"
#include "team.h"
#include "colorbit.h"
#include "config.h"
#include "femath.h"
#include "stdover.h"
#include "slot.h"
#include "actionde.h"
#include "command.h"
#include "proto.h"
#include "save.h"

void (character::*character::PrintBeginStateMessage[STATES])() const = { 0, &character::PrintBeginHasteMessage, &character::PrintBeginSlowMessage, &character::PrintBeginPolymorphControlMessage, &character::PrintBeginLifeSaveMessage, &character::PrintBeginLycanthropyMessage };
void (character::*character::PrintEndStateMessage[STATES])() const = { 0, &character::PrintEndHasteMessage, &character::PrintEndSlowMessage, &character::PrintEndPolymorphControlMessage, &character::PrintEndLifeSaveMessage, &character::PrintEndLycanthropyMessage };
void (character::*character::EnterTemporaryStateHandler[STATES])(ushort) = { 0, &character::EnterTemporaryHaste, &character::EnterTemporarySlow, 0, 0, 0 };
void (character::*character::EndTemporaryStateHandler[STATES])() = { &character::EndTemporaryPolymorph, 0, 0, 0, 0, 0 };
void (character::*character::StateHandler[STATES])() = { 0, 0, 0, 0, 0, &character::LycanthropyHandler };

std::string character::StateDescription[STATES] = { "polymorphed", "hasted", "slowed", "polycontrol", "life saved", "lycanthropy" };

character::character(donothing) : entity(true), NP(25000), AP(0), Player(false), TemporaryState(0), Team(0), WayPoint(-1, -1), Money(0), HomeRoom(0), Action(0), MotherEntity(0), PolymorphBackup(0), PermanentState(0)
{
  Stack = new stack(0, this, HIDDEN);
}

character::~character()
{
  if(GetAction())
    {
      GetAction()->DeleteUsedItems();
      delete GetAction();
    }

  if(GetTeam())
    GetTeam()->Remove(GetTeamIterator());

  delete Stack;

  if(GetTorso())
    GetTorso()->SendToHell();

  delete [] BodyPartSlot;
  delete [] OriginalBodyPartID;

  if(PolymorphBackup)
    PolymorphBackup->SendToHell();

  for(ushort c = 0; CategoryWeaponSkill[c]; ++c)
    delete CategoryWeaponSkill[c];

  delete [] CategoryWeaponSkill;
}

void character::Hunger(ushort Turns) 
{
  switch(GetBurdenState())
    {
    case UNBURDENED:
      EditNP(-Turns);
      break;
    case BURDENED:
      EditNP(-2 * Turns);
      EditExperience(LEGSTRENGTH, 2 * Turns);
      EditExperience(AGILITY, -2 * Turns);
      break;
    case STRESSED:
    case OVERLOADED:
      EditNP(-4 * Turns);
      EditExperience(LEGSTRENGTH, 4 * Turns);
      EditExperience(AGILITY, -4 * Turns);
      break;
    }

  if(GetHungerState() == HUNGRY || GetHungerState() == VERYHUNGRY)
    {
      EditExperience(ARMSTRENGTH, -Turns);
      EditExperience(LEGSTRENGTH, -Turns);
    }

  CheckStarvationDeath("starved to death");
}

uchar character::TakeHit(character* Enemy, item* Weapon, float AttackStrength, float ToHitValue, short Success, uchar Type, bool Critical)
{
  DeActivateVoluntaryAction(Enemy->GetName(DEFINITE) + " seems to be hostile.");
  uchar Dir = Type == BITEATTACK ? YOURSELF : game::GetDirectionForVector(GetPos() - Enemy->GetPos());
  float DodgeValue = CalculateDodgeValue();

  if(Critical)
    {
      ushort Damage = ushort(AttackStrength * (100 + Success) / 2500000) + (RAND() % 3 ? 2 : 1);
      uchar BodyPart = ChooseBodyPartToReceiveHit(ToHitValue, DodgeValue);

      switch(Type)
	{
	case UNARMEDATTACK:
	  Enemy->AddPrimitiveHitMessage(this, Enemy->FirstPersonCriticalUnarmedHitVerb(), Enemy->ThirdPersonCriticalUnarmedHitVerb(), BodyPart);
	  break;
	case WEAPONATTACK:
          Enemy->AddWeaponHitMessage(this, Weapon, BodyPart, true);
	  break;
	case KICKATTACK:
	  Enemy->AddPrimitiveHitMessage(this, Enemy->FirstPersonCriticalKickVerb(), Enemy->ThirdPersonCriticalKickVerb(), BodyPart);
	  break;
	case BITEATTACK:
	  Enemy->AddPrimitiveHitMessage(this, Enemy->FirstPersonCriticalBiteVerb(), Enemy->ThirdPersonCriticalBiteVerb(), BodyPart);
	  break;
	}

      ReceiveBodyPartDamage(this, Damage, PHYSICALDAMAGE, BodyPart, Dir, false, true);

      if(game::WizardModeActivated() && GetLSquareUnder()->CanBeSeen(true))
      	ADD_MESSAGE("(damage: %d)", Damage);

      if(CheckDeath("killed by " + Enemy->GetName(INDEFINITE), Enemy->IsPlayer()))
	return HASDIED;

      return HASHIT;
    }

  if(RAND() % ushort(100 + ToHitValue / DodgeValue * (100 + Success)) >= 100)
    {
      ushort Damage = ushort(AttackStrength * (100 + Success) / 5000000) + (RAND() % 3 ? 1 : 0);
      uchar BodyPart = ChooseBodyPartToReceiveHit(ToHitValue, DodgeValue);

      switch(Type)
	{
	case UNARMEDATTACK:
	  Enemy->AddPrimitiveHitMessage(this, Enemy->FirstPersonUnarmedHitVerb(), Enemy->ThirdPersonUnarmedHitVerb(), BodyPart);
	  break;
	case WEAPONATTACK:
          Enemy->AddWeaponHitMessage(this, Weapon, BodyPart);
	  break;
	case KICKATTACK:
	  Enemy->AddPrimitiveHitMessage(this, Enemy->FirstPersonKickVerb(), Enemy->ThirdPersonKickVerb(), BodyPart);
	  break;
	case BITEATTACK:
	  Enemy->AddPrimitiveHitMessage(this, Enemy->FirstPersonBiteVerb(), Enemy->ThirdPersonBiteVerb(), BodyPart);
	  break;
	}

      if(Enemy->AttackIsBlockable(Type))
	Damage = CheckForBlock(Enemy, Weapon, ToHitValue, Damage, Success, Type);

      if(!Damage || !ReceiveBodyPartDamage(this, Damage, PHYSICALDAMAGE, BodyPart, Dir))
	return HASBLOCKED;

      if(CheckDeath("killed by " + Enemy->GetName(INDEFINITE), Enemy->IsPlayer()))
	return HASDIED;

      return HASHIT;
    }
  else
    {
      Enemy->AddMissMessage(this);
      EditExperience(AGILITY, 50);
      EditExperience(PERCEPTION, 25);
      return HASDODGED;
    }
}

struct svpriorityelement
{
  svpriorityelement(uchar BodyPart, uchar StrengthValue) : BodyPart(BodyPart), StrengthValue(StrengthValue) { }
  bool operator < (const svpriorityelement& AnotherPair) const { return StrengthValue > AnotherPair.StrengthValue; }
  uchar BodyPart;
  ushort StrengthValue;
};

uchar character::ChooseBodyPartToReceiveHit(float ToHitValue, float DodgeValue)
{
  if(BodyParts() == 1)
    return 0;

  std::priority_queue<svpriorityelement> SVQueue;

  for(ushort c = 0; c < BodyParts(); ++c)
    if(GetBodyPart(c))
      SVQueue.push(svpriorityelement(c, GetBodyPart(c)->GetStrengthValue() + GetBodyPart(c)->GetHP()));

  while(SVQueue.size())
    {
      ushort ToHitPercentage = ushort(GLOBAL_WEAK_BODYPART_HIT_MODIFIER * ToHitValue * GetBodyPart(SVQueue.top().BodyPart)->GetVolume() * 100 / (DodgeValue * GetVolume()));

      if(ToHitPercentage < 1)
	ToHitPercentage = 1;
      else if(ToHitPercentage > 99)
	ToHitPercentage = 99;

      if(ToHitPercentage > RAND() % 100)
	return SVQueue.top().BodyPart;

      SVQueue.pop();
    }

  return 0;
}

void character::Be()
{
  if(game::IsLoading())
    {
      if(!IsPlayer())
	return;
      else
	game::SetIsLoading(false);
    }
  else
    {
      switch(GetBurdenState())
	{
	case UNBURDENED:
	  EditAP(CalculateStateAPGain(100 + (GetAttribute(AGILITY) >> 1)));
	  break;
	case BURDENED:
	  EditAP(CalculateStateAPGain(75 + (GetAttribute(AGILITY) >> 1) - (GetAttribute(AGILITY) >> 2)));
	  break;
	case STRESSED:
	case OVERLOADED:
	  EditAP(CalculateStateAPGain(50 + (GetAttribute(AGILITY) >> 2)));
	  break;
	}

      ushort c;

      HandleStates();

      if(!IsEnabled())
	return;

      if(GetAction())
	GetAction()->Handle();

      if(!IsEnabled())
	return;

      if(GetHP() < GetMaxHP() / 3)
	SpillBlood(RAND() % 2);

      if(IsPlayer() && GetHungerState() == VERYHUNGRY && !(RAND() % 50) && (!GetAction() || GetAction()->AllowFaint()))
	{
	  if(GetAction())
	    GetAction()->Terminate(false);

	  Faint();
	}

      if(GetAP() >= 0)
	ActionAutoTermination();

      for(c = 0; c < AllowedWeaponSkillCategories(); ++c)
	if(GetCategoryWeaponSkill(c)->Turn(1) && IsPlayer())
	  GetCategoryWeaponSkill(c)->AddLevelDownMessage();

      //CharacterSpeciality();
      Regenerate();

      if(IsPlayer())
	{
	  if(!GetAction() || GetAction()->AllowFoodConsumption())
	    Hunger();
	}
    }

  if(GetAP() >= 0)
    {
      ApplyExperience();

      if(IsPlayer())
	{
	  static ushort Timer = 0;

	  if(configuration::GetAutoSaveInterval() && !GetAction() && ++Timer >= configuration::GetAutoSaveInterval())
	    {
	      game::Save(game::GetAutoSaveFileName().c_str());
	      Timer = 0;
	    }

	  if(!GetAction())
	    GetPlayerCommand();
	  else
	    {
	      game::DrawEverything();

	      if(READKEY() && GetAction()->IsVoluntary())
		GetAction()->Terminate(false);
	    }
	}
      else
	{
	  if(!GetAction() && !game::IsInWilderness())
	    GetAICommand();
	}

      EditAP(-1000);
    }
}

bool character::GoUp()
{
  if(GetSquareUnder()->GetOTerrain()->GoUp(this))
    {
      EditExperience(LEGSTRENGTH, 50);
      EditNP(-20);
      EditAP(-1000);
      return true;
    }
  else
    return false;
}

bool character::GoDown()
{
  if(GetSquareUnder()->GetOTerrain()->GoDown(this))
    {
      EditExperience(AGILITY, 25);
      EditNP(-10);
      EditAP(-1000);
      return true;
    }
  else
    return false;
}

bool character::Open()
{
  if(CanOpen())
    {
      int Key;
      bool OpenableItems = GetStack()->SortedItems(this, &item::OpenableSorter);

      if(OpenableItems)
	Key = game::AskForKeyPress("What do you wish to open?  [press a direction key, space or i]");
      else
	Key = game::AskForKeyPress("What do you wish to open?  [press a direction key or space]");

      if(Key == 'i' && OpenableItems)
	return OpenItem();

      vector2d DirVect = game::GetDirectionVectorForKey(Key);

      if(DirVect != DIR_ERROR_VECTOR && game::IsValidPos(GetPos() + DirVect))
	return OpenPos(GetPos() + DirVect);
    }
  else
    ADD_MESSAGE("This monster type cannot open anything.");

  return false;
}

bool character::Close()
{
  if(CanOpen())
    {
      uchar Dir = game::DirectionQuestion("Which door do you wish to close?  [press a direction key]", false);

      if(Dir != DIR_ERROR && game::IsValidPos(GetPos() + game::GetMoveVector(Dir)))
	return ClosePos(GetPos() + game::GetMoveVector(Dir));
    }
  else
    ADD_MESSAGE("This monster type cannot close anything.");

  return false;
}

bool character::Drop()
{
  if(!GetStack()->GetItems())
    {
      ADD_MESSAGE("You have nothing to drop!");
      return false;
    }

  item* Item = GetStack()->DrawContents(this, "What do you want to drop?");

  if(Item)
    if(!GetLSquareUnder()->GetRoom() || (GetLSquareUnder()->GetRoom() && GetLSquareUnder()->GetLevelUnder()->GetRoom(GetLSquareUnder()->GetRoom())->DropItem(this, Item)))
      {
	Item->MoveTo(GetLSquareUnder()->GetStack());
	return true;
      }

  return false;
}

bool character::Consume()
{
  if(!game::IsInWilderness() && GetLSquareUnder()->GetOLTerrain()->HasConsumeEffect())
    {
      if(GetLSquareUnder()->GetOLTerrain()->Consume(this))
	return true;
    }

  if((game::IsInWilderness() || !GetLSquareUnder()->GetStack()->SortedItems(this, &item::ConsumableSorter)) && !GetStack()->SortedItems(this, &item::ConsumableSorter))
    {
      ADD_MESSAGE("You have nothing to consume!");
      return false;
    }

  item* Item;

  if(!game::IsInWilderness() && GetLSquareUnder()->GetStack()->SortedItems(this, &item::ConsumableSorter))
      Item = GetStack()->DrawContents(GetLSquareUnder()->GetStack(), this, "What do you wish to consume?", "Items in your inventory", "Items on ground", &item::ConsumableSorter);
  else
      Item = GetStack()->DrawContents(this, "What do you wish to consume?", &item::ConsumableSorter);

  if(Item)
    {
      if(CheckBulimia() && !game::BoolQuestion("You think your stomach will burst if you eat anything more. Force it down? [y/N]"))
	return false;

      return ConsumeItem(Item);
    }

  return false;
}

bool character::ConsumeItem(item* Item)
{
  Item = Item->PrepareForConsuming(this);

  if(!Item)
    return false;

  consume* Consume = new consume(this);
  Consume->SetDescription(Item->GetConsumeVerb());
  Consume->SetWasOnGround(Item->IsOnGround());
  Consume->SetConsuming(Item);
  SetAction(Consume);
  return true;
}

bool character::CheckBulimia() const
{
  return GetHungerState() == BLOATED;
}

void character::Move(vector2d MoveTo, bool TeleportMove)
{
  /* Test whether the player is stuck to something */
  if(!TeleportMove && IsStuck())
    {
      if(!TryToUnstuck(MoveTo - GetPos()))
	return;
    }

  if(GetBurdenState() != OVERLOADED || TeleportMove)
    {
      game::GetCurrentArea()->MoveCharacter(GetPos(), MoveTo);

      if(IsPlayer())
	ShowNewPosInfo();

      if(!TeleportMove)
	{
	  EditAP(-CalculateMoveAPRequirement(GetSquareUnder()->GetEntryAPRequirement()) + 1000);
	  EditNP(-10);
	  EditExperience(AGILITY, 5);
	}
    }
  else
    if(IsPlayer())
      ADD_MESSAGE("You try very hard to crawl forward. But your load is too heavy.");
}

void character::DrawToTileBuffer(bool Animate) const
{
  if(GetTorso())
    GetTorso()->DrawToTileBuffer(Animate);
}

void character::GetAICommand()
{
  SeekLeader();

  if(CheckForEnemies(true))
    return;

  if(CheckForUsefulItemsOnGround())
    return;

  if(FollowLeader())
    return;

  if(CheckForDoors())
    return;

  MoveRandomly();
}

bool character::MoveTowards(vector2d TPos)
{
  vector2d MoveTo[3];

  if(TPos.X < GetPos().X)
    {
      if(TPos.Y < GetPos().Y)
	{
	  MoveTo[0] = vector2d(-1, -1);
	  MoveTo[1] = vector2d(-1,  0);
	  MoveTo[2] = vector2d( 0, -1);
	}

      if(TPos.Y == GetPos().Y)
	{
	  MoveTo[0] = vector2d(-1,  0);
	  MoveTo[1] = vector2d(-1, -1);
	  MoveTo[2] = vector2d(-1,  1);
	}

      if(TPos.Y > GetPos().Y)
	{
	  MoveTo[0] = vector2d(-1, 1);
	  MoveTo[1] = vector2d(-1, 0);
	  MoveTo[2] = vector2d( 0, 1);
	}
    }

  if(TPos.X == GetPos().X)
    {
      if(TPos.Y < GetPos().Y)
	{
	  MoveTo[0] = vector2d( 0, -1);
	  MoveTo[1] = vector2d(-1, -1);
	  MoveTo[2] = vector2d( 1, -1);
	}

      if(TPos.Y == GetPos().Y)
	return false;

      if(TPos.Y > GetPos().Y)
	{
	  MoveTo[0] = vector2d( 0, 1);
	  MoveTo[1] = vector2d(-1, 1);
	  MoveTo[2] = vector2d( 1, 1);
	}
    }

  if(TPos.X > GetPos().X)
    {
      if(TPos.Y < GetPos().Y)
	{
	  MoveTo[0] = vector2d(1, -1);
	  MoveTo[1] = vector2d(1,  0);
	  MoveTo[2] = vector2d(0, -1);
	}

      if(TPos.Y == GetPos().Y)
	{
	  MoveTo[0] = vector2d(1,  0);
	  MoveTo[1] = vector2d(1, -1);
	  MoveTo[2] = vector2d(1,  1);
	}

      if(TPos.Y > GetPos().Y)
	{
	  MoveTo[0] = vector2d(1, 1);
	  MoveTo[1] = vector2d(1, 0);
	  MoveTo[2] = vector2d(0, 1);
	}
    }

  if(TryMove(GetPos() + MoveTo[0])) return true;

  if(RAND() % 2)
    {
      if(TryMove(GetPos() + MoveTo[1])) return true;
      if(TryMove(GetPos() + MoveTo[2])) return true;
    }
  else
    {
      if(TryMove(GetPos() + MoveTo[2])) return true;
      if(TryMove(GetPos() + MoveTo[1])) return true;
    }

  return false;
}

bool character::TryMove(vector2d MoveTo, bool DisplaceAllowed)
{
  if(!game::IsInWilderness())
    if(MoveTo.X >= 0 && MoveTo.Y >= 0 && MoveTo.X < game::GetCurrentLevel()->GetXSize() && MoveTo.Y < game::GetCurrentLevel()->GetYSize())
      {
	character* Character;

	if((Character = game::GetCurrentLevel()->GetLSquare(MoveTo)->GetCharacter()))
	  if(IsPlayer())
	    if(GetTeam() != Character->GetTeam())
	      return Hit(Character);
	    else
	      if(DisplaceAllowed)
		{
		  Displace(Character);
		  return true;
		}
	      else
		return false;
	  else
	    if(GetTeam()->GetRelation(Character->GetTeam()) == HOSTILE)
	      return Hit(Character);
	    else
	      if(GetTeam() == Character->GetTeam() && DisplaceAllowed)
		return Displace(Character);
	      else
		return false;
	else
	  if(game::GetCurrentLevel()->GetLSquare(MoveTo)->IsWalkable(this) || (game::GetGoThroughWallsCheat() && IsPlayer()))
	    {
	      Move(MoveTo);
	      return true;
	    }
	  else
	    {
	      olterrain* Terrain = game::GetCurrentLevel()->GetLSquare(MoveTo)->GetOLTerrain();

	      if(IsPlayer() && Terrain->CanBeOpened())
		{
		  if(CanOpen())
		    {
		      if(Terrain->IsLocked())
			{
			  ADD_MESSAGE("%s is locked.", Terrain->CHARNAME(DEFINITE));
			  return false;
			}
		      else
			{
			  if(game::BoolQuestion("Do you want to open " + Terrain->GetName(UNARTICLED) + "? [y/N]", false, game::GetMoveCommandKeyBetweenPoints(game::GetPlayer()->GetPos(), MoveTo)))
			    {
			      OpenPos(MoveTo);
			      return true;
			    }
			  else
			    return false;
			}
		    }
		  else
		    {
		      ADD_MESSAGE("This monster type cannot open doors.");
		      return false;
		    }
		}
	      else
		return false;
	    }
      }
    else
      {
	if(IsPlayer() && game::GetCurrentLevel()->GetOnGround() && game::BoolQuestion("Do you want to leave " + game::GetCurrentDungeon()->GetLevelDescription(game::GetCurrent()) + "? [y/N]"))
	  {
	    if(HasPetrussNut() && !HasGoldenEagleShirt())
	      {
		game::TextScreen("An undead and sinister voice greets you as you leave the city behind:\n\n\"MoRtAl! ThOu HaSt SlAuGtHeReD pErTtU aNd PlEaSeD mE!\nfRoM tHiS dAy On, YoU aRe ThE dEaReSt SeRvAnT oF aLl EvIl!\"\n\nYou are victorious!");
		game::GetPlayer()->AddScoreEntry("killed Petrus and became the Avatar of Chaos", 3, false);
		game::End();
		return true;
	      }

	    std::vector<character*> TempPlayerGroup;

	    if(!GetLSquareUnder()->GetLevelUnder()->CollectCreatures(TempPlayerGroup, this, false))
	      return false;

	    game::GetCurrentArea()->RemoveCharacter(GetPos());
	    game::GetCurrentDungeon()->SaveLevel();
	    game::LoadWorldMap();
	    game::GetWorldMap()->GetPlayerGroup().swap(TempPlayerGroup);
	    game::SetIsInWilderness(true);
	    game::GetCurrentArea()->AddCharacter(game::GetCurrentDungeon()->GetWorldMapPos(), this);
	    game::SendLOSUpdateRequest();
	    game::UpdateCamera();
	    game::GetCurrentArea()->UpdateLOS();
	    if(configuration::GetAutoSaveInterval())
	      game::Save(game::GetAutoSaveFileName().c_str());
	    return true;
	  }

	return false;
      }
  else
    if(MoveTo.X >= 0 && MoveTo.Y >= 0 && MoveTo.X < game::GetWorldMap()->GetXSize() && MoveTo.Y < game::GetWorldMap()->GetYSize())
      if(game::GetCurrentArea()->GetSquare(MoveTo)->IsWalkable(this) || game::GetGoThroughWallsCheat())
	{
	  Move(MoveTo);
	  return true;
	}
      else
	return false;
    else
      return false;	
}

bool character::ShowInventory()
{
  GetStack()->DrawContents(this, "Your inventory", false);
  return false;
}

bool character::PickUp()
{
  bool ToBeReturned = false;

  if(GetLSquareUnder()->GetStack()->GetItems() > 0)
    if(GetLSquareUnder()->GetStack()->GetItems() > 1)
      {
	for(;;)
	  {
	    item* Item = GetLSquareUnder()->GetStack()->DrawContents(this, "What do you want to pick up?");

	    if(Item)
		if(!GetLSquareUnder()->GetRoom() || (GetLSquareUnder()->GetRoom() && GetLSquareUnder()->GetLevelUnder()->GetRoom(GetLSquareUnder()->GetRoom())->PickupItem(this, Item)))
		  {
		    if(Item->CheckPickUpEffect(this))
		      {
			ADD_MESSAGE("%s picked up.", Item->CHARNAME(INDEFINITE));
			Item->MoveTo(GetStack());
			Item->CheckPickUpEffect(this);
			ToBeReturned = true;
		      }
		    else
		      return false;
		  }

	    if(!Item || !GetLSquareUnder()->GetStack()->GetItems())
	      break;

	    game::DrawEverythingNoBlit();
	  }
      }
    else
      {
	item* Item = GetLSquareUnder()->GetStack()->GetBottomItem();

	if(!GetLSquareUnder()->GetRoom() || (GetLSquareUnder()->GetRoom() && GetLSquareUnder()->GetLevelUnder()->GetRoom(GetLSquareUnder()->GetRoom())->PickupItem(this, Item)))
	  {
	    ADD_MESSAGE("%s picked up.", Item->CHARNAME(INDEFINITE));
	    Item->MoveTo(GetStack());
	    Item->CheckPickUpEffect(this);
	    return true;
	  }
      }
  else
    ADD_MESSAGE("There is nothing here to pick up, %s!", game::Insult());

  return ToBeReturned;
}

bool character::Quit()
{
  if(game::BoolQuestion("Thine Holy Quest is not yet compeleted! Really quit? [y/N]"))
    {
      AddScoreEntry("cowardly quit the game", 0.75f);
      game::End();
      return true;
    }
  else
    return false;
}

void character::CreateCorpse()
{
  corpse* Corpse = new corpse(0, false);
  Corpse->SetDeceased(this);
  GetLSquareUnder()->GetStack()->AddItem(Corpse);
  SetHasBe(false);
}

void character::Die(bool ForceMsg)
{
  // Note for programmers: This function MUST NOT delete any objects in any case! 

  if(!IsEnabled())
    return;

  if(IsPlayer())
    {
      ADD_MESSAGE("You die.");

      if(game::WizardModeActivated())
	{
	  game::DrawEverything();

	  if(!game::BoolQuestion("Do you want to do this, cheater? [y/n]", REQUIRES_ANSWER))
	    {
	      RestoreBodyParts();
	      RestoreHP();
	      SetNP(10000);
	      GetSquareUnder()->SendNewDrawRequest();
	      return;
	    }
	}
    }
  else
    if(GetSquareUnder()->CanBeSeen())
      ADD_MESSAGE(GetDeathMessage().c_str());
    else if(ForceMsg)
      ADD_MESSAGE("You sense the death of something.");

  if(StateIsActivated(LIFE_SAVED))
    {
      SaveLife();
      return;
    }

  if(IsPlayer())
    game::RemoveSaves();

  if(HomeRoom && GetLSquareUnder()->GetLevelUnder()->GetRoom(HomeRoom)->GetMaster() == this)
    GetLSquareUnder()->GetLevelUnder()->GetRoom(HomeRoom)->SetMaster(0);

  GetSquareUnder()->RemoveCharacter();

  if(!game::IsInWilderness())
    CreateCorpse();

  if(IsPlayer())
    {
      if(!game::IsInWilderness())
	GetLSquareUnder()->SetTemporaryEmitation(GetEmitation());

      if(GetStack()->GetItems())
	if(game::BoolQuestion("Do you want to see your inventory? [y/n]", REQUIRES_ANSWER))
	  GetStack()->DrawContents(this, "Your inventory", false);

      if(game::BoolQuestion("Do you want to see your message history? [y/n]", REQUIRES_ANSWER))
	DrawMessageHistory();

      if(!game::IsInWilderness())
	GetLSquareUnder()->SetTemporaryEmitation(0);
    }

  if(!game::IsInWilderness())
    {
      lsquare* Square = GetLSquareUnder();

      while(GetStack()->GetItems())
	GetStack()->MoveItem(GetStack()->GetBottomSlot(), Square->GetStack());

      for(ushort c = 0; c < BodyParts(); ++c)
	if(GetBodyPart(c))
	  GetBodyPart(c)->DropEquipment();

      if(GetAction())
	{
	  GetAction()->DropUsedItems();
	  delete GetAction();
	  SetAction(0);
	}
    }
  else
    {
      /* Drops the equipment to the character's stack */

      for(ushort c = 0; c < BodyParts(); ++c)
	if(GetBodyPart(c))
	  GetBodyPart(c)->DropEquipment();

      if(GetAction())
	{
	  GetAction()->DeleteUsedItems();
	  delete GetAction();
	  SetAction(0);
	}

      while(GetStack()->GetItems())
	{
	  GetStack()->GetBottomItem()->SendToHell();
	  GetStack()->RemoveItem(GetStack()->GetBottomSlot());
	}
    }

  if(GetTeam()->GetLeader() == this)
    GetTeam()->SetLeader(0);

  if(IsPlayer())
    {
      game::TextScreen("Unfortunately thee died during thine journey. The High Priest is not happy.");
      game::End();
    }
}

bool character::OpenItem()
{
  item* Item = Stack->DrawContents(this, "What do you want to open?", &item::OpenableSorter);

  if(Item)
    if(Item->Open(this))
      {
	EditExperience(DEXTERITY, 25);
	EditNP(-10);
	EditAP(-500);
	return true;
      }
    else
      return false;

  return false;
}

void character::AddMissMessage(character* Enemy) const
{
  std::string Msg;

  if(Enemy->IsPlayer())
    Msg = Description(DEFINITE) + " misses you!";
  else if(IsPlayer())
    Msg = "You miss " + Enemy->Description(DEFINITE) + "!";
  else if(GetSquareUnder()->CanBeSeen() || Enemy->GetSquareUnder()->CanBeSeen())
    Msg = Description(DEFINITE) + " misses " + Enemy->Description(DEFINITE) + "!";
  else
    return;

  ADD_MESSAGE("%s", Msg.c_str());
}

void character::AddBlockMessage(character* Enemy, item* Blocker, const std::string& HitNoun, bool Partial) const
{
  std::string Msg;
  std::string BlockVerb = (Partial ? " to partially block the " : " to block the ") + HitNoun;

  if(Enemy->IsPlayer())
    Msg = "You manage" + BlockVerb + " with your " + Blocker->GetName(UNARTICLED) + "!";
  else if(IsPlayer() || GetSquareUnder()->CanBeSeen() || Enemy->GetSquareUnder()->CanBeSeen())
    Msg = Description(DEFINITE) + " manages" + BlockVerb + " with " + PossessivePronoun() + " " + Blocker->GetName(UNARTICLED) + "!";
  else
    return;

  ADD_MESSAGE("%s", Msg.c_str());
}

void character::AddPrimitiveHitMessage(character* Enemy, const std::string& FirstPersonHitVerb, const std::string& ThirdPersonHitVerb, uchar BodyPart) const
{
  std::string Msg;
  std::string BodyPartDescription = BodyPart && Enemy->GetSquareUnder()->CanBeSeen() ? " in " + Enemy->GetBodyPart(BodyPart)->GetName(DEFINITE) : "";

  if(Enemy->IsPlayer())
    Msg = Description(DEFINITE) + " " + ThirdPersonHitVerb + " you" + BodyPartDescription + "!";
  else if(IsPlayer())
    Msg = "You " + FirstPersonHitVerb + " " + Enemy->Description(DEFINITE) + BodyPartDescription + "!";
  else if(GetSquareUnder()->CanBeSeen() || Enemy->GetSquareUnder()->CanBeSeen())
    Msg = Description(DEFINITE) + " " + ThirdPersonHitVerb + " " + Enemy->Description(DEFINITE) + BodyPartDescription + "!";
  else
    return;

  ADD_MESSAGE("%s", Msg.c_str());
}

void character::AddWeaponHitMessage(character* Enemy, item* Weapon, uchar BodyPart, bool Critical) const
{
  std::string Msg;
  std::string BodyPartDescription = BodyPart && Enemy->GetSquareUnder()->CanBeSeen() ? " in " + Enemy->GetBodyPart(BodyPart)->GetName(DEFINITE) : "";

  if(Enemy->IsPlayer())
    {
      Msg = Description(DEFINITE) + (Critical ? " critically hits you" : " hits you") + BodyPartDescription;

      if(GetSquareUnder()->CanBeSeen())
	Msg += " with " + PossessivePronoun() + " " + Weapon->GetName(UNARTICLED);

      Msg += "!";
    }
  else if(IsPlayer())
    Msg = "You hit " + Enemy->Description(DEFINITE) + BodyPartDescription + "!";
  else if(GetSquareUnder()->CanBeSeen() || Enemy->GetSquareUnder()->CanBeSeen())
    {
      Msg = Description(DEFINITE) + (Critical ? " critically hits " : " hits ") + Enemy->Description(DEFINITE) + BodyPartDescription;

      if(GetSquareUnder()->CanBeSeen())
	Msg += " with " + PossessivePronoun() + " " + Weapon->GetName(UNARTICLED);

      Msg += "!";
    }
  else
    return;

  ADD_MESSAGE("%s", Msg.c_str());
}

void character::BeTalkedTo(character*)
{
  ADD_MESSAGE("%s %s.", CHARDESCRIPTION(DEFINITE), GetTalkVerb().c_str());
}

bool character::Talk()
{
  if(!CanTalk())
    {
      ADD_MESSAGE("This race does not know the art of talking.");
      return false;
    }

  character* ToTalk = 0;
  ushort Characters = 0;

  DO_FOR_SQUARES_AROUND(GetPos().X, GetPos().Y, game::GetCurrentArea()->GetXSize(), game::GetCurrentArea()->GetYSize(),
    {
      character* Char = game::GetCurrentArea()->GetSquare(DoX, DoY)->GetCharacter();

      if(Char)
	{
	  ToTalk = Char;
	  ++Characters;
	}
    });

  if(!Characters)
    {
      ADD_MESSAGE("Find yourself someone to talk to first!");
      return false;
    }
  else if(Characters == 1)
    {
      TalkTo(ToTalk);
      return true;
    }
  else
    {
      uchar Dir = game::DirectionQuestion("To whom do you wish to talk to? [press a direction key]", false, true);

      if(Dir == DIR_ERROR || !game::IsValidPos(GetPos() + game::GetMoveVector(Dir)))
	return false;

      if(Dir == YOURSELF)
	{
	  ADD_MESSAGE("You talk to yourself for some time.");
	  return true;
	}

      character* Char = game::GetCurrentArea()->GetSquare(GetPos() + game::GetMoveVector(Dir))->GetCharacter();

      if(Char)
	{
	  TalkTo(Char);
	  return true;
	}
      else
	ADD_MESSAGE("You get no response.");
    }

  return false;
}

void character::TalkTo(character* Who)
{
  if(Who->GetTeam()->GetRelation(GetTeam()) != HOSTILE)
    Who->EditAP(-1000);

  Who->BeTalkedTo(this);
}

bool character::NOP()
{
  EditExperience(AGILITY, -10);
  return true;
}

void character::ApplyExperience()
{
  for(ushort c = 0; c < BodyParts(); ++c)
    if(GetBodyPart(c))
      GetBodyPart(c)->ApplyExperience();

  if(CheckForAttributeIncrease(BaseAttribute[ENDURANCE], BaseExperience[ENDURANCE]))
    {
      if(IsPlayer())
	ADD_MESSAGE("You feel tougher than anything!");
      else
	if(game::IsInWilderness() || GetSquareUnder()->CanBeSeen())
	  ADD_MESSAGE("Suddenly %s looks tougher.", CHARNAME(DEFINITE));
    }
  else if(CheckForAttributeDecrease(BaseAttribute[ENDURANCE], BaseExperience[ENDURANCE]))
    {
      if(IsPlayer())
	ADD_MESSAGE("You feel less healthy.");
      else
	if(game::IsInWilderness() || GetSquareUnder()->CanBeSeen())
	  ADD_MESSAGE("Suddenly %s looks less healthy.", CHARNAME(DEFINITE));
    }

  if(CheckForAttributeIncrease(BaseAttribute[PERCEPTION], BaseExperience[PERCEPTION]))
    {
      if(IsPlayer())
	ADD_MESSAGE("Your don't feel as guru anymore.");
    }
  else if(CheckForAttributeDecrease(BaseAttribute[PERCEPTION], BaseExperience[PERCEPTION]))
    {
      if(IsPlayer())
	{
	  ADD_MESSAGE("You feel very guru.");
	  game::GetGod(1)->AdjustRelation(100);
	}
    }

  if(CheckForAttributeIncrease(BaseAttribute[INTELLIGENCE], BaseExperience[INTELLIGENCE]))
    {
      if(IsPlayer())
	ADD_MESSAGE("Suddenly the inner structure of the Multiverse around you looks quite simple.");
    }
  else if(CheckForAttributeDecrease(BaseAttribute[INTELLIGENCE], BaseExperience[INTELLIGENCE]))
    {
      if(IsPlayer())
	ADD_MESSAGE("It surely is hard to think today.");
    }

  if(CheckForAttributeIncrease(BaseAttribute[WISDOM], BaseExperience[WISDOM]))
    {
      if(IsPlayer())
	ADD_MESSAGE("You feel your life experience increasing all the time.");
    }
  else if(CheckForAttributeDecrease(BaseAttribute[WISDOM], BaseExperience[WISDOM]))
    {
      if(IsPlayer())
	ADD_MESSAGE("You feel like having done something unwise.");
    }

  if(CheckForAttributeIncrease(BaseAttribute[CHARISMA], BaseExperience[CHARISMA]))
    {
      if(IsPlayer())
	ADD_MESSAGE("You feel very confident of your appearance.");
      else
	if(game::IsInWilderness() || GetSquareUnder()->CanBeSeen())
	  if(GetAttribute(CHARISMA) <= 15)
	    ADD_MESSAGE("%s looks less ugly.", CHARNAME(DEFINITE));
	  else
	    ADD_MESSAGE("%s looks more attractive.", CHARNAME(DEFINITE));
    }
  else if(CheckForAttributeDecrease(BaseAttribute[CHARISMA], BaseExperience[CHARISMA]))
    {
      if(IsPlayer())
	ADD_MESSAGE("You feel somehow disliked.");
      else
	if(game::IsInWilderness() || GetSquareUnder()->CanBeSeen())
	  if(GetAttribute(CHARISMA) < 15)
	    ADD_MESSAGE("%s looks more ugly.", CHARNAME(DEFINITE));
	  else
	    ADD_MESSAGE("%s looks less attractive.", CHARNAME(DEFINITE));
    }

  if(CheckForAttributeIncrease(BaseAttribute[MANA], BaseExperience[MANA]))
    {
      if(IsPlayer())
	ADD_MESSAGE("You feel magical forces coursing through your body!");
      else
	if(game::IsInWilderness() || GetSquareUnder()->CanBeSeen())
	  ADD_MESSAGE("You notice an odd glow around %s.", CHARNAME(DEFINITE));
    }
  else if(CheckForAttributeDecrease(BaseAttribute[MANA], BaseExperience[MANA]))
    {
      if(IsPlayer())
	ADD_MESSAGE("You feel your magical abilities withering slowly.");
      else
	if(game::IsInWilderness() || GetSquareUnder()->CanBeSeen())
	  ADD_MESSAGE("You notice strange vibrations in the air around %s. But they disappear rapidly.", CHARNAME(DEFINITE));
    }
}

bool character::HasHeadOfElpuri() const
{
  for(stackiterator i = GetStack()->GetBottomSlot(); i != GetStack()->GetSlotAboveTop(); ++i)
    if((**i)->IsHeadOfElpuri())
      return true;

  return false;
}

bool character::HasPetrussNut() const
{
  for(stackiterator i = GetStack()->GetBottomSlot(); i != GetStack()->GetSlotAboveTop(); ++i)
    if((**i)->IsPetrussNut())
      return true;

  return false;
}

bool character::HasGoldenEagleShirt() const
{
  for(stackiterator i = GetStack()->GetBottomSlot(); i != GetStack()->GetSlotAboveTop(); ++i)
    if((**i)->IsGoldenEagleShirt())
      return true;

  return false;
}

bool character::Save()
{
  if(game::BoolQuestion("Dost thee truly wish to save and flee? [y/N]"))
    {
      game::Save();
      game::End(false);
      return true;
    }
  else
    return false;
}

bool character::Read()
{
  if(!CanRead() && !game::GetSeeWholeMapCheat())
    {
      ADD_MESSAGE("You can't read.");
      return false;
    }

  if(!GetStack()->GetItems())
    {
      ADD_MESSAGE("You have nothing to read!");
      return false;
    }

  item* Item = GetStack()->DrawContents(this, "What do you want to read?", &item::ReadableSorter);

  if(Item)
    return ReadItem(Item);
  else
    return false;
}

bool character::ReadItem(item* ToBeRead)
{
  if(ToBeRead->CanBeRead(this))
    if(GetLSquareUnder()->GetLuminance() >= LIGHT_BORDER || game::GetSeeWholeMapCheat())
      {
	bool LackOfLight = GetLSquareUnder()->GetRawLuminance() < 225 ? true : false;

	if(ToBeRead->Read(this))
	  {
	    ToBeRead->RemoveFromSlot();
	    ToBeRead->SendToHell();

	    if(LackOfLight)
	      EditExperience(PERCEPTION, -25);
	  }

	return true;
      }
    else
      {
	ADD_MESSAGE("It's too dark here to read.");
	return false;
      }
  else
    {
      if(IsPlayer())
	ADD_MESSAGE("You can't read this.");

      return false;
    }
}

uchar character::GetBurdenState(ulong Mass) const
{
  ulong SumOfMasses;
  if(!Mass)
    SumOfMasses = GetWeight();
  else
    SumOfMasses = Mass;
  if(SumOfMasses >= ulong(7500 * GetCarryingStrength()))
    return OVERLOADED;
  if(SumOfMasses >= ulong(5000 * GetCarryingStrength()))
    return STRESSED;
  if(SumOfMasses >= ulong(2500 * GetCarryingStrength()))
    return BURDENED;
  return UNBURDENED;
}

/*ulong character::GetTotalWeight() const
{
  ulong Weight = GetStack()->GetTotalWeight();

  for(ushort c = 0; c < BodyParts(); ++c)
    if(GetBodyPart(c))
      Weight += GetBodyPart(c)->GetWeight();

  if(GetAction())
    Weight += GetAction()->GetWeight();

  return Weight;
}*/

bool character::Dip()
{
  if(!GetStack()->SortedItems(this, &item::DippableSorter))
    {
      ADD_MESSAGE("You have nothing to dip!");
      return false;
    }

  item* Item = GetStack()->DrawContents(this, "What do you want to dip?", &item::DippableSorter);

  if(Item)
    {
      bool HasDipDestination = GetStack()->SortedItems(this, &item::DipDestinationSorter);

      if(!HasDipDestination || game::BoolQuestion("Do you wish to dip in a nearby square? [y/N]"))
	{
	  uchar Dir = game::DirectionQuestion("Where do you want to dip " + Item->GetName(DEFINITE) + "? [press a direction key or '.']", false, true);

	  if(Dir == DIR_ERROR || !game::IsValidPos(GetPos() + game::GetMoveVector(Dir)))
	    return false;
	  
	  return game::GetCurrentLevel()->GetLSquare(GetPos() + game::GetMoveVector(Dir))->DipInto(Item, this);
	}
      else
	{
	  if(!HasDipDestination)
	    {
	      ADD_MESSAGE("You have nothing to dip into!");
	      return false;
	    }

	  item* DipTo = GetStack()->DrawContents(this, "Into what do you wish to dip it?", &item::DipDestinationSorter);

	  if(DipTo && Item != DipTo)
	    {
	      Item->DipInto(DipTo->CreateDipMaterial(), this);
	      return true;
	    }
	}
    }

  return false;
}

void character::Save(outputfile& SaveFile) const
{
  SaveFile << GetType();
  //entity::Save(SaveFile);

  Stack->Save(SaveFile);

  ushort c;

  for(c = 0; c < BASEATTRIBUTES; ++c)
    SaveFile << BaseAttribute[c] << BaseExperience[c];

  SaveFile << NP << AP;
  SaveFile << TemporaryState << PermanentState << Money << HomeRoom << WayPoint << Config;
  SaveFile << HasBe();

  for(c = 0; c < BodyParts(); ++c)
    SaveFile << BodyPartSlot[c] << OriginalBodyPartID[c];

  if(HomeRoom)
    if(!game::IsInWilderness() && GetLSquareUnder()->GetLevelUnder()->GetRoom(HomeRoom)->GetMaster() == this)
      SaveFile << bool(true);
    else
      SaveFile << bool(false);

  SaveFile << Action;

  for(c = 0; c < STATES; ++c)
    SaveFile << TemporaryStateCounter[c];

  if(GetTeam())
    {
      SaveFile << bool(true);
      SaveFile << Team->GetID();
    }
  else
    SaveFile << bool(false);

  if(GetTeam() && GetTeam()->GetLeader() == this)
    SaveFile << bool(true);
  else
    SaveFile << bool(false);

  SaveFile << AssignedName << PolymorphBackup;

  for(c = 0; c < AllowedWeaponSkillCategories(); ++c)
    SaveFile << GetCategoryWeaponSkill(c);

  SaveFile << StuckTo << StuckToBodyPart;
}

void character::Load(inputfile& SaveFile)
{
  entity::Load(SaveFile);
  Stack->Load(SaveFile);

  ushort c;

  for(c = 0; c < BASEATTRIBUTES; ++c)
    SaveFile >> BaseAttribute[c] >> BaseExperience[c];

  SaveFile >> NP >> AP;
  SaveFile >> TemporaryState >> PermanentState >> Money >> HomeRoom >> WayPoint >> Config;
  SetHasBe(ReadType<bool>(SaveFile));

  for(c = 0; c < BodyParts(); ++c)
    {
      SaveFile >> BodyPartSlot[c] >> OriginalBodyPartID[c];

      if(*BodyPartSlot[c])
	{
	  EditVolume(BodyPartSlot[c]->GetVolume());
	  EditWeight(BodyPartSlot[c]->GetCarriedWeight());
	}
    }

  if(HomeRoom)
    if(ReadType<bool>(SaveFile))
      GetLSquareUnder()->GetLevelUnder()->GetRoom(HomeRoom)->SetMaster(this);

  SaveFile >> Action;

  if(Action)
    {
      Action->SetActor(this);
      EditVolume(Action->GetVolume());
      EditWeight(Action->GetWeight());
    }

  for(c = 0; c < STATES; ++c)
    SaveFile >> TemporaryStateCounter[c];

  if(ReadType<bool>(SaveFile))
    SetTeam(game::GetTeam(ReadType<ushort>(SaveFile)));

  if(ReadType<bool>(SaveFile))
    GetTeam()->SetLeader(this);

  SaveFile >> AssignedName >> PolymorphBackup;

  for(c = 0; c < AllowedWeaponSkillCategories(); ++c)
    SaveFile >> GetCategoryWeaponSkill(c);

  SaveFile >> StuckTo >> StuckToBodyPart;
  InstallDataBase();
}

bool character::WizardMode()
{
  if(!game::WizardModeActivated())
    {
      if(game::BoolQuestion("Do you want to cheat, cheater? This action cannot be undone. [y/N]"))
	{
	  game::ActivateWizardMode();
	  ADD_MESSAGE("Wizard mode activated.");

	  for(ushort x = 0; x < 5; ++x)
	    GetStack()->AddItem(new scrollofwishing);
	}
    }
  else
    {
      ADD_MESSAGE("Got some scrolls of wishing.");

      for(ushort x = 0; x < 5; ++x)
	GetStack()->AddItem(new scrollofwishing);
    }

  return false;

}

bool character::RaiseStats()
{
  ushort c = 0;

  for(c = 0; c < BodyParts(); ++c)
    if(GetBodyPart(c))
      GetBodyPart(c)->RaiseStats();

  for(c = 0; c < BASEATTRIBUTES; ++c)
    BaseAttribute[c] += 10;

  RestoreHP();
  game::SendLOSUpdateRequest();
  return false;
}

bool character::LowerStats()
{
  ushort c = 0;

  for(c = 0; c < BodyParts(); ++c)
    if(GetBodyPart(c))
      GetBodyPart(c)->LowerStats();

  for(c = 0; c < BASEATTRIBUTES; ++c)
    BaseAttribute[c] -= 10;

  RestoreHP();
  game::SendLOSUpdateRequest();
  return false;
}

bool character::GainAllItems()
{
  for(ushort c = 1; c < protocontainer<item>::GetProtoAmount(); ++c)
    {
      const item::prototype* Proto = protocontainer<item>::GetProto(c);

      if(!Proto->IsAbstract() && Proto->IsAutoInitializable())
	GetStack()->AddItem(Proto->Clone());

      for(item::databasemap::const_iterator i = Proto->GetConfig().begin(); i != Proto->GetConfig().end(); ++i)
	if(!i->second.IsAbstract && i->second.IsAutoInitializable)
	  GetStack()->AddItem(Proto->Clone(i->first));
    }

  return false;
}

bool character::SeeWholeMap()
{
  game::SeeWholeMap();
  return false;
}

bool character::IncreaseContrast()
{
  configuration::EditContrast(5);
  game::GetCurrentArea()->SendNewDrawRequest();
  return false;
}

bool character::DecreaseContrast()
{
  configuration::EditContrast(-5);
  game::GetCurrentArea()->SendNewDrawRequest();
  return false;
}

ushort character::GetEmitation() const
{
  ushort Emitation = GetBaseEmitation();

  for(ushort c = 0; c < BodyParts(); ++c)
    if(GetBodyPart(c) && GetBodyPart(c)->GetEmitation() > Emitation)
      Emitation = GetBodyPart(c)->GetEmitation();

  if(GetStack()->GetEmitation() > Emitation)
    Emitation = GetStack()->GetEmitation();

  return Emitation;
}

void character::SetSquareUnder(square* Square)
{
  entity::SetSquareUnder(Square);
  Stack->SetSquareUnder(Square);
}

bool character::WalkThroughWalls()
{
  game::GoThroughWalls();
  return false;
}

bool character::ShowKeyLayout()
{
  felist List("Keyboard Layout", WHITE, 0);

  List.AddDescription("");
  List.AddDescription("Key       Description");

  for(ushort c = 1; game::GetCommand(c); ++c)
    if(game::WizardModeActivated() || !game::GetCommand(c)->GetWizardModeFunction())
      {
	std::string Buffer;
	Buffer += game::GetCommand(c)->GetKey();
	Buffer.resize(10, ' ');
	List.AddEntry(Buffer + game::GetCommand(c)->GetDescription(), LIGHTGRAY);
      }

  List.Draw(vector2d(26, 42), 652, 30, MAKE_RGB(0, 0, 16), false);

  return false;
}

bool character::Look()
{
  vector2d Pos = game::PositionQuestion("Press direction keys to move cursor or esc to exit from the mode.", GetPos(), &game::LookHandler, &game::LookBadKeyHandler);

  if(game::WizardModeActivated() && Pos != vector2d(-1, -1))
    {
      character* Char = game::GetCurrentArea()->GetSquare(Pos)->GetCharacter();

      if(Char)
	Char->PrintInfo();
    }

  EditExperience(PERCEPTION, 1);
  EditAP(900);
  return true;
}

bool character::Engrave(const std::string& What)
{
  game::GetCurrentLevel()->GetLSquare(GetPos())->Engrave(What);
  return true;
}

bool character::WhatToEngrave()
{
  game::GetCurrentLevel()->GetLSquare(GetPos())->Engrave(game::StringQuestion("What do you want to engrave here?", vector2d(16, 6), WHITE, 0, 80, true));
  return false;
}

bool character::MoveRandomly()
{
  bool OK = false;

  for(ushort c = 0; c < 10 && !OK; ++c)
    {
      ushort ToTry = RAND() % 8;

      if(game::GetCurrentLevel()->IsValid(GetPos() + game::GetMoveVector(ToTry)))
	OK = TryMove(GetPos() + game::GetMoveVector(ToTry), false);
    }

  return OK;
}

bool character::TestForPickup(item* ToBeTested) const
{
  if(GetBurdenState(ToBeTested->GetWeight() + GetWeight()) != UNBURDENED)
    return false;

  return true;
}

bool character::OpenPos(vector2d APos)
{
  if(game::IsValidPos(APos) && game::GetCurrentLevel()->GetLSquare(APos)->Open(this))
    {
      EditExperience(AGILITY, 25);
      EditNP(-10);
      EditAP(GetLengthOfOpen(APos));
      return true;
    }
  else
    return false;
}

bool character::ClosePos(vector2d APos)
{
  if(game::IsValidPos(APos) && game::GetCurrentLevel()->GetLSquare(APos)->Close(this))
    {
      EditExperience(AGILITY, 25);
      EditNP(-10);
      EditAP(GetLengthOfClose(APos));
      return true;
    }
  else
    return false;
}

bool character::Pray()
{
  felist Panthenon("To Whom shall thee address thine prayers?", WHITE, 0);

  std::vector<uchar> KnownIndex;

  if(!GetLSquareUnder()->GetDivineMaster())
    {
      for(ushort c = 1; c < game::GetGods(); ++c)
	if(game::GetGod(c)->GetKnown())
	  {
	    bitmap Icon(igraph::GetSymbolGraphic(), c << 4, 0, 16, 16);
	    Panthenon.AddEntry(game::GetGod(c)->CompleteDescription(), LIGHTGRAY, &Icon);
	    KnownIndex.push_back(c);
	  }
    }
  else
    if(game::GetGod(GetLSquareUnder()->GetDivineMaster())->GetKnown())
      {
	bitmap Icon(igraph::GetSymbolGraphic(), GetLSquareUnder()->GetDivineMaster() << 4, 0, 16, 16);
	Panthenon.AddEntry(game::GetGod(GetLSquareUnder()->GetDivineMaster())->CompleteDescription(), LIGHTGRAY, &Icon);
      }
    else
      ADD_MESSAGE("Somehow you feel that no deity you know can hear your prayers from this place.");

  ushort Select = Panthenon.Draw(vector2d(26, 42), 652);

  if(Select & 0x8000)
    return false;
  else
    {
      if(GetLSquareUnder()->GetDivineMaster())
	{
	  if(!Select)
	    {
	      if(game::BoolQuestion("Do you really wish to pray to " + game::GetGod(GetLSquareUnder()->GetDivineMaster())->Name() + "? [y/N]"))
		game::GetGod(GetLSquareUnder()->GetDivineMaster())->Pray();
	      else
		return false;
	    }
	  else
	    return false;
	}
      else
	{
	  if(game::BoolQuestion("Do you really wish to pray to " + game::GetGod(KnownIndex[Select])->Name() + "? [y/N]"))
	    game::GetGod(KnownIndex[Select])->Pray();
	  else
	    return false;
	}

      return true;
    }
}

void character::SpillBlood(uchar HowMuch, vector2d GetPos)
{
  if(!HowMuch)
    return;

  if(!game::IsInWilderness()) 
    game::GetCurrentLevel()->GetLSquare(GetPos)->SpillFluid(HowMuch, GetBloodColor(), 5, 60);
}

void character::SpillBlood(uchar HowMuch)
{
  if(!HowMuch)
    return;

  if(!game::IsInWilderness()) 
    GetLSquareUnder()->SpillFluid(HowMuch, GetBloodColor(), 5, 60);
}

bool character::Kick()
{
  if(!CheckKick())
    return false;

  if(GetBurdenState() == OVERLOADED)
    {
      ADD_MESSAGE("You try to kick, but you collapse under your load.");
      return true;
    }

  uchar Dir = game::DirectionQuestion("In what direction do you wish to kick? [press a direction key]", false);

  if(Dir == DIR_ERROR || !game::IsValidPos(GetPos() + game::GetMoveVector(Dir)))
    return false;

  lsquare* Square = game::GetCurrentLevel()->GetLSquare(GetPos() + game::GetMoveVector(Dir));

  if(Square->GetCharacter() && GetTeam()->GetRelation(Square->GetCharacter()->GetTeam()) != HOSTILE)
    if(!game::BoolQuestion("This might cause a hostile reaction. Are you sure? [y/N]"))
      return false;
    else
      Hostility(Square->GetCharacter());

  Kick(Square);
  return true;
}

bool character::Offer()
{
  if(!CheckOffer())
    return false;

  if(GetLSquareUnder()->GetOLTerrain()->CanBeOffered())
    {
      if(!GetStack()->GetItems())
	{
	  ADD_MESSAGE("You have nothing to offer!");
	  return false;
	}

      item* Item = GetStack()->DrawContents(this, "What do you want to offer?");

      if(Item)
	{
	  if(game::GetGod(GetLSquareUnder()->GetOLTerrain()->GetDivineMaster())->ReceiveOffer(Item))
	    {
	      Item->RemoveFromSlot();
	      Item->SendToHell();
	      return true;
	    }
	  else
	    return false;
	}
      else
	return false;
    }
  else
    ADD_MESSAGE("There isn't any altar here!");

  return false;
}

long character::GetStatScore() const
{
  long SkillScore = 0;

  for(ushort c = 0; c < AllowedWeaponSkillCategories(); ++c)
    SkillScore += GetCategoryWeaponSkill(c)->GetHits();

  return (SkillScore >> 2) + (GetAttribute(ARMSTRENGTH) + GetAttribute(LEGSTRENGTH) + GetAttribute(DEXTERITY) + GetAttribute(AGILITY) + GetAttribute(ENDURANCE) + GetAttribute(PERCEPTION) + GetAttribute(INTELLIGENCE) + GetAttribute(WISDOM) + GetAttribute(CHARISMA) + GetAttribute(MANA)) * 40;
}

long character::GetScore() const
{
  return (GetPolymorphBackup() ? GetPolymorphBackup()->GetStatScore() : GetStatScore()) + GetMoney() / 5 + Stack->Score() + game::GodScore();
}

void character::AddScoreEntry(const std::string& Description, float Multiplier, bool AddEndLevel) const
{
  if(!game::WizardModeActivated())
    {
      highscore HScore;
      std::string Desc = game::GetPlayer()->GetAssignedName() + ", " + Description;

      if(AddEndLevel)
	Desc += " in " + (game::IsInWilderness() ? "the World map" : game::GetCurrentDungeon()->GetLevelDescription(game::GetCurrent()));

      HScore.Add(long((GetScore() - game::GetBaseScore()) * Multiplier), Desc);
      HScore.Save();
    }
}

bool character::CheckDeath(const std::string& Msg, bool ForceMsg)
{
  bool Dead = false;

  for(ushort c = 0; c < BodyParts(); ++c)
    if(BodyPartVital(c) && (!GetBodyPart(c) || GetBodyPart(c)->GetHP() < 1))
      {
	Dead = true;
	break;
      }

  if(Dead)
    {
      if(IsPlayer())
	AddScoreEntry(Msg);

      Die(ForceMsg);
      return true;
    }
  else
    return false;
}

bool character::CheckStarvationDeath(const std::string& Msg)
{
  if(GetNP() < 1)
    {
      if(IsPlayer())
	AddScoreEntry(Msg);

      Die();

      return true;
    }
  else
    return false;
}

bool character::DrawMessageHistory()
{
  msgsystem::DrawMessageHistory();
  return false;
}

bool character::Throw()
{
  if(!CheckThrow())
    return false;

  if(!GetStack()->GetItems())
    {
      ADD_MESSAGE("You have nothing to throw!");
      return false;
    }

  item* Item = GetStack()->DrawContents(this, "What do you want to throw?");

  if(Item)
    {
      uchar Answer = game::DirectionQuestion("In what direction do you wish to throw?  [press a direction key]", false);

      if(Answer == DIR_ERROR)
	return false;

      ThrowItem(Answer, Item);

      EditExperience(ARMSTRENGTH, 25);
      EditExperience(AGILITY, 25);
      EditExperience(PERCEPTION, 25);
      EditNP(-50);
      return true;
    }
  else
    return false;
}

bool character::ThrowItem(uchar Direction, item* ToBeThrown)
{
  if(Direction > 7)
    ABORT("Throw in TOO odd direction...");

  return ToBeThrown->Fly(this, Direction, GetAttribute(ARMSTRENGTH));
}

void character::HasBeenHitByItem(character* Thrower, item* Thingy, float Speed)
{
  ushort Damage = ushort(Thingy->GetWeaponStrength() * Thingy->GetWeight() * sqrt(Speed) / 5000000000.0f) + (RAND() % 5 ? 1 : 0);
  ReceiveDamage(Thrower, Damage, PHYSICALDAMAGE, ALL);

  if(IsPlayer())
    ADD_MESSAGE("%s hits you.", Thingy->CHARNAME(DEFINITE));
  else
    if(GetSquareUnder()->CanBeSeen())
      ADD_MESSAGE("%s hits %s.", Thingy->CHARNAME(DEFINITE), CHARNAME(DEFINITE));

  if(game::WizardModeActivated() && GetLSquareUnder()->CanBeSeen(true))
    ADD_MESSAGE("(damage: %d) (speed: %f)", Damage, Speed);

  SpillBlood(1 + RAND() % 1);
  CheckDeath("killed by a flying " + Thingy->GetName(UNARTICLED));
}

bool character::DodgesFlyingItem(item*, float Speed)
{			// Formula requires a little bit of tweaking...
  if(!(RAND() % 10))
    return RAND() % 2 ? true : false;

  if(!GetAttribute(AGILITY) || RAND() % ulong(sqrt(Speed) * GetSize() / GetAttribute(AGILITY) * 10 + 1) > 40)
    return false;
  else
    return true;
}

void character::GetPlayerCommand()
{
  bool HasActed = false;

  while(!HasActed)
    {
      game::DrawEverything();

      game::SetIsInGetCommand(true);
      int Key = GETKEY();
      game::SetIsInGetCommand(false);

      bool ValidKeyPressed = false;
      ushort c;

      for(c = 0; c < DIRECTION_COMMAND_KEYS; ++c)
	if(Key == game::GetMoveCommandKey(c))
	  {
	    HasActed = TryMove(GetPos() + game::GetMoveVector(c));
	    ValidKeyPressed = true;
	  }

      for(c = 1; game::GetCommand(c); ++c)
	if(Key == game::GetCommand(c)->GetKey())
	  {
	    if(game::IsInWilderness() && !game::GetCommand(c)->IsUsableInWilderness())
	      ADD_MESSAGE("This function cannot be used while in wilderness.");
	    else
	      if(!game::WizardModeActivated() && game::GetCommand(c)->GetWizardModeFunction())
		ADD_MESSAGE("Activate wizardmode to use this function.");
	      else
		HasActed = (this->*game::GetCommand(c)->GetLinkedFunction())();

	    ValidKeyPressed = true;
	    break;
	  }

      if(!ValidKeyPressed)
	ADD_MESSAGE("Unknown key, you %s. Press '?' for a list of commands.", game::Insult());
    }
}

void character::Vomit(ushort)
{
  if(IsPlayer())
    ADD_MESSAGE("You vomit.");
  else
    if(GetLSquareUnder()->CanBeSeen())
      ADD_MESSAGE("%s vomits.", CHARNAME(DEFINITE));

  EditExperience(ARMSTRENGTH, -50);
  EditExperience(LEGSTRENGTH, -50);
  EditExperience(ENDURANCE, 50);
  EditNP(-200 - RAND() % 201);

  GetLSquareUnder()->ReceiveVomit(this);
}

bool character::Apply()
{
  if(!CheckApply())
    return false;

  if(!GetStack()->GetItems())
    {
      ADD_MESSAGE("You have nothing to apply!");
      return false;
    }

  item* Item = GetStack()->DrawContents(this, "What do you want to apply?", &item::AppliableSorter);

  if(Item)
    {
      if(!Item->Apply(this))
	return false;

      return true;
    }
  else
    return false;
}

vector2d character::GetPos() const
{
  return SquareUnder->GetPos();
}

bool character::ForceVomit()
{
  ADD_MESSAGE("You push your fingers down to your throat and vomit.");
  Vomit(1 + RAND() % 3);
  return true;
}

bool character::Zap()
{
  if(!GetStack()->GetItems())
    {
      ADD_MESSAGE("You have nothing to zap with.");
      return false;
    }

  item* Item = GetStack()->DrawContents(this, "What do you want to zap with?", &item::ZappableSorter);

  if(Item)
    {
      uchar Answer = game::DirectionQuestion("In what direction do you wish to zap?  [press a direction key or '.']", false, true);

      if(Answer == DIR_ERROR)
	return false;

      return Item->Zap(this, GetPos(), Answer);
    }
  else
    return false;
}

bool character::Polymorph(character* NewForm, ushort Counter)
{
  if(!IsPolymorphable() || (!IsPlayer() && game::IsInWilderness()))
    {
      delete NewForm;
      return false;
    }

  if(GetAction())
    GetAction()->Terminate(false);

  NewForm->SetAssignedName("");

  if(IsPlayer())
    ADD_MESSAGE("Your body glows in a crimson light. You transform into %s!", NewForm->CHARNAME(INDEFINITE));
  else if(GetSquareUnder()->CanBeSeen())
    ADD_MESSAGE("%s glows in a crimson light and %s transform into %s!", CHARNAME(DEFINITE), PersonalPronoun().c_str(), NewForm->CHARNAME(INDEFINITE));

  GetSquareUnder()->RemoveCharacter();
  GetSquareUnder()->AddCharacter(NewForm);
  SetSquareUnder(0);
  NewForm->SetAssignedName(GetAssignedName());
  NewForm->ActivateTemporaryState(POLYMORPHED);
  NewForm->SetTemporaryStateCounter(POLYMORPHED, Counter);

  if(TemporaryStateIsActivated(POLYMORPHED))
    {
      NewForm->SetPolymorphBackup(GetPolymorphBackup());
      SetPolymorphBackup(0);
      SendToHell();
    }
  else
    {
      NewForm->SetPolymorphBackup(this);
      SetHasBe(false);
    }

  while(GetStack()->GetItems())
    GetStack()->MoveItem(GetStack()->GetBottomSlot(), NewForm->GetStack());

  NewForm->SetMoney(GetMoney());

  for(ushort c = 0; c < EquipmentSlots(); ++c)
    {
      item* Item = GetEquipment(c);

      if(Item)
	{
	  if(NewForm->CanUseEquipment() && NewForm->CanUseEquipment(c))
	    {
	      Item->RemoveFromSlot();
	      NewForm->SetEquipment(c, Item);
	    }
	  else
	    Item->MoveTo(NewForm->GetStack());
	}
    }

  NewForm->ChangeTeam(GetTeam());

  if(GetTeam()->GetLeader() == this)
    GetTeam()->SetLeader(NewForm);

  if(IsPlayer())
    {
      game::SetPlayer(NewForm);
      game::SendLOSUpdateRequest();
    }

  return true;
}

wsquare* character::GetWSquareUnder() const
{
  return (wsquare*)SquareUnder;
}

void character::BeKicked(character* Kicker, float KickStrength, float ToHitValue, short Success, bool Critical)
{
  switch(TakeHit(Kicker, 0, KickStrength, ToHitValue, Success, KICKATTACK, Critical))
    {
    case HASHIT:
    case HASBLOCKED:
      if(CheckBalance(KickStrength))
	{
	  if(IsPlayer())
	    ADD_MESSAGE("The kick throws you off balance.");
	  else if(Kicker->IsPlayer())
	    ADD_MESSAGE("The kick throws %s off balance.", CHARDESCRIPTION(DEFINITE));

	  if(game::IsValidPos((GetPos() << 1) - Kicker->GetPos()))
	    FallTo(Kicker, (GetPos() << 1) - Kicker->GetPos());
	}
    }
}

bool character::CheckBalance(float KickStrength)
{
  return KickStrength / 2000 >= RAND() % GetSize();
}

void character::FallTo(character* GuiltyGuy, vector2d Where)
{
  EditAP(-500);

  if(game::GetCurrentLevel()->GetLSquare(Where)->GetOLTerrain()->IsWalkable() && !game::GetCurrentLevel()->GetLSquare(Where)->GetCharacter())
    Move(Where, true);

  if(!game::GetCurrentLevel()->GetLSquare(Where)->GetOLTerrain()->IsWalkable())
    {
      if(IsPlayer()) 
	ADD_MESSAGE("You hit your head on the wall.");
      else if(GetSquareUnder()->CanBeSeen())
	ADD_MESSAGE("%s hits %s head on the wall.", CHARNAME(DEFINITE), PossessivePronoun().c_str());

      ReceiveDamage(GuiltyGuy, 1 + RAND() % 5, PHYSICALDAMAGE, HEAD);
      CheckDeath("killed by hitting a wall");
    }

  // Place code that handles characters bouncing to each other here
}

bool character::CheckCannibalism(ushort What) const
{ 
  return GetTorso()->GetConsumeMaterial()->GetType() == What; 
}

void character::StandIdleAI()
{
  SeekLeader();

  if(CheckForEnemies(true))
    return;

  if(CheckForUsefulItemsOnGround())
    return;

  if(FollowLeader())
    return;

  if(CheckForDoors())
    return;
}

/*bool character::ShowWeaponSkills()
{
  ADD_MESSAGE("This race isn't capable of developing weapon skill experience!");
  return false;
}*/

void character::Faint()
{
  if(IsPlayer())
    ADD_MESSAGE("You faint.");
  else
    if(GetLSquareUnder()->CanBeSeen())
      ADD_MESSAGE("%s faints.", CHARNAME(DEFINITE));

  faint* Faint = new faint(this);
  Faint->SetCounter(100 + RAND() % 101);
  SetAction(Faint);
}

void character::DeActivateVoluntaryAction(const std::string& Reason)
{
  if(GetAction() && GetAction()->IsVoluntary())
    {
      if(IsPlayer() && Reason != "")
	ADD_MESSAGE("%s.", Reason.c_str());

      GetAction()->Terminate(false);
    }
}

void character::ActionAutoTermination()
{
  if(!GetAction() || !GetAction()->IsVoluntary())
    return;

  for(ushort c = 0; c < game::GetTeams(); ++c)
    if(GetTeam()->GetRelation(game::GetTeam(c)) == HOSTILE)
      for(std::list<character*>::const_iterator i = game::GetTeam(c)->GetMember().begin(); i != game::GetTeam(c)->GetMember().end(); ++i)
	if((*i)->IsEnabled() && ((IsPlayer() && (*i)->GetSquareUnder()->CanBeSeen()) || (!IsPlayer() && (*i)->GetSquareUnder()->CanBeSeenFrom(GetPos(), LOSRangeSquare(), HasInfraVision()))))
	  {
	    ADD_MESSAGE("%s seems to be hostile.", (*i)->CHARNAME(DEFINITE));
	    GetAction()->Terminate(false);
	    return;
	  }
}

bool character::CheckForEnemies(bool CheckDoors)
{
  bool HostileCharsNear = false;

  character* NearestChar = 0;
  ulong NearestDistance = 0xFFFFFFFF;

  for(ushort c = 0; c < game::GetTeams(); ++c)
    if(GetTeam()->GetRelation(game::GetTeam(c)) == HOSTILE)
      for(std::list<character*>::const_iterator i = game::GetTeam(c)->GetMember().begin(); i != game::GetTeam(c)->GetMember().end(); ++i)
	if((*i)->IsEnabled())
	  {
	    ulong ThisDistance = GetHypotSquare(long((*i)->GetPos().X) - GetPos().X, long((*i)->GetPos().Y) - GetPos().Y);

	    if(ThisDistance <= LOSRangeSquare())
	      HostileCharsNear = true;

	    if((ThisDistance < NearestDistance || (ThisDistance == NearestDistance && !(RAND() % 3))) && (*i)->GetLSquareUnder()->CanBeSeenFrom(GetPos(), LOSRangeSquare(), HasInfraVision()))
	      {
		NearestChar = *i;
		NearestDistance = ThisDistance;
	      }
	  }

  if(NearestChar)
    {
      if(SpecialEnemySightedReaction(NearestChar))
	return true;

      if(!GetTeam()->GetLeader() && NearestChar->IsPlayer())
	WayPoint = NearestChar->GetPos();

      return MoveTowards(NearestChar->GetPos());
    }
  else
    if(!GetTeam()->GetLeader() && WayPoint.X != -1)
      if(!MoveTowards(WayPoint))
	{
	  WayPoint.X = -1;
	  return false;
	}
      else
	return true;
    else
      if((!GetTeam()->GetLeader() || (GetTeam()->GetLeader() && WayPoint.X == -1)) && HostileCharsNear)
	{
	  if(CheckDoors && CheckForDoors())
	    return true;

	  MoveRandomly(); // one has heard that an enemy is near but doesn't know where
	  return true;
	}
      else
	return false;
}

bool character::CheckForDoors()
{
  if(CanOpen())
    {
      DO_FOR_SQUARES_AROUND(GetPos().X, GetPos().Y, game::GetCurrentLevel()->GetXSize(), game::GetCurrentLevel()->GetYSize(),
			    if(game::GetCurrentLevel()->GetLSquare(DoX, DoY)->GetOLTerrain()->CanBeOpenedByAI() && game::GetCurrentLevel()->GetLSquare(DoX, DoY)->Open(this))
			    return true;);
    }

  return false;
}

bool character::CheckForUsefulItemsOnGround()
{
  for(stackiterator i = GetLSquareUnder()->GetStack()->GetBottomSlot(); i != GetLSquareUnder()->GetStack()->GetSlotAboveTop(); ++i)
    {
//<<<charba.cpp
      /*if(CanWear() && (**i)->GetArmorValue() < CalculateArmorModifier() && GetBurdenState(GetStack()->SumOfMasses() + (**i)->GetWeight() - (GetBodyArmor() ? GetBodyArmor()->GetWeight() : 0)) == UNBURDENED)
	if(!GetLSquareUnder()->GetRoom() || GetLSquareUnder()->GetLevelUnder()->GetRoom(GetLSquareUnder()->GetRoom())->PickupItem(this, ***i))
	  {
	    item* ToWear = GetLSquareUnder()->GetStack()->MoveItem(c, GetStack());

      /if(CanWear() && GetLSquareUnder()->GetStack()->GetItem(c)->GetArmorValue() < CalculateArmorModifier() && GetBurdenState(GetStack()->SumOfMasses() + GetLSquareUnder()->GetStack()->GetItem(c)->GetWeight() - (GetTorsoArmor() ? GetTorsoArmor()->GetWeight() : 0)) == UNBURDENED)
	if(!GetLSquareUnder()->GetRoom() || GetLSquareUnder()->GetLevelUnder()->GetRoom(GetLSquareUnder()->GetRoom())->PickupItem(this, GetLSquareUnder()->GetStack()->GetItem(c)))
	{
	item* ToWear = GetLSquareUnder()->GetStack()->MoveItem(c, GetStack());
>>1.202

	    if(GetBodyArmor())
	      GetStack()->MoveItem(GetStack()->SearchItem(GetBodyArmor()), GetLSquareUnder()->GetStack());
==
	if(GetTorsoArmor())
	GetStack()->MoveItem(GetStack()->SearchItem(GetTorsoArmor()), GetLSquareUnder()->GetStack());
>> 1.202

	    SetBodyArmor(ToWear);
=
	SetTorsoArmor(ToWear);

<< charba.cpp
	    if(GetLSquareUnder()->CanBeSeen())
	      ADD_MESSAGE("%s picks up and wears %s.", CHARNAME(DEFINITE), ToWear->CHARNAME(DEFINITE));
=
	if(GetLSquareUnder()->CanBeSeen())
	ADD_MESSAGE("%s picks up and wears %s.", CGetName(DEFINITE), ToWear->CGetName(DEFINITE));

	return true;
	}*/

      /*if(CanWield() && long((**i)->GetWeaponStrength()) > long(GetAttackStrength()) && GetBurdenState(GetStack()->SumOfMasses() + (**i)->GetWeight() - (GetWielded() ? GetWielded()->GetWeight() : 0)) == UNBURDENED)
	if(!GetLSquareUnder()->GetRoom() || GetLSquareUnder()->GetLevelUnder()->GetRoom(GetLSquareUnder()->GetRoom())->PickupItem(this, ***i))
	  {
	    //item* ToWield = GetLSquareUnder()->GetStack()->MoveItem(c, GetStack());
	    item* ToWield = ***i;
	    ToWield->MoveTo(GetStack());

	    if(GetWielded())
	      GetWielded()->MoveTo(GetLSquareUnder()->GetStack());

	    SetWielded(ToWield);

	    if(GetLSquareUnder()->CanBeSeen())
	      ADD_MESSAGE("%s picks up and wields %s.", CHARNAME(DEFINITE), ToWield->CHARNAME(DEFINITE));

	    return true;
	  }*/

      if((**i)->IsConsumable(this) && !(**i)->IsBadFoodForAI(this) && (**i)->IsPickable(this))
	if(!GetLSquareUnder()->GetRoom() || GetLSquareUnder()->GetLevelUnder()->GetRoom(GetLSquareUnder()->GetRoom())->ConsumeItem(this, ***i))
	  {
	    if(GetLSquareUnder()->CanBeSeen())
	      ADD_MESSAGE("%s begins %s %s.", CHARNAME(DEFINITE), (**i)->GetConsumeVerb().c_str(), (**i)->CHARNAME(DEFINITE));

	    ConsumeItem(***i);
	    return true;
	  }
    }

  return false;
}

bool character::FollowLeader()
{
  if(!GetTeam()->GetLeader())
    return false;

  if(GetTeam()->GetLeader()->GetLSquareUnder()->CanBeSeenFrom(GetPos(), LOSRangeSquare(), HasInfraVision()))
    {
      vector2d Distance = GetPos() - WayPoint;

      if(abs(short(Distance.X)) <= 2 && abs(short(Distance.Y)) <= 2)
	return false;
      else
	return MoveTowards(WayPoint);
    }
  else
    if(WayPoint.X != -1)
      if(!MoveTowards(WayPoint))
	{
	  WayPoint.X = -1;
	  return false;
	}
      else
	return true;
    else
      return false;
}

void character::SeekLeader()
{
  if(GetTeam()->GetLeader() && GetTeam()->GetLeader()->GetLSquareUnder()->CanBeSeenFrom(GetPos(), LOSRangeSquare(), HasInfraVision()))
    WayPoint = GetTeam()->GetLeader()->GetPos();
}

bool character::RestUntilHealed()
{
  long HPToRest = game::NumberQuestion("How many hit points you desire?", vector2d(16, 6), WHITE);

  if(HPToRest <= GetHP())
    {
      ADD_MESSAGE("Your HP is already %d.", GetHP());
      return false;
    }

  GetSquareUnder()->GetOTerrain()->ShowRestMessage(this);
  rest* Rest = new rest(this);
  Rest->SetGoalHP(HPToRest);
  SetAction(Rest);
  return true;
}

bool character::OutlineCharacters()
{
  configuration::SetOutlineCharacters(!configuration::GetOutlineCharacters());
  game::GetCurrentArea()->SendNewDrawRequest();
  return false;
}

bool character::OutlineItems()
{
  configuration::SetOutlineItems(!configuration::GetOutlineItems());
  game::GetCurrentArea()->SendNewDrawRequest();
  return false;
}

float character::GetBattleAttributeModifier() const
{
  switch(GetBurdenState())
    {
    case BURDENED:
      return 0.75f;
    case STRESSED:
    case OVERLOADED:
      return 0.5f;
    default:
      return 1.0f;
    }
}

ulong character::CurrentDanger() const
{
  ulong AttrDanger = ulong(GetAttackStrengthDanger() / 100000);
  ulong TotalDanger = 0;
  ushort TotalWeight = 0;

  for(ushort c = 0; c < BodyParts(); ++c)
    if(GetBodyPart(c))
      {
	TotalWeight += GetBodyPart(c)->DangerWeight();
	TotalDanger += GetBodyPart(c)->Danger(AttrDanger, false);
      }

  return AttrDanger * TotalDanger / TotalWeight;
}

ulong character::MaxDanger() const
{
  ulong AttrDanger = ulong(GetAttackStrengthDanger() / 100000);
  ulong TotalDanger = 0;
  ushort TotalWeight = 0;

  for(ushort c = 0; c < BodyParts(); ++c)
    if(GetBodyPart(c))
      {
	TotalWeight += GetBodyPart(c)->DangerWeight();
	TotalDanger += GetBodyPart(c)->Danger(AttrDanger, true);
      }

  return AttrDanger * TotalDanger / TotalWeight;
}

bool character::RaiseGodRelations()
{
  for(ushort c = 1; c < game::GetGods(); ++c)
    game::GetGod(c)->AdjustRelation(50);

  return false;
}

bool character::LowerGodRelations()
{
  for(ushort c = 1; c < game::GetGods(); ++c)
    game::GetGod(c)->AdjustRelation(-50);

  return false;
}

ushort character::LOSRange() const
{
  return GetAttribute(PERCEPTION) * game::GetCurrentArea()->GetLOSModifier() / 48;
}

ushort character::LOSRangeSquare() const
{
  ulong LOSModifier = game::GetCurrentArea()->GetLOSModifier();

  return GetAttribute(PERCEPTION) * GetAttribute(PERCEPTION) * LOSModifier * LOSModifier / 2304;
}

bool character::GainDivineKnowledge()
{
  for(ushort c = 1; c < game::GetGods(); ++c)
    game::GetGod(c)->SetKnown(true);

  return false;
}

bool character::Displace(character* Who)
{
  if(GetBurdenState() == OVERLOADED)
    {
      if(IsPlayer())
	ADD_MESSAGE("You try to use your claws to crawl forward. But your load is too heavy.");

      return true;
    }

  if(CurrentDanger() > Who->CurrentDanger() && Who->CanBeDisplaced() && (!Who->GetAction() || Who->GetAction()->AllowDisplace()))
    {
      if(IsPlayer())
	if(GetSquareUnder()->CanBeSeen() || Who->GetSquareUnder()->CanBeSeen())
	  ADD_MESSAGE("You displace %s!", Who->CHARNAME(DEFINITE));
	else
	  ADD_MESSAGE("You displace something!");
      else
	if(Who->IsPlayer())
	  if(GetSquareUnder()->CanBeSeen() || Who->GetSquareUnder()->CanBeSeen())
	    ADD_MESSAGE("%s displaces you!", CHARNAME(DEFINITE));
	  else
	    ADD_MESSAGE("Something displaces you!");
	else
	  if(GetSquareUnder()->CanBeSeen() || Who->GetSquareUnder()->CanBeSeen())
	    ADD_MESSAGE("%s displaces %s!", CHARNAME(DEFINITE), Who->CHARNAME(DEFINITE));

      GetLSquareUnder()->SwapCharacter(Who->GetLSquareUnder());

      if(IsPlayer())
	ShowNewPosInfo();

      if(Who->IsPlayer())
	Who->ShowNewPosInfo();

      EditAP(-500);
      return true;
    }
  else
    {
      if(IsPlayer())
	if(Who->GetSquareUnder()->CanBeSeen())
	  ADD_MESSAGE("%s resists!", Who->CHARNAME(DEFINITE));
	else
	  ADD_MESSAGE("Something resists!");

      return false;
    }
}

bool character::Sit()
{
  if(GetLSquareUnder()->GetOLTerrain()->SitOn(this))
    return true;
  else
    return GetLSquareUnder()->GetGLTerrain()->SitOn(this);
}

void character::SetNP(long What)
{
  uchar OldGetHungerState = GetHungerState();
  NP = What;

  if(IsPlayer())
    {
      if(GetHungerState() == VERYHUNGRY && OldGetHungerState != VERYHUNGRY)
	{
	  game::Beep();
	  ADD_MESSAGE("You are getting very hungry.");
	  DeActivateVoluntaryAction();
	}
      else if(GetHungerState() == HUNGRY && OldGetHungerState != HUNGRY && OldGetHungerState != VERYHUNGRY)
	{
	  ADD_MESSAGE("You are getting hungry.");
	  DeActivateVoluntaryAction();
	}
    }
}
void character::ShowNewPosInfo() const
{
  if(GetPos().X < game::GetCamera().X + 2 || GetPos().X > game::GetCamera().X + game::GetScreenSize().X - 2)
    game::UpdateCameraX();

  if(GetPos().Y < game::GetCamera().Y + 2 || GetPos().Y > game::GetCamera().Y + game::GetScreenSize().Y - 2)
    game::UpdateCameraY();

  game::SendLOSUpdateRequest();

  if(!game::IsInWilderness())
    {
      if(GetLSquareUnder()->GetLuminance() < LIGHT_BORDER && !game::GetSeeWholeMapCheat())
	ADD_MESSAGE("It's dark in here!");

      if(GetLSquareUnder()->GetStack()->GetItems() > 0)
	{
	  if(GetLSquareUnder()->GetStack()->GetItems() > 1)
	    ADD_MESSAGE("Several items are lying here.");
	  else
	    ADD_MESSAGE("%s is lying here.", GetLSquareUnder()->GetStack()->GetBottomItem()->CHARNAME(INDEFINITE));
	}
		
      if(game::GetCurrentLevel()->GetLSquare(GetPos())->GetEngraved() != "")
	ADD_MESSAGE("Something has been engraved here: \"%s\"", game::GetCurrentLevel()->GetLSquare(GetPos())->GetEngraved().c_str());
    }
}

void character::Hostility(character* Enemy)
{
  if(GetTeam() != Enemy->GetTeam())
    GetTeam()->Hostility(Enemy->GetTeam());
  else if(Enemy->IsEnabled())
    {
      if(Enemy->GetSquareUnder()->CanBeSeen())
	ADD_MESSAGE("%s becomes enraged.", Enemy->CHARNAME(DEFINITE));

      Enemy->ChangeTeam(game::GetTeam(1));
    }
}

stack* character::GetGiftStack() const
{
  if(GetLSquareUnder()->GetRoom() && !GetLSquareUnder()->GetRoomClass()->AllowDropGifts())
    return GetStack();
  else
    return GetLSquareUnder()->GetStack();
}

bool character::MoveRandomlyInRoom()
{
  bool OK = false;

  for(ushort c = 0; c < 10 && !OK; ++c)
    {
      ushort ToTry = RAND() % 8;

      if(game::GetCurrentLevel()->IsValid(GetPos() + game::GetMoveVector(ToTry)) && !game::GetCurrentLevel()->GetLSquare(GetPos() + game::GetMoveVector(ToTry))->GetOLTerrain()->IsDoor())
	OK = TryMove(GetPos() + game::GetMoveVector(ToTry), false);
    }

  return OK;
}

bool character::Go()
{
  uchar Dir = game::DirectionQuestion("In what direction do you want to go? [press a direction key]", false);

  if(Dir == DIR_ERROR)
    return false;

  go* Go = new go(this);
  Go->SetDirection(Dir);
  uchar OKDirectionsCounter = 0;

  DO_FOR_SQUARES_AROUND(GetPos().X, GetPos().Y, game::GetCurrentLevel()->GetXSize(), game::GetCurrentLevel()->GetYSize(),
  {
    if(game::GetCurrentLevel()->GetLSquare(DoX, DoY)->IsWalkable(this))
      OKDirectionsCounter++;	
  });

  Go->SetWalkingInOpen(OKDirectionsCounter > 2 ? true : false);
  SetAction(Go);
  Go->Handle();
  return GetAction() ? true : false;
}

void character::GoOn(go* Go)
{
  if(GetAP() >= 0)
    {
      ActionAutoTermination();

      if(!GetAction())
	return;

      if(!game::IsValidPos(GetPos() + game::GetMoveVector(Go->GetDirection())))
	{
	  Go->Terminate(false);
	  return;
	}

      lsquare* MoveToSquare = game::GetCurrentLevel()->GetLSquare(GetPos() + game::GetMoveVector(Go->GetDirection()));

      if(!MoveToSquare->IsWalkable(this) || (MoveToSquare->GetCharacter() && GetTeam() != MoveToSquare->GetCharacter()->GetTeam()))
	{
	  Go->Terminate(false);
	  return;
	}

      uchar OKDirectionsCounter = 0;

      DO_FOR_SQUARES_AROUND(GetPos().X, GetPos().Y, game::GetCurrentLevel()->GetXSize(), game::GetCurrentLevel()->GetYSize(),
      {
	if(game::GetCurrentLevel()->GetLSquare(DoX, DoY)->IsWalkable(this))
	  OKDirectionsCounter++;	
      });

      if(!Go->GetWalkingInOpen())
	{
	  if(OKDirectionsCounter > 2)
	    {
	      Go->Terminate(false);
	      return;
	    }
	}
      else
	if(OKDirectionsCounter <= 2)
	  Go->SetWalkingInOpen(false);

      square* BeginSquare = GetSquareUnder();

      if(!TryMove(MoveToSquare->GetPos()) || BeginSquare == GetSquareUnder() || GetLSquareUnder()->GetLuminance() < LIGHT_BORDER || GetLSquareUnder()->GetStack()->GetItems())
	{
	  Go->Terminate(false);
	  return;
	}
    }
}

bool character::ShowConfigScreen()
{
  configuration::ShowConfigScreen();
  return false;
}

void character::SetTeam(team* What)
{
  Team = What;

  if(GetSquareUnder())
    GetSquareUnder()->SendNewDrawRequest();

  SetTeamIterator(GetTeam()->Add(this));
}

void character::ChangeTeam(team* What)
{
  if(GetTeam())
    GetTeam()->Remove(GetTeamIterator());

  Team = What;
  GetSquareUnder()->SendNewDrawRequest();

  if(GetTeam())
    SetTeamIterator(GetTeam()->Add(this));
}

bool character::ChangeRandomStat(short HowMuch)
{
  for(ushort c = 0; c < 50; ++c)
    {
      ushort AttribID = RAND() % ATTRIBUTES;

      if(EditAttribute(AttribID, HowMuch))
	return true;
    }

  return false;
}

uchar character::RandomizeReply(uchar Replies, bool* Said)
{
  bool NotSaid = false;

  for(ushort c = 0; c < Replies; ++c)
    if(!Said[c])
      {
	NotSaid = true;
	break;
      }

  if(!NotSaid)
    for(ushort c = 0; c < Replies; ++c)
      Said[c] = false;

  uchar ToSay;

  while(Said[ToSay = RAND() % Replies]);

  Said[ToSay] = true;

  return ToSay;
}

ushort character::DangerLevel() const
{
  static ulong DangerPointMinimum[] = { 0, 100, 500, 2500, 10000, 50000, 250000, 1000000, 5000000, 25000000 };

  for(ushort c = 9;; --c)
    if(CurrentDanger() >= DangerPointMinimum[c])
      return c;
}

void character::DisplayInfo()
{
  if(IsPlayer())
    ADD_MESSAGE("You are %s here.", GetStandVerb().c_str());
  else
    {
      ADD_MESSAGE("%s is %s here.", CHARNAME(INDEFINITE), GetStandVerb().c_str());

      std::string Msg = PersonalPronoun();

      if(GetTeam() == game::GetPlayer()->GetTeam())
	Msg += std::string(" is at danger level ") + DangerLevel();

      /*if(GetWielded())
	{
	  if(GetBodyArmor())
	    {
	      if(GetTeam() == game::GetPlayer()->GetTeam())
		Msg += " and";

	      Msg += " is wielding " + GetWielded()->GetName(INDEFINITE);

	      ADD_MESSAGE("%s.", Msg.c_str());
	      Msg = PersonalPronoun() + " wears " + GetBodyArmor()->GetName(INDEFINITE);
	    }
	  else
	    {
	      if(GetTeam() == game::GetPlayer()->GetTeam())
		Msg += ",";

	      Msg += " is wielding " + GetWielded()->GetName(INDEFINITE);
	    }

	  Msg += " and";
	}
      else if(GetBodyArmor())
	{
	  if(GetTeam() == game::GetPlayer()->GetTeam())
	    Msg += ",";

	  Msg += " is wearing " + GetBodyArmor()->GetName(INDEFINITE) + " and";
	}
      else*/
	if(GetTeam() == game::GetPlayer()->GetTeam())
	  Msg += " and";

      if(GetTeam() == game::GetPlayer()->GetTeam())
	ADD_MESSAGE("%s is tame.", Msg.c_str());
      else
	{
	  uchar Relation = GetTeam()->GetRelation(game::GetPlayer()->GetTeam());

	  if(Relation == HOSTILE)
	    ADD_MESSAGE("%s is hostile.", Msg.c_str());
	  else if(Relation == UNCARING)
	    ADD_MESSAGE("%s does not care about you.", Msg.c_str());
	  else
	    ADD_MESSAGE("%s is friendly.", Msg.c_str());
	}
    }

  if(game::WizardModeActivated())
    ADD_MESSAGE("(danger: %d)", CurrentDanger());
}

void character::TestWalkability()
{
  if(!GetSquareUnder()->IsWalkable(this))
    {
      bool Alive = false;

      while(true)
	{
	  DO_FOR_SQUARES_AROUND(GetPos().X, GetPos().Y, game::GetCurrentArea()->GetXSize(), game::GetCurrentArea()->GetYSize(),
	  {
	    vector2d Where(DoX, DoY);

	    if(GetSquareUnder()->GetAreaUnder()->GetSquare(Where)->IsWalkable(this) && !GetSquareUnder()->GetAreaUnder()->GetSquare(Where)->GetCharacter()) 
	      {
		ADD_MESSAGE("%s.", GetSquareUnder()->SurviveMessage(this).c_str());
		Move(Where, true); // actually, this shouldn't be a teleport move
		Alive = true;
		break;
	      }
	  });

	  break;
	}

      if(!Alive)
	{
	  if(IsPlayer())
	    {
	      GetSquareUnder()->RemoveCharacter();
	      ADD_MESSAGE("%s.", GetSquareUnder()->DeathMessage(this).c_str());
	      game::DrawEverything();
	      GETKEY();
	      game::GetPlayer()->AddScoreEntry(GetSquareUnder()->ScoreEntry(this));
	      game::End();
	    }
	  else
	    {
	      ADD_MESSAGE("%s.", GetSquareUnder()->MonsterDeathVerb(this).c_str());
	      Die();
	    }
	}
    }
}

material* character::CreateBodyPartBone(ushort, ulong Volume) const
{
  return MAKE_MATERIAL(BONE, Volume);
}

ushort character::GetSize() const
{
  if(GetTorso())
    return GetTorso()->GetSize();
  else
    return 0;
}

torso* character::GetTorso() const { return (torso*)GetBodyPart(0); }
humanoidtorso* character::GetHumanoidTorso() const { return (humanoidtorso*)GetBodyPart(0); }
void character::SetTorso(torso* What) { SetBodyPart(0, What); }

void character::SetMainMaterial(material* NewMaterial)
{
  NewMaterial->SetVolume(GetBodyPart(0)->GetMainMaterial()->GetVolume());
  GetBodyPart(0)->SetMainMaterial(NewMaterial);

  for(ushort c = 1; c < BodyParts(); ++c)
    {
      NewMaterial = NewMaterial->Clone(GetBodyPart(c)->GetMainMaterial()->GetVolume());
      GetBodyPart(c)->SetMainMaterial(NewMaterial);
    }
}

void character::ChangeMainMaterial(material* NewMaterial)
{
  NewMaterial->SetVolume(GetBodyPart(0)->GetMainMaterial()->GetVolume());
  GetBodyPart(0)->ChangeMainMaterial(NewMaterial);

  for(ushort c = 1; c < BodyParts(); ++c)
    {
      NewMaterial = NewMaterial->Clone(GetBodyPart(c)->GetMainMaterial()->GetVolume());
      GetBodyPart(c)->ChangeMainMaterial(NewMaterial);
    }
}

void character::SetSecondaryMaterial(material*)
{
  ABORT("Illegal character::SetSecondaryMaterial call!");
}

void character::ChangeSecondaryMaterial(material*)
{
  ABORT("Illegal character::ChangeSecondaryMaterial call!");
}

void character::SetContainedMaterial(material* NewMaterial)
{
  NewMaterial->SetVolume(GetBodyPart(0)->GetContainedMaterial()->GetVolume());
  GetBodyPart(0)->SetContainedMaterial(NewMaterial);

  for(ushort c = 1; c < BodyParts(); ++c)
    {
      NewMaterial = NewMaterial->Clone(GetBodyPart(c)->GetContainedMaterial()->GetVolume());
      GetBodyPart(c)->SetContainedMaterial(NewMaterial);
    }
}

void character::ChangeContainedMaterial(material* NewMaterial)
{
  NewMaterial->SetVolume(GetBodyPart(0)->GetContainedMaterial()->GetVolume());
  GetBodyPart(0)->SetContainedMaterial(NewMaterial);

  for(ushort c = 1; c < BodyParts(); ++c)
    {
      NewMaterial = NewMaterial->Clone(GetBodyPart(c)->GetContainedMaterial()->GetVolume());
      GetBodyPart(c)->ChangeContainedMaterial(NewMaterial);
    }
}

void character::TeleportRandomly()
{
  Move(game::GetCurrentLevel()->RandomSquare(this, true), true);
}

bool character::SecretKnowledge()
{
  /*felist List("Knowledge of the ancients:", WHITE, 0);

  std::string Buffer = "Name                                                 Weight       SV     Str";
  List.AddDescription(Buffer);

  for(ushort c = 1; c < protocontainer<item>::GetProtoAmount(); ++c)
    if(protocontainer<item>::GetProto(c)->IsAutoInitializable())
      {
	item* Item = protocontainer<item>::GetProto(c)->Clone();
	Buffer = Item->GetName(INDEFINITE);
	Buffer.resize(50,' ');
	Buffer += Item->GetWeight();
	Buffer.resize(63, ' ');
	Buffer += Item->GetStrengthValue();
	Buffer.resize(70, ' ');
	Buffer += ulong(Item->GetWeaponStrength() / 100);
	List.AddEntry(Buffer, LIGHTGRAY);
	delete Item;
      }

  List.Draw(vector2d(26, 42), 652, 10);*/
  return false;
}

short character::GetHP() const
{
  short HP = 0;

  for(ushort c = 0; c < BodyParts(); ++c)
    if(GetBodyPart(c))
      HP += GetBodyPart(c)->GetHP();

  return HP;
}

short character::GetMaxHP() const
{
  short HP = 0;

  for(ushort c = 0; c < BodyParts(); ++c)
    if(GetBodyPart(c))
      HP += GetBodyPart(c)->GetMaxHP();

  return HP;
}

void character::RestoreHP()
{
  for(ushort c = 0; c < BodyParts(); ++c)
    if(GetBodyPart(c))
      GetBodyPart(c)->SetHP(GetBodyPart(c)->GetMaxHP());
}

void character::BlockDamageType(uchar Type)
{
  switch(Type)
    {
    case PHYSICALDAMAGE:
      EditExperience(ARMSTRENGTH, 50);
      EditExperience(ENDURANCE, 50);
    }
}

bool character::AllowDamageTypeBloodSpill(uchar Type) const
{
  switch(Type)
    {
    case PHYSICALDAMAGE:
    case SOUND:
    case ENERGY:
      return true;
    case ACID:
    case FIRE:
    case POISON:
    case BULIMIA:
      return false;
    default:
      ABORT("Unknown blood effect destroyed dungeon!");
      return false;
    }
}

bool character::DamageTypeCanSeverBodyPart(uchar Type) const
{
  switch(Type)
    {
    case PHYSICALDAMAGE:
      return true;
    case SOUND:
    case ENERGY:
    case ACID:
    case FIRE:
    case POISON:
    case BULIMIA:
      return false;
    default:
      ABORT("Unknown reaping effect destroyed dungeon!");
      return false;
    }
}
/* Returns true if the damage really does some damage */
bool character::ReceiveBodyPartDamage(character* Damager, short Damage, uchar Type, uchar BodyPartIndex, uchar Direction, bool PenetrateResistance, bool Critical)
{
  bodypart* BodyPart = GetBodyPart(BodyPartIndex);

  if(!PenetrateResistance)
    Damage -= BodyPart->GetTotalResistance(Type);

  //Damage = SpecialProtection(Damager, Damage, Type, BodyPartIndex, Direction, PenetrateResistance, Critical);

  if(Damage < 1)
    if(Critical)
      Damage = 1;
    else
      {
	BlockDamageType(Type);
	return false;
      }

  if(Critical && AllowDamageTypeBloodSpill(Type))
    {
      SpillBlood(3 + RAND() % 3);

      DO_FOR_SQUARES_AROUND(GetPos().X, GetPos().Y, game::GetCurrentLevel()->GetXSize(), game::GetCurrentLevel()->GetYSize(),
      {
	vector2d Where(DoX, DoY);

	if(game::GetCurrentLevel()->GetLSquare(Where)->GetOTerrain()->IsWalkable()) 
	  SpillBlood(2 + RAND() % 2, Where);
      });
    }

  if(BodyPart->ReceiveDamage(Damager, Damage, Type) && DamageTypeCanSeverBodyPart(Type) && BodyPartCanBeSevered(BodyPartIndex))
    {
      if(IsPlayer())
	ADD_MESSAGE("Your %s is severed off!", BodyPart->CHARNAME(UNARTICLED));
      else if(GetSquareUnder()->CanBeSeen())
	ADD_MESSAGE("%s %s is severed off!", PossessivePronoun().c_str(), BodyPart->CHARNAME(UNARTICLED));

      SevereBodyPart(BodyPartIndex);
      GetSquareUnder()->SendNewDrawRequest();

      if(!game::IsInWilderness())
	{
	  GetLSquareUnder()->GetStack()->AddItem(BodyPart);

	  if(Direction != 8)
	    BodyPart->Fly(0, Direction, Damage);
	}
      else
	  GetStack()->AddItem(BodyPart);

      BodyPart->DropEquipment();

      if(IsPlayer())
	game::AskForKeyPress("Bodypart severed! [press any key to continue]");
    }

  return true;
}

bodypart* character::SevereBodyPart(ushort BodyPartIndex)
{
  bodypart* BodyPart = GetBodyPart(BodyPartIndex);
  BodyPart->SetOwnerDescription("of " + GetName(INDEFINITE));
  BodyPart->SetUnique(GetArticleMode() != NORMALARTICLE || AssignedName != "");
  BodyPart->RemoveFromSlot();
  return BodyPart;
}

/* The second uchar is actually TargetFlags, which is not used here, but seems to be used in humanoid::ReceiveDamage */

bool character::ReceiveDamage(character* Damager, short Damage, uchar Type, uchar, uchar Direction, bool, bool PenetrateArmor, bool Critical)
{
  bool Affected = ReceiveBodyPartDamage(Damager, Damage, Type, 0, Direction, PenetrateArmor, Critical);

  if(!Affected)
    {
      if(IsPlayer())
	ADD_MESSAGE("You are not hurt.");
      else if(GetSquareUnder()->CanBeSeen())
	ADD_MESSAGE("%s is not hurt.", PersonalPronoun().c_str());
    }

  if(DamageTypeAffectsInventory(Type))
    {
      for(ushort c = 0; c < EquipmentSlots(); ++c)
	if(GetEquipment(c))
	  GetEquipment(c)->ReceiveDamage(Damager, Damage, Type);

      GetStack()->ReceiveDamage(Damager, Damage, Type);
    }

  return Affected;
}

bool character::BodyPartVital(ushort Index)
{
  if(Index == 0)
    return true;
  else
    return false;
}

bool character::BodyPartCanBeSevered(ushort Index) const
{
  if(Index == 0)
    return false;
  else
    return true;
}

bool character::AssignName()
{
  vector2d Where = game::PositionQuestion("What do you want to name? [choose a square and press space bar or press ESC to quit]", GetPos());

  if(Where == vector2d(-1,-1) || Where == GetPos())
    return false;

  character* Character = game::GetCurrentLevel()->GetLSquare(Where)->GetCharacter();

  if(Character && Character->GetSquareUnder()->CanBeSeen())
    {
      if(Character->IsNameable())
	{
	  std::string Topic = "What do you want to call this " + Character->GetName(UNARTICLED) + "?";
	  std::string Name = game::StringQuestion(Topic, vector2d(16, 6), WHITE, 0, 80, true);
	  if(Name != "")
	    Character->SetAssignedName(Name);
	}
      else
	ADD_MESSAGE("%s refuses to be called anything else but %s.", Character->CHARNAME(DEFINITE), Character->CHARNAME(DEFINITE));
    }

  return false;
}

std::string character::Description(uchar Case) const
{
  if(GetSquareUnder()->CanBeSeen())
    return GetName(Case);
  else
    return "something";
}

std::string character::PersonalPronoun() const
{
  if(GetSex() == UNDEFINED || !GetSquareUnder()->CanBeSeen())
    return "it";
  else if(GetSex() == MALE)
    return "he";
  else
    return "she";
}

std::string character::PossessivePronoun() const
{
  if(GetSex() == UNDEFINED || !GetSquareUnder()->CanBeSeen())
    return "its";
  else if(GetSex() == MALE)
    return "his";
  else
    return "her";
}

std::string character::ObjectPronoun() const
{
  if(GetSex() == UNDEFINED || !GetSquareUnder()->CanBeSeen())
    return "it";
  else if(GetSex() == MALE)
    return "him";
  else
    return "her";
}

std::string character::GetMaterialDescription(bool Articled) const
{
  return GetTorso()->GetMainMaterial()->GetName(Articled);
}

std::string character::GetName(uchar Case) const
{
  if(!(Case & PLURAL) && AssignedName != "")
    {
      if(!ShowClassDescription())
	return AssignedName;
      else
	return AssignedName + " " + id::GetName((Case|ARTICLEBIT)&~INDEFINEBIT);
    }
  else
    return id::GetName(Case);
}

/*float character::GetAPStateMultiplier() const
{
  if(StateIsActivated(HASTE))
    return 2.0f;
  else if(StateIsActivated(SLOW))
    return 0.5f;
  else
    return 1.0f;
}*/

uchar character::GetHungerState() const
{
  if(GetNP() > BLOATEDLEVEL)
    return BLOATED;
  else if(GetNP() > SATIATEDLEVEL)
    return SATIATED;
  else if(GetNP() > NOTHUNGERLEVEL)
    return NOTHUNGRY;
  else if(GetNP() > HUNGERLEVEL)
    return HUNGRY;
  else
    return VERYHUNGRY;
}

bool character::EqupmentScreen()
{
  if(!CanUseEquipment())
    {
      ADD_MESSAGE("You cannot use equipment.");
      return false;
    }

  ushort Chosen = 0;
  bool EquipmentChanged = false;
  felist List("Equipment menu", WHITE);

  while(true)
    {
      game::DrawEverythingNoBlit();
      List.Empty();

      for(ushort c = 0; c < EquipmentSlots(); ++c)
	{
	  std::string Entry = EquipmentName(c) + ":";
	  Entry.resize(20, ' ');

	  if(!GetBodyPartOfEquipment(c))
	    Entry += "can't use";
	  else if(!GetEquipment(c))
	    Entry += "-";
	  else
	    Entry += GetEquipment(c)->GetName(INDEFINITE);

	  List.AddEntry(Entry, LIGHTGRAY);
	}

      Chosen = List.Draw(vector2d(26, 42), 652, 20, MAKE_RGB(0, 0, 16), true, false);

      if(Chosen >= EquipmentSlots())
	break;

      if(!GetBodyPartOfEquipment(Chosen))
	{
	  ADD_MESSAGE("Bodypart missing!");
	  continue;
	}

      item* OldEquipment = GetEquipment(Chosen);

      if(OldEquipment)
	GetEquipment(Chosen)->MoveTo(GetStack());

      if(!GetStack()->SortedItems(this, EquipmentSorter(Chosen)))
	{
	  ADD_MESSAGE("You haven't got any item that could be used for this purpose.");
	  continue;
	}
      else
	{
	  item* Item = GetStack()->DrawContents(this, "Choose " + EquipmentName(Chosen) + ":", EquipmentSorter(Chosen));

	  if(Item != OldEquipment)
	    EquipmentChanged = true;

	  if(Item)
	    {
	      Item->RemoveFromSlot();
	      SetEquipment(Chosen, Item);
	    }
	}
    }

  return EquipmentChanged;
}

bodypart* character::GetBodyPart(ushort Index) const
{
  return (bodypart*)GetBodyPartSlot(Index)->GetItem();
}

void character::SetBodyPart(ushort Index, bodypart* What)
{
  BodyPartSlot[Index].PutInItem(What);
  SetOriginalBodyPartID(Index, What ? What->GetID() : 0);
}

characterslot* character::GetBodyPartSlot(ushort Index) const
{
  return &BodyPartSlot[Index];
}

bool character::CanEat(material* Material) const
{
  return GetEatFlags() & Material->GetConsumeType() ? true : false;
}

void character::SetTemporaryStateCounter(uchar State, ushort What)
{
  for(ushort c = 0; c < STATES; ++c)
    if((1 << c) & TemporaryState)
      TemporaryStateCounter[c] = What;
}

void character::EditTemporaryStateCounter(uchar State, short What)
{
  for(ushort c = 0; c < STATES; ++c)
    if((1 << c) & TemporaryState)
      TemporaryStateCounter[c] += What;
}

ushort character::GetTemporaryStateCounter(uchar State) const
{
  for(ushort c = 0; c < STATES; ++c)
    if((1 << c) & TemporaryState)
      return TemporaryStateCounter[c];

  ABORT("Illegal GetTemporaryStateCounter request!");
  return 0;
}

bool character::CheckKick() const
{
  if(!CanKick())
    {
      ADD_MESSAGE("This race can't kick.");
      return false;
    }
  else
    return true;
}

ushort character::GetResistance(uchar Type) const
{
  switch(Type)
    {
    case PHYSICALDAMAGE: return GetPhysicalDamageResistance();
    case SOUND: return GetSoundResistance();
    case ENERGY: return GetEnergyResistance();
    case ACID: return GetAcidResistance();
    case FIRE: return GetFireResistance();
    case POISON: return GetPoisonResistance();
    case BULIMIA: return GetBulimiaResistance();
    default:
      ABORT("Resistance lack detected!");
      return 0;
    }
}

void character::SetDivineMaster(uchar Master)
{
  for(stackiterator i = GetStack()->GetBottomSlot(); i != GetStack()->GetSlotAboveTop(); ++i)
    (**i)->SetDivineMaster(Master);
}

bool character::ScrollMessagesDown()
{
  msgsystem::ScrollDown();
  return false;
}

bool character::ScrollMessagesUp()
{
  msgsystem::ScrollUp();
  return false;
}

void character::Regenerate(ushort Turns)
{
  for(ushort c = 0; c < BodyParts(); ++c)
    if(GetBodyPart(c))
      GetBodyPart(c)->Regenerate(Turns);
}

void character::PrintInfo() const
{
  felist Info("All information about " + GetName(INDEFINITE), WHITE, 0);
  Info.AddEntry(std::string("Endurance: ") + GetAttribute(ENDURANCE), LIGHTGRAY);
  Info.AddEntry(std::string("Perception: ") + GetAttribute(PERCEPTION), LIGHTGRAY);
  Info.AddEntry(std::string("Intelligence: ") + GetAttribute(INTELLIGENCE), LIGHTGRAY);
  Info.AddEntry(std::string("Wisdom: ") + GetAttribute(WISDOM), LIGHTGRAY);
  Info.AddEntry(std::string("Charisma: ") + GetAttribute(CHARISMA), LIGHTGRAY);
  Info.AddEntry(std::string("Mana: ") + GetAttribute(MANA), LIGHTGRAY);
  /*Info.AddEntry(std::string("Strength: ") + GetStrength(), LIGHTGRAY);
  Info.AddEntry(std::string("Endurance: ") + GetEndurance(), LIGHTGRAY);
  Info.AddEntry(std::string("Agility: ") + GetAgility(), LIGHTGRAY);
  Info.AddEntry(std::string("Perception: ") + GetPerception(), LIGHTGRAY);*/
  Info.AddEntry(std::string("Total weight: ") + GetWeight(), LIGHTGRAY);

  ushort c;

  for(c = 0; c < BodyParts(); ++c)
    if(GetBodyPart(c))
      Info.AddEntry(GetBodyPart(c)->GetName(UNARTICLED) + " armor value: " + GetBodyPart(c)->GetTotalResistance(PHYSICALDAMAGE), LIGHTGRAY);

  for(c = 0; c < EquipmentSlots(); ++c)
    {
      std::string Entry = EquipmentName(c) + ": ";

      if(!GetBodyPartOfEquipment(c))
	Entry += "can't use";
      else if(!GetEquipment(c))
	Entry += "-";
      else
	Entry += GetEquipment(c)->GetName(INDEFINITE);

      Info.AddEntry(Entry, LIGHTGRAY);
    }

  AddInfo(Info);
  Info.Draw(vector2d(26, 42), 652, 30, MAKE_RGB(0, 0, 16), false);
}

void character::CompleteRiseFromTheDead()
{
  for(ushort c = 0; c < BodyParts(); ++c)
    {
      if(GetBodyPart(c))
	GetBodyPart(c)->SetHP(1);
    }
}

bool character::RaiseTheDead(character*)
{
  bool Useful = false;
  for(ushort c = 0; c < BodyParts(); ++c)
    {
      if(!GetBodyPart(c))
	{
	  CreateBodyPart(c);
	  if(IsPlayer())
	    ADD_MESSAGE("Suddenly you grow a new %s.", GetBodyPart(c)->CHARNAME(UNARTICLED));
	  else if(GetLSquareUnder()->CanBeSeen())
	    ADD_MESSAGE("%s grows a new %s.", CHARNAME(DEFINITE), GetBodyPart(c)->CHARNAME(UNARTICLED));
	  Useful = true;
	}
    }
  if(!Useful)
    {
      if(IsPlayer())
	ADD_MESSAGE("You shudder.");
      else if(GetLSquareUnder()->CanBeSeen())
	ADD_MESSAGE("%s shudders.", CHARNAME(DEFINITE));
    }
  return true;
}

void character::SetSize(ushort Size)
{
  for(ushort c = 0; c < BodyParts(); ++c)
    if(GetBodyPart(c))
      GetBodyPart(c)->SetSize(GetBodyPartSize(c, Size));
}

ulong character::GetBodyPartSize(ushort Index, ushort TotalSize)
{
  if(Index == TORSOINDEX)
    return TotalSize;
  else
    {
      ABORT("Weird bodypart size request for a character!");
      return 0;
    }
}

ulong character::GetBodyPartVolume(ushort Index)
{
  if(Index == TORSOINDEX)
    return GetTotalVolume();
  else
    {
      ABORT("Weird bodypart volume request for a character!");
      return 0;
    }
}

uchar character::GetBodyPartBonePercentile(ushort Index)
{
  if(Index == TORSOINDEX)
    return GetTorsoBonePercentile();
  else
    {
      ABORT("Weird bodypart bone percentile request for a character!");
      return 0;
    }
}

void character::CreateBodyParts()
{
  for(ushort c = 0; c < BodyParts(); ++c) 
    CreateBodyPart(c);
}

void character::RestoreBodyParts()
{
  for(ushort c = 0; c < BodyParts(); ++c)
    if(!GetBodyPart(c))
      CreateBodyPart(c);
}

void character::UpdateBodyPartPictures()
{
  for(ushort c = 0; c < BodyParts(); ++c)
    UpdateBodyPartPicture(c);
}

bodypart* character::MakeBodyPart(ushort Index)
{
  if(Index == TORSOINDEX)
    return new normaltorso(0, false);
  else
    {
      ABORT("Weird bodypart to make for a character!");
      return 0;
    }
}

void character::CreateBodyPart(ushort Index)
{
  bodypart* BodyPart = MakeBodyPart(Index);
  AttachBodyPart(BodyPart,Index);
}

vector2d character::GetBodyPartBitmapPos(ushort Index, ushort Frame)
{
  if(Index == TORSOINDEX)
    return GetTorsoBitmapPos(Frame);
  else
    {
      ABORT("Weird bodypart BitmapPos request for a character!");
      return vector2d();
    }
}

ushort character::GetBodyPartColor0(ushort Index, ushort Frame)
{
  if(Index < BodyParts())
    return GetSkinColor(Frame);
  else
    {
      ABORT("Weird bodypart color 0 request for a character!");
      return 0;
    }
}

ushort character::GetBodyPartColor1(ushort Index, ushort Frame)
{
  if(Index == TORSOINDEX)
    return GetTorsoMainColor(Frame);
  else
    {
      ABORT("Weird bodypart color 1 request for a character!");
      return 0;
    }
}

ushort character::GetBodyPartColor2(ushort Index, ushort)
{
  if(Index == TORSOINDEX)
    return 0; // reserved for future use
  else
    {
      ABORT("Weird bodypart color 2 request for a character!");
      return 0;
    }
}

ushort character::GetBodyPartColor3(ushort Index, ushort Frame)
{
  if(Index == TORSOINDEX)
    return GetTorsoSpecialColor(Frame);
  else
    {
      ABORT("Weird bodypart color 3 request for a character!");
      return 0;
    }
}

/* This should perhaps have a parameter that controls which color vectors are updated... */

void character::UpdateBodyPartPicture(ushort Index)
{
  if(GetBodyPart(Index))
    {
      std::vector<vector2d>& BitmapPos = GetBodyPart(Index)->GetBitmapPosVector();
      std::vector<ushort>& Color1 = GetBodyPart(Index)->GetColor1Vector();
      std::vector<ushort>& Color2 = GetBodyPart(Index)->GetColor2Vector();
      std::vector<ushort>& Color3 = GetBodyPart(Index)->GetColor3Vector();

      BitmapPos.clear();
      Color1.clear();
      Color2.clear();
      Color3.clear();

      for(ushort c = 0; c < GetBodyPartAnimationFrames(Index); ++c)
	{
	  BitmapPos.push_back(GetBodyPartBitmapPos(Index, c));
	  Color1.push_back(GetBodyPartColor1(Index, c));
	  Color2.push_back(GetBodyPartColor2(Index, c));
	  Color3.push_back(GetBodyPartColor3(Index, c));
	}

      material* Material = GetBodyPart(Index)->GetMainMaterial();

      if(Material->IsFlesh())
	{
          std::vector<ushort>& SkinColor = ((flesh*)Material)->GetSkinColorVector();
	  SkinColor.clear();

	  for(ushort c = 0; c < GetBodyPartAnimationFrames(Index); ++c)
	    SkinColor.push_back(GetBodyPartColor0(Index, c));
	}

      GetBodyPart(Index)->SetAnimationFrames(GetBodyPartAnimationFrames(Index));
      GetBodyPart(Index)->UpdatePictures();
    }
}

void character::LoadDataBaseStats()
{
  BaseAttribute[ENDURANCE] = GetDefaultEndurance();
  BaseAttribute[PERCEPTION] = GetDefaultPerception();
  BaseAttribute[INTELLIGENCE] = GetDefaultIntelligence();
  BaseAttribute[WISDOM] = GetDefaultWisdom();
  BaseAttribute[CHARISMA] = GetDefaultCharisma();
  BaseAttribute[MANA] = GetDefaultMana();
  SetMoney(GetDefaultMoney());
}

character* character::Clone(bool MakeBodyParts, bool CreateEquipment) const
{
  return GetProtoType()->Clone(MakeBodyParts, CreateEquipment);
}

character* characterprototype::CloneAndLoad(inputfile& SaveFile) const
{
  character* Char = Clone(0, false, true);
  Char->Load(SaveFile);
  return Char;
}

/*item* character::GetLifeSaver() const
{
  for(ushort c = 0; c < EquipmentSlots(); ++c)
    {
      item* Item = GetEquipment(c);

      if(Item && Item->SavesLifeWhenWorn())
	return Item;
    }
  return 0;
}*/

void character::Initialize(uchar NewConfig, bool CreateEquipment, bool Load)
{
  BodyPartSlot = new characterslot[BodyParts()];
  OriginalBodyPartID = new ulong[BodyParts()];
  CategoryWeaponSkill = new gweaponskill*[AllowedWeaponSkillCategories() + 1];

  ushort c;

  for(c = 0; c < BodyParts(); ++c)
    GetBodyPartSlot(c)->SetMaster(this);

  for(c = 0; c < AllowedWeaponSkillCategories(); ++c)
    CategoryWeaponSkill[c] = new gweaponskill(c);

  CategoryWeaponSkill[AllowedWeaponSkillCategories()] = 0;

  if(!Load)
    {
      Config = NewConfig;
      InstallDataBase();
      LoadDataBaseStats();
      CreateBodyParts();
      InitSpecialAttributes();
      PermanentState |= GetPermanentStates();
    }

  VirtualConstructor(Load);

  if(!Load)
    {
      if(CreateEquipment)
	CreateInitialEquipment();

      RestoreHP();
    }
}

characterprototype::characterprototype(characterprototype* Base) : Base(Base)
{
  Index = protocontainer<character>::Add(this);
}

bool character::TeleportNear(character* Caller)
{
  vector2d Where = game::GetCurrentArea()->GetNearestFreeSquare(Caller, Caller->GetPos());
  Teleport(Where);
  return true;
}

void character::Teleport(vector2d Pos)
{
  Move(Pos, true);
}

void character::InstallDataBase()
{
  if(!Config)
    DataBase = GetProtoType()->GetDataBase();
  else
    {
      const character::databasemap& Configs = GetProtoType()->GetConfig();
      character::databasemap::const_iterator i = Configs.find(Config);

      if(i != Configs.end())
	DataBase = &i->second;
      else
	ABORT("Undefined character configuration #%d sought!", Config);
    }
}

void character::ReceiveHeal(long Amount)
{
  for(ushort c = 0; c < BodyParts(); ++c)
    if(GetBodyPart(c))
      {
	if(GetBodyPart(c)->GetHP() + Amount / BodyParts() > GetBodyPart(c)->GetMaxHP())
	  GetBodyPart(c)->SetHP(GetBodyPart(c)->GetMaxHP());
	else
	  GetBodyPart(c)->EditHP(Amount / BodyParts());
      }
}

void character::AddHealingLiquidConsumeEndMessage() const
{
  if(IsPlayer())
    ADD_MESSAGE("You feel better.");
  else
    if(GetSquareUnder()->CanBeSeen())
      ADD_MESSAGE("%s looks healthier.", CHARNAME(DEFINITE));
}

void character::ReceivePoison(long SizeOfEffect)
{
  SizeOfEffect += RAND() % SizeOfEffect;
  Vomit(SizeOfEffect / 250);
  ReceiveDamage(0, SizeOfEffect / 100, POISON, ALL, true);

  if(CheckDeath("died of acute poisoning"))
    return;

  if(!(RAND() % 3) && SizeOfEffect / 500)
    {
      if(IsPlayer())
	ADD_MESSAGE("You gain a little bit of toughness for surviving this stuff.");
      else
	if(GetSquareUnder()->CanBeSeen())
	  ADD_MESSAGE("Suddenly %s looks tougher.", CHARNAME(DEFINITE));

      EditAttribute(ENDURANCE, SizeOfEffect / 500);
    }
}

void character::AddSchoolFoodConsumeEndMessage() const
{
  if(IsPlayer())
    ADD_MESSAGE("Yuck! This stuff tastes like vomit and old mousepads.");
}

void character::AddSchoolFoodHitMessage() const
{
  if(IsPlayer())
    ADD_MESSAGE("Yuck! This stuff feels like vomit and old mousepads.");
}

void character::ReceiveNutrition(long SizeOfEffect)
{
  if(GetHungerState() == BLOATED)
    {
      ReceiveDamage(0, SizeOfEffect / 1000, BULIMIA, TORSO);
      CheckDeath("choked on his food");
    }

  EditNP(SizeOfEffect);
}

void character::ReceiveOmleUrine(long Amount)
{
  EditExperience(ARMSTRENGTH, Amount * 4);
  EditExperience(LEGSTRENGTH, Amount * 4);
  RestoreHP();
}

void character::AddOmleUrineConsumeEndMessage() const
{
  if(IsPlayer())
    ADD_MESSAGE("You feel a primitive Force coursing through your veins.");
  else
    if(GetSquareUnder()->CanBeSeen())
      ADD_MESSAGE("Suddenly %s looks more powerful.", CHARNAME(DEFINITE));
}

void character::ReceivePepsi(long SizeOfEffect)
{
  ReceiveDamage(0, SizeOfEffect / 100, POISON, TORSO);
  EditExperience(PERCEPTION, SizeOfEffect * 4);

  if(CheckDeath("was poisoned by pepsi"))
    return;

  if(IsPlayer())
    game::DoEvilDeed(SizeOfEffect / 10);
}

void character::AddPepsiConsumeEndMessage() const
{
  if(IsPlayer())
    ADD_MESSAGE("Urgh. You feel your guruism fading away.");
  else
    if(GetSquareUnder()->CanBeSeen())
      ADD_MESSAGE("%s looks very lame.", CHARNAME(DEFINITE));
}

void character::ReceiveDarkness(long SizeOfEffect)
{
  long Penalty = 1 + SizeOfEffect * (100 + RAND() % 26 - RAND() % 26) / 100000;

  /*if(GetStrength() - Penalty > 1)
    SetStrength(GetStrength() - Penalty);
  else
    SetStrength(1);

  if(GetEndurance() - Penalty > 1)
    SetEndurance(GetEndurance() - Penalty);
  else
    SetEndurance(1);

  if(GetAgility() - Penalty > 1)
    SetAgility(GetAgility() - Penalty);
  else
    SetAgility(1);*/

  /*ReceiveDamage(0, SizeOfEffect / 100, POISON, TORSO);

  if(CheckDeath("was poisoned by pepsi"))
    return;*/

  /*if(GetHP() - Penalty / 2 > 1)
    SetHP(GetHP() - Penalty / 2);
  else
    SetHP(1);*/

  if(IsPlayer())
    {
      game::DoEvilDeed(short(SizeOfEffect / 50));

      if(game::WizardModeActivated())
	ADD_MESSAGE("Change in relation: %d.", short(SizeOfEffect / 100));
    }
}

void character::AddFrogFleshConsumeEndMessage() const
{
  if(IsPlayer())
    ADD_MESSAGE("Arg. You feel the fate of a navastater placed upon you...");
  else
    if(GetSquareUnder()->CanBeSeen())
      ADD_MESSAGE("Suddenly %s looks like a navastater.", CHARNAME(DEFINITE));
}

/*void character::ReceiveFireDamage(character* Burner, std::string DeathMsg, long SizeOfEffect)
{
  if(IsPlayer())
    ADD_MESSAGE("You burn.");
  else
    if(GetLSquareUnder()->CanBeSeen())
      ADD_MESSAGE("%s is hurt by the fire.", CHARNAME(DEFINITE));

  if(SizeOfEffect > 1)
    SetHP(GetHP() - (SizeOfEffect + RAND() % (SizeOfEffect / 2)));
  else
    SetHP(GetHP() - SizeOfEffect);

  GetStack()->ReceiveFireDamage(Burner, DeathMsg, SizeOfEffect);
}*/

void character::ReceiveKoboldFlesh(long)
{
  /* As it is commonly known, the possibility of fainting per 500 cubic centimeters of kobold flesh is exactly 1%. */

  if(!(RAND() % 100))
    {
      if(IsPlayer())
	ADD_MESSAGE("You lose control of your legs and fall down.");

      Faint();
    }
}

void character::AddKoboldFleshConsumeEndMessage() const
{
  if(IsPlayer())
    ADD_MESSAGE("This stuff tastes really funny.");
}

void character::AddKoboldFleshHitMessage() const
{
  if(IsPlayer())
    ADD_MESSAGE("You feel very funny.");
}

void character::AddBoneConsumeEndMessage() const
{
  if(IsPlayer())
    ADD_MESSAGE("You feel like a hippie.");
  else if(GetSquareUnder()->CanBeSeen())
    ADD_MESSAGE("%s barks happily.", CHARNAME(DEFINITE)); // this suspects that nobody except dogs can eat bones
}

/* returns true if character manages to unstuck himself (from all traps...). vector2d is the direction which the character has tried to escape to */
bool character::TryToUnstuck(vector2d Direction)
{
  return StuckTo->TryToUnstuck(this, StuckToBodyPart, Direction);
}



bool character::IsStuck() const
{
  if(GetStuckToBodyPart() == NONEINDEX)
    return false;
  

  if(GetBodyPart(GetStuckToBodyPart()))
    return true;
  else
    return false;
}

bool character::CheckForAttributeIncrease(ushort& Attribute, long& Experience, bool DoubleAttribute)
{
  /* Check if attribute is disabled for creature */

  if(!Attribute)
    return false;

  if(!DoubleAttribute)
    if(Experience >= long(Attribute) << 10)
      {
	if(Attribute < 99)
	  {
	    Attribute += 1;
	    Experience = 0;
	    return true;
	  }
      }
  else
    if(Experience >= long(Attribute) << 9)
      {
	if(Attribute < 198)
	  {
	    Attribute += 1;
	    Experience = 0;
	    return true;
	  }
      }

  return false;
}

bool character::CheckForAttributeDecrease(ushort& Attribute, long& Experience, bool DoubleAttribute)
{
  /* Check if attribute is disabled for creature */

  if(!Attribute)
    return false;

  if(!DoubleAttribute)
    if(Experience <= (long(Attribute) - 100) << 8)
      {
	if(Attribute > 1)
	  {
	    Attribute -= 1;
	    Experience = 0;
	    return true;
	  }
      }
  else
    if(Experience <= (long(Attribute) - 200) << 7)
      {
	if(Attribute > 1)
	  {
	    Attribute -= 1;
	    Experience = 0;
	    return true;
	  }
      }

  return false;
}

bool character::RawEditAttribute(ushort& Attribute, short& Amount, bool DoubleAttribute)
{
  /* Check if attribute is disabled for creature */

  if(!Attribute)
    return false;

  if(Amount < 0)
    {
      if(Attribute > 1)
	{
	  Attribute = Attribute > 1 - Amount ? Attribute + Amount : 1;
	  return true;
	}
      else
	return false;
    }
  else
    {
      ushort Limit = DoubleAttribute ? 198 : 99;

      if(Attribute < Limit)
	{
	  Attribute = Attribute + Amount < Limit ? Attribute + Amount : Limit;
	  return true;
	}
      else
	return false;
    }
}

void character::DrawPanel() const
{
  DOUBLEBUFFER->Fill(19 + (game::GetScreenSize().X << 4), 0, RES.X - 19 - (game::GetScreenSize().X << 4), RES.Y, 0);
  DOUBLEBUFFER->Fill(16, 45 + game::GetScreenSize().Y * 16, game::GetScreenSize().X << 4, 9, 0);
  FONT->Printf(DOUBLEBUFFER, 16, 45 + game::GetScreenSize().Y * 16, WHITE, "%s", CHARNAME(INDEFINITE));//, GetVerbalPlayerAlignment().c_str());

  ushort PanelPosX = RES.X - 96;
  ushort PanelPosY = DrawStats() + 1;

  if(game::IsInWilderness())
    FONT->Printf(DOUBLEBUFFER, PanelPosX, (PanelPosY++) * 10, WHITE, "Worldmap");
  else
    FONT->Printf(DOUBLEBUFFER, PanelPosX, (PanelPosY++) * 10, WHITE, "%s", game::GetCurrentDungeon()->GetLevelDescription(game::GetCurrent()).c_str());

  FONT->Printf(DOUBLEBUFFER, PanelPosX, (PanelPosY++) * 10, WHITE, "Time: %d", game::GetTicks() / 10);

  ++PanelPosY;

  if(GetAction())
    {
      std::string Desc = GetAction()->GetDescription();
      Desc[0] &= ~0x20;
      FONT->Printf(DOUBLEBUFFER, PanelPosX, (PanelPosY++) * 10, WHITE, "%s", Desc.c_str());
    }

  for(ushort c = 0; c < STATES; ++c)
    if((1 << c) & TemporaryState || (1 << c) & PermanentState)
      {
	std::string Desc = StateDescription[c];
	Desc[0] &= ~0x20;
	FONT->Printf(DOUBLEBUFFER, PanelPosX, (PanelPosY++) * 10, (1 << c) & PermanentState ? BLUE : WHITE, "%s", Desc.c_str());
      }

  if(GetHungerState() == VERYHUNGRY)
    FONT->Printf(DOUBLEBUFFER, PanelPosX, (PanelPosY++) * 10, RED, "Starving");
  else
    if(GetHungerState() == HUNGRY)
      FONT->Printf(DOUBLEBUFFER, PanelPosX, (PanelPosY++) * 10, BLUE, "Hungry");
  else 
    if(GetHungerState() == SATIATED)
      FONT->Printf(DOUBLEBUFFER, PanelPosX, (PanelPosY++) * 10, WHITE, "Satiated");
  else
    if(GetHungerState() == BLOATED)
      FONT->Printf(DOUBLEBUFFER, PanelPosX, (PanelPosY++) * 10, WHITE, "Bloated");

  switch(GetBurdenState())
    {
    case OVERLOADED:
      FONT->Printf(DOUBLEBUFFER, PanelPosX, (PanelPosY++) * 10, RED, "Overload!");
      break;
    case STRESSED:
      FONT->Printf(DOUBLEBUFFER, PanelPosX, (PanelPosY++) * 10, BLUE, "Stressed");
      break;
    case BURDENED:
      FONT->Printf(DOUBLEBUFFER, PanelPosX, (PanelPosY++) * 10, BLUE, "Burdened");
    case UNBURDENED:
      break;
    }
}

float character::CalculateDodgeValue() const
{
  float DV = 5 * GetBattleAttributeModifier() * GetAttribute(AGILITY) / sqrt(GetSize());
  return DV > 1 ? DV : 1;
}

void character::VirtualConstructor(bool Load)
{
  if(!Load)
    {
      for(ushort c = 0; c < BASEATTRIBUTES; ++c)
	BaseExperience[c] = 0;
      SetStuckTo(0);
      SetStuckToBodyPart(NONEINDEX);
    }
}

bool character::DamageTypeAffectsInventory(uchar Type) const
{
  switch(Type)
    {
    case SOUND:
    case ENERGY:
    case ACID:
    case FIRE:
      return true;
    case PHYSICALDAMAGE:
    case POISON:
    case BULIMIA:
      return false;
    default:
      ABORT("Unknown reaping effect destroyed dungeon!");
      return false;
    }
}

void character::EditVolume(long What)
{
  Volume += What;

  if(GetMotherEntity())
    GetMotherEntity()->EditVolume(What);
}

void character::EditWeight(long What)
{
  Weight += What;

  if(GetMotherEntity())
    GetMotherEntity()->EditWeight(What);
}

ushort character::CheckForBlockWithItem(character* Enemy, item*, item* Blocker, float WeaponToHitValue, float BlockerToHitValue, ushort Damage, short Success, uchar Type)
{
  if(Blocker->GetStrengthValue())
    if(RAND() % ushort(100 + WeaponToHitValue / (BlockerToHitValue * Blocker->GetBlockModifier(this)) * (100 + Success) * 10000) < 100)
      {
	bool Partial = Blocker->GetStrengthValue() < Damage;

	switch(Type)
	  {
	  case UNARMEDATTACK:
	    Enemy->AddBlockMessage(this, Blocker, UnarmedHitNoun(), Partial);
	    break;
	  case WEAPONATTACK:
	    Enemy->AddBlockMessage(this, Blocker, "hit", Partial);
	    break;
	  case KICKATTACK:
	    Enemy->AddBlockMessage(this, Blocker, KickNoun(), Partial);
	    break;
	  case BITEATTACK:
	    Enemy->AddBlockMessage(this, Blocker, BiteNoun(), Partial);
	    break;
	  }

	if(Partial)
	  return Damage - Blocker->GetStrengthValue();
	else
	  return 0;
      }

  return Damage;
}

bool character::ShowWeaponSkills()
{
  felist List("Your experience in weapon categories", WHITE, 0);

  List.AddDescription("");
  List.AddDescription("Category name                 Level     Points    To next level");

  bool Something = false;

  for(ushort c = 0; c < AllowedWeaponSkillCategories(); ++c)
    if(GetCategoryWeaponSkill(c)->GetHits())
      {
	std::string Buffer;

	Buffer += GetCategoryWeaponSkill(c)->Name();
	Buffer.resize(30, ' ');

	Buffer += GetCategoryWeaponSkill(c)->GetLevel();
	Buffer.resize(40, ' ');

	Buffer += GetCategoryWeaponSkill(c)->GetHits();
	Buffer.resize(50, ' ');

	if(GetCategoryWeaponSkill(c)->GetLevel() != 10)
	  List.AddEntry(Buffer + (GetCategoryWeaponSkill(c)->GetLevelMap(GetCategoryWeaponSkill(c)->GetLevel() + 1) - GetCategoryWeaponSkill(c)->GetHits()), LIGHTGRAY);
	else
	  List.AddEntry(Buffer + '-', LIGHTGRAY);

	Something = true;
      }

  if(AddSpecialSkillInfo(List))
    Something = true;

  if(Something)
    List.Draw(vector2d(26, 42), 652, 20, MAKE_RGB(0, 0, 16), false);
  else
    ADD_MESSAGE("You are not experienced in any weapon skill yet!");

  return false;
}

long character::CalculateStateAPGain(long BaseAPGain) const
{
  if(StateIsActivated(HASTE))
    BaseAPGain <<= 1;

  if(StateIsActivated(SLOW))
    BaseAPGain >>= 1;

  return BaseAPGain;
}

void character::SignalEquipmentAdd(ushort EquipmentIndex)
{
  if(EquipmentHasNoPairProblems(EquipmentIndex))
    PermanentState |= GetEquipment(EquipmentIndex)->GetGearStates();
}

void character::SignalEquipmentRemoval(ushort)
{
  CalculateEquipmentStates();
}

void character::CalculateEquipmentStates()
{
  PermanentState = GetPermanentStates();

  for(ushort c = 0; c < EquipmentSlots(); ++c)
    if(GetEquipment(c) && EquipmentHasNoPairProblems(c))
      PermanentState |= GetEquipment(c)->GetGearStates();
}

void character::EnterTemporaryState(ushort State, ushort Counter)
{
  ushort Index;

  for(Index = 0; Index < STATES; ++Index)
    if(1 << Index == State)
      break;

  if(EnterTemporaryStateHandler[Index])
    {
      (this->*EnterTemporaryStateHandler[Index])(Counter);
      return;
    }

  if(TemporaryStateIsActivated(State))
    EditTemporaryStateCounter(State, Counter);
  else
    {
      if(!PermanentStateIsActivated(State))
	(this->*PrintBeginStateMessage[Index])();

      ActivateTemporaryState(State);
      SetTemporaryStateCounter(State, Counter);
    }
}

void character::HandleStates()
{
  for(ushort c = 0; c < STATES; ++c)
    {
      if(TemporaryStateIsActivated(1 << c))
	{
	  if(!GetTemporaryStateCounter(1 << c))
	    {
	      if(EndTemporaryStateHandler[c])
		{
		  (this->*EndTemporaryStateHandler[c])();

		  if(!IsEnabled())
		    return;
		  else
		    continue;
		}

	      if(!PermanentStateIsActivated(1 << c))
		(this->*PrintEndStateMessage[c])();

	      DeActivateTemporaryState(1 << c);
	    }

	  EditTemporaryStateCounter(1 << c, -1);
	}

      if(StateIsActivated(1 << c))
	if(StateHandler[c])
	  (this->*StateHandler[c])();

      if(!IsEnabled())
	return;
    }
}

void character::PrintBeginPolymorphControlMessage() const
{
  if(IsPlayer())
    ADD_MESSAGE("You feel very controlled.");
}

void character::PrintEndPolymorphControlMessage() const
{
  if(IsPlayer())
    ADD_MESSAGE("You are somehow uncertain of your willpower.");
}

void character::PrintBeginLifeSaveMessage() const
{
  if(IsPlayer())
    ADD_MESSAGE("You hear Hell's gates being locked just now.");
}

void character::PrintEndLifeSaveMessage() const
{
  if(IsPlayer())
    ADD_MESSAGE("You feel the Afterlife is welcoming you once again.");
}

void character::PrintBeginLycanthropyMessage() const
{
  if(IsPlayer())
    ADD_MESSAGE("You suddenly notice you've always loved full moons.");
}

void character::PrintEndLycanthropyMessage() const
{
  if(IsPlayer())
    ADD_MESSAGE("You feel the wolf inside you has had enough of your bad habits.");
}

void character::PrintBeginHasteMessage() const
{
  if(IsPlayer())
    ADD_MESSAGE("Time slows down to a crawl.");
  else if(GetSquareUnder()->CanBeSeen())
    ADD_MESSAGE("%s looks faster!", CHARNAME(DEFINITE));
}

void character::PrintEndHasteMessage() const
{
  if(IsPlayer())
    ADD_MESSAGE("Time seems to go by at the normal rate now.");
  else if(game::IsInWilderness() || GetSquareUnder()->CanBeSeen())
    ADD_MESSAGE("%s seems to move at the normal pace now.", CHARNAME(DEFINITE));
}

void character::PrintBeginSlowMessage() const
{
  if(IsPlayer())
    ADD_MESSAGE("Everything seems to move much faster now.");
  else if(GetSquareUnder()->CanBeSeen())
    ADD_MESSAGE("%s looks slower!", CHARNAME(DEFINITE));
}

void character::PrintEndSlowMessage() const
{
  if(IsPlayer())
    ADD_MESSAGE("Time seems to go by at the normal rate now.");
  else if(game::IsInWilderness() || GetSquareUnder()->CanBeSeen())
    ADD_MESSAGE("%s seems to move at the normal pace now.", CHARNAME(DEFINITE));
}

void character::EnterTemporaryHaste(ushort Counter)
{
  if(TemporaryStateIsActivated(SLOW))
    {
      if(!PermanentStateIsActivated(SLOW))
	PrintEndSlowMessage();

      DeActivateTemporaryState(SLOW);
    }   
  else
    {
      if(TemporaryStateIsActivated(HASTE))
	{
	  EditTemporaryStateCounter(HASTE, Counter);
	  return;
	}

      if(!PermanentStateIsActivated(HASTE))
	PrintBeginHasteMessage();

      ActivateTemporaryState(HASTE);
      SetTemporaryStateCounter(HASTE, Counter);
    }
}

void character::EnterTemporarySlow(ushort Counter)
{
  if(TemporaryStateIsActivated(HASTE))
    {
      if(!PermanentStateIsActivated(HASTE))
	PrintEndHasteMessage();

      DeActivateTemporaryState(HASTE);
    }
  else
    {
      if(TemporaryStateIsActivated(SLOW))
	{
	  EditTemporaryStateCounter(SLOW, Counter);
	  return;
	}

      if(!PermanentStateIsActivated(SLOW))
	PrintBeginSlowMessage();

      ActivateTemporaryState(SLOW);
      SetTemporaryStateCounter(SLOW, Counter);
    }
}

void character::EndTemporaryPolymorph()
{
  if(IsPlayer())
    ADD_MESSAGE("You return to your true form.");
  else if(game::IsInWilderness())
    return; // fast gum solution, state ends when the player enters a dungeon
  else if(GetSquareUnder()->CanBeSeen())
    ADD_MESSAGE("%s returns to %s true form.", CHARNAME(DEFINITE), PossessivePronoun().c_str());

  if(GetAction())
    GetAction()->Terminate(false);

  SendToHell();
  GetSquareUnder()->RemoveCharacter();
  character* Char = GetPolymorphBackup();
  SetPolymorphBackup(0);
  GetSquareUnder()->AddCharacter(Char);
  Char->SetHasBe(true);
  SetSquareUnder(0);

  while(GetStack()->GetItems())
    GetStack()->MoveItem(GetStack()->GetBottomSlot(), Char->GetStack());

  SetSquareUnder(Char->GetSquareUnder()); // might be used afterwards

  for(ushort c = 0; c < EquipmentSlots(); ++c)
    {
      item* Item = GetEquipment(c);

      if(Item)
	{
	  if(Char->CanUseEquipment() && Char->CanUseEquipment(c))
	    {
	      Item->RemoveFromSlot();
	      Char->SetEquipment(c, Item);
	    }
	  else
	    Item->MoveTo(Char->GetStack());
	}
    }

  Char->SetMoney(GetMoney());
  Char->ChangeTeam(GetTeam());

  if(GetTeam()->GetLeader() == this)
    GetTeam()->SetLeader(Char);

  Char->TestWalkability();

  if(IsPlayer())
    {
      game::SetPlayer(Char);
      game::SendLOSUpdateRequest();
    }
}

void character::LycanthropyHandler()
{
  if(!(RAND() % 2500))
    Polymorph(new werewolfwolf, RAND() % 2500);
}

void character::SaveLife()
{
  item* LifeSaver;

  if(!TemporaryStateIsActivated(LIFE_SAVED))
    for(ushort c = 0; c < EquipmentSlots(); ++c)
      if(GetEquipment(c) && EquipmentHasNoPairProblems(c) && GetEquipment(c)->GetGearStates() & LIFE_SAVED)
	LifeSaver = GetEquipment(c);

  if(!TemporaryStateIsActivated(LIFE_SAVED) && LifeSaver)
    {
      if(IsPlayer())
	ADD_MESSAGE("But wait! Your %s glows briefly red and disappears and you seem to be in a better shape!", LifeSaver->CHARNAME(UNARTICLED));
      else if(GetSquareUnder()->CanBeSeen())
	ADD_MESSAGE("But wait, suddenly %s %s glows briefly red and disappears and %s seems to be in a better shape!", PossessivePronoun().c_str(), LifeSaver->CHARNAME(UNARTICLED), PersonalPronoun().c_str());

      LifeSaver->RemoveFromSlot();
      LifeSaver->SendToHell();
    }
  else
    {
      if(IsPlayer())
	ADD_MESSAGE("But wait! You glow briefly red and seem to be in a better shape!");
      else if(GetSquareUnder()->CanBeSeen())
	ADD_MESSAGE("But wait, suddenly %s glows briefly red and seems to be in a better shape!", PersonalPronoun().c_str());

      DeActivateTemporaryState(LIFE_SAVED);
    }

  if(IsPlayer())
    game::AskForKeyPress("Life saved! [press any key to continue]");

  RestoreBodyParts();
  RestoreHP();

  if(GetNP() < 10000)
    SetNP(10000);

  GetSquareUnder()->SendNewDrawRequest();
}

void character::PolymorphRandomly(ushort Time)
{
  character* NewForm = 0;

  if(StateIsActivated(POLYMORPH_CONTROL))
    {
      if(IsPlayer())
	while(!NewForm)
	  {
	    std::string Temp = game::StringQuestion("What do you want to become?", vector2d(16, 6), WHITE, 0, 80, false);
	    NewForm = protosystem::CreateMonster(Temp, false);
	  }
      else
	{
	  switch(RAND() % 3)
	    {
	    case 0: NewForm = new communist(0, false); break;
	    case 1: NewForm = new mistress(0, false); break;
	    case 2: NewForm = new mammoth(0, false); break;
	    }
	}
    }
  else
    NewForm = protosystem::CreateMonster(false);

  Polymorph(NewForm, Time);
}

bool character::DetachBodyPart()
{
  ADD_MESSAGE("You haven't got any extra bodyparts.");
  return false;
}


void character::AttachBodyPart(bodypart* BodyPart, ushort Index)
{
  BodyPart->InitMaterials(CreateBodyPartFlesh(Index, GetBodyPartVolume(Index) * (100 - GetBodyPartBonePercentile(Index)) / 100), CreateBodyPartBone(Index, GetBodyPartVolume(Index) * GetBodyPartBonePercentile(Index) / 100), false);
  BodyPart->SetSize(GetBodyPartSize(Index, GetTotalSize()));
  SetBodyPart(Index, BodyPart);
  BodyPart->InitSpecialAttributes();
  UpdateBodyPartPicture(Index);
}

bool character::HasAllBodyParts() const
{
  for(ushort c = 0; c < BodyParts(); ++c)
    if(GetBodyPart(c))
      return false;
  return true;
}

/* Returns false if the character has all body parts else true */ 
bodypart* character::GenerateRandomBodyPart()
{
  std::vector<ushort> NeededBodyParts;
  for(ushort c = 0; c < BodyParts(); ++c)
    if(!GetBodyPart(c))
      {
	NeededBodyParts.push_back(c);
      }

  if(NeededBodyParts.empty())
    return 0;

  ushort NewBodyPartsIndex = NeededBodyParts[RAND() % NeededBodyParts.size()];
  bodypart* NewBodyPart = MakeBodyPart(NewBodyPartsIndex);
  return NewBodyPart;
}

/* searched for character's Stack and if it find some bodyparts their that are the character's old bodyparts returns a stackiterator to one of them (choocen in random). If no fitting bodyparts are found the function returns 0 */
stackiterator character::FindRandomOwnBodyPart()
{
  std::vector<stackiterator> LostAndFound;
  for(ushort c = 0; c < BodyParts(); ++c)
    if(!GetBodyPart(c))
      {
	ushort OriginalID = GetOriginalBodyPartID(c);
	for(stackiterator ii = GetStack()->GetBottomSlot(); ii != GetStack()->GetSlotAboveTop(); ++ii)
	  {
	    if(GetOriginalBodyPartID(c) == (**ii)->GetID())
	      LostAndFound.push_back(ii);
	  }
      }
  if(LostAndFound.empty())
    return 0;
  else
    return LostAndFound[RAND() % LostAndFound.size()];
}
