#include "lterraba.h"
#include "level.h"
#include "charba.h"
#include "dungeon.h"
#include "message.h"
#include "igraph.h"
#include "lsquare.h"
#include "worldmap.h"
#include "rand.h"

bool overlevelterrain::GoUp(character* Who) const // Try to go up
{
	if(game::GetCurrent() && game::GetCurrent() != 9 && game::GetWizardMode())
	{
		game::GetCurrentLevel()->RemoveCharacter(Who->GetPos());
		game::GetCurrentDungeon()->SaveLevel();
		game::SetCurrent(game::GetCurrent() - 1);
		game::GetCurrentDungeon()->PrepareLevel();
		vector2d Pos = game::GetCurrentLevel()->RandomSquare(true);
		game::GetCurrentLevel()->FastAddCharacter(Pos, Who);
		game::GetCurrentLevel()->Luxify();
		game::SendLOSUpdateRequest();
		game::UpdateCamera();
		return true;
	}
	else
		if(!game::GetCurrent() && game::GetWizardMode())
		{
			if(Who == game::GetPlayer())
			{
				std::vector<character*> TempPlayerGroup;

				DO_FOR_SQUARES_AROUND(Who->GetPos().X, Who->GetPos().Y, game::GetCurrentLevel()->GetXSize(), game::GetCurrentLevel()->GetYSize(),
				{
					character* Char = game::GetCurrentLevel()->GetLevelSquare(vector2d(DoX, DoY))->GetCharacter();

					if(Char && Char->GetTeam() == Who->GetTeam())
					{
						if(Char->StateIsActivated(CONSUMING)) 
							Char->EndConsuming();

						TempPlayerGroup.push_back(Char);
						game::GetCurrentLevel()->RemoveCharacter(vector2d(DoX, DoY));
					}
				})

				game::GetCurrentArea()->RemoveCharacter(Who->GetPos());
				game::GetCurrentDungeon()->SaveLevel();
				game::LoadWorldMap();

				game::GetWorldMap()->GetPlayerGroup().swap(TempPlayerGroup);

				game::SetInWilderness(true);
				game::GetCurrentArea()->AddCharacter(game::GetCurrentDungeon()->GetWorldMapPos(), Who);
				game::SendLOSUpdateRequest();
				game::UpdateCamera();
				return true;
			}

			return false;
		}
		else
		{
			if(Who == game::GetPlayer())
				ADD_MESSAGE("You can't go up.");

			return false;
		}
}

bool overlevelterrain::GoDown(character* Who) const // Try to go down
{
	if(game::GetCurrent() < game::GetLevels() - 2 && game::GetWizardMode())
	{
		game::GetCurrentLevel()->RemoveCharacter(Who->GetPos());
		game::GetCurrentDungeon()->SaveLevel();
		game::SetCurrent(game::GetCurrent() + 1);
		game::GetCurrentDungeon()->PrepareLevel();
		vector2d Pos = game::GetCurrentLevel()->RandomSquare(true);
		game::GetCurrentLevel()->FastAddCharacter(Pos, Who);
		game::GetCurrentLevel()->Luxify();
		game::SendLOSUpdateRequest();
		game::UpdateCamera();
		return true;
	}
	else
	{
		if(Who == game::GetPlayer())
			ADD_MESSAGE("You can't go down.");

		return false;
	}
}

void levelterrain::Save(outputfile& SaveFile) const
{
	object::Save(SaveFile);

	SaveFile << VisualFlags;
}

void levelterrain::Load(inputfile& SaveFile)
{
	object::Load(SaveFile);

	SaveFile >> VisualFlags;
}

void groundlevelterrain::DrawToTileBuffer() const
{
	Picture->MaskedBlit(igraph::GetTileBuffer(), 0, 0, 0, 0, 16, 16, VisualFlags);
}

void overlevelterrain::DrawToTileBuffer() const
{
	Picture->MaskedBlit(igraph::GetTileBuffer(), 0, 0, 0, 0, 16, 16, VisualFlags);
}

bool levelterrain::Open(character* Opener)
{
	if(Opener == game::GetPlayer())
		ADD_MESSAGE("There isn't anything to open, %s.", game::Insult());

	return false;
}

bool levelterrain::Close(character* Closer)
{
	if(Closer == game::GetPlayer())
		ADD_MESSAGE("There isn't anything to close, %s.", game::Insult());

	return false;
}

vector2d levelterrain::GetPos() const
{
	return GetLevelSquareUnder()->GetPos();
}

void levelterrain::HandleVisualEffects()
{
	uchar Flags = 0, AcceptedFlags = OKVisualEffects();

	for(uchar c = 0; c < 8; ++c)
		if((AcceptedFlags & (1 << c)) && (RAND() % 2))
			Flags |= 1 << c;

	SetVisualFlags(Flags);
}

void overlevelterrain::SitOn(character* Sitter)
{
	if(Sitter->GetIsPlayer())
		ADD_MESSAGE("You sit for some time. Nothing happens.");
}
