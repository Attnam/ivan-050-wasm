#ifndef __WSQUARE_H__
#define __WSQUARE_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include "square.h"
#include "worldmap.h"

class gwterrain;
class owterrain;

class wsquare : public square
{
 public:
  friend class worldmap;
  wsquare(worldmap*, vector2d);
  virtual ~wsquare();
  virtual void Save(outputfile&) const;
  virtual void Load(inputfile&);
  void Draw();
  void SetGWTerrain(gwterrain*);
  void SetOWTerrain(owterrain*);
  gwterrain* GetGWTerrain() const { return GWTerrain; }
  owterrain* GetOWTerrain() const { return OWTerrain; }
  void ChangeWTerrain(gwterrain*, owterrain*);
  worldmap* GetWorldMap() const { return static_cast<worldmap*>(AreaUnder); }
  void SetWorldMapUnder(worldmap* What) { AreaUnder = What; }
  void UpdateMemorizedDescription(bool = false);
  virtual gterrain* GetGTerrain() const;
  virtual oterrain* GetOTerrain() const;
  void ChangeGWTerrain(gwterrain*);
  void ChangeOWTerrain(owterrain*);
  void SetWTerrain(gwterrain*, owterrain*);
  void SetLastSeen(ulong);
  void CalculateLuminance();
  wsquare* GetNeighbourWSquare(ushort Index) const { return static_cast<worldmap*>(AreaUnder)->GetNeighbourWSquare(Pos, Index); }
  uchar GetWalkability() const;
  virtual uchar GetSquareWalkability() const { return GetWalkability(); }
 protected:
  gwterrain* GWTerrain;
  owterrain* OWTerrain;
};

#endif
