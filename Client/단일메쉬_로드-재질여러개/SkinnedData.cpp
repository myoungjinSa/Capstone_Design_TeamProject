#pragma once
#include "stdafx.h"
#include "SkinnedData.h"




Keyframe::Keyframe()
	:TimePos(0.0f),
	Translation(0.0f, 0.0f, 0.0f),
	Scale(1.0f, 1.0f, 1.0f),
	RotationQuat(0.0f, 0.0f, 0.0f, 1.0f)
{
}

Keyframe::~Keyframe()
{}

float BoneAnimation::GetStartTime()const
{
	// Keyframes are sorted by time, so first keyframe gives start time.
	// 키프레임은 시간순으로 정렬되어야 한다. 그러므로 첫번째 키프레임이 시작 시간이다.

	return Keyframes.front().TimePos;
}

float BoneAnimation::GetEndTime() const
{
	// Keyframes are sorted by time, so last keyframe gives end time.
	// 키프레임은 시간순으로 정렬되어야 한다. 그러므로 마지막 키프레임이 끝 시간이다.

	float f = Keyframes.back().TimePos;

	return f;
}

void BoneAnimation::Interpolate(float t, XMFLOAT4X4& matrix)const
{
	if (t <= Keyframes.front().TimePos)
	{
		XMVECTOR S = XMLoadFloat3(&Keyframes.front().Scale);
		XMVECTOR P = XMLoadFloat3(&Keyframes.front().Translation);
		XMVECTOR Q = XMLoadFloat4(&Keyframes.front().RotationQuat);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		//XMMATRIX XMMatrixAffineTransformation(XMVECTOR scaling,rotationOrigin,qRotation,translation);
		XMStoreFloat4x4(&matrix, XMMatrixAffineTransformation(S, zero, Q, P));
	}
	else if (t >= Keyframes.back().TimePos)
	{
		XMVECTOR S = XMLoadFloat3(&Keyframes.back().Scale);
		XMVECTOR P = XMLoadFloat3(&Keyframes.back().Translation);
		XMVECTOR Q = XMLoadFloat4(&Keyframes.back().RotationQuat);


		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMStoreFloat4x4(&matrix, XMMatrixAffineTransformation(S, zero, Q, P));

	}
	else 
	{
		for (UINT i = 0; i < Keyframes.size() - 1; i++)
		{
			if (t >= Keyframes[i].TimePos && t <= Keyframes[i + 1].TimePos)
			{
				float lerpPercent = (t - Keyframes[i].TimePos) / (Keyframes[i + 1].TimePos - Keyframes[i].TimePos);

				XMVECTOR s0 = XMLoadFloat3(&Keyframes[i].Scale);
				XMVECTOR s1 = XMLoadFloat3(&Keyframes[i + 1].Scale);

				XMVECTOR p0 = XMLoadFloat3(&Keyframes[i].Translation);
				XMVECTOR p1 = XMLoadFloat3(&Keyframes[i + 1].Translation);

				XMVECTOR q0 = XMLoadFloat4(&Keyframes[i].RotationQuat);
				XMVECTOR q1 = XMLoadFloat4(&Keyframes[i + 1].RotationQuat);

				XMVECTOR S = XMVectorLerp(s0, s1, lerpPercent);
				XMVECTOR P = XMVectorLerp(p0, p1, lerpPercent);
				XMVECTOR Q = XMVectorLerp(q0, q1, lerpPercent);

				XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
				XMStoreFloat4x4(&matrix, XMMatrixAffineTransformation(S, zero, Q, P));


				break;
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
//Animation clip

float AnimationClip::GetClipStartTime() const
{

	//이 클립의 모든 뼈에 대해 가장 작은 시작 시간을 찾는다.
	float t = Math::Infinity;

	for(UINT i = 0; i < BoneAnimations.size(); i++)
	{
		t = Math::Min(t, BoneAnimations[i].GetStartTime());

	}

	return t;
}

float AnimationClip::GetClipEndTime() const
{
	//이 클립의 모든 뼈에 대해 가장 큰 종료 시간을 찾는다.
	float t = 0.0f;

	for (UINT i = 0.0f; i < BoneAnimations.size(); ++i)
	{
		t = Math::Max(t, BoneAnimations[i].GetEndTime());

	}
	return t;

}

void AnimationClip::Interpolate(float t, std::vector<XMFLOAT4X4>& boneTransforms) const
{
	for (UINT i = 0; i < BoneAnimations.size(); ++i)
	{
		BoneAnimations[i].Interpolate(t, boneTransforms[i]);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//SkinnedData

float SkinnedData::GetClipStartTime(const std::string& clipName) const
{
	auto clip = m_Animations.find(clipName);
	return clip->second.GetClipStartTime();
}

float SkinnedData::GetClipEndTime(const std::string& clipName)const
{
	auto clip = m_Animations.find(clipName);
	return clip->second.GetClipEndTime();
}

UINT SkinnedData::BoneCount() const
{
	return m_BoneHierarchy.size();
}

void SkinnedData::Set(std::vector<int>& boneHierarchy,
	std::vector<XMFLOAT4X4>& boneOffsets,
	std::unordered_map<std::string, AnimationClip>& animations)
{
	m_BoneHierarchy = boneHierarchy;
	m_BoneOffsets = boneOffsets;
	m_Animations = animations;
}


void SkinnedData::GetFinalTransforms(const std::string& clipName, float timePos, std::vector<XMFLOAT4X4>& finalTransforms) const
{
	UINT numBones = m_BoneOffsets.size();

	std::vector<XMFLOAT4X4> toParentTransforms(numBones);

	// Interpolate all the bones of this clip at the given time instance.
	// 주어진 클립에 이 클립의 모든 뼈를 보간하십시오.
	auto clip = m_Animations.find(clipName);
	clip->second.Interpolate(timePos, toParentTransforms);

	//
	// Traverse the hierarchy and transform all the bones to the root space.
	// 모든 계층구조를 순회하고 루트 공간으로 뼈들을 변환.

	std::vector<XMFLOAT4X4> toRootTransforms(numBones);

	// The root bone has index 0.  The root bone has no parent, so its toRootTransform
	// is just its local bone transform.
	toRootTransforms[0] = toParentTransforms[0];

	//Now find the toRootTransform of the children.
	for (UINT i = 1; i < numBones; ++i)
	{
		XMMATRIX toParent = XMLoadFloat4x4(&toParentTransforms[i]);
		int parentIndex = m_BoneHierarchy[i];

		XMMATRIX parentToRoot = XMLoadFloat4x4(&toRootTransforms[parentIndex]);

		XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);

		XMStoreFloat4x4(&toRootTransforms[i], toRoot);

	}

	// Premultiply by the bone offset transform to get the final transform.
	for (UINT i = 0; i < numBones; ++i)
	{
		XMMATRIX offset = XMLoadFloat4x4(&m_BoneOffsets[i]);
		XMMATRIX toRoot = XMLoadFloat4x4(&toRootTransforms[i]);
		XMMATRIX finalTransform = XMMatrixMultiply(offset, toRoot);
		XMStoreFloat4x4(&finalTransforms[i], XMMatrixTranspose(finalTransform));
	}


}