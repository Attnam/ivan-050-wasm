#ifndef __SLOT_H__
#define __SLOT_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include <list>

#include "typedef.h"

class item;
class stack;
class character;
class outputfile;
class inputfile;
class stack;
class bodypart;
class action;
class square;

class slot
{
 public:
  slot() : Item(0) { }
  virtual void Empty() = 0;
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  item* GetItem() const { return Item; }
  item* operator->() const { return Item; }
  item* operator*() const { return Item; }
  virtual void AddFriendItem(item*) const = 0;
  virtual bool IsOnGround() const { return false; }
  virtual void PutInItem(item*);
  virtual square* GetSquareUnder() const = 0;
  virtual void SignalVolumeAndWeightChange() = 0;
  virtual void SignalEmitationIncrease(ushort) = 0;
  virtual void SignalEmitationDecrease(ushort) = 0;
  virtual void DonateTo(item*);
  virtual bool CanBeSeenBy(const character*) const = 0;
 protected:
  item* Item;
};

class stackslot : public slot
{
 public:
  friend class stack;
  friend class stackiterator;
  stackslot(stack* MotherStack, stackslot* Last) : MotherStack(MotherStack), Last(Last), Next(0) { }
  virtual void Empty();
  virtual void AddFriendItem(item*) const;
  virtual bool IsOnGround() const;
  virtual square* GetSquareUnder() const;
  virtual void SignalVolumeAndWeightChange();
  virtual void SignalEmitationIncrease(ushort);
  virtual void SignalEmitationDecrease(ushort);
  virtual void DonateTo(item*);
  virtual bool CanBeSeenBy(const character*) const;
  stack* GetMotherStack() const { return MotherStack; }
  void SetMotherStack(stack* What) { MotherStack = What; }
 protected:
  stack* MotherStack;
  stackslot* Last;
  stackslot* Next;
};

class characterslot : public slot
{
 public:
  virtual void Empty();
  character* GetMaster() const { return Master; }
  void SetMaster(character* What) { Master = What; }
  virtual void AddFriendItem(item*) const;
  virtual square* GetSquareUnder() const;
  virtual void SignalVolumeAndWeightChange();
  virtual void SignalEmitationIncrease(ushort);
  virtual void SignalEmitationDecrease(ushort);
  virtual void PutInItem(item*);
  virtual void Load(inputfile&);
  virtual bool CanBeSeenBy(const character*) const;
 protected:
  character* Master;
};

class gearslot : public slot
{
 public:
  virtual void Empty();
  bodypart* GetBodyPart() const { return BodyPart; }
  void SetBodyPart(bodypart* What) { BodyPart = What; }
  virtual void AddFriendItem(item*) const;
  void Init(bodypart*, uchar);
  virtual uchar GetEquipmentIndex() const { return EquipmentIndex; }
  virtual void SetEquipmentIndex(uchar What) { EquipmentIndex = What; }
  virtual void PutInItem(item*);
  virtual square* GetSquareUnder() const;
  virtual void SignalVolumeAndWeightChange();
  virtual void SignalEmitationIncrease(ushort);
  virtual void SignalEmitationDecrease(ushort);
  virtual bool CanBeSeenBy(const character*) const;
 protected:
  bodypart* BodyPart;
  uchar EquipmentIndex;
};

class actionslot : public slot
{
 public:
  virtual void Empty();
  action* GetAction() const { return Action; }
  void SetAction(action* What) { Action = What; }
  virtual bool IsActionSlot() const { return true; }
  virtual void AddFriendItem(item*) const;
  void Init(action*);
  virtual square* GetSquareUnder() const;
  virtual void SignalVolumeAndWeightChange();
  virtual void SignalEmitationIncrease(ushort);
  virtual void SignalEmitationDecrease(ushort);
  virtual bool CanBeSeenBy(const character*) const;
 protected:
  action* Action;
};

inline outputfile& operator<<(outputfile& SaveFile, const slot& Slot)
{
  Slot.Save(SaveFile);
  return SaveFile;
}

inline inputfile& operator>>(inputfile& SaveFile, slot& Slot)
{
  Slot.Load(SaveFile);
  return SaveFile;
}

#endif

