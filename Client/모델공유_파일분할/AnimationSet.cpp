#include "stdafx.h"
#include "AnimationSet.h"

CAnimationSet::CAnimationSet()
{
}

CAnimationSet::~CAnimationSet()
{
	if (m_pfKeyFrameTransformTimes) delete[] m_pfKeyFrameTransformTimes;
	for (int j = 0; j < m_nKeyFrameTransforms; j++) if (m_ppxmf4x4KeyFrameTransforms[j]) delete[] m_ppxmf4x4KeyFrameTransforms[j];
	if (m_ppxmf4x4KeyFrameTransforms) delete[] m_ppxmf4x4KeyFrameTransforms;

	if (m_pCallbackKeys) delete[] m_pCallbackKeys;
}

// 정확한 시간에 따라서 애니메이션을 하게 해야된다.
// 왜냐하면, 1.5초의 애니메이션도 1.6초에 애니메이션이 될 수 도 있어서 시간이 안맞을 수 있기 때문에
float CAnimationSet::GetPosition(float fPosition)
{
	float fGetPosition = fPosition;
	// 애니메이션 타입에 따라 구분함
	switch (m_nType)
	{
	case ANIMATION_TYPE_LOOP:
	{
		fGetPosition = fPosition - int(fPosition / m_pfKeyFrameTransformTimes[m_nKeyFrameTransforms - 1]) * m_pfKeyFrameTransformTimes[m_nKeyFrameTransforms - 1];
		//			fGetPosition = fPosition - int(fPosition / m_fLength) * m_fLength;
#ifdef _WITH_ANIMATION_INTERPOLATION			
#else
		m_nCurrentKey++;
		// 보간을 하지 않고, 인덱스를 읽어서 애니메이션 하는 방법
		// 대신에 프레임레이트에 따라서 프레임레이트가 높으면 빨리 애니메이션되고, 프레임레이트가 낮으면 애니메이션이 느리게 되는 단점이 생김
		// 따라서 프레임레이트를 고정시켜서 사용해야한다.
		if (m_nCurrentKey >= m_nKeyFrameTransforms) m_nCurrentKey = 0;
#endif
		break;
	}
	case ANIMATION_TYPE_ONCE:
		break;
	case ANIMATION_TYPE_PINGPONG:
		break;
	}
	return(fGetPosition);
}

#define _WITH_ITERATION

XMFLOAT4X4 CAnimationSet::GetSRT(int nFrame, float fPosition)
{
	XMFLOAT4X4 xmf4x4Transform = Matrix4x4::Identity();
#ifdef _WITH_ANIMATION_INTERPOLATION
#ifdef _WITH_ANIMATION_SRT
	XMVECTOR S, R, T;
	for (int i = 0; i < (m_nKeyFrameTranslations - 1); i++)
	{
		if ((m_pfKeyFrameTranslationTimes[i] <= fPosition) && (fPosition <= m_pfKeyFrameTranslationTimes[i + 1]))
		{
			float t = (fPosition - m_pfKeyFrameTranslationTimes[i]) / (m_pfKeyFrameTranslationTimes[i + 1] - m_pfKeyFrameTranslationTimes[i]);
			T = XMVectorLerp(XMLoadFloat3(&m_ppxmf3KeyFrameTranslations[i][nFrame]), XMLoadFloat3(&m_ppxmf3KeyFrameTranslations[i + 1][nFrame]), t);
			break;
		}
	}
	for (UINT i = 0; i < (m_nKeyFrameScales - 1); i++)
	{
		if ((m_pfKeyFrameScaleTimes[i] <= fPosition) && (fPosition <= m_pfKeyFrameScaleTimes[i + 1]))
		{
			float t = (fPosition - m_pfKeyFrameScaleTimes[i]) / (m_pfKeyFrameScaleTimes[i + 1] - m_pfKeyFrameScaleTimes[i]);
			S = XMVectorLerp(XMLoadFloat3(&m_ppxmf3KeyFrameScales[i][nFrame]), XMLoadFloat3(&m_ppxmf3KeyFrameScales[i + 1][nFrame]), t);
			break;
		}
	}
	for (UINT i = 0; i < (m_nKeyFrameRotations - 1); i++)
	{
		if ((m_pfKeyFrameRotationTimes[i] <= fPosition) && (fPosition <= m_pfKeyFrameRotationTimes[i + 1]))
		{
			float t = (fPosition - m_pfKeyFrameRotationTimes[i]) / (m_pfKeyFrameRotationTimes[i + 1] - m_pfKeyFrameRotationTimes[i]);
			R = XMQuaternionSlerp(XMLoadFloat4(&m_ppxmf4KeyFrameRotations[i][nFrame]), XMLoadFloat4(&m_ppxmf4KeyFrameRotations[i + 1][nFrame]), t);
			break;
		}
	}

	XMStoreFloat4x4(&xmf4x4Transform, XMMatrixAffineTransformation(S, NULL, R, T));
#else   
	for (int i = 0; i < (m_nKeyFrameTransforms - 1); i++)
	{
		if ((m_pfKeyFrameTransformTimes[i] <= fPosition) && (fPosition <= m_pfKeyFrameTransformTimes[i + 1]))
		{
			// i + 1에서 i를 뺀 그 차이만큼 포지션을 나눈다.
			float t = (fPosition - m_pfKeyFrameTransformTimes[i]) / (m_pfKeyFrameTransformTimes[i + 1] - m_pfKeyFrameTransformTimes[i]);
			XMVECTOR S0, R0, T0, S1, R1, T1;
			XMMatrixDecompose(&S0, &R0, &T0, XMLoadFloat4x4(&m_ppxmf4x4KeyFrameTransforms[i][nFrame]));
			XMMatrixDecompose(&S1, &R1, &T1, XMLoadFloat4x4(&m_ppxmf4x4KeyFrameTransforms[i + 1][nFrame]));
			// XMVectorLerp : 벡터 선형 보간 함수
			XMVECTOR S = XMVectorLerp(S0, S1, t);
			XMVECTOR T = XMVectorLerp(T0, T1, t);
			// XMQuaternionSlerp : 쿼터니언 보간
			XMVECTOR R = XMQuaternionSlerp(R0, R1, t);
			XMStoreFloat4x4(&xmf4x4Transform, XMMatrixAffineTransformation(S, XMVectorZero(), R, T));
			break;
		}
	}
#endif
#else
	xmf4x4Transform = m_ppxmf4x4KeyFrameTransforms[m_nCurrentKey][nFrame];
#endif
	return(xmf4x4Transform);
}

void CAnimationSet::SetCallbackKeys(int nCallbackKeys)
{
	m_nCallbackKeys = nCallbackKeys;
	m_pCallbackKeys = new CALLBACKKEY[nCallbackKeys];
}

void CAnimationSet::SetCallbackKey(int nKeyIndex, float fKeyTime, void *pData)
{
	m_pCallbackKeys[nKeyIndex].m_fTime = fKeyTime;
	m_pCallbackKeys[nKeyIndex].m_pCallbackData = pData;
}