/* Compiled through dataset.cpp */

itemdatabase** protosystem::ItemConfigData;
int protosystem::ItemConfigDataSize;
itemdatabase** protosystem::ItemCategoryData[ITEM_CATEGORIES];
int protosystem::ItemCategorySize[ITEM_CATEGORIES];
long protosystem::ItemCategoryPossibility[ITEM_CATEGORIES];
long protosystem::TotalItemPossibility;

character* protosystem::BalancedCreateMonster()
{
  for(int c = 0;; ++c)
    {
      double MinDifficulty = game::GetMinDifficulty(), MaxDifficulty = MinDifficulty * 25;
      std::vector<configid> Possible;

      for(int Type = 1; Type < protocontainer<character>::GetSize(); ++Type)
	{
	  const character::prototype* Proto = protocontainer<character>::GetProto(Type);
	  const character::database*const* ConfigData = Proto->GetConfigData();
	  int ConfigSize = Proto->GetConfigSize();

	  for(int c = 0; c < ConfigSize; ++c)
	    {
	      const character::database* DataBase = ConfigData[c];

	      if(!DataBase->IsAbstract && DataBase->CanBeGenerated)
		{
		  configid ConfigID(Type, ConfigData[c]->Config);
		  const dangerid& DangerID = game::GetDangerMap().find(ConfigID)->second;

		  if(DataBase->IsUnique && DangerID.HasBeenGenerated)
		    continue;

		  if(c >= 100)
		    {
		      Possible.push_back(ConfigID);
		      continue;
		    }

		  if(!DataBase->IgnoreDanger)
		    {
		      double Danger = DangerID.EquippedDanger;

		      if(Danger > 99.0 || Danger < 0.01 || (DataBase->IsUnique && Danger < 3.0))
			continue;

		      double DangerModifier = DataBase->DangerModifier == 100 ? Danger : Danger * 100 / DataBase->DangerModifier;

		      if(DangerModifier < MinDifficulty || DangerModifier > MaxDifficulty)
			continue;
		    }

		  if(PLAYER->GetMaxHP() < DataBase->HPRequirementForGeneration)
		    continue;

		  Possible.push_back(ConfigID);
		}
	    }
	}

      if(Possible.empty())
	continue;

      for(int i = 0; i < 25; ++i)
	{
	  configid Chosen = Possible[RAND() % Possible.size()];
	  const character::prototype* Proto = protocontainer<character>::GetProto(Chosen.Type);
	  character* Monster = Proto->Clone(Chosen.Config);

	   if(c >= 100 || ((Monster->GetFrequency() == 10000 || Monster->GetFrequency() > RAND() % 10000) && (Monster->IsUnique() || (Monster->GetTimeToKill(PLAYER, true) > 5000 && PLAYER->GetTimeToKill(Monster, true) < 75000))))
	    {
	      game::SignalGeneration(Chosen);
	      Monster->SetTeam(game::GetTeam(MONSTER_TEAM));
	      return Monster;
	    }
	  else
	    delete Monster;
	}
    }

  /* This line is never reached, but it prevents warnings given by some (stupid) compilers. */

  return 0;
}

item* protosystem::BalancedCreateItem(long MinPrice, long MaxPrice, long RequiredCategory, int SpecialFlags, int ConfigFlags, bool Polymorph)
{
  typedef item::database database;
  database** PossibleCategory[ITEM_CATEGORIES];
  int PossibleCategorySize[ITEM_CATEGORIES];
  long PartialCategoryPossibilitySum[ITEM_CATEGORIES];
  int PossibleCategories = 0;
  long TotalPossibility = 0;
  long database::*PartialPossibilitySumPtr;

  if(RequiredCategory == ANY_CATEGORY)
    {
      PartialPossibilitySumPtr = &database::PartialPossibilitySum;
      PossibleCategory[0] = ItemConfigData;
      PossibleCategorySize[0] = ItemConfigDataSize;
      TotalPossibility = TotalItemPossibility;
      PartialCategoryPossibilitySum[0] = TotalPossibility;
      PossibleCategories = 1;
    }
  else
    {
      PartialPossibilitySumPtr = &database::PartialCategoryPossibilitySum;

      for(long CategoryIndex = 0, Category = 1; CategoryIndex < ITEM_CATEGORIES; ++CategoryIndex, Category <<= 1)
	if(Category & RequiredCategory)
	  {
	    PossibleCategory[PossibleCategories] = ItemCategoryData[CategoryIndex];
	    PossibleCategorySize[PossibleCategories] = ItemCategorySize[CategoryIndex];
	    TotalPossibility += ItemCategoryPossibility[CategoryIndex];
	    PartialCategoryPossibilitySum[PossibleCategories] = TotalPossibility;
	    ++PossibleCategories;
	  }
    }

  for(;;)
    {
      for(int c1 = 0; c1 < BALANCED_CREATE_ITEM_ITERATIONS; ++c1)
	{
	  long Rand = RAND_N(TotalPossibility);
	  int Category;

	  if(RequiredCategory == ANY_CATEGORY)
	    Category = 0;
	  else
	    {
	      for(int c2 = 0;; ++c2)
		if(PartialCategoryPossibilitySum[c2] > Rand)
		  {
		    Category = c2;
		    break;
		  }

	      if(Category)
		Rand -= PartialCategoryPossibilitySum[Category - 1];
	    }

	  const database*const* ChosenCategory = PossibleCategory[Category];
	  const database* ChosenDataBase;

	  if(ChosenCategory[0]->PartialCategoryPossibilitySum > Rand)
	    ChosenDataBase = ChosenCategory[0];
	  else
	    {
	      long A = 0;
	      long B = PossibleCategorySize[Category] - 1;

	      for(;;)
		{
		  long C = (A + B) >> 1;

		  if(A != C)
		    {
		      if(ChosenCategory[C]->*PartialPossibilitySumPtr > Rand)
			B = C;
		      else
			A = C;
		    }
		  else
		    {
		      ChosenDataBase = ChosenCategory[B];
		      break;
		    }
		}
	    }

	  int Config = ChosenDataBase->Config;

	  if((!(ConfigFlags & NO_BROKEN)
	   || !(Config & BROKEN))
	  && (!Polymorph
	   || ChosenDataBase->IsPolymorphSpawnable))
	    {
	      item* Item = ChosenDataBase->ProtoType->Clone(Config, SpecialFlags);

	      /* Optimization, GetTruePrice() may be rather slow */

	      if((MinPrice == 0 && MaxPrice == MAX_PRICE)
	      || (Config & BROKEN && ConfigFlags & IGNORE_BROKEN_PRICE))
		return Item;

	      long Price = Item->GetTruePrice();

	      if(Item->HandleInPairs())
		Price <<= 1;

	      if(Price >= MinPrice && Price <= MaxPrice)
		return Item;

	      delete Item;
	    }
	}

      MinPrice = MinPrice * 3 >> 2;
      MaxPrice = MaxPrice * 5 >> 2;
    }
}

character* protosystem::CreateMonster(int MinDanger, int MaxDanger, int SpecialFlags)
{
  std::vector<configid> Possible;
  character* Monster = 0;

  for(int c = 0; !Monster; ++c)
    {
      for(int Type = 1; Type < protocontainer<character>::GetSize(); ++Type)
	{
	  const character::prototype* Proto = protocontainer<character>::GetProto(Type);
	  const character::database*const* ConfigData = Proto->GetConfigData();
	  int ConfigSize = Proto->GetConfigSize();

	  for(int c = 0; c < ConfigSize; ++c)
	    {
	      const character::database* DataBase = ConfigData[c];

	      if(!DataBase->IsAbstract && DataBase->CanBeGenerated && DataBase->CanBeWished && !DataBase->IsUnique && (DataBase->Frequency == 10000 || DataBase->Frequency > RAND() % 10000))
		{
		  configid ConfigID(Type, DataBase->Config);

		  if((MinDanger > 0 || MaxDanger < 10000) && c < 25)
		    {
		      const dangerid& DangerID = game::GetDangerMap().find(ConfigID)->second;
		      double RawDanger = SpecialFlags & NO_EQUIPMENT ? DangerID.NakedDanger : DangerID.EquippedDanger;
		      int Danger = int(DataBase->DangerModifier == 100 ? RawDanger * 100 : RawDanger * 10000 / DataBase->DangerModifier);

		      if(Danger < MinDanger || Danger > MaxDanger)
			continue;
		    }

		  Possible.push_back(ConfigID);
		}
	    }
	}

      if(Possible.empty())
	{
	  MinDanger = MinDanger > 0 ? Max(MinDanger * 3 >> 2, 1) : 0;
	  MaxDanger = MaxDanger < 10000 ? Min(MaxDanger * 5 >> 2, 9999) : 10000;
	  continue;
	}

      configid Chosen = Possible[RAND() % Possible.size()];
      Monster = protocontainer<character>::GetProto(Chosen.Type)->Clone(Chosen.Config, SpecialFlags);
      game::SignalGeneration(Chosen);
      Monster->SetTeam(game::GetTeam(MONSTER_TEAM));
    }

  return Monster;
}

template <class type> std::pair<int, int> CountCorrectNameLetters(const typename type::database* DataBase, const festring& Identifier)
{
  std::pair<int, int> Result(0, 0);

  if(!DataBase->NameSingular.IsEmpty())
    ++Result.second;

  if(festring::IgnoreCaseFind(Identifier, " " + DataBase->NameSingular + ' ') != festring::NPos)
    Result.first += DataBase->NameSingular.GetSize();

  if(!DataBase->Adjective.IsEmpty())
    ++Result.second;

  if(DataBase->Adjective.GetSize() && festring::IgnoreCaseFind(Identifier, " " + DataBase->Adjective + ' ') != festring::NPos)
    Result.first += DataBase->Adjective.GetSize();

  if(!DataBase->PostFix.IsEmpty())
    ++Result.second;

  if(DataBase->PostFix.GetSize() && festring::IgnoreCaseFind(Identifier, " " + DataBase->PostFix + ' ') != festring::NPos)
    Result.first += DataBase->PostFix.GetSize();

  for(uint c = 0; c < DataBase->Alias.Size; ++c)
    if(festring::IgnoreCaseFind(Identifier, " " + DataBase->Alias[c] + ' ') != festring::NPos)
      Result.first += DataBase->Alias[c].GetSize();

  return Result;
}

template <class type> std::pair<const typename type::prototype*, int> SearchForProto(const festring& What, bool Output)
{
  typedef typename type::prototype prototype;
  typedef typename type::database database;

  festring Identifier;
  Identifier << ' ' << What << ' ';
  bool Illegal = false, Conflict = false;
  std::pair<const prototype*, int> ID(0, 0);
  std::pair<int, int> Best(0, 0);

  for(int c = 1; c < protocontainer<type>::GetSize(); ++c)
    {
      const prototype* Proto = protocontainer<type>::GetProto(c);
      const database*const* ConfigData = Proto->GetConfigData();
      int ConfigSize = Proto->GetConfigSize();

      for(int c = 0; c < ConfigSize; ++c)
	if(!ConfigData[c]->IsAbstract)
	  {
	    bool BrokenRequested = festring::IgnoreCaseFind(Identifier, " broken ") != festring::NPos;

	    if(BrokenRequested == !(ConfigData[c]->Config & BROKEN))
	      continue;

	    std::pair<int, int> Correct = CountCorrectNameLetters<type>(ConfigData[c], Identifier);

	    if(Correct == Best)
	      Conflict = true;
	    else if(Correct.first > Best.first || (Correct.first == Best.first && Correct.second < Best.second))
	      if(ConfigData[c]->CanBeWished || game::WizardModeIsActive())
		{
		  ID.first = Proto;
		  ID.second = ConfigData[c]->Config;
		  Best = Correct;
		  Conflict = false;
		}
	      else
		Illegal = true;
	  }
    }

  if(Output)
    {
      if(!Best.first)
	{
	  if(Illegal)
	    ADD_MESSAGE("You hear a booming voice: \"No, mortal! This will not be done!\"");
	  else
	    ADD_MESSAGE("What a strange wish!");
	}
      else if(Conflict)
	{
	  ADD_MESSAGE("Be more precise!");
	  return std::pair<const prototype*, int>(0, 0);
	}
    }

  return ID;
}

character* protosystem::CreateMonster(const festring& What, int SpecialFlags, bool Output)
{
  std::pair<const character::prototype*, int> ID = SearchForProto<character>(What, Output);

  if(ID.first)
    return ID.first->Clone(ID.second, SpecialFlags);
  else
    return 0;
}

item* protosystem::CreateItem(const festring& What, bool Output)
{
  std::pair<const item::prototype*, int> ID = SearchForProto<item>(What, Output);

  if(ID.first)
    return ID.first->Clone(ID.second);
  else
    return 0;
}

material* protosystem::CreateMaterial(const festring& What, long Volume, bool Output)
{
  for(int c1 = 1; c1 < protocontainer<material>::Size; ++c1)
    {
      const material::prototype* Proto = protocontainer<material>::GetProto(c1);
      const material::database*const* ConfigData = Proto->GetConfigData();
      int ConfigSize = Proto->GetConfigSize();

      for(int c2 = 1; c2 < ConfigSize; ++c2)
	if(ConfigData[c2]->NameStem == What)
	  if(ConfigData[c2]->CanBeWished || game::WizardModeIsActive())
	    return ConfigData[c2]->ProtoType->Clone(ConfigData[c2]->Config, Volume);
	  else if(Output)
	    {
	      ADD_MESSAGE("You hear a booming voice: \"No, mortal! This will not be done!\"");
	      return 0;
	    }
    }
		
  if(Output)
    ADD_MESSAGE("There is no such material.");

  return 0;
}

#ifdef WIZARD

void protosystem::CreateEveryCharacter(charactervector& Character)
{
  for(int c1 = 1; c1 < protocontainer<character>::GetSize(); ++c1)
    {
      const character::prototype* Proto = protocontainer<character>::GetProto(c1);
      const character::database*const* ConfigData = Proto->GetConfigData();
      int ConfigSize = Proto->GetConfigSize();

      for(int c2 = 0; c2 < ConfigSize; ++c2)
	if(!ConfigData[c2]->IsAbstract)
	  Character.push_back(Proto->Clone(ConfigData[c2]->Config));
    }
}

void protosystem::CreateEveryItem(itemvector& Item)
{
  for(int c = 1; c < protocontainer<item>::GetSize(); ++c)
    {
      const item::prototype* Proto = protocontainer<item>::GetProto(c);
      const item::database*const* ConfigData = Proto->GetConfigData();
      int ConfigSize = Proto->GetConfigSize();

      for(int c2 = 0; c2 < ConfigSize; ++c2)
	if(!ConfigData[c2]->IsAbstract && ConfigData[c2]->IsAutoInitializable)
	  Item.push_back(Proto->Clone(ConfigData[c2]->Config));
    }
}

void protosystem::CreateEveryMaterial(std::vector<material*>& Material)
{
  for(int c = 1; c < protocontainer<material>::Size; ++c)
    {
      const material::prototype* Proto = protocontainer<material>::GetProto(c);
      const material::database*const* ConfigData = Proto->GetConfigData();
      int ConfigSize = Proto->GetConfigSize();

      for(int c2 = 1; c2 < ConfigSize; ++c2)
	Material.push_back(Proto->Clone(ConfigData[c2]->Config));
    }
}

#endif

void protosystem::CreateEveryNormalEnemy(charactervector& EnemyVector)
{
  for(int c = 1; c < protocontainer<character>::GetSize(); ++c)
    {
      const character::prototype* Proto = protocontainer<character>::GetProto(c);
      const character::database*const* ConfigData = Proto->GetConfigData();
      int ConfigSize = Proto->GetConfigSize();

      for(int c2 = 0; c2 < ConfigSize; ++c2)
	if(!ConfigData[c2]->IsAbstract && !ConfigData[c2]->IsUnique && ConfigData[c2]->CanBeGenerated)
	  EnemyVector.push_back(Proto->Clone(ConfigData[c2]->Config, NO_PIC_UPDATE|NO_EQUIPMENT_PIC_UPDATE));
    }
}

void protosystem::Initialize()
{
  typedef item::prototype prototype;
  typedef item::database database;
  int c;

  for(c = 1; c < protocontainer<item>::Size; ++c)
    {
      const prototype* Proto = protocontainer<item>::ProtoData[c];
      ItemConfigDataSize += Proto->GetConfigSize();

      if(Proto->GetConfigData()[0]->IsAbstract)
	--ItemConfigDataSize;
    }

  ItemConfigData = new database*[ItemConfigDataSize];
  int Index = 0;

  for(c = 1; c < protocontainer<item>::Size; ++c)
    {
      const prototype* Proto = protocontainer<item>::ProtoData[c];
      const database*const* ProtoConfigData = Proto->GetConfigData();
      const database* MainDataBase = *ProtoConfigData;

      if(!MainDataBase->IsAbstract)
	ItemConfigData[Index++] = const_cast<database*>(MainDataBase);

      int ConfigSize = Proto->GetConfigSize();

      for(int c2 = 1; c2 < ConfigSize; ++c2)
	ItemConfigData[Index++] = const_cast<database*>(ProtoConfigData[c2]);
    }

  database** DataBaseBuffer = new database*[ItemConfigDataSize];

  for(int CategoryIndex = 0, Category = 1; CategoryIndex < ITEM_CATEGORIES; ++CategoryIndex, Category <<= 1)
    {
      long TotalPossibility = 0;
      int CSize = 0;

      for(int c = 0; c < ItemConfigDataSize; ++c)
	{
	  database* DataBase = ItemConfigData[c];

	  if(DataBase->Category == Category)
	    {
	      DataBaseBuffer[CSize++] = DataBase;
	      TotalPossibility += DataBase->Possibility;
	      DataBase->PartialCategoryPossibilitySum = TotalPossibility;
	    }
	}

      ItemCategoryData[CategoryIndex] = new database*[CSize];
      ItemCategorySize[CategoryIndex] = CSize;
      ItemCategoryPossibility[CategoryIndex] = TotalPossibility;
      memcpy(ItemCategoryData[CategoryIndex], DataBaseBuffer, CSize * sizeof(database*));
    }

  delete [] DataBaseBuffer;

  for(c = 0; c < ItemConfigDataSize; ++c)
    {
      database* DataBase = ItemConfigData[c];
      TotalItemPossibility += DataBase->Possibility;
      DataBase->PartialPossibilitySum = TotalItemPossibility;
    }
}
