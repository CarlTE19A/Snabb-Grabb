#pragma once
#include "stdafx.h"
#include "Script/ScriptCommon.h"
#include "Script/ScriptNodeBase.h"
#include "Script/ScriptNodeTypeRegistry.h"
#include "Script/ScriptStringRegistry.h"
#include <iostream>

class ReturnFiveNode : public ScriptNodeBase
{
public:
	void Init(const ScriptCreationContext& context) override
	{
		
		ScriptPin outputPin = {};
		outputPin.dataType = ScriptLinkDataType::Int;
		outputPin.name = ScriptStringRegistry::RegisterOrGetString("Value");
		outputPin.node = context.GetNodeId();
		outputPin.role = ScriptPinRole::Output;

		context.FindOrCreatePin(outputPin);
	}

	ScriptLinkData ReadPin(ScriptExecutionContext&, ScriptPinId) const override
	{
		return { 5 };
	}
};



class PrintIntNode : public ScriptNodeBase
{
	ScriptPinId myIntPinId;
	ScriptPinId myOutPinId;

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
			ScriptPin intPin = {};
			intPin.dataType = ScriptLinkDataType::Int;
			intPin.name = ScriptStringRegistry::RegisterOrGetString("Value");
			intPin.node = context.GetNodeId();
			intPin.defaultValue = { 0 };
			intPin.role = ScriptPinRole::Input;

			myIntPinId = context.FindOrCreatePin(intPin);
		}
	}

	ScriptNodeResult Execute(ScriptExecutionContext& context, ScriptPinId)const  override
	{
		

		ScriptLinkData data = context.ReadInputPin(myIntPinId);
		std::cout << std::get<int>(data.data) << "\n";

		context.TriggerOutputPin(myOutPinId);

		return ScriptNodeResult::Finished;
	}
};

class PrintStringNode : public ScriptNodeBase
{
	ScriptPinId myStringPinId;
	ScriptPinId myOutPinId;
public:
	void Init(const ScriptCreationContext& context) override
	{
		

		{
			ScriptPin flowInPin = {};
			flowInPin.dataType = ScriptLinkDataType::Flow;
			flowInPin.name = ScriptStringRegistry::RegisterOrGetString("Run");
			flowInPin.node = context.GetNodeId();
			flowInPin.role = ScriptPinRole::Input;

			context.FindOrCreatePin(flowInPin);
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
			ScriptPin intPin = {};
			intPin.dataType = ScriptLinkDataType::String;
			intPin.name = ScriptStringRegistry::RegisterOrGetString("");
			intPin.node = context.GetNodeId();
			intPin.defaultValue = { ScriptStringRegistry::RegisterOrGetString("Text to print") };
			intPin.role = ScriptPinRole::Input;

			myStringPinId = context.FindOrCreatePin(intPin);
		}
	}

	ScriptNodeResult Execute(ScriptExecutionContext& context, ScriptPinId)const  override
	{
		

		ScriptLinkData data = context.ReadInputPin(myStringPinId);

		ScriptStringId stringId = std::get<ScriptStringId>(data.data);
		std::cout << ScriptStringRegistry::GetStringFromStringId(stringId) << "\n";

		context.TriggerOutputPin(myOutPinId);

		return ScriptNodeResult::Finished;
	}
};


void RegisterExampleNodes()
{
	ScriptNodeTypeRegistry::RegisterType<PrintIntNode>("Examples/PrintInt", "Prints an integer");
	ScriptNodeTypeRegistry::RegisterType<PrintStringNode>("Examples/PrintString", "Prints a string");
	ScriptNodeTypeRegistry::RegisterType<ReturnFiveNode>("Examples/ReturnFive", "Returns five");
}
