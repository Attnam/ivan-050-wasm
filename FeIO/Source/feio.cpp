#include <io.h>

#include "graphics.h"
#include "bitmap.h"
#include "feio.h"
#include "whandler.h"
#include "felist.h"
#include "colorbit.h"

#define PENT_WIDTH 70

void iosystem::TextScreen(std::string Text, ushort Color, bool GKey)
{
	char Line[200];
        ushort LastBeginningOfLine = 0;
        ushort c;
        ushort Lines = 0;
	ushort LineNumber = 1;

	DOUBLEBUFFER->ClearToColor(0);

	{
        for(ushort cc = 0; cc < 200; ++cc)
		Line[cc] = 0;
	}

	for(ushort cc = 0; cc < Text.length(); ++cc)
		if(Text[cc] == '\n')
			++LineNumber;

	for(c = 0; Text[c] != 0; ++c)
		if(Text[c] == '\n')
		{
			FONT->Printf(DOUBLEBUFFER, 400 - strlen(Line) * 4, 275 - (LineNumber - Lines) * 15, Color, "%s", Line);
                        LastBeginningOfLine = c + 1;
                        ++Lines;
                        for(ushort cc = 0; cc < 200; ++cc)
                        	Line[cc] = 0;
                }
		else
			Line[c - LastBeginningOfLine] = Text[c];

	FONT->Printf(DOUBLEBUFFER, 400 - strlen(Line) * 4, 275 - (LineNumber - Lines) * 15, Color, "%s", Line);

	graphics::BlitDBToScreen();

	if(GKey)
		GETKEY();
}

unsigned int iosystem::CountChars(char cSF,std::string sSH) // (MENU)
{
	unsigned int iReturnCounter = 0;

	for(unsigned int i = 0; i < sSH.length(); ++i)
		if(sSH[i] == cSF)
			++iReturnCounter;

	return iReturnCounter;
}

int iosystem::Menu(std::string sMS, ushort ColorSelected, ushort ColorNotSelected) // (MENU)
{
	if(CountChars('\r',sMS) < 1)
		return (-1);

	bool bReady = false;
	unsigned int iSelected = 0;
	double Rotation = 0;
	while(!bReady)
	{
		if(Rotation == 2 * 3.141)
			Rotation = 0;
		else
			Rotation += 0.003;

		DOUBLEBUFFER->ClearToColor(0);

		for(int x = 0; x < 10; ++x)
			DOUBLEBUFFER->DrawPolygon(vector2d(150,150), 100, 5, MAKE_RGB(int(255 - 25 * (10 - x)),0,0), true, Rotation + double(x) / 50);
		
		std::string sCopyOfMS = sMS;

		for(x = 0; x < 4; ++x)
			DOUBLEBUFFER->DrawPolygon(vector2d(150,150), 100 + x, 50, MAKE_RGB(int(255 - 12 * x),0,0));

		for(unsigned int i = 0; i < CountChars('\r',sMS); ++i)
		{
			std::string HYVINEPAGURUPRINTF = sCopyOfMS.substr(0,sCopyOfMS.find_first_of('\r'));
			sCopyOfMS.erase(0,sCopyOfMS.find_first_of('\r')+1);
			FONT->Printf(DOUBLEBUFFER, 400 - ((HYVINEPAGURUPRINTF.length() + 4) << 2), 200+(i*50), (i == iSelected ? ColorSelected : ColorNotSelected), "%d. %s", i + 1, HYVINEPAGURUPRINTF.c_str());
		}

		graphics::BlitDBToScreen();
		int k;
		
		switch(k = globalwindowhandler::ReadKey())
		{
			
			case 0x148:
				if (iSelected > 0)
					--iSelected;
				else
					iSelected = (CountChars('\r',sMS)-1);
				break;

			case 0x150:
				if (iSelected < (CountChars('\r',sMS)-1))
					++iSelected;
				else
					iSelected = 0;
				break;

			case 0x00D:
				bReady = true;
				break;
			case 0:
			
			break;
			default:
				if(k > 0x30 && k < int(0x31 + CountChars('\r',sMS)))
					return signed(k - 0x31);
		}
	}

	return signed(iSelected);
}

std::string iosystem::StringQuestion(std::string Topic, vector2d Pos, ushort Color, ushort MinLetters, ushort MaxLetters)
{
	std::string Input;

	bitmap Backup(XRES, YRES);
	DOUBLEBUFFER->Blit(&Backup, 0, 0, 0, 0, XRES, YRES);

	for(int LastKey = 0;; LastKey = 0)
	{
		Backup.Blit(DOUBLEBUFFER, 0, 0, 0, 0, XRES, YRES);
		FONT->Printf(DOUBLEBUFFER, Pos.X, Pos.Y, Color, "%s", Topic.c_str());
		FONT->Printf(DOUBLEBUFFER, Pos.X, Pos.Y + 10, Color, "%s_", Input.c_str());
		graphics::BlitDBToScreen();

		while(!(isalpha(LastKey) || LastKey == ' ' || LastKey == '-' || LastKey == 8 || LastKey == 13))
			LastKey = GETKEY();

		if(LastKey == 8)
		{
			if(Input.length())
				Input.resize(Input.length() - 1);

			continue;
		}

		if(LastKey == 13)
			if(Input.length() >= MinLetters)
				break;
			else
				continue;

		if(Input.length() <= MaxLetters)
			Input += LastKey;
	}


	return Input;
}

long iosystem::NumberQuestion(std::string Topic, vector2d Pos, ushort Color)
{
	std::string Input;

	bitmap Backup(XRES, YRES);
	DOUBLEBUFFER->Blit(&Backup, 0, 0, 0, 0, XRES, YRES);

	for(int LastKey = 0;; LastKey = 0)
	{
		Backup.Blit(DOUBLEBUFFER, 0, 0, 0, 0, XRES, YRES);
		FONT->Printf(DOUBLEBUFFER, Pos.X, Pos.Y, Color, "%s", Topic.c_str());
		FONT->Printf(DOUBLEBUFFER, Pos.X, Pos.Y + 10, Color, "%s_", Input.c_str());
		graphics::BlitDBToScreen();

		while(!(isdigit(LastKey) || LastKey == 8 || LastKey == 13))
		{
			if(LastKey == '-' && !Input.length())
				break;

			LastKey = GETKEY();
		}

		if(LastKey == 8)
		{
			if(Input.length())
				Input.resize(Input.length() - 1);

			continue;
		}

		if(LastKey == 13)
			break;

		if(Input.length() < 12)
			Input += LastKey;
	}

	return atoi(Input.c_str());
}

std::string iosystem::WhatToLoadMenu(ushort TopicColor, ushort ListColor) // for some _very_ strange reason "LoadMenu" occasionaly generates an error!
{
	struct _finddata_t Found;
	long hFile;
	int Check = 0;
	felist Buffer("Chooseth a file and be sorry", TopicColor);
	hFile = _findfirst("Save/*.sav", &Found);

	if(hFile == -1L)
	{
		DOUBLEBUFFER->ClearToColor(0);
		FONT->Printf(DOUBLEBUFFER, 260, 200, TopicColor, "You don't have any previous saves.");
		graphics::BlitDBToScreen();
		GETKEY();
		return "";
	}

	while(!Check)
	{
		Buffer.AddEntry(Found.name, ListColor);
		Check = _findnext(hFile, &Found);
	}

	Check = 0xFFFF;

	while(Check > 0xFFFD)
	{
		DOUBLEBUFFER->ClearToColor(0);
		Check = Buffer.Draw();
	}

	if(Check == 0xFFFD)
		return "";

	return Buffer.GetEntry(Check);
}
