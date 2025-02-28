#include"stdafx.h"
#include "ParticleCache.h"

void ParticleCache::Init()
{
	Texture* temp = new Texture();
	temp->Initialize(TextureInitializationType::DDS, STATIC_ASSET_PATH "Particles/spraksprite_round_c.dds", false);

	std::string textureName = "Particles/spraksprite_round_c.dds";

	myParticleTexturesCached[textureName] = *temp;

	temp->Initialize(TextureInitializationType::DDS, STATIC_ASSET_PATH "Particles/spraksprite_round_m.dds", false);

	textureName = "Particles/spraksprite_round_m.dds";

	myParticleTexturesCached[textureName] = *temp;

	temp->Initialize(TextureInitializationType::DDS, STATIC_ASSET_PATH "Particles/spraksprite_round_fx.dds", false);

	textureName = "Particles/spraksprite_round_fx.dds";

	myParticleTexturesCached[textureName] = *temp;

}
