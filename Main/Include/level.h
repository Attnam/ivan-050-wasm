#ifndef __LEVEL_H__
#define __LEVEL_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include <vector>

#include "area.h"

class levelscript;
class roomscript;
class squarescript;
class glterrain;
class olterrain;
class dungeon;
class lsquare;
class room;

struct explosion
{
  character* Terrorist;
  std::string DeathMsg;
  vector2d Pos;
  ulong ID;
  ushort Strength;
  ushort RadiusSquare;
  uchar Size;
  bool HurtNeutrals;
};

class level : public area
{
 public:
  level();
  virtual ~level();
  void Generate(ushort);
  vector2d GetRandomSquare(const character* = 0, uchar = 0, const rect* = 0) const;
  void GenerateMonsters();
  lsquare* GetLSquare(vector2d Pos) const { return Map[Pos.X][Pos.Y]; }
  lsquare* GetLSquare(ushort x, ushort y) const { return Map[x][y]; }
  void GenerateTunnel(ushort, ushort, ushort, ushort, bool);
  void ExpandPossibleRoute(ushort, ushort, ushort, ushort, bool);
  void ExpandStillPossibleRoute(ushort, ushort, ushort, ushort, bool);
  void Save(outputfile&) const;
  void Load(inputfile&);
  void FiatLux();
  ushort GetIdealPopulation() const { return IdealPopulation; }
  ushort GetDifficulty() const { return Difficulty; }
  void GenerateNewMonsters(ushort, bool = true);
  void AttachPos(ushort, ushort);
  void AttachPos(vector2d Pos) { AttachPos(Pos.X, Pos.Y); }
  void CreateItems(ushort);
  bool MakeRoom(const roomscript*);
  void ParticleTrail(vector2d, vector2d);
  std::string GetLevelMessage() { return LevelMessage; }
  void SetLevelMessage(const std::string& What) { LevelMessage = What; }
  void SetLevelScript(const levelscript* What) { LevelScript = What; }
  bool IsOnGround() const;
  const levelscript* GetLevelScript() const { return LevelScript; }
  virtual void MoveCharacter(vector2d, vector2d);
  ushort GetLOSModifier() const;
  ushort CalculateMinimumEmitationRadius(ulong) const;
  room* GetRoom(ushort) const;
  void SetRoom(ushort Index, room* What) { Room[Index] = What; }
  void AddRoom(room*);
  void Explosion(character*, const std::string&, vector2d, ushort, bool = true);
  bool CollectCreatures(std::vector<character*>&, character*, bool);
  void ApplyLSquareScript(const squarescript*);
  virtual void Draw(bool) const;
  lsquare* GetNeighbourLSquare(vector2d, ushort) const;
  vector2d GetEntryPos(const character*, uchar) const;
  void GenerateRectangularRoom(std::vector<vector2d>&, std::vector<vector2d>&, std::vector<vector2d>&, const roomscript*, room*, vector2d, vector2d);
  void Reveal();
  static void (level::*GetBeam(ushort))(character*, const std::string&, vector2d, ulong, uchar, uchar, uchar);
  void ParticleBeam(character*, const std::string&, vector2d, ulong, uchar, uchar, uchar);
  void LightningBeam(character*, const std::string&, vector2d, ulong, uchar, uchar, uchar);
  void ShieldBeam(character*, const std::string&, vector2d, ulong, uchar, uchar, uchar);
  dungeon* GetDungeon() const { return Dungeon; }
  void SetDungeon(dungeon* What) { Dungeon = What; }
  uchar GetIndex() const { return Index; }
  void SetIndex(uchar What) { Index = What; }
  bool DrawExplosion(const explosion&) const;
  ushort TriggerExplosions(ushort);
  lsquare*** GetMap() const { return Map; }
 protected:
  void GenerateLanterns(ushort, ushort, uchar) const;
  void CreateRoomSquare(glterrain*, olterrain*, ushort, ushort, uchar, uchar) const;
  lsquare*** Map;
  const levelscript* LevelScript;
  std::string LevelMessage;
  std::vector<vector2d> Door;
  std::vector<room*> Room;
  uchar IdealPopulation;
  uchar MonsterGenerationInterval;
  uchar Difficulty;
  dungeon* Dungeon;
  uchar Index;
  std::vector<explosion> ExplosionQueue;
  std::vector<bool> PlayerHurt;
};

outputfile& operator<<(outputfile&, const level*);
inputfile& operator>>(inputfile&, level*&);

#endif
