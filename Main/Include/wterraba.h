#ifndef __WTERRABA_H__
#define __WTERRABA_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include "vector2d.h"
#include "terra.h"
#include "proto.h"

class worldmapsquare;
class worldmap;

class worldmapterrain : public typeable
{
 public:
  worldmapterrain() : WorldMapSquareUnder(0) {}
  virtual vector2d GetPos() const;
  virtual worldmapsquare* GetWorldMapSquareUnder() const { return WorldMapSquareUnder; }
  virtual void SetWorldMapSquareUnder(worldmapsquare* What) { WorldMapSquareUnder = What; }
  virtual worldmap* GetWorldMapUnder() const;
  virtual std::string Name(uchar = 0) const;
  virtual void Load(inputfile&);
 protected:
  virtual std::string NameStem() const = 0;
  virtual std::string Article() const { return "a"; }
  virtual vector2d GetBitmapPos() const = 0;
  worldmapsquare* WorldMapSquareUnder;
};

class groundworldmapterrain : public worldmapterrain, public groundterrain
{
 public:
  groundworldmapterrain(bool = true) {}
  virtual void DrawToTileBuffer() const;
  virtual groundworldmapterrain* Clone(bool = true) const = 0;
  virtual uchar Priority() const = 0;
  virtual ushort GetEntryAPRequirement() const { return 10000; }
};

class overworldmapterrain : public worldmapterrain, public overterrain
{
 public:
  overworldmapterrain(bool = true) {}
  virtual void DrawToTileBuffer() const;
  virtual overworldmapterrain* Clone(bool = true) const = 0;
  virtual bool GoUp(character*) const;
  virtual bool GoDown(character*) const;
};

#ifdef __FILE_OF_STATIC_WTERRAIN_PROTOTYPE_DECLARATIONS__

#define WORLDMAPTERRAIN_PROTOINSTALLER(name, base, protobase, setstats)\
  \
  static class name##_protoinstaller\
  {\
   public:\
    name##_protoinstaller() : Index(protocontainer<protobase>::Add(new name(false))) {}\
    ushort GetIndex() const { return Index; }\
   private:\
    ushort Index;\
  } name##_ProtoInstaller;\
  \
  void name::SetDefaultStats() { setstats }\
  ushort name::StaticType() { return name##_ProtoInstaller.GetIndex(); }\
  const protobase* const name::GetPrototype() { return protocontainer<protobase>::GetProto(StaticType()); }\
  ushort name::Type() const { return name##_ProtoInstaller.GetIndex(); }

#else

#define WORLDMAPTERRAIN_PROTOINSTALLER(name, base, protobase, setstats)

#endif

#define WORLDMAPTERRAIN(name, base, protobase, setstats, data)\
\
name : public base\
{\
 public:\
  name(bool SetStats = true) : base(false) { if(SetStats) SetDefaultStats(); }\
  static ushort StaticType();\
  static const protobase* const GetPrototype();\
  virtual std::string ClassName() const { return #name; }\
 protected:\
  virtual void SetDefaultStats();\
  virtual ushort Type() const;\
  data\
}; WORLDMAPTERRAIN_PROTOINSTALLER(name, base, protobase, setstats)

#define GROUNDWORLDMAPTERRAIN(name, base, setstats, data)\
\
WORLDMAPTERRAIN(\
  name,\
  base,\
  groundworldmapterrain,\
  setstats,\
  virtual groundworldmapterrain* Clone(bool SetStats = true) const { return new name(SetStats); }\
  virtual typeable* CloneAndLoad(inputfile& SaveFile) const { groundworldmapterrain* G = new name(false); G->Load(SaveFile); return G; }\
  data\
);

#define OVERWORLDMAPTERRAIN(name, base, setstats, data)\
\
WORLDMAPTERRAIN(\
  name,\
  base,\
  overworldmapterrain,\
  setstats,\
  virtual overworldmapterrain* Clone(bool SetStats = true) const { return new name(SetStats); }\
  virtual typeable* CloneAndLoad(inputfile& SaveFile) const { overworldmapterrain* O = new name(false); O->Load(SaveFile); return O; }\
  data\
);

#endif
