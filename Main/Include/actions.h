#ifndef __ACTIONS_H__
#define __ACTIONS_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include "action.h"
#include "vector2d.h"

class ACTION
(
  faint,
  action,
 public:
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void Handle();
  void SetCounter(int What) { Counter = What; }
  virtual bool IsVoluntary() const { return false; }
  virtual void Terminate(bool);
  virtual bool AllowFaint() const { return false; }
  virtual const char* GetDescription() const;
  virtual const char* GetDeathExplanation() const;
  virtual bool CanBeTalkedTo() const { return false; }
 protected:
  int Counter;
);

class ACTION
(
  consume,
  action,
 public:
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void Handle();
  virtual void Terminate(bool);
  void SetConsumingID(long What) { ConsumingID = What; }
  virtual bool AllowFaint() const { return false; }
  virtual bool AllowFoodConsumption() const { return false; }
  virtual const char* GetDescription() const;
  virtual void SetDescription(const festring&);
 protected:
  festring Description;
  long ConsumingID;
);

class ACTION
(
  rest,
  action,
 public:
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void Handle();
  void SetGoalHP(int What) { GoalHP = What; }
  virtual void Terminate(bool);
  virtual bool IsRest() const { return true; }
  virtual const char* GetDescription() const;
  void SetMinToStop(int What) { MinToStop = What; }
 protected:
  int GoalHP;
  int MinToStop;
);

class ACTION
(
  dig,
  action,
 public:
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void Handle();
  void SetSquareDug(vector2d What) { SquareDug = What; }
  virtual void Terminate(bool);
  void SetRightBackupID(long What) { RightBackupID = What; }
  void SetLeftBackupID(long What) { LeftBackupID = What; }
  virtual bool TryDisplace() { return false; }
  virtual const char* GetDescription() const;
  virtual bool ShowEnvironment() const { return false; }
  void SetMoveDigger(bool What) { MoveDigger = What; }
 protected:
  long RightBackupID;
  long LeftBackupID;
  vector2d SquareDug;
  bool MoveDigger;
);

class ACTION
(
  go,
  action,
 public:
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void Handle();
  int GetDirection() const { return Direction; }
  void SetDirection(int What) { Direction = What; }
  bool IsWalkingInOpen() const { return WalkingInOpen; }
  void SetIsWalkingInOpen(bool What) { WalkingInOpen = What; }
  virtual bool TryDisplace();
  virtual const char* GetDescription() const;
  virtual bool ShowEnvironment() const { return false; }
 protected:
  int Direction;
  bool WalkingInOpen;
);

class ACTION
(
  study,
  action,
 public:
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  virtual void Handle();
  virtual void Terminate(bool);
  void SetLiteratureID(long What) { LiteratureID = What; }
  virtual const char* GetDescription() const;
  void SetCounter(int What) { Counter = What; }
 protected:
  long LiteratureID;
  int Counter;
);

#endif
