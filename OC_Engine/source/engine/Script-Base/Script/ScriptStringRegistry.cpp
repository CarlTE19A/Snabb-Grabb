#include "stdafx.h"

#include "ScriptStringRegistry.h"

#include <cassert>

std::vector<std::unique_ptr<std::string>> ScriptStringRegistry::myDeduplicatedStrings;
std::unordered_map<std::string_view, ScriptStringId> ScriptStringRegistry::myStringToDeduplicatedString;

ScriptStringId ScriptStringRegistry::RegisterOrGetString(std::string_view string)
{
	auto it = myStringToDeduplicatedString.find(string);
	if (it != myStringToDeduplicatedString.end())
		return it->second;

	ScriptStringId id = { (unsigned int)myDeduplicatedStrings.size() };

	myDeduplicatedStrings.emplace_back(std::make_unique<std::string>(string));
	myStringToDeduplicatedString[*myDeduplicatedStrings.back()] = id;

	return id;
}

std::string_view ScriptStringRegistry::GetStringFromStringId(ScriptStringId stringId)
{
	assert("Invalid string id" && stringId.id < myDeduplicatedStrings.size());
	return *myDeduplicatedStrings[stringId.id];
}
