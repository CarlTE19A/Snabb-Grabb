
#include "stdafx.h"

#include "GameNodes.h"
#include "GameUpdateContext.h"

#include "../Script/ScriptStringRegistry.h"
#include "../Script/ScriptNodeBase.h"
#include "../Script/ScriptNodeTypeRegistry.h"
#include "scene/Scene.h"
#include "gameObject/GameObject.h"
#include "component/PhysicsComponent.h"
#include <vector>

class GetEnemyListSize : public ScriptNodeBase
{
private:

	ScriptPinId myOutPinId;


public:

	void Init(const ScriptCreationContext& context) override
	{
		{
			ScriptPin flowOutPin = {};
			flowOutPin.dataType = ScriptLinkDataType::Int;
			flowOutPin.defaultValue.data = 0;
			flowOutPin.name = ScriptStringRegistry::RegisterOrGetString("EnemyListSize");
			flowOutPin.node = context.GetNodeId();
			flowOutPin.role = ScriptPinRole::Output;

			myOutPinId = context.FindOrCreatePin(flowOutPin);
		}
	}


	ScriptLinkData ReadPin(ScriptExecutionContext& aContext, ScriptPinId) const override
	{
		int countEnemy = aContext.GetUpdateContext().myScene->GetEnemyCount();
		return { countEnemy };
	}
};

class FinishLineNode : public ScriptNodeBase
{
private:

	ScriptPinId myOutPinId;
	ScriptPinId myInputBoolId;


public:

	void Init(const ScriptCreationContext& context) override
	{
		{
			ScriptPin flowPin = {};
			flowPin.dataType = ScriptLinkDataType::Flow;
			flowPin.name = ScriptStringRegistry::RegisterOrGetString("Run");
			flowPin.node = context.GetNodeId();
			flowPin.role = ScriptPinRole::Input;

			context.FindOrCreatePin(flowPin);
		}

		{
			ScriptPin flowOutPin = {};
			flowOutPin.dataType = ScriptLinkDataType::Flow;
			flowOutPin.name = ScriptStringRegistry::RegisterOrGetString("");
			flowOutPin.node = context.GetNodeId();
			flowOutPin.role = ScriptPinRole::Output;

			myOutPinId = context.FindOrCreatePin(flowOutPin);
		}


		{
			ScriptPin flowOutPin = {};
			flowOutPin.dataType = ScriptLinkDataType::Bool;
			flowOutPin.defaultValue.data = false;
			flowOutPin.name = ScriptStringRegistry::RegisterOrGetString("FinishLine");
			flowOutPin.node = context.GetNodeId();
			flowOutPin.role = ScriptPinRole::Input;

			myInputBoolId = context.FindOrCreatePin(flowOutPin);
		}
	}

	ScriptNodeResult Execute(ScriptExecutionContext& context, ScriptPinId)const  override
	{


		std::vector<GameObject*> gameObjectList = context.GetUpdateContext().myScene->GetGameObjectList();
		bool trueOrFalse = std::get<bool>(context.ReadInputPin(myInputBoolId).data);
		for (size_t i = 0; i < gameObjectList.size(); i++)
		{
			if (gameObjectList[i]->GetTag() == "Goal")
			{
				PhysicsComponent* physicsComponent = gameObjectList[i]->GetComponent<PhysicsComponent>();

				if (physicsComponent)
				{
					physicsComponent->SetActive(trueOrFalse);
				}

				gameObjectList[i]->SetActive(trueOrFalse);
			}
		}

		context.TriggerOutputPin(myOutPinId);

		return ScriptNodeResult::Finished;
	}

	/*bool ShouldExecuteAtStart() const override { return false; }*/
};


void RegisterGameNodes()
{
	ScriptNodeTypeRegistry::RegisterType<GetEnemyListSize>("Game/GetEnemyList", "return a int on how much enemy is in the scene");
	ScriptNodeTypeRegistry::RegisterType<FinishLineNode>("Game/Finish", "FinishLine bool");
}

