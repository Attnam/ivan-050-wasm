#ifndef __WINDOWH_H__
#define __WINDOWH_H__

#include <vector>

class window;
class cursor;
class programexitbutton;
class windowhandler
{
 public:
  static void CreateWindow(window*);
  static void Draw();
  static void Init();
  static void DeInit();
  static void Click(vector2d2d2d);
  static vector2d2d<window*>& GetWindowList() { return WindowList; }
  static void BringOnTop(window*);
<<<<<<< windowh.h
  static cursor* GetCursor(void) { return Cursor; }
  static void LoadPerson(window*);
=======
  static cursor* GetCursor() { return Cursor; }
>>>>>>> 1.3
 protected:
  static vector2d2d<window*> WindowList;
  static cursor* Cursor;
  static programexitbutton* ProgramExitButton;
  static int MouseSpeed;
};

#endif

