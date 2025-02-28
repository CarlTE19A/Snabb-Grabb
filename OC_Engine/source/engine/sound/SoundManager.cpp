#include "stdafx.h"

#include "SoundManager.h"

#include "../../assets/Music/fmod_studio_guids.hpp"
#include "../../external/FmodIncludes/All includes, core and studio/fmod.h"
///property of Adam dahlin copy right 2023 visit www.adamdahlin.se 

#define UNREFERENCED_PARAMETER(p) (p)


// The constructor Adds a reciver to listen what messages is sent here
SoundManager::SoundManager()
{
}
//When deconstructor activates we remove the message Reciver
SoundManager::~SoundManager()
{
	if (myAudioContext.myFmodSystem != nullptr)
		myAudioContext.myFmodSystem->release();
}
//-------------------------------------------------------------------
void SoundManager::Init()
{
	// INSERT MAPS.
	// -------Channels-----------------------------------------
	myChannels.insert({ eChannels::eMaster,/*--*/ nullptr });
	myChannels.insert({ eChannels::eSFX,/*-----*/ nullptr });
	myChannels.insert({ eChannels::eAmbience,/**/ nullptr });
	myChannels.insert({ eChannels::eScore,/*---*/ nullptr });
	// -------EventDescription---------------------------------
	myEventDescription.insert({ eDescriptions::eCharacterActions, nullptr });
	//myEventDescription.insert({ eDescriptions::eMovingEnemy, nullptr });
	//myEventDescription.insert({ eDescriptions::eStaticEnemy, nullptr });
	myEventDescription.insert({ eDescriptions::eMenu, nullptr });
	myEventDescription.insert({ eDescriptions::eMusic, nullptr });
	myEventDescription.insert({ eDescriptions::eScriptedEvents, nullptr });
	//myEventDescription.insert({ eDescriptions::eProjectile, nullptr });
	myEventDescription.insert({ eDescriptions::eAmbience, nullptr });
	//myEventDescription.insert({ eDescriptions::, nullptr });

	// --------Init System.
	result = FMOD::Studio::System::create(&myAudioContext.myFmodSystem);
	result = myAudioContext.myFmodSystem->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
	result = myAudioContext.myFmodSystem->getCoreSystem(&myInfoSystem);
	//----------------------------------------------------

	// Init Banks
	//-----------------------------------------
	result = myAudioContext.myFmodSystem->loadBankFile("../assets/Music/Desktop/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &myMaster);
	result = myAudioContext.myFmodSystem->loadBankFile("../assets/Music/Desktop/Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &myStrings);
	result = myAudioContext.myFmodSystem->loadBankFile("../assets/Music/Desktop/Character.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &myCharacter);
	//result = myAudioContext.myFmodSystem->loadBankFile("..././Unity_SP4/Assets/FMOD/Banks/Enemies.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &myEnemies);
	result = myAudioContext.myFmodSystem->loadBankFile("../assets/Music/Desktop/Menu.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &myMenu);
	result = myAudioContext.myFmodSystem->loadBankFile("../assets/Music/Desktop/Music.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &myMusic);
	result = myAudioContext.myFmodSystem->loadBankFile("../assets/Music/Desktop/Ambience.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &myAmbience);

	// Init Bus For Mastering Channels
	result = myAudioContext.myFmodSystem->getBusByID(&FSPRO::Bus::Mixer_Master, &myChannels.at(eChannels::eMaster));
	//myAudioContext.myFmodSystem->getBusByID(&FSPRO::Bus::Ambience, &myChannels.at(eChannels::eAmbience));
	//myAudioContext.myFmodSystem->getBusByID(&FSPRO::Bus::Score, &myChannels.at(eChannels::eScore));
	//myAudioContext.myFmodSystem->getBusByID(&FSPRO::Bus::SFX, &myChannels.at(eChannels::eSFX));


	// Init Base Volume at startup
	myChannels.at(eChannels::eMaster)->setVolume(1.0f);
	myChannels.at(eChannels::eAmbience)->setVolume(1.0f);
	myChannels.at(eChannels::eScore)->setVolume(1.0f);
	myChannels.at(eChannels::eSFX)->setVolume(1.0f);

	// Assign Event ID To myEventDescriptionMap
	myAudioContext.myFmodSystem->getEventByID(&FSPRO::Event::Character, &myEventDescription.at(eDescriptions::eCharacterActions));
	//myAudioContext.myFmodSystem->getEventByID(&FSPRO::Event::Enemy, &myEventDescription.at(eDescriptions::eMovingEnemy));
	//myAudioContext.myFmodSystem->getEventByID(&FSPRO::Event::EnemyStatic, &myEventDescription.at(eDescriptions::eStaticEnemy));
	myAudioContext.myFmodSystem->getEventByID(&FSPRO::Event::Menu, &myEventDescription.at(eDescriptions::eMenu));
	myAudioContext.myFmodSystem->getEventByID(&FSPRO::Event::Music, &myEventDescription.at(eDescriptions::eMusic));
	//myAudioContext.myFmodSystem->getEventByID(&FSPRO::Event::ScriptedEvents, &myEventDescription.at(eDescriptions::eScriptedEvents));
	//myAudioContext.myFmodSystem->getEventByID(&FSPRO::Event::Projectiles, &myEventDescription.at(eDescriptions::eProjectile));
	myAudioContext.myFmodSystem->getEventByID(&FSPRO::Event::Ambience, &myEventDescription.at(eDescriptions::eAmbience));

	//Set Listener transform
	//Forward Unit vector for camera rotation
	myPlayerTransform.up = { 0,1,0 };
	myPlayerTransform.forward = { 1,0,0 };
	myPlayerTransform.position = { 0,0,0 };
	myPlayerTransform.velocity = { 0,0,0 };

	//-------------------------OK--------------------------
	//std::cout << "Init SoundManager Completed" << std::endl;
	//-----------------------------------------------------
}
//-------------------------------------------------------------------
void SoundManager::SetListenerTransform(const DirectX::XMMATRIX& aTransform)
{
	aTransform;
	//myEnemyTransform = Get3DAttributes(aTransform);

}
void SoundManager::Update()
{
	if (myShouldMuteSFX)
	{
		MuteChannels(eChannels::eSFX, true);
		myShouldMuteSFX = false;
		myShouldUnMuteSFX = false;
	}
	if (myShouldUnMuteSFX)
	{
		MuteChannels(eChannels::eSFX, false);
		myShouldMuteSFX = false;
		myShouldUnMuteSFX = false;
	}

	myAudioContext.myFmodSystem->setListenerAttributes(0, &myPlayerTransform);
	myAudioContext.myFmodSystem->update();
}
//-------------------------------------------------------------------

//-------------------------------------------------------------------
void SoundManager::PlayEvent(int anID)
{
	if (myEvents.size() != 0 && myEvents.count(anID) > 0)
	{
		//std::cout << "heeej" << std::endl;
		if (myEvents.at(anID))
		{
			//std::cout << "";
		}
		myEvents.at(anID)->start();

	}
}
//-------------------------------------------------------------------
void SoundManager::PlayGlobalEvent(int anID)
{
	myGlobalEvents.at(anID)->start();
}
//-------------------------------------------------------------------
void SoundManager::EditParameter(int anID, const char* aParameterName, float aParameterValue)
{
	if (myEvents.size() != 0 && myEvents.count(anID) > 0)
	{
		myEvents.at(anID)->setParameterByName(aParameterName, aParameterValue);
	}
	//std::cout << aParameterValue << std::endl;
}
//-------------------------------------------------------------------
void SoundManager::EditGlobalParameter(int anID, const char* aParameterName, float aParameterValue)
{
	if (myGlobalEvents.size() != 0 && myGlobalEvents.count(anID) > 0)
	{
		myGlobalEvents.at(anID)->setParameterByName(aParameterName, aParameterValue);
	}
	//std::cout << aParameterValue << std::endl;
}
//-------------------------------------------------------------------
void SoundManager::SetChannelVolume(const eChannels aChannelID, float aVolume)
{
	auto& bus = myChannels.at(aChannelID);
	bus->setMute(false);
	bus->setVolume(aVolume);
}
//-------------------------------------------------------------------
const float SoundManager::GetChannelVolume(const eChannels aChannelID) const
{
	auto& bus = myChannels.at(aChannelID);
	float volume;
	bus->getVolume(&volume);
	const float returnfloat = volume;
	return returnfloat;
}
//-------------------------------------------------------------------
void SoundManager::MuteChannels(const eChannels aChannelID, const bool aMute)
{
	auto& bus = myChannels.at(aChannelID);
	bus->setMute(aMute);
}
void SoundManager::UpdatePosition(int anID, Vector3<float> aTransform)
{
	if (myEvents.size() != 0)
	{
		myTransforms.at(anID)->position = { aTransform.x, aTransform.y, aTransform.z };
		myEvents.at(anID)->set3DAttributes(myTransforms.at(anID));
		Update();
	}
}
//-------------------------------------------------------------------
void SoundManager::UpdatePlayerPosition(Vector3<float>& aTransform, int anID)
{
	//static int time = 0;
	//time++;
	//if (time > 360) { time = time - 360; }
	//float radians = time * 3.14f / 180.0f;
	if (myEvents.size() != 0)
	{
		//myPlayerTransform.forward = {
		//	sin(radians), 0, cos(radians)
		//};
		//std::cout << *(Tga::Vector3f*)&myPlayerTransform.forward << std::endl;
		//std::cout << aTransform << std::endl;
		myPlayerTransform.position = { aTransform.x, aTransform.y, aTransform.z };
		myEvents.at(anID)->set3DAttributes({ &myPlayerTransform });
	}
}
void SoundManager::StopEvent(int anID)
{
	if (myEvents.size() != 0 && myEvents.count(anID) > 0)
	{
		//std::cout << "heeej" << std::endl;
		myEvents.at(anID)->stop(FMOD_STUDIO_STOP_IMMEDIATE);

	}
}
void SoundManager::SetEventVolume(int anID, float aVol)
{
	if (myEvents.size() != 0 && myEvents.count(anID) > 0)
	{
		//std::cout << "heeej" << std::endl;
		myEvents.at(anID)->setVolume(aVol);

	}
}
//-------------------------------------------------------------------
void SoundManager::InitGlobalAudio(eDescriptions aDescribtion, int anID)
{
	FMOD::Studio::EventInstance* newEvent;
	myGlobalEvents.insert({ anID, newEvent });
	myEventDescription.at(aDescribtion)->createInstance(&myGlobalEvents.at(anID));
}
//-------------------------------------------------------------------
void SoundManager::InitNewInstance2D(eDescriptions aDescribtion, int anID)
{
	FMOD::Studio::EventInstance* newEvent;
	myEvents.insert({ anID, newEvent });
	myEventDescription.at(aDescribtion)->createInstance(&myEvents.at(anID));
}
//-------------------------------------------------------------------
void SoundManager::InitNewInstance3D(eDescriptions aDescribtion, int anID, Vector3<float> aTransform)
{
	aTransform;
	FMOD::Studio::EventInstance* newEvent;
	FMOD_3D_ATTRIBUTES* newTransform = new FMOD_3D_ATTRIBUTES{};
	//myEvents[anID] = newEvent;
	myEvents.insert({ anID, newEvent });
	myEventDescription.at(aDescribtion)->createInstance(&myEvents.at(anID));

	newTransform->up = { 0,1,0 };
	newTransform->forward = { 0,0,1 };
	newTransform->position = { aTransform.x, aTransform.y, aTransform.z };
	newTransform->velocity = { 0,0,0 };
	myTransforms.insert({ anID, newTransform });

}
//-------------------------------------------------------------------
void SoundManager::ClearInstances()
{
	if (myEvents.size() != 0)
	{
		//for (int i = 0; i < myEvents.size(); i++)
		//{
		//	myEvents[i]->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
		//}
		for (auto& e : myEvents)
		{
			e.second->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
		}
		for (auto& event : myEvents)
		{
			event.second->release();
		}
		myEvents.clear();
		myTransforms.clear();
	}
}
//-------------------------------------------------------------------
void SoundManager::ClearGlobalInstances()
{
	if (myGlobalEvents.size() != 0)
	{
		for (int i = 0; i < myGlobalEvents.size(); i++)
		{
			myGlobalEvents[i]->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
		}
		for (auto& event : myGlobalEvents)
		{
			event.second->release();
		}
		myGlobalEvents.clear();
	}
}
//-------------------------------------------------------------------


// ------------Reserve Functions----------



//void SoundManager::StopEvent(int anID)
//{
//	UNREFERENCED_PARAMETER(anID);
//	//if (myEvents.size() != 0 && myEvents.count(anID) > 0)
//	//{
//	//	myEvents.at(anID)->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
//	//}
//}
//
////-------------------------------------------------------------------
//void SoundManager::PauseEvent(int anID)
//{
//	UNREFERENCED_PARAMETER(anID);
//	//if (myEvents.size() != 0 && myEvents.count(anID) > 0)
//	//{
//	//
//	//	myEvents.at(anID)->setPaused(true);
//	//}
//}
////-------------------------------------------------------------------
//void SoundManager::ResumeEvent(int anID)
//{
//	UNREFERENCED_PARAMETER(anID);
//	//if (myEvents.count(anID) > 0)
//	//{
//	//	myEvents.at(anID)->setPaused(false);
//	//}
//}
////-------------------------------------------------------------------
//void SoundManager::PauseGlobalEvent(int anID)
//{
//	UNREFERENCED_PARAMETER(anID);
//	//if (myEvents.size() != 0 && myEvents.count(anID) > 0)
//	//{
//	//
//	//	myEvents.at(anID)->setPaused(true);
//	//}
//}
////-------------------------------------------------------------------
//void SoundManager::ResumeGlobalEvent(int anID)
//{
//	UNREFERENCED_PARAMETER(anID);
//	//if (myEvents.count(anID) > 0)
//	//{
//	//	myEvents.at(anID)->setPaused(false);
//	//}
//}