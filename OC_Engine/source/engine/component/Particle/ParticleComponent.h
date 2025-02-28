#include "component\Component.h"

class ParticleSystem;

class ParticleComponent : public Component
{
public:
	ParticleComponent();
	~ParticleComponent();

	ParticleComponent(const ParticleComponent&);
	// Inherited via Component
	void Init(int aNumberOfParticles);

	void Update(const float aDeltaTime, Scene& aScene) override;
	void ImguiUpdate() override;
	void Reset() override;

	void RenderParticles();

	ParticleSystem& GetParticleSystem() { return *myParticleSystem; }

private:

	bool myIsLodedcorrectly;

	ParticleSystem* myParticleSystem;
};