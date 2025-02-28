#include "stdafx.h"
#include "ScriptEditorCommand.h"
#include "../Script.h"

void ScriptEditorCommand::Execute()
{
	mySequenceNumber = myScript.GetSequenceNumber();
	ExecuteImpl();
};

void ScriptEditorCommand::Undo()
{
	UndoImpl();
	myScript.SetSequenceNumber(mySequenceNumber);
};