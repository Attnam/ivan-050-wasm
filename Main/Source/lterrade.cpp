#include "lterrade.h"
#include "message.h"
#include "god.h"
#include "igraph.h"
#include "save.h"
#include "level.h"
#include "charba.h"
#include "dungeon.h"
#include "feio.h"
#include "hscore.h"
#include "lsquare.h"
#include "worldmap.h"
#include "charba.h"
#include "team.h"
#include "whandler.h"
#include "stack.h"
#include "itemba.h"
#include "femath.h"

bool door::Open(character* Opener)
{
	if(!GetIsWalkable())
	{
		if(IsLocked)
		{
			if(Opener->GetIsPlayer())
				ADD_MESSAGE("The door is locked.");
			
			
			return true;
		}
		else if(RAND() % 20 < Opener->GetStrength())
		{
			if(Opener->GetIsPlayer())
				ADD_MESSAGE("You open the door.");
			else if(GetLevelSquareUnder()->CanBeSeen())
			{
				if(Opener->GetLevelSquareUnder()->CanBeSeen())
					ADD_MESSAGE("%s opens the door.", Opener->CNAME(DEFINITE));
				else
					ADD_MESSAGE("Something opens the door.");
			}
		}
		else
		{
			if(Opener->GetIsPlayer())
				ADD_MESSAGE("The door resists.");
			else if(GetLevelSquareUnder()->CanBeSeen())
				ADD_MESSAGE("%s fails to open the door.", Opener->CNAME(DEFINITE));
			return true;
		}

	}
	else
	{
		if(Opener == game::GetPlayer()) ADD_MESSAGE("The door is already open, %s.", game::Insult());
		return false;
	}

	MakeWalkable();
	return true;
}

bool door::Close(character* Closer)
{
	if(Closer == game::GetPlayer())
		if(GetIsWalkable())
			ADD_MESSAGE("You close the door.");
		else
		{
			ADD_MESSAGE("The door is already closed, %s.", game::Insult());
			return false;
		}

	MakeNotWalkable();
	return true;
}

void altar::DrawToTileBuffer() const
{
	Picture->MaskedBlit(igraph::GetTileBuffer(), 0, 0, 0, 0, 16, 16);
	igraph::GetSymbolGraphic()->MaskedBlit(igraph::GetTileBuffer(), GetOwnerGod() << 4, 0, 0, 0, 16, 16);
}

void altar::Load(inputfile& SaveFile)
{
	levelterrain::Load(SaveFile);

	SaveFile >> OwnerGod;
}

void altar::Save(outputfile& SaveFile) const
{
	levelterrain::Save(SaveFile);

	SaveFile << OwnerGod;
}

bool stairsup::GoUp(character* Who) const  // Try to go up
{
	if(game::GetCurrent())
	{
		if(game::GetCurrent() == 9)
			if(!game::BoolQuestion("Somehow you get the feeling you cannot return. Continue anyway? [y/N]"))
				return false;

		std::vector<character*> MonsterList;

		DO_FOR_SQUARES_AROUND(Who->GetPos().X, Who->GetPos().Y, game::GetCurrentLevel()->GetXSize(), game::GetCurrentLevel()->GetYSize(),
		{
			character* Char = game::GetCurrentLevel()->GetLevelSquare(vector2d(DoX, DoY))->GetCharacter();

			if(Char)
			{
				if(Char->StateIsActivated(CONSUMING)) 
					Char->EndConsuming();	
			
				MonsterList.push_back(Char);
				game::GetCurrentLevel()->RemoveCharacter(vector2d(DoX, DoY));
			}
		})

		game::GetCurrentLevel()->RemoveCharacter(Who->GetPos());
		game::GetCurrentDungeon()->SaveLevel();
		game::SetCurrent(game::GetCurrent() - 1);
		game::GetCurrentDungeon()->PrepareLevel();
		game::GetCurrentLevel()->GetSquare(game::GetCurrentLevel()->GetDownStairs())->KickAnyoneStandingHereAway();
		game::GetCurrentLevel()->FastAddCharacter(game::GetCurrentLevel()->GetDownStairs(), Who);

		for(std::vector<character*>::iterator c = MonsterList.begin(); c != MonsterList.end(); ++c)
			game::GetCurrentLevel()->FastAddCharacter(game::GetCurrentLevel()->GetNearestFreeSquare(game::GetCurrentLevel()->GetDownStairs()), *c);

		game::GetCurrentLevel()->Luxify();
		game::SendLOSUpdateRequest();
		game::UpdateCamera();
		return true;
	}
	else
	{
		if(Who == game::GetPlayer())
		{
			std::vector<character*> TempPlayerGroup;

			DO_FOR_SQUARES_AROUND(Who->GetPos().X, Who->GetPos().Y, game::GetCurrentLevel()->GetXSize(), game::GetCurrentLevel()->GetYSize(),
			{
				character* Char = game::GetCurrentLevel()->GetLevelSquare(vector2d(DoX, DoY))->GetCharacter();

				if(Char)
				{
					if(Char->GetTeam()->GetRelation(Who->GetTeam()) == HOSTILE)
					{
						ADD_MESSAGE("You can't escape when there are hostile creatures nearby.");
						return false;
					}

					if(Char->GetTeam() == Who->GetTeam())
					{
						if(Char->StateIsActivated(CONSUMING)) 
							Char->EndConsuming();	

						TempPlayerGroup.push_back(Char);
						game::GetCurrentLevel()->RemoveCharacter(vector2d(DoX, DoY));
					}
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
}

bool stairsdown::GoDown(character* Who) const  // Try to go down
{
	if(game::GetCurrent() != game::GetLevels() - 1)
	{
		if(game::GetCurrent() == 8)
		{
			ADD_MESSAGE("A great evil power seems to tremble under your feet.");
			ADD_MESSAGE("You feel you shouldn't wander any further.");

			if(!game::BoolQuestion("Continue anyway? [y/N]"))
				return false;

			Who->GetLevelSquareUnder()->ChangeLevelTerrain(new parquet, new empty);
		}

		std::vector<character*> MonsterList;

		DO_FOR_SQUARES_AROUND(Who->GetPos().X, Who->GetPos().Y, game::GetCurrentLevel()->GetXSize(), game::GetCurrentLevel()->GetYSize(),
		{
			character* Char = game::GetCurrentLevel()->GetLevelSquare(vector2d(DoX, DoY))->GetCharacter();

			if(Char)
			{
				if(Char->StateIsActivated(CONSUMING)) 
					Char->EndConsuming();
				MonsterList.push_back(Char);
				game::GetCurrentLevel()->RemoveCharacter(vector2d(DoX, DoY));
			}
		})

		game::GetCurrentLevel()->RemoveCharacter(Who->GetPos());
		game::GetCurrentDungeon()->SaveLevel();
		game::SetCurrent(game::GetCurrent() + 1);
		game::GetCurrentDungeon()->PrepareLevel();
		game::GetCurrentLevel()->GetSquare(game::GetCurrentLevel()->GetUpStairs())->KickAnyoneStandingHereAway();
		game::GetCurrentLevel()->FastAddCharacter(game::GetCurrentLevel()->GetUpStairs(), Who);

		for(std::vector<character*>::iterator c = MonsterList.begin(); c != MonsterList.end(); ++c)
			game::GetCurrentLevel()->FastAddCharacter(game::GetCurrentLevel()->GetNearestFreeSquare(game::GetCurrentLevel()->GetUpStairs()), *c);

		game::GetCurrentLevel()->Luxify();
		game::ShowLevelMessage();
		game::UpdateCamera();
		game::SendLOSUpdateRequest();
		return true;
	}
	else
	{
		if(Who == game::GetPlayer())
			ADD_MESSAGE("You are at the bottom.");

		return false;
	}
}

void door::Kick(ushort Strength, bool ShowOnScreen, uchar)
{
	if(!GetIsWalkable()) 
	{
		if(Strength > RAND() % 30)
		{
			if(ShowOnScreen) ADD_MESSAGE("The door opens.");
			MakeWalkable();
			IsLocked = false;
		}
		else
		{
			//if(ShowOnScreen) ADD_MESSAGE("The door breaks.");
			bool NewLockedStatus;
			if(IsLocked && RAND() % 2) // _can't really think of a good formula for this... 
			{			//Strength isn't everything
				if(ShowOnScreen) ADD_MESSAGE("The lock breaks.");
				NewLockedStatus = false;
			}
			else
			{
				if(ShowOnScreen)
					ADD_MESSAGE("The door is damaged.");
				NewLockedStatus = IsLocked;
			}
			brokendoor* Temp;
			GetLevelSquareUnder()->ChangeOverLevelTerrain(Temp = new brokendoor);
			Temp->SetIsLocked(NewLockedStatus);
		}

		// The door may have been destroyed here, so don't do anything!
	}
}

void door::Save(outputfile& SaveFile) const
{
	levelterrain::Save(SaveFile);

	SaveFile << IsOpen;
	SaveFile << IsLocked;
}

void door::Load(inputfile& SaveFile)
{
	levelterrain::Load(SaveFile);

	SaveFile >> IsOpen;
	SaveFile >> IsLocked;
}

void door::MakeWalkable()
{
	IsOpen = true;

	UpdatePicture();

	GetLevelSquareUnder()->SendNewDrawRequest();
	GetLevelSquareUnder()->SendMemorizedUpdateRequest();
	GetLevelSquareUnder()->SetDescriptionChanged(true);
	GetLevelSquareUnder()->ForceEmitterEmitation();

	if(GetLevelSquareUnder()->GetLastSeen() == game::GetLOSTurns())
		game::SendLOSUpdateRequest();
}

void door::MakeNotWalkable()
{
	GetLevelSquareUnder()->ForceEmitterNoxify();

	IsOpen = false;

	UpdatePicture();

	GetLevelSquareUnder()->SendNewDrawRequest();
	GetLevelSquareUnder()->SendMemorizedUpdateRequest();
	GetLevelSquareUnder()->SetDescriptionChanged(true);
	GetLevelSquareUnder()->ForceEmitterEmitation();

	if(GetLevelSquareUnder()->GetLastSeen() == game::GetLOSTurns())
		game::SendLOSUpdateRequest();
}

void altar::StepOn(character* Stepper)
{
	if(Stepper->GetIsPlayer() && OwnerGod && !game::GetGod(OwnerGod)->GetKnown())
	{
		ADD_MESSAGE("The ancient altar is covered with strange markings. You manage to decipher them.");
		ADD_MESSAGE("The altar is dedicated to %s, the %s.", game::GetGod(OwnerGod)->Name().c_str(), game::GetGod(OwnerGod)->Description().c_str());
		ADD_MESSAGE("You now know the sacred rituals that allow you to contact this deity via prayers.");
		game::GetGod(OwnerGod)->SetKnown(true);
	}
}

void throne::SitOn(character* Sitter)
{
	if(Sitter->HasPetrussNut() && Sitter->HasGoldenEagleShirt() && game::GetGod(1)->GetRelation() != 1000)
	{
		ADD_MESSAGE("You have a strange vision of yourself becoming great ruler.");
		ADD_MESSAGE("The daydream fades in a whisper: \"Thou shalt be a My Champion first!\"");
		return;
	}

	if(Sitter->HasPetrussNut() && !Sitter->HasGoldenEagleShirt() && game::GetGod(1)->GetRelation() == 1000)
	{
		ADD_MESSAGE("You have a strange vision of yourself becoming great ruler.");
		ADD_MESSAGE("The daydream fades in a whisper: \"Thou shalt wear My shining armor first!\"");
		return;
	}

	if(!Sitter->HasPetrussNut() && Sitter->HasGoldenEagleShirt() && game::GetGod(1)->GetRelation() == 1000)
	{
		ADD_MESSAGE("You have a strange vision of yourself becoming great ruler.");
		ADD_MESSAGE("The daydream fades in a whisper: \"Thou shalt surpass thy predecessor first!\"");
		return;
	}

	if(Sitter->HasPetrussNut() && Sitter->HasGoldenEagleShirt() && game::GetGod(1)->GetRelation() == 1000)
	{
		iosystem::TextScreen("A heavenly choir starts to sing Grandis Rana and a booming voice fills the air:\n\n\"Mortal! Thou hast surpassed Petrus, and pleaseth Me greatly during thine adventures!\nI hereby title thee as My new High Priest!\"\n\nYou are victorious!");
		game::RemoveSaves();

		if(!game::GetWizardMode())
		{
			game::GetPlayer()->AddScoreEntry("killed Petrus and became the new High Priest of the Great Frog", 5, false);
			highscore HScore;
			HScore.Draw();
		}

		game::Quit();
		return;
	}

	ADD_MESSAGE("You feel somehow out of place.");
}

void altar::Kick(ushort, bool ShowOnScreen, uchar)
{
	// This function does not currently support AI kicking altars when they are in player's LOS

	if(ShowOnScreen)
		ADD_MESSAGE("You feel like a sinner.");

	game::GetGod(GetLevelSquareUnder()->GetDivineOwner())->PlayerKickedAltar();

	if(GetLevelSquareUnder()->GetDivineOwner() > 1)
		game::GetGod(GetLevelSquareUnder()->GetDivineOwner() - 1)->PlayerKickedFriendsAltar();

	if(GetLevelSquareUnder()->GetDivineOwner() < game::GetGodNumber())
		game::GetGod(GetLevelSquareUnder()->GetDivineOwner() + 1)->PlayerKickedFriendsAltar();
}

void altar::ReceiveVomit(character* Who)
{
	if(Who->GetIsPlayer())
		game::GetGod(GetLevelSquareUnder()->GetDivineOwner())->PlayerVomitedOnAltar();
}

std::string door::Name(uchar Case) const
{
	if(!(Case & PLURAL))
		if(!(Case & DEFINEBIT))
			return std::string(IsOpen ? "open " : "closed ") + GetMaterial(0)->Name()  + " " + NameSingular();
		else
			if(!(Case & INDEFINEBIT))
				return std::string(IsOpen ? "the open " : "the closed ") + GetMaterial(0)->Name()  + " " + NameSingular();
			else
				return std::string(IsOpen ? "an open " : "a closed ") + GetMaterial(0)->Name()  + " " + NameSingular();
	else
		if(!(Case & DEFINEBIT))
			return GetMaterial(0)->Name()  + " " + NamePlural();
		else
			if(!(Case & INDEFINEBIT))
				return std::string("the ") + GetMaterial(0)->Name()  + " " + NamePlural();
			else
				return GetMaterial(0)->Name()  + " " + NamePlural();
}

void couch::SitOn(character*)
{
	ADD_MESSAGE("The couch is extremely soft and confortable. You relax well.");
}

void pool::SitOn(character*)
{
	ADD_MESSAGE("You sit on the pool. Oddly enough, you sink. You feel stupid.");
}

void stairsup::StepOn(character* Stepper)
{
	if(Stepper->GetIsPlayer()) 
		ADD_MESSAGE("There is stairway leading upwards here.");
}

void stairsdown::StepOn(character* Stepper)
{
	if(Stepper->GetIsPlayer()) 
		ADD_MESSAGE("There is stairway leading downwards here.");
}

void bookcase::SitOn(character*)
{
	ADD_MESSAGE("The bookcase is very unconfortable to sit on.");
}

void fountain::SitOn(character*)
{
	ADD_MESSAGE("You sit on the fountain. Water falls on your head and you get quite wet. You feel like a moron.");
}

void doublebed::SitOn(character*)
{
	ADD_MESSAGE("The beautiful bed is very soft. You get a feeling it's not meant for your kind of people.");
}

void fountain::Consume(character* Drinker)
{
	switch(RAND() % 5)
	{
	case 0:
		ADD_MESSAGE("The water tastes good.");
		Drinker->SetNP(Drinker->GetNP() + 30);
	break;

	case 1:
		ADD_MESSAGE("The water is contaminated!");
		Drinker->SetNP(Drinker->GetNP() + 5);
		Drinker->ChangeRandomStat(-1);

		if(!(RAND() % 5))
			Drinker->Polymorph(protosystem::CreateMonster(false));
	break;
	
	case 2:
		ADD_MESSAGE("The water tasted very good.");
		Drinker->SetNP(Drinker->GetNP() + 50);
		Drinker->ChangeRandomStat(1);
	break;

	case 3:
		if(!(RAND() % 5))
		{
			ADD_MESSAGE("You have freed a spirit that grants you a wish. You may wish for an item. - press any key -");
			game::DrawEverything();
			GETKEY();

			while(true)
			{
				std::string Temp = game::StringQuestion("What do you want to wish for?", vector2d(7,7), WHITE, 0, 80);
				item* TempItem = protosystem::CreateItem(Temp, Drinker->GetIsPlayer());

				if(TempItem)
				{
					Drinker->GetStack()->AddItem(TempItem);
					ADD_MESSAGE("%s appears from nothing and the spirit flies happily away!", TempItem->CNAME(INDEFINITE));
					break;
				}
				else
				{
					ADD_MESSAGE("You may try again. - press any key -");
					DRAW_MESSAGES();
					game::DrawEverything();
					GETKEY();
				}
			}
		}


	default:
		DryOut(); 
		// fountain no longer exists: don't do anything here.
	}
}

void fountain::DryOut()
{
	ADD_MESSAGE("%s dries out.", CNAME(DEFINITE));
	GetLevelSquareUnder()->ChangeOverLevelTerrain(new empty);
}

void brokendoor::Kick(ushort Strength, bool ShowOnScreen, uchar)
{
	if(!GetIsWalkable()) 
	{
		if(IsLocked) 
		{
			if(Strength > RAND() % 50)
			{
				if(ShowOnScreen)
					ADD_MESSAGE("The doors opens from the force of your kick.");
				IsLocked = false;
				MakeWalkable();
			}
			else if(Strength > RAND() % 20)
			{
				ADD_MESSAGE("The lock breaks from the force of your kick.");
				IsLocked = false;
			}
			else
				ADD_MESSAGE("The door won't budge!");
		}
		else
			if(Strength > RAND() % 100)
			{
				ADD_MESSAGE("The broken door opens.");
				MakeWalkable();
			}
			else
			{
				ADD_MESSAGE("The door resists.");
			}
	}
}

bool door::ReceiveStrike()
{
	if(RAND() % 2)
	{
		bool NewLockedStatus = IsLocked;

		if(GetLevelSquareUnder()->CanBeSeen())
			ADD_MESSAGE("The wand strikes the door and the door breaks.");

		brokendoor* Temp;
		GetLevelSquareUnder()->ChangeOverLevelTerrain(Temp = new brokendoor);
		Temp->SetIsLocked(NewLockedStatus);
	}
	else
	{
		if(GetLevelSquareUnder()->CanBeSeen())
			ADD_MESSAGE("The wand strikes the door and the door opens.");
		
		MakeWalkable();
		IsLocked = false;
	}

	return true;
}

bool brokendoor::ReceiveStrike()
{
	if(RAND() % 2)
	{
		MakeWalkable();
		IsLocked = false;
		if(GetLevelSquareUnder()->CanBeSeen())
			ADD_MESSAGE("The wand strikes the door and the door opens.");
	}
	else
		ADD_MESSAGE("The wand strikes the door, but the door won't budge.");

	return true;
}

bool altar::Polymorph(character*)
{
	if(GetSquareUnder()->CanBeSeen())
		ADD_MESSAGE("%s glows briefly.", CNAME(DEFINITE));
	
	uchar OldGod = OwnerGod;

	while(OwnerGod == OldGod)
		OwnerGod = RAND() % game::GetGodNumber() + 1;

	GetSquareUnder()->SendNewDrawRequest();
	GetSquareUnder()->SendMemorizedUpdateRequest();
	GetSquareUnder()->SetDescriptionChanged(true);

	if(GetSquareUnder()->CanBeSeen())
		GetSquareUnder()->UpdateMemorizedDescription();

	return true;	
}
