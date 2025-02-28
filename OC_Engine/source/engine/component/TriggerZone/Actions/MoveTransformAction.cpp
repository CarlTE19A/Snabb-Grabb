#include "stdafx.h"
#include "MoveTransformAction.h"
#include "gameObject/GameObject.h"

void MoveTransformAction::Activate(Scene&)
{
	percentage = 0;
	myStartPosition = target->GetTransform()->GetPosition();
	myEndPosition = myStartPosition + myOffset;
}
void MoveTransformAction::Reset()
{
	//target->GetTransform()->SetPosition(myStartPosition);
}

bool MoveTransformAction::Update(float deltaTime, Scene&)
{
	percentage += deltaTime / myTime;
	if (percentage >= 1)
	{
		percentage = 1;
	}
	target->GetTransform()->SetPosition(Vector3f::Lerp(myStartPosition, myEndPosition, percentage));
	if (percentage >= 1)
	{
		ActivateKeys();
		myActive = false;
		return true;
	}
	return false;
}

MoveTransformAction::~MoveTransformAction()
{
}
