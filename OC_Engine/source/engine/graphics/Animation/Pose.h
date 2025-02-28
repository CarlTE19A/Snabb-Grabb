#pragma once
#include <DirectXMath.h>

#define MAX_ANIMATION_BONES 64

/// <summary>
	/// Represents a Pose of a model. Each transform is relative to its parent bone. The parent hierarchy is stored in a Skeleton class.
	/// A pose is only valid for models with the same skeleton as the animation it was created with.
	/// </summary>
struct LocalSpacePose
{
	DirectX::XMMATRIX JointTransforms[MAX_ANIMATION_BONES];
	size_t Count;
};

/// <summary>
/// Represents a Pose of a model. Each transform is relative to the model it is animating.
/// A pose is only valid for models with the same skeleton as the animation it was created with.
/// </summary>
struct ModelSpacePose
{
	DirectX::XMMATRIX JointTransforms[MAX_ANIMATION_BONES];
	size_t Count;
};