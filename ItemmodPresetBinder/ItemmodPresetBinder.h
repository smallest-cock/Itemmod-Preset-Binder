#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "Events.hpp"
#include "Cvars.hpp"
#include "GameStructs.hpp"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);
constexpr auto plugin_short_version = "v" stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH);


struct ItemmodPresetBinding
{
	bool enabled = true;
	std::string code;

	json toJson() const;
	void fromJson(const json& j);
};

struct GaragePresetData
{
	std::string		label;
	int				index;
	bool			equipped;
};


class ItemmodPresetBinder: public BakkesMod::Plugin::BakkesModPlugin
	,public SettingsWindowBase
	,public PluginWindowBase
{
	std::shared_ptr<bool> m_enabled;

	// for imgui
	std::shared_ptr<bool> m_showIndexes;
	std::shared_ptr<LinearColor> m_bindingColor;
	std::shared_ptr<LinearColor> m_equippedPresetColor;
	int m_guiSelectedPresetIndex = 0;

	fs::path m_pluginFolder;
	fs::path m_bindingsJson;

	static constexpr auto m_itemmodCommand = "cl_itemmod_code";

	std::vector<GaragePresetData> m_garagePresetState; // represents the state of LoadoutSets in UGFxData_LoadoutSets_TA (aka garage presets)
	std::map<int, ItemmodPresetBinding> m_itemmodBindings; // garage preset index --> itemmod binding

private:
	void pluginInit();
	void setFilepaths();
	void loadBindingsFromJson();

private:
	// event callbacks
	void event_GFxData_LoadoutSets_TA_EquipPreset(ActorWrapper Caller, void* Params, std::string event);
	void event_GFxData_LoadoutSets_TA_DeletePreset(ActorWrapper Caller, void* Params, std::string event);
	void event_ProfileLoadoutSave_TA_GetLoadoutSet(ActorWrapper Caller, void* Params, std::string event);

private:
	void updateGaragePresetState(const TArray<FGFxLoadoutSet>& loadoutSets);
	void updateGaragePresetState(UProfileLoadoutSave_TA* loadoutSave);
	void updateBindingsFromDeletedGaragePreset(int deletedIndex);

	void addBinding();
	void deleteSelectedBinding();
	void writeBindingsToJson();
	void findAndApplyBinding(int idx);

public:
	void onLoad() override;
	//void onUnload() override; // Uncomment and implement if you need a unload method
	void RenderSettings() override;
	void RenderWindow() override;
	void display_BindingsList();
	void display_BindingInfo();
};
