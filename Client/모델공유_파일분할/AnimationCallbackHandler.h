#pragma once

class CAnimationCallbackHandler
{
public:
	CAnimationCallbackHandler();
	~CAnimationCallbackHandler();
	virtual void HandleCallback(void *pCallbackData);
};
