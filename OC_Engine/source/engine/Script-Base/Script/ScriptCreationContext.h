#pragma once

#include "../Script/ScriptCommon.h"



class Script;

class ScriptCreationContext
{
	Script& myScript;
	ScriptNodeId myNodeId;
public:
	ScriptCreationContext(Script& script, ScriptNodeId nodeId);
	ScriptNodeId GetNodeId() const { return myNodeId; };
	ScriptPinId FindOrCreatePin(ScriptPin pinData) const;
};

