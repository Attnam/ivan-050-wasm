#include <ctime>
#include <ctype.h>

#ifdef WIN32
#include <io.h>
#endif

#ifdef USE_SDL
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <algorithm>
#endif

#ifdef LINUX
#include <dirent.h>
#endif

#ifdef __DJGPP__
#include <dir.h>
#endif

#include "graphics.h"
#include "bitmap.h"
#include "feio.h"
#include "whandler.h"
#include "felist.h"
#include "colorbit.h"
#include "femath.h"
#include "festring.h"

#define PENT_WIDTH 70

void iosystem::TextScreen(const std::string& Text, ushort Color, bool GKey, void (*BitmapEditor)(bitmap*))
{
  bitmap Buffer(RES);
  Buffer.Fill(0);
  ushort c, LineNumber = 0;

  for(c = 0; c < Text.length(); ++c)
    if(Text[c] == '\n')
      ++LineNumber;

  LineNumber >>= 1;
  char Line[200];
  ushort Lines = 0, LastBeginningOfLine = 0;

  for(c = 0; c < Text.length(); ++c)
    if(Text[c] == '\n')
      {
	Line[c - LastBeginningOfLine] = 0;
	FONT->Printf(&Buffer, (RES.X >> 1) - (strlen(Line) << 2), (RES.Y << 1) / 5 - (LineNumber - Lines) * 15, Color, Line);
	++Lines;
	LastBeginningOfLine = c + 1;
      }
    else
      Line[c - LastBeginningOfLine] = Text[c];

  Line[c - LastBeginningOfLine] = 0;
  FONT->Printf(&Buffer, (RES.X >> 1) - (strlen(Line) << 2), (RES.Y << 1) / 5 - (LineNumber - Lines) * 15, Color, Line);
  Buffer.FadeToScreen(BitmapEditor);

  if(GKey)
      GET_KEY();
}

ulong iosystem::CountChars(char cSF, const std::string& sSH)
{
  ulong iReturnCounter = 0;

  for(ulong i = 0; i < sSH.length(); ++i)
    if(sSH[i] == cSF)
      ++iReturnCounter;

  return iReturnCounter;
}

int iosystem::Menu(bitmap* BackGround, vector2d Pos, const std::string& Topic, const std::string& sMS, ushort Color, const std::string& SmallText1, const std::string& SmallText2)
{
  if(CountChars('\r',sMS) < 1)
    return (-1);

  bool bReady = false;
  ulong iSelected = 0;
  bitmap Backup(RES);
  DOUBLE_BUFFER->Blit(&Backup);
  bitmap Buffer(RES);
  ushort c = 0;

  if(BackGround)
    BackGround->Blit(&Buffer);
  else
    Buffer.Fill(0);

  std::string sCopyOfMS;
  std::string VeryUnGuruPrintf;

  while(!bReady)
    {
      clock_t StartTime = clock();
      sCopyOfMS = Topic;
      ulong i;

      for(i = 0; i < CountChars('\r', Topic); ++i)
	{
	  VeryUnGuruPrintf = sCopyOfMS.substr(0,sCopyOfMS.find_first_of('\r'));
	  sCopyOfMS.erase(0,sCopyOfMS.find_first_of('\r')+1);
	  FONT->Printf(&Buffer, Pos.X - (VeryUnGuruPrintf.length() << 2), Pos.Y - 30 - (CountChars('\r', Topic) + CountChars('\r', sMS)) * 25 + i * 25, RED, "%s", VeryUnGuruPrintf.c_str());
	}

      sCopyOfMS = sMS;

      for(i = 0; i < CountChars('\r', sMS); ++i)
	{
	  VeryUnGuruPrintf = sCopyOfMS.substr(0,sCopyOfMS.find_first_of('\r'));
	  sCopyOfMS.erase(0,sCopyOfMS.find_first_of('\r')+1);

	  ushort XPos = Pos.X - ((VeryUnGuruPrintf.length() + 3) << 2);
	  ushort YPos = Pos.Y - CountChars('\r', sMS) * 25 + i * 50;

	  if(i == iSelected)
	    {
	      Buffer.Fill(XPos, YPos, ((VeryUnGuruPrintf.length() + 3) << 3), 8, 0);
	      FONT->PrintfShade(&Buffer, XPos, YPos, Color, "%d. %s", i + 1, VeryUnGuruPrintf.c_str());
	    }
	  else
	    FONT->Printf(&Buffer, XPos, YPos, Color, "%d. %s", i + 1, VeryUnGuruPrintf.c_str());
	}

      sCopyOfMS = SmallText1;

      for(i = 0; i < CountChars('\r', SmallText1); ++i)
	{
	  VeryUnGuruPrintf = sCopyOfMS.substr(0,sCopyOfMS.find_first_of('\r'));
	  sCopyOfMS.erase(0,sCopyOfMS.find_first_of('\r')+1);
	  FONT->Printf(&Buffer, 3, RES.Y - CountChars('\r', SmallText1) * 10 + i * 10, Color, "%s", VeryUnGuruPrintf.c_str());
	}

      sCopyOfMS = SmallText2;

      for(i = 0; i < CountChars('\r', SmallText2); ++i)
	{
	  VeryUnGuruPrintf = sCopyOfMS.substr(0,sCopyOfMS.find_first_of('\r'));
	  sCopyOfMS.erase(0,sCopyOfMS.find_first_of('\r')+1);
	  FONT->Printf(&Buffer, RES.X - (VeryUnGuruPrintf.length() << 3) - 2, RES.Y - CountChars('\r', SmallText2) * 10 + i * 10, Color, "%s", VeryUnGuruPrintf.c_str());
	}

      int k;

      if(c < 5)
	{
	  ushort Element = 127 - c * 25;
	  Backup.MaskedBlit(DOUBLE_BUFFER, MakeRGB24(Element, Element, Element), 0);
	  Buffer.SimpleAlphaBlit(DOUBLE_BUFFER, c++ * 50, 0);
	  graphics::BlitDBToScreen();
	  while(clock() - StartTime < 0.05f * CLOCKS_PER_SEC);
	  k = READ_KEY();
	}
      else
	{
	  Buffer.Blit(DOUBLE_BUFFER);
	  graphics::BlitDBToScreen();
	  k = GET_KEY(false);
	}
		
      switch(k)
	{	
	case KEY_UP:
	  if(iSelected > 0)
	    --iSelected;
	  else
	    iSelected = (CountChars('\r',sMS)-1);
	  break;

	case KEY_DOWN:
	  if(iSelected < (CountChars('\r',sMS)-1))
	    ++iSelected;
	  else
	    iSelected = 0;
	  break;

	case 0x00D:
	  bReady = true;
	  break;

	default:
	  if(k > 0x30 && k < int(0x31 + CountChars('\r',sMS)))
	    return signed(k - 0x31);
	}
    }

  return signed(iSelected);
}

std::string iosystem::StringQuestion(const std::string& Topic, vector2d Pos, ushort Color, ushort MinLetters, ushort MaxLetters, bool Fade, bool AllowExit)
{
  if(Fade)
    {
      bitmap Buffer(RES);
      Buffer.Fill(0);
      FONT->Printf(&Buffer, Pos.X, Pos.Y, Color, "%s", Topic.c_str());
      FONT->Printf(&Buffer, Pos.X, Pos.Y + 10, Color, "_");
      Buffer.FadeToScreen();
    }

  std::string Input;
  bool TooShort = false;
  FONT->Printf(DOUBLE_BUFFER, Pos.X, Pos.Y, Color, "%s", Topic.c_str());

  for(int LastKey = 0;; LastKey = 0)
    {
      DOUBLE_BUFFER->Fill(Pos.X, Pos.Y + 10, (MaxLetters << 3) + 9, 9, 0);
      FONT->Printf(DOUBLE_BUFFER, Pos.X, Pos.Y + 10, Color, "%s_", Input.c_str());

      if(TooShort)
	{
	  FONT->Printf(DOUBLE_BUFFER, Pos.X, Pos.Y + 30, Color, "Too short!");
	  TooShort = false;
	}

      graphics::BlitDBToScreen();

      if(TooShort)
	DOUBLE_BUFFER->Fill(Pos.X, Pos.Y + 30, 81, 9, 0);
		
      while(!(LastKey >= 0x20 || LastKey == KEY_BACK_SPACE || LastKey == KEY_ENTER || LastKey == KEY_ESC))
	LastKey = GET_KEY(false);

      if(LastKey == KEY_ESC && AllowExit)
	return "";
		
      if(LastKey == KEY_BACK_SPACE)
	{
	  if(Input.length())
	    Input.resize(Input.length() - 1);

	  continue;
	}

      if(LastKey == KEY_ENTER)
	if(Input.length() >= MinLetters)
	  break;
	else
	  {
	    TooShort = true;
	    continue;
	  }

      if(LastKey >= 0x20 && Input.length() < MaxLetters && (LastKey != ' ' || Input.length()))
	Input += char(LastKey);
    }

  strsize LastAlpha = strsize(-1);

  for(strsize c = 0; c < Input.size(); ++c)
    if(Input[c] != ' ')
      LastAlpha = c;

  Input.resize(LastAlpha + 1);
  return Input;
}

long iosystem::NumberQuestion(const std::string& Topic, vector2d Pos, ushort Color, bool Fade)
{
  if(Fade)
    {
      bitmap Buffer(RES);
      Buffer.Fill(0);
      FONT->Printf(&Buffer, Pos.X, Pos.Y, Color, "%s", Topic.c_str());
      FONT->Printf(&Buffer, Pos.X, Pos.Y + 10, Color, "_");
      Buffer.FadeToScreen();
    }

  std::string Input;
  FONT->Printf(DOUBLE_BUFFER, Pos.X, Pos.Y, Color, "%s", Topic.c_str());

  for(int LastKey = 0;; LastKey = 0)
    {
      DOUBLE_BUFFER->Fill(Pos.X, Pos.Y + 10, 105, 9, 0);
      FONT->Printf(DOUBLE_BUFFER, Pos.X, Pos.Y + 10, Color, "%s_", Input.c_str());
      graphics::BlitDBToScreen();

      while(!isdigit(LastKey) && LastKey != KEY_BACK_SPACE && LastKey != KEY_ENTER && (LastKey != '-' || Input.length()))
	LastKey = GET_KEY(false);

      if(LastKey == KEY_BACK_SPACE)
	{
	  if(Input.length())
	    Input.resize(Input.length() - 1);

	  continue;
	}

      if(LastKey == KEY_ENTER)
	break;

      if(Input.length() < 12)
	Input += char(LastKey);
    }

  return atoi(Input.c_str());
}

long iosystem::ScrollBarQuestion(const std::string& Topic, vector2d Pos, long StartValue, long Step, long Min, long Max, long AbortValue, ushort TopicColor, ushort Color1, ushort Color2, bool Fade, void (*Handler)(long))
{
  long BarValue = StartValue;
  std::string Input;
  bool FirstTime = true;

  if(Fade)
    {
      bitmap Buffer(RES);
      Buffer.Fill(0);
      FONT->Printf(&Buffer, Pos.X, Pos.Y, TopicColor, "%s %s_", Topic.c_str(), Input.c_str());
      Buffer.DrawLine(Pos.X + 1, Pos.Y + 15, Pos.X + 201, Pos.Y + 15, Color2, false);
      Buffer.DrawLine(Pos.X + 201, Pos.Y + 12, Pos.X + 201, Pos.Y + 18, Color2, false);
      Buffer.DrawLine(Pos.X + 1, Pos.Y + 15, Pos.X + 1 + (BarValue - Min) * 200 / (Max - Min), Pos.Y + 15, Color1, true);
      Buffer.DrawLine(Pos.X + 1, Pos.Y + 12, Pos.X + 1, Pos.Y + 18, Color1, true);
      Buffer.DrawLine(Pos.X + 1 + (BarValue - Min) * 200 / (Max - Min), Pos.Y + 12, Pos.X + 1 + (BarValue - Min) * 200 / (Max - Min), Pos.Y + 18, Color1, true);
      Buffer.FadeToScreen();
    }

  for(int LastKey = 0;; LastKey = 0)
    {
      if(!FirstTime)
	BarValue = Input.empty() ? Min : atoi(Input.c_str());

      if(BarValue < Min)
	BarValue = Min;

      if(BarValue > Max)
	BarValue = Max;

      if(Handler)
	Handler(BarValue);

      DOUBLE_BUFFER->Fill(Pos.X, Pos.Y, ((Topic.length() + 14) << 3) + 1, 10, 0);
      DOUBLE_BUFFER->Fill(Pos.X, Pos.Y + 10, 203, 10, 0);

      if(FirstTime)
	{
	  FONT->Printf(DOUBLE_BUFFER, Pos.X, Pos.Y, TopicColor, "%s %d", Topic.c_str(), StartValue);
	  FONT->Printf(DOUBLE_BUFFER, Pos.X + (Topic.length() << 3) + 8, Pos.Y + 1, TopicColor, "_");
	  FirstTime = false;
	}
      else
	{
	  FONT->Printf(DOUBLE_BUFFER, Pos.X, Pos.Y, TopicColor, "%s %s_", Topic.c_str(), Input.c_str());
	  FONT->Printf(DOUBLE_BUFFER, Pos.X + ((Topic.length() + Input.length()) << 3) + 8, Pos.Y + 1, TopicColor, "_");
	}
      
      DOUBLE_BUFFER->DrawLine(Pos.X + 1, Pos.Y + 15, Pos.X + 201, Pos.Y + 15, Color2, false);
      DOUBLE_BUFFER->DrawLine(Pos.X + 201, Pos.Y + 12, Pos.X + 201, Pos.Y + 18, Color2, false);
      DOUBLE_BUFFER->DrawLine(Pos.X + 1, Pos.Y + 15, Pos.X + 1 + (BarValue - Min) * 200 / (Max - Min), Pos.Y + 15, Color1, true);
      DOUBLE_BUFFER->DrawLine(Pos.X + 1, Pos.Y + 12, Pos.X + 1, Pos.Y + 18, Color1, true);
      DOUBLE_BUFFER->DrawLine(Pos.X + 1 + (BarValue - Min) * 200 / (Max - Min), Pos.Y + 12, Pos.X + 1 + (BarValue - Min) * 200 / (Max - Min), Pos.Y + 18, Color1, true);
      graphics::BlitDBToScreen();

      while(!isdigit(LastKey) && LastKey != KEY_ESC && LastKey != KEY_BACK_SPACE && LastKey != KEY_ENTER && LastKey != '<' && LastKey != '>' && LastKey != KEY_RIGHT && LastKey != KEY_LEFT)
	LastKey = GET_KEY(false);

      if(LastKey == KEY_ESC)
	{
	  BarValue = AbortValue;
	  break;
	}

      if(LastKey == KEY_BACK_SPACE)
	{
	  if(Input.length())
	    Input.resize(Input.length() - 1);

	  continue;
	}

      if(LastKey == KEY_ENTER)
	break;

      if(LastKey == '<' || LastKey == KEY_LEFT)
	{
	  BarValue -= Step;

	  if(BarValue < Min)
	    BarValue = Min;

	  Input.resize(0);
	  Input += BarValue;
	  continue;
	}

      if(LastKey == '>' || LastKey == KEY_RIGHT)
	{
	  BarValue += Step;

	  if(BarValue > Max)
	    BarValue = Max;

	  Input.resize(0);
	  Input += BarValue;
	  continue;
	}

      if(Input.length() < 12)
	Input += char(LastKey);
    }

  return BarValue;
}

std::string iosystem::ContinueMenu(ushort TopicColor, ushort ListColor, const std::string& DirectoryName)
{
#ifdef WIN32
  struct _finddata_t Found;
  long hFile;
  int Check = 0;
  felist Buffer("Choose a file and be sorry:", TopicColor);
  hFile = _findfirst((DirectoryName + "*.sav").c_str(), &Found);

  if(hFile == -1L)
    {
      TextScreen("You don't have any previous saves.", TopicColor);
      return "";
    }

  while(!Check)
    {
      Buffer.AddEntry(Found.name, ListColor);
      Check = _findnext(hFile, &Found);
    }

  Check = Buffer.Draw();

  if(Check & FELIST_ERROR_BIT)
    return "";

  return Buffer.GetEntry(Check);
#endif

#ifdef LINUX
  DIR* dp;
  struct dirent* ep;
  std::string Buffer;
  felist List("Choose a file and be sorry:", TopicColor);
  dp = opendir(DirectoryName.c_str());
  if(dp)
    {
      while(ep = readdir(dp))
	{
	  Buffer = ep->d_name;
	  if(Buffer.find(".sav") != Buffer.npos)
	    {
	      List.AddEntry(Buffer, ListColor);
	    }
	}
      if(List.IsEmpty())
	{
	  TextScreen("You don't have any previous saves.", TopicColor);
	  return "";
	}
      else
	{
	  int Check = List.Draw();

	  if(Check & FELIST_ERROR_BIT)
	    return "";

	  return List.GetEntry(Check);
	}

    }
#endif

#ifdef __DJGPP__
  struct ffblk Found;
  int Check = 0;
  felist Buffer("Choose a file and be sorry:", TopicColor);
  Check = findfirst((DirectoryName + "*.sav").c_str(), &Found, FA_HIDDEN | FA_ARCH);

  if(Check)
    {
      TextScreen("You don't have any previous saves.", TopicColor);
      return "";
    }

  while(!Check)
    {
      Buffer.AddEntry(Found.ff_name, ListColor);
      Check = findnext(&Found);
    }

  Check = Buffer.Draw();

  if(Check & FELIST_ERROR_BIT)
    return "";

  return Buffer.GetEntry(Check);
#endif
}
