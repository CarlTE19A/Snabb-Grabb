#include "stdafx.h"
#include "UISpriteSheet.h"
#include "Engine.h"
#include "math\Timer.h"

void UISpriteSheet::Update()
{

	if (!myIsActive)
	{
		return;
	}

	if (myIsPlaying)
	{
		myTimer += Engine::GetInstance()->GetTimer()->GetDeltaTime();
		if (myTimer >= myTime)
		{
			myIndex++;
			if (myIndex > myCol * myRow - 1 - myMissing)
			{
				myIndex = 0;
				if (!myLoop)
				{
					myIsPlaying = false;
				}
			}
			LONG left = (LONG)(myUvs[myIndex].myStart.x * (float)myHeight);
			LONG top = (LONG)(myUvs[myIndex].myStart.y * (float)myHeight);
			LONG right = (LONG)(myUvs[myIndex].myEnd.x * (float)myWidth);
			LONG bottom = (LONG)(myUvs[myIndex].myEnd.y * (float)myWidth);
			myTextureRect = { left, top, right, bottom };
			mySprite.SetRect(myTextureRect);
			myTimer = 0.0f;
		}
	}
}
