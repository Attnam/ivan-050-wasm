#include <cstdio>

#include "god.h"
#include "level.h"
#include "lsquare.h"
#include "charde.h"
#include "itemde.h"
#include "stack.h"
#include "message.h"
#include "script.h"
#include "lterrade.h"
#include "team.h"
#include "femath.h"

void god::Pray()
{
  if(!Timer)
    if(Relation > -(RAND() % 501))
      {
	ADD_MESSAGE("You feel %s is pleased.", GOD_NAME);
	PrayGoodEffect();
	AdjustTimer(5000);
	AdjustRelation(50);
	game::ApplyDivineAlignmentBonuses(this, true);

	if(Relation > 500 && !(RAND() % 100))
	  {
	    character* Angel = CreateAngel();

	    if(Angel)
	      {
		Angel->SetTeam(game::GetPlayer()->GetTeam());
		ADD_MESSAGE("%s seems to be very friendly towards you.", Angel->CNAME(DEFINITE));
	      }
	  }
      }
    else
      {
	ADD_MESSAGE("You feel %s is displeased today.", GOD_NAME);
	PrayBadEffect();
	AdjustTimer(10000);
	game::ApplyDivineAlignmentBonuses(this, false);
      }
  else
    if(Relation >= RAND() % 501)
      {
	ADD_MESSAGE("You feel %s is displeased, but helps you anyway.", GOD_NAME);
	PrayGoodEffect();
	AdjustTimer(25000);
	AdjustRelation(-50);
	game::ApplyDivineAlignmentBonuses(this, false);
      }
    else
      {
	ADD_MESSAGE("You feel %s is angry.", GOD_NAME);
	PrayBadEffect();
	AdjustTimer(50000);
	AdjustRelation(-100);
	game::ApplyDivineAlignmentBonuses(this, false);

	if(Relation < -500 && !(RAND() % 50))
	  {
	    character* Angel = CreateAngel();

	    if(Angel)
	      {
		Angel->SetTeam(game::GetTeam(5));
		ADD_MESSAGE("%s seems to be hostile.", Angel->CNAME(DEFINITE));
	      }
	  }
      }
}

std::string god::CompleteDescription() const
{
  std::string Desc = game::CAlignment(Alignment());

  Desc.resize(4, ' ');

  Desc += Name();

  Desc.resize(30, ' ');

  if(game::GetWizardMode())
    {
      char Buffer[256];

      sprintf(Buffer, "%d - %d", int(Timer), int(Relation));

      return Desc + Buffer;
    }

  return Desc + "the " + Description();
}

void god::PrayGoodEffect()
{
  ADD_MESSAGE("%s doesn't know what good he could do to you for now.", GOD_NAME);
}

void god::PrayBadEffect()
{
  ADD_MESSAGE("%s doesn't know how to punish at the moment.", GOD_NAME);
}

void god::AdjustRelation(god* Competitor, bool Good, short Multiplier)
{
  short Adjustment = 2 * Multiplier - abs((schar)(Alignment()) - Competitor->Alignment()) * Multiplier;

  if(!Good && Adjustment > 0)
    Adjustment = -Adjustment;

  AdjustRelation(Adjustment);
}

void god::AdjustRelation(short Amount)
{
  Relation += Amount;

  if(Relation < -1000)
    Relation = -1000;

  if(Relation > 1000)
    Relation = 1000;
}

void god::AdjustTimer(long Amount)
{
  Timer += Amount;

  if(Timer < 0)
    Timer = 0;

  if(Timer > 1000000000)
    Timer = 1000000000;
}

void consummo::PrayGoodEffect()
{
  ADD_MESSAGE("Suddenly, the fabric of space experiences an unnaturaly powerful quantum displacement!");
  ADD_MESSAGE("You teleport away!");

  game::GetPlayer()->Move(game::GetCurrentLevel()->RandomSquare(game::GetPlayer(), true), true);
}

void consummo::PrayBadEffect()
{
  ADD_MESSAGE("Suddenly, the fabric of space experiences an unnaturaly powerful quantum displacement!");
  ADD_MESSAGE("Some parts of you teleport away!");

  game::GetPlayer()->SetHP(game::GetPlayer()->GetHP() - RAND() % game::GetPlayer()->GetMaxHP());
  game::GetPlayer()->CheckDeath(std::string("shattered to pieces by the wrath of ") + Name());
}

void valpurus::PrayGoodEffect()
{
  ADD_MESSAGE("You hear booming voice: \"DEFEAT ERADO WITH THIS, MY PALADIN!\"");
  ADD_MESSAGE("A sword glittering with holy might appears from nothing.");

  game::GetPlayer()->GetGiftStack()->AddItem(new curvedtwohandedsword(new valpurium));
}

void valpurus::PrayBadEffect()
{
  ADD_MESSAGE("Valpurus smites you with a small hammer.");
  game::GetPlayer()->SetHP(game::GetPlayer()->GetHP() - 5);
  game::GetPlayer()->CheckDeath(std::string("faced the hammer of Justice from the hand of ") + Name());
}

void venius::PrayGoodEffect()
{
  ADD_MESSAGE("A booming voice echoes: \"Xunil! Xunil! Save us!\"");
  ADD_MESSAGE("A huge firestorm engulfs everything around you.");

  game::GetCurrentLevel()->Explosion(game::GetPlayer(), "killed accidentally by " + Name(), game::GetPlayer()->GetPos(), 40, false);
}

void venius::PrayBadEffect()
{
  ADD_MESSAGE("%s casts a beam of horrible, yet righteous, fire on you.", GOD_NAME);
  game::GetPlayer()->ReceiveFireDamage(game::GetPlayer(), "killed accidentally by " + Name(), 20);
  game::GetPlayer()->CheckDeath(std::string("burned to death by the wrath of ") + Name());
}

void dulcis::PrayGoodEffect()
{
  ADD_MESSAGE("A beautiful melody echoes around you.");
  DO_FOR_SQUARES_AROUND(game::GetPlayer()->GetPos().X, game::GetPlayer()->GetPos().Y, game::GetCurrentLevel()->GetXSize(), game::GetCurrentLevel()->GetYSize(),
  {
    character* Char = game::GetCurrentLevel()->GetLevelSquare(vector2d(DoX, DoY))->GetCharacter();

    if(Char)
      if(Char->Charmable())
	if(Char->CurrentDanger() * 2 < game::GetPlayer()->MaxDanger())
	  {
	    if(Char->GetTeam() == game::GetPlayer()->GetTeam())
	      ADD_MESSAGE("%s seems to be very happy.", Char->CNAME(DEFINITE));
	    else if(Char->GetTeam()->GetRelation(game::GetPlayer()->GetTeam()) == HOSTILE)
	      ADD_MESSAGE("%s stops fighting.", Char->CNAME(DEFINITE));
	    else
	      ADD_MESSAGE("%s seems to be very friendly towards you.", Char->CNAME(DEFINITE));

	    Char->ChangeTeam(game::GetPlayer()->GetTeam());
	  }
	else
	  ADD_MESSAGE("%s resists its charming call.", Char->CNAME(DEFINITE));
      else
	ADD_MESSAGE("%s seems not affected.", Char->CNAME(DEFINITE));
  });
}

void dulcis::PrayBadEffect()
{
  ADD_MESSAGE("%s plays a horrible tune that rots your brain.", GOD_NAME);
  game::GetPlayer()->SetHP(game::GetPlayer()->GetHP() - RAND() % 9 - 1);
  game::GetPlayer()->CheckDeath(std::string("became insane by listening ") + Name() + " too much");
}

void inasnum::PrayGoodEffect()
{
  ADD_MESSAGE("%s gives you a hint.", GOD_NAME);

  switch(RAND() % 7)
    {
    case 0:
      ADD_MESSAGE("His eye was found in a mug.");
      break;
    case 1:
      ADD_MESSAGE("Master Evil, the Pepsi Daemon King, has escapeth not upwards but downwards.");
      ADD_MESSAGE("In the pits of dev/null he carrieth a great foe of the sword.");
      break;
    case 2:
      ADD_MESSAGE("Thou shalt not hit thine lesser brethren with a biggest thing you can find.");
      break;
    case 3:
      ADD_MESSAGE("If thy hast fought but not conquered thine foe thy must lick thine wounds in peace.");
      break;
    case 4:
      ADD_MESSAGE("School... No, no... Its right behind me!!! Nooo... Why is it coming into my dreams?");
      break;
    case 5:
      ADD_MESSAGE("Thou shalt not hurry in thine killing.");
      break;
    case 6:
      ADD_MESSAGE("If thy shall eat frogs, thy will be foul.");
      break;
    default:
      ADD_MESSAGE("He created me from the fire to destroy all gods who oppose him.");
    }
}

void inasnum::PrayBadEffect()
{
  ADD_MESSAGE("%s gives you a hint.", GOD_NAME);
  switch(RAND() % 3)
    {
    case 0:
      ADD_MESSAGE("Dancing in front of Bill's SWAT professional will calm him down.");
      break;
    case 1:
      ADD_MESSAGE("Writing \"Elbereth\" to the ground will make you safe.");
      break;
    default:
      ADD_MESSAGE("Engraving \"29392172387Fda3419080418012838\" will give thou a bag of money.");
    }
}

void seges::PrayGoodEffect()
{
  ADD_MESSAGE("Your stomach feels full again.");

  if(game::GetPlayer()->GetNP() < 10000)
    game::GetPlayer()->SetNP(10000);
}

void seges::PrayBadEffect()
{
  ADD_MESSAGE("You feel Seges altering the contents of your stomach in an eerie way.");

  game::GetPlayer()->EditNP(-1000);

  if(game::GetPlayer()->GetNP() < 1)
    game::GetPlayer()->CheckStarvationDeath(std::string("starved by ") + Name());
}

void atavus::PrayGoodEffect()
{
  ADD_MESSAGE("A mithril platemail materializes before you.");
  item* Reward = new platemail(false);
  Reward->InitMaterials(new mithril);
  game::GetPlayer()->GetGiftStack()->AddItem(Reward);
}

void atavus::PrayBadEffect()
{
  ADD_MESSAGE("You have not been good the whole year.");

  if(game::GetPlayer()->GetStack()->GetItems())
    {
      ushort ToBeDeleted = RAND() % game::GetPlayer()->GetStack()->GetItems();
      item* Disappearing = game::GetPlayer()->GetStack()->GetItem(ToBeDeleted);

      if(Disappearing->Destroyable())
	{
	  ADD_MESSAGE("Your %s disappears.", Disappearing->CNAME(UNARTICLED));
	  game::GetPlayer()->GetStack()->RemoveItem(ToBeDeleted);
	  if(game::GetPlayer()->GetWielded() == Disappearing) game::GetPlayer()->SetWielded(0);
	  if(game::GetPlayer()->GetTorsoArmor() == Disappearing) game::GetPlayer()->SetTorsoArmor(0);
	  delete Disappearing;
	}
      else
	{
	  ADD_MESSAGE("%s tries to remove your %s, but fails.", GOD_NAME, Disappearing->CNAME(UNARTICLED));
	  ADD_MESSAGE("You feel you are not so gifted anymore.");
	  game::GetPlayer()->SetAgility(game::GetPlayer()->GetAgility() - 1);
	  game::GetPlayer()->SetStrength(game::GetPlayer()->GetStrength() - 1);
	  game::GetPlayer()->SetEndurance(game::GetPlayer()->GetEndurance() - 1);
	}
    }
  else
    {
      ADD_MESSAGE("You feel you are not so gifted anymore.");
      game::GetPlayer()->SetAgility(game::GetPlayer()->GetAgility() - 1);
      game::GetPlayer()->SetStrength(game::GetPlayer()->GetStrength() - 1);
      game::GetPlayer()->SetEndurance(game::GetPlayer()->GetEndurance() - 1);
    }
}

void silva::PrayGoodEffect()
{
  if(!*game::GetCurrentLevel()->GetLevelScript()->GetOnGround())
    {
      ADD_MESSAGE("Suddenly a horrible earthquake shakes the level.");

      uchar c, Tunnels = 2 + RAND() % 3;

      for(c = 0; c < Tunnels; ++c)
	game::GetCurrentLevel()->AttachPos(game::GetCurrentLevel()->RandomSquare(0, false));

      uchar ToEmpty = 10 + RAND() % 11;

      for(c = 0; c < ToEmpty; ++c)
	for(ushort i = 0; i < 50; ++i)
	  {
	    vector2d Pos = game::GetCurrentLevel()->RandomSquare(0, false);
	    bool Correct = false;

	    DO_FOR_SQUARES_AROUND(Pos.X, Pos.Y, game::GetCurrentLevel()->GetXSize(), game::GetCurrentLevel()->GetYSize(),
	    {
	      if(game::GetCurrentLevel()->GetLevelSquare(vector2d(DoX, DoY))->GetOverTerrain()->GetIsWalkable())
		{
		  Correct = true;
		  break;
		}
	    });

	    if(Correct)
	      {
		game::GetCurrentLevel()->GetLevelSquare(Pos)->ChangeOverLevelTerrain(new empty);

		for(uchar p = 0; p < 4; ++p)
		  game::GetCurrentLevel()->GetLevelSquare(Pos)->GetSideStack(p)->Clean();

		break;
	      }
	  }

      uchar ToGround = 20 + RAND() % 21;

      for(c = 0; c < ToGround; ++c)
	for(ushort i = 0; i < 50; ++i)
	  {
	    vector2d Pos = game::GetCurrentLevel()->RandomSquare(0, true, RAND() % 2 ? true : false);

	    character* Char = game::GetCurrentLevel()->GetLevelSquare(Pos)->GetCharacter();

	    if(game::GetCurrentLevel()->GetLevelSquare(Pos)->GetOverLevelTerrain()->GetType() != empty::StaticType() || (Char && (Char->GetIsPlayer() || Char->GetTeam()->GetRelation(game::GetPlayer()->GetTeam()) != HOSTILE)))
	      continue;

	    uchar Walkables = 0;

	    DO_FOR_SQUARES_AROUND(Pos.X, Pos.Y, game::GetCurrentLevel()->GetXSize(), game::GetCurrentLevel()->GetYSize(),
	    {
	      if(game::GetCurrentLevel()->GetLevelSquare(vector2d(DoX, DoY))->GetOverTerrain()->GetIsWalkable())
		++Walkables;
	    });

	    if(Walkables > 6)
	      {
		game::GetCurrentLevel()->GetLevelSquare(Pos)->ChangeOverLevelTerrain(new earth);

		if(Char)
		  {
		    if(Char->GetSquareUnder()->CanBeSeen())
		      ADD_MESSAGE("%s is hit by a brick of earth falling from the roof!", Char->CNAME(DEFINITE));

		    Char->SetHP(Char->GetHP() - 20 - RAND() % 21);
		    Char->CheckDeath("killed by an earthquake");
		  }

		game::GetCurrentLevel()->GetLevelSquare(Pos)->KickAnyoneStandingHereAway();

		ushort p;

		for(p = 0; p < 4; ++p)
		  game::GetCurrentLevel()->GetLevelSquare(Pos)->GetSideStack(p)->Clean();

		for(p = 0; p < game::GetCurrentLevel()->GetLevelSquare(Pos)->GetStack()->GetItems();)
		  if(!game::GetCurrentLevel()->GetLevelSquare(Pos)->GetStack()->GetItem(p)->ImpactDamage(0xFFFF, game::GetCurrentLevel()->GetLevelSquare(Pos)->CanBeSeen(), game::GetCurrentLevel()->GetLevelSquare(Pos)->GetStack()))
		    ++p;

		break;
	      }
	  }
    }
  else
    {
      uchar Created = 0;

      DO_FOR_SQUARES_AROUND(game::GetPlayer()->GetPos().X, game::GetPlayer()->GetPos().Y, game::GetCurrentLevel()->GetXSize(), game::GetCurrentLevel()->GetYSize(),
      {
	wolf* Wolf = new wolf;

	if(game::GetCurrentLevel()->GetLevelSquare(vector2d(DoX, DoY))->GetIsWalkable(Wolf) && !game::GetCurrentLevel()->GetLevelSquare(vector2d(DoX, DoY))->GetCharacter())
	  {
	    Wolf->SetTeam(game::GetPlayer()->GetTeam());
	    game::GetCurrentLevel()->GetLevelSquare(vector2d(DoX, DoY))->AddCharacter(Wolf);
	    ++Created;
	  }
	else
	  delete Wolf;
      });

      if(!Created)
	ADD_MESSAGE("You hear a sad howling of a wolf inprisoned in the earth.");

      if(Created == 1)
	ADD_MESSAGE("Suddenly a tame wolf materializes beside you.");

      if(Created > 1)
	ADD_MESSAGE("Suddenly some tame wolves materialize around you.");
    }
}

void silva::PrayBadEffect()
{
  switch(RAND() % 300 / 100)
    {
    case 0:
      game::GetPlayer()->Polymorph(new spider(true, true, false), 1000 + RAND() % 1000);
      break;
    case 1:
      game::GetPlayer()->Polymorph(new donkey(true, true, false), 1000 + RAND() % 1000);
      break;
    case 2:
      game::GetPlayer()->Polymorph(new jackal(true, true, false), 1000 + RAND() % 1000);
      break;
    }
}

void loricatus::PrayGoodEffect()
{
  for(ushort c = 0; c < game::GetPlayer()->GetStack()->GetItems(); c++)
    {
      item* Old = game::GetPlayer()->GetStack()->GetItem(c);

      if(Old->GetType() == brokenplatemail::StaticType())
	{
	  bool Worn = false, Wielded = false;

	  if(Old == game::GetPlayer()->GetTorsoArmor())
	    Worn = true;
	  else
	    if(Old == game::GetPlayer()->GetWielded())
	      Wielded = true;

	  game::GetPlayer()->GetStack()->RemoveItem(c);
	  ADD_MESSAGE("Loricatus fixes your %s.", Old->CNAME(UNARTICLED));
	  Old->SetExists(false);
	  item* Plate = new platemail(false);
	  Plate->InitMaterials(Old->GetMaterial(0));
	  Old->PreserveMaterial(0);
	  game::GetPlayer()->GetStack()->AddItem(Plate);

	  if(Worn)
	    game::GetPlayer()->SetTorsoArmor(Plate);
	  else
	    if(Wielded)
	      game::GetPlayer()->SetWielded(Plate);

	  return;
	}
    }
	
  if(game::GetPlayer()->GetWielded())
    if(game::GetPlayer()->GetWielded()->IsMaterialChangeable())
      {
	std::string OldName = game::GetPlayer()->GetWielded()->Name(UNARTICLED);
	game::GetPlayer()->GetWielded()->SetMaterial(0, new mithril(game::GetPlayer()->GetWielded()->GetMaterial(0)->GetVolume()));
	ADD_MESSAGE("Your %s changes into %s.", OldName.c_str(), game::GetPlayer()->GetWielded()->CNAME(INDEFINITE));
	return;
      }
    else
      ADD_MESSAGE("%s glows in a strange light but remains unchanged.", game::GetPlayer()->GetWielded()->CNAME(DEFINITE));
  else
    ADD_MESSAGE("You feel a slight tingling in your hands.");
}

void loricatus::PrayBadEffect()
{
  std::string OldName;

  if(game::GetPlayer()->GetWielded())
    if(game::GetPlayer()->GetWielded()->IsMaterialChangeable())
      {
	OldName = game::GetPlayer()->GetWielded()->Name(UNARTICLED);
	game::GetPlayer()->GetWielded()->ChangeMaterial(0, new bananaflesh(game::GetPlayer()->GetWielded()->GetMaterial(0)->GetVolume()));
	ADD_MESSAGE("Your %s changes into %s.", OldName.c_str(), game::GetPlayer()->GetWielded()->CNAME(INDEFINITE));
      }
    else
      ADD_MESSAGE("%s glows in a strange light but remain unchanged.", game::GetPlayer()->GetWielded()->CNAME(DEFINITE));
  else
    ADD_MESSAGE("You feel a slight tingling in your hands.");
}

void calamus::PrayGoodEffect()
{
  ADD_MESSAGE("%s gives you the talent for speed.", GOD_NAME);
  game::GetPlayer()->SetAgility(game::GetPlayer()->GetAgility() + 1);
}

void calamus::PrayBadEffect()
{
  if(game::GetPlayer()->GetAgility() > 5)
    {
      ADD_MESSAGE("%s slows you down.", GOD_NAME);
      game::GetPlayer()->SetAgility(game::GetPlayer()->GetAgility() - 1);
    }
  else
    ADD_MESSAGE("Suprisingly you feel nothing.");
}

void god::Save(outputfile& SaveFile) const
{
  SaveFile << Relation << Timer << Known;
}

void god::Load(inputfile& SaveFile)
{
  SaveFile >> Relation >> Timer >> Known;
}

void erado::PrayGoodEffect()
{
  ADD_MESSAGE("The air vibrates violently around you.");
  ADD_MESSAGE("A terrible undead voice echoes through the caverns:");
  ADD_MESSAGE("\"SlAvE! ThOu HaSt PlAeSeD mE! lIfT tHiNe ReWaRd, ChAmPiOn!\"");
  ADD_MESSAGE("A heavy weapon of pure corruption materializes before you.");

  game::GetPlayer()->GetGiftStack()->AddItem(new neercseulb);
}

void erado::PrayBadEffect()
{
  ADD_MESSAGE("A dark, booming voice shakes the area:");
  ADD_MESSAGE("\"PuNy MoRtAl! YoU aRe NoT wOrThY! i ShAlL DeStRoY yOu LiKe EvErYoNe ElSe!\"");
  ADD_MESSAGE("A bolt of black energy hits you.");

  game::GetPlayer()->SetHP(game::GetPlayer()->GetHP() - game::GetPlayer()->GetMaxHP() + 1);
  game::GetPlayer()->SetAgility(game::GetPlayer()->GetAgility() - 1);
  game::GetPlayer()->SetStrength(game::GetPlayer()->GetStrength() - 1);
  game::GetPlayer()->SetEndurance(game::GetPlayer()->GetEndurance() - 1);
  game::GetPlayer()->CheckDeath(std::string("obliterated by the unholy power of ") + Name());
}

void mellis::PrayGoodEffect()
{
  bool Success = false;
  ushort JustCreated;
  item* NewVersion;

  if(game::GetPlayer()->GetStack()->GetItems())
    {
      ADD_MESSAGE("%s tries to trade some of your items into better ones.", GOD_NAME);
      bool Cont = true;

      while(Cont)
	{
	  Cont = false;

	  for(ushort c = 0; c < game::GetPlayer()->GetStack()->GetItems(); ++c)
	    if(game::GetPlayer()->GetStack()->GetItem(c))
	      {
		NewVersion = game::GetPlayer()->GetStack()->GetItem(c)->BetterVersion();

		if(NewVersion)
		  {
		    item* ToBeDeleted = game::GetPlayer()->GetStack()->GetItem(c);
		    game::GetPlayer()->GetStack()->RemoveItem(c);
		    JustCreated = game::GetPlayer()->GetStack()->AddItem(NewVersion);
		    Success = true;
		    ADD_MESSAGE("%s manages to trade %s into %s.", GOD_NAME, ToBeDeleted->CNAME(DEFINITE), game::GetPlayer()->GetStack()->GetItem(JustCreated)->CNAME(INDEFINITE));
		    if(ToBeDeleted == game::GetPlayer()->GetWielded()) game::GetPlayer()->SetWielded(0);
		    if(ToBeDeleted == game::GetPlayer()->GetTorsoArmor()) game::GetPlayer()->SetTorsoArmor(0);
		    delete ToBeDeleted;
		    Cont = true;
		    break;
		  }
	      }
	}
    }

  if(!Success)
    ADD_MESSAGE("You have no good items for trade.");
}

void mellis::PrayBadEffect()
{
  for(ushort c = 1;; ++c)
    if(game::GetGod(c))
      {
	if(game::GetGod(c) != this)
	  game::GetGod(c)->AdjustRelation(-100);
      }
    else
      break;

  ADD_MESSAGE("%s spreads bad rumours about you to other gods.", GOD_NAME);
}

void pestifer::PrayGoodEffect()
{
  ADD_MESSAGE("Suddenly a very ugly head appears beside you, groaning horribly into your ear!");
  game::GetPlayer()->GetGiftStack()->AddItem(new headofennerbeast);
  game::GetPlayer()->SetEndurance(game::GetPlayer()->GetEndurance() + 1);
}

void pestifer::PrayBadEffect()
{
  character* EnnerBeast = new ennerbeast;
  EnnerBeast->SetTeam(game::GetTeam(4));
  game::GetCurrentLevel()->GetLevelSquare(game::GetCurrentLevel()->RandomSquare(EnnerBeast, true))->AddCharacter(EnnerBeast);
  ADD_MESSAGE("You hear the roaring of a new enner beast!");
}

void valpurus::Pray()
{
  if(!Timer && Relation == 1000)
    {
      ADD_MESSAGE("You feel %s is very pleased.", GOD_NAME);
      PrayGoodEffect();
      AdjustTimer(100000);
      AdjustRelation(-500);
      game::ApplyDivineAlignmentBonuses(this, true);

      if(Relation > 500 && !(RAND() % 5))
	{
	  character* Angel = CreateAngel();

	  if(Angel)
	    {
	      Angel->SetTeam(game::GetPlayer()->GetTeam());
	      ADD_MESSAGE("%s seems to be very friendly towards you.", Angel->CNAME(DEFINITE));
	    }
	}
    }
  else
    {
      ADD_MESSAGE("You feel you are not yet worthy enough for %s.", GOD_NAME);
      PrayBadEffect();
      AdjustTimer(50000);
      AdjustRelation(-100);
      game::ApplyDivineAlignmentBonuses(this, false);

      if(Relation < -500 && !(RAND() % 25))
	{
	  character* Angel = CreateAngel();

	  if(Angel)
	    {
	      Angel->SetTeam(game::GetTeam(5));
	      ADD_MESSAGE("%s seems to be hostile.", Angel->CNAME(DEFINITE));
	    }
	}
    }
}

void atavus::Pray()
{
  if(!Timer && Relation > 500 + RAND() % 500)
    {
      ADD_MESSAGE("You feel %s is pleased.", GOD_NAME);
      PrayGoodEffect();
      AdjustTimer(50000);
      AdjustRelation(-250);
      game::ApplyDivineAlignmentBonuses(this, true);

      if(Relation > 500 && !(RAND() % 5))
	{
	  character* Angel = CreateAngel();

	  if(Angel)
	    {
	      Angel->SetTeam(game::GetPlayer()->GetTeam());
	      ADD_MESSAGE("%s seems to be very friendly towards you.", Angel->CNAME(DEFINITE));
	    }
	}
    }
  else
    {
      ADD_MESSAGE("You feel you are not yet worthy enough for %s.", GOD_NAME);
      PrayBadEffect();
      AdjustTimer(50000);
      AdjustRelation(-100);
      game::ApplyDivineAlignmentBonuses(this, false);

      if(Relation < -500 && !(RAND() % 25))
	{
	  character* Angel = CreateAngel();

	  if(Angel)
	    {
	      Angel->SetTeam(game::GetTeam(5));
	      ADD_MESSAGE("%s seems to be hostile.", Angel->CNAME(DEFINITE));
	    }
	}
    }
}

void erado::Pray()
{
  if(!Timer && Relation == 1000)
    {
      ADD_MESSAGE("You feel %s is very pleased.", GOD_NAME);
      PrayGoodEffect();
      AdjustTimer(100000);
      AdjustRelation(-500);
      game::ApplyDivineAlignmentBonuses(this, true);

      if(Relation > 500 && !(RAND() % 5))
	{
	  character* Angel = CreateAngel();

	  if(Angel)
	    {
	      Angel->SetTeam(game::GetPlayer()->GetTeam());
	      ADD_MESSAGE("%s seems to be very friendly towards you.", Angel->CNAME(DEFINITE));
	    }
	}
    }
  else
    {
      ADD_MESSAGE("You feel you are not yet worthy enough for %s.", GOD_NAME);
      PrayBadEffect();
      AdjustTimer(50000);
      AdjustRelation(-100);
      game::ApplyDivineAlignmentBonuses(this, false);

      if(Relation < -500 && !(RAND() % 5))
	{
	  character* Angel = CreateAngel();

	  if(Angel)
	    {
	      Angel->SetTeam(game::GetTeam(5));
	      ADD_MESSAGE("%s seems to be hostile.", Angel->CNAME(DEFINITE));
	    }
	}
    }
}

void infuscor::PrayBadEffect()
{
  ADD_MESSAGE("Vile and evil knowledge pulps into your brain. It's too much for it to handle; you faint.");
  game::GetPlayer()->Faint();
}

void macellarius::PrayGoodEffect()
{
  ADD_MESSAGE("%s wishes you to have fun with this potion.", GOD_NAME);
  item* Reward = new potion(false);
  Reward->InitMaterials(2, new glass, new omleurine);
  game::GetPlayer()->GetGiftStack()->AddItem(Reward);
  ADD_MESSAGE("%s drops on the ground.", Reward->CNAME(DEFINITE));
}

void macellarius::PrayBadEffect()
{
  ADD_MESSAGE("A potion drops on your head and shatters into small bits.");
  game::GetPlayer()->SetHP(game::GetPlayer()->GetHP() - RAND() % 7);
  game::GetPlayer()->GetLevelSquareUnder()->GetStack()->AddItem(new brokenbottle);
  game::GetPlayer()->CheckDeath(std::string("killed while enjoying the company of ") + Name());
}

void scabies::PrayGoodEffect()
{
  ADD_MESSAGE("Five cans full of school food drop from somewhere above!");

  for(uchar c = 0; c < 5; ++c)
    {
      item* Reward = new can(false);
      Reward->InitMaterials(2, new iron, new schoolfood);
      game::GetPlayer()->GetGiftStack()->AddItem(Reward);
    }
}

void scabies::PrayBadEffect()
{
  ADD_MESSAGE("%s makes you eat a LOT of school food.", GOD_NAME);

  for(uchar c = 0; c < 5; ++c)
    {
      material* SchoolFood = new schoolfood(600);
      SchoolFood->EatEffect(game::GetPlayer(), 600);
      delete SchoolFood;
    }

  ADD_MESSAGE("You feel your muscles softening terribly...");

  game::GetPlayer()->SetStrength(game::GetPlayer()->GetStrength() - 1);
  game::GetPlayer()->SetAgility(game::GetPlayer()->GetAgility() - 1);
}

void infuscor::PrayGoodEffect()
{
  ADD_MESSAGE("Suddenly five scrolls appear from nothing!");

  for(uchar c = 0; c < 5; ++c)
    game::GetPlayer()->GetGiftStack()->AddItem(new scrollofteleport);
}

void cruentus::PrayGoodEffect()
{
  ADD_MESSAGE("Cruentus recommends you to its master, Erado.");
  game::GetGod(16)->AdjustRelation(100);
}

void cruentus::PrayBadEffect()
{
  item* ToBe;

  ToBe = game::GetPlayer()->GetWielded();

  if(ToBe && !ToBe->Destroyable())
    ADD_MESSAGE("%s tries to destroy your weapon, but fails.", GOD_NAME);

  if(ToBe && ToBe->Destroyable())
    {
      ADD_MESSAGE("%s destroys your weapon.", GOD_NAME);

      game::GetPlayer()->GetStack()->RemoveItem(game::GetPlayer()->GetStack()->SearchItem(ToBe));
      game::GetPlayer()->SetWielded(0);
      delete ToBe;
    }
  else
    {
      ADD_MESSAGE("%s gets mad and hits you!", GOD_NAME);
      game::GetPlayer()->SetHP(game::GetPlayer()->GetHP() - RAND() % 20);
      game::GetPlayer()->CheckDeath(std::string("destroyed by ") + Name());
    }
}

void cruentus::Pray()
{
  if(!Timer && Relation > 500 + RAND() % 500)
    {
      ADD_MESSAGE("You feel %s is pleased.", GOD_NAME);
      PrayGoodEffect();
      AdjustTimer(50000);
      AdjustRelation(-250);
      game::ApplyDivineAlignmentBonuses(this, true);

      if(Relation > 500 && !(RAND() % 10))
	{
	  character* Angel = CreateAngel();

	  if(Angel)
	    {
	      Angel->SetTeam(game::GetPlayer()->GetTeam());
	      ADD_MESSAGE("%s seems to be very friendly towards you.", Angel->CNAME(DEFINITE));
	    }
	}
    }
  else
    {
      ADD_MESSAGE("You feel you are not yet worthy enough for %s.", GOD_NAME);
      PrayBadEffect();
      AdjustTimer(50000);
      AdjustRelation(-100);
      game::ApplyDivineAlignmentBonuses(this, false);

      if(Relation < -500 && !(RAND() % 10))
	{
	  character* Angel = CreateAngel();

	  if(Angel)
	    {
	      Angel->SetTeam(game::GetTeam(5));
	      ADD_MESSAGE("%s seems to be hostile.", Angel->CNAME(DEFINITE));
	    }
	}
    }
}

bool god::ReceiveOffer(item* Sacrifice)
{
  short OfferValue = Sacrifice->CalculateOfferValue(BasicAlignment());

  if(abs(OfferValue) > 5)
    {
      if(!Sacrifice->Destroyable())
	{
	  ADD_MESSAGE("%s is too important for you to be sacrificed.", Sacrifice->CNAME(DEFINITE));

	  return false;
	}

      AdjustRelation(OfferValue);
      game::ApplyDivineAlignmentBonuses(this, OfferValue > 0 ? true : false);

      if(OfferValue > 0)
	ADD_MESSAGE("%s thanks you for your gift.", GOD_NAME);
      else
	ADD_MESSAGE("%s seems not to appreciate your gift at all.", GOD_NAME);

      PrintRelation();

      if(OfferValue > 0 && Relation > 500 && !(RAND() % 100))
	{
	  character* Angel = CreateAngel();

	  if(Angel)
	    {
	      Angel->SetTeam(game::GetPlayer()->GetTeam());
	      ADD_MESSAGE("%s seems to be very friendly towards you.", Angel->CNAME(DEFINITE));
	    }
	}

      return true;
    }
  else
    {
      ADD_MESSAGE("Nothing happens.");
      return false;
    }
}

void god::PrintRelation() const
{
  std::string VerbalRelation;

  if(GetRelation() == 1000)
    VerbalRelation = "greets you as a Champion of the Cause!";
  else if(GetRelation() > 750)
    VerbalRelation = "is extremely pleased.";
  else if(GetRelation() > 250)
    VerbalRelation = "is very pleased.";
  else if(GetRelation() > 50)
    VerbalRelation = "is pleased.";
  else if(GetRelation() > -50)
    VerbalRelation = "is content.";
  else if(GetRelation() > -250)
    VerbalRelation = "is angry.";
  else if(GetRelation() > -750)
    VerbalRelation = "is very angry.";
  else if(GetRelation() > -1000)
    VerbalRelation = "is extremely angry.";
  else VerbalRelation = "hates you more than any other mortal.";

  ADD_MESSAGE("%s %s", GOD_NAME, VerbalRelation.c_str());
}

void god::AddPriestMessage() const
{
  ADD_MESSAGE("\"Not currently implemented.\"");
}

void venius::AddPriestMessage() const
{
  ADD_MESSAGE("\"%s is the Great Protector of all Law and Order.", GOD_NAME);
  ADD_MESSAGE("Prayeth upon, He may burn thy enemies with the Fire of Justice, if thou areth worthy.\"");
}

void dulcis::AddPriestMessage() const
{
  ADD_MESSAGE("\"%s is the Creator of everything that we call Art and Beauty.", GOD_NAME);
  ADD_MESSAGE("When thou pray for Her help, She may calm thine worst enemies with Her love.");
  ADD_MESSAGE("But beware! There areth some villains that may resist even Her call!\"");
}

void seges::AddPriestMessage() const
{
  ADD_MESSAGE("\"%s brings Life, Health and Nutrition to all who follow Her.", GOD_NAME);
  ADD_MESSAGE("When thou call upon Her with an empty stomach, a miracle may indeed fill it.\"");
}

void consummo::AddPriestMessage() const
{
  ADD_MESSAGE("\"The Wise bow before %s, for He maketh the Universe as rational as it is.", GOD_NAME);
  ADD_MESSAGE("Those who follow Him are not bound to space and time, since knowledge controls them.");
  ADD_MESSAGE("This is why those chosen by Him may escape any danger with their wisdom.");
  ADD_MESSAGE("Alas, beware! Soon thou may find thyself in an even worse situation!\"");
}

void god::PlayerVomitedOnAltar()
{
  ADD_MESSAGE("The vomit drops on the altar, but then suddenly gravity changes its direction.");
  ADD_MESSAGE("The vomit lands on your face.");
  AdjustRelation(-200);
  game::GetPlayer()->SetHP(game::GetPlayer()->GetHP() - 1 - RAND() % 2);
  game::GetPlayer()->CheckDeath("chocked to death by own vomit");

  if(!(RAND() % 50))
    {
      character* Angel = CreateAngel();

      if(Angel)
	{
	  Angel->SetTeam(game::GetTeam(5));
	  ADD_MESSAGE("%s seems to be hostile.", Angel->CNAME(DEFINITE));
	}
    }
}

void scabies::PlayerVomitedOnAltar()
{
  ADD_MESSAGE("%s feels that you are indeed her follower.", GOD_NAME); 
  AdjustRelation(1);
}

void valpurus::AddPriestMessage() const
{
  ADD_MESSAGE("\"%s the Great Frog is the highest of all gods.", GOD_NAME);
  ADD_MESSAGE("In the ancient times the World and its deities were born from His eggs.");
  ADD_MESSAGE("That's why this cathedral and the whole city of Attnam is dedicated to His worship.\"");
  ADD_MESSAGE("\"In thine prayers thou must understand He is a busy god who knows His importance.");
  ADD_MESSAGE("He will not help newbies. Pray Him only when He calls thee a Champion!\"");
}

character* god::CreateAngel()
{
  vector2d TryToCreate;

  for(uchar c = 0; c < 100; ++c)
    {
      TryToCreate = game::GetPlayer()->GetPos() + game::GetMoveVector(RAND() % DIRECTION_COMMAND_KEYS);

      angel* Angel = new angel;

      if(game::IsValidPos(TryToCreate) && game::GetCurrentLevel()->GetLevelSquare(TryToCreate)->GetIsWalkable(Angel) && game::GetCurrentLevel()->GetLevelSquare(TryToCreate)->GetCharacter() == 0)
	{
	  /* This is a most unitelligent gum solution, but... */

	  for(uchar c = 1; game::GetGod(c); ++c)
	    if(game::GetGod(c) == this)
	      Angel->SetMaster(c);

	  game::GetCurrentLevel()->GetLevelSquare(TryToCreate)->AddCharacter(Angel);
	  ADD_MESSAGE("Suddenly %s appears!", Angel->CNAME(INDEFINITE));
	  return Angel;
	}
      else
	delete Angel;
    }

  return 0;
}
