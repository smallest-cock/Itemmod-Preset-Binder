#include "pch.h"
#include "ItemmodPresetBinder.h"


void ItemmodPresetBinder::event_GFxData_LoadoutSets_TA_EquipPreset(ActorWrapper Caller, void* Params, std::string event)
{
	if (!*m_enabled)
		return;

	auto params = reinterpret_cast<EquipPreset_params*>(Params);
	if (!params)
		return;

	findAndApplyBinding(params->Index);
}

void ItemmodPresetBinder::event_GFxData_LoadoutSets_TA_DeletePreset(ActorWrapper Caller, void* Params, std::string event)
{
	auto caller = reinterpret_cast<UGFxData_LoadoutSets_TA*>(Caller.memory_address);
	if (!caller)
		return;

	auto params = reinterpret_cast<DeletePreset_params*>(Params);
	if (!params)
		return;

	LOG("DeletePreset Index: {}", params->Index);
	updateBindingsFromDeletedGaragePreset(params->Index);
	updateGaragePresetState(caller->LoadoutSetsArray);
}

void ItemmodPresetBinder::event_ProfileLoadoutSave_TA_GetLoadoutSet(ActorWrapper Caller, void* Params, std::string event)
{
	//LOG("EVENT: \"{}\"", event);

	auto caller = reinterpret_cast<UProfileLoadoutSave_TA*>(Caller.memory_address);
	if (!caller)
		return;

	updateGaragePresetState(caller);

	// apply item mod code here if a preset w binding is equipped
	int loadoutSetsCount = caller->LoadoutSets.size();
	for (int i = 0; i < loadoutSetsCount; ++i)
	{
		ULoadoutSet_TA* set = caller->LoadoutSets[i];
		if (!set)
			continue;

		if (set == caller->EquippedLoadoutSet)
			findAndApplyBinding(i);
	}
}
