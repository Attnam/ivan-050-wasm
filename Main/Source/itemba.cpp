#include <ctime>
#include <cmath>

#include "itemba.h"
#include "error.h"
#include "charba.h"
#include "stack.h"
#include "level.h"
#include "lsquare.h"
#include "lterraba.h"
#include "message.h"
#include "wskill.h"
#include "femath.h"
#include "slot.h"

item::item(bool CreateMaterials, bool SetStats, bool AddToPool) : object(AddToPool, false), Slot(0), Cannibalised(false)
{
  ID = game::CreateNewItemID();

  if(CreateMaterials || SetStats)
    ABORT("Boo!");
}

void item::PositionedDrawToTileBuffer(uchar) const
{
  Picture->MaskedBlit(igraph::GetTileBuffer(), 0, 0, 0, 0, 16, 16);
}

ulong item::GetWeight() const
{
  ulong TotalWeight = 0;

  for(ushort c = 0; c < GetMaterials(); ++c)
    if(GetMaterial(c))
      TotalWeight += GetMaterial(c)->GetWeight();

  return TotalWeight;
}

bool item::Consumable(character* Eater) const
{
  return Eater->ConsumeItemType(GetConsumeType());
}

ushort item::GetEmitation() const
{
  ushort Emitation = 0;

  for(ushort c = 0; c < GetMaterials(); ++c)
    if(GetMaterial(c))
      if(GetMaterial(c)->GetEmitation() > Emitation)
	Emitation = GetMaterial(c)->GetEmitation();

  return Emitation;
}

short item::CalculateOfferValue(char GodAlignment) const
{
  float OfferValue = 0;
  for(ushort c = 0; c < GetMaterials(); ++c)
    {
      if(GetMaterial(c))
	{
	  if(GetMaterial(c)->Alignment() == EVIL)
	    {
	      if(GodAlignment == EVIL || GodAlignment == NEUTRAL)
		OfferValue += Material[c]->GetVolume() * Material[c]->OfferValue();
	      else
		if(GodAlignment == GOOD)
		  OfferValue -= Material[c]->GetVolume() * Material[c]->OfferValue();
	    }
	  else if(GetMaterial(c)->Alignment() == GOOD)
	    {
	      if(GodAlignment == GOOD || GodAlignment == NEUTRAL)
		OfferValue += Material[c]->GetVolume() * Material[c]->OfferValue();
	      else
		if(GodAlignment == EVIL)
		  OfferValue -= Material[c]->GetVolume() * Material[c]->OfferValue();
	    }
	  else
	    OfferValue += Material[c]->GetVolume() * Material[c]->OfferValue();
	}
    }
  return short(OfferValue * (OfferModifier() / 250));
}

/******************************************
*This fly system seems to have been made, *
*just to handle only player               *
*kicking and throwing things...		  *
******************************************/

bool item::Fly(uchar Direction, ushort Force, stack* Start, bool Hostile)
{
  vector2d StartingPos = Start->GetPos();
  vector2d Pos = Start->GetPos();
  bool Breaks = false;
  float Speed = float(Force) / GetWeight() * 1500;

  for(;;)
    {
      if(!game::IsValidPos(Pos + game::GetMoveVector(Direction)))
	break;
      lsquare* JustHit = game::GetCurrentLevel()->GetLSquare(Pos + game::GetMoveVector(Direction));
      if(!(JustHit->GetOLTerrain()->GetIsWalkable()))
	{
	  Breaks = true;
	  JustHit->HasBeenHitBy(this, Speed, Direction, JustHit->CanBeSeen());
	  break;
	}
      else
	{
	  clock_t StartTime = clock();

	  Pos += game::GetMoveVector(Direction);
	  Speed *= 0.7f;

	  if(Speed < 0.5)
	    break;

	  MoveTo(game::GetCurrentLevel()->GetLSquare(Pos)->GetStack());
	  game::DrawEverything(false);
	  Start = game::GetCurrentLevel()->GetLSquare(Pos)->GetStack();

	  if(game::GetCurrentLevel()->GetLSquare(Pos)->GetCharacter())
	    {
	      if(Hostile)
		game::GetPlayer()->Hostility(game::GetCurrentLevel()->GetLSquare(Pos)->GetCharacter());

	      if(HitCharacter(game::GetCurrentLevel()->GetLSquare(Pos)->GetCharacter(), Speed, game::GetPlayer()))
		{
		  Breaks = true;
		  break;
		}
	    }

	  while(clock() - StartTime < 0.05f * CLOCKS_PER_SEC);
	}
    }

  MoveTo(game::GetCurrentLevel()->GetLSquare(Pos)->GetStack());

  if(Breaks)
    ImpactDamage(ushort(Speed));

  if(Pos == StartingPos)
    return false;
  else
    return true;
}

bool item::HitCharacter(character* Dude, float Speed, character* Hitter)
{
  if(Dude->Catches(this, Speed))
    return true;

  if(Dude->DodgesFlyingItem(this, Speed)) 
    {
      if(Dude->GetIsPlayer())
	ADD_MESSAGE("%s misses you.", CHARNAME(DEFINITE));
      else
	if(Dude->GetLSquareUnder()->CanBeSeen())
	  ADD_MESSAGE("%s misses %s.", CHARNAME(DEFINITE), Dude->CHARNAME(DEFINITE));

      return false;
    }
  if(RAND() % 2) 
    ReceiveHitEffect(Dude, Hitter);

  Dude->HasBeenHitByItem(this, Speed);

  return true;
}

item* item::PrepareForConsuming(character*, stack*)
{
  return this;
}

float item::GetWeaponStrength() const
{
  return sqrt(float(GetFormModifier()) * Material[0]->GetStrengthValue() * GetWeight());
}

void item::DrawToTileBuffer() const
{
  PositionedDrawToTileBuffer(CENTER);
}

item* item::CreateWishedItem() const
{
  return protocontainer<item>::GetProto(Type())->Clone();
}

bool item::Apply(character* Applier)
{
  if(Applier->GetIsPlayer())
    ADD_MESSAGE("You can't apply this!");

  return false;
}

bool item::Zap(character*, vector2d, uchar)
{
  return false; 
}

bool item::Polymorph(stack* CurrentStack)
{
  CurrentStack->AddItem(protosystem::BalancedCreateItem());
  SetExists(false);
  return true;
}

void item::ChangeMainMaterial(material* NewMaterial)
{
  ChangeMaterial(0, NewMaterial);
}

uchar item::GetWeaponCategory() const
{
  return UNCATEGORIZED;
}

bool item::StruckByWandOfStriking(character*, std::string) 
{ 
  return ImpactDamage(10);
}

bool item::Consume(character* Eater, float Amount)
{
  GetMainMaterial()->EatEffect(Eater, Amount, NPModifier());

  if(!Cannibalised && Eater->GetIsPlayer() && Eater->CheckCannibalism(GetMainMaterial()->GetType()))
    {
      game::DoEvilDeed(25);
      ADD_MESSAGE("You feel that this was an evil deed.");
      Cannibalised = true;
    }

  return GetMainMaterial()->GetVolume() ? false : true;
}

bool item::IsBadFoodForAI(character* Eater) const
{
  if(Eater->CheckCannibalism(GetMainMaterial()->GetType()))
    return true;
  else
    return GetMaterial(GetConsumeMaterial())->GetIsBadFoodForAI();
}

void item::Save(outputfile& SaveFile) const
{
  object::Save(SaveFile);

  SaveFile << Cannibalised << Size << ID;
}

void item::Load(inputfile& SaveFile)
{
  object::Load(SaveFile);

  game::PopItemID(ID);

  SaveFile >> Cannibalised >> Size >> ID;
}

void item::Teleport()
{
  /* This uses Player as the character that is used for walkability calculations, which might not be very wise. Please fix.*/
  MoveTo(game::GetCurrentLevel()->GetLSquare(game::GetCurrentLevel()->RandomSquare(game::GetPlayer(), true, false))->GetStack());
}

void item::DrawToTileBuffer(vector2d Pos) const
{
  ushort FromX, FromY, ToX, ToY, Width, Height;

  if(Pos.X < 0)
    {
      FromX = -Pos.X;
      ToX = 0;
      Width = 16 + Pos.X;
    }
  else
    {
      FromX = 0;
      ToX = Pos.X;
      Width = 16 - Pos.X;
    }

  if(Pos.Y < 0)
    {
      FromY = -Pos.Y;
      ToY = 0;
      Height = 16 + Pos.Y;
    }
  else
    {
      FromY = 0;
      ToY = Pos.Y;
      Height = 16 - Pos.Y;
    }

  Picture->MaskedBlit(igraph::GetTileBuffer(), FromX, FromY, ToX, ToY, Width, Height);
}

ushort item::GetStrengthValue() const
{
  return ulong(GetStrengthModifier()) * GetMainMaterial()->GetStrengthValue() / 1000;
}

ulong item::GetVolume() const
{
  ulong Volume = 0;

  for(ushort c = 0; c < GetMaterials(); ++c)
    if(GetMaterial(c))
      Volume += GetMaterial(c)->GetVolume();

  return Volume;
}

void item::PlaceToSlot(slot* Slot)
{
  SetSlot(Slot);
  Slot->SetItem(this);
}

void item::RemoveFromSlot()
{
  GetSlot()->Empty();
  SetSlot(0);
}

void item::MoveTo(stack* Stack)
{
  GetSlot()->MoveItemTo(Stack);
}

void item::DonateSlotTo(item* Item)
{
  Item->SetSlot(GetSlot());
  GetSlot()->SetItem(Item);
  SetSlot(0);
}

std::string item::ItemCategoryName(uchar Category)
{
  switch(Category)
    {
    case HELMET: return "Helmets";
    case AMULET: return "Amulets";
    case CLOAK: return "Cloaks";
    case BODYARMOR: return "Body armors";
    case WEAPON: return "Weapons";
    case SHIELD: return "Shields";
    case RING: return "Rings";
    case GAUNTLET: return "Gauntlets";
    case BELT: return "Belts";
    case BOOT: return "Boots";
    case FOOD: return "Food";
    case POTION: return "Potions";
    case SCROLL: return "Scrolls";
    case BOOK: return "Books";
    case WAND: return "Wands";
    case TOOL: return "Tools";
    case VALUABLE: return "Valuables";
    case MISC: return "Miscellaneous items";
    default: return "Illegal items";
    }
}
