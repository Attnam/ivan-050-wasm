#ifndef __STACK_H__
#define __STACK_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#define DOWN 0
#define LEFT 1
#define UP 2
#define RIGHT 3

#define CENTER 4

#include <string>
#include <list>
#include <vector>

#include "typedef.h"
#include "vector2d.h"

class item;
class character;
class lsquare;
class bitmap;
class square;
class outputfile;
class inputfile;
class stackslot;
class felist;

typedef std::list<stackslot*> stacklist;
typedef std::list<stackslot*>::iterator stackiterator;
typedef std::vector<item*> itemvector;

/* Presentation of the stack class */

class stack
{
 public:
  stack(square* = 0, uchar = CENTER);
  ~stack();
  void Load(inputfile&);
  bool DrawToTileBuffer() const;
  void AddItem(item*);
  void FastAddItem(item*);
  void RemoveItem(stackiterator);
  void FastRemoveItem(stackiterator);
  item* GetItem(ushort) const;
  stackiterator GetBottomSlot() const;
  stackiterator GetSlotAboveTop() const;
  item* GetBottomItem() const;
  ushort GetItems() const { return Item->size(); }
  void SetSquareUnder(square*);
  item* DrawContents(character*, std::string, bool (item::*)(character*) const = 0) const;
  item* DrawContents(character*, std::string, bool, bool (item::*)(character*) const = 0) const;
  item* DrawContents(stack*, character*, std::string, std::string, std::string, bool (item::*)(character*) const = 0) const;
  item* DrawContents(stack*, character*, std::string, std::string, std::string, bool, bool (item::*)(character*) const = 0) const;
  item* MoveItem(stackiterator, stack*);
  ushort GetEmitation() const;
  vector2d GetPos() const;
  void Clean();
  ulong SumOfMasses() const;
  void Save(outputfile&) const;
  ushort SearchItem(item*) const;
  square* GetSquareUnder() const { return SquareUnder; }
  lsquare* GetLSquareUnder() const;
  bool SortedItems(character*, bool (item::*jipii)(character*) const) const;
  void DrawItemData(ushort, ushort) const;
  void DeletePointers();
  void Kick(ushort, bool, uchar);
  long Score() const;
  void Polymorph();
  void ReceiveSound(float);
  void StruckByWandOfStriking(character*, std::string);
  void CheckForStepOnEffect(character*);
  square* GetSquareTrulyUnder() const;
  lsquare* GetLSquareTrulyUnder() const;
  void ImpactDamage(ushort);
  void ReceiveFireDamage(character*, std::string, long);
  void Teleport();
  void FillItemVector(itemvector&) const;
  void AddContentsToList(felist&, character*, std::string, bool, bool (item::*)(character*) const) const;
  item* SearchChosen(ushort&, ushort, character*, bool (item::*)(character*) const) const;
 private:
  stacklist* Item;
  square* SquareUnder;
  uchar SquarePosition;
};

#endif
