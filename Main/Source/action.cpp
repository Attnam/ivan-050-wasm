/*
 *
 *  Iter Vehemens ad Necem 
 *  Copyright (C) Timo Kiviluoto
 *  See LICENSING which should included with this file
 *
 */

/* Compiled through actset.cpp */

actionprototype::actionprototype(action* (*Cloner)(bool), const char* ClassID) : Cloner(Cloner), ClassID(ClassID) { Index = protocontainer<action>::Add(this); }

action::action(donothing) : Actor(0), Flags(0) { }
action::~action() { }

void action::Terminate(bool)
{
  GetActor()->SetAction(0);
  delete this;
}

void action::Save(outputfile& SaveFile) const
{
  SaveFile << (ushort)GetType() << Flags;
}

void action::Load(inputfile& SaveFile)
{
  SaveFile >> Flags;
}

action* actionprototype::CloneAndLoad(inputfile& SaveFile) const
{
  action* Action = Cloner(true);
  Action->Load(SaveFile);
  return Action;
}
