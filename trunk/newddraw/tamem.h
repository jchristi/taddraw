#ifndef tamemH
#define tamemH

#include <dsound.h>


        

#pragma pack(1)

struct _GAFSequence;

enum PlayerType;
enum PlayerPropertyMask;
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
struct _OFFSCREEN;
struct PlayerResourcesStruct;
struct _RaceSideData;
struct _Vertices;
struct _Shatte;
struct _TAProgramStruct;

struct Point3{
	int x;
	int y;
	int z;
};


struct PlayerResourcesStruct
{
	float fCurrentEnergy;//here,  need to make sure, that float is 32 bit length
	float fEnergyProducton;
	float fEnergyExpense;
	float fCurrentMetal;
	float fMetalProduction ;
	float fMetalExpense  ;
	float fMaxEnergyStorage ;
	float fMaxMetalStorage ;
	double fTotalEnergyProduced ;
	double fTotalMetalProduced;
	double fTotalEnergyConsumed;
	double fTotalMetalConsumed ;
	double fEnergyWasted;
	double fMetalWasted;
	float fPlayerEnergyStorage ;
	float PlayerMetalStorage ;
};

struct PlayerStruct{
  unsigned int PlayerActive;
  char data1[8];
  char PlayerNum;///?????
  char data2[26];
  PlayerInfoStruct *PlayerInfo;
  char Name[30];
  char SecondName[30];
  UnitStruct *Units; //0x67 //Units_Begin
  UnitStruct *UnitsAry_End; //Units_End
  unsigned int data4;
  unsigned char My_PlayerType;
  unsigned int data5;
  unsigned int data6;
  unsigned int data7;
  unsigned int data8;
  unsigned int data9;
  unsigned int data10;
  PlayerResourcesStruct PlayerRes;
  unsigned char data11[12];
  unsigned int UpdateTime;
  unsigned int WinLoseTime;
  unsigned int DisplayTimer;
  unsigned short Kills_word;
  unsigned short Losses;
  unsigned char data12[8];
  unsigned char data13[10];
  unsigned char data14[46];

  unsigned int UnitsCounters;
  unsigned short UnitsNumber;
  unsigned char PlayerAryIndex;
  unsigned short  data15;
  unsigned short AddPlayerStorage_word;
 // char data5[208];
}; //0x14b

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
  char data7[4];
  unsigned int WeaponTypeMask;
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

struct TAdynmemStruct{
	char data21[12];
	_TAProgramStruct * TAProgramStruct_Ptr;
	DSoundStruct *DSound;
	char data1[0x1B4F];

	PlayerStruct Players[10];	//0x1B63 , end at 0x2851

	char data4[331];
	unsigned int data4_;
	unsigned int AllyStruct_Ptr;
	char data4_1[144];
	unsigned int PacketBufferSize;
	unsigned int PacketBuffer_p;
	unsigned short PlayerCounters;
	unsigned int ChatTextIndex;
	char LocalHumanPlayer_PlayerID;
	char LOS_Sight_PlayerID;
	char NetworkLayerEnabled;
	char data4_2[585];
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
	char data12[2];
	unsigned short EyeBallState;// 0x14281
	char data12_[4];
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
	char data11[0x52c];
	_GAFSequence * CurcosNormal;//0x148CB
	char data13_[0x4c];
	int NumExplosions; //0x1491B
	//char data9[0x6270];
	ExplosionStruct Explosions[300]; //0x1491F
	LPVOID Unk2; //0x1AB8F
	char data10[0x1D39C]; //0x38A47 0x037F2F  B18
	unsigned short SoftwareDebugMode;// 0x37f2f
	char data10_[0xB16];
	int GameTime; //0x38A47
	char ShotUrl;// 0x38a47+c= TA截图目录的字符串，即TA目录+当前用户名 
	char data6[0x792];
	MapFileStruct *MapFile; //0x391E9
	int data19_;
	char data_20[11];
	int State_GUI_CallBack;//0x0391F1
	LPVOID GUI_CallBack;
};

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

struct PlayerInfoStruct 
{
	char MapName[0x20];
	char data1[0x75];
	char RaceSide;
	char PlayerLogoColor;
	char SharedBits;//  enum SharedStates
	char data2[3];
	char PropertyMask;
};

struct UnitDefStruct {
	char Name[0x20];
	char UnitName[0x20];
	char UnitDescription[0x40];
	char ObjectName[0x20];
	char Side[3];
	char data5[0xA7];
	short FootX;  //0x14A
	short FootY;  //0x14C
	char *YardMap;
	int canbuildCount;
	LPVOID CANBUILD_ptr;
	char data6[0x4];
	unsigned short __X_Width;
	unsigned short X_Width;
	unsigned long data_7;
	unsigned short Y_Width;
	unsigned short __Y_Width;
	unsigned long data_8;
	unsigned short __Z_Width;
	unsigned short Z_Width;
	unsigned long data_9;
	unsigned long data_10;
	unsigned long data_11;
	unsigned long data_12;
	unsigned long data_13;
	unsigned long buildcostenergy;
	unsigned long buildcostmetal;
	unsigned long data_14;
	unsigned long lRawSpeed_maxvelocity;
	unsigned long data_15;
	unsigned long data_16;
	unsigned long cceleration;
	unsigned long  bankscale;
	unsigned long  pitchscale;
	unsigned long  damagemodifier;
	unsigned long  moverate1;
	unsigned long  moverate2;
	unsigned long  movementclass;
	unsigned short  turnrate;
	unsigned short  corpse;
	unsigned short  maxwaterdepth;
	unsigned short   minwaterdepth;
	unsigned long   energymake;
	unsigned long   energyuse ;
	unsigned long   metalmake;
	unsigned long   extractsmetal;
	unsigned long   windgenerator;
	unsigned long   tidalgenerator ;
	unsigned long   cloakcost ;
	unsigned long   cloakcostmoving;
	unsigned long   energystorage ;
	unsigned long   metalstorage ;
	unsigned long   buildtime;
	WeaponStruct *  weapon1  ;
	WeaponStruct *  weapon2 ;
	WeaponStruct *  weapon3;
	unsigned short   nMaxHP;
	unsigned short   data8  ;
	unsigned short nWorkerTime;
	unsigned short nHealTime ;
	unsigned short nSightDistance;
	unsigned short nRadarDistance ;
	unsigned short nSonarDistance;
	unsigned short mincloakdistance ;
	unsigned short radardistancejam;
	unsigned short sonardistancejam ;
	unsigned short SoundClassIndex;
	unsigned short nBuildDistance;
	unsigned short builddistance;
	unsigned short nManeuverLeashLength;
	unsigned short attackrunlength ;
	unsigned short kamikazedistance ;
	unsigned short sortbias     ;
	unsigned char cruisealt  ;
	unsigned char data4  ;
	unsigned short UnitCategoryMask  ;

	WeaponStruct *ExplodeAs;
	WeaponStruct *SelfeDestructAs;
	unsigned char  maxslope;
	unsigned char  badslope  ;
	unsigned char  transportsize;
	unsigned char  transportcapacity;
	unsigned char  waterline ;
	unsigned short  makesmetal;
	unsigned char  bmcode  ;
	unsigned char  defaultmissiontypeIndex;
	unsigned long *  wpri_badTargetCategory_MaskAryPtr ;
	unsigned long *  wsec_badTargetCategory_MaskAryPtr;
	unsigned long *  wspe_badTargetCategory_MaskAryPtr;
	unsigned long *  noChaseCategory_MaskAryPtr;
	unsigned long  UnitTypeMask_0;
	unsigned long  UnitTypeMask_1 ;

	/*
	char data7[0x36];
	unsigned short WorkerTime;
	char data8[0x49];*/
}; //0x249

enum UnitSelectState
{
	UnitValid_State        = 0x4,
	UnitSelected_State     = 0x10,
	UnitValid2_State       = 0x20,
	UnitInSight_State		 = 0x40
};

struct UnitStruct {
  int IsUnit;
  char data1[12];
  WeaponStruct *Weapon1;
  char data2[11];
  char Builder;
  char data3[12];
  WeaponStruct *Weapon2;
  char data4[24];
  WeaponStruct *Weapon3;
  char data5[16];
  UnitOrdersStruct *UnitOrders;  //5c
  char UnitState;
  char data6[3];
  unsigned short ZTurn;
  unsigned short XTurn;
  unsigned short YTurn;
  unsigned short XPos__ ;                    ; //0x6A
  unsigned short XPos;
  unsigned short ZPos__  ;
  unsigned short ZPos  ;
  unsigned short YPos__ ;
  unsigned short YPos ;
  short XGridPos;
  short YGridPos;
  short XLargeGridPos;
  short YLargeGridPos;
  char data8[4];
  LPVOID UnkPTR1;
  char data15[4];
  int Unknow_Order;//
  UnitStruct *FirstUnit; //?
  UnitDefStruct *UnitType; //0x92
  PlayerStruct *SpotFor_PlayerPt; //?
  LPVOID UnkPTR2;
  Object3doStruct *Object3do;
  int data_4;
  short int UnitCategoryMask;
  char data9[16];

  short Kills;
  char data17[50];
  PlayerStruct *ValidOwner_PlayerPtr; //?
  char data16[6];
  char HealthPerA;  //health in percent
  char HealthPerB;  //health in percent, changes slower (?)
  char data19[2];
  unsigned char RecentDamage;  //0xFA
  unsigned char Height;
  char data10[8];
  float Nanoframe;
  short Health;
  char data14[6];
  unsigned int UnitSelected;//and UnitSelectState
  char data11[4];
}; //0x118

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
  UnitOrdersStruct* PriorOrder_uosp;
  unsigned char COBHandler_index;
  unsigned char State;
  unsigned short unknow_1;
  unsigned short field_8;
  unsigned int unknow_0;
  UnitStruct * Unit_ptr;
  unsigned int unknow_3;
  UnitStruct* UnitTargat_p;
  unsigned int field_1A;
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

typedef struct _OPENTAFILE 
{
	unsigned CFILE;
	_OPENTAFILE* Parent_ptr;
	DWORD *CHUNKSizes_Ptr;
	int Null;
	LPDWORD Chunk_Sizes;
	int field_14;
	char FilePath[256];
} OPENTAFILE, * POPENTAFILE;

typedef struct _RaceSideData
{
	char name[30];
	char nameprefix[4];
	char commanderUnitName[32];
	RECT Logo_rect;
	RECT Energy_rect;
	RECT EnergyNum_rect;
	RECT METALBAR_rect;
	RECT METALNUM_rect;
	RECT TOTALUNITS_rect;
	RECT TOTALTIME_rect;
	RECT ENERGYMAX_rect;
	RECT METALMAX_rect;
	RECT ENERGY0_rect;
	RECT METAL0_rect;
	RECT ENERGYPRODUCED;
	RECT ENERGYCONSUMED;
	RECT METALPRODUCED;
	RECT METALCONSUMED;
	RECT LOGO2;
	RECT UNITNAME;
	RECT DAMAGEBAR;
	RECT UNITENERGYMAKE;
	RECT UNITENERGYUSE;
	RECT UNITMETALMAKE;
	RECT UNITMETALUSE;
	RECT MISSIONTEXT;
	RECT UNITNAME2;
	RECT field_1C2;
	RECT NAME;
	RECT DESCRIPTION;
	RECT RELOAD_RaceID;
	char field_202[32];
	int energycolor;
	int metalcolor;
	int field_22A;
	int Font_File;
}RaceSideData, *PRaceSideData;

typedef struct _Shatte
{
	int field_0;
	int field_4;
	int field_8;
	int field_C;
	int GafFramePtr;
	int field_14;
	int field_18;
	int Mask_B;
}Shatte, * PShatte;

typedef struct _Vertices
{
	int field_0;
	int field_4;
	int field_8;
}Vertices, * PVertices;

typedef struct _OFFSCREEN
{
	unsigned int  Width ;
	unsigned int  Height ;
	unsigned int  lPitch  ;
	LPVOID  lpSurface ;
	unsigned int  field_10 ;
	unsigned int  field_14 ;
	unsigned  short field_18 ;
	unsigned short field_1A;
	RECT ScreenRect;
	unsigned int field_2C ;
}OFFSCREEN;
typedef struct _TAProgramStruct 
{
	int HInstance;
	int CmdShow;
	int WindowsClassName;
	int WindowsName;
	int field_10;
	__int16 MenuResID;
	char field_16[2];
	WNDCLASS WndClass;
	HWND TAClass_Hwnd;
	int Screen_DIBSECTION;
	int WndMemHDC;
	int Palette_H;
	int DIB_OFFSCREEN;
	int BackMemHDC;
	char field_58[40];
	int field_80;
	int lpDD_DDraw;
	int lpDD_BackSurface;
	int lpDD_AttachedSurface;
	int field_90;
	int field_94;
	int field_98;
	int field_9C;
	RECT DDrawSurfaceRect;
	int field_B0;
	int lpDD_BackSurface_1;
	int field_B8;
	int CurrentOFFSCREEN;
	int ALPHA_TABLE;
	int SHADE_TABLE;
	int LIGHT_TABLE;
	int GRAY_TABLE;
	int BLUE_TABLE;
	int ScreenWidth;
	int ScreenHeight;
	int NewOFFSCREEN_Notify;
	int minimized_b;
	int field_E4;
	int srandTick;
	int OrgMOUSETRAILS;
	char IsFullScreen_mask;
	char field_F1;
	int Max_InputBuffer;
	int InputBuffer[30];
	int Total_InputBuffer;
	int ReadCount_InputBuffer;
	char field_176[20];
	int MOUSE_EVENTS;
	int field_18E;
	int field_192;
	POINT CursorPos_Buf[3];
	char field_1AE;
	char field_1AF[3];
	int Cursor;
	int CursorX;
	int CursorY;
	int SAVEMOUSE_1;
	int SAVEMOUSE_2;
	int SAVEMOUSE_3;
	int field_1CA;
	int field_1CE;
	int field_1D2;
	int MouseThreadRunning;
	int field_1DA;
	int field_1DE;
	int field_1E2;
	int field_1E6;
	int field_1EA;
	int field_1EE;
	int field_1F2;
	int field_1F6;
	int MainMenuWidth;
	int MainMenuHeight;
	__int16 IsFullScreen;
	int Font_Length;
	int field_208;
	int field_20C;
	int field_210;
	char field_214;
	char field_215[111];
	int field_284;
	char field_288[908];
	int field_614;
	int HAPIFILE_array;
	int HAPIFILECount;
	int dwTotalPhys;
	int field_624;
	char TAPath[256];
	char field_728;
}TAProgramStruct;

enum PlayerPropertyMask
{
	WATCH= 0x40,
	HUMANPLAYER= 0x80,
	PLAYERCHEATING= 0x2000
};

enum PlayerType
{
	Player_none      = 0,
	Player_LocalHuman  = 1,
	Player_LocalAI   = 2,
	Player_RemoteHuman  = 3,
	Player_RemoteAI  = 4  //
};      

enum EyeBallEnum
{
	FULLLOS = 2,
	Updating= 8
};

enum WeaponMask
{
	 stockpile_mask   = 0x10000000,
	 targetable_mask  = 0x20000000,
	 interceptor_mask  = 0x40000000
};

#pragma pack()


#endif
