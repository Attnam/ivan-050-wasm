#ifndef __GAME_H__
#define __GAME_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include <vector>

#include "typedef.h"
#include "vector2d.h"
#include "graphics.h"
#include "ivandef.h"
#include "save.h"

class area;
class material;
class level;
class character;
class felist;
class item;
class god;
class item;
class command;
class worldmap;
class square;
class dungeon;
class outputfile;
class inputfile;
class team;
class bitmap;
class petrus;
class gamescript;

class game
{
 public:
  static bool Init(const std::string& = "");
  static void DeInit();
  static void Run();
  static int GetMoveCommandKey(ushort Index) { return MoveCommandKey[Index]; }
  static const vector2d GetMoveVector(ushort Index) { return MoveVector[Index]; }
  static area* GetCurrentArea();
  static level* GetCurrentLevel();
  static bool WorldMapLOSHandler(long, long);
  static bool LevelLOSHandler(long, long);
  static ushort*** GetLuxTable() { return LuxTable; }
  static ushort* GetLuxTableSize() { return LuxTableSize; }
  static character* GetPlayer() { return Player; }
  static void SetPlayer(character*);
  static vector2d GetCamera() { return Camera; }
  static void UpdateCameraX();
  static void UpdateCameraY();
  static bool IsLoading() { return Loading; }
  static void SetIsLoading(bool What) { Loading = What; }
  static level* GetLevel(ushort);
  static void InitLuxTable();
  static void DeInitLuxTable();
  static const char* Insult();
  static bool BoolQuestion(const std::string&, int = NO, int = 0);
  static void DrawEverything();
  static bool Save(const std::string& = SaveName(""));
  static uchar Load(const std::string& = SaveName(""));
  static bool IsRunning() { return Running; }
  static void SetIsRunning(bool What) { Running = What; }
  static void ActivateWizardMode() { WizardMode = true; }
  static bool WizardModeActivated() { return WizardMode; }
  static void SeeWholeMap();
  static bool GetSeeWholeMapCheat() { return SeeWholeMapCheat; }
  static bool EmitationHandler(long, long);
  static bool NoxifyHandler(long, long);
  static bool GetGoThroughWallsCheat() { return GoThroughWallsCheat; }
  static void GoThroughWalls() { GoThroughWallsCheat = !GoThroughWallsCheat; }
  static void UpdateCameraXWithPos(ushort);
  static void UpdateCameraYWithPos(ushort);
  static bool KeyIsOK(char);
  static void SetCurrent(ushort What) { Current = What; }
  static ushort GetCurrent() { return Current; }
  static int GetMoveCommandKeyBetweenPoints(vector2d, vector2d);
  static void DrawEverythingNoBlit();
  static god* GetGod(ushort Index) { return God[Index]; }
  static uchar GetGods() { return God.size(); }
  static const std::string& GetAlignment(ushort Index) { return Alignment[Index]; }
  static void ApplyDivineTick(ushort = 1);
  static void ApplyDivineAlignmentBonuses(god*, bool, short = 25);
  static vector2d GetDirectionVectorForKey(int);
  static std::string SaveName(const std::string& = "");
  static bool EyeHandler(long, long);
  static long GodScore();
  static void ShowLevelMessage();
  static float Difficulty();
  static void TriggerQuestForGoldenEagleShirt();
  static void CalculateGodNumber();
  static long GetBaseScore() { return BaseScore; }
  static void Tick(ushort Tick = 1) { Ticks += Tick; }
  static ulong GetTicks() { return Ticks; }
  static std::string GetAutoSaveFileName() { return AutoSaveFileName; }
  static uchar DirectionQuestion(const std::string&, bool = true, bool = false);
  static command* GetCommand(ushort Index) { return Command[Index]; }
  static void RemoveSaves(bool = true);
  static bool IsInWilderness() { return InWilderness; }
  static void SetIsInWilderness(bool What) { InWilderness = What; }
  static worldmap* GetWorldMap() { return WorldMap; }
  static void SetWorldMap(worldmap* What) { WorldMap = What; }
  static void SetAreaInLoad(area* What) { AreaInLoad = What; }
  static void SetSquareInLoad(square* What) { SquareInLoad = What; }
  static area* GetAreaInLoad() { return AreaInLoad; }
  static square* GetSquareInLoad() { return SquareInLoad; }
  static uchar GetLevels();
  static dungeon* GetCurrentDungeon() { return Dungeon[CurrentDungeon]; }
  static dungeon* GetDungeon(ushort Index) { return Dungeon[Index]; }
  static void SetCurrentDungeon(uchar What) { CurrentDungeon = What; }
  static void InitDungeons();
  static bool OnScreen(vector2d);
  static void DoGoodDeed(ushort);
  static void DoNeutralDeed(ushort);
  static void DoEvilDeed(ushort);
  static void SaveWorldMap(const std::string& = SaveName(""), bool = false);
  static void LoadWorldMap(const std::string& = SaveName(""));
  static void UpdateCamera();
  static ulong CreateNewItemID();
  static void PopItemID(ulong ID) { if(NextItemID == ID + 1) --NextItemID; }
  static team* GetTeam(ushort Index) { return Team[Index]; }
  static uchar GetTeams() { return Team.size(); }
  static void Hostility(team*, team*);
  static void CreateTeams();
  static std::string StringQuestion(const std::string&, vector2d, ushort, ushort, ushort, bool);
  static long NumberQuestion(const std::string&, vector2d, ushort);
  static void LOSTurn();
  static ulong GetLOSTurns() { return LOSTurns; }
  static void SendLOSUpdateRequest() { LOSUpdateRequested = true; }
  static void RemoveLOSUpdateRequest() { LOSUpdateRequested = false; }
  static petrus* GetPetrus() { return Petrus; }
  static void SetPetrus(petrus* What) { Petrus = What; }
#ifndef __DJGPP__
  static bool HandleQuitMessage();
#endif
  static void Beep();
  static uchar GetDirectionForVector(vector2d);
  static void SetIsInGetCommand(bool What) { InGetCommand = What; }
  static bool IsInGetCommand() { return InGetCommand; }
  static std::string GetVerbalPlayerAlignment();
  static void CreateGods();
  static vector2d GetScreenSize() { return ScreenSize; }
  static void SetScreenSize(vector2d What) { ScreenSize = What; }
  static vector2d game::CalculateScreenCoordinates(vector2d Pos) { return (Pos - Camera + vector2d(1, 2)) << 4; }
  static void BusyAnimation(bitmap* = DOUBLEBUFFER);
  static vector2d PositionQuestion(const std::string&, vector2d, void (*)(vector2d) = 0, void (*)(vector2d, int) = 0, bool = true);
  static void LookHandler(vector2d);
  static int AskForKeyPress(const std::string&);
  static bool AnimationController();
  static gamescript* GetGameScript() { return GameScript; }
  static void InitScript();
  static const valuemap& GetGlobalValueMap() { return GlobalValueMap; }
  static void InitGlobalValueMap();
  static void TextScreen(const std::string&, ushort = 0xFFFF, bool = true, void (*)(bitmap*) = 0);
  static vector2d GetCursorPos() { return CursorPos; }
  static void SetCursorPos(vector2d What) { CursorPos = What; }
  static bool DoZoom() { return Zoom; }
  static void SetDoZoom(bool What) { Zoom = What; }
  static int KeyQuestion(const std::string&, int, int, ...);
  static void LookKeyHandler(vector2d, int);
  static void NameKeyHandler(vector2d, int);
  static const std::string& GetLockDescription(ushort Index) { return LockDescription[Index]; }
  static void End(bool = true, bool = true);
  static uchar CalculateRoughDirection(vector2d);
  static void SetCurrentEmitterEmitation(ushort What) { CurrentEmitterEmitation = What; }
  static void SetCurrentEmitterPos(vector2d);
  static long ScrollBarQuestion(const std::string&, vector2d, long, long, long, long, ushort, ushort, ushort, void (*)(long) = 0);
  static bool IsGenerating() { return Generating; }
  static void SetIsGenerating(bool What) { Generating = What; }
 private:
  static std::string Alignment[];
  static std::vector<god*> God;
  static ushort Current;
  static uchar CurrentDungeon;
  static int MoveCommandKey[];
  static const vector2d MoveVector[];
  static ushort*** LuxTable;
  static ushort* LuxTableSize;
  static bool Running;
  static character* Player;
  static vector2d Camera;
  static bool WizardMode;
  static bool SeeWholeMapCheat;
  static bool GoThroughWallsCheat;
  static long BaseScore;
  static ulong Ticks;
  static std::string AutoSaveFileName;
  static command* Command[];
  static bool InWilderness;
  static worldmap* WorldMap;
  static area* AreaInLoad;
  static square* SquareInLoad;
  static std::vector<dungeon*> Dungeon;
  static ulong NextItemID;
  static std::vector<team*> Team;
  static ulong LOSTurns;
  static bool LOSUpdateRequested;
  static petrus* Petrus;
  static bool InGetCommand;
  static bool Loading;
  static vector2d ScreenSize;
  static gamescript* GameScript;
  static valuemap GlobalValueMap;
  static vector2d CursorPos;
  static bool Zoom;
  static std::string LockDescription[];
  static ushort CurrentEmitterEmitation;
  static long CurrentEmitterPosX;
  static long CurrentEmitterPosY;
  static vector2d CurrentEmitterPos;
  static bool Generating;
};

#endif

