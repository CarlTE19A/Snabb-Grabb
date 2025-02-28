#include "ParticleComponent.h"
#include "stdafx.h"
#include "ParticleComponent.h"
#include "math\Transform.h"
#include "gameObject\GameObject.h"

#include "ParticleSystem\ParticleSystem.h"

ParticleComponent::ParticleComponent()
{
}

ParticleComponent::~ParticleComponent()
{
	delete myParticleSystem;
	myParticleSystem = nullptr;
}

ParticleComponent::ParticleComponent(const ParticleComponent& aOther)
	: myIsLodedcorrectly(aOther.myIsLodedcorrectly)
	, myParticleSystem(new ParticleSystem(*aOther.myParticleSystem))
{

}

void ParticleComponent::Init(int aNumberOfParticles)
{

	aNumberOfParticles;
	myParticleSystem = new ParticleSystem();
	myIsLodedcorrectly = myParticleSystem->Init(aNumberOfParticles);

}

void ParticleComponent::Update(const float aDeltaTime, Scene& aScene)
{
	aDeltaTime;
	aScene;
	//if (myIsLodedcorrectly)
	//{
	myParticleSystem->SetPosition(myParent->GetTransform()->GetPosition());
	myParticleSystem->SetTransform(myParent->GetTransform()->GetMatrix());
	myParticleSystem->Update();
	//RenderParticles();
//}
}

void ParticleComponent::ImguiUpdate()
{}

void ParticleComponent::Reset()
{}

void ParticleComponent::RenderParticles()
{
	myParticleSystem->EmitParticles();
}
