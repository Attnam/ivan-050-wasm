#define __FILE_OF_STATIC_ROOM_PROTOTYPE_DEFINITIONS__

#include "proto.h"
#include "roomba.h"

std::vector<room::prototype*> protocontainer<room>::ProtoData;
valuemap protocontainer<room>::CodeNameMap;

#include "roomde.h"

#undef __FILE_OF_STATIC_ROOM_PROTOTYPE_DEFINITIONS__

#include "charba.h"
#include "message.h"
#include "itemba.h"
#include "godba.h"
#include "lsquare.h"
#include "stack.h"
#include "team.h"
#include "game.h"
#include "save.h"
#include "lterraba.h"

void shop::HandleInstantiatedCharacter(character* Character)
{
  room::HandleInstantiatedCharacter(Character);
  Master = Character;
}

void shop::Enter(character* Customer)
{
  if(Customer->IsPlayer())
    if(Master)
      {
	if(Master->GetTeam()->GetRelation(Customer->GetTeam()) != HOSTILE && Customer->CanBeSeenBy(Master))
	  if(Master->CanBeSeenByPlayer())
	    ADD_MESSAGE("%s welcomes you warmly to the shop.", Master->CHARNAME(DEFINITE));
	  else
	    ADD_MESSAGE("Something welcomes you warmly to the shop.");
      }
    else
      ADD_MESSAGE("The shop appears to be deserted.");
}

/* item* ForSale can also be in chest or other container, so don't assume anything else in this function */

bool shop::PickupItem(character* Customer, item* ForSale)
{
  if(!Master || Customer == Master || Master->GetTeam()->GetRelation(Customer->GetTeam()) == HOSTILE)
    return true;

  /* This should depend on charisma */

  ulong Price = ForSale->GetPrice();

  if(!Customer->IsPlayer())
    if(Customer->CanBeSeenByPlayer() && Customer->GetMoney() >= Price)
      {
	ADD_MESSAGE("%s buys %s.", Customer->CHARNAME(DEFINITE), ForSale->CHARNAME(DEFINITE));
	Customer->EditMoney(-Price);
	Master->EditMoney(Price);
	return true;
      }
    else
      return false;

  if(Customer->CanBeSeenBy(Master))
    {
      if(ForSale->IsHeadOfElpuri() || ForSale->IsGoldenEagleShirt() || ForSale->IsPetrussNut() || ForSale->IsTheAvatar())
	{
	  ADD_MESSAGE("\"I think it is yours. Take it.\"");
	  return true;
	}

      if(!Price)
	{
	  ADD_MESSAGE("\"Thank you for cleaning that junk out of my floor.\"");
	  return true;
	}

      if(Customer->GetMoney() >= Price)
	{
	  ADD_MESSAGE("\"Ah! That %s costs %d gold pieces. No haggling, please.\"", ForSale->CHARNAME(UNARTICLED), Price);

	  if(game::BoolQuestion("Do you want to buy this item? [y/N]"))
	    {
	      Customer->EditMoney(-Price);
	      Master->EditMoney(+Price);
	      return true;
	    }
	  else
	    return false;
	}
      else
	{
	  ADD_MESSAGE("\"Don't touch that %s, beggar! It is worth %d gold pieces!\"", ForSale->CHARNAME(UNARTICLED));
	  return false;
	}
    }
  else
    if(game::BoolQuestion("Are you sure you want to steal this item? [y/N]"))
      {
	Customer->Hostility(Master);
	return true;
      }
    else
      return false;
}

bool shop::DropItem(character* Customer, item* ForSale)
{
  if(!Master || Customer == Master || Master->GetTeam()->GetRelation(Customer->GetTeam()) == HOSTILE)
    return true;

  /* This should depend on charisma */

  ulong Price = (ForSale->GetPrice() >> 1);

  if(!Customer->IsPlayer())
    if(Price && Customer->CanBeSeenByPlayer() && Master->GetMoney() >= Price)
      {
	ADD_MESSAGE("%s sells %s.", Customer->CHARNAME(DEFINITE), ForSale->CHARNAME(DEFINITE));
	Customer->EditMoney(Price);
	Master->EditMoney(-Price);
	return true;
      }
    else
      return false;

  if(Customer->CanBeSeenBy(Master))
    {
      if(ForSale->IsHeadOfElpuri() || ForSale->IsGoldenEagleShirt() || ForSale->IsPetrussNut() || ForSale->IsTheAvatar())
	{
	  ADD_MESSAGE("\"Oh no! You need it far more than I!\"");
	  return false;
	}

      if(!Price)
	{
	  ADD_MESSAGE("\"Hah! I wouldn't take that even if you paid me for it!\"");
	  return false;
	}

      if(Master->GetMoney() >= Price)
	{
	  ADD_MESSAGE("\"What a fine %s. I'll pay %d gold pieces for it.\"", ForSale->CHARNAME(UNARTICLED), Price);

	  if(game::BoolQuestion("Do you want to sell this item? [y/N]"))
	    {
	      Customer->SetMoney(Customer->GetMoney() + Price);
	      Master->SetMoney(Master->GetMoney() - Price);
	      return true;
	    }
	  else
	    return false;
	}
      else
	{
	  ADD_MESSAGE("\"I would pay you %d gold pieces for it, but I don't have so much. Sorry.\"", Price);
	  return false;
	}
    }
  else
    return true;
}

void temple::HandleInstantiatedCharacter(character* Character)
{
  room::HandleInstantiatedCharacter(Character);
  Master = Character;
}

void temple::Enter(character* Pilgrim)
{
  if(Pilgrim->IsPlayer())
    if(Master)
      {
	if(Master->GetTeam()->GetRelation(Pilgrim->GetTeam()) != HOSTILE && Pilgrim->CanBeSeenBy(Master))
	  if(Master->CanBeSeenByPlayer())
	    ADD_MESSAGE("%s opens %s mouth: \"Welcome to the shrine of %s!\"", Master->CHARNAME(DEFINITE), Master->PossessivePronoun().c_str(), game::GetGod(DivineMaster)->Name().c_str());
	  else
	    ADD_MESSAGE("You hear a voice say: \"Welcome to the shrine of %s!\"", game::GetGod(DivineMaster)->Name().c_str());
      }
    else
      ADD_MESSAGE("The temple appears to be deserted.");
}

void shop::KickSquare(character* Infidel, lsquare* Square)
{
  if(Square->GetStack()->GetItems() && Master && Infidel != Master && Master->GetTeam()->GetRelation(Infidel->GetTeam()) != HOSTILE && Square->CanBeSeenBy(Master))
    {
      ADD_MESSAGE("\"You infidel!\"");
      Infidel->Hostility(Master);
    }
}

bool shop::ConsumeItem(character* Customer, item*)
{
  if(!Master || Customer == Master || Master->GetTeam()->GetRelation(Customer->GetTeam()) == HOSTILE)
    return true;

  if(!Customer->IsPlayer())
    return false;

  if(Customer->CanBeSeenBy(Master))
    {
      ADD_MESSAGE("\"Buy that first, please.\"");
      return false;
    }
  else
    if(game::BoolQuestion("It's illegal to eat property of others. Are you sure you sure? [y/N]"))
      {
	Customer->Hostility(Master);
	return true;
      }
    else
      return false;
}

void cathedral::Enter(character* Visitor)
{
  if(Visitor->IsPlayer() && !Entered)
    {
      ADD_MESSAGE("The majestetic Cathedral of Valpurus looms before you. You watch it with utter respect.");
      Entered = true;
    }
}

/* Item can also be in a chest, so don't assume anything else... */

bool cathedral::PickupItem(character* Visitor, item* Item)
{
  if(game::GetTeam(2)->GetRelation(Visitor->GetTeam()) == HOSTILE)
    return true;

  if(Visitor->IsPlayer())
    {
      if(Item->IsHeadOfElpuri() || Item->IsGoldenEagleShirt() || Item->IsPetrussNut() || !Item->GetPrice())
	return true;

      ADD_MESSAGE("Picking up property of the Cathedral is prohibited.");

      if(game::BoolQuestion("Do you still want to do this? [y/N]"))
	{
	  Visitor->GetTeam()->Hostility(game::GetTeam(2));
	  return true;
	}
    }

  return false;
}

bool cathedral::DropItem(character* Visitor, item* Item)
{
  if(game::GetTeam(2)->GetRelation(Visitor->GetTeam()) == HOSTILE)
    return true;

  if(Visitor->IsPlayer())
    {
      if(Item->IsHeadOfElpuri() || Item->IsGoldenEagleShirt() || Item->IsPetrussNut() || Item->IsTheAvatar())
	{
	  ADD_MESSAGE("Donating this to the Cathedral wouldn't be wise. You may still need it.");
	  return false;
	}

      if(game::BoolQuestion("Do you wish to donate this item to the Cathedral? [y/N]"))
	return true;
    }

  return false;
}

void cathedral::KickSquare(character* Kicker, lsquare* Square)
{
  if(game::GetTeam(2)->GetRelation(Kicker->GetTeam()) == HOSTILE)
    return;

  if(Kicker->IsPlayer() && Square->GetStack()->GetItems())
    {
      ADD_MESSAGE("You have harmed the property of the Cathedral!");
      Kicker->GetTeam()->Hostility(game::GetTeam(2));
    }
}

bool cathedral::ConsumeItem(character* HungryMan, item*)
{
  if(game::GetTeam(2)->GetRelation(HungryMan->GetTeam()) == HOSTILE)
    return true;

  if(HungryMan->IsPlayer())
    {
      ADD_MESSAGE("Eating the property of the Cathedral is forbidden.");

      if(game::BoolQuestion("Do you still want to do this? [y/N]"))
	{
	  HungryMan->GetTeam()->Hostility(game::GetTeam(2));
	  return true;
	}
    }

  return false;
}

void cathedral::Save(outputfile& SaveFile) const
{
  room::Save(SaveFile);
  SaveFile << Entered;
}

void cathedral::Load(inputfile& SaveFile)
{
  room::Load(SaveFile);
  SaveFile >> Entered;
}

bool cathedral::Drink(character* Thirsty) const
{
  if(game::GetTeam(2)->GetRelation(Thirsty->GetTeam()) == HOSTILE)
    return game::BoolQuestion("Do you want to drink? [y/N]");

  if(Thirsty->IsPlayer())
    {
      ADD_MESSAGE("Drinking property of the Cathedral is prohibited.");

      if(game::BoolQuestion("Do you still want to do this? [y/N]"))
	{
	  Thirsty->GetTeam()->Hostility(game::GetTeam(2));
	  return true;
	}
    }

  return false;
}

void shop::TeleportSquare(character* Infidel, lsquare* Square)
{
  if(Square->GetStack()->GetItems() && Master && Infidel != Master && Master->GetTeam()->GetRelation(Infidel->GetTeam()) != HOSTILE && Square->CanBeSeenBy(Master))
    {
      ADD_MESSAGE("\"You infidel!\"");
      Infidel->Hostility(Master);
    }
}

void cathedral::TeleportSquare(character* Teleporter, lsquare* Square)
{
  if(game::GetTeam(2)->GetRelation(Teleporter->GetTeam()) == HOSTILE)
    return;

  if(Teleporter->IsPlayer() && Square->GetStack()->GetItems())
    {
      ADD_MESSAGE("You have done unnatural things to the property of the Cathedral!");
      Teleporter->GetTeam()->Hostility(game::GetTeam(2));
    }
}

bool cathedral::Dip(character* Thirsty) const
{
  if(game::GetTeam(2)->GetRelation(Thirsty->GetTeam()) == HOSTILE)
    return true;

  if(Thirsty->IsPlayer())
    {
      /* What if it's not water? */

      ADD_MESSAGE("Stealing the precious water of the Cathedral is prohibited.");

      if(game::BoolQuestion("Are you sure you want to dip? [y/N]"))
	{
	  Thirsty->GetTeam()->Hostility(game::GetTeam(2));
	  return true;
	}
    }

  return false;
}

void cathedral::VirtualConstructor(bool)
{
  SetEntered(false);
}


void library::HandleInstantiatedCharacter(character* Character)
{
  room::HandleInstantiatedCharacter(Character);
  Master = Character;
}

void library::Enter(character* Customer)
{
  if(Customer->IsPlayer())
    if(Master)
      {
	if(Master->GetTeam()->GetRelation(Customer->GetTeam()) != HOSTILE && Customer->CanBeSeenBy(Master))
	  if(Master->CanBeSeenByPlayer())
	    ADD_MESSAGE("%s looks at you suspiciously. \"Be quiet in the library!\" %s whispers.", Master->CHARNAME(DEFINITE), Master->PersonalPronoun().c_str());
	  else
	    ADD_MESSAGE("You feel somebody staring at you.");
      }
    else
      ADD_MESSAGE("The library appears to be deserted.");
}

bool library::PickupItem(character* Customer, item* ForSale)
{
  if(!Master || Customer == Master || Master->GetTeam()->GetRelation(Customer->GetTeam()) == HOSTILE)
    return true;

  /* This should depend on charisma */

  ulong Price = ForSale->GetPrice();

  if(!Customer->IsPlayer())
    {
      if(Customer->CanBeSeenByPlayer() && Customer->GetMoney() >= Price)
	{
	  ADD_MESSAGE("%s buys %s.", Customer->CHARNAME(DEFINITE), ForSale->CHARNAME(DEFINITE));
	  Customer->EditMoney(-Price);
	  Master->EditMoney(Price);
	  return true;
	}
      else
	return false;
    }

  if(Customer->CanBeSeenBy(Master))
    {
      if(ForSale->IsHeadOfElpuri() || ForSale->IsGoldenEagleShirt() || ForSale->IsPetrussNut() || ForSale->IsTheAvatar())
	{
	  ADD_MESSAGE("\"I think it is yours. Take it.\"");
	  return true;
	}

      if(!Price || !ForSale->CanBeSoldInLibrary(Master))
	{
	  ADD_MESSAGE("\"Thank you for cleaning that junk out of my floor.\"");
	  return true;
	}

      if(Customer->GetMoney() >= Price)
	{
	  ADD_MESSAGE("\"Ah! That %s costs %d gold pieces. No haggling, please.\"", ForSale->CHARNAME(UNARTICLED), Price);

	  if(game::BoolQuestion("Do you want to buy this item? [y/N]"))
	    {
	      Customer->EditMoney(-Price);
	      Master->EditMoney(Price);
	      return true;
	    }
	  else
	    return false;
	}
      else
	{
	  ADD_MESSAGE("\"Don't touch that %s, beggar! It is worth %d gold pieces!\"", ForSale->CHARNAME(UNARTICLED));
	  return false;
	}
    }
  else
    if(game::BoolQuestion("Are you sure you want to steal this item? [y/N]"))
      {
	Customer->Hostility(Master);
	return true;
      }
    else
      return false;
}

bool library::DropItem(character* Customer, item* ForSale)
{
  if(!Master || Customer == Master || Master->GetTeam()->GetRelation(Customer->GetTeam()) == HOSTILE)
    return true;

  /* This should depend on charisma */

  ulong Price = (ForSale->GetPrice() >> 1);

  if(!Customer->IsPlayer())
    if(Price && Customer->CanBeSeenByPlayer() && Master->GetMoney() >= Price)
      {
	ADD_MESSAGE("%s sells %s.", Customer->CHARNAME(DEFINITE), ForSale->CHARNAME(DEFINITE));
	Customer->SetMoney(Customer->GetMoney() + Price);
	Master->SetMoney(Master->GetMoney() - Price);
	return true;
      }
    else
      return false;

  if(Customer->CanBeSeenBy(Master))
    {
      if(ForSale->IsHeadOfElpuri() || ForSale->IsGoldenEagleShirt() || ForSale->IsPetrussNut() || ForSale->IsTheAvatar())
	{
	  ADD_MESSAGE("\"Oh no! You need it far more than I!\"");
	  return false;
	}

      if(!Price || !ForSale->CanBeSoldInLibrary(Master))
	{
	  ADD_MESSAGE("\"Sorry I don't think that fits into my collection.\"");
	  return false;
	}

      if(Master->GetMoney() >= Price)
	{
	  ADD_MESSAGE("\"What an interesting %s. I'll pay %d gold pieces for it.\"", ForSale->CHARNAME(UNARTICLED), Price);

	  if(game::BoolQuestion("Do you want to sell this item? [y/N]"))
	    {
	      Customer->EditMoney(Price);
	      Master->EditMoney(-Price);
	      return true;
	    }
	  else
	    return false;
	}
      else
	{
	  ADD_MESSAGE("\"I would pay you %d gold pieces for it, but I don't have so much. Sorry.\"", Price);
	  return false;
	}
    }
  else
    return true;
}

void library::KickSquare(character* Infidel, lsquare* Square)
{
  if(Square->GetStack()->GetItems() && Master && Infidel != Master && Master->GetTeam()->GetRelation(Infidel->GetTeam()) != HOSTILE && Square->CanBeSeenBy(Master))
    {
      ADD_MESSAGE("\"You book vandal!\"");
      Infidel->Hostility(Master);
    }
}

bool library::ConsumeItem(character*, item*)
{
  return true;
}

void library::TeleportSquare(character* Infidel, lsquare* Square)
{
  if(Square->GetStack()->GetItems() && Master && Infidel != Master && Master->GetTeam()->GetRelation(Infidel->GetTeam()) != HOSTILE && Square->CanBeSeenBy(Master))
    {
      ADD_MESSAGE("\"You book hater!\"");
      Infidel->Hostility(Master);
    }
}

/* returns true if player agrees to continue */

bool library::DestroyTerrain(character* Infidel, olterrain* Terrain) 
{ 
  if(!Master || Infidel == Master || Master->GetTeam()->GetRelation(Infidel->GetTeam()) == HOSTILE || !Terrain->CanBeSeenBy(Master))
    return true;

  ADD_MESSAGE("The librarian might not like this.");

  if(game::BoolQuestion("Are you sure you want to do this? [y/N]"))
    {
      ADD_MESSAGE("\"You book worm!\"");
      Infidel->Hostility(Master);
      return true;
    }
  else
    return false; 
}

/* returns true if player agrees to continue */

bool cathedral::DestroyTerrain(character* Infidel, olterrain*) 
{
  if(game::GetTeam(2)->GetRelation(Infidel->GetTeam()) == HOSTILE)
    return true;

  ADD_MESSAGE("This is prohibited in the cathedral.");

  if(game::BoolQuestion("Are you sure? [y/N]"))
    {
      Infidel->GetTeam()->Hostility(game::GetTeam(2));
      return true;
    }
  else
    return false; 
}

/* returns true if player agrees to continue */

bool shop::DestroyTerrain(character* Infidel, olterrain* Terrain) 
{
  if(!Master || Infidel == Master || Master->GetTeam()->GetRelation(Infidel->GetTeam()) == HOSTILE || !Terrain->CanBeSeenBy(Master))
    return true;

  ADD_MESSAGE("The shopkeeper might not like this.");

  if(game::BoolQuestion("Are you sure you want to do this? [y/N]"))
    {
      ADD_MESSAGE("\"You communist!\"");
      Infidel->Hostility(Master);
      return true;
    }
  else
    return false; 
}

