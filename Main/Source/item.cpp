/* Compiled through itemset.cpp */

const char* ToHitValueDescription[] = { "unbelievably inaccurate", "extremely inaccurate", "inaccurate", "decently accurate", "accurate", "highly accurate", "extremely accurate", "unbelievably accurate" };
const char* StrengthValueDescription[] = { "fragile", "rather sturdy", "sturdy", "durable", "very durable", "extremely durable", "almost unbreakable" };

itemprototype::itemprototype(itemprototype* Base, item* (*Cloner)(int, int), const char* ClassID) : Base(Base), Cloner(Cloner), ClassID(ClassID) { Index = protocontainer<item>::Add(this); }

bool itemdatabase::AllowRandomInstantiation() const { return !(Config & S_LOCK_ID); }

item::item(donothing) : Slot(0), CloneMotherID(0), Fluid(0) { }
bool item::IsOnGround() const { return Slot[0]->IsOnGround(); }
bool item::IsSimiliarTo(item* Item) const { return Item->GetType() == GetType() && Item->GetConfig() == GetConfig(); }
int item::GetBaseMinDamage() const { return int(sqrt(GetWeaponStrength() / 20000.) * 0.75); }
int item::GetBaseMaxDamage() const { return int(sqrt(GetWeaponStrength() / 20000.) * 1.25) + 1; }
int item::GetBaseToHitValue() const { return int(10000. / (1000 + GetWeight()) + GetTHVBonus()); }
int item::GetBaseBlockValue() const { return int((10000. / (1000 + GetWeight()) + GetTHVBonus()) * GetBlockModifier() / 10000.); }
bool item::IsInCorrectSlot(int I) const { return I == RIGHT_WIELDED_INDEX || I == LEFT_WIELDED_INDEX; }
bool item::IsInCorrectSlot() const { return IsInCorrectSlot(static_cast<gearslot*>(*Slot)->GetEquipmentIndex()); }
int item::GetEquipmentIndex() const { return static_cast<gearslot*>(*Slot)->GetEquipmentIndex(); }
int item::GetGraphicsContainerIndex() const { return GR_ITEM; }
bool item::IsBroken() const { return !!(GetConfig() & BROKEN); }
const char* item::GetBreakVerb() const { return "breaks"; }
square* item::GetSquareUnderEntity(int I) const { return GetSquareUnder(I); }
square* item::GetSquareUnder(int I) const { return Slot[I] ? Slot[I]->GetSquareUnder() : 0; }
lsquare* item::GetLSquareUnder(int I) const { return static_cast<lsquare*>(Slot[I]->GetSquareUnder()); }
void item::SignalStackAdd(stackslot* StackSlot, void (stack::*)(item*)) { Slot[0] = StackSlot; }
bool item::IsAnimated() const { return object::IsAnimated() || Fluid; }
bool item::IsRusted() const { return MainMaterial->GetRustLevel() != NOT_RUSTED; }
bool item::IsConsumable(const character* Eater) const { return !!GetConsumeMaterial(Eater); }
bool item::IsEatable(const character* Eater) const { return !!GetConsumeMaterial(Eater, &material::IsSolid); }
bool item::IsDrinkable(const character* Eater) const { return !!GetConsumeMaterial(Eater, &material::IsLiquid); }
pixelpredicate item::GetFluidPixelAllowedPredicate() const { return &rawbitmap::IsTransparent; }
void item::Cannibalize() { Flags |= CANNIBALIZED; }
void item::SetMainMaterial(material* NewMaterial, int SpecialFlags) { SetMaterial(MainMaterial, NewMaterial, GetDefaultMainVolume(), SpecialFlags); }
void item::ChangeMainMaterial(material* NewMaterial, int SpecialFlags) { ChangeMaterial(MainMaterial, NewMaterial, GetDefaultMainVolume(), SpecialFlags); }
void item::InitMaterials(const materialscript* M, const materialscript*, bool CUP) { InitMaterials(M->Instantiate(), CUP); }

item::item(const item& Item) : object(Item), Slot(0), Size(Item.Size), DataBase(Item.DataBase), Volume(Item.Volume), Weight(Item.Weight), CloneMotherID(Item.CloneMotherID), Fluid(0), SquaresUnder(Item.SquaresUnder)
{
  Flags &= ENTITY_FLAGS|SQUARE_POSITION_BITS;
  ID = game::CreateNewItemID(this);
  CloneMotherID.push_back(Item.ID);
  Slot = new slot*[SquaresUnder];

  for(int c = 0; c < SquaresUnder; ++c)
    Slot[c] = 0;
}

item::~item()
{
  delete [] Slot;
  game::RemoveItemID(ID);

  fluid** FP = Fluid;

  if(FP)
    {
      for(int c = 0; c < SquaresUnder; ++c)
	for(fluid* F = FP[c]; F;)
	  {
	    fluid* ToDel = F;
	    F = F->Next;
	    delete ToDel;
	  }

      delete [] FP;
    }
}

void item::Fly(character* Thrower, int Direction, int Force)
{
  MoveTo(GetLSquareUnder()->GetStack());

  if(GetSquaresUnder() != 1)
    return;

  int Range = Force * 25UL / Max<long>(long(sqrt(GetWeight())), 1);

  if(!Range)
    return;

  if(Direction == RANDOM_DIR)
    Direction = RAND() & 7;

  vector2d StartingPos = GetPos();
  vector2d Pos = StartingPos;
  vector2d DirVector = game::GetMoveVector(Direction);
  bool Breaks = false;
  double BaseDamage, BaseToHitValue;

  /*** check ***/

  if(Thrower)
    {
      int Bonus = Thrower->IsHumanoid() ? Thrower->GetCWeaponSkill(GetWeaponCategory())->GetBonus() : 1000;
      BaseDamage = sqrt(5e-12 * GetWeaponStrength() * Force / Range) * Bonus;
      BaseToHitValue = 10 * Bonus * Thrower->GetMoveEase() / (500 + GetWeight()) * Thrower->GetAttribute(DEXTERITY) * sqrt(2.5e-8 * Thrower->GetAttribute(PERCEPTION)) / Range;
    }
  else
    {
      BaseDamage = sqrt(5e-6 * GetWeaponStrength() * Force / Range);
      BaseToHitValue = 10 * 100 / (500 + GetWeight()) / Range;
    }

  int RangeLeft;

  for(RangeLeft = Range; RangeLeft; --RangeLeft)
    {
      if(!GetLevel()->IsValidPos(Pos + DirVector))
	break;

      lsquare* JustHit = GetNearLSquare(Pos + DirVector);

      if(!JustHit->IsFlyable())
	{
	  Breaks = true;
	  JustHit->GetOLTerrain()->HasBeenHitByItem(Thrower, this, int(BaseDamage * sqrt(RangeLeft)));
	  break;
	}
      else
	{
	  clock_t StartTime = clock();
	  Pos += DirVector;
	  MoveTo(JustHit->GetStack());
	  bool Draw = game::OnScreen(JustHit->GetPos()) && JustHit->CanBeSeenByPlayer();

	  if(Draw)
	    game::DrawEverything();

	  if(JustHit->GetCharacter())
	    {
	      int Damage = int(BaseDamage * sqrt(RangeLeft));
	      double ToHitValue = BaseToHitValue * RangeLeft;
	      int Returned = HitCharacter(Thrower, JustHit->GetCharacter(), Damage, ToHitValue, Direction);

	      if(Returned == HIT)
		Breaks = true;

	      if(Returned != MISSED)
		break;
	    }

	  if(Draw)
	    while(clock() - StartTime < 0.03 * CLOCKS_PER_SEC);
	}
    }

  if(Breaks)
    ReceiveDamage(Thrower, int(sqrt(GetWeight() * RangeLeft) / 10), THROW|PHYSICAL_DAMAGE, Direction);
}

int item::HitCharacter(character* Thrower, character* Dude, int Damage, double ToHitValue, int Direction)
{
  if(Dude->Catches(this))
    return CATCHED;

  if(Thrower && !EffectIsGood())
    Thrower->Hostility(Dude);

  if(Dude->DodgesFlyingItem(this, ToHitValue)) 
    {
      if(Dude->IsPlayer())
	ADD_MESSAGE("%s misses you.", CHAR_NAME(DEFINITE));
      else if(Dude->CanBeSeenByPlayer())
	ADD_MESSAGE("%s misses %s.", CHAR_NAME(DEFINITE), Dude->CHAR_NAME(DEFINITE));

      return MISSED;
    }

  Dude->HasBeenHitByItem(Thrower, this, Damage, ToHitValue, Direction);
  return HIT;
}

double item::GetWeaponStrength() const
{
  return GetFormModifier() * GetMainMaterial()->GetStrengthValue() * sqrt(GetMainMaterial()->GetWeight());
}

int item::GetStrengthRequirement() const
{
  double WeightTimesSize = GetWeight() * GetSize();
  return int(1.25e-10 * WeightTimesSize * WeightTimesSize);
}

bool item::Apply(character* Applier)
{
  if(Applier->IsPlayer())
    ADD_MESSAGE("You can't apply this!");

  return false;
}

/* Returns bool that tells whether the Polymorph really happened */

bool item::Polymorph(character* Polymorpher, stack* CurrentStack)
{
  if(!IsPolymorphable())
    return false;
  else
    {
      if(Polymorpher && IsOnGround())
	{
	  room* Room = GetRoom();

	  if(Room)
	    Room->HostileAction(Polymorpher);
	}

      if(GetSquarePosition() != CENTER)
	{
	  stack* Stack = CurrentStack->GetLSquareUnder()->GetStackOfAdjacentSquare(GetSquarePosition());

	  if(Stack)
	    CurrentStack = Stack;
	}

      CurrentStack->AddItem(protosystem::BalancedCreateItem(0, MAX_PRICE, ANY_CATEGORY, 0, 0, true));
      RemoveFromSlot();
      SendToHell();
      return true;
    }
}

/* Returns whether the Eater must stop eating the item */

bool item::Consume(character* Eater, long Amount)
{
  material* ConsumeMaterial = GetConsumeMaterial(Eater);

  if(!ConsumeMaterial)
    return true;

  if(Eater->IsPlayer() && !(Flags & CANNIBALIZED) && Eater->CheckCannibalism(ConsumeMaterial))
    {
      game::DoEvilDeed(25);
      ADD_MESSAGE("You feel that this was an evil deed.");
      Cannibalize();
    }

  ulong ID = GetID();
  material* Garbage = ConsumeMaterial->EatEffect(Eater, Amount);
  item* NewConsuming = GetID() ? this : game::SearchItem(ID);
  material* NewConsumeMaterial = NewConsuming->GetConsumeMaterial(Eater);

  if(!NewConsuming->Exists() || !NewConsumeMaterial || !NewConsumeMaterial->IsSameAs(ConsumeMaterial))
    ConsumeMaterial->FinishConsuming(Eater);

  delete Garbage;
  return !NewConsuming->Exists() || !NewConsumeMaterial;
}

bool item::CanBeEatenByAI(const character* Eater) const
{
  return GetConsumeMaterial(Eater)->CanBeEatenByAI(Eater);
}

void item::Save(outputfile& SaveFile) const
{
  SaveFile << (ushort)GetType();
  object::Save(SaveFile);
  SaveFile << (ushort)GetConfig();
  SaveFile << (uchar)Flags;
  SaveFile << Size << ID << CloneMotherID;

  if(Fluid)
    {
      SaveFile << bool(true);

      for(int c = 0; c < SquaresUnder; ++c)
	SaveLinkedList(SaveFile, Fluid[c]);
    }
  else
    SaveFile << bool(false);
}

void item::Load(inputfile& SaveFile)
{
  object::Load(SaveFile);
  databasecreator<item>::InstallDataBase(this, ReadType<ushort>(SaveFile));
  Flags |= ReadType<uchar>(SaveFile) & ~ENTITY_FLAGS;
  SaveFile >> Size >> ID >> CloneMotherID;
  game::AddItemID(this, ID);

  if(ReadType<bool>(SaveFile))
    {
      Fluid = new fluid*[SquaresUnder];

      for(int c = 0; c < SquaresUnder; ++c)
	{
	  LoadLinkedList(SaveFile, Fluid[c]);

	  for(fluid* F = Fluid[c]; F; F = F->Next)
	    F->SetMotherItem(this);
	}
    }
}

void item::TeleportRandomly()
{
  if(GetSquaresUnder() != 1) // gum solution
    {
      lsquare* Square = GetNearLSquare(GetLevel()->GetRandomSquare());
      MoveTo(Square->GetStack());

      if(Square->CanBeSeenByPlayer())
	ADD_MESSAGE("Suddenly %s appears!", CHAR_NAME(INDEFINITE));
    }
}

int item::GetStrengthValue() const
{
  return long(GetStrengthModifier()) * GetMainMaterial()->GetStrengthValue() / 2000;
}

void item::RemoveFromSlot()
{
  for(int c = 0; c < SquaresUnder; ++c)
    if(Slot[c])
      {
	Slot[c]->Empty();
	Slot[c] = 0;
      }
}

void item::MoveTo(stack* Stack)
{
  RemoveFromSlot();
  Stack->AddItem(this);
}

const char* item::GetItemCategoryName(long Category) // convert to array
{
  switch(Category)
    {
    case HELMET: return "Helmets";
    case AMULET: return "Amulets";
    case CLOAK: return "Cloaks";
    case BODY_ARMOR: return "Body armors";
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
    }

  return "Warezzz";
}

int item::GetResistance(int Type) const
{
  switch(Type&0xFFF)
    {
    case PHYSICAL_DAMAGE: return GetStrengthValue();
    case SOUND:
    case ENERGY:
    case ACID:
    case DRAIN:
      return 0;
    case FIRE: return GetFireResistance();
    case POISON: return GetPoisonResistance();
    case ELECTRICITY: return GetElectricityResistance();
    }

  ABORT("Resistance lack detected!");
  return 0;
}

bool item::Open(character* Char)
{
  if(Char->IsPlayer())
    ADD_MESSAGE("You can't open %s.", CHAR_NAME(DEFINITE));

  return false;
}

item* itemprototype::CloneAndLoad(inputfile& SaveFile) const
{
  item* Item = Cloner(0, LOAD);
  Item->Load(SaveFile);
  Item->CalculateAll();
  return Item;
}

void item::LoadDataBaseStats()
{
  SetSize(GetDefaultSize());
}

void item::Initialize(int NewConfig, int SpecialFlags)
{
  CalculateSquaresUnder();
  Slot = new slot*[SquaresUnder];

  for(int c = 0; c < SquaresUnder; ++c)
    Slot[c] = 0;

  if(!(SpecialFlags & LOAD))
    {
      ID = game::CreateNewItemID(this);
      databasecreator<item>::InstallDataBase(this, NewConfig);
      LoadDataBaseStats();
      RandomizeVisualEffects();
      Flags |= CENTER << SQUARE_POSITION_SHIFT;

      if(!(SpecialFlags & NO_MATERIALS))
	GenerateMaterials();
    }

  VirtualConstructor(SpecialFlags & LOAD);

  if(!(SpecialFlags & (LOAD|NO_MATERIALS)))
    {
      CalculateAll();

      if(!(SpecialFlags & NO_PIC_UPDATE))
	UpdatePictures();
    }
}

bool item::ShowMaterial() const
{
  if(GetMainMaterialConfig().Size == 1)
    return GetMainMaterial()->GetConfig() != GetMainMaterialConfig()[0];
  else
    return true;
}

long item::GetBlockModifier() const
{
  if(!IsShield(0))
    return GetSize() * GetRoundness() << 1;
  else
    return GetSize() * GetRoundness() << 2;
}

bool item::CanBeSeenByPlayer() const
{
  return CanBeSeenBy(PLAYER);
}

bool item::CanBeSeenBy(const character* Who) const
{
  for(int c = 0; c < SquaresUnder; ++c)
    if(Slot[c] && Slot[c]->CanBeSeenBy(Who))
      return true;

  return Who->IsPlayer() && game::GetSeeWholeMapCheatMode();
}

festring item::GetDescription(int Case) const
{
  if(CanBeSeenByPlayer())
    return GetName(Case);
  else
    return CONST_S("something");
}

void item::SignalVolumeAndWeightChange()
{
  CalculateVolumeAndWeight();

  for(int c = 0; c < SquaresUnder; ++c)
    if(Slot[c])
      Slot[c]->SignalVolumeAndWeightChange();
}

void item::CalculateVolumeAndWeight()
{
  Volume = Weight = 0;

  for(int c = 0; c < GetMaterials(); ++c)
    if(GetMaterial(c))
      {
	Volume += GetMaterial(c)->GetVolume();
	Weight += GetMaterial(c)->GetWeight();
      }
}

void item::SignalEmitationIncrease(color24 EmitationUpdate)
{
  if(game::CompareLights(EmitationUpdate, Emitation) > 0)
    {
      game::CombineLights(Emitation, EmitationUpdate);

      for(int c = 0; c < SquaresUnder; ++c)
	if(Slot[c])
	  Slot[c]->SignalEmitationIncrease(EmitationUpdate);
    }
}

void item::SignalEmitationDecrease(color24 EmitationUpdate)
{
  if(game::CompareLights(EmitationUpdate, Emitation) >= 0 && Emitation)
    {
      color24 Backup = Emitation;
      CalculateEmitation();

      if(Backup != Emitation)
	for(int c = 0; c < SquaresUnder; ++c)
	  if(Slot[c])
	    Slot[c]->SignalEmitationDecrease(EmitationUpdate);
    }
}

void item::CalculateAll()
{
  CalculateVolumeAndWeight();
  CalculateEmitation();
}

/* Temporary and buggy. */

void item::WeaponSkillHit(int Hits)
{
  if(Slot[0] && Slot[0]->IsGearSlot())
    static_cast<arm*>(static_cast<gearslot*>(*Slot)->GetBodyPart())->WieldedSkillHit(Hits);
}

/* Returns 0 if item cannot be cloned */

item* item::Duplicate()
{
  if(!CanBeCloned())
    return 0;

  item* Clone = RawDuplicate();
  CloneMotherID.push_back(ID);
  game::RemoveItemID(ID);
  ID = game::CreateNewItemID(this);
  Clone->UpdatePictures();
  return Clone;
}

void item::AddInventoryEntry(const character*, festring& Entry, int Amount, bool ShowSpecialInfo) const
{
  if(Amount == 1)
    AddName(Entry, INDEFINITE);
  else
    {
      Entry << Amount << ' ';
      AddName(Entry, PLURAL);
    }

  if(ShowSpecialInfo)
    Entry << " [" << GetWeight() * Amount << "g]";
}

const itemdatabase* itemprototype::ChooseBaseForConfig(itemdatabase** TempConfig, int Configs, int ConfigNumber)
{
  if(!(ConfigNumber & BROKEN))
    return *TempConfig;
  else
    {
      ConfigNumber ^= BROKEN;

      for(int c = 0; c < Configs; ++c)
	if(TempConfig[c]->Config == ConfigNumber)
	  return TempConfig[c];

      return *TempConfig;
    }
}

bool item::ReceiveDamage(character* Damager, int Damage, int Type, int Dir)
{
  if(CanBeBroken() && !IsBroken() && Type & (PHYSICAL_DAMAGE|SOUND|ENERGY|ACID))
    {
      int StrengthValue = GetStrengthValue();

      if(!StrengthValue)
	StrengthValue = 1;

      if(Damage > StrengthValue << 2 && RAND() & 3 && RAND() % (25 * Damage / StrengthValue) >= 100)
	{
	  Break(Damager, Dir);
	  return true;
	}
    }

  if(Type & ACID && IsBroken() && IsDestroyable())
    {
      int StrengthValue = GetStrengthValue();

      if(!StrengthValue)
	StrengthValue = 1;

      if(Damage > StrengthValue << 4 && !(RAND() & 3) && RAND() % (100 * Damage / StrengthValue) >= 100)
	{
	  Destroy(Damager, Dir);
	  return true;
	}
    }

  return false;
}

void itemdatabase::InitDefaults(const itemprototype* NewProtoType, int NewConfig)
{
  IsAbstract = false;
  ProtoType = NewProtoType;
  Config = NewConfig;

  if(NewConfig & BROKEN)
    {
      if(Adjective.GetSize())
	Adjective.Insert(0, "broken ");
      else
	Adjective = CONST_S("broken");

      DefaultSize >>= 1;
      FormModifier >>= 2;
      StrengthModifier >>= 1;
    }
}

long item::GetNutritionValue() const
{
  long NV = 0;

  for(int c = 0; c < GetMaterials(); ++c)
    if(GetMaterial(c))
      NV += GetMaterial(c)->GetTotalNutritionValue();

  return NV;
}

void item::SignalSpoil(material*)
{
  if(!Exists())
    return;

  if(CanBeSeenByPlayer())
    ADD_MESSAGE("%s spoils completely.", CHAR_NAME(DEFINITE));

  RemoveFromSlot();
  SendToHell();
}

item* item::DuplicateToStack(stack* CurrentStack)
{
  item* Duplicated = Duplicate();

  if(!Duplicated)
    return 0;

  CurrentStack->AddItem(Duplicated);
  return Duplicated;
}

bool item::CanBePiledWith(const item* Item, const character* Viewer) const
{
  return GetType() == Item->GetType()
      && GetConfig() == Item->GetConfig()
      && Weight == Item->Weight
      && MainMaterial->IsSameAs(Item->MainMaterial)
      && MainMaterial->GetSpoilLevel() == Item->MainMaterial->GetSpoilLevel()
      && Viewer->GetCWeaponSkillLevel(this) == Viewer->GetCWeaponSkillLevel(Item)
      && Viewer->GetSWeaponSkillLevel(this) == Viewer->GetSWeaponSkillLevel(Item)
      && !Fluid && !Item->Fluid;
}

void item::Break(character* Breaker, int)
{
  if(CanBeSeenByPlayer())
    ADD_MESSAGE("%s %s.", CHAR_NAME(DEFINITE), GetBreakVerb());

  if(Breaker && IsOnGround())
    {
      room* Room = GetRoom();

      if(Room)
	Room->HostileAction(Breaker);
    }

  item* Broken = RawDuplicate();
  Broken->SetConfig(GetConfig() | BROKEN);
  Broken->SetSize(Broken->GetSize() >> 1);
  DonateFluidsTo(Broken);
  DonateIDTo(Broken);
  DonateSlotTo(Broken);
  SendToHell();

  if(PLAYER->Equips(Broken))
    game::AskForKeyPress(CONST_S("Equipment broken! [press any key to continue]"));
}

void item::Be()
{
  MainMaterial->Be();
}

int item::GetOfferValue(int Receiver) const
{
  /* Temporary */

  int OfferValue = int(sqrt(GetTruePrice()));

  if(Receiver == GetAttachedGod())
    OfferValue <<= 1;
  else
    OfferValue >>= 1;

  return OfferValue;
}

void item::SignalEnchantmentChange()
{
  for(int c = 0; c < SquaresUnder; ++c)
    if(Slot[c])
      Slot[c]->SignalEnchantmentChange();
}

long item::GetEnchantedPrice(int Enchantment) const
{
  return !PriceIsProportionalToEnchantment() ? item::GetPrice() : Max<int>(item::GetPrice() * Enchantment, 0);
}

item* item::Fix()
{
  item* Fixed = 0;

  if(IsBroken())
    {
      Fixed = RawDuplicate();
      Fixed->SetConfig(GetConfig() ^ BROKEN);
      Fixed->SetSize(Fixed->GetSize() << 1);
      DonateFluidsTo(Fixed);
      DonateIDTo(Fixed);
      DonateSlotTo(Fixed);
      SendToHell();
    }

  return Fixed;
}

void item::DonateSlotTo(item* Item)
{
  Slot[0]->DonateTo(Item);
  Slot[0] = 0;

  for(int c = 1; c < SquaresUnder; ++c)
    if(Slot[c])
      {
	Slot[c]->Empty();
	Slot[c] = 0;
      }
}

int item::GetSpoilLevel() const
{
  return MainMaterial->GetSpoilLevel();
}

void item::SignalSpoilLevelChange(material*)
{
  if(!IsAnimated() && GetSpoilLevel() && Slot[0] && Slot[0]->IsVisible())
    for(int c = 0; c < SquaresUnder; ++c)
      GetSquareUnder(c)->IncAnimatedEntities();

  SignalVolumeAndWeightChange(); // gum
  UpdatePictures();
}

bool item::AllowSpoil() const
{
  if(IsOnGround())
    {
      lsquare* Square = GetLSquareUnder();
      int RoomNumber = Square->GetRoomIndex();
      return !RoomNumber || Square->GetLevel()->GetRoom(RoomNumber)->AllowSpoil(this);
    }
  else
    return true;
}

void item::ResetSpoiling()
{
  for(int c = 0; c < GetMaterials(); ++c)
    if(GetMaterial(c))
      GetMaterial(c)->ResetSpoiling();
}

const char* item::GetBaseToHitValueDescription() const
{
  if(GetBaseToHitValue() < 10)
    return ToHitValueDescription[Min(GetBaseToHitValue(), 6)];
  else
    return ToHitValueDescription[7];
}

const char* item::GetBaseBlockValueDescription() const
{
  if(GetBaseBlockValue() < 20)
    return ToHitValueDescription[Min(GetBaseBlockValue() >> 1, 6)];
  else
    return ToHitValueDescription[7];
}

const char* item::GetStrengthValueDescription() const
{
  int SV = GetStrengthValue();

  if(SV < 3)
    return StrengthValueDescription[0];
  else if(SV < 5)
    return StrengthValueDescription[1];
  else if(SV < 8)
    return StrengthValueDescription[2];
  else if(SV < 11)
    return StrengthValueDescription[3];
  else if(SV < 16)
    return StrengthValueDescription[4];
  else if(SV < 20)
    return StrengthValueDescription[5];
  else
    return StrengthValueDescription[6];
}

void item::SpecialGenerationHandler()
{
  if(HandleInPairs())
    Slot[0]->AddFriendItem(Duplicate());
}

void item::SortAllItems(itemvector& AllItems, const character* Character, sorter Sorter) const
{
  if(Sorter == 0 || (this->*Sorter)(Character))
    AllItems.push_back(const_cast<item*>(this));
}

int item::GetAttachedGod() const
{
  return DataBase->AttachedGod ? DataBase->AttachedGod : MainMaterial->GetAttachedGod();
}

long item::GetMaterialPrice() const
{
  return MainMaterial->GetRawPrice();
}

long item::GetTruePrice() const
{
  return Max(GetPrice(), GetMaterialPrice());
}

bool item::IsSparkling() const
{
  for(int c = 0; c < GetMaterials(); ++c)
    if(GetMaterial(c) && GetMaterial(c)->IsSparkling())
      return true;

  return false;
}

#ifdef WIZARD

void item::AddAttackInfo(felist& List) const
{
  festring Entry(40, ' ');
  Entry << int(GetWeight());
  Entry.Resize(50);
  Entry << int(GetSize());
  Entry.Resize(60);
  Entry << int(GetStrengthRequirement());
  Entry.Resize(70);
  Entry << int(GetBaseMinDamage()) << '-' << GetBaseMaxDamage();
  int DamageBonus = int(GetDamageBonus());

  if(DamageBonus)
    {
      if(DamageBonus > 0)
	Entry << '+';

      Entry << DamageBonus;
    }

  List.AddEntry(Entry, LIGHT_GRAY);
}

void item::AddMiscellaneousInfo(felist& List) const
{
  festring Entry(40, ' ');
  Entry << int(GetTruePrice());
  Entry.Resize(55);
  Entry << GetOfferValue(0);
  Entry.Resize(70);
  Entry << int(GetNutritionValue());
  List.AddEntry(Entry, LIGHT_GRAY);
}

#endif

void item::PreProcessForBone()
{
  if(IsQuestItem())
    {
      RemoveFromSlot();
      SendToHell();
    }
  else
    {
      game::RemoveItemID(ID);
      ID = -ID;
      game::AddItemID(this, ID);
    }
}

void item::PostProcessForBone()
{
  boneidmap::iterator BI = game::GetBoneItemIDMap().find(-ID);
  game::RemoveItemID(ID);

  if(BI == game::GetBoneItemIDMap().end())
    {
      ulong NewID = game::CreateNewItemID(this);
      game::GetBoneItemIDMap().insert(std::pair<ulong, ulong>(-ID, NewID));
      ID = NewID;
    }
  else
    {
      if(game::SearchItem(BI->second))
	int esko = 2;

      ID = BI->second;
      game::AddItemID(this, ID);
    }

  for(uint c = 0; c < CloneMotherID.size(); ++c)
    {
      BI = game::GetBoneItemIDMap().find(CloneMotherID[c]);

      if(BI == game::GetBoneItemIDMap().end())
	{
	  ulong NewCloneMotherID = game::CreateNewItemID(0);
	  game::GetBoneItemIDMap().insert(std::pair<ulong, ulong>(CloneMotherID[c], NewCloneMotherID));
	  CloneMotherID[c] = NewCloneMotherID;
	}
      else
	CloneMotherID[c] = BI->second;
    }
}

void item::SetConfig(int NewConfig, int SpecialFlags)
{
  databasecreator<item>::InstallDataBase(this, NewConfig);
  CalculateAll();

  if(!(SpecialFlags & NO_PIC_UPDATE))
    UpdatePictures();
}

god* item::GetMasterGod() const
{
  return game::GetGod(GetConfig());
}

int itemprototype::CreateSpecialConfigurations(itemdatabase** TempConfig, int Configs)
{
  if(TempConfig[0]->CreateDivineConfigurations)
    Configs = databasecreator<item>::CreateDivineConfigurations(this, TempConfig, Configs);

  /* Gum solution */

  if(TempConfig[0]->CreateLockConfigurations)
    {
      const item::database*const* KeyConfigData = key_ProtoType.GetConfigData();
      int KeyConfigSize = key_ProtoType.GetConfigSize();
      int OldConfigs = Configs;

      for(int c1 = 0; c1 < OldConfigs; ++c1)
	if(!TempConfig[c1]->IsAbstract)
	  {
	    int BaseConfig = TempConfig[c1]->Config;
	    int NewConfig = BaseConfig | BROKEN_LOCK;
	    itemdatabase* ConfigDataBase = new itemdatabase(*TempConfig[c1]);//Config.insert(std::pair<int, itemdatabase>(NewConfig, i1->second)).first->second;
	    ConfigDataBase->InitDefaults(this, NewConfig);
	    ConfigDataBase->PostFix << "with a broken lock";
	    ConfigDataBase->Possibility = 0;
	    TempConfig[Configs++] = ConfigDataBase;

	    for(int c2 = 0; c2 < KeyConfigSize; ++c2)
	      {
		NewConfig = BaseConfig | KeyConfigData[c2]->Config;
		ConfigDataBase = new itemdatabase(*TempConfig[c1]);// = Config.insert(std::pair<int, itemdatabase>(NewConfig, i1->second)).first->second;
		ConfigDataBase->InitDefaults(this, NewConfig);
		ConfigDataBase->PostFix << "with " << KeyConfigData[c2]->AdjectiveArticle << ' ' << KeyConfigData[c2]->Adjective << " lock";
		ConfigDataBase->Possibility = 0;
		TempConfig[Configs++] = ConfigDataBase;
	      }
	  }
    }

  return Configs;
}

void item::Draw(bitmap* Bitmap, vector2d Pos, color24 Luminance, int SquareIndex, bool AllowAnimate, bool AllowAlpha) const
{
  const int AF = GraphicData.AnimationFrames;
  const bitmap* P = GraphicData.Picture[!AllowAnimate || AF == 1 ? 0 : GET_TICK() % AF];

  if(AllowAlpha)
    P->AlphaBlit(Bitmap, 0, 0, Pos, 16, 16, Luminance);
  else
    P->LuminanceMaskedBlit(Bitmap, 0, 0, Pos, 16, 16, Luminance);

  if(Fluid && ShowFluids())
    DrawFluids(Bitmap, Pos, Luminance, SquareIndex, AllowAnimate);
}

vector2d item::GetLargeBitmapPos(vector2d BasePos, int I) const
{
  const int SquareIndex = I ? I / (GraphicData.AnimationFrames >> 2) : 0;
  return vector2d(SquareIndex & 1 ? BasePos.X + 16 : BasePos.X, SquareIndex & 2 ? BasePos.Y + 16 : BasePos.Y);
}

void item::LargeDraw(bitmap* Bitmap, vector2d Pos, color24 Luminance, int SquareIndex, bool AllowAnimate, bool AllowAlpha) const
{
  const int TrueAF = GraphicData.AnimationFrames >> 2;
  const bitmap* P = GraphicData.Picture[!AllowAnimate ? SquareIndex * TrueAF : SquareIndex * TrueAF + (GET_TICK() % TrueAF)];

  if(AllowAlpha)
    P->AlphaBlit(Bitmap, 0, 0, Pos, 16, 16, Luminance);
  else
    P->LuminanceMaskedBlit(Bitmap, 0, 0, Pos, 16, 16, Luminance);
}

void item::DonateIDTo(item* Item)
{
  game::RemoveItemID(Item->ID);
  game::UpdateItemID(Item, ID);
  Item->ID = ID;
  ID = 0;
}

void item::SignalRustLevelChange()
{
  SignalVolumeAndWeightChange();
  UpdatePictures();
  SendNewDrawAndMemorizedUpdateRequest();
}

const rawbitmap* item::GetRawPicture() const
{
  return igraph::GetRawGraphic(GetGraphicsContainerIndex());
}

void item::RemoveFluid(fluid* ToBeRemoved)
{
  bool HasFluids = false;

  for(int c = 0; c < SquaresUnder; ++c)
    {
      fluid* F = Fluid[c];

      if(F == ToBeRemoved)
	Fluid[c] = F->Next;
      else if(F)
	{
	  fluid* LF = F;

	  for(F = F->Next; F; LF = F, F = F->Next)
	    if(F == ToBeRemoved)
	      {
		LF->Next = F->Next;
		break;
	      }
	}

      if(Fluid[c])
	HasFluids = true;
    }

  UpdatePictures();

  if(!HasFluids)
    {
      delete [] Fluid;
      Fluid = 0;

      if(!IsAnimated() && Slot[0]->IsVisible())
	GetSquareUnder()->DecAnimatedEntities();
    }

  SignalEmitationDecrease(ToBeRemoved->GetEmitation());
  SignalVolumeAndWeightChange();
}

void item::AddFluid(liquid* ToBeAdded, int SquareIndex)
{
  if(Slot[0])
    {
      if(!IsAnimated() && !Fluid && Slot[0]->IsVisible() && ToBeAdded->GetVolume())
	GetSquareUnder()->IncAnimatedEntities();

      SendNewDrawAndMemorizedUpdateRequest();
    }

  if(Fluid)
    {
      fluid* F = Fluid[SquareIndex];

      if(!F)
	Fluid[SquareIndex] = new fluid(ToBeAdded, this, ShowFluids());
      else
	{
	  fluid* LF;

	  do
	    {
	      if(ToBeAdded->IsSameAs(F->GetLiquid()))
		{
		  F->AddLiquidAndVolume(ToBeAdded->GetVolume());
		  delete ToBeAdded;
		  return;
		}

	      LF = F;
	      F = F->Next;
	    }
	  while(F);

	  LF->Next = new fluid(ToBeAdded, this, ShowFluids());
	}
    }
  else
    {
      Fluid = new fluid*[SquaresUnder];
      memset(Fluid, 0, SquaresUnder * sizeof(fluid*));
      Fluid[SquareIndex] = new fluid(ToBeAdded, this, ShowFluids());
    }

  UpdatePictures();
  SignalVolumeAndWeightChange();
  SignalEmitationIncrease(ToBeAdded->GetEmitation());
}

void item::SendNewDrawAndMemorizedUpdateRequest() const
{
  if(!game::IsInWilderness())
    for(int c = 0; c < SquaresUnder; ++c)
      {
	GetLSquareUnder(c)->SendNewDrawRequest();
	GetLSquareUnder(c)->SendMemorizedUpdateRequest();
      }
}

void item::CalculateEmitation()
{
  object::CalculateEmitation();

  if(Fluid)
    for(int c = 0; c < SquaresUnder; ++c)
      for(const fluid* F = Fluid[c]; F; F = F->Next)
	game::CombineLights(Emitation, F->GetEmitation());
}

void item::FillFluidVector(fluidvector& Vector, int SquareIndex) const
{
  if(Fluid)
    for(fluid* F = Fluid[SquareIndex]; F; F = F->Next)
      Vector.push_back(F);
}

void item::SpillFluid(character*, liquid* Liquid, int SquareIndex)
{
  if(AllowFluids() && Liquid->GetVolume())
    AddFluid(Liquid, SquareIndex);
  else
    delete Liquid;
}

void item::TryToRust(long LiquidModifier)
{
  if(MainMaterial->TryToRust(LiquidModifier))
    {
      if(CanBeSeenByPlayer())
	if(MainMaterial->GetRustLevel() == NOT_RUSTED)
	  ADD_MESSAGE("%s rusts.", CHAR_NAME(DEFINITE));
	else
	  ADD_MESSAGE("%s rusts more.", CHAR_NAME(DEFINITE));

      MainMaterial->SetRustLevel(MainMaterial->GetRustLevel() + 1);
    }
}

void item::CheckFluidGearPictures(vector2d ShadowPos, int SpecialFlags, bool BodyArmor)
{
  if(Fluid)
    for(fluid* F = Fluid[0]; F; F = F->Next)
      F->CheckGearPicture(ShadowPos, SpecialFlags, BodyArmor);
}

void item::DrawFluidGearPictures(bitmap* Bitmap, vector2d Pos, color24 Luminance, int SpecialFlags, bool AllowAnimate) const
{
  if(Fluid)
    for(const fluid* F = Fluid[0]; F; F = F->Next)
      F->DrawGearPicture(Bitmap, Pos, Luminance, SpecialFlags, AllowAnimate);
}

void item::DrawFluidBodyArmorPictures(bitmap* Bitmap, vector2d Pos, color24 Luminance, int SpecialFlags, bool AllowAnimate) const
{
  if(Fluid)
    for(const fluid* F = Fluid[0]; F; F = F->Next)
      F->DrawBodyArmorPicture(Bitmap, Pos, Luminance, SpecialFlags, AllowAnimate);
}

void item::DrawFluids(bitmap* Bitmap, vector2d Pos, color24 Luminance, int SquareIndex, bool AllowAnimate) const
{
  for(const fluid* F = Fluid[SquareIndex]; F; F = F->Next)
    F->Draw(Bitmap, Pos, Luminance, AllowAnimate);
}

void item::ReceiveAcid(material*, long Modifier)
{
  if(!GetMainMaterial()->IsImmuneToAcid())
    {
      int Damage = Modifier / 1000;

      if(Damage)
	{
	  Damage += RAND() % Damage;
	  ReceiveDamage(0, Damage, ACID);
	}
    }
}

void item::DonateFluidsTo(item* Item)
{
  if(Fluid)
    for(int c = 0; c < GetSquaresUnder(); ++c)
      for(fluid* F = Fluid[c]; F; F = F->Next)
	{
	  liquid* Liquid = F->GetLiquid();
	  Item->AddFluid(Liquid->CloneLiquid(Liquid->GetVolume()), c);
	}
}

void item::Destroy(character* Destroyer, int)
{
  if(CanBeSeenByPlayer())
    ADD_MESSAGE("%s is destroyed.", CHAR_NAME(DEFINITE));

  if(Destroyer && IsOnGround())
    {
      room* Room = GetRoom();

      if(Room)
	Room->HostileAction(Destroyer);
    }

  bool Equipped = PLAYER->Equips(this);
  RemoveFromSlot();
  SendToHell();

  if(Equipped)
    game::AskForKeyPress(CONST_S("Equipment destroyed! [press any key to continue]"));
}

void item::RemoveRust()
{
  for(int c = 0; c < GetMaterials(); ++c)
    if(GetMaterial(c))
      GetMaterial(c)->SetRustLevel(NOT_RUSTED);
}

void item::SetSpoilPercentage(int Value)
{
  for(int c = 0; c < GetMaterials(); ++c)
    {
      material* Material = GetMaterial(c);

      if(Material && Material->CanSpoil())
	Material->SetSpoilCounter(Material->GetSpoilModifier() * Value / 100);
    }
}

void item::RedistributeFluids()
{
  if(Fluid)
    for(int c = 0; c < GetSquaresUnder(); ++c)
      for(fluid* F = Fluid[c]; F; F = F->Next)
	F->Redistribute();
}

material* item::GetConsumeMaterial(const character* Consumer, materialpredicate Predicate) const
{
  return (MainMaterial->*Predicate)() && Consumer->CanConsume(MainMaterial) ? MainMaterial : 0;
}

/* The parameter can only be MainMaterial */

material* item::RemoveMaterial(material*)
{
  RemoveFromSlot();
  SendToHell();
  return 0;
}

void item::InitMaterials(material* FirstMaterial, bool CallUpdatePictures)
{
  InitMaterial(MainMaterial, FirstMaterial, GetDefaultMainVolume());
  SignalVolumeAndWeightChange();

  if(CallUpdatePictures)
    UpdatePictures();
}

void item::GenerateMaterials()
{
  int Chosen = RandomizeMaterialConfiguration();
  const fearray<long>& MMC = GetMainMaterialConfig();
  InitMaterial(MainMaterial,
	       MAKE_MATERIAL(MMC.Data[MMC.Size == 1 ? 0 : Chosen]),
	       GetDefaultMainVolume());
}

void item::SignalSquarePositionChange(int Position)
{
  Flags &= ~SQUARE_POSITION_BITS;
  Flags |= Position << SQUARE_POSITION_SHIFT;
}

bool item::Read(character* Reader)
{
  Reader->StartReading(this, GetReadDifficulty());
  return true;
}

bool item::CanBeHardened(const character*) const
{
  return MainMaterial->GetHardenedMaterial() != NONE;
}
