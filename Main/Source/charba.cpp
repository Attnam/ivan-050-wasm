#include <cmath>

#include "charba.h"
#include "stack.h"
#include "error.h"
#include "message.h"
#include "itemde.h"
#include "lsquare.h"
#include "lterraba.h"
#include "wsquare.h"
#include "wterraba.h"
#include "dungeon.h"
#include "whandler.h"
#include "level.h"
#include "worldmap.h"
#include "igraph.h"
#include "hscore.h"
#include "feio.h"
#include "god.h"
#include "felist.h"
#include "team.h"
#include "femath.h"
#include "colorbit.h"
#include "graphics.h"

character::character(bool CreateMaterials, bool SetStats, bool CreateEquipment, bool AddToPool) : object(AddToPool), Stack(new stack), Wielded(0), RegenerationCounter(0), NP(1000), AP(0), StrengthExperience(0), EnduranceExperience(0), AgilityExperience(0), PerceptionExperience(0), IsPlayer(false), State(0), Team(0), WayPoint(0xFFFF, 0xFFFF)
{
	if(CreateMaterials || SetStats || CreateEquipment)
		ABORT("BOOO!");

	StateHandler[FAINTED] = &character::FaintHandler;
	StateHandler[EATING] = &character::EatHandler;
	StateHandler[POLYMORPHED] = &character::PolymorphHandler;
	StateHandler[RESTING] = &character::RestHandler;
	StateHandler[DIGGING] = &character::DigHandler;
}

character::~character()
{
	delete Stack;
}

void character::ReceiveSound(char* Pointer, short Success, float ScreamStrength)
{
	if(GetIsPlayer())
		ADD_MESSAGE(Pointer);

	ushort Damage = ushort(ScreamStrength * (1 + float(Success) / 100) / 20000);

	SetHP(HP - Damage);
	GetStack()->ReceiveSound(ScreamStrength);
	CheckDeath("killed by an Enner Beast's scream");
	if(GetWielded() && !GetWielded()->GetExists())
		SetWielded(0);
}

void character::Hunger(ushort Turns) 
{
	ulong BNP = GetNP();

	switch(GetBurdenState())
	{
	case UNBURDENED:
		SetNP(GetNP() - Turns);
		break;
	case BURDENED:
		SetNP(GetNP() - 2 * Turns);
		SetStrengthExperience(GetStrengthExperience() + 25 * Turns);
		SetAgilityExperience(GetAgilityExperience() - 10 * Turns);
		break;
	case STRESSED:
	case OVERLOADED:
		SetNP(GetNP() - 4 * Turns);
		SetStrengthExperience(GetStrengthExperience() + 50 * Turns);
		SetAgilityExperience(GetAgilityExperience() - 25 * Turns);
		break;
	}

	if(GetNP() < HUNGERLEVEL)
		SetStrengthExperience(GetStrengthExperience() - 10 * Turns);

	if(GetNP() < HUNGERLEVEL && BNP >= HUNGERLEVEL) if(GetIsPlayer()) ADD_MESSAGE("You are getting hungry.");
	if(GetNP() < CRITICALHUNGERLEVEL && BNP >= CRITICALHUNGERLEVEL) if(GetIsPlayer()) ADD_MESSAGE("You are getting very hungry.");
	if(GetNP() < 1)
	{
		if(!game::GetWizardMode())
			AddScoreEntry("starved to death");

		Die();
	}
}

bool character::Hit(character* Enemy)
{
	if(GetTeam()->GetRelation(Enemy->GetTeam()) != HOSTILE)
		if(GetIsPlayer() && !game::BoolQuestion("This might cause a hostile reaction. Are you sure? [Y/N]"))
			return false;

	GetTeam()->Hostility(Enemy->GetTeam());

	if(GetTeam() == Enemy->GetTeam())
		Enemy->SetTeam(game::GetTeam(1));

	short Success = rand() % 26 - rand() % 26;

	switch(Enemy->TakeHit(GetSpeed(), Success, GetAttackStrength(), this)) //there's no breaks and there shouldn't be any
	{
	case HAS_HIT:
	case HAS_BLOCKED:
		if(GetWielded())
			GetWielded()->ReceiveHitEffect(Enemy, this);
	case HAS_DIED:
		SetStrengthExperience(GetStrengthExperience() + 50);
	case HAS_DODGED:
		SetAgilityExperience(GetAgilityExperience() + 25);
	}

	SetNP(GetNP() - 4);

	return true;
}

ushort character::CalculateArmorModifier() const
{
	return 100;
}

uchar character::TakeHit(ushort Speed, short Success, float WeaponStrength, character* Enemy)
{
	DeActivateVoluntaryStates(Enemy->Name(DEFINITE) + " seems to be hostile");

	if(!(rand() % 20))
	{
		ushort Damage = ushort(WeaponStrength * Enemy->GetStrength() * (1 + float(Success) / 100) * CalculateArmorModifier() / 1000000) + (rand() % 5 ? 2 : 1);

		Enemy->AddHitMessage(this,true);

		if(game::GetWizardMode() && GetLevelSquareUnder()->CanBeSeen())
			ADD_MESSAGE("(damage: %d)", Damage);

		SetHP(GetHP() - Damage);

		if(rand() % 4) SpillBlood(rand() % 3);

		if(CheckDeath(std::string("killed by ") + Enemy->Name(INDEFINITE)))
			return HAS_DIED;

		return HAS_HIT;
	}

	if((Success + Speed + GetSize() - GetAgility()) > 0 && rand() % (Success + Speed + GetSize() - GetAgility()) > 50)
	{
		ushort Damage = ushort(WeaponStrength * Enemy->GetStrength() * (1 + float(Success) / 100) * CalculateArmorModifier() / 2000000) + (rand() % 5 ? 1 : 0);

		if(!Damage)
		{
			Enemy->AddBlockMessage(this);

			SetStrengthExperience(GetStrengthExperience() + 25);
			SetEnduranceExperience(GetEnduranceExperience() + 25);

			return HAS_BLOCKED;
		}
		else
		{
			Enemy->AddHitMessage(this);

			if(game::GetWizardMode() && GetLevelSquareUnder()->CanBeSeen())
				ADD_MESSAGE("(damage: %d)", Damage);

			SetHP(GetHP() - Damage);

			if(CheckDeath(std::string("killed by ") + Enemy->Name(INDEFINITE)))
				return HAS_DIED;

			return HAS_HIT;
		}
	}
	else
	{
		Enemy->AddDodgeMessage(this);

		SetAgilityExperience(GetAgilityExperience() + 100);

		return HAS_DODGED;
	}
}

void character::Be()
{
	if(game::GetPlayerBackup() != this)
	{
		for(uchar c = 0; c < STATES; ++c)
			if(StateIsActivated(c))
				(this->*StateHandler[c])();
			
		if(!game::Flag)
		{
			ApplyExperience();

			if(GetHP() < GetEndurance())
				SpillBlood(rand() % 2);

			if(GetIsPlayer() && GetNP() < CRITICALHUNGERLEVEL && !(rand() % 50) && !StateIsActivated(FAINTED))
				Faint();

			switch(GetBurdenState())
			{
			case UNBURDENED:
				SetAP(GetAP() + 100 + (GetAgility() >> 1));
			break;
			case BURDENED:
				SetAP(GetAP() + 75 + (GetAgility() >> 1) - (GetAgility() >> 2));
			break;
			case STRESSED:
			case OVERLOADED:
				SetAP(GetAP() + 50 + (GetAgility() >> 2));
			break;
			}
		}
		else
			game::Flag = false;

		if(GetAP() >= 1000)
		{
			if(GetIsPlayer())
			{
				static ushort Timer = 0;
				if(CanMove() && Timer++ == 20)
				{
					game::Save(game::GetAutoSaveFileName().c_str());
					Timer = 0;
				}
				CharacterSpeciality();
				StateAutoDeactivation();
				if(CanMove())
					GetPlayerCommand();
				Hunger();
				Regenerate();
				game::Turn();
				game::ApplyDivineTick();
			}
			else
			{
				StateAutoDeactivation();
				if(CanMove())
					GetAICommand(); //This should handle AI suicide also...
				Regenerate();
			}

			SetAP(GetAP() - 1000);
		}
	}
}


bool character::GoUp()
{
	if(GetSquareUnder()->GetOverTerrain()->GoUp(this))
	{
		SetStrengthExperience(GetStrengthExperience() + 25);
		SetNP(GetNP() - 2);
		return true;
	}
	else
		return false;
}

bool character::GoDown()
{
	if(GetSquareUnder()->GetOverTerrain()->GoDown(this))
	{
		SetAgilityExperience(GetAgilityExperience() + 25);
		SetNP(GetNP() - 1);
		return true;
	}
	else
		return false;
}

bool character::Open()
{
	ADD_MESSAGE("Where is this famous door you wish to open? Press i for inventory.");

	DRAW_MESSAGES();

	EMPTY_MESSAGES();

	graphics::BlitDBToScreen();

	while(true)
	{
		int Key = GETKEY();

		if(Key == 'i') 
			return OpenItem();

		if(Key == 0x1B)
			return false;

		for(uchar c = 0; c < DIRECTION_COMMAND_KEYS; ++c)
			if(Key == game::GetMoveCommandKey(c))
				return OpenPos(GetPos() + game::GetMoveVector(c));
	}
}

bool character::Close()
{
	ADD_MESSAGE("Where is this door you wish to close?");

	DRAW_MESSAGES();

	EMPTY_MESSAGES();

	graphics::BlitDBToScreen();

	while(true)
	{
		int Key = GETKEY();

		if(Key == 0x1B)
			return false;

		for(uchar c = 0; c < DIRECTION_COMMAND_KEYS; ++c)
			if(Key == game::GetMoveCommandKey(c))
				if(game::GetCurrentLevel()->GetLevelSquare(GetPos() + game::GetMoveVector(c))->Close(this))
				{
					SetAgilityExperience(GetAgilityExperience() + 25);
					SetNP(GetNP() - 1);
					return true;
				}
				else
					return false;
	}
}

bool character::Drop()
{
	ushort Index = GetStack()->DrawContents("What do you want to drop?");

	if(Index < GetStack()->GetItems() && GetStack()->GetItem(Index))
		if(GetStack()->GetItem(Index) == GetWielded())
			ADD_MESSAGE("You can't drop something you wield!");
		else
		{
			GetStack()->MoveItem(Index, GetLevelSquareUnder()->GetStack());

			return true;
		}

        return false;
}

bool character::Consume()
{
	if(!game::GetInWilderness() && GetLevelSquareUnder()->GetStack()->ConsumableItems(this) && game::BoolQuestion("Do you wish to consume one of the items lying on the ground? [Y/N]"))
	{
		ushort Index = GetLevelSquareUnder()->GetStack()->DrawConsumableContents("What do you wish to consume?", this);

		if(Index < GetLevelSquareUnder()->GetStack()->GetItems())
		{
			if(CheckBulimia() && !game::BoolQuestion("You think your stomach will burst if you eat anything more. Force it down? (Y/N)"))
				return false;

			if(ConsumeItem(GetLevelSquareUnder()->GetStack()->GetItem(Index), GetLevelSquareUnder()->GetStack()))
			{
				ReceiveBulimiaDamage();
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}

	if(GetStack()->ConsumableItems(this))
	{
		ushort Index = GetStack()->DrawConsumableContents("What do you wish to consume?", this);

		if(Index < GetStack()->GetItems())
		{
			if(!CheckIfConsumable(Index))
				return false;

			if(CheckBulimia() && !game::BoolQuestion("You think your stomach will burst if you eat anything more. Force it down? (Y/N)"))
				return false;

			if(ConsumeItem(GetStack()->GetItem(Index), GetStack()))
			{
				ReceiveBulimiaDamage();
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}

	if(GetIsPlayer())
		ADD_MESSAGE("You have nothing to eat.");

	return false;
}

bool character::CheckBulimia() const
{
	return GetNP() > (GetSize() << 5) ? true : false;
}

void character::ReceiveBulimiaDamage()
{
	if((GetNP() - (GetSize() << 5)) / 50 > 0)
	{
		ADD_MESSAGE("Urgh... Your stomach hurts.");

		SetHP(GetHP() - (GetNP() - (GetSize() << 5)) / 50);

		CheckDeath("died of bulimia");
	}
}

bool character::CheckIfConsumable(ushort Index) const
{
	return (GetTorsoArmor() != GetStack()->GetItem(Index) && GetWielded() != GetStack()->GetItem(Index));
}

bool character::ConsumeItem(item* ToBeEaten, stack* ItemsStack)
{
	if(ConsumeItemType(ToBeEaten->GetConsumeType()))
	{
		if(ToBeEaten = ToBeEaten->PrepareForConsuming(this, ItemsStack))
		{
			SetConsumingCurrently(ToBeEaten);
			ActivateState(EATING);
			StateCounter[EATING] = 500;
			return true;
		}
	}
	else
		if(GetIsPlayer())
			ADD_MESSAGE("You can't consume this.");

	return false;
}

bool character::ConsumeItemType(uchar Type) const
{
	switch(Type)
	{
	case LIQUID:
		return true;
	break;
		
	case ODD:
		return false;
	break;
	case FRUIT:
		return true;
	break;
	case MEAT:
		return true;
	break;
	case SPOILED:
		return true;
	break;
	case HARD:
		return false;
	break;
	case SCHOOLFOOD:
		return true;
	break;
	case CONTAINER:
		return false;
	break;
	case BONE:
		return false;
	break;

	default:
		ADD_MESSAGE("ERRRRORRRRRR in Consumeitemtype."); //All hail SpykoS! He is the author of this file, and his might is over that of PMGR!!!
	}
		
	return false;
}

void character::Move(vector2d MoveTo, bool TeleportMove)
{
	if(GetBurdenState() || TeleportMove)
	{
		game::GetCurrentArea()->RemoveCharacter(GetPos());

		game::GetCurrentArea()->AddCharacter(MoveTo, this);

		if(GetIsPlayer())
		{
			if(GetPos().X < game::GetCamera().X + 2 || GetPos().X > game::GetCamera().X + 48)
				game::UpdateCameraX();

			if(GetPos().Y < game::GetCamera().Y + 2 || GetPos().Y > game::GetCamera().Y + 27)
				game::UpdateCameraY();

			game::GetCurrentArea()->UpdateLOS();

			if(!game::GetInWilderness())
			{
				if(GetLevelSquareUnder()->GetLuminance() < 64 && !game::GetSeeWholeMapCheat())
					ADD_MESSAGE("It's dark in here!");

				if(GetLevelSquareUnder()->GetStack()->GetItems() > 0)
				{
					if (GetLevelSquareUnder()->GetStack()->GetItems() > 1)
						ADD_MESSAGE("Several items are lying here.");
					else
						ADD_MESSAGE("%s is lying here.", GetLevelSquareUnder()->GetStack()->GetItem(0)->CNAME(INDEFINITE));
				}

				if(game::GetCurrentLevel()->GetLevelSquare(GetPos())->GetEngraved() != "")
					ADD_MESSAGE("Something has been engraved here: \"%s\"", game::GetCurrentLevel()->GetLevelSquare(GetPos())->GetEngraved().c_str());
			}
		}

		SetNP(GetNP() - 1);
		SetAgilityExperience(GetAgilityExperience() + 10);
	}
}

void character::DrawToTileBuffer() const
{
	Picture->MaskedBlit(igraph::GetTileBuffer(), 0, 0, 0, 0, 16, 16);
}

bool character::Wield()
{
	ushort Index;
	if(!CanWield())
	{
		ADD_MESSAGE("This monster can not wield anything.");
		return false;
	}
	if((Index = GetStack()->DrawContents("What do you want to wield? or press '-' for hands")) == 0xFFFF)
	{
		ADD_MESSAGE("You have nothing to wield.");
		return false;
	}
	if(Index == 0xFFFE)
		SetWielded(0);
	else
		if(Index < GetStack()->GetItems())
			SetWielded(GetStack()->GetItem(Index));
		else
			return false;

	return true;
}

void character::GetAICommand() // Freedom is slavery. Love is hate. War is peace.
			       // Shouldn't it be "Ignorance is strength", not "Love is hate"?
{
	SeekLeader();

	if(CheckForEnemies())
		return;

	if(CheckForDoors())
		return;

	if(CheckForUsefulItemsOnGround())
		return;

	if(FollowLeader())
		return;

	MoveRandomly();
}

void character::MoveTowards(vector2d TPos)
{
	vector2d MoveTo[3];

	if(TPos.X < GetPos().X)
	{
		if(TPos.Y < GetPos().Y)
		{
			MoveTo[0] = vector2d(-1, -1);
			MoveTo[1] = vector2d(-1,  0);
			MoveTo[2] = vector2d( 0, -1);
		}

		if(TPos.Y == GetPos().Y)
		{
			MoveTo[0] = vector2d(-1,  0);
			MoveTo[1] = vector2d(-1, -1);
			MoveTo[2] = vector2d(-1,  1);
		}

		if(TPos.Y > GetPos().Y)
		{
			MoveTo[0] = vector2d(-1, 1);
			MoveTo[1] = vector2d(-1, 0);
			MoveTo[2] = vector2d( 0, 1);
		}
	}

	if(TPos.X == GetPos().X)
	{
		if(TPos.Y < GetPos().Y)
		{
			MoveTo[0] = vector2d( 0, -1);
			MoveTo[1] = vector2d(-1, -1);
			MoveTo[2] = vector2d( 1, -1);
		}

		if(TPos.Y == GetPos().Y)	//???
			return;

		if(TPos.Y > GetPos().Y)
		{
			MoveTo[0] = vector2d( 0, 1);
			MoveTo[1] = vector2d(-1, 1);
			MoveTo[2] = vector2d( 1, 1);
		}
	}

	if(TPos.X > GetPos().X)
	{
		if(TPos.Y < GetPos().Y)
		{
			MoveTo[0] = vector2d(1, -1);
			MoveTo[1] = vector2d(1,  0);
			MoveTo[2] = vector2d(0, -1);
		}

		if(TPos.Y == GetPos().Y)
		{
			MoveTo[0] = vector2d(1,  0);
			MoveTo[1] = vector2d(1, -1);
			MoveTo[2] = vector2d(1,  1);
		}

		if(TPos.Y > GetPos().Y)
		{
			MoveTo[0] = vector2d(1, 1);
			MoveTo[1] = vector2d(1, 0);
			MoveTo[2] = vector2d(0, 1);
		}
	}

	if(TryMove(GetPos() + MoveTo[0])) return;

	if(rand() % 2)
	{
		if(TryMove(GetPos() + MoveTo[1])) return;
		if(TryMove(GetPos() + MoveTo[2])) return;
	}
	else
	{
		if(TryMove(GetPos() + MoveTo[2])) return;
		if(TryMove(GetPos() + MoveTo[1])) return;
	}
}

bool character::TryMove(vector2d MoveTo)
{
	if(!game::GetInWilderness())
		if(MoveTo.X < game::GetCurrentLevel()->GetXSize() && MoveTo.Y < game::GetCurrentLevel()->GetYSize())
			if(game::GetCurrentLevel()->GetLevelSquare(MoveTo)->GetCharacter())
				if(GetIsPlayer() || GetTeam()->GetRelation(game::GetCurrentLevel()->GetLevelSquare(MoveTo)->GetCharacter()->GetTeam()) == HOSTILE)
					return Hit(game::GetCurrentLevel()->GetLevelSquare(MoveTo)->GetCharacter());
				else
					return false;
			else
				if(game::GetCurrentLevel()->GetLevelSquare(MoveTo)->GetOverLevelTerrain()->GetIsWalkable() || (game::GetGoThroughWallsCheat() && GetIsPlayer()))
				{
					Move(MoveTo);
					return true;
				}
				else
					if(GetIsPlayer() && game::GetCurrentLevel()->GetLevelSquare(MoveTo)->GetOverLevelTerrain()->CanBeOpened())
						if(game::BoolQuestion("Do you want to open this door? [Y/N]", false, game::GetMoveCommandKey(game::GetPlayer()->GetPos(), MoveTo)))
						{
							OpenPos(MoveTo);
							return true;
						}
						else
							return false;
					else
						return false;
		else
		{
			if(GetIsPlayer() && game::GetCurrentLevel()->GetOnGround())
			{
				game::GetCurrentArea()->RemoveCharacter(GetPos());
				game::GetCurrentDungeon()->SaveLevel();
				game::LoadWorldMap();
				game::SetInWilderness(true);
				game::GetCurrentArea()->AddCharacter(game::GetCurrentDungeon()->GetWorldMapPos(), this);
				game::GetCurrentArea()->SendNewDrawRequest();
				game::UpdateCamera();
				return true;
			}

			return false;
		}
	else
		if(MoveTo.X < game::GetWorldMap()->GetXSize() && MoveTo.Y < game::GetWorldMap()->GetYSize())
			if(game::GetCurrentArea()->GetSquare(MoveTo)->GetGroundTerrain()->GetIsWalkable() || (game::GetGoThroughWallsCheat() && GetIsPlayer()))
			{
				Move(MoveTo);
				return true;
			}
			else
				return false;
		else
			return false;
	
}

bool character::ShowInventory()
{
	GetStack()->DrawContents("Character's Inventory, press ESC to exit");

	return false;
}

bool character::PickUp()
{
	bool ToBeReturned = false;

	if (GetLevelSquareUnder()->GetStack()->GetItems() > 0)
	{
		if (GetLevelSquareUnder()->GetStack()->GetItems() > 1)
		{
			ushort Index;

			bitmap Backup(800, 600);

			DOUBLEBUFFER->Blit(&Backup, 0, 0, 0, 0, 800, 600);

			for(;;)
			{
				Index = GetLevelSquareUnder()->GetStack()->DrawContents("What do you want to pick up?");

				if(Index < GetLevelSquareUnder()->GetStack()->GetItems())
					if(GetLevelSquareUnder()->GetStack()->GetItem(Index))
					{
						ADD_MESSAGE("%s picked up.", GetLevelSquareUnder()->GetStack()->GetItem(Index)->CNAME(INDEFINITE));
						GetLevelSquareUnder()->GetStack()->MoveItem(Index, GetStack())->CheckPickUpEffect(this);
						ToBeReturned = true;
					}

				if(Index == 0xFFFD || !GetLevelSquareUnder()->GetStack()->GetItems())
					break;

				Backup.Blit(DOUBLEBUFFER, 0, 0, 0, 0, 800, 600);
			}
		}
		else
		{
			ADD_MESSAGE("%s picked up.", GetLevelSquareUnder()->GetStack()->GetItem(0)->CNAME(INDEFINITE));
			GetLevelSquareUnder()->GetStack()->MoveItem(0, GetStack())->CheckPickUpEffect(this);
			return true;
		}
	}
	else
		ADD_MESSAGE("There is nothing here to pick up, %s!", game::Insult());

	return ToBeReturned;
}

bool character::Quit()
{
	if(game::BoolQuestion("Thine Holy Quest is not yet compeleted! Really quit? [Y/N]"))
	{
		game::Quit();

		game::RemoveSaves();

		if(!game::GetWizardMode())
		{
			AddScoreEntry("cowardly quit the game", 0.75f);
			highscore HScore;
			HScore.Draw();
		}

		return true;
	}
	else
		return false;
}

void character::CreateCorpse()
{
	GetLevelSquareUnder()->GetStack()->AddItem(new corpse(GetMaterial(0)));

	SetMaterial(0, 0);
}

void character::Die()
{
	// Not for programmers: This function MUST NOT delete any objects!

	if(!Exists)
		return;

	if(GetIsPlayer())
	{
		ADD_MESSAGE("You die.");

		if(game::GetWizardMode())
		{
			game::DrawEverything(false);

			if(!game::BoolQuestion("Do you want to do this, cheater? [Y/N]", 2, 'y'))
			{
				SetHP(Endurance << 1);
				SetNP(1000);
				return;
			}
		}
	}
	else
		if(GetLevelSquareUnder()->CanBeSeen())
			ADD_MESSAGE(DeathMessage().c_str());

	if(!game::GetInWilderness())
		while(GetStack()->GetItems())
			GetStack()->MoveItem(0, GetLevelSquareUnder()->GetStack());
	else
		while(GetStack()->GetItems())
		{
			GetStack()->GetItem(0)->SetExists(false);
			GetStack()->RemoveItem(0);
		}
	
	GetSquareUnder()->RemoveCharacter();

	if(!game::GetInWilderness())
		CreateCorpse();

	SetExists(false);

	if(GetIsPlayer())
	{
		game::Quit();

		if(!game::GetWizardMode())
		{
			game::DrawEverything();

			GETKEY();
		}

		iosystem::TextScreen("Unfortunately thee died during thine journey. The �berpriest is not happy.");

		game::RemoveSaves();

		highscore HScore;
		HScore.Draw();
	}
}

bool character::OpenItem()
{
	ushort Index = Stack->DrawContents("What do you want to open?");

	if(Index < GetStack()->GetItems())
		if(GetStack()->GetItem(Index)->TryToOpen(Stack))
		{
			SetAgilityExperience(GetAgilityExperience() + 25);
			SetNP(GetNP() - 1);
			return true;
		}
		else
		{
			ADD_MESSAGE("You can't open %s.", GetStack()->GetItem(Index)->CNAME(DEFINITE));
			return false;
		}

	return false;
}

void character::Regenerate(ushort Turns)
{
	SetRegenerationCounter(CRegenerationCounter() + GetEndurance() * Turns);

	while(CRegenerationCounter() > 100)
	{
		if(GetHP() < (GetEndurance() << 1))
		{
			SetHP(GetHP() + 1);
			SetEnduranceExperience(GetEnduranceExperience() + 100);
		}

		SetRegenerationCounter(CRegenerationCounter() - 100);
	}
}

bool character::WearArmor()
{
	ADD_MESSAGE("This monster type can't use armor.");

	return false;
}

void character::AddBlockMessage(character* Enemy) const
{
	std::string ThisDescription = GetLevelSquareUnder()->CanBeSeen() ? CNAME(DEFINITE) : "something";
	std::string EnemyDescription = Enemy->GetLevelSquareUnder()->CanBeSeen() ? Enemy->CNAME(DEFINITE) : "something";

	if(Enemy->GetIsPlayer())
		ADD_MESSAGE("You block %s!", ThisDescription.c_str());
	else
		if(GetIsPlayer())
			ADD_MESSAGE("%s blocks you!", EnemyDescription.c_str());
		else
			if(GetLevelSquareUnder()->CanBeSeen() || Enemy->GetLevelSquareUnder()->CanBeSeen())
				ADD_MESSAGE("%s blocks %s!", EnemyDescription.c_str(), ThisDescription.c_str());
}

void character::AddDodgeMessage(character* Enemy) const
{
	std::string ThisDescription = GetLevelSquareUnder()->CanBeSeen() ? CNAME(DEFINITE) : "something";
	std::string EnemyDescription = Enemy->GetLevelSquareUnder()->CanBeSeen() ? Enemy->CNAME(DEFINITE) : "something";

	if(Enemy->GetIsPlayer())
		ADD_MESSAGE("You dodge %s!", ThisDescription.c_str());
	else
		if(GetIsPlayer())
			ADD_MESSAGE("%s dodges you!", EnemyDescription.c_str());
		else
			if(GetLevelSquareUnder()->CanBeSeen() || Enemy->GetLevelSquareUnder()->CanBeSeen())
				ADD_MESSAGE("%s dodges %s!", EnemyDescription.c_str(), ThisDescription.c_str());
}

void character::AddHitMessage(character* Enemy, const bool Critical) const
{
	std::string ThisDescription = GetLevelSquareUnder()->CanBeSeen() ? CNAME(DEFINITE) : "something";
	std::string EnemyDescription = Enemy->GetLevelSquareUnder()->CanBeSeen() ? Enemy->CNAME(DEFINITE) : "something";

	if(Enemy->GetIsPlayer())
		if(GetWielded() && GetLevelSquareUnder()->CanBeSeen())
			ADD_MESSAGE("%s %s you with %s %s!", CNAME(DEFINITE), ThirdPersonWeaponHitVerb(Critical).c_str(), game::PossessivePronoun(GetSex()), Wielded->CNAME(0));
		else
			ADD_MESSAGE("%s %s you!", ThisDescription.c_str(), ThirdPersonMeleeHitVerb(Critical).c_str());
	else
		if(GetIsPlayer())
			ADD_MESSAGE("You %s %s!", FirstPersonHitVerb(Enemy, Critical).c_str(), EnemyDescription.c_str());
		else
			if(GetLevelSquareUnder()->CanBeSeen() || Enemy->GetLevelSquareUnder()->CanBeSeen())
				ADD_MESSAGE("%s %s %s!", ThisDescription.c_str(), AICombatHitVerb(Enemy, Critical).c_str(), EnemyDescription.c_str());
}

void character::BeTalkedTo(character*)
{
	ADD_MESSAGE("%s grunts.", CNAME(DEFINITE));
}

bool character::Talk()
{
	int k;
	ADD_MESSAGE("To whom do you wish to talk to?");

	DRAW_MESSAGES();

	EMPTY_MESSAGES();

	graphics::BlitDBToScreen();

	bool CorrectKey = false;

	while(!CorrectKey)
	{
		k = GETKEY();

		if(k == 0x1B)
			CorrectKey = true;

		for(uchar c = 0; c < DIRECTION_COMMAND_KEYS; ++c)
			if(k == game::GetMoveCommandKey(c) || (k ^ 32) == game::GetMoveCommandKey(c))
			{
				if(game::GetCurrentLevel()->GetLevelSquare(GetPos() + game::GetMoveVector(c))->GetCharacter())
				{
					game::GetCurrentLevel()->GetLevelSquare(GetPos() + game::GetMoveVector(c))->GetCharacter()->BeTalkedTo(this);
					return true;
				}
				else
				{
					ADD_MESSAGE("There is no one in that square.");
					return false;
				}

				CorrectKey = true;
			}
	}

	return false;
}

bool character::NOP()
{
	SetAgilityExperience(GetAgilityExperience() - 5);

	return true;
}

void character::ApplyExperience()
{
	if(GetStrengthExperience() > pow(1.18, long(GetStrength())) * 193)
	{
		if(GetIsPlayer())
			ADD_MESSAGE("You feel you could lift Bill with one hand!");
		else
			if(GetLevelSquareUnder()->CanBeSeen())
				ADD_MESSAGE("Suddenly %s looks stronger.", CNAME(DEFINITE));

		SetStrength(GetStrength() + 1);

		SetStrengthExperience(0);
	}

	if(GetStrengthExperience() < -pow(1.18, long(100 - GetStrength())) * 193)
	{
		if(GetIsPlayer())
			ADD_MESSAGE("You collapse under your load.");
		else
			if(GetLevelSquareUnder()->CanBeSeen())
				ADD_MESSAGE("Suddenly %s looks weaker.", CNAME(DEFINITE));

		SetStrength(GetStrength() - 1);

		SetStrengthExperience(0);
	}

	if(GetEnduranceExperience() > pow(1.18, long(GetEndurance())) * 193)
	{
		if(GetIsPlayer())
			ADD_MESSAGE("You feel Valpuri's toughness around you!");
		else
			if(GetLevelSquareUnder()->CanBeSeen())
				ADD_MESSAGE("Suddenly %s looks tougher.", CNAME(DEFINITE));

		SetEndurance(GetEndurance() + 1);

		SetEnduranceExperience(0);
	}

	if(GetEnduranceExperience() < -pow(1.18, long(100 - GetEndurance())) * 193)
	{
		if(GetIsPlayer())
			ADD_MESSAGE("You seem as tough as Jari.");
		else
			if(GetLevelSquareUnder()->CanBeSeen())
				ADD_MESSAGE("Suddenly %s looks less tough.", CNAME(DEFINITE));

		SetEndurance(GetEndurance() - 1);

		SetEnduranceExperience(0);
	}

	if(GetAgilityExperience() > pow(1.18, long(GetAgility())) * 193)
	{
		if(GetIsPlayer())
			ADD_MESSAGE("Your agility challenges even the Valpuri's angels!");
		else
			if(GetLevelSquareUnder()->CanBeSeen())
				ADD_MESSAGE("Suddenly %s looks faster.", CNAME(DEFINITE));

		SetAgility(GetAgility() + 1);

		SetAgilityExperience(0);
	}

	if(GetAgilityExperience() < -pow(1.18, long(100 - GetAgility())) * 193)
	{
		if(GetIsPlayer())
			ADD_MESSAGE("You seem as fast as a flat mommo.");
		else
			if(GetLevelSquareUnder()->CanBeSeen())
				ADD_MESSAGE("Suddenly %s looks slower.", CNAME(DEFINITE));

		SetAgility(GetAgility() - 1);

		SetAgilityExperience(0);
	}

	if(GetPerceptionExperience() > pow(1.18, long(GetPerception())) * 193)
	{
		if(GetIsPlayer())
			ADD_MESSAGE("Your sight seem to sharpen. This is bad. Very bad.");

		SetPerception(GetPerception() + 1);

		SetPerceptionExperience(0);
	}

	if(GetPerceptionExperience() < -pow(1.18, long(100 - GetPerception())) * 193)
	{
		if(GetIsPlayer())
			ADD_MESSAGE("You feel very guru.");

		SetPerception(GetPerception() - 1);

		SetPerceptionExperience(0);

		game::GetGod(1)->AdjustRelation(100);
	}
}

bool character::HasHeadOfElpuri() const
{
	for(ushort c = 0; c < GetStack()->GetItems(); ++c)
		if(GetStack()->GetItem(c)->IsHeadOfElpuri())
			return true;

	return false;
}

bool character::HasPerttusNut() const
{
	for(ushort c = 0; c < GetStack()->GetItems(); ++c)
		if(GetStack()->GetItem(c)->IsPerttusNut())
			return true;

	return false;
}

bool character::HasMaakotkaShirt() const
{
	for(ushort c = 0; c < GetStack()->GetItems(); ++c)
		if(GetStack()->GetItem(c)->IsMaakotkaShirt())
			return true;

	return false;
}

bool character::Save()
{
	if(game::BoolQuestion("Dost thee truly wish to save and flee? [Y/N]"))
	{
		game::Save();

		game::Quit();

		return true;
	}
        else
		return false;
}

bool character::Read()
{
	ushort Index = GetStack()->DrawContents("What do you want to read?");

	if(Index < GetStack()->GetItems())
		return ReadItem(Index, GetStack());
	else
		return false;
}

bool character::ReadItem(int ToBeRead, stack* ItemsStack)
{
	if(ItemsStack->GetItem(ToBeRead)->CanBeRead(this))
	{
		if(ItemsStack->GetItem(ToBeRead)->Read(this))
			ItemsStack->RemoveItem(ToBeRead);

		return true;
	}
	else
	{
		if(GetIsPlayer())
			ADD_MESSAGE("You can't read this.");

		return false;
	}
}

uchar character::GetBurdenState(ulong Mass) const
{
	ulong SumOfMasses;
	if(!Mass)
		SumOfMasses = GetStack()->SumOfMasses();
	else
		SumOfMasses = Mass;
	if(SumOfMasses > ulong(7000 * GetStrength()))
		return OVERLOADED;
	if(SumOfMasses > ulong(5000 * GetStrength()))
		return STRESSED;
	if(SumOfMasses > ulong(2500 * GetStrength()))
		return BURDENED;
	return UNBURDENED;
}

bool character::Dip()
{
	ushort What = GetStack()->DrawContents("What do you want to dip?");

	if(What < GetStack()->GetItems() && GetStack()->GetItem(What)->CanBeDipped())
	{
		game::DrawEverything();
		ushort To = GetStack()->DrawContents("In what do you wish to dip it into?");
		if(To < GetStack()->GetItems() && GetStack()->GetItem(To) && GetStack()->GetItem(What) != GetStack()->GetItem(To))
		{
			if(GetStack()->GetItem(To)->CanBeDippedInto(GetStack()->GetItem(What)))
			{
				GetStack()->GetItem(What)->DipInto(GetStack()->GetItem(To));
				return true;
			}
		}
	}
	ADD_MESSAGE("Invalid selection!");
        return false;
}

void character::Save(outputfile& SaveFile) const
{
	object::Save(SaveFile);

	Stack->Save(SaveFile);

	ushort Index = Wielded ? Stack->SearchItem(Wielded) : 0xFFFF;

	SaveFile << Index << Strength << Endurance << Agility << Perception << RegenerationCounter;
	SaveFile << HP << NP << AP;
	SaveFile << StrengthExperience << EnduranceExperience << AgilityExperience << PerceptionExperience;
	SaveFile << State;

	if(StateIsActivated(EATING))
		SaveFile << GetLevelSquareUnder()->GetStack()->SearchItem(GetConsumingCurrently());
	else
		SaveFile << ushort(0);

	if(StateIsActivated(DIGGING))
	{
		SaveFile << ushort(1);
		SaveFile << GetStack()->SearchItem(GetOldWieldedItem());
		SaveFile << StateVariables.Digging.SquareBeingDiggedX << StateVariables.Digging.SquareBeingDiggedY;
	}
	else
		SaveFile << ushort(0);

	for(uchar c = 0; c < STATES; ++c)
		SaveFile << StateCounter[c];

	SaveFile << Team->GetID();

	if(StateIsActivated(EATING))
	{
		Index = GetStack()->SearchItem(GetConsumingCurrently());

		if(Index != 0xFFFF)
			SaveFile << uchar(1) << Index;
		else
		{
			if(!game::GetInWilderness())
				Index = GetLevelSquareUnder()->GetStack()->SearchItem(GetConsumingCurrently());

			if(Index == 0xFFFF)
				ABORT("Consuming unknown item!");

			SaveFile << uchar(2) << Index;
		}
	}
	else
		SaveFile << uchar(0);

	if(GetTeam()->GetLeader() == this)
		SaveFile << uchar(1);
	else
		SaveFile << uchar(0);
}

void character::Load(inputfile& SaveFile)
{
	object::Load(SaveFile);

	if(Stack)
		delete Stack;

	Stack = new stack;
	Stack->Load(SaveFile);

	ushort Index;

	SaveFile >> Index;

	Wielded = Index != 0xFFFF ? Stack->GetItem(Index) : 0;

	SaveFile >> Strength >> Endurance >> Agility >> Perception >> RegenerationCounter;
	SaveFile >> HP >> NP >> AP;
	SaveFile >> StrengthExperience >> EnduranceExperience >> AgilityExperience >> PerceptionExperience;
	SaveFile >> State;

	SaveFile >> Index;

	if(Index)
		SetConsumingCurrently(GetLevelSquareUnder()->GetStack()->GetItem(Index));

	SaveFile >> Index;

	if(Index)
	{
		SaveFile >> Index;
		SetOldWieldedItem(GetStack()->GetItem(Index));
		SaveFile >> StateVariables.Digging.SquareBeingDiggedX >> StateVariables.Digging.SquareBeingDiggedY;
	}

	for(uchar c = 0; c < STATES; ++c)
		SaveFile >> StateCounter[c];

	ushort TeamID;

	SaveFile >> TeamID;

	Team = game::GetTeam(TeamID);

	uchar Stacky;

	SaveFile >> Stacky;

	if(Stacky)
	{
		SaveFile >> Index;

		if(Stacky == 1)
			SetConsumingCurrently(GetStack()->GetItem(Index));

		if(Stacky == 2)
			SetConsumingCurrently(GetLevelSquareUnder()->GetStack()->GetItem(Index));
	}

	uchar Leader;

	SaveFile >> Leader;

	if(Leader)
		GetTeam()->SetLeader(this);
}

bool character::WizardMode()
{
	if(!game::GetWizardMode())
	{
		if(game::BoolQuestion("Do you want to cheat, cheater? This action cannot be undone. [Y/N]"))
		{
			game::EnableWizardMode();

			for(ushort x = 0; x < 5; ++x)
				GetStack()->AddItem(new scrollofwishing);
		}
	}
	else
		ADD_MESSAGE("You have already activated Wizard Mode. It cannot be toggled off.");

	return false;

}

bool character::RaiseStats()
{
	if(game::GetWizardMode())
	{
		Strength += 10;   // I won't touch these just in case we'd do something
		Endurance += 10;  // really odd with GetStrength() etc.
		Agility += 10;
		Perception += 10;
		HP = Endurance << 1;
		game::GetCurrentArea()->UpdateLOS();
	}
	else
		ADD_MESSAGE("Activate wizardmode to use this function.");

	return false;
}

bool character::LowerStats()
{
	if(game::GetWizardMode())
	{
		Strength -= 10;
		Endurance -= 10;
		Agility -= 10;
		Perception -= 10;
		HP = Endurance << 1;
		game::GetCurrentArea()->UpdateLOS();
	}
	else
		ADD_MESSAGE("Activate wizardmode to use this function.");

	return false;
}

bool character::GainAllItems()
{
	if(game::GetWizardMode())
		for(ushort c = 1; c <= protocontainer<item>::GetProtoAmount(); ++c)
			Stack->AddItem(protocontainer<item>::GetProto(c)->Clone());
	else
		ADD_MESSAGE("Activate wizardmode to use this function.");

	return false;
}

bool character::SeeWholeMap()
{
	if(game::GetWizardMode())
	{
		game::SeeWholeMap();
	}
	else
		ADD_MESSAGE("Activate wizardmode to use this function.");

	return false;
}

bool character::IncreaseSoftGamma()
{
	game::EditSoftGamma(0.05f);

	game::GetCurrentArea()->SendNewDrawRequest();

	return false;
}

bool character::DecreaseSoftGamma()
{
	game::EditSoftGamma(-0.05f);

	game::GetCurrentArea()->SendNewDrawRequest();

	return false;
}

ushort character::GetEmitation() const
{
	ushort Emitation = 0;

	for(ushort c = 0; c < GetMaterials(); ++c)
		if(Material[c])
			if(Material[c]->GetEmitation() > Emitation)
				Emitation = Material[c]->GetEmitation();

	if(GetStack()->GetEmitation() > Emitation)
		Emitation = GetStack()->GetEmitation();

	return Emitation;
}

void character::SetSquareUnder(square* Square)
{
	typeable::SetSquareUnder(Square);
	Stack->SetSquareUnder(Square);
}

bool character::WalkThroughWalls()
{
	if(game::GetWizardMode())
		game::GoThroughWalls();
	else
		ADD_MESSAGE("Activate wizardmode to use this function.");

	return false;
}

bool character::ShowKeyLayout()
{
	felist List("Keyboard Layout");

	List.AddDescription("");
	List.AddDescription("Key       Description");

	for(uchar c = 1; game::GetCommand(c); ++c)
	{
		std::string Buffer;
		Buffer += game::GetCommand(c)->GetKey();
		Buffer.resize(10, ' ');
		List.AddEntry(Buffer + game::GetCommand(c)->GetDescription(), RED);
	}

	List.Draw();

	return false;
}

bool character::Look()
{
	int Key;
	std::string OldMemory;
	vector2d CursorPos = GetPos();
	game::DrawEverythingNoBlit();
	FONT->Printf(DOUBLEBUFFER, 16, 514, WHITE, "Press direction keys to move cursor or esc to exit from the mode.");
	DRAW_MESSAGES();
	EMPTY_MESSAGES();
	graphics::BlitDBToScreen();

	while(Key != ' ' && Key != 0x1B)
	{
		for(uchar c = 0; c < DIRECTION_COMMAND_KEYS; ++c)
			if(Key == game::GetMoveCommandKey(c))
			{
				DOUBLEBUFFER->ClearToColor((CursorPos.X - game::GetCamera().X) << 4, (CursorPos.Y - game::GetCamera().Y + 2) << 4, 16, 16, 0);
				CursorPos += game::GetMoveVector(c);

				if (short(CursorPos.X) > game::GetCurrentArea()->GetXSize()-1)	CursorPos.X = 0;
				if (short(CursorPos.X) < 0)					CursorPos.X = game::GetCurrentArea()->GetXSize()-1;
				if (short(CursorPos.Y) > game::GetCurrentArea()->GetYSize()-1)	CursorPos.Y = 0;
				if (short(CursorPos.Y) < 0)					CursorPos.Y = game::GetCurrentArea()->GetYSize()-1;
			}

			if(GetIsPlayer())
			{
				if(CursorPos.X < game::GetCamera().X + 2 || CursorPos.X > game::GetCamera().X + 48)
					game::UpdateCameraXWithPos(CursorPos.X);

				if(CursorPos.Y < game::GetCamera().Y + 2 || CursorPos.Y > game::GetCamera().Y + 27)
					game::UpdateCameraYWithPos(CursorPos.Y);

			}

		if(game::GetSeeWholeMapCheat())
		{
			OldMemory = game::GetCurrentArea()->GetSquare(CursorPos)->GetMemorizedDescription();
			game::GetCurrentArea()->GetSquare(CursorPos)->UpdateMemorizedDescription();
		}

		if(game::GetCurrentArea()->GetSquare(CursorPos)->GetKnown() || game::GetSeeWholeMapCheat())
		{
			std::string Msg;

			if(game::GetCurrentArea()->GetSquare(CursorPos)->CanBeSeen() || game::GetSeeWholeMapCheat())
				Msg = "You see here ";
			else
				Msg = "You remember here ";

			Msg += game::GetCurrentArea()->GetSquare(CursorPos)->GetMemorizedDescription();

			ADD_MESSAGE("%s.", Msg.c_str());

			if(game::GetCurrentArea()->GetSquare(CursorPos)->GetCharacter() && ((game::GetCurrentArea()->GetSquare(CursorPos)->CanBeSeen()) || game::GetSeeWholeMapCheat()) && (game::GetInWilderness() || game::GetCurrentLevel()->GetLevelSquare(CursorPos)->GetLuminance() > 63))
				ADD_MESSAGE("%s is standing here.", game::GetCurrentArea()->GetSquare(CursorPos)->GetCharacter()->CNAME(INDEFINITE));
		}
		else
			ADD_MESSAGE("You have no idea what might lie here.");

		if(game::GetSeeWholeMapCheat())
		{
			game::GetCurrentArea()->GetSquare(CursorPos)->SetMemorizedDescription(OldMemory);
			game::GetCurrentArea()->GetSquare(CursorPos)->SetDescriptionChanged(true);
		}
		if(game::GetWizardMode())
			ADD_MESSAGE("(%d, %d)", CursorPos.X, CursorPos.Y);

		game::DrawEverythingNoBlit();
		igraph::DrawCursor((CursorPos - game::GetCamera() + vector2d(0,2)) << 4);
		game::GetCurrentArea()->GetSquare(CursorPos)->SendNewDrawRequest();
		FONT->Printf(DOUBLEBUFFER, 16, 514, WHITE, "Press direction keys to move cursor or esc to exit from the mode.");
		graphics::BlitDBToScreen();
		EMPTY_MESSAGES();

		Key = GETKEY();
	}


	DOUBLEBUFFER->ClearToColor((CursorPos.X - game::GetCamera().X) << 4, (CursorPos.Y - game::GetCamera().Y + 2) << 4, 16, 16, 0);
	return false;
}

float character::GetDifficulty() const
{
	return float(GetStrength()) * GetEndurance() * GetAgility() * GetAttackStrength() / (float(CalculateArmorModifier()) * 25000);
}

float character::GetAttackStrength() const
{
	return GetWielded() ? GetWielded()->GetWeaponStrength() : GetMeleeStrength();
}

bool character::Engrave(std::string What)
{
	game::GetCurrentLevel()->GetLevelSquare(GetPos())->Engrave(What);
	return true;
}

bool character::WhatToEngrave()
{
	game::GetCurrentLevel()->GetLevelSquare(GetPos())->Engrave(game::StringQuestion("What do you want to engrave here?", vector2d(7,7), WHITE, 0, 50));
	return false;
}

void character::MoveRandomly()
{
	bool OK = false;

	for(uchar c = 0; c < 10 && !OK; ++c)
	{
		ushort ToTry = rand() % 8;

		if(game::GetCurrentLevel()->IsValid(GetPos() + game::GetMoveVector(ToTry)) && !game::GetCurrentLevel()->GetLevelSquare(GetPos() + game::GetMoveVector(ToTry))->GetCharacter());
			OK = TryMove(GetPos() + game::GetMoveVector(ToTry));
	}
}

bool character::TestForPickup(item* ToBeTested) const
{
	if(GetBurdenState(ToBeTested->GetWeight() + GetStack()->SumOfMasses()) != UNBURDENED)
		return false;
	return true;
}

bool character::OpenPos(vector2d APos)
{
	if(game::GetCurrentLevel()->GetLevelSquare(APos)->Open(this))
	{
		SetAgilityExperience(GetAgilityExperience() + 25);
		SetNP(GetNP() - 1);
		return true;
	}
	return false;
}

bool character::Pray()
{
	felist Panthenon("To Whom shall thee address thine prayers?", WHITE, 0, true);

	if(!GetLevelSquareUnder()->GetDivineOwner())
		for(ushort c = 1; game::GetGod(c); ++c)
			Panthenon.AddEntry(game::GetGod(c)->CompleteDescription(), RED);
	else
		Panthenon.AddEntry(game::GetGod(GetLevelSquareUnder()->GetDivineOwner())->CompleteDescription(), RED);

	ushort Select = Panthenon.Draw();

	if(Select & 0x8000)
		return false;
	else
	{
		if(GetLevelSquareUnder()->GetDivineOwner())
		{
			if(!Select)
			{
				if(game::BoolQuestion("Do you really wish to pray? [Y/N]"))
					game::GetGod(GetLevelSquareUnder()->GetDivineOwner())->Pray();
				else
					return false;
			}
			else
				return false;
		}
		else
		{
			if(game::BoolQuestion(std::string("Do you really wish to pray to ") + game::GetGod(Select+1)->Name().c_str() + "? [Y/N]"))
				game::GetGod(Select+1)->Pray();
			else
				return false;
		}

		return true;
	}
}

void character::SpillBlood(uchar HowMuch)
{
	if(!game::GetInWilderness()) GetLevelSquareUnder()->SpillFluid(HowMuch, GetBloodColor(),5,40);
}

void character::ReceiveSchoolFoodEffect(long SizeOfEffect)
{
	if(GetIsPlayer())
		ADD_MESSAGE("Yuck! This stuff feels like vomit and old mousepads.");

	SetHP(GetHP() - SizeOfEffect / 100);
	Vomit(SizeOfEffect / 250);

	if(CheckDeath("was poisoned by school food"))
		return;

	if(!(rand() % 5) && SizeOfEffect / 500)
	{
		if(GetIsPlayer())
			ADD_MESSAGE("You gain a little bit of toughness for surviving this stuff.");
		else
			if(GetLevelSquareUnder()->CanBeSeen())
				ADD_MESSAGE("Suddenly %s looks tougher.", CNAME(DEFINITE));

		SetEndurance(GetEndurance() + SizeOfEffect / 500);
	}
}

void character::ReceiveNutrition(long SizeOfEffect)
{
	SetNP(GetNP() + SizeOfEffect);
}

void character::ReceiveOmleUrineEffect(long SizeOfEffect)
{
	if(GetIsPlayer())
		ADD_MESSAGE("You feel a primitive Force coursing through your veins.");
	else
		if(GetLevelSquareUnder()->CanBeSeen())
			ADD_MESSAGE("%s looks more powerful.", CNAME(DEFINITE));

	SetStrength(GetStrength() + 1 + rand() % 2);
	SetHP(GetHP() + 2);
}

void character::ReceivePepsiEffect(long SizeOfEffect)
{
	if(GetIsPlayer())
		ADD_MESSAGE("Urgh. You feel your guruism fading away.");

	SetHP(GetHP() - short(sqrt(SizeOfEffect / 20)));

	if(short(GetPerception() - short(sqrt(SizeOfEffect / 20))) > 0)
		SetPerception(GetPerception() - short(sqrt(SizeOfEffect / 20)));
	else
		SetPerception(1);

	if(CheckDeath("was poisoned by pepsi"))
		return;

	if(GetIsPlayer())
		game::DoEvilDeed(SizeOfEffect / 10);
}

void character::Darkness(long SizeOfEffect)
{
	ushort x = 30 + rand() % SizeOfEffect;

	if(GetIsPlayer())
		ADD_MESSAGE("Arg. You feel the fate of a navastater placed upon you...");
	else
		if(GetLevelSquareUnder()->CanBeSeen())
			ADD_MESSAGE("Suddenly %s looks like a navastater.", CNAME(DEFINITE));

	if(GetStrength() - x / 30 > 1) SetStrength(GetStrength() - x / 30); // Old comment was about eating... This
	else SetStrength(1);                                        // can happen with drinkin, hitting etc.
	if(GetEndurance() - x / 30 > 1) SetEndurance(GetEndurance() - x / 30);
	else SetEndurance(1);
	if(GetAgility() - x / 30 > 1) SetAgility(GetAgility() - x / 30);
	else SetAgility(1);
	if(GetHP() - x / 10 > 1) SetHP(GetHP() - x / 10);
	else SetHP(1);
	if(GetIsPlayer())
	{
		game::DoEvilDeed(short(SizeOfEffect / 2));

		if(game::GetWizardMode())
			ADD_MESSAGE("Change in relation: %d.", short(SizeOfEffect / 2));
	}
}

bool character::Kick()
{
	uchar Direction;
	if(!CanKick())
	{
		ADD_MESSAGE("This monster type can not kick.");
		return false;
	}
	ADD_MESSAGE("In what direction do you wish to kick?");
	game::DrawEverything();
	vector2d KickPos = game::AskForDirectionVector();
	if(KickPos != vector2d(0,0))
	{
		for(uchar c = 0; c < 8; ++c)
		{
			if(KickPos == game::GetMoveVector(c))
				Direction = c;
		}
		
		return game::GetCurrentLevel()->GetLevelSquare(GetPos() + KickPos)->Kick(GetStrength(), Direction,this);
		
	}

	return false;
}

bool character::ScreenShot()
{
	bitmap TempDB(XRES, YRES);

	DOUBLEBUFFER->Blit(&TempDB, 0, 0, 0, 0, XRES, YRES);

	TempDB.Save("Scrshot.bmp");

	return false;
}

bool character::Offer()
{
	if(GetLevelSquareUnder()->GetOverLevelTerrain()->CanBeOffered())
	{
		ushort Index = GetStack()->DrawContents("What do you want to offer?");
		if(Index < GetStack()->GetItems())
		{
			if(GetStack()->GetItem(Index) == GetWielded())
			{
				ADD_MESSAGE("You can't offer something that you wield.");
				return false;
			}
			if(GetStack()->GetItem(Index) == GetTorsoArmor())
			{
				ADD_MESSAGE("You can't offer something that you wear.");
				return false;
			}

			if(game::GetGod(GetLevelSquareUnder()->GetOverLevelTerrain()->GetOwnerGod())->ReceiveOffer(GetStack()->GetItem(Index)))
			{
				item* Temp = GetStack()->GetItem(Index);
				GetStack()->RemoveItem(Index);
				delete Temp;
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}
	else
		ADD_MESSAGE("There isn't any altar here!");

	return false;
}

long character::Score() const
{
	long Score = GetHP() * 5 + GetNP() / 10 + GetEndurance() * 30 + (GetStrength() + GetAgility() + GetPerception()) * 40;

	Score += Stack->Score();

	Score += game::GodScore();

	return Score;
}

void character::AddScoreEntry(std::string Description, float Multiplier) const
{
	highscore HScore;

	HScore.Add(long((Score() - game::GetBaseScore()) * Multiplier), game::GetPlayerName() + ", " + Description);

	HScore.Save();
}

bool character::CheckDeath(std::string Msg)
{
	if(GetHP() < 1)
	{
		if(GetIsPlayer() && !game::GetWizardMode())
			AddScoreEntry(Msg);

		Die();

		return true;
	}
	else
		return false;
}

bool character::DrawMessageHistory()
{
	globalmessagingsystem::DrawMessageHistory();

	return false;
}

bool character::Throw()
{
	ushort Index;
	if((Index = GetStack()->DrawContents("What do you want to throw?")) == 0xFFFF)
	{
		ADD_MESSAGE("You have nothing to throw.");
		return false;
	}

	if(Index < GetStack()->GetItems())
	{
		if(GetStack()->GetItem(Index) == GetWielded())
		{
			ADD_MESSAGE("You can't throw something that you wield.");
			return false;
		}
		uchar Answer = game::DirectionQuestion("In what direction do you wish to throw?", 8, false);
		if(Answer == 0xFF)
			return false;
		ThrowItem(Answer, GetStack()->GetItem(Index));

	}
	else
		return false;

	return true;
}

bool character::ThrowItem(uchar Direction, item* ToBeThrown)
{
	if(Direction > 7)
		ABORT("Throw in TOO odd direction...");

	return ToBeThrown->Fly(Direction, GetStrength(), GetStack(), GetIsPlayer());
}

void character::HasBeenHitByItem(item* Thingy, float Speed)
{
	ushort Damage = ushort(Thingy->GetWeaponStrength() * Thingy->GetWeight() * CalculateArmorModifier() * sqrt(Speed) / 2000000000) + (rand() % 5 ? 1 : 0);

	SetHP(GetHP() - Damage);

	if(GetLevelSquareUnder()->CanBeSeen())
	{
		if(GetIsPlayer())
			ADD_MESSAGE("%s hits you.", Thingy->CNAME(DEFINITE));
		else
			ADD_MESSAGE("%s hits %s.", Thingy->CNAME(DEFINITE), CNAME(DEFINITE));

		if(game::GetWizardMode())
			ADD_MESSAGE("(damage: %d) (speed: %f)", Damage, Speed);
	}

	SpillBlood(1 + rand() % 1);
	CheckDeath(std::string("died by thrown ") + Thingy->CNAME(INDEFINITE) );
}

bool character::DodgesFlyingItem(item*, float Speed)
{			// Formula requires a little bit of tweaking...
	if(!(rand() % 20))
		return rand() % 2 ? true : false;

	if(rand() % ulong(sqrt(Speed) * GetSize() / GetAgility() * 10 + 1) > 40)
		return false;
	else
		return true;
}

void character::ReceiveFireDamage(long SizeOfEffect)
{
	if(GetIsPlayer())
		ADD_MESSAGE("You burn.");
	else
		if(GetLevelSquareUnder()->CanBeSeen())
			ADD_MESSAGE("%s is hurt by the fire.", CNAME(DEFINITE));

	SetHP(GetHP() - (rand() % SizeOfEffect + SizeOfEffect));
}

void character::GetPlayerCommand()
{
	bool HasActed = false;

	while(!HasActed)
	{
		game::DrawEverything();

		int Key = GETKEY();

		bool ValidKeyPressed = false;

		{
		for(uchar c = 0; c < DIRECTION_COMMAND_KEYS; ++c)
			if(Key == game::GetMoveCommandKey(c))
			{
				HasActed = TryMove(GetPos() + game::GetMoveVector(c));
				ValidKeyPressed = true;
			}
		}

		for(uchar c = 1; game::GetCommand(c); ++c)
			if(Key == game::GetCommand(c)->GetKey())
			{
				if(game::GetInWilderness() && !game::GetCommand(c)->GetCanBeUsedInWilderness())
					ADD_MESSAGE("This function cannot be used while in wilderness.");
				else
					HasActed = (this->*game::GetCommand(c)->GetLinkedFunction())();

				ValidKeyPressed = true;
			}

		if (!ValidKeyPressed)
			ADD_MESSAGE("Unknown key, you %s. Press '?' for a list of commands.", game::Insult());
	}
}

void character::Vomit(ushort HowMuch)
{
	if(GetIsPlayer())
		ADD_MESSAGE("You vomit.");
	else
		if(GetLevelSquareUnder()->CanBeSeen())
			ADD_MESSAGE("%s vomits.", CNAME(DEFINITE));

	SetNP(GetNP() - 20 - rand() % 21);
	GetLevelSquareUnder()->SpillFluid(HowMuch, MAKE_RGB(10,230,10),3,50);
}

bool character::Apply()
{
	ushort Index;

	if((Index = GetStack()->DrawContents("What do you want to apply?")) == 0xFFFF)
		return false;

	if(Index < GetStack()->GetItems())
	{
		if(!GetStack()->GetItem(Index)->Apply(this, GetStack()))
			return false;

		if(!GetWielded()->GetExists()) 
			SetWielded(0);
	}
	else
		return false;

	return true;
}

vector2d character::GetPos() const
{
	return SquareUnder->GetPos();
}

bool character::ForceVomit()
{
	ADD_MESSAGE("You push your fingers down to your throat and vomit.");
	Vomit(1 + rand() % 3);
	return true;
}

bool character::Zap()
{
	ushort Index;
	if((Index = GetStack()->DrawContents("What do you want to zap with?")) == 0xFFFF)
	{
		ADD_MESSAGE("You have nothing to zap with.");
		return false;
	}

	if(Index < GetStack()->GetItems())
	{
		if(!GetStack()->GetItem(Index)->CanBeZapped())
		{
			ADD_MESSAGE("You can't zap that!");
			return false;
		}
		uchar Answer = game::DirectionQuestion("In what direction do you wish to zap? Press . to zap yourself.", 8, false, true);
		if(Answer == 0xFF)
			return false;
		return GetStack()->GetItem(Index)->Zap(this, GetPos(), Answer);
	}
	else
		return false;

	return true;	
}

bool character::Polymorph()
{
	if(GetIsPlayer() && State & POLYMORPHED)
	{
		ADD_MESSAGE("You shudder.");
		return true;
	}

	character* NewForm = protosystem::BalancedCreateMonster(5);

	GetSquareUnder()->RemoveCharacter();
	GetSquareUnder()->AddCharacter(NewForm);
	SetSquareUnder(0);

	while(GetStack()->GetItems())
		GetStack()->MoveItem(0, NewForm->GetStack());

	if(NewForm->CanWield())
		NewForm->SetWielded(GetWielded());

	if(NewForm->CanWear())
		NewForm->SetTorsoArmor(GetTorsoArmor());

	NewForm->SetTeam(GetTeam());

	if(GetTeam()->GetLeader() == this)
		GetTeam()->SetLeader(NewForm);

	if(GetIsPlayer())
	{
		ADD_MESSAGE("Your body glows in a crimson light. You transform into %s!", NewForm->CNAME(INDEFINITE));
		game::SetPlayerBackup(this);
		game::SetPlayer(NewForm);
		NewForm->ActivateState(POLYMORPHED);
		NewForm->SetStateCounter(POLYMORPHED, 1000);
	}
	else
		SetExists(false);

	return true;
}

worldmapsquare* character::GetWorldMapSquareUnder() const
{
	return (worldmapsquare*)SquareUnder;
}

ulong character::GetBloodColor() const
{
	return MAKE_RGB(75,0,0);
}

void character::BeKicked(ushort KickStrength, bool ShowOnScreen, uchar Direction, character*)
{
	if(rand() % 10 +  rand() % 3 * KickStrength / 2 > GetAgility())
	{
		if(KickStrength > 8 + rand() % 4 - rand() % 4)
		{
			FallTo(GetPos() + game::GetMoveVector(Direction), ShowOnScreen);
			if(ShowOnScreen)
			{
				if(GetIsPlayer())
					ADD_MESSAGE("The kick throws you off balance.");
				else
					ADD_MESSAGE("The kick throws %s off balance.", Name(DEFINITE).c_str());
			}
			SetHP(GetHP() - rand() % (KickStrength / 5));
			CheckDeath("kicked to death");		
		}
		else
			if(ShowOnScreen)
			{
				if(GetIsPlayer())
					ADD_MESSAGE("The kick hits you, but you remain standing.");
				else
					ADD_MESSAGE("The kick hits %s.", Name(DEFINITE).c_str());
				SetHP(GetHP() - rand() % (KickStrength / 7));
				CheckDeath("kicked to death");
			}
	}
	else
		if(ShowOnScreen)
		{
			if(GetIsPlayer())
				ADD_MESSAGE("The kick misses you.");
			else
				ADD_MESSAGE("The kick misses %s.", Name(DEFINITE).c_str());
		}
	
}

void character::FallTo(vector2d Where, bool OnScreen)
{
	SetAP(GetAP() - 500);

	if(game::GetCurrentLevel()->GetLevelSquare(Where)->GetOverLevelTerrain()->GetIsWalkable() && !game::GetCurrentLevel()->GetLevelSquare(Where)->GetCharacter())
	{
		Move(Where, true);
	}

	if(!game::GetCurrentLevel()->GetLevelSquare(Where)->GetOverLevelTerrain()->GetIsWalkable())
	{
		if(OnScreen)
		{
			if(GetIsPlayer()) 
				ADD_MESSAGE("You hit your head on the wall.");
			else
				ADD_MESSAGE("%s hits %s head on the wall.", Name(DEFINITE).c_str(), game::PossessivePronoun(GetSex()));
		}

		SetHP(GetHP() - rand() % 2);
		CheckDeath("killed by hitting a wall");
	}

	// Place code that handles characters bouncing to each other here
}

bool character::CheckCannibalism(ushort What)
{ 
	return (GetMaterial(0)->GetType() == What); 
}

void character::SoldierAICommand()
{
	SeekLeader();

	if(CheckForEnemies())
		return;

	if(CheckForDoors())
		return;

	if(CheckForUsefulItemsOnGround())
		return;
}

ushort character::GetSpeed() const
{
	return GetWielded() ? ushort(sqrt((ulong(GetAgility() << 2) + GetStrength()) * 20000 / Wielded->GetWeight())) : ulong(GetAgility() << 2) + GetStrength();
}

bool character::ShowWeaponSkills()
{
	ADD_MESSAGE("This race isn't capable of developing weapon skill experience!");

	return false;
}

void character::Faint()
{
	DeActivateVoluntaryStates();

	if(GetIsPlayer())
		ADD_MESSAGE("You faint.");
	else
		if(GetLevelSquareUnder()->CanBeSeen())
			ADD_MESSAGE("%s faints.", CNAME(DEFINITE));

	SetStrengthExperience(GetStrengthExperience() - 100);
	ActivateState(FAINTED);
	StateCounter[FAINTED] = 100 + rand() % 101;
}

void character::FaintHandler()
{
	if(!(StateCounter[FAINTED]--))
	{
		if(GetIsPlayer())
			ADD_MESSAGE("You wake up.");
		else
			if(GetLevelSquareUnder()->CanBeSeen())
				ADD_MESSAGE("%s wakes up.", CNAME(DEFINITE));

		EndFainted();
	}
}

void character::EatHandler()
{
	if(GetConsumingCurrently()->Consume(this, 1000))
	{
		item* ToBeDeleted = GetStack()->RemoveItem(GetStack()->SearchItem(GetConsumingCurrently()));

		if(!ToBeDeleted)
			ToBeDeleted = GetLevelSquareUnder()->GetStack()->RemoveItem(GetLevelSquareUnder()->GetStack()->SearchItem(GetConsumingCurrently()));

		if(GetIsPlayer())
			ADD_MESSAGE("You finish eating %s.", ToBeDeleted->CNAME(DEFINITE));
		else
			if(GetLevelSquareUnder()->CanBeSeen())
				ADD_MESSAGE("%s finishes eating %s.", CNAME(DEFINITE), ToBeDeleted->CNAME(DEFINITE));

		EndEating();

		delete ToBeDeleted;
	}

	if(StateIsActivated(EATING) && !(StateCounter[EATING]--))
	{
		if(GetIsPlayer())
			ADD_MESSAGE("You have eaten for a long time now. You stop eating.");
		else
			if(GetLevelSquareUnder()->CanBeSeen())
				ADD_MESSAGE("%s finishes eating %s.", CNAME(DEFINITE), GetConsumingCurrently()->CNAME(DEFINITE));

		EndEating();
	}
}

void character::PolymorphHandler()
{
	if(!(StateCounter[POLYMORPHED]--))
	{
		ADD_MESSAGE("You return to your true form.");

		EndPolymorph();
	}
}

void character::EndFainted()
{
	if(StateIsActivated(FAINTED))
		DeActivateState(FAINTED);
}

void character::EndEating()
{
	if(StateIsActivated(EATING))
	{
		DeActivateState(EATING);

		SetConsumingCurrently(0);
	}
}

void character::EndPolymorph()
{
	if(StateIsActivated(POLYMORPHED))
	{
		SetExists(false);

		GetSquareUnder()->RemoveCharacter();
		GetSquareUnder()->AddCharacter(game::GetPlayerBackup());

		while(GetStack()->GetItems())
			GetStack()->MoveItem(0, game::GetPlayerBackup()->GetStack());

		game::SetPlayer(game::GetPlayerBackup());
		game::SetPlayerBackup(0);

		if(GetTeam()->GetLeader() == this)
			GetTeam()->SetLeader(game::GetPlayer());
	}
}

bool character::CanMove()
{
	if(StateIsActivated(FAINTED) || StateIsActivated(EATING) || StateIsActivated(RESTING) || StateIsActivated(DIGGING))
		return false;
	else
		return true;
}

void character::DeActivateVoluntaryStates(std::string Reason)
{
	if(GetIsPlayer())
	{
		if((StateIsActivated(EATING) || StateIsActivated(RESTING) || StateIsActivated(DIGGING)) && Reason != "")
			ADD_MESSAGE("%s.", Reason.c_str());

		if(StateIsActivated(EATING))
			ADD_MESSAGE("You stop eating.");

		if(StateIsActivated(RESTING))
			ADD_MESSAGE("You stop resting.");

		if(StateIsActivated(DIGGING))
		{
			ADD_MESSAGE("You stop digging.");
			SetAP(750);
		}

	}

	EndEating();
	EndRest();
	EndDig();
}

void character::StateAutoDeactivation()
{
	if(!StateIsActivated(EATING) && !StateIsActivated(RESTING) && !StateIsActivated(DIGGING))
		return;

	DO_FILLED_RECTANGLE(GetPos().X, GetPos().Y, 0, 0, game::GetCurrentArea()->GetXSize() - 1, game::GetCurrentArea()->GetYSize() - 1, LOSRange(),
	{
		character* Character = game::GetCurrentArea()->GetSquare(vector2d(XPointer,YPointer))->GetCharacter();

		if(Character && ((GetIsPlayer() && Character->GetSquareUnder()->CanBeSeen()) || (!GetIsPlayer() && Character->GetSquareUnder()->CanBeSeenFrom(GetPos()))))
			if(GetTeam()->GetRelation(Character->GetTeam()) == HOSTILE)
			{
				DeActivateVoluntaryStates(Character->Name(DEFINITE) + " seems to be hostile");
				return;
			}
	});
}

void character::StruckByWandOfStriking()
{
	if(GetIsPlayer())
		ADD_MESSAGE("The wand hits you.");
	else
		if(GetLevelSquareUnder()->CanBeSeen())
			ADD_MESSAGE("The wand hits %s.", CNAME(DEFINITE));

	SetHP(GetHP() - 10 - rand() % 10);
	
	CheckDeath("killed by a wand of striking");
}

bool character::CheckForEnemies()
{
	character* NearestChar = 0;
	ulong NearestDistance = 0xFFFFFFFF;

	DO_FILLED_RECTANGLE(GetPos().X, GetPos().Y, 0, 0, game::GetCurrentLevel()->GetXSize() - 1, game::GetCurrentLevel()->GetYSize() - 1, LOSRange(),
	{
		character* Char = game::GetCurrentLevel()->GetLevelSquare(vector2d(XPointer,YPointer))->GetCharacter();

		if(Char && GetTeam()->GetRelation(Char->GetTeam()) == HOSTILE && Char->GetLevelSquareUnder()->CanBeSeenFrom(GetPos()))
		{
			ulong ThisDistance = GetHypotSquare(long(XPointer) - GetPos().X, long(YPointer) - GetPos().Y);

			if(ThisDistance < NearestDistance || (ThisDistance == NearestDistance && !(rand() % 3)))
			{
				NearestChar = Char;
				NearestDistance = ThisDistance;
			}
		}
	});

	if(NearestChar)
	{
		MoveTowards(NearestChar->GetPos());
		return true;
	}
	else
		return false;
}

bool character::CheckForDoors()
{
	DO_FOR_SQUARES_AROUND(GetPos().X, GetPos().Y, game::GetCurrentLevel()->GetXSize(), game::GetCurrentLevel()->GetYSize(),
	if(game::GetCurrentLevel()->GetLevelSquare(vector2d(DoX, DoY))->Open(this))
	{
		if(game::GetCurrentLevel()->GetLevelSquare(vector2d(DoX, DoY))->CanBeSeen())
			if(GetLevelSquareUnder()->CanBeSeen())
				ADD_MESSAGE("%s opens the door.", CNAME(DEFINITE));
			else
				ADD_MESSAGE("Something opens the door.");

		return true;
	})

	return false;
}

bool character::CheckForUsefulItemsOnGround()
{
	for(ushort c = 0; c < GetLevelSquareUnder()->GetStack()->GetItems(); ++c)
	{
		if(GetLevelSquareUnder()->GetStack()->GetItem(c)->GetWeaponStrength() > GetAttackStrength() && GetBurdenState(GetStack()->SumOfMasses() + GetLevelSquareUnder()->GetStack()->GetItem(c)->GetWeight()) == UNBURDENED && CanWield())
		{
			item* ToWield = GetLevelSquareUnder()->GetStack()->MoveItem(c, GetStack());

			if(GetWielded())
				GetStack()->MoveItem(GetStack()->SearchItem(GetWielded()), GetLevelSquareUnder()->GetStack());

			SetWielded(ToWield);

			if(GetLevelSquareUnder()->CanBeSeen())
				ADD_MESSAGE("%s picks up and wields %s.", CNAME(DEFINITE), ToWield->CNAME(DEFINITE));

			return true;
		}

		if(GetLevelSquareUnder()->GetStack()->GetItem(c)->GetArmorValue() < CalculateArmorModifier() && GetBurdenState(GetStack()->SumOfMasses() + GetLevelSquareUnder()->GetStack()->GetItem(c)->GetWeight()) == UNBURDENED && CanWear())
		{
			item* ToWear = GetLevelSquareUnder()->GetStack()->MoveItem(c, GetStack());

			if(GetTorsoArmor())
				GetStack()->MoveItem(GetStack()->SearchItem(GetTorsoArmor()), GetLevelSquareUnder()->GetStack());

			SetTorsoArmor(ToWear);

			if(GetLevelSquareUnder()->CanBeSeen())
				ADD_MESSAGE("%s picks up and wears %s.", CNAME(DEFINITE), ToWear->CNAME(DEFINITE));

			return true;
		}

		if(GetLevelSquareUnder()->GetStack()->GetItem(c)->Consumable(this))
		{
			if(GetLevelSquareUnder()->CanBeSeen())
				ADD_MESSAGE("%s eats %s.", CNAME(DEFINITE), GetLevelSquareUnder()->GetStack()->GetItem(c)->CNAME(DEFINITE));

			ConsumeItem(GetLevelSquareUnder()->GetStack()->GetItem(c), GetLevelSquareUnder()->GetStack());

			return true;
		}
	}

	return false;
}

bool character::FollowLeader()
{
	if(!GetTeam()->GetLeader())
		return false;

	if(GetTeam()->GetLeader()->GetLevelSquareUnder()->CanBeSeenFrom(GetPos()))
	{
		vector2d Distance = GetPos() - WayPoint;

		if(abs(long(Distance.X)) <= 2 && abs(long(Distance.Y)) <= 2)
			return false;
		else
		{
			MoveTowards(WayPoint);
			return true;
		}
	}
	else
		if(WayPoint.X != 0xFFFF && WayPoint != GetPos())
		{
			MoveTowards(WayPoint);
			return true;
		}
		else
			return false;
}

void character::SeekLeader()
{
	if(GetTeam()->GetLeader() && GetTeam()->GetLeader()->GetLevelSquareUnder()->CanBeSeenFrom(GetPos()))
		WayPoint = GetTeam()->GetLeader()->GetPos();
}

bool character::RestUntilHealed(void)
{
	long HPToRest = game::NumberQuestion("Rest until X HP", vector2d(7,7), WHITE);

	if(HPToRest < GetHP())
	{
		ADD_MESSAGE("Your HP is already %d", GetHP());
		return false;
	}

	if(HPToRest > GetEndurance() << 1)
		HPToRest = GetEndurance() << 1;

	StateCounter[RESTING] = HPToRest;
	ActivateState(RESTING);
	return true;
}


void character::RestHandler(void)
{
	if(GetHP() >= StateCounter[RESTING])
		EndRest();
}

void character::EndRest(void)
{
	 DeActivateState(RESTING);
}

void character::DigHandler(void)
{
	if(StateCounter[DIGGING] > 0)
	{
		StateCounter[DIGGING]--;
	}
	else
		EndDig();
}

void character::EndDig(void)
{
	if(StateIsActivated(DIGGING))
	{
		if(StateCounter[DIGGING] == 0)
		{
			game::GetCurrentLevel()->GetLevelSquare(GetSquareBeingDigged())->Dig(this, GetWielded());
			SetWielded(GetOldWieldedItem());
		}
		DeActivateState(DIGGING);
	}
}
