#include <typeinfo>

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

template <class type> type* datamembertemplate<type>::GetMember(bool AbortOnError) const
{
  if(Member)
    return Member;
  else
    if(Base)
      return Base->GetMember(AbortOnError);
    else
    {
      if(AbortOnError)
	ABORT("Undefined script member %s sought!", Identifier.c_str());

      return 0;
    }
}

/* Forced instantiations are here to decrease compilation time, at least in the future */

template bool* datamembertemplate<bool>::GetMember(bool) const;
template uchar* datamembertemplate<uchar>::GetMember(bool) const;
template ushort* datamembertemplate<ushort>::GetMember(bool) const;
template vector2d* datamembertemplate<vector2d>::GetMember(bool) const;
template std::string* datamembertemplate<std::string>::GetMember(bool) const;
template posscript* datamembertemplate<posscript>::GetMember(bool) const;
template squarescript* datamembertemplate<squarescript>::GetMember(bool) const;
template levelscript* datamembertemplate<levelscript>::GetMember(bool) const;
template dungeonscript* datamembertemplate<dungeonscript>::GetMember(bool) const;
template contentscript<character>* datamembertemplate<contentscript<character> >::GetMember(bool) const;
template contentscript<item>* datamembertemplate<contentscript<item> >::GetMember(bool) const;
template contentscript<glterrain>* datamembertemplate<contentscript<glterrain> >::GetMember(bool) const;
template contentscript<olterrain>* datamembertemplate<contentscript<olterrain> >::GetMember(bool) const;
template contentmap<character>* datamembertemplate<contentmap<character> >::GetMember(bool) const;
template contentmap<item>* datamembertemplate<contentmap<item> >::GetMember(bool) const;
template contentmap<glterrain>* datamembertemplate<contentmap<glterrain> >::GetMember(bool) const;
template contentmap<olterrain>* datamembertemplate<contentmap<olterrain> >::GetMember(bool) const;

template <class type> void datamembertemplate<type>::SetBase(datamemberbase* What)
{
  /* No type checking here, since only scriptwithbase<basetype>::SetBase uses this */

  Base = (datamembertemplate<type>*)What;
}

template <class type> bool datamember<type>::Load(const std::string& Word, inputfile& SaveFile, const valuemap& ValueMap)
{
  if(Word == Identifier)
    {
      if(!Member)
	Member = new type;

      ReadData(*Member, SaveFile, ValueMap);
      return true;
    }

  return false;
}

template <class type> void datamember<type>::Load(inputfile& SaveFile, const valuemap& ValueMap)
{
  if(!Member)
    Member = new type;

  ReadData(*Member, SaveFile, ValueMap);
}

template <class type> bool protonamedmember<type>::Load(const std::string& Word, inputfile& SaveFile, const valuemap&)
{
  if(Word == Identifier)
    {
      if(!Member)
	Member = new ushort;

      ReadData(*Member, SaveFile, protocontainer<type>::GetCodeNameMap());
      return true;
    }

  return false;
}

template <class type> void protonamedmember<type>::Load(inputfile& SaveFile, const valuemap&)
{
  if(!Member)
    Member = new ushort;

  ReadData(*Member, SaveFile, protocontainer<type>::GetCodeNameMap());
}

template <class basetype> void scriptwithbase<basetype>::SetBase(basetype* NewBase)
{
  if(NewBase == this)
    return;

  Base = NewBase;

  if(Base)
    for(ushort c = 0; c < Data.size(); ++c)
      Data[c]->SetBase(NewBase->Data[c]);
}

posscript::posscript()
{
  INITMEMBER(Vector);
  INITMEMBER(Walkable);
  INITMEMBER(InRoom);
}

void posscript::ReadFrom(inputfile& SaveFile)
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

      if(SaveFile.ReadWord() != "{")
	ABORT("Script error in level script!");

      for(SaveFile.ReadWord(Word); Word != "}"; SaveFile.ReadWord(Word))
	{
	  ushort c;

	  for(c = 0; c < Data.size(); ++c)
	    if(Data[c]->Load(Word, SaveFile, ValueMap))
	      break;

	  if(c == Data.size())
	    ABORT("Odd script term %s encountered in position script!", Word.c_str());
	}
    }
}

materialscript::materialscript() : Type(0)
{
  INITMEMBER(Volume);
}

void materialscript::ReadFrom(inputfile& SaveFile)
{
  std::string Word;
  SaveFile.ReadWord(Word);

  if(Word == "=")
    Word = SaveFile.ReadWord();

  Type = protocontainer<material>::SearchCodeName(Word);

  if(!Type)
    ABORT("Odd script term %s encountered in material script!", Word.c_str());

  if(SaveFile.ReadWord() != "{")
    return;

  for(SaveFile.ReadWord(Word); Word != "}"; SaveFile.ReadWord(Word))
    {
      ushort c;

      for(c = 0; c < Data.size(); ++c)
	if(Data[c]->Load(Word, SaveFile, ValueMap))
	  break;

      if(c == Data.size())
	ABORT("Odd script term %s encountered in material script!", Word.c_str());
    }
}

material* materialscript::Instantiate() const
{
  material* Instance = protocontainer<material>::GetProto(Type)->Clone();

  if(GetVolume(false))
    Instance->SetVolume(*GetVolume());

  return Instance;
}

template <class type> basecontentscript<type>::basecontentscript<type>()
{
  INITMEMBER(MainMaterial);
  INITMEMBER(SecondaryMaterial);
  INITMEMBER(ContainedMaterial);
}

template <class type> void basecontentscript<type>::ReadFrom(inputfile& SaveFile)
{
  std::string Word = SaveFile.ReadWord();

  if(Word == "=")
    Word = SaveFile.ReadWord();

  ushort Index = protocontainer<material>::SearchCodeName(Word);

  if(Index)
    {
      if(!GetMainMaterial(false))
	MainMaterial.SetMember(new materialscript);

      GetMainMaterial()->SetType(Index);
      Word = SaveFile.ReadWord();
    }

  ContentType = protocontainer<type>::SearchCodeName(Word);

  if(ContentType || Word == "0")
    Word = SaveFile.ReadWord();
  else
    ABORT("Odd script term %s encountered content script of %s!", Word.c_str(), typeid(type).name());

  ValueMap["NONE"] = NONE;
  ValueMap["MIRROR"] = MIRROR;
  ValueMap["FLIP"] = FLIP;
  ValueMap["ROTATE"] = ROTATE_90;

  if(Word == "{")
    for(SaveFile.ReadWord(Word); Word != "}"; SaveFile.ReadWord(Word))
      {
	ushort c;

	for(c = 0; c < Data.size(); ++c)
	  if(Data[c]->Load(Word, SaveFile, ValueMap))
	    break;

	if(c == Data.size())
	  ABORT("Odd script term %s encountered content script of %s!", Word.c_str(), typeid(type).name());
      }
  else
    if(Word != ";" && Word != ",")
      ABORT("Script error: Odd terminator %s encountered in content script of %s!", Word.c_str(), typeid(type).name());
}

template <class type> type* basecontentscript<type>::Instantiate() const
{
  type* Instance = protocontainer<type>::GetProto(ContentType)->Clone();

  if(GetMainMaterial(false))
    Instance->ChangeMainMaterial(GetMainMaterial()->Instantiate());

  if(GetSecondaryMaterial(false))
    Instance->ChangeSecondaryMaterial(GetSecondaryMaterial()->Instantiate());

  if(GetContainedMaterial(false))
    Instance->ChangeContainedMaterial(GetContainedMaterial()->Instantiate());

  return Instance;
}

contentscript<character>::contentscript<character>()
{
  INITMEMBER(Team);
}

character* contentscript<character>::Instantiate() const
{
  character* Instance = basecontentscript<character>::Instantiate();

  if(GetTeam(false))
    Instance->SetTeam(game::GetTeam(*GetTeam()));

  return Instance;
}

contentscript<olterrain>::contentscript<olterrain>()
{
  INITMEMBER(Locked);
  INITMEMBER(VisualFlags);
}

olterrain* contentscript<olterrain>::Instantiate() const
{
  olterrain* Instance = basecontentscript<olterrain>::Instantiate();

  if(GetLocked(false) && *GetLocked())
    Instance->Lock();

  if(GetVisualFlags(false))
    Instance->SetVisualFlags(*GetVisualFlags());

  return Instance;
}

squarescript::squarescript()
{
  INITMEMBER(Position);
  INITMEMBER(Character);
  INITMEMBER(Item);
  INITMEMBER(GTerrain);
  INITMEMBER(OTerrain);
  INITMEMBER(IsUpStairs);
  INITMEMBER(IsDownStairs);
  INITMEMBER(IsWorldMapEntry);
  INITMEMBER(Times);
}

void squarescript::ReadFrom(inputfile& SaveFile)
{
  std::string Word;
  SaveFile.ReadWord(Word);

  if(Word != "=")
    {
      Position.Load(SaveFile, ValueMap);

      if(SaveFile.ReadWord() != "{")
	ABORT("Bracket missing in square script!");

      for(SaveFile.ReadWord(Word); Word != "}"; SaveFile.ReadWord(Word))
	{
	  ushort c;

	  for(c = 0; c < Data.size(); ++c)
	    if(Data[c]->Load(Word, SaveFile, ValueMap))
	      break;

	  if(c == Data.size())
	    ABORT("Odd script term %s encountered in square script!", Word.c_str());
	}
    }
  else
    {
      GTerrain.Load(SaveFile, ValueMap);
      OTerrain.Load(SaveFile, ValueMap);
    }
}

template <class type> contentmap<type>::contentmap<type>() : ContentScriptMap(0)
{
  INITMEMBER(Size);
  INITMEMBER(Pos);
}

template <class type> void contentmap<type>::ReadFrom(inputfile& SaveFile)
{
  if(SaveFile.ReadWord() != "{")
    ABORT("Bracket missing in content map script of %s!", typeid(type).name());

  std::map<char, contentscript<type>*> SymbolMap;

  SymbolMap['.'] = 0;

  for(std::string Word = SaveFile.ReadWord(); Word != "}"; Word = SaveFile.ReadWord())
    {
      if(Word == "Types")
	{
	  if(SaveFile.ReadWord() != "{")
	    ABORT("Missing bracket in content map script of %s!", typeid(type).name());

	  for(std::string Word = SaveFile.ReadWord(); Word != "}"; Word = SaveFile.ReadWord())
	    {
	      /* This may cause a memory leak if room's ReCalculate == true!!! */

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

      ushort c;

      for(c = 0; c < Data.size(); ++c)
	if(Data[c]->Load(Word, SaveFile, ValueMap))
	  break;

      if(c == Data.size())
	ABORT("Odd script term %s encountered in content script of %s!", Word.c_str(), typeid(type).name());
    }

  if(!ContentScriptMap)
    Alloc2D(ContentScriptMap, GetSize()->X, GetSize()->Y);

  if(SaveFile.ReadWord() != "{")
    ABORT("Missing bracket in content map script of %s!", typeid(type).name());

  for(ushort y = 0; y < GetSize()->Y; ++y)
    for(ushort x = 0; x < GetSize()->X; ++x)
      {
	char Char = SaveFile.ReadLetter();

	std::map<char, contentscript<type>*>::iterator Iterator = SymbolMap.find(Char);

	if(Iterator != SymbolMap.end())
	  ContentScriptMap[x][y] = Iterator->second;
	else
	  ABORT("Illegal content %c in content map of %s!", Char, typeid(type).name());
      }

  if(SaveFile.ReadWord() != "}")
    ABORT("Missing bracket in content map script of %s!", typeid(type).name());
}

roomscript::roomscript()
{
  INITMEMBER(CharacterMap);
  INITMEMBER(ItemMap);
  INITMEMBER(GTerrainMap);
  INITMEMBER(OTerrainMap);
  INITMEMBER(WallSquare);
  INITMEMBER(FloorSquare);
  INITMEMBER(DoorSquare);
  INITMEMBER(Size);
  INITMEMBER(Pos);
  INITMEMBER(AltarPossible);
  INITMEMBER(GenerateDoor);
  INITMEMBER(ReCalculate);
  INITMEMBER(GenerateTunnel);
  INITMEMBER(DivineMaster);
  INITMEMBER(GenerateLanterns);
  INITMEMBER(Type);
  INITMEMBER(GenerateFountains);
  INITMEMBER(AllowLockedDoors);
  INITMEMBER(AllowBoobyTrappedDoors);
}

void roomscript::ReadFrom(inputfile& SaveFile, bool ReRead)
{
  if(ReRead)
    {
      if(Base)
	if(Base->GetReCalculate(false) && *Base->GetReCalculate(false))
	  Base->ReadFrom(SaveFile, true);

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
    ABORT("Bracket missing in room script!");

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

      ushort c;

      for(c = 0; c < Data.size(); ++c)
	if(Data[c]->Load(Word, SaveFile, ValueMap))
	  break;

      if(c == Data.size())
	ABORT("Odd script term %s encountered in room script!", Word.c_str());
    }
}

levelscript::levelscript()
{
  INITMEMBER(RoomDefault);
  INITMEMBER(FillSquare);
  INITMEMBER(LevelMessage);
  INITMEMBER(Size);
  INITMEMBER(Items);
  INITMEMBER(Rooms);
  INITMEMBER(GenerateMonsters);
  INITMEMBER(ReCalculate);
  INITMEMBER(GenerateUpStairs);
  INITMEMBER(GenerateDownStairs);
  INITMEMBER(OnGround);
  INITMEMBER(TeamDefault);
  INITMEMBER(AmbientLight);
  INITMEMBER(Description);
  INITMEMBER(LOSModifier);
}

void levelscript::ReadFrom(inputfile& SaveFile, bool ReRead)
{
  if(ReRead)
    {
      if(Base)
	if(Base->GetReCalculate(false) && *Base->GetReCalculate(false))
	  Base->ReadFrom(SaveFile, true);

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
    ABORT("Bracket missing in level script!");

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
	  uchar Index = SaveFile.ReadNumber(ValueMap);
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

      ushort c;

      for(c = 0; c < Data.size(); ++c)
	if(Data[c]->Load(Word, SaveFile, ValueMap))
	  break;

      if(c == Data.size())
	ABORT("Odd script term %s encountered in level script!", Word.c_str());
    }

  if(Base && Base->GetRoomDefault(false) && GetRoomDefault(false))
    GetRoomDefault(false)->SetBase(Base->GetRoomDefault(false));
}

dungeonscript::dungeonscript()
{
  INITMEMBER(LevelDefault);
  INITMEMBER(Levels);
}

void dungeonscript::ReadFrom(inputfile& SaveFile)
{
  if(SaveFile.ReadWord() != "{")
    ABORT("Bracket missing in dungeon script!");

  std::string Word;

  for(SaveFile.ReadWord(Word); Word != "}"; SaveFile.ReadWord(Word))
    {
      if(Word == "Level")
	{
	  uchar Index = SaveFile.ReadNumber(ValueMap);
	  std::map<uchar, levelscript*>::iterator Iterator = Level.find(Index);
	  levelscript* LS = Iterator == Level.end() ? new levelscript : Iterator->second;
	  LS->SetValueMap(ValueMap);

	  if(GetLevelDefault(false))
	    LS->SetBase(GetLevelDefault());

	  LS->ReadFrom(SaveFile);
	  Level[Index] = LS;
	  continue;
	}

      ushort c;

      for(c = 0; c < Data.size(); ++c)
	if(Data[c]->Load(Word, SaveFile, ValueMap))
	  break;

      if(c == Data.size())
	ABORT("Odd script term %s encountered in dungeon script!", Word.c_str());
    }

  if(Base && Base->GetLevelDefault(false) && GetLevelDefault(false))
    GetLevelDefault(false)->SetBase(Base->GetLevelDefault(false));
}

teamscript::teamscript()
{
  INITMEMBER(AttackEvilness);
}

void teamscript::ReadFrom(inputfile& SaveFile)
{
  ValueMap["HOSTILE"] = HOSTILE;
  ValueMap["NEUTRAL"] = NEUTRAL;
  ValueMap["FRIEND"] = FRIEND;

  if(SaveFile.ReadWord() != "{")
    ABORT("Bracket missing in team script!");

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

      ushort c;

      for(c = 0; c < Data.size(); ++c)
	if(Data[c]->Load(Word, SaveFile, ValueMap))
	  break;

      if(c == Data.size())
	ABORT("Odd script term %s encountered in team script!", Word.c_str());
    }
}

gamescript::gamescript()
{
  INITMEMBER(DungeonDefault);
  INITMEMBER(Dungeons);
  INITMEMBER(Teams);
}

void gamescript::ReadFrom(inputfile& SaveFile)
{
  std::string Word;

  for(SaveFile.ReadWord(Word, false); !SaveFile.Eof(); SaveFile.ReadWord(Word, false))
    {
      if(Word == "Dungeon")
	{
	  uchar Index = SaveFile.ReadNumber(ValueMap);
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

      ushort c;

      for(c = 0; c < Data.size(); ++c)
	if(Data[c]->Load(Word, SaveFile, ValueMap))
	  break;

      if(c == Data.size())
	ABORT("Odd script term %s encountered in game script!", Word.c_str());
    }
}

