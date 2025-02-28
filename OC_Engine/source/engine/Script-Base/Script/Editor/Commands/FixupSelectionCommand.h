#pragma once

#include "../ScriptEditorCommand.h"
#include <vector>


class FixupSelectionCommand : public ScriptEditorCommand
{
	std::shared_ptr<AbstractCommand> myCommand;
	std::vector<ScriptNodeId> mySelectedNodes;
	std::vector<ScriptLinkId> mySelectedLinks;
public:
	FixupSelectionCommand(Script& script, ScriptEditorSelection& selection, const std::shared_ptr<AbstractCommand>& command)
		: ScriptEditorCommand(script, selection)
		, myCommand(command)
	{}

	void ExecuteImpl() override;
	void UndoImpl() override;
};
