#include "stdafx.h"

#include "Skeleton.h"

void Skeleton::ConvertPoseToModelSpace(const LocalSpacePose& in, ModelSpacePose& out) const
{
	ConvertPoseToModelSpace(in, out, 0, DirectX::XMMatrixIdentity());
	out.Count = in.Count;
}

void Skeleton::ConvertPoseToModelSpace(const LocalSpacePose& in, ModelSpacePose& out, unsigned aBoneIdx, const DirectX::XMMATRIX& aParentTransform) const
{
	const Skeleton::Bone& joint = Bones[aBoneIdx];

	out.JointTransforms[aBoneIdx] = in.JointTransforms[aBoneIdx] * aParentTransform;

	for (size_t c = 0; c < joint.Children.size(); c++)
	{
		ConvertPoseToModelSpace(in, out, joint.Children[c], out.JointTransforms[aBoneIdx]);
	}
}

void Skeleton::ApplyBindPoseInverse(const ModelSpacePose& in, DirectX::XMMATRIX* out) const
{
	for (size_t i = 0; i < Bones.size(); i++)
	{
		const Skeleton::Bone& joint = Bones[i];
		out[i] = joint.BindPoseInverse * in.JointTransforms[i];
	}
}