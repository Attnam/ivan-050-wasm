#ifndef __WTERRABA_H__
#define __WTERRABA_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include <string>

#include "vector2d.h"
#include "terra.h"

class wsquare;
class worldmap;
class gwterrain;
class owterrain;
class bitmap;

class wterrain
{
 public:
  wterrain() : WSquareUnder(0) { }
  virtual vector2d GetPos() const;
  virtual wsquare* GetWSquareUnder() const { return WSquareUnder; }
  virtual void SetWSquareUnder(wsquare* What) { WSquareUnder = What; }
  virtual worldmap* GetWorldMapUnder() const;
  virtual std::string Name(uchar) const;
  virtual void Load(inputfile&);
  virtual bool IsAnimated() const { return false; }
 protected:
  virtual void VirtualConstructor() { }
  virtual std::string GetNameStem() const = 0; // should be const std::string&
  //virtual const char* GetArticle() const { return "a"; }
  virtual bool LongerArticle() const { return false; }
  virtual vector2d GetBitmapPos(ushort) const = 0;
  wsquare* WSquareUnder;
};

class gwterrainprototype
{
 public:
  gwterrainprototype();
  virtual gwterrain* Clone() const = 0;
  gwterrain* CloneAndLoad(inputfile&) const;
  virtual std::string GetClassId() const = 0;
  ushort GetIndex() const { return Index; }
 protected:
  ushort Index;
};

class gwterrain : public wterrain, public gterrain
{
 public:
  typedef gwterrainprototype prototype;
  virtual void Save(outputfile&) const;
  void Draw(bitmap*, vector2d, ushort, bool, bool) const;
  virtual uchar Priority() const = 0;
  virtual uchar GetEntryDifficulty() const { return 10; }
  virtual const prototype* GetProtoType() const = 0;
  ushort GetType() const { return GetProtoType()->GetIndex(); }
};

class owterrainprototype
{
 public:
  owterrainprototype();
  virtual owterrain* Clone() const = 0;
  owterrain* CloneAndLoad(inputfile&) const;
  virtual std::string GetClassId() const = 0;
  ushort GetIndex() const { return Index; }
 protected:
  ushort Index;
};

class owterrain : public wterrain, public oterrain
{
 public:
  typedef owterrainprototype prototype;
  virtual void Save(outputfile&) const;
  void Draw(bitmap*, vector2d, ushort, bool, bool) const;
  virtual bool GoUp(character*) const;
  virtual bool GoDown(character*) const;
  virtual const prototype* GetProtoType() const = 0;
  ushort GetType() const { return GetProtoType()->GetIndex(); }
};

#ifdef __FILE_OF_STATIC_WTERRAIN_PROTOTYPE_DEFINITIONS__

#define WTERRAIN_PROTOTYPE(name, protobase)\
  \
  class name##_prototype : public protobase::prototype\
  {\
   public:\
    virtual protobase* Clone() const { return new name; }\
    virtual std::string GetClassId() const { return #name; }\
  } name##_ProtoType;\
  \
  ushort name::StaticType() { return name##_ProtoType.GetIndex(); }\
  const protobase::prototype* name::GetProtoType() const { return &name##_ProtoType; }

#else

#define WTERRAIN_PROTOTYPE(name, protobase)

#endif

#define WTERRAIN(name, base, protobase, data)\
\
name : public base\
{\
 public:\
  name() { VirtualConstructor(); }\
  static ushort StaticType();\
  virtual const prototype* GetProtoType() const;\
  data\
}; WTERRAIN_PROTOTYPE(name, protobase);

#define GWTERRAIN(name, base, data)\
\
WTERRAIN(\
  name,\
  base,\
  gwterrain,\
  data\
);

#define OWTERRAIN(name, base, data)\
\
WTERRAIN(\
  name,\
  base,\
  owterrain,\
  data\
);

#endif



