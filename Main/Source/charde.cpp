#include <cmath>

#include "charde.h"
#include "stack.h"
#include "itemde.h"
#include "level.h"
#include "lsquare.h"
#include "femath.h"
#include "message.h"
#include "igraph.h"
#include "bitmap.h"
#include "hscore.h"
#include "save.h"
#include "god.h"
#include "feio.h"
#include "wskill.h"
#include "felist.h"
#include "strover.h"

void humanoid::VirtualConstructor()
{
	WeaponSkill = new weaponskill*[WEAPON_SKILL_GATEGORIES];

	for(uchar c = 0; c < WEAPON_SKILL_GATEGORIES; ++c)
		WeaponSkill[c] = new weaponskill(c);
}

humanoid::~humanoid()
{
	for(uchar c = 0; c < WEAPON_SKILL_GATEGORIES; ++c)
		delete WeaponSkill[c];

	delete [] WeaponSkill;
}

void perttu::CreateInitialEquipment()
{
	SetWielded(GetStack()->GetItem(GetStack()->FastAddItem(new valpurijustifier)));
	SetTorsoArmor(GetStack()->GetItem(GetStack()->FastAddItem(new platemail(new valpurium(4000)))));
}

void oree::CreateInitialEquipment()
{
	GetStack()->FastAddItem(new maakotkashirt);

	for(ushort c = 0; c < 6; ++c)
	{
		item* Can = new can(false);
		Can->InitMaterials(2, new iron(10), new pepsi(330));
		GetStack()->FastAddItem(Can);
	}
}

void swatcommando::CreateInitialEquipment()
{
	SetWielded(GetStack()->GetItem(GetStack()->FastAddItem(new curvedtwohandedsword)));
}

void fallenvalpurist::CreateInitialEquipment()
{
	SetWielded(GetStack()->GetItem(GetStack()->FastAddItem(new pickaxe)));
}

void froggoblin::CreateInitialEquipment()
{
	SetWielded(GetStack()->GetItem(GetStack()->FastAddItem(new spear)));
}

ushort humanoid::CalculateArmorModifier() const
{
	return Armor.Torso ? Armor.Torso->GetArmorValue() : 100;
}

bool ennerbeast::Hit(character*)
{
	char Message[256] ;
	if(rand() % 2) sprintf(Message,"The Enner Beast yells: UGH UGHAaaa!");
	else sprintf(Message, "The Enner Beast yells: Uga Ugar Ugade Ugat!");

	DO_FILLED_RECTANGLE(GetPos().X, GetPos().Y, 0, 0, game::GetCurrentLevel()->GetXSize() - 1, game::GetCurrentLevel()->GetYSize() - 1, 100,
	{
		character* Char = game::GetCurrentLevel()->GetLevelSquare(vector2d(XPointer, YPointer))->GetCharacter();
		float ScreamStrength = GetMeleeStrength() * GetStrength() / GetHypotSquare<float>(float(GetPos().X) - XPointer, float(GetPos().Y) - YPointer);
		if(Char && Char != this)
			Char->ReceiveSound(Message, rand() % 26 - rand() % 26,ScreamStrength);
		game::GetCurrentLevel()->GetLevelSquare(vector2d(XPointer, YPointer))->GetStack()->ReceiveSound(ScreamStrength);
		for(int x = 0; x < 4; x++)
			game::GetCurrentLevel()->GetLevelSquare(vector2d(XPointer, YPointer))->GetSideStack(x)->ReceiveSound(ScreamStrength);
	});

	SetStrengthExperience(GetStrengthExperience() + 100);

	return true;
}

bool humanoid::Drop()
{
	ushort Index = GetStack()->DrawContents("What do you want to drop?");

	if(Index < GetStack()->GetItems() && GetStack()->GetItem(Index))
		if(GetStack()->GetItem(Index) == GetWielded())
			ADD_MESSAGE("You can't drop something you wield!");
		else if(GetStack()->GetItem(Index) == Armor.Torso)
			ADD_MESSAGE("You can't drop something you wear!");
		else
		{
			GetStack()->MoveItem(Index, GetLevelSquareUnder()->GetStack());

			return true;
		}

	return false;
}

void golem::DrawToTileBuffer() const
{
	Picture->MaskedBlit(igraph::GetTileBuffer(), 0, 0, 0, 0, 16, 16);
}

void humanoid::DrawToTileBuffer() const
{	
	vector2d InHandsPic, ArmPos, HeadPos;

	if(GetArmType() > 16)
	{
		ArmPos.X = 80;
		ArmPos.Y = (GetArmType() - 16) * 16;
	}
	else
	{
		ArmPos.X = 64;
		ArmPos.Y = GetArmType() * 16;
	}
	if(GetHeadType() > 16)
	{
		HeadPos.X = 112;
		HeadPos.Y = (GetHeadType() - 16) * 16;
	}
	else
	{
		HeadPos.X = 96;
		HeadPos.Y = GetHeadType() * 16;
	}

	if(GetWielded() != 0) InHandsPic = GetWielded()->GetInHandsPic();

	igraph::GetHumanGraphic()->MaskedBlit(igraph::GetTileBuffer(), 0, 0, 0, 0, 16, 16); // Legs
	igraph::GetHumanGraphic()->MaskedBlit(igraph::GetTileBuffer(), 32, 0, 0, 0, 16, 16); // Torso
	igraph::GetHumanGraphic()->MaskedBlit(igraph::GetTileBuffer(), ArmPos.X, ArmPos.Y, 0, 0, 16, 16); // Arms
	igraph::GetHumanGraphic()->MaskedBlit(igraph::GetTileBuffer(), HeadPos.X, HeadPos.Y, 0, 0, 16, 16); // Head

	if(GetWielded() != 0)
		if(InHandsPic.X != 0 || InHandsPic.Y != 0) igraph::GetHumanGraphic()->MaskedBlit(igraph::GetTileBuffer(), InHandsPic.X , InHandsPic.Y, 0, 0, 16, 16); // Wielded
}

bool humanoid::Wield()
{
	ushort Index;
	if((Index = GetStack()->DrawContents("What do you want to wield? or press '-' for hands")) == 0xFFFF)
	{
		ADD_MESSAGE("You have nothing to wield.");
		return false;
	}

	if(Index == 0xFFFE)
		SetWielded(0);
	else
		if(Index < GetStack()->GetItems())
	{
	if(GetStack()->GetItem(Index) != Armor.Torso)
		SetWielded(GetStack()->GetItem(Index));
	else ADD_MESSAGE("You can't wield something that you wear!");
	}

		else
			return false;

	return true;
	
}

void fallenvalpurist::CreateCorpse()
{
	ushort Amount = 2 + rand() % 4;

	for(ushort c = 0; c < Amount; ++c)
		GetLevelSquareUnder()->GetStack()->AddItem(new abone);
}

void elpuri::CreateCorpse()
{
	character::CreateCorpse();

	GetLevelSquareUnder()->GetStack()->AddItem(new headofelpuri);
}

void perttu::CreateCorpse()
{
	GetLevelSquareUnder()->GetStack()->AddItem(new leftnutofperttu);
}

bool humanoid::WearArmor()
{
	ushort Index;

	if((Index = GetStack()->DrawContents("What do you want to wear? or press '-' for nothing")) == 0xFFFF)
		return false;

	if(Index == 0xFFFE)
	{
		Armor.Torso = 0;
		return true;
	}
	else
		if(Index < GetStack()->GetItems())		// Other Armor types should be coded...
		{
			if(GetStack()->GetItem(Index)->CanBeWorn())
			{
				if(GetStack()->GetItem(Index) != GetWielded())
				{
					Armor.Torso = GetStack()->GetItem(Index);
					return true;
				}
				else
					ADD_MESSAGE("You can't wear something that you wield!");
			}
			else
				ADD_MESSAGE("You can't wear THAT!");
		}

	return false;
}

void perttu::BeTalkedTo(character* Talker)
{
	static bool Triggered = false;

	if(Talker->HasMaakotkaShirt())
	{
		iosystem::TextScreen(FONTW, "Thou hast slain the Pepsi Daemon King, and Perttu is happy!\n\nYou are victorious!");
		game::RemoveSaves();
		game::Quit();

		if(!game::GetWizardMode())
		{
			AddScoreEntry("retrieved the Holy Maakotka Shirt and was titled as the Avatar of Law", 3);
			highscore HScore;
			HScore.Draw(FONTW, FONTB);
		}
	}
	else
		if(Triggered)
			ADD_MESSAGE("Perttu says: \"Bring me the Maakotka shirt and we'll talk.\"");

	if(Talker->HasHeadOfElpuri() && !Triggered)
	{
		if(game::GetGod(1)->GetRelation() >= 500 && Talker->GetDifficulty() >= 2500 && game::BoolQuestion("Perttu smiles. \"Thou areth indeed a great Champion of the Great Frog! Elpuri is not a foe worthy for thee. Dost thou wish to stay in the dungeon for a while more and complete another quest for me?\" (Y/n)", 'y'))
		{
			iosystem::TextScreen(FONTW, "Champion of Law!\n\nSeek out the Master Evil: Oree the Pepsi Daemon King,\nwho hast stolenth one of the most powerful of all of my artifacts:\nthe Holy Maakotka Shirt! Return with it and immortal glory shall be thine!");

			game::TriggerQuestForMaakotkaShirt();

			Triggered = true;
		}
		else
		{
		iosystem::TextScreen(FONTW, "Thou hast slain Elpuri, and Perttu is happy!\n\nYou are victorious!");
		game::RemoveSaves();
		game::Quit();

		if(!game::GetWizardMode())
		{
			AddScoreEntry("defeated Elpuri and continued to further adventures", 2);
			highscore HScore;
			HScore.Draw(FONTW, FONTB);
		}
		}
	}
	else
		if(!Triggered)
			ADD_MESSAGE("Perttu says: \"Bring me the head of Elpuri and we'll talk.\"");
}

void humanoid::Save(outputfile& SaveFile) const
{
	character::Save(SaveFile);

	ushort Index = Armor.Torso ? Stack->SearchItem(Armor.Torso) : 0xFFFF;

	SaveFile << Index << ArmType << HeadType << LegType << TorsoType;

	for(uchar c = 0; c < WEAPON_SKILL_GATEGORIES; ++c)
		SaveFile << GetWeaponSkill(c);
}

void humanoid::Load(inputfile& SaveFile)
{
	character::Load(SaveFile);

	ushort Index;

	SaveFile >> Index >> ArmType >> HeadType >> LegType >> TorsoType;

	Armor.Torso = Index != 0xFFFF ? Stack->GetItem(Index) : 0;

	for(uchar c = 0; c < WEAPON_SKILL_GATEGORIES; ++c)
		SaveFile >> GetWeaponSkill(c);
}

float golem::GetMeleeStrength() const
{
	return 75 * GetMaterial(0)->GetHitValue();
}

ushort golem::CalculateArmorModifier() const
{
	if(GetMaterial(0)->GetArmorValue() / 2 > 90)
		return 10;
	else
		return 100 - GetMaterial(0)->GetArmorValue() / 2;
}

void golem::MoveRandomly()
{
	ushort ToTry = rand() % 9;

	if(ToTry == 8)
	{
		if(!(rand () % 100))
			Engrave("Golem Needs Master");
	}
	else
		if(!game::GetCurrentLevel()->GetLevelSquare(GetPos() + game::GetMoveVector(ToTry))->GetCharacter())
			TryMove(GetPos() + game::GetMoveVector(ToTry));
}

void ennerbeast::HostileAICommand()
{
	if(GetLevelSquareUnder()->RetrieveFlag() && (float(GetPos().X) - game::GetPlayer()->GetPos().X) * (float(GetPos().X) - game::GetPlayer()->GetPos().X) + (float(GetPos().Y) - game::GetPlayer()->GetPos().Y) * (float(GetPos().Y) - game::GetPlayer()->GetPos().Y) <= LOSRange())
	{
		Charge(game::GetPlayer());
		return;
	}
	else
	{
		if(rand() % 3)
			Hit(0);
		else
			MoveRandomly();
	}
}

void perttu::NeutralAICommand()
{
	SetHealTimer(GetHealTimer() + 1);
	DO_FOR_SQUARES_AROUND(GetPos().X, GetPos().Y, game::GetCurrentLevel()->GetXSize(), game::GetCurrentLevel()->GetYSize(),
	if(game::GetCurrentLevel()->GetLevelSquare(vector2d(DoX, DoY))->GetCharacter())
	{
		if(game::GetCurrentLevel()->GetLevelSquare(vector2d(DoX, DoY))->GetCharacter() == game::GetPlayer())
		{
			if(game::GetCurrentLevel()->GetLevelSquare(vector2d(DoX, DoY))->GetCharacter()->GetHP() < (game::GetCurrentLevel()->GetLevelSquare(vector2d(DoX, DoY))->GetCharacter()->GetEndurance() << 1) / 3 && game::GetCurrentLevel()->GetLevelSquare(vector2d(DoX, DoY))->GetCharacter() == game::GetPlayer() && GetHealTimer() > 100)
				HealFully(game::GetPlayer());
		}
		else if(game::GetCurrentLevel()->GetLevelSquare(vector2d(DoX, DoY))->GetCharacter()->GetRelations() == HOSTILE)
			Hit(game::GetCurrentLevel()->GetLevelSquare(vector2d(DoX, DoY))->GetCharacter());
	})
}

void perttu::HealFully(character* ToBeHealed)
{
	SetHealTimer(0);
	ToBeHealed->SetHP(ToBeHealed->GetEndurance() << 1);
	if(ToBeHealed->GetIsPlayer())
		ADD_MESSAGE("%s heals you fully.", CNAME(DEFINITE));
}

void perttu::Save(outputfile& SaveFile) const
{
	humanoid::Save(SaveFile);

	SaveFile << HealTimer;
}

void perttu::Load(inputfile& SaveFile)
{
	humanoid::Load(SaveFile);

	SaveFile >> HealTimer;
}

ulong golem::Danger() const
{
	return ulong(GetMeleeStrength() * 100 / (CalculateArmorModifier() * 15));
}

bool humanoid::Throw()
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
		if(GetStack()->GetItem(Index) == GetTorsoArmor())
		{
			ADD_MESSAGE("You can't throw something that you wear.");
			return false;
		}
		uchar Answer = game::DirectionQuestion("In what direction do you wish to throw it?", 8, false);
		if(Answer == 0xFF)
			return false;
		ThrowItem(Answer, GetStack()->GetItem(Index));
	}
	else
		return false;

	return true;
}

bool dog::Catches(item* Thingy, float)
{
	if(Thingy->DogWillCatchAndConsume())
	{
		if(GetIsPlayer())
			ADD_MESSAGE("You catch %s in mid-air and consume it.", Thingy->CNAME(DEFINITE));
		else
			if(GetSquareUnder()->CanBeSeen())
				ADD_MESSAGE("%s catches %s and eats it.", CNAME(DEFINITE), Thingy->CNAME(DEFINITE));

		ConsumeItem(GetLevelSquareUnder()->GetStack()->SearchItem(Thingy), GetLevelSquareUnder()->GetStack());
		return true;
	}

	return false;
}

bool dog::ConsumeItemType(uchar Type) const     // We need a better system for this... Writing this to every F***ing character that needs one
{					  // is Stoo-bit
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
	case BONE: // Bone is an odd item, cause it actually can be opened, but not always...
		return true;
	break;


	default:
		ADD_MESSAGE("ERRRRORRRRRR in dog::Consumeitemtype."); //All hail SpykoS! He is the author of this file, and his might is over that of PMGR!!!
	}
		
	return false;
}

bool humanoid::Apply()
{
	ushort Index;
	if((Index = GetStack()->DrawContents("What do you want to apply?")) == 0xFFFF)
	{
		ADD_MESSAGE("You have nothing to apply.");
		return false;
	}


	if(Index < GetStack()->GetItems())
	{
		if(GetStack()->GetItem(Index) == GetWielded())
		{
			ADD_MESSAGE("You can't apply something that you wield.");
			return false;
		}
		if(GetStack()->GetItem(Index) == GetTorsoArmor())
		{
			ADD_MESSAGE("You can't apply something that you wear.");
			return false;
		}
		
		if(!GetStack()->GetItem(Index)->Apply(this))
			return false;
	}
	else
		return false;

	return true;
}

float humanoid::GetAttackStrength() const
{
	return GetWielded() ? GetWielded()->GetWeaponStrength() * GetWeaponSkill(GetWielded()->GetWeaponCategory())->GetBonus() : GetMeleeStrength() * GetWeaponSkill(UNARMED)->GetBonus();
}

ushort humanoid::GetSpeed() const
{
	return GetWielded() ? ushort(sqrt((ulong(GetAgility() << 2) + GetStrength()) * 20000 / GetWielded()->GetWeight()) * GetWeaponSkill(GetWielded()->GetWeaponCategory())->GetBonus()) : ushort(((GetAgility() << 2) + GetStrength()) * GetWeaponSkill(UNARMED)->GetBonus());
}

bool humanoid::Hit(character* Enemy)
{
	if(Enemy->GetRelations())
		if(GetIsPlayer())
			if(!game::BoolQuestion("This might cause a hostile reaction. Are you sure? [Y/N]"))
				return false;

	short Success = rand() % 26 - rand() % 26;

	switch(Enemy->TakeHit(GetSpeed(), Success, GetAttackStrength(), this)) //there's no breaks and there shouldn't be any
	{
	case HAS_HIT:
	case HAS_BLOCKED:
		if(GetWielded())
			GetWielded()->ReceiveHitEffect(Enemy, this);
	case HAS_DIED:
		SetStrengthExperience(GetStrengthExperience() + 50);
		GetWeaponSkill(GetWielded() ? GetWielded()->GetWeaponCategory() : UNARMED)->AddHit(GetIsPlayer());
	case HAS_DODGED:
		SetAgilityExperience(GetAgilityExperience() + 25);
	}

	SetNP(GetNP() - 4);

	return true;
}

void humanoid::CharacterSpeciality()
{
	for(uchar c = 0; c < WEAPON_SKILL_GATEGORIES; ++c)
		GetWeaponSkill(c)->Turn(GetIsPlayer());
}

bool humanoid::ShowWeaponSkills()
{
	felist List("Current Weapon Skills");

	List.AddDescription("");
	List.AddDescription("Skill Name          Level     Points    To Next Level");

	for(uchar c = 0; c < WEAPON_SKILL_GATEGORIES; ++c)
	{
		std::string Buffer;

		Buffer += GetWeaponSkill(c)->Name();
		Buffer.resize(20, ' ');

		Buffer += GetWeaponSkill(c)->GetLevel();
		Buffer.resize(30, ' ');

		Buffer += int(GetWeaponSkill(c)->GetHits());
		Buffer.resize(40, ' ');

		if(GetWeaponSkill(c)->GetLevel() != 10)
			List.AddString(Buffer + (GetWeaponSkill(c)->GetLevelMap(GetWeaponSkill(c)->GetLevel() + 1) - GetWeaponSkill(c)->GetHits()));
		else
			List.AddString(Buffer + '-');
	}

	List.Draw(FONTW, FONTR, false);

	return false;
}
