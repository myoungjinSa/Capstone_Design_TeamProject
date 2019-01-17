#pragma once
#include "AnimationSet.h"

class CAnimationTrack
{
public:
	CAnimationTrack();
	~CAnimationTrack();

public:
	BOOL 						m_bEnable = true;
	float 							m_fSpeed = 1.0f;
	float 							m_fPosition = 0.0f;
	float 							m_fWeight = 1.0f;

	CAnimationSet*		m_pAnimationSet = NULL;
};