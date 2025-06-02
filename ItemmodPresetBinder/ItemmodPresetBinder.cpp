#include "pch.h"
#include "ItemmodPresetBinder.h"


BAKKESMOD_PLUGIN(ItemmodPresetBinder, "Bind itemmod presets to garage presets", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void ItemmodPresetBinder::onLoad()
{
	_globalCvarManager = cvarManager;

	pluginInit();


	// register cvars
	m_enabled = std::make_shared<bool>(true);
	m_showIndexes = std::make_shared<bool>(false);
	m_bindingColor = std::make_shared<LinearColor>(0.1f, 0.7f, 1.0f, 1.0f); // blue
	m_equippedPresetColor = std::make_shared<LinearColor>(0.1f, 1.0f, 0.1f, 1.0f); // green

	cvarManager->registerCvar(Cvars::enabled, "1", "Enable the plugin", true, true, 0, true, 1).bindTo(m_enabled);
	cvarManager->registerCvar(Cvars::showIndexes, "0", "Show garage preset indexes", true, true, 0, true, 1).bindTo(m_showIndexes);
	cvarManager->registerCvar(Cvars::bindingColor, "#25C5FFFF", "Highlight color for garage presets that have a binding").bindTo(m_bindingColor);
	cvarManager->registerCvar(Cvars::equippedPresetColor, "#17FF09FF", "Highlight color for equipped garage preset").bindTo(m_equippedPresetColor);
	

	// hook events
	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::GFxData_LoadoutSets_TA_EquipPreset,
		std::bind(&ItemmodPresetBinder::event_GFxData_LoadoutSets_TA_EquipPreset,
			this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::GFxData_LoadoutSets_TA_DeletePreset,
		std::bind(&ItemmodPresetBinder::event_GFxData_LoadoutSets_TA_DeletePreset,
			this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	gameWrapper->HookEventWithCallerPost<ActorWrapper>(Events::ProfileLoadoutSave_TA_GetLoadoutSet,
		std::bind(&ItemmodPresetBinder::event_ProfileLoadoutSave_TA_GetLoadoutSet,
			this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}
