/*
 *
 *  Iter Vehemens ad Necem 
 *  Copyright (C) Timo Kiviluoto
 *  See LICENSING which should included with this file
 *
 */

#ifndef __TEAM_H__
#define __TEAM_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include <vector>
#include <list>
#include <map>

#include "typedef.h"

class outputfile;
class inputfile;
class character;

typedef std::vector<character*> charactervector;

class team
{
 public:
  team();
  team(ulong);
  void SetRelation(team*, int);
  int GetRelation(const team*) const;
  void Hostility(team*);
  ulong GetID() const { return ID; }
  void SetID(ulong What) { ID = What; }
  void Save(outputfile&) const;
  void Load(inputfile&);
  void SetLeader(character* What) { Leader = What; }
  character* GetLeader() const { return Leader; }
  std::list<character*>::iterator Add(character*);
  void Remove(std::list<character*>::iterator);
  const std::list<character*>& GetMember() const { return Member; }
  int GetKillEvilness() const { return KillEvilness; }
  void SetKillEvilness(int What) { KillEvilness = What; }
  bool HasEnemy() const;
  int GetMembers() const { return Member.size(); }
  int GetEnabledMembers() const;
  void MoveMembersTo(charactervector&);
 private:
  character* Leader;
  std::map<ulong, int> Relation;
  std::list<character*> Member;
  ulong ID;
  int KillEvilness;
};

outputfile& operator<<(outputfile&, const team*);
inputfile& operator>>(inputfile&, team*&);

#endif
