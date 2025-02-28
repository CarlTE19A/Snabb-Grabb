

#include "stdafx.h"

#include "FixUpSelectionCommand.h"
#include "../ScriptEditorSelection.h"


void FixupSelectionCommand::ExecuteImpl()
{
	mySelectedLinks = mySelection.mySelectedLinks;
	mySelectedNodes = mySelection.mySelectedNodes;
	myCommand->Execute();
}

void FixupSelectionCommand::UndoImpl()
{
	myCommand->Undo();
	mySelection.mySelectedLinks = mySelectedLinks;
	mySelection.mySelectedNodes = mySelectedNodes;
}