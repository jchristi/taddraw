#pragma once

#define SFXVECTORLIMITNUM (18)
class IncreaseSfxLimit
{
public:
	DWORD CurtSfxVectorNum;
	DWORD CurtMaxLimit;
private:
	SingleHook * SHookSfxVectorAry[SFXVECTORLIMITNUM];
	InlineSingleHook * Sfx_mallocBufSize;
public:
	IncreaseSfxLimit ();
	IncreaseSfxLimit (DWORD SfxVectorNum);
	~IncreaseSfxLimit ();
	//IncreaseSfxLimit (DWORD SfxVectorNum, DWORD MaxLimit);
private:
	void SetNewLimit ( DWORD SfxVectorNum, DWORD MaxLimit);
};
extern IncreaseSfxLimit * NowIncreaseSfxLimit;