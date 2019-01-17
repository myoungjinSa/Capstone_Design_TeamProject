#pragma once
#include "AnimationSet.h"
#include "AnimationTrack.h"
#include "AnimationCallbackHandler.h"
#include "GameObject.h"

class CAnimationController
{
public:
	CAnimationController(int nAnimationTracks = 1);
	~CAnimationController();

public:
	float 							m_fTime = 0.0f;

	int								m_nAnimationSets = 0;
	CAnimationSet*		m_pAnimationSets = NULL;

	int								m_nAnimationSet = 0;

	int								m_nAnimationBoneFrames = 0;
	CGameObject**		m_ppAnimationBoneFrameCaches = NULL;

	int 							m_nAnimationTracks = 0;
	CAnimationTrack*	m_pAnimationTracks = NULL;

	int  				 			m_nAnimationTrack = 0;

	CGameObject*			m_pRootFrame = NULL;

public:
	void SetAnimationSet(int nAnimationSet);

	void SetCallbackKeys(int nAnimationSet, int nCallbackKeys);
	void SetCallbackKey(int nAnimationSet, int nKeyIndex, float fTime, void *pData);

	void AdvanceTime(float fElapsedTime, CAnimationCallbackHandler *pCallbackHandler);
};