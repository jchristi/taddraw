#pragma once

class IncreaseMixingBuffers
{
public:
	IncreaseMixingBuffers (DWORD num);
	~IncreaseMixingBuffers()
	{
		;
	}
};

extern IncreaseMixingBuffers * NowIncreaseMixingBuffers;