
#include "stdafx.h"

#include "ScriptRuntimeInstance.h"
#include <cassert>
#include <iostream>

ScriptRuntimeInstance::ScriptRuntimeInstance(std::shared_ptr<const Script>& script)
	: myScript(script)
{

}

void ScriptRuntimeInstance::Init()
{
	if (!myScript)
	{
		std::cout << "no script found";
		return;
	}
	ScriptNodeId lastNodeId = myScript->GetLastNodeId();
	myNodeInstances.resize(lastNodeId.id + 1);

	for (ScriptNodeId currentNodeId = myScript->GetFirstNodeId(); currentNodeId.id != ScriptNodeId::InvalidId; currentNodeId = myScript->GetNextNodeId(currentNodeId))
	{
		const ScriptNodeBase& node = myScript->GetNode(currentNodeId);
		myNodeInstances[currentNodeId.id] = node.CreateRuntimeInstanceData();

		if (node.ShouldExecuteAtStart())
		{
			myActiveNodes.push_back(currentNodeId);
		}
	}
}

void ScriptRuntimeInstance::Update(const ScriptUpdateContext& updateContext)
{
	if (!myScript)
		return;
	for (int i = 0; i < myActiveNodes.size(); i++)
	{
		ScriptNodeId nodeId = myActiveNodes[i];
		ScriptExecutionContext executionContext(*this, updateContext, nodeId, myNodeInstances[nodeId.id].get());
		const ScriptNodeBase& node = myScript->GetNode(nodeId);

		ScriptNodeResult result = node.Execute(executionContext, { ScriptPinId::InvalidId });
		if (result == ScriptNodeResult::Finished)
		{
			myActiveNodes.erase(begin(myActiveNodes) + i);
			i--;
		}
	}
}

void ScriptRuntimeInstance::TriggerPin(ScriptPinId pinId, const ScriptUpdateContext& updateContext)
{
	ScriptPin pin = myScript->GetPin(pinId);
	ScriptNodeId nodeId = pin.node;
	ScriptExecutionContext executionContext(*this, updateContext, nodeId, myNodeInstances[nodeId.id].get());
	const ScriptNodeBase& node = myScript->GetNode(nodeId);

	assert(pin.dataType == ScriptLinkDataType::Flow);
	if (pin.role == ScriptPinRole::Input)
	{
		ScriptNodeResult result = node.Execute(executionContext, pinId);
		if (result == ScriptNodeResult::KeepRunning)
		{
			ActivateNode(nodeId);
		}
		else
		{
			DeactivateNode(nodeId);
		}
	}
	else
	{
		executionContext.TriggerOutputPin(pinId);
	}
}

void ScriptRuntimeInstance::ReadAgain(std::shared_ptr<const Script>& aScript)
{
	myScript = aScript;
	myNodeInstances.clear();
	myActiveNodes.clear();
	Init();
}

const Script& ScriptRuntimeInstance::GetScript() const
{
	return *myScript;
}

ScriptNodeRuntimeInstanceBase* ScriptRuntimeInstance::GetRuntimeInstance(ScriptNodeId nodeId)
{
	assert("Invalid node" && nodeId.id < myNodeInstances.size());
	return myNodeInstances[nodeId.id].get();
}

void ScriptRuntimeInstance::ActivateNode(ScriptNodeId nodeId)
{
	for (int i = 0; i < myActiveNodes.size(); i++)
	{
		if (myActiveNodes[i] == nodeId)
		{
			return;
		}
	}

	myActiveNodes.push_back(nodeId);
}

void ScriptRuntimeInstance::DeactivateNode(ScriptNodeId nodeId)
{
	for (int i = 0; i < myActiveNodes.size(); i++)
	{
		if (myActiveNodes[i] == nodeId)
		{
			myActiveNodes.erase(begin(myActiveNodes) + i);
			return;
		}
	}
}
