#include "itemba.h"
#include "stack.h"
#include "stdover.h"
#include "felist.h"
#include "lsquare.h"
#include "message.h"
#include "graphics.h"
#include "charba.h"
#include "area.h"
#include "femath.h"
#include "slot.h"
#include "game.h"
#include "save.h"
#include "config.h"

stack::stack(square* MotherSquare, entity* MotherEntity, uchar SquarePosition) : MotherSquare(MotherSquare), SquarePosition(SquarePosition), MotherEntity(MotherEntity), Volume(0), Weight(0), Emitation(0)
{
  Item = new stacklist;
}

stack::~stack()
{
  Clean(true);
  delete Item;
}

void stack::Draw(bitmap* Bitmap, vector2d Pos, ushort Luminance, bool AllowAlpha, bool AllowAnimate, bool AllowOutline) const
{
  ushort VisibleItems = 0;

  for(stackiterator i = Item->begin(); i != Item->end(); ++i)
    if((**i)->IsVisible() || game::GetSeeWholeMapCheat())
      {
	(**i)->Draw(Bitmap, Pos, Luminance, AllowAlpha, AllowAnimate);
	++VisibleItems;
      }

  if(VisibleItems && AllowOutline && configuration::GetOutlineItems())
    {
      igraph::GetTileBuffer()->Fill(TRANSPARENTCOL);

      for(stackiterator i = Item->begin(); i != Item->end(); ++i)
	if((**i)->IsVisible() || game::GetSeeWholeMapCheat())
	  (**i)->Draw(igraph::GetTileBuffer(), vector2d(0, 0), 256, false, AllowAnimate);

      igraph::GetTileBuffer()->CreateOutlineBitmap(igraph::GetOutlineBuffer(), configuration::GetItemOutlineColor());
      igraph::GetOutlineBuffer()->MaskedBlit(Bitmap, 0, 0, Pos, 16, 16, configuration::GetContrastLuminance());
    }

  if(SquarePosition == CENTER && VisibleItems > 1)
    igraph::GetSymbolGraphic()->MaskedBlit(Bitmap, 0, 16, Pos, 16, 16, configuration::GetContrastLuminance());
}

void stack::AddItem(item* ToBeAdded)
{
  stackslot* StackSlot = new stackslot;
  StackSlot->SetMotherStack(this);
  StackSlot->SetStackIterator(Item->insert(Item->end(), StackSlot));
  ToBeAdded->PlaceToSlot(StackSlot);

  if(GetSquareUnder() && SquarePosition != HIDDEN)
    {
      GetSquareUnder()->SetDescriptionChanged(true);

      if(GetSquareUnder()->CanBeSeenByPlayer())
	GetLSquareUnder()->UpdateMemorizedDescription();
    }

  if(GetSquareTrulyUnder())
    {
      if(SquarePosition != HIDDEN)
	{
	  GetLSquareTrulyUnder()->SendNewDrawRequest();
	  GetLSquareTrulyUnder()->SendMemorizedUpdateRequest();

	  if(GetLSquareTrulyUnder()->CanBeSeenByPlayer())
	    GetLSquareTrulyUnder()->UpdateMemorized();

	  if(ToBeAdded->IsAnimated())
	    GetSquareTrulyUnder()->IncAnimatedEntities();
	}
    }
}

void stack::FastAddItem(item* ToBeAdded)
{
  stackslot* StackSlot = new stackslot;
  StackSlot->SetMotherStack(this);
  StackSlot->SetStackIterator(Item->insert(Item->end(), StackSlot));
  ToBeAdded->PlaceToSlot(StackSlot);

  if(SquarePosition != HIDDEN && ToBeAdded->IsAnimated() && GetSquareTrulyUnder())
    GetSquareTrulyUnder()->IncAnimatedEntities();
}

void stack::RemoveItem(stackiterator Iterator)
{
  bool WasAnimated = (**Iterator)->IsAnimated();
  ushort Emit = (**Iterator)->GetEmitation();
  delete *Iterator;
  Item->erase(Iterator);
  SignalVolumeAndWeightChange();
  SignalEmitationDecrease(Emit);

  if(GetSquareUnder() && SquarePosition != HIDDEN)
    {
      GetSquareUnder()->SetDescriptionChanged(true);

      if(GetSquareUnder()->CanBeSeenByPlayer())
	GetLSquareUnder()->UpdateMemorizedDescription();
    }

  if(GetSquareTrulyUnder())
    {
      if(SquarePosition != HIDDEN)
	{
	  GetLSquareTrulyUnder()->SendNewDrawRequest();
	  GetLSquareTrulyUnder()->SendMemorizedUpdateRequest();

	  if(GetLSquareTrulyUnder()->CanBeSeenByPlayer())
	    GetLSquareTrulyUnder()->UpdateMemorized();

	  if(WasAnimated)
	    GetSquareTrulyUnder()->DecAnimatedEntities();
	}
    }
}

void stack::FastRemoveItem(stackiterator Iterator)
{
  if(SquarePosition != HIDDEN && (**Iterator)->IsAnimated() && GetSquareTrulyUnder())
    GetSquareTrulyUnder()->DecAnimatedEntities();

  ushort Emit = (**Iterator)->GetEmitation();
  delete *Iterator;
  Item->erase(Iterator);
  SignalVolumeAndWeightChange();
  SignalEmitationDecrease(Emit);
}

void stack::Clean(bool LastClean)
{
  for(stackiterator i = Item->begin(); i != Item->end(); ++i)
    {
      if(SquarePosition != HIDDEN && (**i)->IsAnimated() && GetSquareTrulyUnder())
	GetSquareTrulyUnder()->DecAnimatedEntities();

      (**i)->SendToHell();
      delete *i;
    }

  Item->clear();

  if(!LastClean)
    {
      Volume = Weight = 0;
      SignalVolumeAndWeightChange();
    }
}

item* stack::MoveItem(stackiterator Iterator, stack* MoveTo)
{
  item* Item = ***Iterator;

  if(MoveTo->GetLSquareTrulyUnder() == GetLSquareTrulyUnder())
    {
      MoveTo->FastAddItem(***Iterator);
      FastRemoveItem(Iterator);

      if(SquarePosition != HIDDEN)
	{
	  if(GetLSquareTrulyUnder())
	    {
	      GetLSquareTrulyUnder()->SendNewDrawRequest();
	      GetLSquareTrulyUnder()->SendMemorizedUpdateRequest();
	    }

	  if(GetSquareUnder())
	    GetSquareUnder()->SetDescriptionChanged(true);
	}

      if(SquarePosition != HIDDEN || MoveTo->SquarePosition != HIDDEN)
	{
	  if(GetSquareUnder() && GetSquareUnder()->CanBeSeenByPlayer())
	    GetLSquareUnder()->UpdateMemorizedDescription();

	  if(GetSquareTrulyUnder() && GetSquareTrulyUnder()->CanBeSeenByPlayer())
	    GetLSquareTrulyUnder()->UpdateMemorized();
	}
    }
  else
    {
      MoveTo->AddItem(***Iterator);
      RemoveItem(Iterator);
    }

  return Item;
}

void stack::Save(outputfile& SaveFile) const
{
  SaveFile << *Item << SquarePosition;
}

void stack::Load(inputfile& SaveFile)
{
  SaveFile >> *Item >> SquarePosition;
  Volume = Weight = 0;

  for(stackiterator i = Item->begin(); i != Item->end(); ++i)
    {
      (*i)->SetStackIterator(i);
      (*i)->SetMotherStack(this);
      Volume += (**i)->GetVolume();
      Weight += (**i)->GetWeight();
    }
}

vector2d stack::GetPos() const
{
  return GetSquareUnder()->GetPos();
}

bool stack::SortedItems(character* Viewer, bool (*SorterFunction)(item*, character*)) const
{
  if(SorterFunction == 0)
    {
      for(stackiterator i = Item->begin(); i != Item->end(); ++i)
	if((**i)->IsVisible())
	  return true;
    }
  else
    {
      for(stackiterator i = Item->begin(); i != Item->end(); ++i)
	if(SorterFunction(***i, Viewer) && (**i)->IsVisible())
	  return true;
    }

  return false;
}

void stack::DeletePointers()
{
  while(GetItems())
    FastRemoveItem(GetBottomSlot());
}

void stack::BeKicked(character* Kicker, float KickDamage)
{
  if(KickDamage >= 25000)
    {
      ReceiveDamage(Kicker, KickDamage, PHYSICALDAMAGE);

      /* Bug: you can kick mines with this. */

      for(ushort c = 0; c < 1 + (RAND() & 1); ++c)
	if(GetItems())
	  (*Item->back())->Fly(Kicker, game::GetDirectionForVector(GetPos() - Kicker->GetPos()), KickDamage);
    }
  else
    if(GetVisibleItems() && Kicker->IsPlayer())
      ADD_MESSAGE("Your weak kick has no effect.");
}

long stack::Score() const
{
  long Score = 0;

  for(stackiterator i = Item->begin(); i != Item->end(); ++i)
    Score += (**i)->GetScore();

  return Score;
}

void stack::Polymorph()
{
  itemvector ItemVector;
  FillItemVector(ItemVector);
  ushort p = 0;

  for(ushort c = 0; c < ItemVector.size(); ++c)
    if(ItemVector[c]->Exists() && ItemVector[c]->Polymorph(this) && ++p == 5)
      break;
}

void stack::CheckForStepOnEffect(character* Stepper)
{
  itemvector ItemVector;
  FillItemVector(ItemVector);

  for(ushort c = 0; c < ItemVector.size(); ++c)
    if(ItemVector[c]->Exists())
      ItemVector[c]->StepOnEffect(Stepper);
}

square* stack::GetSquareTrulyUnder() const
{
  switch(SquarePosition)
    {
    case DOWN:
      if(GetAreaUnder()->IsValidPos(GetPos() + vector2d(0, -1)))
	return GetNearSquare(GetPos() + vector2d(0, -1));
      else
	return 0;
    case LEFT:
      if(GetAreaUnder()->IsValidPos(GetPos() + vector2d(1, 0)))
	return GetNearSquare(GetPos() + vector2d(1, 0));
      else
	return 0;
    case UP:
      if(GetAreaUnder()->IsValidPos(GetPos() + vector2d(0, 1)))
	return GetNearSquare(GetPos() + vector2d(0, 1));
      else
	return 0;
    case RIGHT:
      if(GetAreaUnder()->IsValidPos(GetPos() + vector2d(-1, 0)))
	return GetNearSquare(GetPos() + vector2d(-1, 0));
      else
	return 0;
    default:
      return GetSquareUnder();
    }
}

void stack::ReceiveDamage(character* Damager, ushort Damage, uchar Type)
{
  itemvector ItemVector;
  FillItemVector(ItemVector);

  for(ushort c = 0; c < ItemVector.size(); ++c)
    if(ItemVector[c]->Exists())
      ItemVector[c]->ReceiveDamage(Damager, Damage, Type);
}

void stack::TeleportRandomly()
{
  itemvector ItemVector;
  FillItemVector(ItemVector);

  for(ushort c = 0; c < ItemVector.size(); ++c)
    if(ItemVector[c]->Exists())
      ItemVector[c]->TeleportRandomly();
}

void stack::FillItemVector(itemvector& ItemVector) const
{
  for(stackiterator i = Item->begin(); i != Item->end(); ++i)
    ItemVector.push_back(***i);
}

item* stack::GetBottomItem() const
{
  return ***GetBottomSlot();
}

item* stack::GetItem(ushort Index) const
{
  ushort c = 0;

  for(stackiterator i = Item->begin(); i != Item->end(); ++i, ++c)
    if(c == Index)
      return ***i;

  return 0;
}

ushort stack::SearchItem(item* ToBeSearched) const
{
  ushort c = 0;

  for(stackiterator i = Item->begin(); i != Item->end(); ++i, ++c)
    if(***i == ToBeSearched)
      return c;

  return 0xFFFF;
}

stackiterator stack::GetBottomSlot() const
{
  return Item->begin();
}

stackiterator stack::GetSlotAboveTop() const
{
  return Item->end();
}

item* stack::DrawContents(character* Viewer, const std::string& Topic, bool (*SorterFunction)(item*, character*)) const
{
  return DrawContents(0, Viewer, Topic, "", "", true, SorterFunction);
}

item* stack::DrawContents(stack* MergeStack, character* Viewer, const std::string& Topic, const std::string& ThisDesc, const std::string& ThatDesc, bool (*SorterFunction)(item*, character*)) const
{
  return DrawContents(MergeStack, Viewer, Topic, ThisDesc, ThatDesc, true, SorterFunction);
}

item* stack::DrawContents(character* Viewer, const std::string& Topic, bool SelectItem, bool (*SorterFunction)(item*, character*)) const
{
  return DrawContents(0, Viewer, Topic, "", "", SelectItem, SorterFunction);
}

item* stack::DrawContents(stack* MergeStack, character* Viewer, const std::string& Topic, const std::string& ThisDesc, const std::string& ThatDesc, bool SelectItem, bool (*SorterFunction)(item*, character*)) const
{
  felist ItemNames(Topic, WHITE, 0);

  ItemNames.AddDescription("");
  ItemNames.AddDescription(std::string("Overall weight: ") + (GetWeight() + (MergeStack ? MergeStack->GetWeight() : 0)) + " grams");
  ItemNames.AddDescription("");

  std::string Buffer = "Icon  Name                                         Weight SV   Str";
  Viewer->AddSpecialItemInfoDescription(Buffer);
  ItemNames.AddDescription(Buffer);

  if(MergeStack)
    MergeStack->AddContentsToList(ItemNames, Viewer, ThatDesc, SelectItem, SorterFunction);

  AddContentsToList(ItemNames, Viewer, ThisDesc, SelectItem, SorterFunction);
  ushort Chosen = ItemNames.Draw(vector2d(26, 42), 652, 12, MAKE_RGB(0, 0, 16), SelectItem, false);

  if(Chosen & 0x8000)
    return 0;

  ushort Pos = 0;
  item* Item;

  if(MergeStack)
    {
      Item = MergeStack->SearchChosen(Pos, Chosen, Viewer, SorterFunction);

      if(Item)
	return Item;
    }

  Item = SearchChosen(Pos, Chosen, Viewer, SorterFunction);
  return Item;
}

void stack::AddContentsToList(felist& ItemNames, character* Viewer, const std::string& Desc, bool SelectItem, bool (*SorterFunction)(item*, character*)) const
{
  bool UseSorterFunction = SorterFunction != 0;
  bool DescDrawn = false;

  for(ushort c = 0; c < ITEM_CATEGORIES; ++c)
    {
      bool CatDescDrawn = false;

      for(stackiterator i = Item->begin(); i != Item->end(); ++i)
	if((**i)->GetCategory() == c && (!UseSorterFunction || SorterFunction(***i, Viewer)) && (**i)->IsVisible())
	  {
	    if(!DescDrawn && Desc.length())
	      {
		if(!ItemNames.IsEmpty())
		  ItemNames.AddEntry("", WHITE, 0, false);

		ItemNames.AddEntry(Desc, WHITE, 0, false);
		ItemNames.AddEntry("", WHITE, 0, false);
		DescDrawn = true;
	      }

	    if(!CatDescDrawn)
	      {
		ItemNames.AddEntry(item::ItemCategoryName(c), LIGHTGRAY, 0, false);
		CatDescDrawn = true;
	      }

	    std::string Buffer;
	    (**i)->AddName(Buffer, INDEFINITE);

	    if(Buffer.length() > 44)
	      {
		Buffer[41] = Buffer[42] = Buffer[43] = '.';
		Buffer[44] = ' ';
	      }

	    Buffer.resize(45,' ');
	    Buffer += (**i)->GetWeight();
	    Buffer.resize(52, ' ');
	    Buffer += (**i)->GetStrengthValue();
	    Buffer.resize(57, ' ');
	    Buffer += ulong((**i)->GetWeaponStrength() / 100);
	    Viewer->AddSpecialItemInfo(Buffer, ***i);

	    if(!SelectItem)
	      Buffer = "   " + Buffer;

	    ItemNames.AddEntry(Buffer, LIGHTGRAY, (**i)->GetPicture());
	  }
    }
}

item* stack::SearchChosen(ushort& Pos, ushort Chosen, character* Viewer, bool (*SorterFunction)(item*, character*)) const
{
  bool UseSorterFunction = SorterFunction != 0;

  for(ushort c = 0; c < ITEM_CATEGORIES; ++c)
    for(stackiterator i = Item->begin(); i != Item->end(); ++i)
      if((**i)->GetCategory() == c && (!UseSorterFunction || SorterFunction(***i, Viewer)) && (**i)->IsVisible() && Pos++ == Chosen)
	return ***i;

  return 0;
}

bool stack::RaiseTheDead(character* Summoner)
{
  for(stackiterator i = Item->begin(); i != Item->end(); ++i)
    {
      if((**i)->RaiseTheDead(Summoner))
	return true;
    }

  return false;
}

bool stack::TryKey(item* Key, character* Applier)
{
  for(stackiterator i = Item->begin(); i != Item->end(); ++i)
    if((**i)->IsVisible())
      {
	if((**i)->TryKey(Key, Applier))
	  return true;
      }

  return false;
}

bool stack::Open(character* Opener)
{
  item* ToBeOpened = DrawContents(Opener, "What do you wish to open?", &item::OpenableSorter);

  if(ToBeOpened == 0)
    return false;

  return ToBeOpened->Open(Opener);
}

void stack::MoveAll(stack* ToStack)
{
  while(GetItems())
    MoveItem(GetBottomSlot(), ToStack);
}

ushort stack::GetVisibleItems() const
{
  ushort VisibleItems = 0;

  for(stackiterator i = Item->begin(); i != Item->end(); ++i)
    if((**i)->IsVisible())
      ++VisibleItems;

  return VisibleItems;
}

item* stack::GetBottomVisibleItem() const
{
  for(stackiterator i = Item->begin(); i != Item->end(); ++i)
    if((**i)->IsVisible())
      return ***i;

  return 0;
}

square* stack::GetSquareUnder() const
{
  if(MotherEntity)
    return MotherEntity->GetSquareUnder();
  else
    return MotherSquare;
}

void stack::SignalVolumeAndWeightChange()
{
  CalculateVolumeAndWeight();

  if(MotherEntity)
    MotherEntity->SignalVolumeAndWeightChange();
}

void stack::CalculateVolumeAndWeight()
{
  Volume = Weight = 0;

  for(stackiterator i = Item->begin(); i != Item->end(); ++i)
    {
      Volume += (**i)->GetVolume();
      Weight += (**i)->GetWeight();
    }
}

void stack::SignalEmitationIncrease(ushort EmitationUpdate)
{
  if(EmitationUpdate > Emitation)
    {
      Emitation = EmitationUpdate;

      if(MotherEntity)
	MotherEntity->SignalEmitationIncrease(EmitationUpdate);
      else
	GetLSquareTrulyUnder()->SignalEmitationIncrease(EmitationUpdate);
    }
}

void stack::SignalEmitationDecrease(ushort EmitationUpdate)
{
  if(EmitationUpdate == Emitation)
    {
      CalculateEmitation();

      if(EmitationUpdate != Emitation && MotherEntity)
	MotherEntity->SignalEmitationDecrease(EmitationUpdate);
      else
	GetLSquareTrulyUnder()->SignalEmitationDecrease(EmitationUpdate);
    }
}

void stack::CalculateEmitation()
{
  Emitation = 0;

  for(stackiterator i = Item->begin(); i != Item->end(); ++i)
    if((**i)->GetEmitation() > Emitation)
      Emitation = (**i)->GetEmitation();
}

