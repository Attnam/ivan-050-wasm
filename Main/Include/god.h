/*
 *
 *  Iter Vehemens ad Necem 
 *  Copyright (C) Timo Kiviluoto
 *  Released under GNU General Public License
 *
 *  See LICENSING which should included with 
 *  this file for more details
 *
 */

#ifndef __GOD_H__
#define __GOD_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include "typedef.h"

class item;
class character;
class outputfile;
class inputfile;
class festring;
class god;
class liquid;
class team;

class godprototype
{
 public:
  godprototype(god* (*)(bool), const char*);
  god* Clone() const { return Cloner(false); }
  god* CloneAndLoad(inputfile&) const;
  const char* GetClassID() const { return ClassID; }
  int GetIndex() const { return Index; }
 private:
  int Index;
  god* (*Cloner)(bool);
  const char* ClassID;
};

class god
{
 public:
  typedef godprototype prototype;
  god();
  virtual void Pray();
  virtual const char* GetName() const = 0;
  virtual const char* GetDescription() const = 0;
  virtual int GetAlignment() const = 0;
  festring GetCompleteDescription() const;
  void ApplyDivineTick();
  void AdjustRelation(god*, int, bool);
  void AdjustRelation(int);
  void AdjustTimer(long);
  void Save(outputfile&) const;
  void Load(inputfile&);
  void SetRelation(int Value) { Relation = Value; }
  void SetTimer(long Value) { Timer = Value; }
  bool ReceiveOffer(item*);
  virtual int GetBasicAlignment() const;
  int GetRelation() const { return Relation; }
  void PrintRelation() const;
  void SetIsKnown(bool What) { Known = What; }
  bool IsKnown() const { return Known; }
  void PlayerKickedAltar() { AdjustRelation(-100); }
  void PlayerKickedFriendsAltar() { AdjustRelation(-50); }
  virtual bool PlayerVomitedOnAltar(liquid*);
  character* CreateAngel(team*, int = 0);
  virtual color16 GetColor() const = 0;
  virtual color16 GetEliteColor() const = 0;
  virtual const prototype* GetProtoType() const = 0;
  int GetType() const { return GetProtoType()->GetIndex(); }
 protected:
  virtual void PrayGoodEffect() = 0;
  virtual void PrayBadEffect() = 0;
  int Relation;
  long Timer;
  bool Known;
};

#ifdef __FILE_OF_STATIC_GOD_PROTOTYPE_DEFINITIONS__
#define GOD_PROTOTYPE(name)\
god* name##_Clone(bool) { return new name; }\
godprototype name##_ProtoType(&name##_Clone, #name);\
const godprototype* name::GetProtoType() const { return &name##_ProtoType; }
#else
#define GOD_PROTOTYPE(name)
#endif

#define GOD(name, base, data)\
\
name : public base\
{\
 public:\
  virtual const prototype* GetProtoType() const;\
  data\
}; GOD_PROTOTYPE(name);

#endif
