#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include "vector2d.h"

#define ADD_MESSAGE msgsystem::AddMessage

class felist;
class outputfile;
class inputfile;
class bitmap;
class festring;

class msgsystem
{
 public:
  static void AddMessage(const char*, ...);
  static void Draw();
  static void DrawMessageHistory();
  static void Format();
  static void Save(outputfile&);
  static void Load(inputfile&);
  static void ScrollDown();
  static void ScrollUp();
  static void EnableMessages() { Enabled = true; }
  static void DisableMessages() { Enabled = false; }
  static void EnterBigMessageMode() { BigMessageMode = true; }
  static void LeaveBigMessageMode();
  static void Init();
 private:
  static felist MessageHistory;
  static festring LastMessage;
  static festring BigMessage;
  static int Times;
  static vector2d Begin, End;
  static bool Enabled;
  static bool BigMessageMode;
  static bool MessagesChanged;
  static bitmap* QuickDrawCache;
};

#endif
