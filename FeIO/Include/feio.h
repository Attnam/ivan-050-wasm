#ifndef __FEIO_H__
#define __FEIO_H__

#pragma warning(disable : 4786)

#include <string>

#include "typedef.h"
#include "vector2d.h"

class bitmap;
class colorizablebitmap;

class iosystem
{
public:
	static std::string WhatToLoadMenu(ushort, ushort);
	static std::string StringQuestion(std::string, vector2d, ushort, ushort, ushort, bool);
	static long NumberQuestion(std::string, vector2d, ushort, bool);
	static int Menu(bitmap*, std::string, ushort, ushort);
	static void TextScreen(std::string, ushort = 0xFFFF, bool = true);
private:
	static ulong CountChars(char, std::string);
};

#endif
