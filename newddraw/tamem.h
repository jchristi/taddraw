#ifndef tamemH
#define tamemH

#include <dsound.h>


        

#pragma pack(1)


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
struct _GUIInfo;
struct _GUIMEMSTRUCT;
struct _GUI1IDControl;
struct _GUI0IDControl;
struct _GUI2IDControl;
struct _GUI3_4IDControl;
struct _GUI5IDControl;
struct _GUI78IDControl;
struct _GUI6IDControl;
struct _GUI9IDControl;
struct _TAProgramStruct ;
struct _GAFSequence;
struct RadarUnit_ ;
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
	float fPlayerMetalStorage ;
};
struct PlayerStruct
{
	int PlayerActive;
	int DirectPlayID;
	int field_8;
	char PlayerNum;
	char field_D[3];
	int field_10;
	char field_14[14];
	char field_22;
	char field_23[4];
	PlayerInfoStruct * PlayerInfo;
	char Name[30];
	char SecondName[30];
	UnitStruct * Units;
	UnitStruct * UnitsAry_End;
	__int16 UnitsIndex_Begin;
	__int16 UnitsIndex_End;
	char My_PlayerType;
	int AiConfig;
	int field_78;
	unsigned char * LOS_MEMORY_p;
	int LOS_Tilewidth;
	int LOS_Tileheight;
	int LOS_bitsNum;
	PlayerResourcesStruct PlayerRes;
	float ShareMetal;
	float ShareEnergy;
	int field_EC;
	int UpdateTime;
	int WinLoseTime;
	int DisplayTimer;
	__int16 Kills;
	__int16 Losses;
	int field_100;
	__int16 kills_2;
	__int16 losse_2;
	char AllyFlagAry[10];
	char field_112;
	char field_113;
	char field_114[21];
	char field_129;
	char field_12A[21];
	char AllyTeam;
	int WholeUnitsCounters;
	__int16 UnitsNumber;
	char PlayerAryIndex;
	char field_147;
	char field_148;
	__int16 AddPlayerStorage_word;
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


typedef struct _GUIInfo
{
	int field_0;
	int commongui_GAF;
	char field_8[16];
	_GUIMEMSTRUCT * TheActive_GUIMEM;
	int Cursor_0;
	int Cursor_1;
	int Cursor_2;
	int field_28;
	int Cursor_3;
	__int16 field_30;
	__int16 field_32;
	int field_34;
	int field_38;
	char field_3C[24];
	int field_54;
	int field_58;
	int field_5C;
	int UIChange_f;
	int field_64;
	int field_68;
	int field_6C;
	int field_70;
	int field_74;
	int field_78;
	char field_7C[60];
	char field_B8[3090];
	int GUIUpdated_b;
}GUIInfo;

//settimer 4fb368

struct TAdynmemStruct{
	char data1[12];
	_TAProgramStruct * TAProgramStruct_Ptr;
	DSoundStruct *DSound;
	char data2[0x505];
	_GUIInfo desktopGUI;
	char data3[0x97C];
	PlayerStruct Players[10];	//0x1B63 , end at 0x2851

	char data4[331];
	unsigned int data5;
	unsigned int AllyStruct_Ptr;
	char data6[144];
	unsigned int PacketBufferSize;
	unsigned int PacketBuffer_p;
	unsigned short PlayerCounters;
	unsigned int ChatTextIndex;
	char LocalHumanPlayer_PlayerID;
	char LOS_Sight_PlayerID;
	char NetworkLayerEnabled;// 02A44 
	char data7[0x231];  
	POINT CurtMousePostion;//0x2C76
	char data7_[0x10];  //
	short BuildPosX; //0x2C8E
	short BuildPosY;
	int BuildPosRealX; //0x2C92
	int Height;
	int BuildPosRealY;
	int unk1;
	int Height2;

	char data8[0x6];

	short MouseMapPosX;  //0x2CAC
	char data9[6];
	short MouseMapPosY;  //0x2CB4
	char data10[4];
	unsigned short MouseOverUnit; //0x2CBA
	char data11[0x8];
	short BuildNum;  //0x2CC4,  unitindex for selected OwnUnitBegin to build
	char BuildSpotState; //0x40=notoktobuild
	char data12[0x2C];

	WeaponStruct Weapons[256];  //0x2CF3  size=0x11500
	//char data7[4];
	int NumProjectiles;
	ProjectileStruct *Projectiles; //0x141F7
	char data13[0x10];
	WreckageInfoStruct *WreckageInfo; //0x1420B
	char data14[0x1c];
	int MapSizeX;
	int MapSizeY;
	int FeatureMapSizeX; //0x14233
	int FeatureMapSizeY; //0x14237
	char data15[0x18];
	int NumFeatureDefs;
	char data16[0x18];
	FeatureDefStruct *FeatureDef; //0x1426F
	unsigned short * MAPPED_MEMORY_p;
	DWORD LastZPos;
	LPVOID	*EyeBallMemory;  //0x1427B
	char data18[2];
	unsigned short LosType;// 0x14281
	char data19[4];
	FeatureStruct *Features; //0x14287
	char data20[0x40];
	tagRECT MinimapRect;//0x142CB
	RadarPicStruct *RadarFinal; //0x142DB
	RadarPicStruct *RadarMapped;  //0x142DF
	RadarPicStruct *RadarPicture;  //0x142E3
	char data21[4];
	short RadarPicSizeX;  //0x142EB
	short RadarPicSizeY;  //0x142ED
	char data22[4];
	int CirclePointer;//0x142F3 //used in drawcircle funktion
	char data23[0x28];
	int MapX;	//0x1431f
	int MapY;   //0x14323
	int MapXScrollingTo; //0x14327
	int MapYScrollingTo; //0x1432B
	char data24[0x24];
	DWORD UnitsCounter;
	UnitStruct *OwnUnitBegin;//UnitStruct *
	UnitStruct *OwnUnitEnd; //0x1435B UnitStruct *
	short int *HotUnits;//0x1435F
	RadarUnit_ * RadarUnits;
	int NumHotUnits; //0x14367
	int NumHotRadarUnits;
	char data25[0x2c];
	UnitDefStruct *UnitDef;  //0x1439b 
	char data26[0x440];
	_GAFSequence * radlogo; //0147DF
	char data26_[0xE8];
	_GAFSequence * CurcosNormal;//0x148CB
	char data27[0x4c];
	int NumExplosions; //0x1491B
	//char data9[0x6270];
	ExplosionStruct Explosions[300]; //0x1491F
	LPVOID data28; //0x1AB8F
	char data29[0x1D294]; //0x1AB93
	RECT GameSreen_Rect; //0x37E27 
	char data30[0xC3];
	int InterfaceType;
	char data31[0x31];
	unsigned short SoftwareDebugMode;// 0x37f2f
	char data32[0xB16];
	short int GameTime; //0x38A47
	short int GameSpeed;
	char ShotUrl;// 0x38a47+c= TA截图目录的字符串，即TA目录+当前用户名 
	char data33[0x792];
	MapFileStruct *MapFile; //0x391E9
	int data34;
	char data_35[11];
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
  PlayerStruct  * Owner_PlayerPtr0; //?
  LPVOID UnkPTR2;
  Object3doStruct *Object3do;
  int Order_Unknow ;
  short int UnitID;
  char data9[16];

  short Kills;
  char data17[50];
  PlayerStruct * Owner_PlayerPtr1; //?
  char data16[6];
  char HealthPerA;  //health in percent
  char HealthPerB;  //health in percent, changes slower (?)
  char data19[2];
  unsigned char RecentDamage;  //0xFA
  unsigned char Height;
  short int  OwnerIndex ;
  char data28;
  char myLos_PlayerID;
  char data10[4];
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


/*  105 */


/*  126 */

typedef struct _GUIMEMSTRUCT
{
	int per_active;
	_GUI0IDControl * topControl;
	int OnCommand;
	int TAMainStructPtr;
	int field_10;
	int field_14;
	int ActiveUp;
	int Update_proc;
	int field_20;
	int field_24;
	char field_28[19];
	int field_3B;
	char field_3F[119];
	__int16 SubGUICount;
	int field_B8;
	int field_BC;
	int field_C0;
	char field_C4[153];
	char field_15D[2137];
	char HPIPath[256];
	char field_AB6[66660];
	char field_10F1A[61];
}GUIMEMSTRUCT;


/*  134 */

typedef struct _GUI1IDControl
{
	char id;
	char assoc;
	char name[16];
	char gap_12[1];
	__int16 xpos;
	__int16 ypos;
	__int16 width;
	__int16 height;
	int attribs;
	int colorf;
	int colorb;
	char texturenumber;
	char fontnumber;
	char active;
	char commonattribs;
	char gap_2B[8];
	char help[128];
	char gap_B3[1];
	__int16 gaffile;
	char text[128];
	char stages;
	char status_curnt;
	__int16 status_init;
	__int16 quickkey;
	int grayedout;
	__int16 field_140;
	__int16 field_142;
	int unk_proc;
	__int16 field_148;
	int TAMainPtr;
	char field_14E[13];
}GUI1IDControl;


/*  151 */

typedef struct _GUI0IDControl
{
	char id;
	char assoc;
	char name[16];
	char gap_12[1];
	__int16 xpos;
	__int16 ypos;
	__int16 width;
	__int16 height;
	int attribs;
	int colorf;
	int colorb;
	char texturenumber;
	char fontnumber;
	char active;
	char commonattribs;
	char field_2B[8];
	char help[128];
	char gap_B3[1];
	__int16 gaffile;
	__int16 totalgadgets;
	__int16 field_B8;
	__int16 field_BA;
	int OFFSCREEN_p;
	char field_C0[8];
	char field_C8;
	char major;
	char minor;
	char revision;
	char crdefault[16];
	char escdefault[16];
	char defaultfocus[16];
	char panel[16];
	char field_10C[42];
	__int16 field_136;
	__int16 status;
	__int16 field_13A;
	int field_13C;
	__int16 field_140;
	__int16 field_142;
	int unk_proc;
	__int16 field_148;
	int TAMainPtr;
	char field_14E[13];
}GUI0IDControl;


/*  152 */

typedef struct _GUI2IDControl
{
	char id;
	char assoc;
	char name[16];
	char gap_12[1];
	__int16 xpos;
	__int16 ypos;
	__int16 width;
	__int16 height;
	int attribs;
	int colorf;
	int colorb;
	char texturenumber;
	char fontnumber;
	char active;
	char commonattribs;
	char field_2B[8];
	char help[128];
	char field_B3[7];
	__int16 selected_i;
	char field_BC[22];
	int unk_3;
	int unk_0;
	int itemheight;
	int unk_1;
	char field_E2[98];
	int unk_proc;
	__int16 field_148;
	int TAMainPtr;
	char field_14E[13];
}GUI2IDControl;


/*  154 */
typedef void (__stdcall * _PosProc)(GUIInfo * , int);
typedef struct _GUI3_4IDControl
{
	char id;
	char assoc;
	char name[16];
	char gap_12[1];
	__int16 xpos;
	__int16 ypos;
	__int16 width;
	__int16 height;
	int attribs;
	int colorf;
	int colorb;
	char texturenumber;
	char fontnumber;
	char active;
	char commonattribs;
	char field_2B[8];
	char help[128];
	char field_B3[43];
	char field_DE[24];
	char text[64];
	__int16 range;
	char maxchars;
	char gap_139[3];
	int thick;
	__int16 knobpos;
	__int16 knobsize;
	_PosProc pos_proc;
	__int16 field_148;
	int TAMainPtr;
	char field_14E[13];
}GUI3_4IDControl;


/*  155 */

typedef struct _GUI5IDControl
{
	char id;
	char assoc;
	char name[16];
	char gap_12[1];
	__int16 xpos;
	__int16 ypos;
	__int16 width;
	__int16 height;
	int attribs;
	int colorf;
	int colorb;
	char texturenumber;
	char fontnumber;
	char active;
	char commonattribs;
	char field_2B[8];
	char help[128];
	char field_B3[67];
	char text[64];
	char link[16];
	char gap_146[1];
	char unk_0;
	char gap_148[2];
	int TAMainPtr;
	char field_14E[13];
}GUI5IDControl;


/*  156 */

typedef struct _GUI78IDControl
{
	char id;
	char assoc;
	char name[16];
	char gap_12[1];
	__int16 xpos;
	__int16 ypos;
	__int16 width;
	__int16 height;
	int attribs;
	int colorf;
	int colorb;
	char texturenumber;
	char fontnumber;
	char active;
	char commonattribs;
	char field_2B[8];
	char help[128];
	char field_B3[67];
	char filename[32];
	char field_116[52];
	int TAMainPtr;
	char field_14E[13];
}GUI78IDControl;


/*  157 */

typedef struct _GUI6IDControl
{
	char id;
	char assoc;
	char name[16];
	char gap_12[1];
	__int16 xpos;
	__int16 ypos;
	__int16 width;
	__int16 height;
	int attribs;
	int colorf;
	int colorb;
	char texturenumber;
	char fontnumber;
	char active;
	char commonattribs;
	char field_2B[8];
	char help[128];
	char field_B3[49];
	int hotornot;
	char field_E8[98];
	int TAMainPtr;
	char field_14E[13];
}GUI6IDControl;


/*  158 */

typedef struct _GUI9IDControl
{
	char id;
	char assoc;
	char name[16];
	char gap_12[1];
	__int16 xpos;
	__int16 ypos;
	__int16 width;
	__int16 height;
	int attribs;
	int colorf;
	int colorb;
	char texturenumber;
	char fontnumber;
	char active;
	char commonattribs;
	char field_2B[8];
	char help[128];
	char field_B3[67];
	int nuttin;
	char field_FA[80];
	int TAMainPtr;
	char field_14E[13];
}GUI9IDControl;


typedef struct RadarUnit_ 
{
	short int ID ;
	unsigned int x;
	unsigned int y;
}RadarUnit; 

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
	LPBYTE ALPHA_TABLE;
	LPBYTE SHADE_TABLE;
	LPBYTE LIGHT_TABLE;
	LPBYTE GRAY_TABLE;
	LPBYTE BLUE_TABLE;
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

enum LOSTYPE
{
	 NOMAPPING        = 1,
	 Permanent        = 2,
	 LOSTYPE          = 4,
	 Updating         = 8

};

enum WeaponMask
{
	 stockpile_mask   = 0x1000000,
	 targetable_mask  = 0x20000000,
	 interceptor_mask  = 0x40000000
};

enum SharedStates
{
	 SharedMetal      = 2,
	 SharedEnergy     = 4,
	 SharedLOS        = 8,
	 SharedMappings   = 0x20,
	 SharedRadar      = 0x40
};

namespace softwaredebugmode
{
	enum SOFTWAREDEBUGMODE
	{
		Drop             = 1,
		Now              = 2,
		Noshake          = 0x10,
		Clock            = 0x40,
		Doubleshot       = 0x80,
		Halfshot         = 0x100,
		Radar            = 0x200,
		Shootall         = 0x400
	};
};

#define PLAYERNUM (10)

#pragma pack()


#endif
