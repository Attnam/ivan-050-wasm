#include <ctype.h>

#include "save.h"
#include "strover.h"
#include "error.h"
#include "femath.h"
#include "graphics.h"

outputfile::outputfile(const std::string& FileName, bool AbortOnErr) : Buffer(fopen(FileName.c_str(), "wb"))
{
  if(AbortOnErr && !IsOpen())
    ABORT("Can't open %s for output!", FileName.c_str());
}

inputfile::inputfile(const std::string& FileName, bool AbortOnErr) : Buffer(fopen(FileName.c_str(), "rb"))
{
  if(AbortOnErr && !IsOpen())
    ABORT("File %s not found!", FileName.c_str());
}

/* This function is a gum solution */

bool inputfile::Eof()
{
  int Char = fgetc(Buffer);

  if(feof(Buffer) != 0)
    return true;
  else
    {
      ungetc(Char, Buffer);
      return false;
    }
}

int inputfile::Peek()
{
  int Char = fgetc(Buffer);
  ungetc(Char, Buffer);
  return Char;
}

std::string inputfile::ReadWord(bool AbortOnEOF)
{
  std::string ToReturn;
  ReadWord(ToReturn, AbortOnEOF);
  return ToReturn;
}

void inputfile::ReadWord(std::string& Buffer, bool AbortOnEOF)
{
  uchar Mode = 0;
  Buffer.resize(0);

  for(;;)
    {
      if(Eof())
	{
	  if(AbortOnEOF)
	    ABORT("Unexpected end of script file!");

	  return;
	}

      int Char = Peek();

      if(isalpha(Char) || Char == '_')
	{
	  if(!Mode)
	    Mode = 1;
	  else
	    if(Mode == 2)
	      return;

	  Buffer += char(Char);
	}

      if(Char == ' ' && Mode)
	return;

      if(isdigit(Char))
	{
	  if(!Mode)
	    Mode = 2;
	  else
	    if(Mode == 1)
	      return;

	  Buffer += char(Char);
	}

      if(ispunct(Char) && Char != '_')
	{
	  if(Char == '/')
	    {
	      Get();

	      if(!Eof())
		if(Peek() == '*')
		  {
		    for(;;)
		      {
			Char = Get();

			if(Eof())
			  ABORT("Script error: Unterminated comment!");

			if(Char == '*' && Peek() == '/')
			  {
			    Get();

			    if(Mode == 2)
			      return;
			    else
			      break;
			  }		
		      }

		    continue;
		  }

	      if(Mode)
		return;

	      Buffer += char(Char);
	      return;
	    }

	  if(Mode)
	    return;

	  if(Char == '"')
	    {
	      Get();

	      if(Eof())
		ABORT("Script error: Unterminated comment");

	      if(Peek() == '"')
		{
		  Get();
		  return;
		}

	      for(;;)
		{
		  Char = Get();

		  if(Eof())
		    ABORT("Script error: Unterminated comment");

		  if(Peek() == '"')
		    if(Char == '\\')
		      {
			Buffer += '"';
			Get();

			if(Peek() == '"')
			  {
			    Get();
			    break;
			  }
		      }
		    else
		      {
			Buffer += char(Char);
			Get();
			break;
		      }
		  else
		    Buffer += char(Char);
		}

	      return;
	    }

	  Buffer += char(Char);
	  Get();
	  return;
	}

      Get();
    }
}

char inputfile::ReadLetter(bool AbortOnEOF)
{
  for(;;)
    {
      if(Eof())
	{
	  if(AbortOnEOF)
	    ABORT("Unexpected end of script file!");

	  return 0;
	}

      int Char = Get();

      if(isalpha(Char) || isdigit(Char))
	return Char;

      if(ispunct(Char))
	{
	  if(Char == '/')
	    {
	      if(!Eof())
		if(Peek() == '*')
		  {
		    for(;;)
		      {
			Char = Get();

			if(Eof())
			  ABORT("Script error: Unterminated comment!");

			if(Char == '*' && Peek() == '/')
			  {
			    Get();
			    break;
			  }		
		      }

		    continue;
		  }
		else
		  return Char;
	    }
	  else
	    return Char;
	}
    }
}

/*
 * Reads a number or a formula from inputfile. Valid values could be for instance "3", "5 * 4+5", "2+rand%4" etc.
 * ValueMap contains keyword pairs that attach a certain numeric value to a word.
 */

long inputfile::ReadNumber(const valuemap& ValueMap, uchar CallLevel)
{
  long Value = 0;
  std::string Word;

  for(;;)
    {
      ReadWord(Word);

      if(isdigit(Word[0]))
	{
	  Value = atoi(Word.c_str());
	  continue;
	}

      if(Word == ";" || Word == ",")
	{
	  if(CallLevel != 0xFF)
	    SeekPosCur(-1);

	  return Value;
	}

      /* Convert this into an inline function! */

	#define CHECK_OP(op, cl)\
	\
	if(Word == #op)\
		if(cl < CallLevel)\
		{\
			Value op##= ReadNumber(ValueMap, cl);\
			continue;\
		}\
		else\
		{\
			SeekPosCur(-1);\
			return Value;\
		}

      CHECK_OP(&, 1) CHECK_OP(|, 1) CHECK_OP(^, 1)
      CHECK_OP(*, 2) CHECK_OP(/, 2) CHECK_OP(%, 2)
      CHECK_OP(+, 3) CHECK_OP(-, 3)

	if(Word == "(")
	  {
	    Value = ReadNumber(ValueMap, 4);
	    continue;
	  }

      if(Word == ")")
	return Value;

      if(Word == "rand")
	{
	  Value = RAND();
	  continue;
	}

      if(Word == "rgb")
	{
	  Value = MAKE_RGB(ReadNumber(ValueMap), ReadNumber(ValueMap), ReadNumber(ValueMap));
	  continue;
	}

      valuemap::const_iterator Iterator = ValueMap.find(Word);

      if(Iterator != ValueMap.end())
	{
	  Value = Iterator->second;
	  continue;
	}

      if(Word == "=" && CallLevel == 0xFF)
	continue;

      ABORT("Odd script value \"%s\" encountered!", Word.c_str());
    }
}

vector2d inputfile::ReadVector2d(const valuemap& ValueMap)
{
  vector2d Vector;

  Vector.X = ReadNumber(ValueMap);
  Vector.Y = ReadNumber(ValueMap);

  return Vector;
}

bool inputfile::ReadBool()
{
  std::string Word;
  ReadWord(Word);

  if(Word == "=")
    ReadWord(Word);

  if(ReadWord() != ";")
    ABORT("Bool value terminated incorrectly!");

  if(Word == "true" || Word == "1")
    return true;

  if(Word == "false" || Word == "0")
    return false;

  ABORT("Odd bool value \"%s\" encountered!", Word.c_str());

  return RAND() % 2 ? true : false;
}

outputfile& operator<<(outputfile& SaveFile, const std::string& String)
{
  uchar Length = String.length();
  SaveFile << Length;

  if(Length)
    SaveFile.Write(String.c_str(), Length);

  return SaveFile;
}

inputfile& operator>>(inputfile& SaveFile, std::string& String)
{
  char Buffer[256];
  uchar Length;
  SaveFile >> Length;

  if(Length)
    {
      SaveFile.Read(Buffer, Length);
      Buffer[Length] = 0;
      String = Buffer;
    }
  else
    String = "";

  return SaveFile;
}

/* Little easier-to-call version of the ReadNumber routine */

long inputfile::ReadNumber()
{
  return ReadNumber(valuemap());
}

void ReadData(std::string& String, inputfile& SaveFile, const valuemap&)
{
  SaveFile.ReadWord(String);

  if(String == "=")
    SaveFile.ReadWord(String);

  SaveFile.ReadWord();
}
