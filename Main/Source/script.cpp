#include "script.h"
#include "lterraba.h"
#include "charba.h"
#include "allocate.h"
#include "team.h"
#include "itemba.h"
#include "roomba.h"
#include "godba.h"
#include "error.h"
#include "game.h"
#include "proto.h"
#include "save.h"
#include "materba.h"
#include "femath.h"

#define ANALYZEMEMBER(name)\
{\
  if(Identifier == #name)\
    return &name;\
}

template <class type> void datamember<type>::Load(inputfile& SaveFile, const valuemap& ValueMap)
{
  if(!Member)
    Member = new type;

  ReadData(*Member, SaveFile, ValueMap);
}

template void datamember<dungeonscript>::Load(inputfile&, const valuemap&); // this is insane

template <class type> void protonamedmember<type>::Load(inputfile& SaveFile, const valuemap&)
{
  if(!Member)
    Member = new ushort;

  ReadData(*Member, SaveFile, protocontainer<type>::GetCodeNameMap());
}

bool script::LoadData(inputfile& SaveFile, const std::string& Word)
{
  datamemberbase* Data = GetData(Word);

  if(Data)
    {
      Data->Load(SaveFile, ValueMap);
      return true;
    }
  else
    return false;
}

datamemberbase* posscript::GetData(const std::string& Identifier)
{
  ANALYZEMEMBER(Vector);
  ANALYZEMEMBER(Flags);
  ANALYZEMEMBER(Borders);
  return 0;
}

void posscript::ReadFrom(inputfile& SaveFile, bool)
{
  std::string Word;
  SaveFile.ReadWord(Word);

  if(Word == "Pos")
    {
      Random = false;
      Vector.Load(SaveFile, ValueMap);
    }

  if(Word == "Random")
    {
      Random = true;
      Flags.Load(SaveFile, ValueMap);
    }

  if(Word == "BoundedRandom")
    {
      Random = true;
      Borders.Load(SaveFile, ValueMap);
      Flags.Load(SaveFile, ValueMap);
    }
}

datamemberbase* materialscript::GetData(const std::string& Identifier)
{
  ANALYZEMEMBER(Volume);
  return 0;
}

void materialscript::ReadFrom(inputfile& SaveFile, bool)
{
  std::string Word;
  SaveFile.ReadWord(Word);

  if(Word == "=")
    Word = SaveFile.ReadWord();

  valuemap::const_iterator Iterator = ValueMap.find(Word);

  if(Iterator != ValueMap.end())
    Config = Iterator->second;
  else
    ABORT("Unconfigured material script detected at line %d!", SaveFile.TellLine());

  if(SaveFile.ReadWord() != "{")
    return;

  for(SaveFile.ReadWord(Word); Word != "}"; SaveFile.ReadWord(Word))
    if(!LoadData(SaveFile, Word))
      ABORT("Odd script term %s encountered in material script line %d!", Word.c_str(), SaveFile.TellLine());
}

material* materialscript::Instantiate() const
{
  material* Instance = MAKE_MATERIAL(Config);

  if(GetVolume(false))
    Instance->SetVolume(*GetVolume());

  return Instance;
}

datamemberbase* basecontentscript::GetData(const std::string& Identifier)
{
  ANALYZEMEMBER(MainMaterial);
  ANALYZEMEMBER(SecondaryMaterial);
  ANALYZEMEMBER(ContainedMaterial);
  ANALYZEMEMBER(Parameters);
  return 0;
}

void basecontentscript::ReadFrom(inputfile& SaveFile, bool)
{
  std::string Word = SaveFile.ReadWord();

  if(Word == "=")
    Word = SaveFile.ReadWord();

  valuemap::const_iterator Iterator = ValueMap.find(Word);

  if(Iterator != ValueMap.end())
    {
      if(!GetMainMaterial(false))
	MainMaterial.SetMember(new materialscript);

      GetMainMaterial()->SetConfig(Iterator->second);
      Word = SaveFile.ReadWord();
      Iterator = ValueMap.find(Word);

      if(Iterator != ValueMap.end())
	{
	  if(!GetSecondaryMaterial(false))
	    SecondaryMaterial.SetMember(new materialscript);

	  GetSecondaryMaterial()->SetConfig(Iterator->second);
	  Word = SaveFile.ReadWord();
	}
    }

  ContentType = SearchCodeName(Word);

  if(ContentType || Word == "0")
    Word = SaveFile.ReadWord();
  else
    ABORT("Odd script term %s encountered in %s content script, file %s line %d!", Word.c_str(), GetClassId().c_str(), SaveFile.GetFileName().c_str(), SaveFile.TellLine());

  if(Word == "(")
    {
      Config = SaveFile.ReadNumber(ValueMap);
      Word = SaveFile.ReadWord();
    }
  else
    Config = 0;

  if(Word == "{")
    for(SaveFile.ReadWord(Word); Word != "}"; SaveFile.ReadWord(Word))
      {
	if(!LoadData(SaveFile, Word))
	  ABORT("Odd script term %s encountered in %s content script, file %s line %d!", Word.c_str(), GetClassId().c_str(), SaveFile.GetFileName().c_str(), SaveFile.TellLine());
      }
  else
    if(Word != ";" && Word != ",")
      ABORT("Odd terminator %s encountered in %s content script, file %s line %d!", Word.c_str(), GetClassId().c_str(), SaveFile.GetFileName().c_str(), SaveFile.TellLine());
}

template <class type> const std::string& contentscripttemplate<type>::GetClassId() const
{
  return protocontainer<type>::GetMainClassId();
}

template <class type> void contentscripttemplate<type>::BasicInstantiate(std::vector<type*>& Instance, ulong Amount, ushort SpecialFlags) const
{
  const typename type::prototype* Proto = protocontainer<type>::GetProto(ContentType);
  Instance.resize(Amount, 0);

  if(!Config && Proto->IsAbstract())
    {
      const typename type::databasemap& Config = Proto->GetConfig();

      for(ulong c = 0; c < Amount; ++c)
	{
	  ushort ChosenConfig = 1 + RAND() % (Config.size() - 1);

	  for(typename type::databasemap::const_iterator i = Config.begin(); i != Config.end(); ++i)
	    if(!ChosenConfig--)
	      {
		Instance[c] = Proto->Clone(i->first, SpecialFlags|NOPICUPDATE);
		break;
	      }
	}
    }
  else
    {
      for(ulong c = 0; c < Amount; ++c)
	Instance[c] = Proto->Clone(Config, SpecialFlags|NOPICUPDATE);
    }

  if(GetParameters(false))
    for(ulong c = 0; c < Amount; ++c)
      Instance[c]->SetParameters(*GetParameters());

  if(GetMainMaterial(false))
    for(ulong c = 0; c < Amount; ++c)
      Instance[c]->ChangeMainMaterial(GetMainMaterial()->Instantiate(), SpecialFlags|NOPICUPDATE);

  if(GetSecondaryMaterial(false) && Instance[0]->HasSecondaryMaterial())
    for(ulong c = 0; c < Amount; ++c)
      Instance[c]->ChangeSecondaryMaterial(GetSecondaryMaterial()->Instantiate(), SpecialFlags|NOPICUPDATE);

  if(GetContainedMaterial(false) && Instance[0]->HasContainedMaterial())
    for(ulong c = 0; c < Amount; ++c)
      Instance[c]->ChangeContainedMaterial(GetContainedMaterial()->Instantiate(), SpecialFlags|NOPICUPDATE);

  if(!(SpecialFlags & NOPICUPDATE))
    for(ulong c = 0; c < Amount; ++c)
      Instance[c]->UpdatePictures();
}

template <class type> ushort contentscripttemplate<type>::SearchCodeName(const std::string& Word) const
{
  return protocontainer<type>::SearchCodeName(Word);
}

datamemberbase* contentscript<character>::GetData(const std::string& Identifier)
{
  ANALYZEMEMBER(Team);
  return contentscripttemplate<character>::GetData(Identifier);
}

void contentscript<character>::Instantiate(std::vector<character*>& Instance, ulong Amount, ushort SpecialFlags) const
{
  contentscripttemplate<character>::BasicInstantiate(Instance, Amount, SpecialFlags);

  if(GetTeam(false))
    for(ulong c = 0; c < Amount; ++c)
      Instance[c]->SetTeam(game::GetTeam(*GetTeam()));

  for(ulong c = 0; c < Amount; ++c)
    Instance[c]->RestoreHP();
}

character* contentscript<character>::Instantiate(ushort SpecialFlags) const
{
  if(!ContentType)
    return 0;

  std::vector<character*> Instance;
  Instantiate(Instance, 1, SpecialFlags);
  return Instance[0];
}

datamemberbase* contentscript<item>::GetData(const std::string& Identifier)
{
  ANALYZEMEMBER(Team);
  ANALYZEMEMBER(Active);
  ANALYZEMEMBER(SideStackIndex);
  ANALYZEMEMBER(Enchantment);
  return contentscripttemplate<item>::GetData(Identifier);
}

void contentscript<item>::Instantiate(std::vector<item*>& Instance, ulong Amount, ushort SpecialFlags) const
{
  contentscripttemplate<item>::BasicInstantiate(Instance, Amount, SpecialFlags);

  if(GetTeam(false))
    for(ulong c = 0; c < Amount; ++c)
      Instance[c]->SetTeam(*GetTeam());

  if(GetActive(false))
    for(ulong c = 0; c < Amount; ++c)
      Instance[c]->SetIsActive(*GetActive());

  if(GetEnchantment(false))
    for(ulong c = 0; c < Amount; ++c)
      Instance[c]->SetEnchantment(*GetEnchantment());
}

item* contentscript<item>::Instantiate(ushort SpecialFlags) const
{
  if(!ContentType)
    return 0;

  std::vector<item*> Instance;
  Instantiate(Instance, 1, SpecialFlags);
  return Instance[0];
}

void contentscript<glterrain>::Instantiate(std::vector<glterrain*>& Instance, ulong Amount, ushort SpecialFlags) const
{
  contentscripttemplate<glterrain>::BasicInstantiate(Instance, Amount, SpecialFlags);
}

glterrain* contentscript<glterrain>::Instantiate(ushort SpecialFlags) const
{
  if(!ContentType)
    return 0;

  std::vector<glterrain*> Instance;
  Instantiate(Instance, 1, SpecialFlags);
  return Instance[0];
}

datamemberbase* contentscript<olterrain>::GetData(const std::string& Identifier)
{
  ANALYZEMEMBER(VisualEffects);
  ANALYZEMEMBER(AttachedArea);
  ANALYZEMEMBER(AttachedEntry);
  return contentscripttemplate<olterrain>::GetData(Identifier);
}

void contentscript<olterrain>::Instantiate(std::vector<olterrain*>& Instance, ulong Amount, ushort SpecialFlags) const
{
  contentscripttemplate<olterrain>::BasicInstantiate(Instance, Amount, SpecialFlags);

  if(GetVisualEffects(false))
    for(ulong c = 0; c < Amount; ++c)
      {
	Instance[c]->SetVisualEffects(*GetVisualEffects());
	Instance[c]->UpdatePictures();
      }

  if(GetAttachedArea(false))
    for(ulong c = 0; c < Amount; ++c)
      Instance[c]->SetAttachedArea(*GetAttachedArea());

  if(GetAttachedEntry(false))
    for(ulong c = 0; c < Amount; ++c)
      Instance[c]->SetAttachedEntry(*GetAttachedEntry());
}

olterrain* contentscript<olterrain>::Instantiate(ushort SpecialFlags) const
{
  if(!ContentType)
    return 0;

  std::vector<olterrain*> Instance;
  Instantiate(Instance, 1, SpecialFlags);
  return Instance[0];
}

datamemberbase* squarescript::GetData(const std::string& Identifier)
{
  ANALYZEMEMBER(Position);
  ANALYZEMEMBER(Character);
  ANALYZEMEMBER(Item);
  ANALYZEMEMBER(GTerrain);
  ANALYZEMEMBER(OTerrain);
  ANALYZEMEMBER(Times);
  ANALYZEMEMBER(AttachRequired);
  ANALYZEMEMBER(EntryIndex);
  return 0;
}

void squarescript::ReadFrom(inputfile& SaveFile, bool)
{
  std::string Word;
  SaveFile.ReadWord(Word);

  if(Word != "=")
    {
      Position.Load(SaveFile, ValueMap);

      if(SaveFile.ReadWord() != "{")
	ABORT("Bracket missing in square script line %d!", SaveFile.TellLine());

      for(SaveFile.ReadWord(Word); Word != "}"; SaveFile.ReadWord(Word))
	if(!LoadData(SaveFile, Word))
	  ABORT("Odd script term %s encountered in square script line %d!", Word.c_str(), SaveFile.TellLine());
    }
  else
    {
      GTerrain.Load(SaveFile, ValueMap);
      OTerrain.Load(SaveFile, ValueMap);
    }
}

template <class type> datamemberbase* contentmap<type>::GetData(const std::string& Identifier)
{
  ANALYZEMEMBER(Size);
  ANALYZEMEMBER(Pos);
  return 0;
}

template <class type> void contentmap<type>::DeleteContents()
{
  for(ushort y1 = 0; y1 < GetSize()->Y; ++y1)
    for(ushort x1 = 0; x1 < GetSize()->X; ++x1)
      {
	contentscript<type>* CS = ContentScriptMap[x1][y1];

	for(ushort y2 = 0; y2 < GetSize()->Y; ++y2)
	  for(ushort x2 = 0; x2 < GetSize()->X; ++x2)
	    if(ContentScriptMap[x2][y2] == CS)
	      ContentScriptMap[x2][y2] = 0;

	delete CS;
      }
}

template <class type> void contentmap<type>::ReadFrom(inputfile& SaveFile, bool)
{
  if(SaveFile.ReadWord() != "{")
    ABORT("Bracket missing in %s content map script line %d!", protocontainer<type>::GetMainClassId().c_str(), SaveFile.TellLine());

  std::map<char, contentscript<type>*> SymbolMap;

  SymbolMap['.'] = 0;

  for(std::string Word = SaveFile.ReadWord(); Word != "}"; Word = SaveFile.ReadWord())
    {
      if(Word == "Types")
	{
	  if(SaveFile.ReadWord() != "{")
	    ABORT("Missing bracket in %s content map script line %d!", protocontainer<type>::GetMainClassId().c_str(), SaveFile.TellLine());

	  for(std::string Word = SaveFile.ReadWord(); Word != "}"; Word = SaveFile.ReadWord())
	    {
	      contentscript<type>* ContentScript = new contentscript<type>;
	      ContentScript->SetValueMap(ValueMap);
	      ContentScript->ReadFrom(SaveFile);

	      if(ContentScript->GetContentType())
		SymbolMap[Word[0]] = ContentScript;
	      else
		{
		  SymbolMap[Word[0]] = 0;
		  delete ContentScript;
		}
	    }

	  continue;
	}

      if(!LoadData(SaveFile, Word))
	ABORT("Odd script term %s encountered in %s content script line %d!", Word.c_str(), protocontainer<type>::GetMainClassId().c_str(), SaveFile.TellLine());
    }

  if(!ContentScriptMap)
    Alloc2D(ContentScriptMap, GetSize()->X, GetSize()->Y);
  else
    DeleteContents();

  if(SaveFile.ReadWord() != "{")
    ABORT("Missing bracket in %s content map script line %d!", protocontainer<type>::GetMainClassId().c_str(), SaveFile.TellLine());

  for(ushort y = 0; y < GetSize()->Y; ++y)
    for(ushort x = 0; x < GetSize()->X; ++x)
      {
	char Char = SaveFile.ReadLetter();

	typename std::map<char, contentscript<type>*>::iterator Iterator = SymbolMap.find(Char);

	if(Iterator != SymbolMap.end())
	  ContentScriptMap[x][y] = Iterator->second;
	else
	  ABORT("Illegal content %c in %s content map line %d!", Char, protocontainer<type>::GetMainClassId().c_str(), SaveFile.TellLine());
      }

  if(SaveFile.ReadWord() != "}")
    ABORT("Missing bracket in %s content map script line %d!", protocontainer<type>::GetMainClassId().c_str(), SaveFile.TellLine());
}

datamemberbase* roomscript::GetData(const std::string& Identifier)
{
  ANALYZEMEMBER(CharacterMap);
  ANALYZEMEMBER(ItemMap);
  ANALYZEMEMBER(GTerrainMap);
  ANALYZEMEMBER(OTerrainMap);
  ANALYZEMEMBER(WallSquare);
  ANALYZEMEMBER(FloorSquare);
  ANALYZEMEMBER(DoorSquare);
  ANALYZEMEMBER(Size);
  ANALYZEMEMBER(Pos);
  ANALYZEMEMBER(AltarPossible);
  ANALYZEMEMBER(GenerateDoor);
  ANALYZEMEMBER(ReCalculate);
  ANALYZEMEMBER(GenerateTunnel);
  ANALYZEMEMBER(DivineMaster);
  ANALYZEMEMBER(GenerateLanterns);
  ANALYZEMEMBER(Type);
  ANALYZEMEMBER(GenerateFountains);
  ANALYZEMEMBER(AllowLockedDoors);
  ANALYZEMEMBER(AllowBoobyTrappedDoors);
  return 0;
}

roomscript::~roomscript()
{
  for(std::vector<squarescript*>::iterator i = Square.begin(); i != Square.end(); ++i)
    delete *i;
}

void roomscript::ReadFrom(inputfile& SaveFile, bool ReRead)
{
  if(ReRead)
    {
      roomscript* RoomBase = static_cast<roomscript*>(Base);

      if(RoomBase)
	if(RoomBase->GetReCalculate(false) && *RoomBase->GetReCalculate(false))
	  RoomBase->ReadFrom(SaveFile, true);

      if(GetReCalculate(false) && *GetReCalculate(false))
	{
	  SaveFile.ClearFlags();
	  SaveFile.SeekPosBeg(BufferPos);
	}
      else
	return;
    }
  else
    BufferPos = SaveFile.TellPos();

  if(SaveFile.ReadWord() != "{")
    ABORT("Bracket missing in room script line %d!", SaveFile.TellLine());

  std::string Word;

  for(SaveFile.ReadWord(Word); Word != "}"; SaveFile.ReadWord(Word))
    {
      if(Word == "Square")
	{
	  squarescript* SS = new squarescript;
	  SS->SetValueMap(ValueMap);
	  SS->ReadFrom(SaveFile);

	  if(!ReRead)
	    Square.push_back(SS);
	  else
	    delete SS;

	  continue;
	}

      if(!LoadData(SaveFile, Word))
	ABORT("Odd script term %s encountered in room script line %d!", Word.c_str(), SaveFile.TellLine());
    }
}

datamemberbase* levelscript::GetData(const std::string& Identifier)
{
  ANALYZEMEMBER(RoomDefault);
  ANALYZEMEMBER(FillSquare);
  ANALYZEMEMBER(LevelMessage);
  ANALYZEMEMBER(Size);
  ANALYZEMEMBER(Items);
  ANALYZEMEMBER(Rooms);
  ANALYZEMEMBER(GenerateMonsters);
  ANALYZEMEMBER(ReCalculate);
  ANALYZEMEMBER(OnGround);
  ANALYZEMEMBER(TeamDefault);
  ANALYZEMEMBER(AmbientLight);
  ANALYZEMEMBER(Description);
  ANALYZEMEMBER(LOSModifier);
  ANALYZEMEMBER(IgnoreDefaultSpecialSquares);
  return 0;
}

levelscript::~levelscript()
{
  for(std::vector<squarescript*>::iterator i1 = Square.begin(); i1 != Square.end(); ++i1)
    delete *i1;

  for(std::map<uchar, roomscript*>::iterator i2 = Room.begin(); i2 != Room.end(); ++i2)
    delete i2->second;
}

void levelscript::ReadFrom(inputfile& SaveFile, bool ReRead)
{
  if(ReRead)
    {
      levelscript* LevelBase = static_cast<levelscript*>(Base);

      if(LevelBase)
	if(LevelBase->GetReCalculate(false) && *LevelBase->GetReCalculate(false))
	  LevelBase->ReadFrom(SaveFile, true);

      if(GetReCalculate(false) && *GetReCalculate())
	{
	  SaveFile.ClearFlags();
	  SaveFile.SeekPosBeg(BufferPos);
	}
      else
	return;
    }
  else
    BufferPos = SaveFile.TellPos();

  if(SaveFile.ReadWord() != "{")
    ABORT("Bracket missing in level script line %d!", SaveFile.TellLine());

  std::string Word;

  for(SaveFile.ReadWord(Word); Word != "}"; SaveFile.ReadWord(Word))
    {
      if(Word == "Square")
	{
	  squarescript* SS = new squarescript;
	  SS->SetValueMap(ValueMap);
	  SS->ReadFrom(SaveFile);

	  if(!ReRead)
	    Square.push_back(SS);
	  else
	    delete SS;

	  continue;
	}

      if(Word == "Room")
	{
	  ushort Index = SaveFile.ReadNumber(ValueMap);
	  std::map<uchar, roomscript*>::iterator Iterator = Room.find(Index);
	  roomscript* RS = Iterator == Room.end() ? new roomscript : Iterator->second;

	  RS->SetValueMap(ValueMap);

	  if(GetRoomDefault(false))
	    RS->SetBase(GetRoomDefault());

	  RS->ReadFrom(SaveFile, ReRead);
	  Room[Index] = RS;
	  continue;
	}

      if(Word == "Variable")
	{
	  SaveFile.ReadWord(Word);
	  ValueMap[Word] = SaveFile.ReadNumber(ValueMap);
	  continue;
	}

      if(!LoadData(SaveFile, Word))
	ABORT("Odd script term %s encountered in level scrip line %d!", Word.c_str(), SaveFile.TellLine());
    }

  levelscript* LevelBase = static_cast<levelscript*>(Base);

  if(LevelBase && RoomDefault.GetMember())
    RoomDefault.GetMember()->SetBase(LevelBase->RoomDefault.GetMember());
}

datamemberbase* dungeonscript::GetData(const std::string& Identifier)
{
  ANALYZEMEMBER(LevelDefault);
  ANALYZEMEMBER(Levels);
  return 0;
}

dungeonscript::~dungeonscript()
{
  for(std::map<uchar, levelscript*>::iterator i = Level.begin(); i != Level.end(); ++i)
    delete i->second;
}

void dungeonscript::ReadFrom(inputfile& SaveFile, bool)
{
  if(SaveFile.ReadWord() != "{")
    ABORT("Bracket missing in dungeon script line %d!", SaveFile.TellLine());

  std::string Word;

  for(SaveFile.ReadWord(Word); Word != "}"; SaveFile.ReadWord(Word))
    {
      if(Word == "Level")
	{
	  ushort Index = SaveFile.ReadNumber(ValueMap);
	  std::map<uchar, levelscript*>::iterator Iterator = Level.find(Index);
	  levelscript* LS = Iterator == Level.end() ? new levelscript : Iterator->second;
	  LS->SetValueMap(ValueMap);

	  if(GetLevelDefault(false))
	    LS->SetBase(GetLevelDefault());

	  LS->ReadFrom(SaveFile);
	  Level[Index] = LS;
	  continue;
	}

      if(!LoadData(SaveFile, Word))
	ABORT("Odd script term %s encountered in dungeon script line %d!", Word.c_str(), SaveFile.TellLine());
    }

  dungeonscript* DungeonBase = static_cast<dungeonscript*>(Base);

  if(DungeonBase && LevelDefault.GetMember())
    LevelDefault.GetMember()->SetBase(DungeonBase->LevelDefault.GetMember());
}

datamemberbase* teamscript::GetData(const std::string& Identifier)
{
  ANALYZEMEMBER(AttackEvilness);
  return 0;
}

void teamscript::ReadFrom(inputfile& SaveFile, bool)
{
  if(SaveFile.ReadWord() != "{")
    ABORT("Bracket missing in team script line %d!", SaveFile.TellLine());

  std::string Word;

  for(SaveFile.ReadWord(Word); Word != "}"; SaveFile.ReadWord(Word))
    {
      if(Word == "Relation")
	{
	  std::pair<uchar, uchar> Rel;
	  Rel.first = SaveFile.ReadNumber(ValueMap);
	  Rel.second = SaveFile.ReadNumber(ValueMap);
	  Relation.push_back(Rel);
	  continue;
	}

      if(!LoadData(SaveFile, Word))
	ABORT("Odd script term %s encountered in team script line %d!", Word.c_str(), SaveFile.TellLine());
    }
}

datamemberbase* gamescript::GetData(const std::string& Identifier)
{
  ANALYZEMEMBER(DungeonDefault);
  ANALYZEMEMBER(Dungeons);
  ANALYZEMEMBER(Teams);
  return 0;
}

gamescript::~gamescript()
{
  for(std::vector<std::pair<uchar, teamscript*> >::iterator i1 = Team.begin(); i1 != Team.end(); ++i1)
    delete i1->second;

  for(std::map<uchar, dungeonscript*>::iterator i2 = Dungeon.begin(); i2 != Dungeon.end(); ++i2)
    delete i2->second;
}

void gamescript::ReadFrom(inputfile& SaveFile, bool)
{
  std::string Word;

  for(SaveFile.ReadWord(Word, false); !SaveFile.Eof(); SaveFile.ReadWord(Word, false))
    {
      if(Word == "Dungeon")
	{
	  ushort Index = SaveFile.ReadNumber(ValueMap);
	  std::map<uchar, dungeonscript*>::iterator Iterator = Dungeon.find(Index);
	  dungeonscript* DS = Iterator == Dungeon.end() ? new dungeonscript : Iterator->second;
	  DS->SetValueMap(ValueMap);
	  DS->SetBase(GetDungeonDefault(false));
	  DS->ReadFrom(SaveFile);
	  Dungeon[Index] = DS;
	  continue;
	}

      if(Word == "Team")
	{
	  ushort Index = SaveFile.ReadNumber(ValueMap);
	  teamscript* TS = new teamscript;
	  TS->SetValueMap(ValueMap);
	  TS->ReadFrom(SaveFile);
	  Team.push_back(std::pair<uchar, teamscript*>(Index, TS));
	  continue;
	}

      if(!LoadData(SaveFile, Word))
	ABORT("Odd script term %s encountered in game script line %d!", Word.c_str(), SaveFile.TellLine());
    }
}

