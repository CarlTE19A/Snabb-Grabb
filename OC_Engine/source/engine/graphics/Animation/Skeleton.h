#pragma once
//#include <string>
//#include <vector>
#include <unordered_map>
//#include <DirectXMath.h>
#include "Pose.h"


/// <summary>
/// Holds the Skeleton for a model and useful metadata.
/// Can be used to lookup joints from names and hierarchical information for joints.
/// </summary>
struct Skeleton
{
	std::string Name;

	struct Bone
	{
		DirectX::XMMATRIX BindPoseInverse;
		int Parent;
		std::vector<unsigned int> Children;
		std::string Name;

		bool operator==(const Bone& aBone) const
		{
			const bool A = DirectX::XMVector4Equal(BindPoseInverse.r[0], aBone.BindPoseInverse.r[0]) &&
				DirectX::XMVector4Equal(BindPoseInverse.r[1], aBone.BindPoseInverse.r[1]) &&
				DirectX::XMVector4Equal(BindPoseInverse.r[2], aBone.BindPoseInverse.r[2]) &&
				DirectX::XMVector4Equal(BindPoseInverse.r[3], aBone.BindPoseInverse.r[3]);
			const bool B = Parent == aBone.Parent;
			const bool C = Name == aBone.Name;
			const bool D = Children == aBone.Children;

			return A && B && C && D;
		}

		Bone()
			: BindPoseInverse{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }, Parent(-1)
		{
		}
	};

	std::vector<Bone> Bones;
	std::unordered_map<std::string, size_t> BoneNameToIndex;
	std::vector<std::string> BoneName;

	std::unordered_map<std::wstring, size_t> myAnimationNameToIndex;

	const Bone* GetRoot() const { if (Bones.empty()) return nullptr; return &Bones[0]; }

	bool operator==(const Skeleton& aSkeleton) const
	{
		return Bones == aSkeleton.Bones;
	}

	void ConvertPoseToModelSpace(const LocalSpacePose& in, ModelSpacePose& out) const;
	void ApplyBindPoseInverse(const ModelSpacePose& in, DirectX::XMMATRIX* out) const;
private:
	void ConvertPoseToModelSpace(const LocalSpacePose& in, ModelSpacePose& out, unsigned aBoneIdx, const DirectX::XMMATRIX& aParentTransform)  const;
	
};