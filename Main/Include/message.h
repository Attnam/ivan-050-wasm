#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#define ADD_MESSAGE globalmessagingsystem::AddMessage
#define DRAW_MESSAGES globalmessagingsystem::Draw
#define EMPTY_MESSAGES globalmessagingsystem::Empty

#include "typedef.h"

class felist;

class globalmessagingsystem
{
 public:
  static void AddMessage(const char*, ...);
  static void Draw();
  static void Empty();
  static void DrawMessageHistory();
  static void Format();
  static felist* GetMessageHistory() { return &MessageHistory; }
 private:
  static char* MessageBuffer;
  static ushort BufferLength;
  static felist MessageHistory;
};

#endif



