#include <unordered_map>
#include <graphics\Texture.h>
#include <memory>


class ParticleCache
{
public:

	void Init();

	Texture& GetParticleTexture(std::string aString) { return myParticleTexturesCached[aString]; };

private:
	std::unordered_map<std::string, Texture> myParticleTexturesCached;

};