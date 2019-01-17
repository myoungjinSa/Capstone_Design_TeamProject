#include "stdafx.h"
#include "AnimationController.h"

CAnimationController::CAnimationController(int nAnimationTracks)
{
	m_nAnimationTracks = nAnimationTracks;
	m_pAnimationTracks = new CAnimationTrack[nAnimationTracks];
}

CAnimationController::~CAnimationController()
{
	if (m_pAnimationSets) delete[] m_pAnimationSets;
	if (m_ppAnimationBoneFrameCaches) delete[] m_ppAnimationBoneFrameCaches;
	if (m_pAnimationTracks) delete[] m_pAnimationTracks;
}

void CAnimationController::SetCallbackKeys(int nAnimationSet, int nCallbackKeys)
{
	m_pAnimationSets[nAnimationSet].m_nCallbackKeys = nCallbackKeys;
	m_pAnimationSets[nAnimationSet].m_pCallbackKeys = new CALLBACKKEY[nCallbackKeys];
}

void CAnimationController::SetCallbackKey(int nAnimationSet, int nKeyIndex, float fKeyTime, void *pData)
{
	m_pAnimationSets[nAnimationSet].m_pCallbackKeys[nKeyIndex].m_fTime = fKeyTime;
	m_pAnimationSets[nAnimationSet].m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
}

void CAnimationController::SetAnimationSet(int nAnimationSet)
{
	if (m_pAnimationSets && (nAnimationSet < m_nAnimationSets))
	{
		m_nAnimationSet = nAnimationSet;
		m_pAnimationTracks[m_nAnimationTrack].m_pAnimationSet = &m_pAnimationSets[m_nAnimationSet];
	}
}

void CAnimationController::AdvanceTime(float fTimeElapsed, CAnimationCallbackHandler *pCallbackHandler)
{
	m_fTime += fTimeElapsed;
	if (m_pAnimationSets)
	{
		for (int i = 0; i < m_nAnimationTracks; i++)
		{
			if (m_pAnimationTracks[i].m_bEnable)
			{
				m_pAnimationTracks[i].m_fPosition += (fTimeElapsed * m_pAnimationTracks[i].m_fSpeed);

				CAnimationSet *pAnimationSet = m_pAnimationTracks[i].m_pAnimationSet;
				pAnimationSet->m_fPosition += (fTimeElapsed * pAnimationSet->m_fSpeed);

				if (pCallbackHandler)
				{
					void *pCallbackData = pAnimationSet->GetCallback(pAnimationSet->m_fPosition);
					if (pCallbackData) pCallbackHandler->HandleCallback(pCallbackData);
				}

				float fPositon = pAnimationSet->GetPosition(pAnimationSet->m_fPosition);
				for (int i = 0; i < m_nAnimationBoneFrames; i++)
				{
					m_ppAnimationBoneFrameCaches[i]->m_xmf4x4ToParent = pAnimationSet->GetSRT(i, fPositon);
				}
			}
		}
	}
}
