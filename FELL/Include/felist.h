#ifndef __LIST_H__
#define __LIST_H__

#include <string>

#include "dynarray.h"

class bitmap;

class felist
{
public:
	felist(ushort Maximum = 0) : Maximum(Maximum) {}
	felist(std::string Topic, ushort Maximum = 0) : Maximum(Maximum) { Description.Add(Topic); }
	void AddString(std::string S);
	void AddDescription(std::string S) { Description.Add(S); }
	void DrawDescription(bitmap*) const;
	ushort Draw(bitmap*, bitmap*, bool = true) const;
	void Empty();
	std::string GetString(ushort Index) { return String.Access(Index); }
	ushort Length() const { return String.Length(); }
protected:
	dynarray<std::string> String;
	dynarray<std::string> Description;
	std::string Topic;
	ushort Maximum;
};

#endif
