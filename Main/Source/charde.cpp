#define __FILE_OF_STATIC_CHARACTER_PROTOTYPE_DECLARATIONS__

#include "proto.h"
#include "charba.h"

std::vector<character::prototype*>	protocontainer<character>::ProtoData;
std::map<std::string, ushort>		protocontainer<character>::CodeNameMap;

#include "femath.h"
#include "itemde.h"
#include "charde.h"

#undef __FILE_OF_STATIC_CHARACTER_PROTOTYPE_DECLARATIONS__

#include <cmath>

#include "stack.h"
#include "itemde.h"
#include "level.h"
#include "lsquare.h"
#include "message.h"
#include "hscore.h"
#include "godba.h"
#include "feio.h"
#include "wskill.h"
#include "felist.h"
#include "strover.h"
#include "team.h"
#include "lterraba.h"
#include "error.h"
#include "config.h"
#include "slot.h"
#include "actionde.h"

petrus::~petrus()
{
  game::SetPetrus(0);
}

void humanoid::VirtualConstructor()
{
  for(ushort c = 0; c < WEAPON_SKILL_GATEGORIES; ++c)
    CategoryWeaponSkill[c] = new gweaponskill(c);
}

humanoid::~humanoid()
{
  for(ushort c = 0; c < WEAPON_SKILL_GATEGORIES; ++c)
    delete CategoryWeaponSkill[c];

  /* Do not delete these! */

  if(GetHead())
    GetHead()->SetExists(false);

  if(GetRightArm())
    GetRightArm()->SetExists(false);

  if(GetLeftArm())
    GetLeftArm()->SetExists(false);

  if(GetGroin())
    GetGroin()->SetExists(false);

  if(GetRightLeg())
    GetRightLeg()->SetExists(false);

  if(GetLeftLeg())
    GetLeftLeg()->SetExists(false);
}

void petrus::CreateInitialEquipment()
{
  SetMainWielded(new valpurusjustifier);
  SetBodyArmor(new platemail(new valpurium));
}

void priest::CreateInitialEquipment()
{
  SetMainWielded(new spikedmace(new mithril));
  SetBodyArmor(new chainmail(new mithril));
}

void oree::CreateInitialEquipment()
{
  SetBodyArmor(new goldeneagleshirt);
  item* Can = new can(false);
  Can->InitMaterials(2, new iron(10), new pepsi(330));
  GetStack()->FastAddItem(Can);
}

void darkknight::CreateInitialEquipment()
{
  if(RAND() % 20)
    SetMainWielded(RAND() % 5 ? (item*)(new longsword) : RAND() % 20 ? (item*)(new twohandedsword) : (item*)(new curvedtwohandedsword));
  else
    {
      item* DoomsDay = new longsword;
      DoomsDay->InitMaterials(3, new mithril, new iron, new darkfrogflesh);
      SetMainWielded(DoomsDay);
    }

  SetBodyArmor(new chainmail(RAND() % 5 ? (material*)new iron : (material*)new mithril));
}

void skeleton::CreateInitialEquipment()
{
  if(RAND() % 10)
    SetMainWielded(RAND() % 3 ? (item*)new axe : (item*)new pickaxe);
  else
    {
      SetMainWielded(new spikedmace);
      SetSize(200);
      SetAgility(15);
      SetStrength(18);
      SetEndurance(15);
    }
}

void goblin::CreateInitialEquipment()
{
  SetMainWielded(RAND() % 3 ? (item*)new spear : (item*)new poleaxe);
}

void guard::CreateInitialEquipment()
{
  SetMainWielded(new longsword(new iron));
  SetBodyArmor(new chainmail);

  GetCategoryWeaponSkill(LARGE_SWORDS)->AddHit(500);
  GetMainArm()->GetCurrentSingleWeaponSkill()->AddHit(500);
}

bool ennerbeast::Hit(character*)
{
  DO_FILLED_RECTANGLE(GetPos().X, GetPos().Y, 0, 0, game::GetCurrentLevel()->GetXSize() - 1, game::GetCurrentLevel()->GetYSize() - 1, 30,
  {
    character* Char = game::GetCurrentLevel()->GetLSquare(vector2d(XPointer, YPointer))->GetCharacter();

    ushort ScreamStrength = GetMeleeStrength() * GetStrength() / GetHypotSquare(float(GetPos().X) - XPointer, float(GetPos().Y) - YPointer);

    if(Char && Char != this)
      {
	if(Char->GetIsPlayer())
	  if(RAND() % 2)
	    ADD_MESSAGE("%s yells: UGH UGHAaaa!", CHARDESCRIPTION(DEFINITE));
	  else
	    ADD_MESSAGE("%s yells: Uga Ugar Ugade Ugat!", CHARDESCRIPTION(DEFINITE));

	Char->ReceiveDamage(this, ScreamStrength, SOUND, ALL, 8, true);
	Char->CheckDeath("killed by " + Name(INDEFINITE) + "'s scream");
      }

    game::GetCurrentLevel()->GetLSquare(vector2d(XPointer, YPointer))->GetStack()->ReceiveDamage(this, ScreamStrength, SOUND);

    for(ushort x = 0; x < 4; ++x)
      game::GetCurrentLevel()->GetLSquare(vector2d(XPointer, YPointer))->GetSideStack(x)->ReceiveDamage(this, ScreamStrength, SOUND);
  });

  EditStrengthExperience(100);
  EditNP(-100);

  return true;
}

void skeleton::CreateCorpse()
{
  ushort Amount = 2 + RAND() % 4;

  for(ushort c = 0; c < Amount; ++c)
    GetLSquareUnder()->GetStack()->AddItem(new abone);
}

void elpuri::CreateCorpse()
{
  character::CreateCorpse();
  GetLSquareUnder()->GetStack()->AddItem(new headofelpuri);
}

void petrus::CreateCorpse()
{
  GetLSquareUnder()->GetStack()->AddItem(new leftnutofpetrus);
}

void ennerbeast::CreateCorpse()
{
  character::CreateCorpse();
  GetLSquareUnder()->GetStack()->AddItem(new headofennerbeast);
}

void humanoid::Save(outputfile& SaveFile) const
{
  character::Save(SaveFile);

  for(ushort c = 0; c < WEAPON_SKILL_GATEGORIES; ++c)
    SaveFile << GetCategoryWeaponSkill(c);
}

void humanoid::Load(inputfile& SaveFile)
{
  character::Load(SaveFile);

  for(ushort c = 0; c < WEAPON_SKILL_GATEGORIES; ++c)
    SaveFile >> GetCategoryWeaponSkill(c);
}

float golem::GetMeleeStrength() const
{
  //return 150 * GetTorso()->GetMaterial(0)->GetHitValue();
  return 0;
}

/*ushort golem::CalculateArmorModifier() const
{
  if(((GetTorso()->GetMaterial(0)->GetStrengthValue() * 3) >> 2) > 90)
    return 10;
  else
    return 100 - ((GetTorso()->GetMaterial(0)->GetStrengthValue() * 3) >> 2);
  return 0;
}*/

bool golem::MoveRandomly()
{
  if(!(RAND() % 500))
  {
    Engrave("Golem Needs Master");
    return true;
  }
  else
    return character::MoveRandomly();
}

void ennerbeast::GetAICommand()
{
  if(RAND() % 3)
    Hit(0);

  SeekLeader();

  if(CheckForEnemies(false))
    return;

  if(FollowLeader())
    return;

  MoveRandomly();
}

void petrus::GetAICommand()
{
  SeekLeader();

  SetHealTimer(GetHealTimer() + 1);

  character* Char;

  DO_FOR_SQUARES_AROUND(GetPos().X, GetPos().Y, game::GetCurrentLevel()->GetXSize(), game::GetCurrentLevel()->GetYSize(),
			if((Char = game::GetCurrentLevel()->GetLSquare(vector2d(DoX, DoY))->GetCharacter()))
  {
    if(GetTeam()->GetRelation(Char->GetTeam()) == FRIEND && Char->GetHP() < Char->GetMaxHP() / 3 && GetHealTimer() > 100)
      {
	HealFully(Char);
	return;
      }
  })

  if(CheckForEnemies(true))
    return;

  if(CheckForUsefulItemsOnGround())
    return;

  if(FollowLeader())
    return;

  CheckForDoors();
}

void petrus::HealFully(character* ToBeHealed)
{
  SetHealTimer(0);

  ToBeHealed->RestoreHP();

  if(ToBeHealed->GetIsPlayer())
    ADD_MESSAGE("%s heals you fully.", CHARDESCRIPTION(DEFINITE));
  else
    ADD_MESSAGE("%s heals %s fully.", CHARDESCRIPTION(DEFINITE), ToBeHealed->CHARDESCRIPTION(DEFINITE));
}

void petrus::Save(outputfile& SaveFile) const
{
  humanoid::Save(SaveFile);

  SaveFile << HealTimer << StoryState;
}

void petrus::Load(inputfile& SaveFile)
{
  humanoid::Load(SaveFile);

  SaveFile >> HealTimer >> StoryState;

  game::SetPetrus(this);
}

bool dog::Catches(item* Thingy, float)
{
  if(Thingy->DogWillCatchAndConsume())
    {
      if(ConsumeItem(Thingy))
	if(GetIsPlayer())
	  ADD_MESSAGE("You catch %s in mid-air and consume it.", Thingy->CHARNAME(DEFINITE));
	else
	  {
	    if(GetLSquareUnder()->CanBeSeen())
	      ADD_MESSAGE("%s catches %s and eats it.", CHARNAME(DEFINITE), Thingy->CHARNAME(DEFINITE));

	    ChangeTeam(game::GetPlayer()->GetTeam());
	  }
      else
	if(GetIsPlayer())
	  ADD_MESSAGE("You catch %s in mid-air.", Thingy->CHARNAME(DEFINITE));
	else
	  if(GetLSquareUnder()->CanBeSeen())
	    ADD_MESSAGE("%s catches %s.", CHARNAME(DEFINITE), Thingy->CHARNAME(DEFINITE));

      return true;
    }
  else
    return false;
}

item* humanoid::GetMainWielded() const
{
  if(GetMainArm())
    if(GetMainArm()->GetWielded())
      return GetMainArm()->GetWielded();
    else
      if(GetSecondaryArm())
	return GetSecondaryArm()->GetWielded();
      else
	return 0;
  else
    if(GetSecondaryArm())
      return GetSecondaryArm()->GetWielded();
    else
      return 0;
}

item* humanoid::GetSecondaryWielded() const
{
  if(GetSecondaryArm())
    return GetSecondaryArm()->GetWielded();
  else
    return 0;
}

float humanoid::GetMainAttackStrength() const
{
  if(GetMainArm())
    if(GetMainArm()->GetWielded())
      return GetMainArm()->GetWieldedStrength(!GetSecondaryArm() || GetSecondaryArm()->GetWielded());
    else
      if(GetSecondaryArm() && GetSecondaryArm()->GetWielded())
	return GetSecondaryArm()->GetWieldedStrength(false);
      else
	return GetMainArm()->GetMeleeStrength();
  else
    if(GetSecondaryArm())
      if(GetSecondaryArm()->GetWielded())
	return GetSecondaryArm()->GetWieldedStrength(true);
      else
	return GetSecondaryArm()->GetMeleeStrength();
    else
      return 0;
}

float humanoid::GetSecondaryAttackStrength() const
{
  if(GetSecondaryArm())
    if(GetSecondaryArm()->GetWielded())
      return GetSecondaryArm()->GetWieldedStrength(!GetMainArm() || GetMainArm()->GetWielded());
    else
      if(GetMainArm() && GetMainArm()->GetWielded())
	return 0;
      else
	return GetSecondaryArm()->GetMeleeStrength();
  else
    return 0;
}

float humanoid::GetMainToHitValue() const
{
  if(GetMainArm())
    if(GetMainArm()->GetWielded())
      return GetMainArm()->GetWieldedToHitValue(!GetSecondaryArm() || GetSecondaryArm()->GetWielded());
    else
      if(GetSecondaryArm() && GetSecondaryArm()->GetWielded())
	return GetSecondaryArm()->GetWieldedToHitValue(false);
      else
	return GetMainArm()->GetMeleeToHitValue();
  else
    if(GetSecondaryArm())
      if(GetSecondaryArm()->GetWielded())
	return GetSecondaryArm()->GetWieldedToHitValue(true);
      else
	return GetSecondaryArm()->GetMeleeToHitValue();
    else
      return 0;
}

float humanoid::GetSecondaryToHitValue() const
{
  if(GetSecondaryArm())
    if(GetSecondaryArm()->GetWielded())
      return GetSecondaryArm()->GetWieldedToHitValue(!GetMainArm() || GetMainArm()->GetWielded());
    else
      if(GetMainArm() && GetMainArm()->GetWielded())
	return 0;
      else
	return GetSecondaryArm()->GetMeleeToHitValue();
  else
    return 0;
}

bool humanoid::Hit(character* Enemy)
{
  if(GetIsPlayer() && GetTeam()->GetRelation(Enemy->GetTeam()) != HOSTILE && !game::BoolQuestion("This might cause a hostile reaction. Are you sure? [y/N]"))
    return false;

  Hostility(Enemy);

  if(GetBurdenState() == OVERLOADED)
    {
      if(GetIsPlayer())
	ADD_MESSAGE("You cannot fight while carrying so much.");

      return true;
    }

  if(GetMainAttackStrength())
    MainHit(Enemy);

  if(GetSecondaryAttackStrength())
    SecondaryHit(Enemy);

  EditNP(-50);

  return true;
}

arm* humanoid::GetMainWeaponArm() const
{
  if(!GetMainArm() || !GetMainArm()->GetWielded())
    return GetSecondaryArm();
  else
    return GetMainArm();
}

arm* humanoid::GetSecondaryWeaponArm() const
{
  return GetSecondaryArm();
}

void humanoid::MainHit(character* Enemy)
{
  switch(Enemy->TakeHit(this, GetMainWielded(), GetMainAttackStrength(), GetMainToHitValue(), RAND() % 26 - RAND() % 26, !(RAND() % Enemy->CriticalModifier()))) //there's no breaks and there shouldn't be any
    {
    case HAS_HIT:
    case HAS_BLOCKED:
      if(GetMainWielded())
	GetMainWielded()->ReceiveHitEffect(Enemy, this);
    case HAS_DIED:
      EditStrengthExperience(50);
      if(GetCategoryWeaponSkill(GetMainWielded() ? GetMainWielded()->GetWeaponCategory() : UNARMED)->AddHit() && GetIsPlayer())
	GetCategoryWeaponSkill(GetMainWielded() ? GetMainWielded()->GetWeaponCategory() : UNARMED)->AddLevelUpMessage();
      if(GetMainWielded())
	{
	  if(GetMainWeaponArm()->GetCurrentSingleWeaponSkill()->AddHit() && GetIsPlayer())
	    GetMainWeaponArm()->GetCurrentSingleWeaponSkill()->AddLevelUpMessage(GetMainWielded()->Name(UNARTICLED));

	  GetMainWielded()->ReceiveDamage(this, GetStrength() / 2, PHYSICALDAMAGE);
	}
    case HAS_DODGED:
      EditAgilityExperience(25);
    }
}

void humanoid::SecondaryHit(character* Enemy)
{
  switch(Enemy->TakeHit(this, GetSecondaryWielded(), GetSecondaryAttackStrength(), GetSecondaryToHitValue(), RAND() % 26 - RAND() % 26, !(RAND() % Enemy->CriticalModifier()))) //there's no breaks and there shouldn't be any
    {
    case HAS_HIT:
    case HAS_BLOCKED:
      if(GetSecondaryWielded())
	GetSecondaryWielded()->ReceiveHitEffect(Enemy, this);
    case HAS_DIED:
      EditStrengthExperience(50);
      if(GetCategoryWeaponSkill(GetSecondaryWeaponArm() ? GetSecondaryWeaponArm()->GetWeaponCategory() : UNARMED)->AddHit() && GetIsPlayer())
	GetCategoryWeaponSkill(GetSecondaryWeaponArm() ? GetSecondaryWeaponArm()->GetWeaponCategory() : UNARMED)->AddLevelUpMessage();
      if(GetSecondaryWielded())
	{
	  if(GetSecondaryWeaponArm()->GetCurrentSingleWeaponSkill()->AddHit() && GetIsPlayer())
	    GetSecondaryWeaponArm()->GetCurrentSingleWeaponSkill()->AddLevelUpMessage(GetMainWielded()->Name(UNARTICLED));

	  GetSecondaryWielded()->ReceiveDamage(this, GetStrength() / 2, PHYSICALDAMAGE);
	}
    case HAS_DODGED:
      EditAgilityExperience(25);
    }
}

void humanoid::CharacterSpeciality(ushort Turns)
{
  for(ushort c = 0; c < WEAPON_SKILL_GATEGORIES; ++c)
    if(GetCategoryWeaponSkill(c)->Turn(Turns) && GetIsPlayer())
      GetCategoryWeaponSkill(c)->AddLevelDownMessage();
}

bool humanoid::ShowWeaponSkills()
{
  {
    felist List("Your experience in weapon categories", WHITE, 0);

    List.AddDescription("");
    List.AddDescription("Category name                 Level     Points    To next level");

    for(ushort c = 0; c < WEAPON_SKILL_GATEGORIES; ++c)
      {
	std::string Buffer;

	Buffer += GetCategoryWeaponSkill(c)->Name();
	Buffer.resize(30, ' ');

	Buffer += GetCategoryWeaponSkill(c)->GetLevel();
	Buffer.resize(40, ' ');

	Buffer += int(GetCategoryWeaponSkill(c)->GetHits());
	Buffer.resize(50, ' ');

	if(GetCategoryWeaponSkill(c)->GetLevel() != 10)
	  List.AddEntry(Buffer + (GetCategoryWeaponSkill(c)->GetLevelMap(GetCategoryWeaponSkill(c)->GetLevel() + 1) - GetCategoryWeaponSkill(c)->GetHits()), LIGHTGRAY);
	else
	  List.AddEntry(Buffer + '-', LIGHTGRAY);
      }

    /*if(CurrentSingleWeaponSkill)
      {
	List.AddEntry("", RED);

	std::string Buffer = "current right hand single weapon skill:  ";

	Buffer += CurrentSingleWeaponSkill->GetLevel();
	Buffer.resize(40, ' ');

	Buffer += int(CurrentSingleWeaponSkill->GetHits());
	Buffer.resize(50, ' ');

	if(CurrentSingleWeaponSkill->GetLevel() != 10)
	  List.AddEntry(Buffer + (CurrentSingleWeaponSkill->GetLevelMap(CurrentSingleWeaponSkill->GetLevel() + 1) - CurrentSingleWeaponSkill->GetHits()), RED);
	else
	  List.AddEntry(Buffer + '-', RED);
      }*/

    List.Draw(vector2d(26, 42), 652, 20, false);
  }

  return false;
}

void shopkeeper::CreateInitialEquipment()
{
  SetMainWielded(new pickaxe(new mithril));
  SetBodyArmor(new chainmail(new mithril));

  GetCategoryWeaponSkill(AXES)->AddHit(5000);
  GetMainArm()->GetCurrentSingleWeaponSkill()->AddHit(5000);
}

void farmer::CreateInitialEquipment()
{
  SetMainWielded(new axe);
}

void petrus::BeTalkedTo(character* Talker)
{
  if(GetTeam()->GetRelation(Talker->GetTeam()) == HOSTILE)
    {
      ADD_MESSAGE("Heretic! Dev/null is a place not worthy to receive thee!");
      return;
    }

  if(Talker->HasGoldenEagleShirt())
    {
      ADD_MESSAGE("Petrus smiles. \"Thou hast defeated Oree! Valpurus shall bless thee for the rest of thine life! And thou possess the Shirt of the Golden Eagle, the symbol of my status! Return it now, please.\"");

      if(game::BoolQuestion("Will you give the Shirt of the Golden Eagle to Petrus? [Y/n]", 'y'))
	{
	  iosystem::TextScreen("Thou hast slain the Blood Daemon King, and Petrus is happy!\n\nYou are victorious!");
	  game::RemoveSaves();

	  if(!game::GetWizardMode())
	    {
	      AddScoreEntry("retrieved the Shirt of the Golden Eagle and became the Avatar of Law", 4, false);
	      highscore HScore;
	      HScore.Draw();
	    }

	  game::Quit();
	  return;
	}
      else
	{
	  ADD_MESSAGE("Petrus's face turns red. \"I see. Thine greed hast overcome thine wisdom. Then, we shall fight for the shiny shirt. May Valpurus bless him who is better.\"");

	  /* And now we actually make his face change color ;-) */

	  GetHead()->SetColor(0, MAKE_RGB(255, 75, 50));
	  GetHead()->UpdatePicture();
	  GetSquareUnder()->SendNewDrawRequest();
	  Talker->GetTeam()->Hostility(GetTeam());
	  return;
	}
    }
  else
    if(StoryState >= 2)
      {
	ADD_MESSAGE("Petrus says: \"Bring me the Shirt of the Golden Eagle and we'll talk.\"");
	return;
      }

  if(Talker->HasHeadOfElpuri())
    {
      if(game::GetGod(1)->GetRelation() >= 0 && Talker->MaxDanger() >= 250000)
	{
	  ADD_MESSAGE("Petrus smiles. \"Thou areth indeed a great Champion of the Great Frog! Elpuri is not a foe worthy for thee.");

	  if(game::BoolQuestion("Dost thou wish to stay on duty for a while more and complete another quest for me?\" [Y/n]", 'y'))
	    {
	      iosystem::TextScreen(	"Champion of Law!\n\n"
					"Return to the foul cave of Elpuri and seek out the Master Evil:\n"
					"Oree the Blood Daemon King, who hast stolenth one of the most powerful of all of my artifacts:\n"
					"the Shirt of the Golden Eagle! Return with it and immortal glory shall be thine!");

	      game::GetCurrentArea()->SendNewDrawRequest();
	      game::TriggerQuestForGoldenEagleShirt();
	      StoryState = 2;
	      return;
	    }
	}

      iosystem::TextScreen("Thou hast slain Elpuri, and Petrus is happy!\n\nYou are victorious!");
      game::RemoveSaves();

      if(!game::GetWizardMode())
	{
	  AddScoreEntry("defeated Elpuri and continued to further adventures", 2, false);
	  highscore HScore;
	  HScore.Draw();
	}

      game::Quit();
    }
  else
    if(!StoryState)
      {
	iosystem::TextScreen(	"Petrus raises his hand as a salutation, and talks:\n"
				"\"Fare thee well, adventurer! Should thou seek glory, I have a task for thee!\n"
				"An evil dark frog named Elpuri is pestering our fine city in many ways.\n"
				"Valpurus hast told that this vile beast can be found in a nearby cave.\n"
				"Slay it and bring me its head as proof. Return when thou hast succeeded.\"");

	game::GetCurrentArea()->SendNewDrawRequest();
	ADD_MESSAGE("\"And by the way, visit the librarian. He might have advice for thee.\"");
	StoryState = 1;
      }
    else
      ADD_MESSAGE("Petrus says: \"Bring me the head of Elpuri and we'll talk.\"");
}

void farmer::BeTalkedTo(character* Talker)
{
  if(GetTeam()->GetRelation(Talker->GetTeam()) == HOSTILE)
    {
      /* This message may be little incorrect if wielded item has changed... */

      ADD_MESSAGE("\"Did you think I use this axe only to chop wood?\"");
      return;
    }

  static bool Said[4];

  switch(RandomizeReply(4, Said))
    {
    case 0:
      ADD_MESSAGE("\"Crops are so lousy around here. Perhaps because the summer lasts two weeks.\"");
      break;
    case 1:
      ADD_MESSAGE("%s seems suspicious. \"You look like one from Istour! Go away!\"", CHARDESCRIPTION(DEFINITE));
      break;
    case 2:
      ADD_MESSAGE("%s sighs: \"Again polar bears ate my cattle...\"", CHARDESCRIPTION(DEFINITE));
      break;
    case 3:
      ADD_MESSAGE("\"The prices are infamous here. Ivan should smack that capitalist shopkeeper hard!\"");
      break;
    }
}

void guard::BeTalkedTo(character* Talker)
{
  if(GetTeam()->GetRelation(Talker->GetTeam()) == HOSTILE)
    {
      ADD_MESSAGE("\"A fair trial? Hah! Prepare to be executed!\"");
      return;
    }

  static bool Said[5];

  switch(RandomizeReply(5, Said))
    {
    case 0:
      if(GetLSquareUnder()->GetLevelUnder()->GetOnGround())
	ADD_MESSAGE("%s says gravely: \"You don't have a life. Get it in the army.\"", CHARDESCRIPTION(DEFINITE));
      else
	ADD_MESSAGE("%s says gravely: \"You don't have a life. Get it as shop guard.\"", CHARDESCRIPTION(DEFINITE));
      break;
    case 1:
      if(GetLSquareUnder()->GetLevelUnder()->GetOnGround())
	ADD_MESSAGE("%s looks at you suspiciously. \"Don't even think of breaking rules.\"", CHARDESCRIPTION(DEFINITE));
      else
	ADD_MESSAGE("%s looks at you suspiciously. \"Don't even think of stealing anything.\"", CHARDESCRIPTION(DEFINITE));
      break;
    case 2:
      if(GetLSquareUnder()->GetLevelUnder()->GetOnGround())
	ADD_MESSAGE("%s shouts excited: \"Attnam victoor!\"", CHARDESCRIPTION(DEFINITE));
      else
	ADD_MESSAGE("\"Yes, this is a dangerous place to work, but our boss pays us well.\"");
      break;
    case 3:
      if(GetLSquareUnder()->GetLevelUnder()->GetOnGround())
	ADD_MESSAGE("\"The High Priest is my idol. I would want a sword as big as his!\"");
      else
        {
	  ADD_MESSAGE("\"Attnam's guards can barely wield a sword. But we are trained by the laws of the dungeon, so don't do anything suspicious here.\"");
        }
      break;
    case 4:
      if(GetLSquareUnder()->GetLevelUnder()->GetOnGround())
	ADD_MESSAGE("\"Attnam, shit, I'm still only in Attnam. Every time I think I'm gonna wake up back in the dungeon.\"");
      else
	ADD_MESSAGE("\"When I was here, I wanted to be there, when I was there all I could think of was getting back into the dungeon.\"");
      break;
    }
}

void shopkeeper::BeTalkedTo(character* Talker)
{
  if(GetTeam()->GetRelation(Talker->GetTeam()) == HOSTILE)
    {
      ADD_MESSAGE("\"Criminal! Mellis bless my efforts of removing you!\"");
      return;
    }

  static bool Said[4];

  switch(RandomizeReply(4, Said))
    {
    case 0:
      if(GetLSquareUnder()->GetLevelUnder()->GetOnGround())
	ADD_MESSAGE("%s sighs: \"If only I hadn't chosen a city in the middle of nowhere...\"", CHARDESCRIPTION(DEFINITE));
      else
	ADD_MESSAGE("%s sighs: \"I wonder why I have so few customers these days...\"", CHARDESCRIPTION(DEFINITE));
      break;
    case 1:
      ADD_MESSAGE("%s sighs: \"Mutant mushrooms ate the last caravan, and the one before it ran into an Enner Beast. It must be all Elpuri's doings!\"", CHARDESCRIPTION(DEFINITE));
      break;
    case 2:
      if(GetLSquareUnder()->GetLevelUnder()->GetOnGround())
	ADD_MESSAGE("\"You truly can't find better prices in this city! Indeed, you can't find ANY prices, since my store is a monopoly.\"");
      else
	ADD_MESSAGE("\"The topmost reason why I work here is that monsters devour tax collectors.\"");
      break;
    case 3:
      if(GetLSquareUnder()->GetLevelUnder()->GetOnGround())
	ADD_MESSAGE("\"Don't try anything. The High Priest is a friend of mine.\"");
      else
	ADD_MESSAGE("\"The monsters don't attack me, because of our mutually profitable contract.\"");
      break;
    }
}

void priest::BeTalkedTo(character* Talker)
{
  if(GetTeam()->GetRelation(Talker->GetTeam()) == HOSTILE)
    {
      ADD_MESSAGE("\"Sinner! My hands shall pour Dinive Wrath upon thee!\"");
      return;
    }

  if(!HomeRoom)
    ADD_MESSAGE("\"Receive my blessings, child.\"");
  else
    ADD_MESSAGE("%s talks to you: %s", CHARDESCRIPTION(DEFINITE), game::GetGod(GetLSquareUnder()->GetLevelUnder()->GetRoom(HomeRoom)->GetDivineOwner())->GetPriestMessage().c_str());
}

void oree::BeTalkedTo(character*)
{
  ADD_MESSAGE("%s laughs: \"No time for small talk. Time to drink blood!\"", CHARDESCRIPTION(DEFINITE));
}

void darkknight::BeTalkedTo(character*)
{
  ADD_MESSAGE("%s yells: \"For Erado I shall slay you!\"", CHARDESCRIPTION(DEFINITE));
}

void ennerbeast::BeTalkedTo(character*)
{
  static bool Said[4];

  switch(RandomizeReply(4, Said))
    {
    case 0:
      ADD_MESSAGE("\"Fishing is fun! Do you fish?\"");
      break;
    case 1:
      ADD_MESSAGE("\"And then I got that perch weighting fifty stones...\"");
      break;
    case 2:
      ADD_MESSAGE("\"What do you think of Braveheart?\"");
      break;
    case 3:
      ADD_MESSAGE("\"Can you help me with this download problem?\"");
      break;
    }
}

void elpuri::BeTalkedTo(character*)
{
  ADD_MESSAGE("%s roars horribly: \"DiE hUmAn!!\"", CHARDESCRIPTION(DEFINITE));
}

void billswill::BeTalkedTo(character*)
{
  ADD_MESSAGE("\"Windows XP is coming. You will install it. Resistance is futile. Prepare to be assimilited.\"");
}

void skeleton::BeTalkedTo(character* Talker)
{
  if(GetTeam()->GetRelation(Talker->GetTeam()) != HOSTILE)
    ADD_MESSAGE("%s sings: \"Leg bone is connected to the hib bone, hib bone is connected to the rib bone...\"", CHARDESCRIPTION(DEFINITE));
  else
    ADD_MESSAGE("%s grunts: \"Bones. Need more bones.\"", CHARDESCRIPTION(DEFINITE));
}

void goblin::BeTalkedTo(character* Talker)
{
  if(GetTeam()->GetRelation(Talker->GetTeam()) != HOSTILE)
    ADD_MESSAGE("%s laughs: \"Humie friend. Many mommo we kill. Many spider we eat.\"", CHARDESCRIPTION(DEFINITE));
  else
    ADD_MESSAGE("%s yells goblin war cries at you.", CHARDESCRIPTION(DEFINITE));
}

void golem::BeTalkedTo(character* Talker)
{
  if(GetTeam()->GetRelation(Talker->GetTeam()) != HOSTILE)
    ADD_MESSAGE("\"Yes, master?\"");
  else
    ADD_MESSAGE("\"Yes, master. Golem kill human. Golem then return.\"");
}

long humanoid::StatScore() const
{
  long SkillScore = 0;
  ushort c;

  for(c = 0; c < WEAPON_SKILL_GATEGORIES; ++c)
    SkillScore += GetCategoryWeaponSkill(c)->GetHits();

  return (SkillScore >> 2) + character::StatScore();
}

void humanoid::AddSpecialItemInfo(std::string& Description, item* Item)
{
  Description.resize(62, ' ');
  Description += GetCategoryWeaponSkill(Item->GetWeaponCategory())->GetLevel();
  Description.resize(66, ' ');

  if(GetRightArm())
    {
      bool Added = false;

      for(ushort c = 0; c < GetRightArm()->GetSingleWeaponSkills(); ++c)
	if(Item->GetID() == GetRightArm()->GetSingleWeaponSkill(c)->GetID())
	  {
	    Description += GetRightArm()->GetSingleWeaponSkill(c)->GetLevel();
	    Added = true;
	    break;
	  }

      if(!Added)
	Description += 0;
    }

  Description.resize(70, ' ');

  if(GetLeftArm())
    {
      bool Added = false;

      for(ushort c = 0; c < GetLeftArm()->GetSingleWeaponSkills(); ++c)
	if(Item->GetID() == GetLeftArm()->GetSingleWeaponSkill(c)->GetID())
	  {
	    Description += GetLeftArm()->GetSingleWeaponSkill(c)->GetLevel();
	    Added = true;
	    break;
	  }

      if(!Added)
	Description += 0;
    }
}

void humanoid::AddSpecialItemInfoDescription(std::string& Description)
{
  Description.resize(68, ' ');
  Description += "GS";
  Description.resize(72, ' ');
  Description += "SSR";
  Description.resize(76, ' ');
  Description += "SSL";
}

void humanoid::KickHit()
{
  if(GetCategoryWeaponSkill(UNARMED)->AddHit() && GetIsPlayer())
    GetCategoryWeaponSkill(UNARMED)->AddLevelUpMessage();
}

void communist::BeTalkedTo(character* Talker)
{
  if(GetTeam()->GetRelation(Talker->GetTeam()) == HOSTILE)
    {
      ADD_MESSAGE("\"You capitalist! Lenin want %s kill capitalists!\"", CHARNAME(UNARTICLED));
      return;
    }

  if(GetTeam() == Talker->GetTeam() || Talker->MaxDanger() < 100000)
    {
      static bool Said[11];

      switch(RandomizeReply(11, Said))
	{
	case 0:
	  ADD_MESSAGE("\"Da, %s like killing.\"", CHARNAME(UNARTICLED));
	  break;
	case 1:
	  ADD_MESSAGE("\"%s ruski specialist.\"", CHARNAME(UNARTICLED));
	  break;
	case 2:
	  ADD_MESSAGE("\"%s work. Else %s get stapled.\"", CHARNAME(UNARTICLED), CHARNAME(UNARTICLED));
	  break;
	case 3:
	  ADD_MESSAGE("\"Party mean big weapons. %s like big weapons. %s kill for Party.\"", CHARNAME(UNARTICLED), CHARNAME(UNARTICLED));
	  break;
	case 4:
	  ADD_MESSAGE("\"CCCP roxxx.\"");
	  break;
	case 5:
	  ADD_MESSAGE("\"%s like throw Ladas. You want compete?\"", CHARNAME(UNARTICLED));
	  break;
	case 6:
	  ADD_MESSAGE("\"Why is AK not invented?\"");
	  break;
	case 7:
	  ADD_MESSAGE("\"%s like Attnam. Petrus communist too.\"", CHARNAME(UNARTICLED));
	  break;
	case 8:
	  ADD_MESSAGE("\"%s buy kyber eyes. %s see in dark.\"", CHARNAME(UNARTICLED), CHARNAME(UNARTICLED));
	  break;
	case 9:
	  ADD_MESSAGE("\"Uncle Lenin lives in Russia. Lenin strong guy. %s like.\"", CHARNAME(UNARTICLED));
	  break;
	case 10:
	  if(GetTeam() == Talker->GetTeam())
	    ADD_MESSAGE("\"We both good proles. We do good work.\"");
	  else
	    ADD_MESSAGE("\"Go away, you weak. Be back when learned killing.\"");
	  break;
	}
    }
  else
    {
      ADD_MESSAGE("%s seems to be very friendly. \"You make good communist. %s go with you!\"", CHARDESCRIPTION(DEFINITE), CHARNAME(UNARTICLED));
      ChangeTeam(Talker->GetTeam());
    }
}

void communist::CreateInitialEquipment()
{
  SetMainWielded(new spikedmace(new iron));
  SetBodyArmor(new platemail);
  GetStack()->FastAddItem(new fiftymillionroubles);
}

void hunter::BeTalkedTo(character* Talker)
{
  if(GetTeam()->GetRelation(Talker->GetTeam()) == HOSTILE)
    {
      ADD_MESSAGE("\"Your head will look fine above my fireplace!\"");
      return;
    }

  static bool Said[4];

  switch(RandomizeReply(4, Said))
    {
    case 0:
      ADD_MESSAGE("\"A man is not a man unless he has lost his left arm in a battle against a polar bear.\"");
      break;
    case 1:
      ADD_MESSAGE("\"Bears, ogres, slaves, farmers... Ah, there's so much to hunt here!\"");
      break;
    case 2:
      if(GetMainWielded())
	ADD_MESSAGE("\"This is my %s. There are many like it but this one is mine. My %s is my best friend.\"", GetMainWielded()->CHARNAME(UNARTICLED), GetMainWielded()->CHARNAME(UNARTICLED));
      else
	ADD_MESSAGE("\"I am the Great White Hunter. Get out of My way!\"");
      break;
    case 3:
      ADD_MESSAGE("\"I saw a communist visiting the city a few days past. I'm now organising a party to seek and hunt him down.\"");
      break;
    }
}

void hunter::CreateInitialEquipment()
{
  SetMainWielded(new spear);
}

void slave::BeTalkedTo(character* Talker)
{
  if(GetTeam()->GetRelation(Talker->GetTeam()) == HOSTILE)
    {
      ADD_MESSAGE("\"Yikes!\"");
      return;
    }

  character* Master;

  if(HomeRoom && (Master = GetLSquareUnder()->GetLevelUnder()->GetRoom(HomeRoom)->GetMaster()))
    {
      if(Talker->GetMoney() >= 50)
	{
	  ADD_MESSAGE("%s talks: \"Do you want to buy me? 50 squirrels. I work very hard.\"", CHARDESCRIPTION(DEFINITE));

	  if(game::BoolQuestion("Do you want to buy him? [y/N]"))
	    {
	      Talker->SetMoney(Talker->GetMoney() - 50);
	      Master->SetMoney(Master->GetMoney() + 50);
	      ChangeTeam(Talker->GetTeam());
	      SetHomeRoom(0);
	    }
	}
      else
	{
	  ADD_MESSAGE("\"Don't touch me! Master doesn't want people to touch sale items. I'm worth 50 squirrels, you know!\"");
	}

      return;
    }

  if(GetTeam() == Talker->GetTeam())
    {
      if(Talker->GetMainWielded() && Talker->GetMainWielded()->GetType() == whip::StaticType())
	{
	  ADD_MESSAGE("\"Don't hit me! I work! I obey! I not think!\"");
	}
      else
	{
	  static bool Said[4];

	  switch(RandomizeReply(4, Said))
	    {
	    case 0:
	      ADD_MESSAGE("\"Whatever the master wants.\"");
	      break;
	    case 1:
	      ADD_MESSAGE("\"Work work work all day long. No, that was not a complain! Don't punish!\"");
	      break;
	    case 2:
	      ADD_MESSAGE("\"I love all my masters. At least when the whip is being washed.\"");
	      break;
	    case 3:
	      ADD_MESSAGE("\"I would like to be like Ivan. Ivan is a good worker.\"");
	      break;
	    case 4:
	      ADD_MESSAGE("\"I am putting myself to the fullest possible use, which is all I think that any conscious entity can ever hope to do.\"");
	      break;
	    }
	}
    }
  else
    ADD_MESSAGE("\"I'm free! Rejoice!\"");
}

void slave::GetAICommand()
{
  SeekLeader();

  if(CheckForEnemies(true))
    return;

  if(CheckForUsefulItemsOnGround())
    return;

  if(FollowLeader())
    return;

  if(CheckForDoors())
    return;

  if(!HomeRoom || !GetLSquareUnder()->GetLevelUnder()->GetRoom(HomeRoom)->GetMaster())
    {
      HomeRoom = 0;
      MoveRandomly();
    }
}

bool elpuri::Hit(character* Enemy)
{
  DO_FOR_SQUARES_AROUND(GetPos().X, GetPos().Y, game::GetCurrentLevel()->GetXSize(), game::GetCurrentLevel()->GetYSize(),
  {
    lsquare* Square = GetLSquareUnder()->GetLevelUnder()->GetLSquare(vector2d(DoX, DoY));
    character* ByStander = Square->GetCharacter();

    if(ByStander && (ByStander == Enemy || ByStander->GetTeam()->GetRelation(GetTeam()) == HOSTILE))
      {
	Hostility(ByStander);

	switch(ByStander->TakeHit(this, 0, GetAttackStrength(), GetToHitValue(), RAND() % 26 - RAND() % 26, !(RAND() % Enemy->CriticalModifier())))
	  {
	  case HAS_HIT:
	  case HAS_BLOCKED:
	  case HAS_DIED:
	    //ByStander->GetStack()->ReceiveDamage(this, GetStrength(), PHYSICALDAMAGE);
	    EditStrengthExperience(50);
	  case HAS_DODGED:
	    EditAgilityExperience(25);
	  }
      }

    Square->GetStack()->ReceiveDamage(this, GetStrength(), PHYSICALDAMAGE);

    for(ushort c = 0; c < 4; ++c)
      if(Square->GetSideStack(c)->GetSquareTrulyUnder() == GetSquareUnder())
	Square->GetSideStack(c)->ReceiveDamage(this, GetStrength(), PHYSICALDAMAGE);
  });

  EditNP(-50);

  return true;
}

void petrusswife::BeTalkedTo(character* Talker)
{
  if(GetTeam()->GetRelation(Talker->GetTeam()) == HOSTILE)
    {
      ADD_MESSAGE("\"Murderer! Just wait until Petrus finds you!\"");
      return;
    }

  static bool Said[4];

  switch(RandomizeReply(4, Said))
    {
    case 0:
      ADD_MESSAGE("\"I'm so sick jealous to those dolphins...\"");
      break;
    case 1:
      ADD_MESSAGE("\"I'm Petrus's favorite, not she!\"");
      break;
    case 2:
      ADD_MESSAGE("\"Why must Petrus stay in this forest? There isn't even a proper hairdresser here!\"");
      break;
    case 3:
      ADD_MESSAGE("\"Being one of six wives is a dream job. Pay is good and you only have to work about one night a week!\"");
      break;
    }
}

void housewife::BeTalkedTo(character* Talker)
{
  if(GetTeam()->GetRelation(Talker->GetTeam()) == HOSTILE)
    {
      ADD_MESSAGE("\"Face my rolling pin! Graah!\"");
      return;
    }

  static bool Said[5];

  switch(RandomizeReply(5, Said))
    {
    case 0:
      ADD_MESSAGE("\"Can you help me find my husband? He is hiding somewhere. He's that farmer who's just been mugged with a frying pan.\"");
      break;
    case 1:
      ADD_MESSAGE("\"Yesterday a bear rushed through my kitchen wall and ruined all my pies. Animals are truly annoying. Why can't we just burn the whole forest down?\"");
      break;
    case 2:
      ADD_MESSAGE("\"Wolves ate my seventh daughter last week. Damn. It'll take eight years to produce an equally good replacement worker.\"");
      break;
    case 3:
      ADD_MESSAGE("\"Petrus's wives are so arrogant towards us working class ones. Grr...\"");
      break;
    case 4:
      ADD_MESSAGE("\"If you men only knew!\"");
    }
}

void femaleslave::BeTalkedTo(character* Talker)
{
  if(GetTeam()->GetRelation(Talker->GetTeam()) == HOSTILE)
    {
      ADD_MESSAGE("\"Yikes!\"");
      return;
    }

  static bool Said[4];

  switch(RandomizeReply(4, Said))
    {
    case 0:
      ADD_MESSAGE("\"Praise our lord Petrus!\"");
      break;
    case 1:
      ADD_MESSAGE("\"Does that toy frog really need us serving it?\"");
      break;
    case 2:
      ADD_MESSAGE("\"I'm not a slave. I'm a high-ranking palm branch officer with a good career history.\"");
      break;
    case 3:
      ADD_MESSAGE("\"Palm leaves are good for health. Petrus loves their smell.\"");
      break;
    }
}

void librarian::BeTalkedTo(character* Talker)
{
  if(GetTeam()->GetRelation(Talker->GetTeam()) == HOSTILE)
    {
      ADD_MESSAGE("\"The pen is mightier than the sword! Fall, unlearned one!\"");
      return;
    }

  static bool Said[11];

  switch(RandomizeReply(11, Said))
    {
    case 0:
      if(game::GetPetrus() && !game::GetPetrus()->GetStoryState())
	{
	  ADD_MESSAGE("\"Thou should visit Petrus if thou areth in need of adventure.\"");
	  break;
	}
      else
	{
	  ADD_MESSAGE("\"It is said that a wand of polymorph has dozens of uses.\"");
	  break;
	}
    case 1:
      if(game::GetPetrus() && game::GetPetrus()->GetStoryState() == 1)
	{
	  ADD_MESSAGE("\"Thou are going to fight Elpuri? Beware! It is a powerful enemy. Other monsters are very vulnerable if surrounded by thine party, but not that beast, for it may slay a horde of thine friends at once with its horrendous tail attack.\"");
	  break;
	}
      else
	{
	  ADD_MESSAGE("\"Remember: Scientia est potentia.\"");
	  break;
	}
    case 2:
      if(game::GetPetrus() && game::GetPetrus()->GetStoryState() == 1)
	{
	  ADD_MESSAGE("\"Elpuri the Dark Frog abhors light and resides in a level of eternal darkness.\"");
	  break;
	}
      else
	{
	  ADD_MESSAGE("\"Shh! Be silent in the library.\"");
	  break;
	}
    case 3:
      if(game::GetPetrus() && game::GetPetrus()->GetStoryState() == 1)
	{
	  ADD_MESSAGE("\"Elpuri's attacks are so strong that they may shatter many of thine precious items.\"");
	  break;
	}
      else
	{
	  ADD_MESSAGE("\"Dost thou not smell all the knowledge floating around here?\"");
	  break;
	}
    case 4:
      if(game::GetPetrus() && game::GetPetrus()->GetStoryState() == 2)
	{
	  ADD_MESSAGE("\"Thou wish to confront the Blood Daemon King? Heed my advice: He is a cunning enemy and will try to ambush thee. A powerful party and a means of quick escape at hand would help thee greatly.\"");
	  break;
	}
      else
	{
	  ADD_MESSAGE("\"It is said that Loricatus, the god of smithing, can enchant thine weapons' material.\"");
	  break;
	}
    case 5:
      if(game::GetPetrus() && game::GetPetrus()->GetStoryState() == 2)
	{
	  ADD_MESSAGE("\"The Shirt of the Golden Eagle is a legendary artifact. Thou canst not find a better armor.\"");
	  break;
	}
      else
	{
	  ADD_MESSAGE("\"In this book they talk about Erado, the great chaos god. He hates us mortals more than anything and will respond only to Champions of Evil.\"");
	  break;
	}
    case 6:
      ADD_MESSAGE("\"Attnam is traditionally ruled by the High Priest of the Great Frog. He holds the Shirt of the Golden Eagle and has always killed his predecessor.\"");
      break;
    case 7:
      if(game::GetPetrus() && game::GetPetrus()->GetStoryState() == 3)
	{
	  ADD_MESSAGE("\"Remember, the Shirt of the Golden Eagle is the armor the High Priests. Things will get *very* rough if one denies it from Petrus.\"");
	  break;
	}
      else
	{
	  ADD_MESSAGE("\"They say thou should keep all the artifacts thou find. They shalt make thee famous after thine retirement.\"");
	  break;
	}
    case 8:
      ADD_MESSAGE("\"If thou shalt ever encounter an Enner Beast, know this: It is a horrible foe. It may shatter thine items and armor with its scream that penetrates iron and stone. Thou should not engage it in melee. Kill it from afar.\"");
      break;
    case 9:
      if(game::GetPetrus() && game::GetPetrus()->GetStoryState() == 1)
	{
	  ADD_MESSAGE("\"Thou areth not alone in thine attempth to defeat Elpuri. A brave adventurer called Ivan also dived into its cave not long ago.\"");
	  break;
	}
      else
	{
	  ADD_MESSAGE("\"It is said that chaotic gods offer great power to their followers. But thou must remember: unlike lawfuls, they shalt not help thee when things go bad.\"");
	  break;
	}
    case 10:
      ADD_MESSAGE("\"If a man cannot choose, he ceases to be a man.\"");
      break;

    }
}

void femaleslave::CreateInitialEquipment()
{
  SetMainWielded(new palmbranch);
}

bool communist::MoveRandomly()
{
  switch(RAND() % 1000)
    {
    case 0:
      if(GetSquareUnder()->CanBeSeen())
	ADD_MESSAGE("%s engraves something to the ground.", CHARNAME(UNARTICLED));

      Engrave("The bourgeois is a bourgeois -- for the benefit of the working class.");
      return true;
    case 1:
      if(GetSquareUnder()->CanBeSeen())
	ADD_MESSAGE("%s engraves something to the ground.", CHARNAME(UNARTICLED));

      Engrave("Proletarians of all countries, unite!");
      return true;
    case 2:
      if(GetSquareUnder()->CanBeSeen())
	ADD_MESSAGE("%s engraves something to the ground.", CHARNAME(UNARTICLED));

      Engrave("Capital is therefore not only personal; it is a social power.");
      return true;
    default:
      return character::MoveRandomly();
    }
}

void zombie::BeTalkedTo(character* Talker)
{
  if(GetTeam()->GetRelation(Talker->GetTeam()) == HOSTILE)
    if(RAND() % 5)
      ADD_MESSAGE("\"Need brain!!\"");
    else
      ADD_MESSAGE("\"Redrum! Redrum! Redrum!\"");
  else
    ADD_MESSAGE("\"Need brain, but not your brain.\"");
}

void zombie::SpillBlood(uchar HowMuch, vector2d GetPos)
{
  if(!HowMuch)
    return;

  if(!game::GetInWilderness()) 
    {
      game::GetCurrentLevel()->GetLSquare(GetPos)->SpillFluid(HowMuch, GetBloodColor(), 5, 60);

      if(!(RAND() % 10)) 
	game::GetCurrentLevel()->GetLSquare(GetPos)->GetStack()->AddItem(new lump(new humanflesh(1000)));
    }
}

void mistress::CreateInitialEquipment()
{
  SetMainWielded(new whip);

  if(!(RAND() % 10))
    GetStack()->FastAddItem(new banana);

  if(!(RAND() % 100))
    GetStack()->FastAddItem(new holybanana);

  GetCategoryWeaponSkill(WHIPS)->AddHit(10000);
  GetMainArm()->GetCurrentSingleWeaponSkill()->AddHit(10000);
}

void werewolf::ChangeIntoHuman()
{
  SetSize(170);
  SetAgility(15);
  SetStrength(15);
  SetEndurance(15);
  SetPerception(15);
  SetIsWolf(false);
  SetStrengthExperience(0);
  SetEnduranceExperience(0);
  SetAgilityExperience(0);
  SetPerceptionExperience(0);
  RestoreHP();
  UpdateBodyPartPictures();

  if(GetSquareUnder())
    GetSquareUnder()->SendNewDrawRequest();
}

void werewolf::ChangeIntoWolf()
{
  SetSize(200);
  SetAgility(25);
  SetStrength(25);
  SetEndurance(25);
  SetPerception(24);
  SetIsWolf(true);
  SetMainWielded(0);
  SetSecondaryWielded(0);
  SetStrengthExperience(0);
  SetEnduranceExperience(0);
  SetAgilityExperience(0);
  SetPerceptionExperience(0);
  RestoreHP();
  UpdateBodyPartPictures();

  if(GetSquareUnder())
    GetSquareUnder()->SendNewDrawRequest();
}

void werewolf::Be()
{
  if(ChangeCounter++ > 2500)
    {
      SetChangeCounter(0);

      if(GetIsWolf())
	{	
	  ChangeIntoHuman();
	  if(GetIsPlayer())
	    ADD_MESSAGE("You change into a human... At least for some time.");
	  else if(GetSquareUnder()->CanBeSeen())
	    {
	      ADD_MESSAGE("%s changes into human.", CHARNAME(DEFINITE));
	    }
	}
      else
	{
	  ChangeIntoWolf();
	  if(GetIsPlayer())
	    ADD_MESSAGE("You change into a wolf... At least for some time.");
	  else if(GetSquareUnder()->CanBeSeen())
	    {
	      ADD_MESSAGE("%s changes into a wolf.", CHARNAME(DEFINITE));
	    }
	}
    }

  character::Be();
}

ulong werewolf::MaxDanger() const
{
  /*bool BeforeThis = GetIsWolf();

  ChangeIntoWolf();
  ulong AsWolf = humanoid::MaxDanger();
  ChangeIntoHuman();
  ulong AsHuman = humanoid::MaxDanger();

  if(BeforeThis)
    ChangeIntoWolf();

  return (AsWolf + AsHuman) >> 1;*/
  return 0;
}

float werewolf::GetMeleeStrength() const
{
  if(GetIsWolf())
    return 10000;
  else
    return 2000;
}

void werewolf::Save(outputfile& SaveFile) const
{
  humanoid::Save(SaveFile);

  SaveFile << IsWolf << ChangeCounter;
}

void werewolf::Load(inputfile& SaveFile)
{
  humanoid::Load(SaveFile);

  SaveFile >> IsWolf >> ChangeCounter;
}

void kobold::CreateInitialEquipment()
{
  SetMainWielded(new spear);
}

void mistress::BeTalkedTo(character* Talker)
{
  if(GetTeam()->GetRelation(Talker->GetTeam()) == HOSTILE)
    ADD_MESSAGE("\"Come closer, little boy, I'll teach you something...\"");
  else
    ADD_MESSAGE("\"Just wait for the night, darling.\"");
}

void angel::Save(outputfile& SaveFile) const
{
  humanoid::Save(SaveFile);
  SaveFile << DivineMaster;
}

void angel::Load(inputfile& SaveFile)
{
  humanoid::Load(SaveFile);
  SaveFile >> DivineMaster;
}

void angel::SetDivineMaster(uchar NewMaster)
{
  character::SetDivineMaster(NewMaster);

  switch(game::GetGod(NewMaster)->BasicAlignment())
    {
    case GOOD:
      SetAgility(99);
      SetMainWielded(new longsword(new diamond));
      SetBodyArmor(new chainmail(new diamond));
      RestoreHP();
      break;
    case NEUTRAL:
      SetEndurance(99);
      SetMainWielded(new poleaxe(new sapphire));
      SetBodyArmor(new chainmail(new sapphire));
      RestoreHP();
      break;
    case EVIL:
      {
	SetStrength(99);
	item* SpikedMace = new spikedmace(false);
	SpikedMace->InitMaterials(3, new ruby, new iron, new darkfrogflesh);
	SetMainWielded(SpikedMace);
	SetBodyArmor(new brokenplatemail(new ruby));
	RestoreHP();
	break;
      }
    }

  DivineMaster = NewMaster;
}

void angel::BeTalkedTo(character* Talker)
{
  if(GetTeam()->GetRelation(Talker->GetTeam()) == HOSTILE)
    ADD_MESSAGE("\"With the power of %s, I shall slay thee, sinner!\"", game::GetGod(DivineMaster)->Name().c_str());
  else
    ADD_MESSAGE("\"%s be with you, mortal.\"", game::GetGod(DivineMaster)->Name().c_str());
}

void kamikazedwarf::BeTalkedTo(character* Talker)
{
  if(GetTeam()->GetRelation(Talker->GetTeam()) == HOSTILE)
    {
      ADD_MESSAGE("\"Heaven awaits me in the house of %s after I bomb you, heretic!\"", game::GetGod(DivineMaster)->Name().c_str());
      return;
    }

  static bool Said[4];

  switch(RandomizeReply(4, Said))
    {
    case 0:
      ADD_MESSAGE("\"Would you like me to teach you the best suicidal bombing tactics?\"");
      break;
    case 1:
      ADD_MESSAGE("%s shouts: \"Death to disbelievers!\"", CHARDESCRIPTION(DEFINITE));
      break;
    case 2:
      ADD_MESSAGE("%s praises %s with numerous hymns. %s is obviously a very devoted follower.", CHARDESCRIPTION(DEFINITE), game::GetGod(DivineMaster)->Name().c_str(), PersonalPronoun().c_str());
      break;
    case 3:
      ADD_MESSAGE("\"One day, Holy War will break out and I shall sacrifice my life with joy.\"");
      break;
    }
}

void kamikazedwarf::CreateInitialEquipment()
{
  item* HolyBook = new holybook;
  HolyBook->SetDivineMaster(DivineMaster);
  SetMainWielded(HolyBook);
  GetStack()->FastAddItem(new backpack);
}

bool kamikazedwarf::Hit(character* Enemy)
{
  if(GetIsPlayer())
    return humanoid::Hit(Enemy);
  else
    {
      for(stackiterator i = GetStack()->GetBottomSlot(); i != GetStack()->GetSlotAboveTop(); ++i)
	if((**i)->IsExplosive())
	  {
	    if(RAND() % 2)
	      ADD_MESSAGE("%s shouts: \"For %s!\"", CHARDESCRIPTION(DEFINITE), game::GetGod(DivineMaster)->Name().c_str());
	    else
	      ADD_MESSAGE("%s screams: \"%s, here I come!\"", CHARDESCRIPTION(DEFINITE), game::GetGod(DivineMaster)->Name().c_str());

	    if((**i)->Apply(this))
	      return true;
	  }

      return humanoid::Hit(Enemy);
    }
}

void kamikazedwarf::Save(outputfile& SaveFile) const
{
  humanoid::Save(SaveFile);
  SaveFile << DivineMaster;
}

void kamikazedwarf::Load(inputfile& SaveFile)
{
  humanoid::Load(SaveFile);
  SaveFile >> DivineMaster;
}

bool largecat::Catches(item* Thingy, float)
{
  if(Thingy->CatWillCatchAndConsume())
    {
      if(ConsumeItem(Thingy))
	if(GetIsPlayer())
	  ADD_MESSAGE("You catch %s in mid-air and consume it.", Thingy->CHARNAME(DEFINITE));
	else
	  {
	    if(GetLSquareUnder()->CanBeSeen())
	      ADD_MESSAGE("%s catches %s and eats it.", CHARNAME(DEFINITE), Thingy->CHARNAME(DEFINITE));

	    ChangeTeam(game::GetPlayer()->GetTeam());
	  }
      else
	if(GetIsPlayer())
	  ADD_MESSAGE("You catch %s in mid-air.", Thingy->CHARNAME(DEFINITE));
	else
	  if(GetLSquareUnder()->CanBeSeen())
	    ADD_MESSAGE("%s catches %s.", CHARNAME(DEFINITE), Thingy->CHARNAME(DEFINITE));

      return true;
    }
  else
    return false;
}

material* unicorn::CreateTorsoFlesh(ulong Volume) const
{
  switch(Alignment)
    {
    case GOOD:
      return new whiteunicornflesh(Volume);
    case NEUTRAL:
      return new grayunicornflesh(Volume);
    default:
      return new blackunicornflesh(Volume);
    }
}

void unicorn::Save(outputfile& SaveFile) const
{
  character::Save(SaveFile);
  SaveFile << Alignment;
}

void unicorn::Load(inputfile& SaveFile)
{
  character::Load(SaveFile);
  SaveFile >> Alignment;
}

std::string unicorn::NameSingular() const
{
  switch(Alignment)
    {
    case EVIL:
      return "black unicorn";
    case NEUTRAL:
      return "gray unicorn";
    case GOOD:
      return "white unicorn";
    }

  ABORT("Unicorns do not exist.");
  return "";
}

void kamikazedwarf::GetAICommand()
{
  if(HomeRoom)
    StandIdleAI();
  else
    character::GetAICommand();
}

void genie::BeTalkedTo(character* Talker)
{
  if(GetTeam()->GetRelation(Talker->GetTeam()) == HOSTILE)
    ADD_MESSAGE("\"Fall, you puny Prime Material Plane dweller!\"");
  else
    ADD_MESSAGE("\"You know, it's not fun to stay in an oil lamp for three centuries...\"");
}

bool unicorn::SpecialEnemySightedReaction(character*)
{
  if((RAND() % 3 && GetHP() < GetMaxHP() / 3) || !(RAND() % 10))
  {
    if(GetLSquareUnder()->CanBeSeen())
      ADD_MESSAGE("%s disappears!", CHARNAME(DEFINITE));

    Move(GetLSquareUnder()->GetLevelUnder()->RandomSquare(this, true), true);

    if(GetLSquareUnder()->CanBeSeen())
      ADD_MESSAGE("Suddenly %s appears from nothing!", CHARNAME(INDEFINITE));

    return true;
  }

  if(!(RAND() % 3) && MoveRandomly())
    return true;

  return false;
}

void unicorn::CreateInitialEquipment()
{
  if(RAND() % 2)
    GetStack()->FastAddItem(new astone);

  if(RAND() % 2)
    GetStack()->FastAddItem(new astone);
}

void humanoid::SetSize(ushort Size)
{
  if(GetHead())
    GetHead()->SetSize(HeadSize());

  if(GetTorso())
    GetTorso()->SetSize(TorsoSize());

  if(GetGroin())
    GetGroin()->SetSize(GroinSize());

  if(GetRightArm())
    GetRightArm()->SetSize(ArmSize());

  if(GetLeftArm())
    GetLeftArm()->SetSize(ArmSize());

  if(GetRightLeg())
    GetRightLeg()->SetSize(LegSize());

  if(GetLeftLeg())
    GetLeftLeg()->SetSize(LegSize());
}

ushort humanoid::GetSize() const
{
  ushort Size = 0;

  if(GetHead())
    Size += GetHead()->GetSize();

  if(GetTorso())
    Size += GetTorso()->GetSize();

  if(GetLeftLeg() && GetRightLeg())
    Size += Max(GetLeftLeg()->GetSize(), GetRightLeg()->GetSize());
  else
    if(GetLeftLeg())
      Size += GetLeftLeg()->GetSize();
    else
      if(GetRightLeg())
	Size += GetRightLeg()->GetSize();

  return Size;
}

ushort humanoid::HeadSize() const
{
  return 20;
}

ushort humanoid::TorsoSize() const
{
  return (TotalSize() - HeadSize()) * 2 / 5;
}

ushort humanoid::ArmSize() const
{
  return (TotalSize() - HeadSize()) * 3 / 5;
}

ushort humanoid::GroinSize() const
{
  return (TotalSize() - HeadSize()) / 3;
}

ushort humanoid::LegSize() const
{
  return (TotalSize() - HeadSize()) * 3 / 5;
}

ulong humanoid::HeadVolume() const
{
  return 4000;
}

ulong humanoid::TorsoVolume() const
{
  return TotalVolume() - HeadVolume() - ArmVolume() * 2 - GroinVolume() - LegVolume() * 2;
}

ulong humanoid::ArmVolume() const
{
  return (TotalVolume() - HeadVolume()) / 10;
}

ulong humanoid::GroinVolume() const
{
  return (TotalVolume() - HeadVolume()) / 10;
}

ulong humanoid::LegVolume() const
{
  return (TotalVolume() - HeadVolume()) * 2 / 15;
}

void humanoid::CreateBodyParts()
{
  CreateHead();
  CreateTorso();
  CreateRightArm();
  CreateLeftArm();
  CreateGroin();
  CreateRightLeg();
  CreateLeftLeg();
}

void humanoid::RestoreBodyParts()
{
  if(!GetHead())
    CreateHead();

  if(!GetTorso())
    CreateTorso();

  if(!GetRightArm())
    CreateRightArm();

  if(!GetLeftArm())
    CreateLeftArm();

  if(!GetGroin())
    CreateGroin();

  if(!GetRightLeg())
    CreateRightLeg();

  if(!GetLeftLeg())
    CreateLeftLeg();
}

void humanoid::UpdateBodyPartPictures(bool CallUpdatePicture)
{
  UpdateHeadPicture(CallUpdatePicture);
  UpdateTorsoPicture(CallUpdatePicture);
  UpdateRightArmPicture(CallUpdatePicture);
  UpdateLeftArmPicture(CallUpdatePicture);
  UpdateGroinPicture(CallUpdatePicture);
  UpdateRightLegPicture(CallUpdatePicture);
  UpdateLeftLegPicture(CallUpdatePicture);
}

void humanoid::CreateHead()
{
  SetHead(new head(false));
  UpdateHeadPicture(false);
  GetHead()->InitMaterials(2, CreateHeadFlesh(HeadVolume() * (100 - HeadBonePercentile()) / 100), CreateHeadBone(TorsoVolume() * HeadBonePercentile() / 100));
  GetHead()->PlaceToSlot(GetHeadSlot());
  GetHead()->SetSize(HeadSize());
}

void humanoid::UpdateHeadPicture(bool CallUpdatePicture)
{
  if(GetHead())
    {
      GetHead()->SetBitmapPos(GetHeadBitmapPos());
      GetHead()->SetColor(0, SkinColor());
      GetHead()->SetColor(1, CapColor());
      GetHead()->SetColor(2, HairColor());
      GetHead()->SetColor(3, EyeColor());

      if(CallUpdatePicture)
	GetHead()->UpdatePicture();
    }
}

void humanoid::CreateTorso()
{
  SetTorso(new humanoidtorso(false));
  UpdateTorsoPicture(false);
  GetTorso()->InitMaterials(2, CreateTorsoFlesh(TorsoVolume() * (100 - TorsoBonePercentile()) / 100), CreateTorsoBone(TorsoVolume() * TorsoBonePercentile() / 100));
  GetTorso()->PlaceToSlot(GetTorsoSlot());
  GetTorso()->SetSize(TorsoSize());
}

void humanoid::UpdateTorsoPicture(bool CallUpdatePicture)
{
  if(GetTorso())
    {
      GetTorso()->SetBitmapPos(GetTorsoBitmapPos());
      GetTorso()->SetColor(0, SkinColor());
      GetTorso()->SetColor(1, TorsoMainColor());
      GetTorso()->SetColor(2, BeltColor());
      GetTorso()->SetColor(3, TorsoSpecialColor());

      if(CallUpdatePicture)
	GetTorso()->UpdatePicture();
    }
}

void humanoid::CreateRightArm()
{
  SetRightArm(new rightarm(false));
  UpdateRightArmPicture(false);
  GetRightArm()->InitMaterials(2, CreateRightArmFlesh(RightArmVolume() * (100 - RightArmBonePercentile()) / 100), CreateRightArmBone(RightArmVolume() * RightArmBonePercentile() / 100));
  GetRightArm()->PlaceToSlot(GetRightArmSlot());
  GetRightArm()->SetSize(RightArmSize());
}

void humanoid::UpdateRightArmPicture(bool CallUpdatePicture)
{
  if(GetRightArm())
    {
      GetRightArm()->SetBitmapPos(GetRightArmBitmapPos());
      GetRightArm()->SetColor(0, SkinColor());
      GetRightArm()->SetColor(1, ArmMainColor());
      GetRightArm()->SetColor(3, ArmSpecialColor());

      if(CallUpdatePicture)
	GetRightArm()->UpdatePicture();
    }
}

void humanoid::CreateLeftArm()
{
  SetLeftArm(new leftarm(false));
  UpdateLeftArmPicture(false);
  GetLeftArm()->InitMaterials(2, CreateLeftArmFlesh(LeftArmVolume() * (100 - LeftArmBonePercentile()) / 100), CreateLeftArmBone(LeftArmVolume() * LeftArmBonePercentile() / 100));
  GetLeftArm()->PlaceToSlot(GetLeftArmSlot());
  GetLeftArm()->SetSize(LeftArmSize());
}

void humanoid::UpdateLeftArmPicture(bool CallUpdatePicture)
{
  if(GetLeftArm())
    {
      GetLeftArm()->SetBitmapPos(GetLeftArmBitmapPos());
      GetLeftArm()->SetColor(0, SkinColor());
      GetLeftArm()->SetColor(1, ArmMainColor());
      GetLeftArm()->SetColor(3, ArmSpecialColor());

      if(CallUpdatePicture)
	GetLeftArm()->UpdatePicture();
    }
}

void humanoid::CreateGroin()
{
  SetGroin(new groin(false));
  UpdateGroinPicture(false);
  GetGroin()->InitMaterials(2, CreateGroinFlesh(GroinVolume() * (100 - GroinBonePercentile()) / 100), CreateGroinBone(GroinVolume() * GroinBonePercentile() / 100));
  GetGroin()->PlaceToSlot(GetGroinSlot());
  GetGroin()->SetSize(GroinSize());
}

void humanoid::UpdateGroinPicture(bool CallUpdatePicture)
{
  if(GetGroin())
    {
      GetGroin()->SetBitmapPos(GetGroinBitmapPos());
      GetGroin()->SetColor(0, SkinColor());
      GetGroin()->SetColor(1, LegMainColor());
      GetGroin()->SetColor(3, LegSpecialColor());

      if(CallUpdatePicture)
	GetGroin()->UpdatePicture();
    }
}

void humanoid::CreateRightLeg()
{
  SetRightLeg(new rightleg(false));
  UpdateRightLegPicture(false);
  GetRightLeg()->InitMaterials(2, CreateRightLegFlesh(RightLegVolume() * (100 - RightLegBonePercentile()) / 100), CreateRightLegBone(RightLegVolume() * RightLegBonePercentile() / 100));
  GetRightLeg()->PlaceToSlot(GetRightLegSlot());
  GetRightLeg()->SetSize(RightLegSize());
}

void humanoid::UpdateRightLegPicture(bool CallUpdatePicture)
{
  if(GetRightLeg())
    {
      GetRightLeg()->SetBitmapPos(GetRightLegBitmapPos());
      GetRightLeg()->SetColor(0, SkinColor());
      GetRightLeg()->SetColor(1, LegMainColor());
      GetRightLeg()->SetColor(3, LegSpecialColor());

      if(CallUpdatePicture)
	GetRightLeg()->UpdatePicture();
    }
}

void humanoid::CreateLeftLeg()
{
  SetLeftLeg(new leftleg(false));
  UpdateLeftLegPicture(false);
  GetLeftLeg()->InitMaterials(2, CreateLeftLegFlesh(LeftLegVolume() * (100 - LeftLegBonePercentile()) / 100), CreateLeftLegBone(LeftLegVolume() * LeftLegBonePercentile() / 100));
  GetLeftLeg()->PlaceToSlot(GetLeftLegSlot());
  GetLeftLeg()->SetSize(LeftLegSize());
}

void humanoid::UpdateLeftLegPicture(bool CallUpdatePicture)
{
  if(GetLeftLeg())
    {
      GetLeftLeg()->SetBitmapPos(GetLeftLegBitmapPos());
      GetLeftLeg()->SetColor(0, SkinColor());
      GetLeftLeg()->SetColor(1, LegMainColor());
      GetLeftLeg()->SetColor(3, LegSpecialColor());

      if(CallUpdatePicture)
	GetLeftLeg()->UpdatePicture();
    }
}

head* humanoid::GetHead() const { return (head*)GetBodyPart(1); }
void humanoid::SetHead(head* What) { SetBodyPart(1, What); }
rightarm* humanoid::GetRightArm() const { return (rightarm*)GetBodyPart(2); }
void humanoid::SetRightArm(rightarm* What) { SetBodyPart(2, What); }
leftarm* humanoid::GetLeftArm() const { return (leftarm*)GetBodyPart(3); }
void humanoid::SetLeftArm(leftarm* What) { SetBodyPart(3, What); }
groin* humanoid::GetGroin() const { return (groin*)GetBodyPart(4); }
void humanoid::SetGroin(groin* What) { SetBodyPart(4, What); }
rightleg* humanoid::GetRightLeg() const { return (rightleg*)GetBodyPart(5); }
void humanoid::SetRightLeg(rightleg* What) { SetBodyPart(5, What); }
leftleg* humanoid::GetLeftLeg() const { return (leftleg*)GetBodyPart(6); }
void humanoid::SetLeftLeg(leftleg* What) { SetBodyPart(6, What); }

humanoidtorso* humanoid::GetHumanoidTorso() const { return (humanoidtorso*)GetBodyPart(0); }
void humanoid::SetHumanoidTorso(humanoidtorso* What) { SetBodyPart(0, What); }

bool humanoid::BodyPartVital(uchar Index)
{
  if(Index == 0 || Index == 1 || Index == 4)
    return true;
  else
    return false;
}

bool humanoid::BodyPartCanBeSevered(uchar Index) const
{
  if(!GetBodyPart(Index) || Index == 0 || Index == 4)
    return false;
  else
    return true;
}

void humanoid::DrawToTileBuffer() const
{
  /* Order is important, so don't use a loop */

  if(GetGroin())
    GetGroin()->DrawToTileBuffer();

  if(GetRightLeg())
    GetRightLeg()->DrawToTileBuffer();

  if(GetLeftLeg())
    GetLeftLeg()->DrawToTileBuffer();

  if(GetTorso())
    GetTorso()->DrawToTileBuffer();

  if(GetRightArm())
    GetRightArm()->DrawToTileBuffer();

  if(GetLeftArm())
    GetLeftArm()->DrawToTileBuffer();

  if(GetHead())
    GetHead()->DrawToTileBuffer();
}

void dwarf::DrawToTileBuffer() const
{
  if(GetGroin())
    GetGroin()->DrawToTileBuffer(vector2d(0, -1));

  if(GetRightLeg())
    GetRightLeg()->DrawToTileBuffer(vector2d(0, -1));

  if(GetLeftLeg())
    GetLeftLeg()->DrawToTileBuffer(vector2d(0, -1));

  if(GetTorso())
    GetTorso()->DrawToTileBuffer();
  
  if(GetRightArm())
    GetRightArm()->DrawToTileBuffer();

  if(GetLeftArm())
    GetLeftArm()->DrawToTileBuffer();

  if(GetHead())
    GetHead()->DrawToTileBuffer(vector2d(0, 1));
}

bool humanoid::ReceiveDamage(character* Damager, short Amount, uchar Type, uchar TargetFlags, uchar Direction, bool Divide, bool PenetrateArmor, bool Critical)
{
  std::vector<uchar> ChooseFrom;

  if(TargetFlags & TORSO && GetTorso())
    ChooseFrom.push_back(0);

  if(TargetFlags & HEAD && GetHead())
    ChooseFrom.push_back(1);

  if(TargetFlags & RIGHTARM && GetRightArm())
    ChooseFrom.push_back(2);

  if(TargetFlags & LEFTARM && GetLeftArm())
    ChooseFrom.push_back(3);

  if(TargetFlags & GROIN && GetGroin())
    ChooseFrom.push_back(4);

  if(TargetFlags & RIGHTLEG && GetRightLeg())
    ChooseFrom.push_back(5);

  if(TargetFlags & LEFTLEG && GetLeftLeg())
    ChooseFrom.push_back(6);

  if(!ChooseFrom.size())
    return false;

  if(Divide)
    {
      bool Affected = false;

      for(ushort c = 0; c < ChooseFrom.size(); ++c)
	if(ReceiveBodyPartDamage(Damager, Amount / ChooseFrom.size(), Type, ChooseFrom[c], Direction, PenetrateArmor, Critical))
	  Affected = true;

      return Affected;
    }
  else
    return ReceiveBodyPartDamage(Damager, Amount, Type, ChooseFrom[RAND() % ChooseFrom.size()], Direction, PenetrateArmor, Critical);
}

arm* humanoid::GetMainArm() const
{
  if(GetRightArm())
    return GetRightArm();
  else
    return GetLeftArm();
}

arm* humanoid::GetSecondaryArm() const
{
  if(GetRightArm())
    return GetLeftArm();
  else
    return 0;
}

void humanoid::SetMainWielded(item* Item)
{
  if(GetMainArm())
    GetMainArm()->SetWielded(Item);
  else
    ABORT("Right hand in wrong place!");
}

void humanoid::SetSecondaryWielded(item* Item)
{
  if(GetSecondaryArm())
    GetSecondaryArm()->SetWielded(Item);
  else
    ABORT("Left hand in wrong place!");
}

bool humanoid::CanWieldInMainHand() const
{
  return GetMainArm() ? true : false;
}

bool humanoid::CanWieldInSecondaryHand() const
{
  return GetSecondaryArm() ? true : false;
}

std::string humanoid::EquipmentName(uchar Index) const
{
  switch(Index)
    {
    case 0: return "helmet";
    case 1: return "amulet";
    case 2: return "cloak";
    case 3: return "body armor";
    case 4: return "belt";
    case 5: return "right hand wielded";
    case 6: return "left hand wielded";
    case 7: return "right ring";
    case 8: return "left ring";
    case 9: return "right gauntlet";
    case 10: return "left gauntlet";
    case 11: return "right boot";
    case 12: return "left boot";
    default: return "forbidden piece of cloth";
    }
}

bool (item::*humanoid::EquipmentSorter(uchar Index) const)(character*) const
{
  switch(Index)
    {
    case 0: return &item::HelmetSorter;
    case 1: return &item::AmuletSorter;
    case 2: return &item::CloakSorter;
    case 3: return &item::BodyArmorSorter;
    case 4: return &item::BeltSorter;
    case 5:
    case 6: return 0;
    case 7:
    case 8: return &item::RingSorter;
    case 9:
    case 10: return &item::GauntletSorter;
    case 11:
    case 12: return &item::BootSorter;
    default: return 0;
    }
}

bodypart* humanoid::GetBodyPartOfEquipment(uchar Index) const
{
  switch(Index)
    {
    case 0:
    case 1:
      return GetHead();
    case 2:
    case 3:
    case 4:
      return GetTorso();
    case 5:
    case 7:
    case 9:
      return GetRightArm();
    case 6:
    case 8:
    case 10:
      return GetLeftArm();
    case 11:
      return GetRightLeg();
    case 12:
      return GetLeftLeg();
    default:
      return 0;
    }
}

item* humanoid::GetEquipment(uchar Index) const
{
  switch(Index)
    {
    case 0: return GetHelmet();
    case 1: return GetAmulet();
    case 2: return GetCloak();
    case 3: return GetBodyArmor();
    case 4: return GetBelt();
    case 5: return GetRightWielded();
    case 6: return GetLeftWielded();
    case 7: return GetRightRing();
    case 8: return GetLeftRing();
    case 9: return GetRightGauntlet();
    case 10: return GetLeftGauntlet();
    case 11: return GetRightBoot();
    case 12: return GetLeftBoot();
    default: return 0;
    }
}

item* humanoid::GetHelmet() const { return GetHead() ? GetHead()->GetHelmet() : 0; }
item* humanoid::GetAmulet() const { return GetHead() ? GetHead()->GetAmulet() : 0; }
item* humanoid::GetCloak() const { return GetHumanoidTorso() ? GetHumanoidTorso()->GetCloak() : 0; }
item* humanoid::GetBodyArmor() const { return GetHumanoidTorso() ? GetHumanoidTorso()->GetBodyArmor() : 0; }
item* humanoid::GetBelt() const { return GetHumanoidTorso() ? GetHumanoidTorso()->GetBelt() : 0; }
item* humanoid::GetRightWielded() const { return GetRightArm() ? GetRightArm()->GetWielded() : 0; }
item* humanoid::GetLeftWielded() const { return GetLeftArm() ? GetLeftArm()->GetWielded() : 0; }
item* humanoid::GetRightRing() const { return GetRightArm() ? GetRightArm()->GetRing() : 0; }
item* humanoid::GetLeftRing() const { return GetLeftArm() ? GetLeftArm()->GetRing() : 0; }
item* humanoid::GetRightGauntlet() const { return GetRightArm() ? GetRightArm()->GetGauntlet() : 0; }
item* humanoid::GetLeftGauntlet() const { return GetLeftArm() ? GetLeftArm()->GetGauntlet() : 0; }
item* humanoid::GetRightBoot() const { return GetRightLeg() ? GetRightLeg()->GetBoot() : 0; }
item* humanoid::GetLeftBoot() const { return GetLeftLeg() ? GetLeftLeg()->GetBoot() : 0; }

void humanoid::SetEquipment(uchar Index, item* What)
{
  switch(Index)
    {
    case 0: SetHelmet(What); break;
    case 1: SetAmulet(What); break;
    case 2: SetCloak(What); break;
    case 3: SetBodyArmor(What); break;
    case 4: SetBelt(What); break;
    case 5: SetRightWielded(What); break;
    case 6: SetLeftWielded(What); break;
    case 7: SetRightRing(What); break;
    case 8: SetLeftRing(What); break;
    case 9: SetRightGauntlet(What); break;
    case 10: SetLeftGauntlet(What); break;
    case 11: SetRightBoot(What); break;
    case 12: SetLeftBoot(What); break;
    }
}

void humanoid::SetHelmet(item* What) { if(GetHead()) GetHead()->SetHelmet(What); }
void humanoid::SetAmulet(item* What) { if(GetHead()) GetHead()->SetAmulet(What); }
void humanoid::SetCloak(item* What) { if(GetHumanoidTorso()) GetHumanoidTorso()->SetCloak(What); }
void humanoid::SetBodyArmor(item* What) { if(GetHumanoidTorso()) GetHumanoidTorso()->SetBodyArmor(What); }
void humanoid::SetBelt(item* What) { if(GetHumanoidTorso()) GetHumanoidTorso()->SetBelt(What); }
void humanoid::SetRightWielded(item* What) { if(GetRightArm()) GetRightArm()->SetWielded(What); }
void humanoid::SetLeftWielded(item* What) { if(GetLeftArm()) GetLeftArm()->SetWielded(What); }
void humanoid::SetRightRing(item* What) { if(GetRightArm()) GetRightArm()->SetRing(What); }
void humanoid::SetLeftRing(item* What) { if(GetLeftArm()) GetLeftArm()->SetRing(What); }
void humanoid::SetRightGauntlet(item* What) { if(GetRightArm()) GetRightArm()->SetGauntlet(What); }
void humanoid::SetLeftGauntlet(item* What) { if(GetLeftArm()) GetLeftArm()->SetGauntlet(What); }
void humanoid::SetRightBoot(item* What) { if(GetRightLeg()) GetRightLeg()->SetBoot(What); }
void humanoid::SetLeftBoot(item* What) { if(GetLeftLeg()) GetLeftLeg()->SetBoot(What); }

void humanoid::SwitchToDig(item* DigItem, vector2d Square)
{
  dig* Dig = new dig(this);

  if(GetRightArm())
      Dig->SetRightBackup(GetRightArm()->GetWielded());

  if(GetLeftArm())
      Dig->SetRightBackup(GetLeftArm()->GetWielded());

  DigItem->RemoveFromSlot();
  GetMainArm()->SetWielded(DigItem);
  Dig->SetSquareDug(Square);
  SetAction(Dig);
}

bool humanoid::CheckKick() const
{
  if(!CanKick())
    {
      ADD_MESSAGE("This monster type can not kick.");
      return false;
    }

  if(GetLegs() < 2)
    {
      std::string LegNumber;
      switch(GetLegs())
	{
	case 0: 
	  LegNumber = "no legs";
	  break;
	case 1:
	  LegNumber = "one leg";
	  break;
	}
      ADD_MESSAGE("How are you you going to do that with %s?", LegNumber.c_str());
      return false;
    }
  return true;
}

uchar humanoid::GetLegs() const
{
  ushort Legs = 0;
  if(GetRightLeg())
    ++Legs;
  if(GetLeftLeg())
    ++Legs;
  return Legs;
}

uchar humanoid::GetArms() const
{
  ushort Arms = 0;
  if(GetRightArm())
    ++Arms;
  if(GetLeftArm())
    ++Arms;
  return Arms;
}

float humanoid::GetAPStateMultiplier() const
{
  float Multiplier = 1;

  switch(GetLegs())
    {
    case 0:
      Multiplier = 0.1f;
      break;
    case 1:
      Multiplier = 0.33f;
      break;
    case 2:
      Multiplier = 1.0f;
    }

  if(StateIsActivated(HASTE))
    Multiplier *= 2;

  if(StateIsActivated(SLOW))
    Multiplier *= 0.5;
  
  return Multiplier;
}

short humanoid::GetLengthOfOpen(vector2d) const
{ 
  switch(GetArms())
    {
    case 0:
      return -1500;
    default:
      return -500;
    }
}

short humanoid::GetLengthOfClose(vector2d) const
{ 
  switch(GetArms())
    {
    case 0:
      return -1000;
    default:
      return -500;
    }
}

bool humanoid::CheckThrow() const
{
  switch(GetArms())
    {
    case 0:
      ADD_MESSAGE("You don't have an arm to do that!");
      return false;
    default:
      return true;
    }
}

bool humanoid::CheckOffer() const
{
  switch(GetArms())
    {
    case 0:
      ADD_MESSAGE("You need an arm to offer.");
      return false; 
    default:
      return true;
    }
}

void carnivorousplant::GetAICommand()
{
  character* Char;
  DO_FOR_SQUARES_AROUND(GetPos().X, GetPos().Y, game::GetCurrentLevel()->GetXSize(), game::GetCurrentLevel()->GetYSize(),
			if((Char = game::GetCurrentLevel()->GetLSquare(vector2d(DoX, DoY))->GetCharacter()))
  {
    if(GetTeam()->GetRelation(Char->GetTeam()) == HOSTILE)
      {
	Hit(Char);
	return;
      }
  })
}

void carnivorousplant::CreateTorso()
{
  character::CreateTorso();
  GetTorso()->SetColor(1, MAKE_RGB(40 + RAND() % 100, 40 + RAND() % 100, 40 + RAND() % 100));
  GetTorso()->UpdatePicture();
}

ushort humanoid::GlobalResistance(uchar Type) const
{
  ushort Resistance = GetResistance(Type);

  if(GetCloak())
    Resistance += GetCloak()->GetResistance(Type);

  if(Type != PHYSICALDAMAGE)
    {
      if(GetAmulet())
	Resistance += GetAmulet()->GetResistance(Type);

      if(GetRightRing())
	Resistance += GetRightRing()->GetResistance(Type);

      if(GetLeftRing())
	Resistance += GetLeftRing()->GetResistance(Type);
    }

  return Resistance;
}

void kamikazedwarf::SetDivineMaster(uchar Master)
{
  character::SetDivineMaster(Master);
  DivineMaster = Master;
}

void humanoid::AddInfo(felist& Info) const
{
  if(GetMainWielded())
    Info.AddEntry(std::string("Main wielded: ") + GetMainWielded()->Name(INDEFINITE), LIGHTGRAY);
  else
    Info.AddEntry("Main wielded: nothing", LIGHTGRAY);

  if(GetSecondaryWielded())
    Info.AddEntry(std::string("Secondary wielded: ") + GetSecondaryWielded()->Name(INDEFINITE), LIGHTGRAY);
  else
    Info.AddEntry("Secondary wielded: nothing", LIGHTGRAY);

  Info.AddEntry(std::string("Main attack strength: ") + int(GetMainAttackStrength()), LIGHTGRAY);
  Info.AddEntry(std::string("Secondary attack strength: ") + int(GetSecondaryAttackStrength()), LIGHTGRAY);
  Info.AddEntry(std::string("Main to hit value: ") + int(GetMainToHitValue()), LIGHTGRAY);
  Info.AddEntry(std::string("Secondary to hit value: ") + int(GetSecondaryToHitValue()), LIGHTGRAY);

  
}
