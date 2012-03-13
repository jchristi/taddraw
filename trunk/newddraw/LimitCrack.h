#pragma  once
class IncreaseAISearchMapEntriesLimit;
class IncreaseWeaponTypeLimit;
class IncreaseUnitTypeLimit;
class IncreaseCompositeBuf;
class IncreaseMixingBuffers;
class Switchalt;
class InlineSingleHook;
class IncreaseSfxLimit;
class ModifyWeaponPacket;

class LimitCrack
{
public:
	IncreaseAISearchMapEntriesLimit * NowIncreaseAISearchMapEntriesLimit;
	IncreaseWeaponTypeLimit * NowIncreaseWeaponTypeLimit;
	IncreaseUnitTypeLimit * NowIncreaseUnitTypeLimit;
	IncreaseCompositeBuf * NowIncreaseCompositeBuf;
	//IncreaseMixingBuffers * NowIncreaseMixingBuffers;
	//Switchalt * NowSwitchalt;
	InlineSingleHook* HookInCircleSelect;
	IncreaseSfxLimit * NowIncreaseSfxLimit;
	ModifyWeaponPacket * NowModifyWeaponPacket;

public:
	LimitCrack (void);
	~LimitCrack (void);
};

extern LimitCrack* NowCrackLimit;