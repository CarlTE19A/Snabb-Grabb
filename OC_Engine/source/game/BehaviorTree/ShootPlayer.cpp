#include "ShootPlayer.h"

#include "../game/PollingStation.h"
#include "scene/Scene.h"
#include "../game/Ammo/AmmoPool.h"
#include "../game/Ammo/AmmoComponent.h"
#include "component/Component.h"
#include "../game/EnemyComponent/EnemyComponent.h"
#include "gameObject/GameObject.h"
#include "math/Transform.h"

#include "../game/EnemyAnimation/AnimationType.h"

ShootPlayer::ShootPlayer(EnemyComponent* aActor)
	:ActionNode(aActor)
{
}

ShootPlayer::~ShootPlayer()
{
	myActor = nullptr;
}

bool ShootPlayer::Execute(const float aDeltaTime, Scene& aScene)
{
	aScene;
	aDeltaTime;

	if (!myActor->IsTheAnimationFinish(TurretAnimation::eWindup))
		return false;

	GameObject* ammo;

	ammo = myActor->GetPollingStation()->GetAmmoPool()->GetAmmo(AmmoPool::AmmoType::EnemyBullet);
	if (!ammo)
	{
		return false;
	}
	ammo->SetActive(true);

	Vector3f direction = (myActor->GetPollingStation()->GetPlayer()->GetPosition() - myActor->GetPosition()).GetNormalized();
	ammo->GetComponent<AmmoComponent>()->Shoot(myActor->GetPosition() + direction*50.f, direction);
	aScene.AddAmmoObject(ammo);
	myActor->SetAttackTimer();

	myActor->ChangeAnimation(TurretAnimation::eShoot);

	return true;
}
