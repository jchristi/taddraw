#ifndef tamemH
#define tamemH

#include <dsound.h>

#pragma pack(1)

struct PlayerStruct;
struct PlayerInfoStruct;
struct UnitStruct;
struct UnitOrdersStruct;
struct WeaponStruct;
struct MapFileStruct;
struct UnitDefStruct;
struct GafAnimStruct;
struct Object3doStruct;
struct PrimitiveStruct;
struct PrimitiveInfoStruct;
struct ProjectileStruct;
struct FeatureDefStruct;
struct FXGafStruct;
struct FeatureStruct;
struct WreckageInfoStruct;
struct DebrisStruct;
struct Unk1Struct;
struct Point3;
struct SmokeListNode;
struct ParticleSystemStruct;
struct SmokeListNode;
struct ParticleBase;
struct SmokeGraphics;
struct RadarPicStruct;
struct DSoundStruct;
struct Context_Screen;
struct RaceSideData;
struct AllyStruct;
struct MapDataStruct;

struct Point3{
	int x;
	int y;
	int z;
};


struct WeaponStruct {
	char WeaponName[0x20];
	char WeaponDescription[0x40];
	int Unkn1;
	char data3[20];
	GafAnimStruct *LandExplodeAsGFX;
	GafAnimStruct *WaterExplodeAsGFX;
	char data4[86];
	short Damage; //d6
	short AOE;
	float EdgeEffectivnes;
	short Range;
	char data5[21];
	float ShakeMagnitude;
	char data6[17];
	unsigned char ID;
	char data8[1];
	char RenderType;
	char data7[8];
};  //0x115

struct ExplosionStruct{
	DebrisStruct *Debris;
	short Frame;
	char data2[6];
	FXGafStruct *FXGaf;
	char data3[12];
	int XPos;
	int ZPos;
	int YPos;
	char data4[36];
	short XTurn; //0x4c
	short ZTurn;
	short YTurn;
	char data5[2];
};//0x54

struct DebrisStruct{
	char data1[0x24];
	Point3 *Vertices;
	Unk1Struct *Unk;
	char data2[8];
};//0x34

struct Unk1Struct{
	char data1[0x18];
	FXGafStruct *Texture;
	char data2[0x4];
};//0x20

struct FXGafStruct{
	short Frames;
	char data1[6];
	char Name[0x20];
	int FramePointers[1];
};

struct DSoundStruct{
	char data1[0x24];
	LPDIRECTSOUND Directsound;
	LPDIRECTSOUNDBUFFER DirectsoundBuffer;
	char data2[0xC];
	LPDIRECTSOUNDBUFFER DirectsoundBuffer2;
};
//settimer 4fb368
/*
struct TAdynmemStruct{
char data21[0x10];
DSoundStruct *DSound;
char data1[0x1B4F];

PlayerStruct Players[10];	//0x1B63 , end at 0x2851

char data4[0x43D];
short BuildPosX; //0x2C8E
short BuildPosY;
int BuildPosRealX; //0x2C92
int Height;
int BuildPosRealY;
int unk1;
int Height2;

char data22[0x6];

short MouseMapPosX;  //0x2CAC
char data16[6];
short MouseMapPosY;  //0x2CB4
char data23[4];
unsigned short MouseOverUnit; //0x2CBA
char data17[0x8];
short BuildNum;  //0x2CC4,  unitindex for selected OwnUnitBegin to build
char BuildSpotState; //0x40=notoktobuild
char data18[0x2C];

WeaponStruct Weapons[256];  //0x2CF3  size=0x11500
//char data7[4];
int NumProjectiles;
ProjectileStruct *Projectiles; //0x141F7
char data13[0x10];
WreckageInfoStruct *WreckageInfo; //0x1420B
char data14[0x24];
int FeatureMapSizeX; //0x14233
int FeatureMapSizeY; //0x14237
char data7[0x18];
int NumFeatureDefs;
char data15[0x18];
FeatureDefStruct *FeatureDef; //0x1426F
char data8[8];
LPVOID	*EyeBallMemory;  //0x1427B
char data12[8];
FeatureStruct *Features; //0x14287
char data3[0x40];
tagRECT MinimapRect;//0x142CB
RadarPicStruct *RadarFinal; //0x142DB
RadarPicStruct *RadarMapped;  //0x142DF
RadarPicStruct *RadarPicture;  //0x142E3
char data20[4];
short RadarPicSizeX;  //0x142EB
short RadarPicSizeY;  //0x142ED
char data25[4];
int CirclePointer;//0x142F3 //used in drawcircle funktion
char data19[0x28];
int MapX;	//0x1431f
int MapY;   //0x14323
int MapXScrollingTo; //0x14327
int MapYScrollingTo; //0x1432B
char data24[0x24];
DWORD UnitsCounter;
UnitStruct *OwnUnitBegin;//UnitStruct *
UnitStruct *OwnUnitEnd; //0x1435B UnitStruct *
short int *HotUnits;//0x1435F
short int *HotRadarUnits;
int NumHotUnits; //0x14367
int NumHotRadarUnits;
char data5[0x2c];
UnitDefStruct *UnitDef;  //0x1439b
char data11[0x57C];
int NumExplosions; //0x1491B
//char data9[0x6270];
ExplosionStruct Explosions[300]; //0x1491F
LPVOID Unk2; //0x1AB8F
char data10[0x1DEB4];
int GameTime; //0x38A47
char ShotUrl;// 0x38a47+c= TA截图目录的字符串，即TA目录+当前用户名 
char data6[0x792];
MapFileStruct *MapFile; //0x391E9
};
*/

struct WreckageInfoStruct{
	int unk1;
	LPVOID unk2;
	int XPos;
	int ZPos;
	int YPos;
	char data1[0xC];
	short ZTurn;
	short XTurn;
	short YTurn;
	char data2[0xA];
};

struct FeatureStruct{
	char data1[8];
	short FeatureDefIndex;
	short WreckageInfoIndex;
	char data2[1];
}; //0xD

struct FeatureDefStruct {
	char Name[0x20];
	char data1[0x60];
	char Description[20];
	char Data2[108];
}; //0x100

struct ProjectileStruct {
	WeaponStruct *Weapon;
	int XPos;
	int ZPos;
	int YPos;
	int XPosStart;
	int ZPosStart;
	int YPosStart;
	int XSpeed;
	int ZSpeed;
	int YSpeed;
	char data1[14];
	short XTurn;
	short ZTurn;
	short YTurn;
	char data2[45];
	struct {
		bool unk1 : 1;
		bool Inactive : 1;
		char unk2 : 6;
	} Inactive;
	char data3[1];
}; //0x6B

struct MapFileStruct{
	char data[0x204];
	char TNTFile[MAX_PATH];
};

struct PlayerInfoStruct{
	char MapName[0x20];
	char data1[0x76];
	char PlayerColor;
};


struct Object3doStruct {
	short NumParts;
	char data1[2];
	int TimeVisible;
	char data2[4];
	UnitStruct *ThisUnit;
	LPVOID *UnkPTR1;
	LPVOID *UnkPTR2;
	char data3[6];
	PrimitiveStruct *BaseObject;
};

struct PrimitiveInfoStruct{
	char data1[28];
	char *Name;
};

struct PrimitiveStruct{
	PrimitiveInfoStruct *PrimitiveInfo;
	int XPos;
	int ZPos;
	int YPos;
	unsigned short XTurn;
	unsigned short ZTurn;
	unsigned short YTurn;
	char data3[18];
	struct {
		bool Visible: 1;
		bool unk1 : 7;
	} Visible;
	char data2[1];
	PrimitiveStruct *SiblingObject;
	PrimitiveStruct *ChildObject;
	PrimitiveStruct *ParrentObject;
}; //0x36


struct UnitOrdersStruct {
	char data1[30];
	UnitOrdersStruct *ThisPTR;
	//char data2[2];
	int PosX;
	int PosZ;
	int PosY;
	char data3[6];
	char FootPrint;
	char data4[12];
	struct {
		char unk1 : 4;
		bool ThisStart : 1;
		char unk2 : 3;
	} ThisStart;
	char data5[6];
	UnitOrdersStruct *NextOrder;
};

struct GafAnimStruct {

};

struct RadarPicStruct{
	int XSize;
	int YSize;
	int Unk1;
	LPVOID *PixelPTR;
};

struct ParticleSystemStruct{
	LPVOID DrawFunc; //4FD5F8 wake or smoke?; 4FD638 - Smoke1, 4FD618 - Smoke2; 4FD5B8, 4FD5A8 - Nanolath; 4FD5D8 - fire; //?
	char data1[8];
	int Type; //1 smoke, 2 wake, 6 nano, 7 fire
	SmokeGraphics* firstDraw;		//0 om denna partikel ej 鋜 aktiv (?)
	SmokeGraphics* lastDraw;			//rita alla fram till men inte denna ?
	LPVOID *Unk;				//? inte f鰎 sista ?
	char data2[48];
};//76

struct ParticleBase{
	char data[8];
	ParticleSystemStruct **particles;		
	char data2[8];
	ParticleSystemStruct **ParticlePTRArray;
	int SmokeParticleStructSize; //? 76
	int maxParticles; //? 1000
	int curParticles;			//antalet aktiva i arrayen men de 鋜 inte n鰀v鋘digtvis i ordning
};

struct SmokeGraphics{
	FXGafStruct* gaf;
	int XPos,ZPos,YPos;
	int unknown;
	int frame;
	int unknown2;
	int MoreSubs;  //0 ifall inga fler subparticles efter denna
};//0x20

/*struct SmokeListNode{
SmokeParticleStruct* next;
SmokeParticleStruct* me;
};*///?

struct Context_Screen
{
	int Width;
	int Height;
	int PixelPerLine; // TA is8bits colors mode, so Just Equ Width
	LPVOID ScreenColorBits_Ptr;// 0xc
	char unknow_[0xc];
	RECT ScreenRect;// 0x1c
	int unknow_1;
};

typedef struct 
{
	WORD   Left;
	WORD   Top;
	WORD   Right;
	WORD   Buttom;
	BYTE   TransparentColorMask;
	BYTE   IfPackedFont;
	BYTE   field_A;
	DWORD   FontColorBits_Ptr;
}FontStruct,*pFontStruct;
typedef struct 
{
	BYTE   name[30];
	BYTE   nameprefix[4];
	BYTE   commanderUnitName[32];
	RECT   Logo_rect;
	RECT   Energy_rect;
	RECT   EnergyNum_rect;
	RECT   METALBAR_rect;
	RECT   METALNUM_rect;
	RECT   TOTALUNITS_rect;
	RECT   TOTALTIME_rect;
	RECT   ENERGYMAX_rect;
	RECT   METALMAX_rect;
	RECT   ENERGY0_rect;
	RECT   METAL0_rect;
	RECT   ENERGYPRODUCED;
	RECT   ENERGYCONSUMED;
	RECT   METALPRODUCED;
	RECT   METALCONSUMED;
	RECT   LOGO2;
	RECT   UNITNAME;
	RECT   DAMAGEBAR;
	RECT   UNITENERGYMAKE;
	RECT   UNITENERGYUSE;
	RECT   UNITMETALMAKE;
	RECT   UNITMETALUSE;
	RECT   MISSIONTEXT;
	RECT   UNITNAME2;
	RECT   field_1C2;
	RECT   NAME;
	RECT   DESCRIPTION;
	RECT   RELOAD_RaceID;
	BYTE   field_202[32];
	DWORD   energycolor;
	DWORD   metalcolor;
	DWORD   field_22A;
	DWORD   Font_File;
}RaceSideData,*pRaceSideData;
typedef struct 
{
	DWORD   PlayerType;
	DWORD   RaceSide;
	DWORD   AllyTeamID;
	DWORD   StartMetal;
	DWORD   StartEnergy;
	DWORD   field_14;
	DWORD   field_18;
}AllyStruct,*pAllyStruct;

typedef struct 
{
	DWORD   ValidMapStartPos_;
	DWORD   PlayerID;
	WORD   X_Pos;
	WORD   Y_Pos;
}MapStartPos,*pMapStartPos;

typedef struct TAdynmemStruct
{
	BYTE   TAVersionStr[12];
	DWORD   TAProgramStruct_Ptr;
	DWORD   DSoundP;
	DWORD   HAPINET_Object;
	BYTE   unknow_0_[1201];
	DWORD   unknow_0__;
	DWORD   unknow_0___;
	BYTE   unknow_0____[72];
	DWORD   TAIngameGUIStruct;
	BYTE   unknow_0______[108];
	BYTE   AlteredUnitLimit;
	BYTE   field_58A[2127];
	BYTE   PlayerCameraRectColorInRadar;
	BYTE   field_DDA[1301];
	DWORD   ChatTextBegin;
	BYTE   field_12F3[59];
	DWORD   field_132E;
	BYTE   field_1332[1782];
	DWORD   field_1A28;
	BYTE   field_1A2C[311];
	PlayerStruct   PlayerStructArray[10];
	BYTE   unknow_2[331];
	DWORD   unknow_2_;
	DWORD   AllyStruct_Ptr;
	BYTE   field_29A4[144];
	DWORD   PacketBufferSize;
	DWORD   PacketBuffer_p;
	WORD   CurrentPlayerNumbers;
	DWORD   ChatTextIndex;
	BYTE   HumanPlayer_EyeBallID;
	BYTE   LOS_Sight_PlayerID_Game;
	BYTE   NetworkLayerEnabled;
	BYTE   unknow_2____[585];
	WORD   BuildPosX;
	WORD   BuildPosY;
	DWORD   BuildPosRealX;
	DWORD   Height;
	DWORD   BuildPosRealY;
	DWORD   unknow_3;
	DWORD   Height2;
	BYTE   unknow_4[6];
	WORD   MouseMapPosX;
	BYTE   unknow_5[6];
	WORD   MouseMapPosY;
	BYTE   unknow_6[4];
	WORD   MouseOverUnit;
	WORD   unknow_7;
	BYTE   field_2CBE;
	BYTE   field_2CBF[5];
	WORD   BuildNum;
	BYTE   BuildSpotState;
	BYTE   unknow_8[44];
	WeaponStruct   WeaponsTypedefArray[256];
	DWORD   NumProjectiles;
	DWORD   Projectiles_p;
	BYTE   MapDataStruct_NotPtr[16];
	DWORD   WreckageInfo_p;
	BYTE   unknow_10[28];
	DWORD   RadarPictureSize_InGameMeasure;
	DWORD   RadarPictureHeight_InGameMeasure;
	DWORD   FeatureMapSizeX;
	DWORD   FeatureMapSizeY;
	DWORD   EyeBallMapWidth;
	DWORD   EyeBallMapHeight;
	BYTE   field_14243[16];
	DWORD   NumFeatureDefs;
	BYTE   unknow_12[20];
	DWORD   TEDGENERATEDPIC;
	DWORD   FeatureDef_p;
	BYTE   unknow_13[8];
	DWORD   EyeBallMemory_p;
	WORD   unknow_14;
	WORD   MovingEyeBall;
	BYTE   field_14283[4];
	DWORD   FeaturesArray_p;
	BYTE   unknow_15[48];
	DWORD   field_142BB;
	DWORD   field_142BF;
	DWORD   field_142C3;
	DWORD   field_142C7;
	BYTE   MinimapEyeBallRect[16];
	DWORD   RadarFinal_p;
	DWORD   RadarMapped_p;
	DWORD   RadarPicture_p;
	WORD   XOffset_RadarMap_MiniMapMeasure;
	WORD   YOffset_RadarMap_MiniMapMeasure;
	WORD   RadarPicSizeWidth_MiniMapMeasure;
	WORD   RadarPicSizeHeight_MiniMapMeasure;
	WORD   unknow_16;
	WORD   field_142F1;
	DWORD   CirclePointer;
	BYTE   unknow_17[40];
	DWORD   EyeBallMapX;
	DWORD   EyeBallMapY;
	DWORD   EyeBallMapXScrollingTo_;
	DWORD   EyeBallMapYScrollingTo_;
	BYTE   unknow_18[30];
	WORD   scrollspeed;
	WORD   EveryPlayerUnitsNumber;
	BYTE   TotalGameUnits[6];
	DWORD   BeginUnitsArray_p;
	DWORD   EndOfUnitsArray_p;
	DWORD   HotUnits_AryPtr;
	DWORD   HotRadarUnits_pw;
	DWORD   NumHotUnits;
	DWORD   NumHotRadarUnits;
	BYTE   unknow_20[32];
	DWORD   UnitsTypedefArrayCount_p;
	BYTE   field_14393[8];
	DWORD   UnitsTypedefArray_p;
	BYTE   unknow_21[1088];
	DWORD   _0_FontValidCharNumber;
	DWORD   _1_FontValidCharNumber;
	BYTE   field_147E7[308];
	DWORD   NumExplosions;
	ExplosionStruct   ExplosionsAry[300];
	DWORD   unknow_p_23;
	BYTE   unknow_24[102012];
	DWORD   NoVoiceButtonAryCounter;
	BYTE   field_33A13[1024];
	BYTE   NoVoiceButtonAry;
	BYTE   field_33E14[16383];
	DWORD   SoundClassAry;
	DWORD   SoundClassNumber;
	DWORD   field_37E1B;
	BYTE   field_37E1F[24];
	DWORD   InGamePos_X;
	DWORD   InGamePos_Y;
	BYTE   field_37E3F[167];
	WORD   EveryPlayerUnitsNumber_0;
	BYTE   field_37EE8[6];
	DWORD   Difficulty;
	DWORD   side;
	DWORD   field_37EF6;
	DWORD   InterfaceType;
	DWORD   field_37EFE;
	DWORD   SingleLOSType;
	DWORD   screenchat;
	BYTE   field_37F0A[15];
	WORD   SoundMode;
	DWORD   DisplaymodeWidth;
	DWORD   DisplaymodeHeight;
	BYTE   field_37F23[12];
	WORD   field_37F2F;
	BYTE   field_37F31[12];
	RaceSideData   RaceSideDataAry[5];
	BYTE   field_38A37[842];
	DWORD   TotalPlayers_CanBe;
	BYTE   field_38D85[1124];
	DWORD   GameSettingStruct_Ptr;
	BYTE   field_391ED[44];
	DWORD   SingleCommanderDeath;
	DWORD   SingleMapping;
	DWORD   SingleLineOfSight;
	DWORD   SingleLOSType_;
	BYTE   field_39229[3543];
}TAMainStruct,*pTAMainStruct;

typedef struct tagTAProgramStruct
{
	DWORD   HInstance;
	DWORD   field_4;
	DWORD   WindowsClassName;
	DWORD   WindowsName;
	BYTE   field_10[48];
	DWORD   TAMainWndHwnd;
	DWORD   WholeScreen_TAWindowDIBSECTION;
	DWORD   WndMemHDC_TAWindowHDC;
	DWORD   Palette_TAWindowHPALETTE;
	DWORD   field_50;
	DWORD   BackMemHDC_TAWindowHDC;
	BYTE   field_58[44];
	DWORD   TAlplpDD_TAFullScreen;
	DWORD   TAlpTADrawSurface_TAFullScreen;
	DWORD   lplp_unknow_TAFullScreen;
	BYTE   field_90[44];
	DWORD   MainScreenContext;
	BYTE   field_C0[20];
	DWORD   TA_LPDDSURFACEDESC;
	DWORD   lplpTADrawSurface;
	DWORD   GameScreenUpdatingFlag_1;
	BYTE   field_E0[16];
	WORD   field_F0;
	DWORD   GameScreenUpdatingFlag_2;
	DWORD   InputBuffer[30];
	DWORD   TotalChar_InputBuffer;
	DWORD   ReadChar_InputBuffer;
	BYTE   field_176[1186];
	DWORD   HPIFileAryPtr;
	DWORD   HPIFileNumber;
}TAProgramStruct,*pTAProgramStruct;

typedef struct tagPlayerStruct
{
	DWORD   PlayerActive;
	DWORD   field_4;
	BYTE   PlayerNum;
	DWORD   field_10;
	BYTE   field_22;
	DWORD   PlayerInfo;
	BYTE   Name[30];
	BYTE   SecondName[30];
	UnitStruct   Units_Begin;
	UnitsInGameStructPtr *   Units_End;
	BYTE   PlayerType;
	DWORD   field_74;
	DWORD   field_7C;
	DWORD   field_88;
	PlayerResourcesStruct   Resource;
	DWORD   UpdateTime;
	DWORD   WinLoseTime;
	DWORD   DisplayTimer;
	WORD   Kills_word;
	WORD   Losses;
	BYTE   field_108[10];
	DWORD   UnitsCounters;
	WORD   UnitsNumber;
	BYTE   PlayerNumber;
	WORD   AddPlayerStorage_word;
}PlayerStruct,*pPlayerStruct;

typedef struct tagPlayerResourcesStruct
{
	BYTE   fCurrentEnergy[4];
	BYTE   fEnergyProducton[4];
	BYTE   fEnergyExpense[4];
	BYTE   fCurrentMetal[4];
	BYTE   fMetalProduction[4];
	BYTE   fMetalExpense[4];
	BYTE   fMaxEnergyStorage[4];
	BYTE   fMaxMetalStorage[4];
	BYTE   fTotalEnergyProduced[8];
	BYTE   fTotalMetalProduced[8];
	BYTE   fTotalEnergyConsumed[8];
	BYTE   fTotalMetalConsumed[8];
	BYTE   fEnergyWasted[8];
	BYTE   fMetalWasted[8];
	DWORD   fPlayerEnergyStorage;
	BYTE   PlayerMetalStorage[8];
}PlayerResourcesStruct,*pPlayerResourcesStruct;


enum UnitSelectState
{
	UnitValid        = 0x4,
	UnitSelected     = 0x10,
	UnitValid2       = 0x20
};



typedef struct tagUnitDefStruct
{
	BYTE   Name[32];
	BYTE   UnitName[32];
	BYTE   UnitDescription[64];
	BYTE   ObjectName[32];
	BYTE   Side[3];
	BYTE   data5[167];
	WORD   FootX;
	WORD   FootY;
	DWORD   YardMap;
	BYTE   unknow_1[12];
	DWORD   XWidth;
	BYTE   unknow_2[4];
	DWORD   YWidth;
	BYTE   unknow_3[4];
	DWORD   ZWidth;
	DWORD   unknow_4;
	DWORD   field_176;
	DWORD   field_17A;
	DWORD   field_17E;
	DWORD   field_182;
	DWORD   buildcostenergy;
	DWORD   buildcostmetal;
	DWORD   field_18E;
	DWORD   lRawSpeed_maxvelocity;
	BYTE   data9[8];
	DWORD   acceleration;
	DWORD   bankscale;
	DWORD   pitchscale;
	DWORD   damagemodifier;
	DWORD   moverate1;
	DWORD   moverate2;
	DWORD   movementclass;
	WORD   turnrate;
	WORD   corpse;
	WORD   maxwaterdepth;
	WORD   minwaterdepth;
	DWORD   energymake;
	DWORD   energyuse;
	DWORD   metalmake;
	DWORD   extractsmetal;
	DWORD   windgenerator;
	DWORD   tidalgenerator;
	DWORD   cloakcost;
	DWORD   cloakcostmoving;
	DWORD   energystorage;
	DWORD   metalstorage;
	DWORD   buildtime;
	DWORD   weapon1;
	DWORD   weapon2;
	DWORD   weapon3;
	WORD   nMaxHP;
	BYTE   data8[2];
	WORD   nWorkerTime;
	WORD   nHealTime;
	WORD   nSightDistance;
	WORD   nRadarDistance;
	WORD   nSonarDistance;
	WORD   mincloakdistance;
	WORD   radardistancejam;
	WORD   sonardistancejam;
	WORD   SoundClassIndex;
	WORD   nBuildDistance;
	BYTE   builddistance[2];
	WORD   nManeuverLeashLength;
	WORD   attackrunlength;
	WORD   kamikazedistance;
	WORD   sortbias;
	BYTE   cruisealt;
	BYTE   data4;
	WORD   UnitTypeID;
	DWORD   explodeas;
	DWORD   selfdestructas;
	BYTE   maxslope;
	BYTE   badslope;
	BYTE   transportsize;
	BYTE   transportcapacity;
	BYTE   waterline;
	BYTE   makesmetal;
	BYTE   bmcode;
	BYTE   defaultmissiontypeIndex;
	DWORD   wpri_badTargetCategory_MaskAryPtr;
	DWORD   wsec_badTargetCategory_MaskAryPtr;
	DWORD   wspe_badTargetCategory_MaskAryPtr;
	DWORD   noChaseCategory_MaskAryPtr;
#define COMMANDER_MASKBIT (1<< 0x12)
#define WEAPONUNIT_MASKBIT (0x10000)
	DWORD   UnitTypeMask_;
#define CANFLY_MASKBIT (1<< 0xb)
	DWORD   UnitTypeMask_1;
}UnitDefStruct,*pUnitDefStruct;



typedef struct UnitStruct
{
	DWORD   MoveClass;
	DWORD   UsedSpot;
	DWORD   field_8;
	DWORD   field_C;
	DWORD   Weapon1_p;
	BYTE   unknow_1[11];
	BYTE   IsBuilder;
	BYTE   unknow_2[12];
	DWORD   Weapon2_p;
	BYTE   unknow_3[24];
	DWORD   Weapon3_p;
	BYTE   unknow_4[16];
	DWORD   UnitOrders_p;
	BYTE   UnitState;
	BYTE   unknow_5[3];
	WORD   ZTurn;
	WORD   XTurn;
	WORD   YTurn;
	WORD   unknow3;
	WORD   XPos;
	WORD   unknow4;
	WORD   ZPos;
	WORD   YPos__;
	WORD   YPos;
	WORD   XGridPos;
	WORD   YGridPos;
	WORD   XLargeGridPos;
	WORD   YLargeGridPos;
	BYTE   unknow_6[4];
	DWORD   unknow_p_7;
	BYTE   unknow_8[8];
	DWORD   FirstUnit_p;
	DWORD   UnitType_p;
	DWORD   Owner_p;
	DWORD   UnitScriptingData_p;
	DWORD   Object3do_P;
	DWORD   UNKnow_10;
	WORD   UnitTypeID_Word_;
	DWORD   UnitInGameID;
	BYTE   field_AC[12];
	WORD   Kills;
	BYTE   UNKnow_11[50];
	DWORD   Owner2_p;
	DWORD   Attacker_p;
	BYTE   unknow_12[2];
	BYTE   HealthPerA;
	BYTE   HealthPerB;
	BYTE   unKnow_13[2];
	BYTE   RecentDamage;
	BYTE   Height;
	WORD   OwnerIndex;
	BYTE   Los_Sight_PlayerID_Unit;
	BYTE   unknow_14[4];
	DWORD   Nanoframe;
	WORD   Health;
	BYTE   unKnow_15[4];
	WORD   cIsCloaked;
	DWORD   UnitSelectedStateMask;
	BYTE   unknow_16[4];
}UnitsInGameStruct,*pUnitsInGameStruct;

#pragma pack()

#endif
