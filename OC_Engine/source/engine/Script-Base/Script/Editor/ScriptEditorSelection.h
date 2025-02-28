#pragma once

#include <vector>
#include "../ScriptCommon.h"



	struct ScriptEditorSelection
	{
		std::vector<ScriptNodeId> mySelectedNodes;
		std::vector<ScriptLinkId> mySelectedLinks;
	};
