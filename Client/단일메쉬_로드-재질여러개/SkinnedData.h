#pragma once



//키 프레임은 그 시간의 bone Transfromation을 정의한다.
struct Keyframe
{
	Keyframe();
	~Keyframe();

	float TimePos;

	XMFLOAT3 Translation;
	XMFLOAT3 Scale;
	XMFLOAT4 RotationQuat;


};


//BoneAnimation은 키프레임 list를 정의한다. 
//두 개의 가까운 KeyFrames들의 시간을 보간한다.
//하나의 애니메이션은 두개의 키프레임을 가지고 있어야한다.
struct BoneAnimation
{
	float GetStartTime() const;
	float GetEndTime() const;

	void Interpolate(float t, XMFLOAT4X4& matrix) const;

	std::vector<Keyframe> Keyframes;
};


struct AnimationClip
{
	float GetClipStartTime() const;
	float GetClipEndTime() const;

	void Interpolate(float t, std::vector<XMFLOAT4X4>& boneTransform) const;

	std::vector<BoneAnimation> BoneAnimations;
};


class SkinnedData
{
public:

	UINT BoneCount() const;

	float GetClipStartTime(const std::string& clipName) const;
	float GetClipEndTime(const std::string& clipName) const;

	void Set(std::vector<int>& boneHierarchy, std::vector<XMFLOAT4X4>& boneOffsets, std::unordered_map<std::string, AnimationClip>& animations);

	// In a real project, you'd want to cache the result if there was a chance
	// that you were calling this several times with the same clipName at 
	// the same timePos.
	void GetFinalTransforms(const std::string& clipName, float timePos, std::vector<XMFLOAT4X4>& finalTransforms) const;


private:
	//프레임 번호, 본오프셋 행렬, 애니메이션 클립 데이터()
	// Gives parentIndex of ith bone.

	std::vector<int> m_BoneHierarchy;

	std::vector<XMFLOAT4X4> m_BoneOffsets;

	std::unordered_map<std::string, AnimationClip> m_Animations;
};
