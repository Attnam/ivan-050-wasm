#define __FILE_OF_STATIC_PROTOTYPE_DECLARATIONS__

#include "proto.h"

std::vector<material*>			protocontainer<material>::ProtoData;
std::vector<item*>			protocontainer<item>::ProtoData;
std::vector<character*>			protocontainer<character>::ProtoData;
std::vector<groundlevelterrain*>	protocontainer<groundlevelterrain>::ProtoData;
std::vector<overlevelterrain*>		protocontainer<overlevelterrain>::ProtoData;
std::vector<groundworldmapterrain*>	protocontainer<groundworldmapterrain>::ProtoData;
std::vector<overworldmapterrain*>	protocontainer<overworldmapterrain>::ProtoData;
std::vector<room*>			protocontainer<room>::ProtoData;
std::map<std::string, ushort>		protocontainer<material>::CodeNameMap;
std::map<std::string, ushort>		protocontainer<item>::CodeNameMap;
std::map<std::string, ushort>		protocontainer<character>::CodeNameMap;
std::map<std::string, ushort>		protocontainer<groundlevelterrain>::CodeNameMap;
std::map<std::string, ushort>		protocontainer<overlevelterrain>::CodeNameMap;
std::map<std::string, ushort>		protocontainer<groundworldmapterrain>::CodeNameMap;
std::map<std::string, ushort>		protocontainer<overworldmapterrain>::CodeNameMap;
std::map<std::string, ushort>		protocontainer<room>::CodeNameMap;

#include "materde.h"
#include "itemde.h"
#include "charde.h"
#include "lterrade.h"
#include "wterrade.h"
#include "roomde.h"
#include "message.h"

#include "error.h"

character* protosystem::BalancedCreateMonster(float Multiplier, bool CreateItems)
{
	for(ushort c = 0;; ++c)
	{
		ushort Chosen = 1 + rand() % protocontainer<character>::GetProtoAmount();

		if(!protocontainer<character>::GetProto(Chosen)->Possibility())
			continue;

		character* Monster = protocontainer<character>::GetProto(Chosen)->Clone(true, true, CreateItems);

		float Danger = Monster->Danger(), Difficulty = game::Difficulty();

		if(c == 24 || (Danger < Difficulty * Multiplier * 2.0f && Danger > Difficulty * Multiplier * 0.5f))
		{
			Monster->SetTeam(game::GetTeam(1));
			return Monster;
		}
		else
			delete Monster;
	}
}

item* protosystem::BalancedCreateItem()
{
	ushort SumOfPossibilities = 0, Counter = 0, RandomOne;

	ushort c;

	for(c = 1; c <= protocontainer<item>::GetProtoAmount(); ++c)
		SumOfPossibilities += protocontainer<item>::GetProto(c)->Possibility();
		
	RandomOne = 1 + rand() % (SumOfPossibilities);
	
	for(c = 1; c <= protocontainer<item>::GetProtoAmount(); ++c)
	{
		Counter += protocontainer<item>::GetProto(c)->Possibility();

		if(Counter >= RandomOne)
			return CreateItem(c);
	}

	ABORT("Balanced Create Item kaatuuu");

	return 0;
}

character* protosystem::CreateMonster(ushort Index)
{
	return protocontainer<character>::GetProto(Index)->Clone();
}

item* protosystem::CreateItem(ushort Index)
{
	return protocontainer<item>::GetProto(Index)->Clone();
}

item* protosystem::CreateItem(std::string What, bool Output)
{
	for(ushort c = 1; c <= protocontainer<item>::GetProtoAmount(); ++c)
		if(protocontainer<item>::GetProto(c)->GetNameSingular() == What)
			if(protocontainer<item>::GetProto(c)->CanBeWished())
				return protocontainer<item>::GetProto(c)->CreateWishedItem();
			else if(Output)
			{
				ADD_MESSAGE("You hear a booming voice: \"No, mortal! This will not be done!\"");
				return 0;
			}

	if(Output)
		ADD_MESSAGE("There is no such item.");

	return 0;
}

material* protosystem::CreateMaterial(std::string What, ulong Volume, bool Output)
{
	for(ushort c = 1; c <= protocontainer<material>::GetProtoAmount(); ++c)
		if(protocontainer<material>::GetProto(c)->Name() == What)
			if(protocontainer<material>::GetProto(c)->CanBeWished())
				return protocontainer<material>::GetProto(c)->CreateWishedMaterial(Volume);
			else if(Output)
			{
				ADD_MESSAGE("You hear a booming voice: \"No, mortal! This will not be done!\"");
				return 0;
			}
		
	if(Output)
		ADD_MESSAGE("There is no such item.");

	return 0;
}

material* protosystem::CreateRandomSolidMaterial(ulong Volume)
{
	for(ushort c = 1 + rand() % protocontainer<material>::GetProtoAmount();; c = 1 + rand() % protocontainer<material>::GetProtoAmount())
		if(protocontainer<material>::GetProto(c)->IsSolid())
			return protocontainer<material>::GetProto(c)->Clone(Volume);
}

material* protosystem::CreateMaterial(ushort Index, ulong Volume)
{
	return protocontainer<material>::GetProto(Index)->Clone(Volume);
}
