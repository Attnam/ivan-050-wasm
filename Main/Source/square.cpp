#include "char.h"
#include "square.h"
#include "bitmap.h"
#include "lterrain.h"
#include "game.h"
#include "igraph.h"
#include "area.h"
#include "level.h"

square::square(area* MotherArea, vector Pos) : MotherArea(MotherArea), Rider(0), Character(0), Flyer(0), Known(false), Pos(Pos)
{
}

square::~square(void)
{
	delete Rider;
	delete Character;
	delete Flyer;
}

void square::Save(std::ofstream* SaveFile) const
{
	if(Character)
		Character->Save(SaveFile);
	else
	{
		ushort Type = 0;

		SaveFile->write((char*)&Type, sizeof(Type));
	}

	SaveFile->write((char*)&Known, sizeof(Known));

	if(Known)
		GetMotherArea()->GetMemorized()->Save(SaveFile, Pos.X << 4, Pos.Y << 4, 16, 16);

	SaveFile->write((char*)&Flag, sizeof(Flag));
}

void square::Load(std::ifstream* SaveFile)
{
	Character = game::LoadCharacter(SaveFile);
	if(Character) Character->SetSquareUnder(this);

	SaveFile->read((char*)&Known, sizeof(Known));

	if(Known)
		GetMotherArea()->GetMemorized()->Load(SaveFile, Pos.X << 4, Pos.Y << 4, 16, 16);

	SaveFile->read((char*)&Flag, sizeof(Flag));
}

/*void square::Save(std::ofstream* SaveFile) const
{
	GroundLevelTerrain->Save(SaveFile);
	OverLevelTerrain->Save(SaveFile);

	if(Character)
		Character->Save(SaveFile);
	else
	{
		ushort Type = 0;

		SaveFile->write((char*)&Type, sizeof(Type));
	}

	SaveFile->write((char*)&Known, sizeof(Known));

	if(Known)
		GetMotherArea()->GetMemorized()->Save(SaveFile, Pos.X << 4, Pos.Y << 4, 16, 16);
}

square::square(area* MotherArea, std::ifstream* SaveFile, vector Pos) : MotherArea(MotherArea), Rider(0), Flyer(0), Pos(Pos)
{
	GroundLevelTerrain = game::LoadGroundLevelTerrain(SaveFile);
	GroundLevelTerrain->SetSquareUnder(this);
	OverLevelTerrain = game::LoadOverLevelTerrain(SaveFile);
	OverLevelTerrain->SetSquareUnder(this);

	Character = game::LoadCharacter(SaveFile);
	if(Character) Character->SetSquareUnder(this);

	SaveFile->read((char*)&Known, sizeof(Known));

	if(Known)
		GetMotherArea()->GetMemorized()->Load(SaveFile, Pos.X << 4, Pos.Y << 4, 16, 16);
}*/

void square::DrawCheat(void) const
{
	DrawToTileBuffer();

	if(CCharacter())
		CCharacter()->DrawToTileBuffer();

	igraph::BlitTileBuffer(vector((GetPos().X - game::CCamera().X) << 4, (GetPos().Y - game::CCamera().Y + 2) << 4));
}

void square::DrawMemorized(void) const
{
	if(GetKnown())
	{
		MotherArea->GetMemorized()->Blit(igraph::GetTileBuffer(), Pos.X << 4, Pos.Y << 4, 0, 0, 16, 16);
		igraph::BlitTileBuffer(vector((GetPos().X - game::CCamera().X) << 4, (GetPos().Y - game::CCamera().Y + 2) << 4));
	}
}

void square::AddCharacter(character* Guy)
{
	Character = Guy;
	Guy->SetSquareUnder(this);
}	//lis�tk�� aborttiii!!!

void square::RemoveCharacter(void)
{
	SetCharacter(0);
}




