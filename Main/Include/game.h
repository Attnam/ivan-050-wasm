#ifndef __GAME_H__
#define __GAME_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include <map>
#include <vector>

#include "vector2d.h"
#include "femath.h"
#include "festring.h"

#ifndef LIGHT_BORDER
#define LIGHT_BORDER 80
#endif

#define PLAYER game::GetPlayer()

class area;
class level;
class dungeon;
class felist;
class team;
class character;
class gamescript;
class item;
class outputfile;
class inputfile;
class worldmap;
class god;
class square;
class wsquare;
class lsquare;
class bitmap;
class festring;
class rain;
class liquid;
struct explosion;

typedef std::map<festring, long> valuemap;

struct homedata
{
  vector2d Pos;
  uchar Dungeon;
  uchar Level;
  uchar Room;
};

outputfile& operator<<(outputfile&, const homedata*);
inputfile& operator>>(inputfile&, homedata*&);

#ifdef VC
#pragma pack(1)
#endif

struct configid
{
  configid() { }
  configid(ushort Type, ushort Config) : Type(Type), Config(Config) { }
  bool operator<(const configid& CI) const { return memcmp(this, &CI, sizeof(configid)) < 0; }
  ushort Type NO_ALIGNMENT;
  ushort Config NO_ALIGNMENT;
};

#ifdef VC
#pragma pack()
#endif

outputfile& operator<<(outputfile&, const configid&);
inputfile& operator>>(inputfile&, configid&);

struct dangerid
{
  dangerid() { }
  dangerid(float NakedDanger, float EquippedDanger) : NakedDanger(NakedDanger), EquippedDanger(EquippedDanger), HasBeenGenerated(false) { }
  float NakedDanger;
  float EquippedDanger;
  bool HasBeenGenerated;
};

outputfile& operator<<(outputfile&, const dangerid&);
inputfile& operator>>(inputfile&, dangerid&);

typedef std::map<configid, dangerid> dangermap;
typedef std::map<ulong, character*> characteridmap;
typedef std::map<ulong, item*> itemidmap;
typedef std::map<configid, ushort> massacremap;
typedef std::map<ulong, ulong> boneidmap;
typedef std::vector<item*> itemvector;
typedef std::vector<character*> charactervector;

class quitrequest { };
class areachangerequest { };

class game
{
 public:
  static bool Init(const festring& = CONST_S(""));
  static void DeInit();
  static void Run();
  static int GetMoveCommandKey(ushort);
  static const vector2d GetMoveVector(ushort Index) { return MoveVector[Index]; }
  static const vector2d GetRelativeMoveVector(ushort Index) { return RelativeMoveVector[Index]; }
  static const vector2d GetBasicMoveVector(ushort Index) { return BasicMoveVector[Index]; }
  static const vector2d GetLargeMoveVector(ushort Index) { return LargeMoveVector[Index]; }
  static area* GetCurrentArea() { return CurrentArea; }
  static level* GetCurrentLevel() { return CurrentLevel; }
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
  static bool BoolQuestion(const festring&, int = 0, int = 0);
  static void DrawEverything();
  static bool Save(const festring& = SaveName(""));
  static uchar Load(const festring& = SaveName(""));
  static bool IsRunning() { return Running; }
  static void SetIsRunning(bool What) { Running = What; }
  static bool EmitationHandler(long, long);
  static bool NoxifyHandler(long, long);
  static void UpdateCameraXWithPos(ushort);
  static void UpdateCameraYWithPos(ushort);
  static ushort GetCurrentLevelIndex() { return CurrentLevelIndex; }
  static int GetMoveCommandKeyBetweenPoints(vector2d, vector2d);
  static void DrawEverythingNoBlit(bool = false);
  static god* GetGod(ushort Index) { return God[Index]; }
  static const char* GetAlignment(ushort Index) { return Alignment[Index]; }
  static void ApplyDivineTick();
  static void ApplyDivineAlignmentBonuses(god*, short, bool);
  static vector2d GetDirectionVectorForKey(int);
  static festring SaveName(const festring& = CONST_S(""));
  static bool EyeHandler(long, long);
  static bool WalkabilityHandler(long, long);
  static void ShowLevelMessage();
  static float GetMinDifficulty();
  static void TriggerQuestForGoldenEagleShirt();
  static void CalculateGodNumber();
  static void Tick(ushort Tick = 1) { Ticks += Tick; }
  static ulong GetTicks() { return Ticks; }
  static festring GetAutoSaveFileName() { return AutoSaveFileName; }
  static uchar DirectionQuestion(const festring&, bool = true, bool = false);
  static void RemoveSaves(bool = true);
  static bool IsInWilderness() { return InWilderness; }
  static void SetIsInWilderness(bool What) { InWilderness = What; }
  static worldmap* GetWorldMap() { return WorldMap; }
  static void SetAreaInLoad(area* What) { AreaInLoad = What; }
  static void SetSquareInLoad(square* What) { SquareInLoad = What; }
  static area* GetAreaInLoad() { return AreaInLoad; }
  static square* GetSquareInLoad() { return SquareInLoad; }
  static ushort GetLevels();
  static dungeon* GetCurrentDungeon() { return Dungeon[CurrentDungeonIndex]; }
  static dungeon* GetDungeon(ushort Index) { return Dungeon[Index]; }
  static ushort GetCurrentDungeonIndex() { return CurrentDungeonIndex; }
  static void InitDungeons();
  static bool OnScreen(vector2d);
  static void DoEvilDeed(ushort);
  static void SaveWorldMap(const festring& = SaveName(""), bool = true);
  static worldmap* LoadWorldMap(const festring& = SaveName(""));
  static void UpdateCamera();
  static ulong CreateNewCharacterID(character*);
  static ulong CreateNewItemID(item*);
  static team* GetTeam(ushort Index) { return Team[Index]; }
  static uchar GetTeams() { return Teams; }
  static void Hostility(team*, team*);
  static void CreateTeams();
  static festring StringQuestion(const festring&, vector2d, ushort, ushort, ushort, bool);
  static long NumberQuestion(const festring&, vector2d, ushort);
  static void LOSTurn();
  static ulong GetLOSTurns() { return LOSTurns; }
  static void SendLOSUpdateRequest() { LOSUpdateRequested = true; }
  static void RemoveLOSUpdateRequest() { LOSUpdateRequested = false; }
  static character* GetPetrus() { return Petrus; }
  static void SetPetrus(character* What) { Petrus = What; }
  static bool HandleQuitMessage();
  static uchar GetDirectionForVector(vector2d);
  static const char* GetVerbalPlayerAlignment();
  static void CreateGods();
  static ushort GetScreenXSize() { return 42; }
  static ushort GetScreenYSize() { return 26; }
  static vector2d CalculateScreenCoordinates(vector2d Pos) { return (Pos - Camera + vector2d(1, 2)) << 4; }
  static void BusyAnimation();
  static void BusyAnimation(bitmap*);
  static vector2d PositionQuestion(const festring&, vector2d, void (*)(vector2d) = 0, void (*)(vector2d, int) = 0, bool = true);
  static void LookHandler(vector2d);
  static int AskForKeyPress(const festring&);
  static bool AnimationController();
  static gamescript* GetGameScript() { return GameScript; }
  static void InitScript();
  static valuemap& GetGlobalValueMap() { return GlobalValueMap; }
  static void InitGlobalValueMap();
  static void TextScreen(const festring&, ushort = 0xFFFF, bool = true, void (*)(bitmap*) = 0);
  static vector2d GetCursorPos() { return CursorPos; }
  static void SetCursorPos(vector2d What) { CursorPos = What; }
  static bool DoZoom() { return Zoom; }
  static void SetDoZoom(bool What) { Zoom = What; }
  static int KeyQuestion(const festring&, int, int, ...);
  static void LookKeyHandler(vector2d, int);
  static void NameKeyHandler(vector2d, int);
  static void End(bool = true, bool = true);
  static uchar CalculateRoughDirection(vector2d);
  static void InstallCurrentEmitter(vector2d, ulong);
  static void InstallCurrentNoxifier(vector2d Pos) { CurrentEmitterPos = Pos; }
  static long ScrollBarQuestion(const festring&, vector2d, long, long, long, long, long, ushort, ushort, ushort, void (*)(long) = 0);
  static bool IsGenerating() { return Generating; }
  static void SetIsGenerating(bool What) { Generating = What; }
  static void CalculateNextDanger();
  static int Menu(bitmap*, vector2d, const festring&, const festring&, ushort, const festring& = "", const festring& = "");
  static void InitDangerMap();
  static const dangermap& GetDangerMap();
  static bool TryTravel(uchar, uchar, uchar, bool = false);
  static bool LeaveArea(charactervector&, bool);
  static void EnterArea(charactervector&, uchar, uchar);
  static char CompareLights(ulong, ulong);
  static char CompareLightToInt(ulong, uchar);
  static void CombineLights(ulong&, ulong);
  static bool IsDark(ulong);
  static void SetStandardListAttributes(felist&);
  static void SignalGeneration(configid);
  static float GetAveragePlayerArmStrength() { return AveragePlayerArmStrength; }
  static float GetAveragePlayerLegStrength() { return AveragePlayerLegStrength; }
  static float GetAveragePlayerDexterity() { return AveragePlayerDexterity; }
  static float GetAveragePlayerAgility() { return AveragePlayerAgility; }
  static void InitPlayerAttributeAverage();
  static void UpdatePlayerAttributeAverage();
  static void CallForAttention(vector2d, ushort);
  static character* SearchCharacter(ulong);
  static item* SearchItem(ulong);
  static void AddCharacterID(character*, ulong);
  static void RemoveCharacterID(ulong);
  static void AddItemID(item*, ulong);
  static void RemoveItemID(ulong);
  static void UpdateItemID(item*, ulong);
  static uchar GetStoryState() { return StoryState; }
  static void SetStoryState(uchar What) { StoryState = What; }
  static void SetIsInGetCommand(bool What) { InGetCommand = What; }
  static bool IsInGetCommand() { return InGetCommand; }
  static festring GetHomeDir();
  static festring GetSaveDir();
  static festring GetGameDir();
  static festring GetBoneDir();
  static bool ExplosionHandler(long, long);
  static ulong CreateNewExplosionID() { return NextExplosionID++; }
  static void SetCurrentExplosion(const explosion* What) { CurrentExplosion = What; }
  static bool PlayerWasHurtByExplosion() { return PlayerHurtByExplosion; }
  static void SetPlayerWasHurtByExplosion(bool What) { PlayerHurtByExplosion = What; }
  static void SetCurrentArea(area* What) { CurrentArea = What; }
  static void SetCurrentLevel(level* What) { CurrentLevel = What; }
  static void SetCurrentWSquareMap(wsquare*** What) { CurrentWSquareMap = What; }
  static void SetCurrentLSquareMap(lsquare*** What) { CurrentLSquareMap = What; }
  static const festring& GetDefaultPolymorphTo() { return DefaultPolymorphTo; }
  static void SetDefaultPolymorphTo(const festring& What) { DefaultPolymorphTo = What; }
  static void SignalDeath(const character*, const character*);
  static void DisplayMassacreLists();
  static void DisplayMassacreList(const massacremap&, const char*, ulong);
  static bool MassacreListsEmpty();
#ifdef WIZARD
  static void ActivateWizardMode() { WizardMode = true; }
  static bool WizardModeIsActive() { return WizardMode; }
  static void SeeWholeMap();
  static uchar GetSeeWholeMapCheatMode() { return SeeWholeMapCheatMode; }
  static bool GoThroughWallsCheatIsActive() { return GoThroughWallsCheat; }
  static void GoThroughWalls() { GoThroughWallsCheat = !GoThroughWallsCheat; }
#else
  static bool WizardModeIsActive() { return false; }
  static uchar GetSeeWholeMapCheatMode() { return 0; }
  static bool GoThroughWallsCheatIsActive() { return false; }
#endif
  static bool WizardModeIsReallyActive() { return WizardMode; }
  static void CreateBone();
  static ushort GetQuestMonstersFound() { return QuestMonstersFound; }
  static void SignalQuestMonsterFound() { ++QuestMonstersFound; }
  static void SetQuestMonstersFound(ushort What) { QuestMonstersFound = What; }
  static bool PrepareRandomBone(ushort);
  static boneidmap& GetBoneItemIDMap() { return BoneItemIDMap; }
  static boneidmap& GetBoneCharacterIDMap() { return BoneCharacterIDMap; }
  static float CalculateAverageDanger(const charactervector&, character*);
  static float CalculateAverageDangerOfAllNormalEnemies();
  static character* CreateGhost();
  static bool TooGreatDangerFound() { return TooGreatDangerFoundBool; }
  static void SetTooGreatDangerFound(bool What) { TooGreatDangerFoundBool = What; }
  static void CreateBusyAnimationCache();
  static long GetScore();
  static void SetMoveType(uchar What) { MoveType = What; }
  static bool TweraifIsFree();
  static bool IsXMas();
  static ushort AddToItemDrawVector(item*);
  static void ClearItemDrawVector();
  static void ItemEntryDrawer(bitmap*, vector2d, ushort);
  static ushort AddToCharacterDrawVector(character*);
  static void ClearCharacterDrawVector();
  static void CharacterEntryDrawer(bitmap*, vector2d, ushort);
  static void GodEntryDrawer(bitmap*, vector2d, ushort);
  static itemvector& GetItemDrawVector() { return ItemDrawVector; }
  static charactervector& GetCharacterDrawVector() { return CharacterDrawVector; }
  static bool IsSumoWrestling() { return SumoWrestling; }
  static void SetIsSumoWrestling(bool What) { SumoWrestling = What; }
  static bool AllowHostilities() { return !SumoWrestling; }
  static bool AllBodyPartsVanish() { return SumoWrestling; }
  static bool TryToEnterSumoArena();
  static bool TryToExitSumoArena();
  static bool EndSumoWrestling(uchar);
  static character* GetSumo();
  static const festring& GetPlayerName() { return PlayerName; }
  static rain* ConstructGlobalRain();
  static void SetGlobalRainLiquid(liquid* What) { GlobalRainLiquid = What; }
  static bool PlayerIsSumoChampion() { return PlayerSumoChampion; }
 private:
  static const char* const Alignment[];
  static god** God;
  static ushort CurrentLevelIndex;
  static ushort CurrentDungeonIndex;
  static const int MoveNormalCommandKey[];
  static const int MoveAbnormalCommandKey[];
  static const vector2d MoveVector[];
  static const vector2d RelativeMoveVector[];
  static const vector2d BasicMoveVector[];
  static const vector2d LargeMoveVector[];
  static ushort*** LuxTable;
  static ushort* LuxTableSize;
  static bool Running;
  static character* Player;
  static vector2d Camera;
  static ulong Ticks;
  static festring AutoSaveFileName;
  static bool InWilderness;
  static worldmap* WorldMap;
  static area* AreaInLoad;
  static square* SquareInLoad;
  static dungeon** Dungeon;
  static ulong NextCharacterID;
  static ulong NextItemID;
  static ulong NextExplosionID;
  static team** Team;
  static ulong LOSTurns;
  static bool LOSUpdateRequested;
  static character* Petrus;
  static bool Loading;
  static gamescript* GameScript;
  static valuemap GlobalValueMap;
  static vector2d CursorPos;
  static bool Zoom;
  static ushort** CurrentRedLuxTable;
  static ushort** CurrentGreenLuxTable;
  static ushort** CurrentBlueLuxTable;
  static long CurrentEmitterPosXMinus16;
  static long CurrentEmitterPosYMinus16;
  static vector2d CurrentEmitterPos;
  static bool Generating;
  static dangermap DangerMap;
  static configid NextDangerID;
  static float AveragePlayerArmStrength;
  static float AveragePlayerLegStrength;
  static float AveragePlayerDexterity;
  static float AveragePlayerAgility;
  static characteridmap CharacterIDMap;
  static itemidmap ItemIDMap;
  static uchar Teams;
  static uchar Dungeons;
  static uchar StoryState;
  static bool InGetCommand;
  static const explosion* CurrentExplosion;
  static bool PlayerHurtByExplosion;
  static area* CurrentArea;
  static level* CurrentLevel;
  static wsquare*** CurrentWSquareMap;
  static lsquare*** CurrentLSquareMap;
  static festring DefaultPolymorphTo;
  static massacremap PlayerMassacreMap;
  static massacremap PetMassacreMap;
  static massacremap MiscMassacreMap;
  static ulong PlayerMassacreAmount;
  static ulong PetMassacreAmount;
  static ulong MiscMassacreAmount;
  static bool WizardMode;
  static uchar SeeWholeMapCheatMode;
  static bool GoThroughWallsCheat;
  static ushort QuestMonstersFound;
  static boneidmap BoneItemIDMap;
  static boneidmap BoneCharacterIDMap;
  static bool TooGreatDangerFoundBool;
  static bitmap* BusyAnimationCache[48];
  static uchar MoveType;
  static itemvector ItemDrawVector;
  static charactervector CharacterDrawVector;
  static bool SumoWrestling;
  static festring PlayerName;
  static liquid* GlobalRainLiquid;
  static vector2d GlobalRainSpeed;
  static ulong GlobalRainTimeModifier;
  static bool PlayerSumoChampion;
};

inline void game::CombineLights(ulong& L1, ulong L2)
{
  L1 = MakeRGB24(Max(GetRed24(L1), GetRed24(L2)), Max(GetGreen24(L1), GetGreen24(L2)), Max(GetBlue24(L1), GetBlue24(L2)));
}

inline bool game::IsDark(ulong Light)
{
  return !Light || (GetRed24(Light) < LIGHT_BORDER && GetGreen24(Light) < LIGHT_BORDER && GetBlue24(Light) < LIGHT_BORDER);
}

#endif
