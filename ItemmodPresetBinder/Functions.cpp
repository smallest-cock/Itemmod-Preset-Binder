#include "pch.h"
#include "ItemmodPresetBinder.h"


namespace files
{
	json getJson(const fs::path& file_path)
	{
		json j;

		if (!fs::exists(file_path))
		{
			LOG("[ERROR] File doesn't exist: '{}'", file_path.string());
			return j;
		}

		try
		{
			std::ifstream file(file_path);
			file >> j;
		}
		catch (const std::exception& e)
		{
			LOG("[ERROR] Unable to read '{}': {}", file_path.filename().string(), e.what());
		}

		return j;
	}

	bool writeJson(const fs::path& file_path, const json& j)
	{
		try
		{
			std::ofstream file(file_path);

			if (file.is_open())
			{
				file << j.dump(4); // pretty-print with 4 spaces indentation
				file.close();
			}
			else
			{
				LOG("[ERROR] Couldn't open file for writing: {}", file_path.string());
			}

			return true;
		}
		catch (const std::exception& e)
		{
			LOG("[ERROR] Unable to write to '{}': {}", file_path.filename().string(), e.what());
			return false;
		}
	}
}


json ItemmodPresetBinding::toJson() const
{
	return json{ {"enabled", enabled}, {"code", code} };
}

void ItemmodPresetBinding::fromJson(const json& j)
{
	if (j.contains("enabled"))
		enabled = j["enabled"].get<bool>();
	if (j.contains("code"))
		code = j["code"].get<std::string>();
}


void ItemmodPresetBinder::pluginInit()
{
	setFilepaths();
	loadBindingsFromJson();
}

void ItemmodPresetBinder::setFilepaths()
{
	m_pluginFolder = gameWrapper->GetDataFolder() / "ItemmodPresetBinder";
	m_bindingsJson = m_pluginFolder / "bindings.json";

	if (!fs::exists(m_pluginFolder))
	{
		LOG("Folder not found: \"{}\"", m_pluginFolder.string());
		LOG("Creating it...");
		fs::create_directories(m_pluginFolder);
	}

	if (!fs::exists(m_bindingsJson))
	{
		LOG("File not found: \"{}\"", m_bindingsJson.string());
		LOG("Creating it...");

		fs::create_directories(m_bindingsJson.parent_path());
		std::ofstream jsonFile(m_bindingsJson);
		jsonFile << "{}";
		jsonFile.close();
	}
}

void ItemmodPresetBinder::loadBindingsFromJson()
{
	json data = files::getJson(m_bindingsJson);
	if (data.empty() || !data.is_object())
		return;

	int loaded = 0;
	for (const auto& [key, val] : data.items())
	{
		try
		{
			int index = std::stoi(key);
			ItemmodPresetBinding binding;
			binding.fromJson(val);
			m_itemmodBindings[index] = binding;
			loaded++;
		}
		catch (const std::exception& e)
		{
			LOG("ERROR: Failed to parse binding for key {}: {}", key, e.what());
		}
	}

	LOG("Loaded {} itemmod bindings from JSON", loaded);
}



void ItemmodPresetBinder::updateGaragePresetState(const TArray<FGFxLoadoutSet>& loadoutSets)
{
	m_garagePresetState.clear();

	int loadoutSetsCount = loadoutSets.size();
	for (int i = 0; i < loadoutSetsCount; ++i)
	{
		const FGFxLoadoutSet& set = loadoutSets[i];
		m_garagePresetState.emplace_back(set.Name.to_wrapper().ToString(), set.Index, static_cast<bool>(set.bEquipped));
	}
}

void ItemmodPresetBinder::updateGaragePresetState(UProfileLoadoutSave_TA* loadoutSave)
{
	if (!loadoutSave)
		return;

	m_garagePresetState.clear();

	int loadoutSetsCount = loadoutSave->LoadoutSets.size();
	for (int i = 0; i < loadoutSetsCount; ++i)
	{
		ULoadoutSet_TA* set = loadoutSave->LoadoutSets[i];
		if (!set)
			continue;

		m_garagePresetState.emplace_back(set->LoadoutSetName.to_wrapper().ToString(), i, set == loadoutSave->EquippedLoadoutSet);
	}
}

void ItemmodPresetBinder::addBinding()
{
	m_itemmodBindings[m_guiSelectedPresetIndex] = ItemmodPresetBinding{};
}

void ItemmodPresetBinder::deleteSelectedBinding()
{
	m_itemmodBindings.erase(m_guiSelectedPresetIndex);
	
	// update selected preset in gui to be the equipped one
	for (int i = 0; i < m_garagePresetState.size(); ++i)
	{
		const auto& preset = m_garagePresetState[i];
		if (preset.equipped)
		{
			m_guiSelectedPresetIndex = i;
			break;
		}
	}
}

void ItemmodPresetBinder::writeBindingsToJson()
{
	json data;

	int added = 0;
	for (const auto& [garageIndex, binding] : m_itemmodBindings)
	{
		if (binding.code.empty())
			continue;

		data[std::to_string(garageIndex)] = binding.toJson();
		added++;
	}

	files::writeJson(m_bindingsJson, data);
	LOG("Wrote {} bindings to JSON", added);
}

void ItemmodPresetBinder::updateBindingsFromDeletedGaragePreset(int deletedIndex)
{
	m_itemmodBindings.erase(deletedIndex); // remove the element at deletedIndex (if it exists)

	// decrement all keys that are greater than deletedIndex
	std::map<int, ItemmodPresetBinding> updatedBindings;
	for (auto& [key, binding] : m_itemmodBindings)
	{
		if (key > deletedIndex)
			updatedBindings[key - 1] = std::move(binding); // Shift down
		else
			updatedBindings[key] = std::move(binding); // Keep unchanged
	}

	m_itemmodBindings = std::move(updatedBindings); // replace the original map
}

void ItemmodPresetBinder::findAndApplyBinding(int idx)
{
	auto it = m_itemmodBindings.find(idx);
	if (it == m_itemmodBindings.end())
		return;

	ItemmodPresetBinding& binding = it->second;
	if (!binding.enabled || binding.code.empty())
		return;

	cvarManager->executeCommand(std::format("{} {}", m_itemmodCommand, binding.code)); // "cl_itemmod_code <code>"
}