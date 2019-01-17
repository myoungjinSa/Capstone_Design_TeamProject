#pragma once
#define ANIMATION_TYPE_ONCE			0
#define ANIMATION_TYPE_LOOP			1
#define ANIMATION_TYPE_PINGPONG	2

struct CALLBACKKEY
{
	float  							m_fTime = 0.0f;
	void*							m_pCallbackData = NULL;
};

//#define _WITH_ANIMATION_SRT
#define _WITH_ANIMATION_INTERPOLATION

class CAnimationSet
{
public:
	CAnimationSet();
	~CAnimationSet();

public:
	char							m_pstrName[64];

	float							m_fLength = 0.0f;
	int								m_nFramesPerSecond = 0; //m_fTicksPerSecond

	int								m_nAnimationBoneFrames = 0;

	int								m_nKeyFrameTransforms = 0;
	float*						m_pfKeyFrameTransformTimes = NULL;
	XMFLOAT4X4**		m_ppxmf4x4KeyFrameTransforms = NULL;

#ifdef _WITH_ANIMATION_SRT
	int								m_nKeyFrameScales = 0;
	float*						m_pfKeyFrameScaleTimes = NULL;
	XMFLOAT3**			m_ppxmf3KeyFrameScales = NULL;
	int								m_nKeyFrameRotations = 0;
	float*						m_pfKeyFrameRotationTimes = NULL;
	XMFLOAT4**			m_ppxmf4KeyFrameRotations = NULL;
	int								m_nKeyFrameTranslations = 0;
	float*						m_pfKeyFrameTranslationTimes = NULL;
	XMFLOAT3**			m_ppxmf3KeyFrameTranslations = NULL;
#endif

	float 							m_fSpeed = 1.0f;
	float 							m_fPosition = 0.0f;
	int 							m_nType = ANIMATION_TYPE_LOOP; //Once, Loop, PingPong

	int								m_nCurrentKey = -1;

	int 							m_nCallbackKeys = 0;
	CALLBACKKEY*			m_pCallbackKeys = NULL;

public:
	float GetPosition(float fPosition);
	XMFLOAT4X4 GetSRT(int nFrame, float fPosition);

	void SetCallbackKeys(int nCallbackKeys);
	void SetCallbackKey(int nKeyIndex, float fTime, void *pData);

	void *GetCallback(float fPosition) { return(NULL); }
};
