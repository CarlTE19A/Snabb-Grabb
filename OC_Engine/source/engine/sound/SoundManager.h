#pragma once
#include <iostream>
#include <map>
#include "../../external/FmodIncludes/All includes, core and studio/fmod_studio.hpp"
#include "../../external/FmodIncludes/All includes, core and studio/fmod_common.h"

#include "../math/Matrix.h"
#include <unordered_map>
///property of Adam dahlin copy right 2023,2024 osv  visit www.adamdahlin.se 
// Contact for questions:  kontakt@adamdahlin.se

enum class eChannels
{
	eMaster,
	eAmbience,
	eSFX,
	eScore
};

enum class eEvent
{
	eNull,
	ePlayer,
};



//-----------------------Get Fmod Classes----------------------------
namespace FMOD
{
	class System;
	namespace Studio
	{
		class EventInstance;
		class Bus;
		class Bank;
		class System;
		class EventDescription;

	}
}

struct SoundContext
{
	//FMOD::System* myFmodSystemFactory = NULL;
	FMOD::Studio::System* myFmodSystem;
};


class SoundManager
{
public:

	enum class eDescriptions
	{
		eMusic,
		eCharacterActions,
		eAmbience,
		/*eStaticEnemy,
		eMovingEnemy,*/
		eCheckpoint,
		eMenu,
		eScriptedEvents,
		eProjectile


	};
	SoundManager();
	~SoundManager();
	void Init();
	void Update();

	void SetListenerTransform(const DirectX::XMMATRIX& aTransform);
	void InitNewInstance2D(eDescriptions aDescribtion, int anID);
	void InitNewInstance3D(eDescriptions aDescribtion, int anID, Vector3<float> aTransform);
	void InitGlobalAudio(eDescriptions aDescribtion, int anID);
	void EditParameter(int anID, const char* aParameterName, float aParameterValue);
	void EditGlobalParameter(int anID, const char* aParameterName, float aParameterValue);
	void PauseEvent(int anID);
	void ResumeEvent(int anID);
	void PauseGlobalEvent(int anID);
	void ResumeGlobalEvent(int anID);
	void PlayEvent(int anID);
	void PlayGlobalEvent(int anID);
	void UpdatePosition(int anID, Vector3<float> aTransform);
	void UpdatePlayerPosition(Vector3<float>& aTransform, int anID);
	void StopEvent(int anID);
	void SetEventVolume(int anID, float aVol);
	void SetChannelVolume(const eChannels aChannelID, float aVolume);
	const float GetChannelVolume(const eChannels aChannelID)const;
	void MuteChannels(const eChannels aChannelID, const bool aMute);
	void ClearInstances();
	void ClearGlobalInstances();

	inline void MuteSFX() { myShouldMuteSFX = true; myShouldUnMuteSFX = false; }
	inline void UnmuteSFX() { myShouldMuteSFX = false; myShouldUnMuteSFX = true; }
private:
	static SoundManager* mySoundManagerInstance;
	FMOD::System* myInfoSystem;
	//	void HandleAudioMessage(const P3::Sound::AudioMessage& aMSG);
		//SoundManager(const SoundManager&) = delete;
		//SoundManager operator=(const SoundManager&) = delete;

	FMOD_3D_ATTRIBUTES myPlayerTransform;
	FMOD_3D_ATTRIBUTES myEnemyTransform;
	//---Transform Map---
	std::map <int, FMOD_3D_ATTRIBUTES*  > myTransforms;

	//---System---
	SoundContext myAudioContext;

	//---Channels---
	std::map<eChannels, FMOD::Studio::Bus*> myChannels;

	//---EventInstance---
	std::map<int, FMOD::Studio::EventInstance*> myEvents;



	std::map<int, FMOD::Studio::EventInstance*> myGlobalEvents;

	//---EventInstance-MAP--


	//---EventDEscription
	std::map<eDescriptions, FMOD::Studio::EventDescription* > myEventDescription;

	//---Banks----
	FMOD::Studio::Bank* myMaster = nullptr;
	FMOD::Studio::Bank* myStrings = nullptr;
	FMOD::Studio::Bank* myAmbience = nullptr;
	FMOD::Studio::Bank* myCharacter = nullptr;
	//FMOD::Studio::Bank* myEnemies = nullptr;
	FMOD::Studio::Bank* myMusic = nullptr;
	FMOD::Studio::Bank* myMenu = nullptr;



	FMOD::Studio::EventInstance* myPlayer;
	//FMOD::Studio::EventInstance* myMusic;

	FMOD_RESULT result;

	bool myShouldMuteSFX;
	bool myShouldUnMuteSFX;
};
