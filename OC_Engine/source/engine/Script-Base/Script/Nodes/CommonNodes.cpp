#include "stdafx.h"
#include "CommonNodes.h"
#include "../../Script/ScriptNodeTypeRegistry.h"
#include "../../Script/ScriptStringRegistry.h"
#include "../../Script/ScriptCommon.h"
#include "../../Script/ScriptNodeBase.h"

class StartNode : public ScriptNodeBase
{
	ScriptPinId myOutputPin;

public:
	void Init(const ScriptCreationContext& context) override
	{
		ScriptPin outputPin = {};
		outputPin.dataType = ScriptLinkDataType::Flow;
		outputPin.name = ScriptStringRegistry::RegisterOrGetString("Start");
		outputPin.node = context.GetNodeId();
		outputPin.role = ScriptPinRole::Output;

		myOutputPin = context.FindOrCreatePin(outputPin);
	}

	ScriptNodeResult Execute(ScriptExecutionContext& context, ScriptPinId)const  override
	{
		context.TriggerOutputPin(myOutputPin);

		return ScriptNodeResult::Finished;
	}

	bool ShouldExecuteAtStart() const override { return true; }
};

class TickNode : public ScriptNodeBase
{
	ScriptPinId myOutPinId;

public:
	void Init(const ScriptCreationContext& context) override
	{
		{
			ScriptPin flowOutPin = {};
			flowOutPin.dataType = ScriptLinkDataType::Flow;
			flowOutPin.name = ScriptStringRegistry::RegisterOrGetString("Tick");
			flowOutPin.node = context.GetNodeId();
			flowOutPin.role = ScriptPinRole::Output;

			myOutPinId = context.FindOrCreatePin(flowOutPin);
		}
	}

	ScriptNodeResult Execute(ScriptExecutionContext& aContext, ScriptPinId)const override
	{
		aContext.TriggerOutputPin(myOutPinId);
		return ScriptNodeResult::KeepRunning;
	}

	bool ShouldExecuteAtStart() const override
	{
		return true;
	}
};

class BranchNode : public ScriptNodeBase
{

	ScriptPinId myIfTrue;
	ScriptPinId myIfFalse;
	ScriptPinId myCondition;

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
			flowOutPin.name = ScriptStringRegistry::RegisterOrGetString("True");
			flowOutPin.node = context.GetNodeId();
			flowOutPin.role = ScriptPinRole::Output;

			myIfTrue = context.FindOrCreatePin(flowOutPin);
		}
		{
			ScriptPin flowOutPin = {};
			flowOutPin.dataType = ScriptLinkDataType::Flow;
			flowOutPin.name = ScriptStringRegistry::RegisterOrGetString("False");
			flowOutPin.node = context.GetNodeId();
			flowOutPin.role = ScriptPinRole::Output;

			myIfFalse = context.FindOrCreatePin(flowOutPin);
		}
		{
			ScriptPin flowOutPin = {};
			flowOutPin.dataType = ScriptLinkDataType::Bool;
			flowOutPin.defaultValue.data = false;
			flowOutPin.name = ScriptStringRegistry::RegisterOrGetString("Condition");
			flowOutPin.node = context.GetNodeId();
			flowOutPin.role = ScriptPinRole::Input;

			myCondition = context.FindOrCreatePin(flowOutPin);
		}
	}

	ScriptNodeResult Execute(ScriptExecutionContext& aContext, ScriptPinId)const  override
	{

		bool condition = std::get<bool>(aContext.ReadInputPin(myCondition).data);
		if (condition)
		{
			aContext.TriggerOutputPin(myIfTrue);

		}
		else
		{
			aContext.TriggerOutputPin(myIfFalse);

		}
		return ScriptNodeResult::KeepRunning;
	}

	bool ShouldExecuteAtStart() const override
	{
		return true;
	}
};

class CompareInt : public ScriptNodeBase
{
	ScriptPinId myNumber;
	ScriptPinId myCompareWith;
	ScriptPinId myGreater;
	ScriptPinId myLower;
	ScriptPinId myEqual;

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
			flowOutPin.name = ScriptStringRegistry::RegisterOrGetString(">");
			flowOutPin.node = context.GetNodeId();
			flowOutPin.role = ScriptPinRole::Output;

			myGreater = context.FindOrCreatePin(flowOutPin);
		}
		{
			ScriptPin flowOutPin = {};
			flowOutPin.dataType = ScriptLinkDataType::Flow;
			flowOutPin.name = ScriptStringRegistry::RegisterOrGetString("<");
			flowOutPin.node = context.GetNodeId();
			flowOutPin.role = ScriptPinRole::Output;

			myLower = context.FindOrCreatePin(flowOutPin);
		}
		{
			ScriptPin flowOutPin = {};
			flowOutPin.dataType = ScriptLinkDataType::Flow;
			flowOutPin.defaultValue.data = false;
			flowOutPin.name = ScriptStringRegistry::RegisterOrGetString("=");
			flowOutPin.node = context.GetNodeId();
			flowOutPin.role = ScriptPinRole::Output;

			myEqual = context.FindOrCreatePin(flowOutPin);
		}

		{
			ScriptPin flowOutPin = {};
			flowOutPin.dataType = ScriptLinkDataType::Int;
			flowOutPin.defaultValue.data = 0;
			flowOutPin.name = ScriptStringRegistry::RegisterOrGetString("my Number");
			flowOutPin.node = context.GetNodeId();
			flowOutPin.role = ScriptPinRole::Input;

			myNumber = context.FindOrCreatePin(flowOutPin);
		}

		{
			ScriptPin flowOutPin = {};
			flowOutPin.dataType = ScriptLinkDataType::Int;
			flowOutPin.defaultValue.data = 0;
			flowOutPin.name = ScriptStringRegistry::RegisterOrGetString("Compare With");
			flowOutPin.node = context.GetNodeId();
			flowOutPin.role = ScriptPinRole::Input;

			myCompareWith = context.FindOrCreatePin(flowOutPin);
		}
	}

	ScriptNodeResult Execute(ScriptExecutionContext& aContext, ScriptPinId)const  override
	{

		int theNumberThatIsMy = std::get<int>(aContext.ReadInputPin(myNumber).data);
		int compareWith = std::get<int>(aContext.ReadInputPin(myCompareWith).data);

		if (theNumberThatIsMy > compareWith)
		{
			aContext.TriggerOutputPin(myGreater);

		}
		else if (theNumberThatIsMy <compareWith)
		{
			aContext.TriggerOutputPin(myLower);

		}
		else if (theNumberThatIsMy == compareWith)
		{
			aContext.TriggerOutputPin(myEqual);
		}
		return ScriptNodeResult::Finished;
	}
};




void RegisterCommonNodes()
{
	ScriptNodeTypeRegistry::RegisterType<StartNode>("Common/Start", "A node that executes once when the script starts");
	ScriptNodeTypeRegistry::RegisterType<TickNode>("Common/Tick", "Ticks every frame");
	ScriptNodeTypeRegistry::RegisterType<BranchNode>("Common/Branch", "Condition");
	ScriptNodeTypeRegistry::RegisterType<CompareInt>("Common/CompareInt", "compare two int with each other");
}
