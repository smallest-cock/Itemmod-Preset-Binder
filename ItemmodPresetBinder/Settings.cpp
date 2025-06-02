#include "pch.h"
#include "ItemmodPresetBinder.h"


namespace gui
{
	void spacing(int amt = 1)
	{
		for (int i = 0; i < amt; ++i)
			ImGui::Spacing();
	}

	void samelineSpacing(float xOffset)
	{
		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);
	}

	struct ScopedID
	{
		ScopedID(const char* str_id) { ImGui::PushID(str_id); }
		ScopedID(const char* str_id_begin, const char* str_id_end) { ImGui::PushID(str_id_begin, str_id_end); }
		ScopedID(const void* ptr_id) { ImGui::PushID(ptr_id); }
		ScopedID(int id) { ImGui::PushID(id); }

		~ScopedID() { ImGui::PopID(); }
	};

	struct ScopedChild
	{
		ScopedChild(const char* str_id, const ImVec2& size = ImVec2(0, 0), bool border = false, ImGuiWindowFlags flags = 0)
		{
			ImGui::BeginChild(str_id, size, border, flags);
		}

		~ScopedChild() { ImGui::EndChild(); }
	};

	namespace colors
	{
		ImVec4 pink = { 1.0f, 0.0f, 1.0f, 1.0f };
	}
}


void ItemmodPresetBinder::RenderSettings()
{
	ImGui::Text(plugin_short_version);

	gui::spacing(2);

	ImGui::TextColored(gui::colors::pink, "Idea by @atomictyler");

	gui::spacing(4);

	auto enabled_cvar = cvarManager->getCvar(Cvars::enabled);
	if (!enabled_cvar)
		return;
	bool enabled = enabled_cvar.getBoolValue();
	if (ImGui::Checkbox("Enable", &enabled))
		enabled_cvar.setValue(enabled);

	gui::spacing(8);

	// open bindings window button
	std::string openMenuCommand = "togglemenu " + GetMenuName();

	if (ImGui::Button("Open menu"))
	{
		gameWrapper->Execute([this, openMenuCommand](...)
		{
			cvarManager->executeCommand(openMenuCommand);
		});
	}

	gui::spacing(4);

	ImGui::Text("or bind this command:  ");
	ImGui::SameLine();
	ImGui::PushItemWidth(200);
	ImGui::InputText("", &openMenuCommand, ImGuiInputTextFlags_ReadOnly);
	ImGui::PopItemWidth();

	gui::spacing(8);

	if (ImGui::CollapsingHeader("Menu Settings"))
	{
		gui::spacing(2);

		auto equippedPresetColor_cvar =		cvarManager->getCvar(Cvars::equippedPresetColor);
		auto bindingColor_cvar =			cvarManager->getCvar(Cvars::bindingColor);
		auto showIndexes_cvar =				cvarManager->getCvar(Cvars::showIndexes);

		bool showIndexes = showIndexes_cvar.getBoolValue();
		if (ImGui::Checkbox("Show garage preset indexes", &showIndexes))
			showIndexes_cvar.setValue(showIndexes);

		gui::spacing(2);

		LinearColor equippedPresetColor = equippedPresetColor_cvar.getColorValue() / 255;
		if (ImGui::ColorEdit4("Equipped garage preset highlight color", &equippedPresetColor.R, ImGuiColorEditFlags_NoInputs))
			equippedPresetColor_cvar.setValue(equippedPresetColor * 255);

		LinearColor bindingColor = bindingColor_cvar.getColorValue() / 255;
		if (ImGui::ColorEdit4("Presets with a binding highlight color", &bindingColor.R, ImGuiColorEditFlags_NoInputs))
			bindingColor_cvar.setValue(bindingColor * 255);
	}

/*
	gui::spacing(2);

	if (ImGui::CollapsingHeader("debug"))
	{
		ImGui::Text("m_guiSelectedPresetIndex: %i", m_guiSelectedPresetIndex);
		ImGui::Text("m_garagePresetState size: %i", m_garagePresetState.size());
		ImGui::Text("m_presetBindings size: %i", m_itemmodBindings.size());

		gui::spacing(4);

		ImGui::TextUnformatted("m_garagePresetState:");

		for (const auto& preset : m_garagePresetState)
			ImGui::Text("[%i] %s", preset.index, preset.equipped ? std::string(preset.label + "<---").c_str() : preset.label.c_str());
	}
*/
}

void ItemmodPresetBinder::RenderWindow()
{
	if (!*m_enabled)
	{
		ImGui::TextUnformatted("Plugin disabled....");
		return;
	}

	ImVec2 total_size = ImGui::GetContentRegionAvail();

	{
		gui::ScopedChild c{ "PresetList", ImVec2(total_size.x * 0.3f, 0), true };

		display_BindingsList();
	}

	ImGui::SameLine();

	{
		gui::ScopedChild c{ "PresetInfo", ImGui::GetContentRegionAvail(), true };

		display_BindingInfo();
	}
}


float GetCurrentWindowFontScale()
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	return window->FontWindowScale;
}

void ItemmodPresetBinder::display_BindingsList()
{
	{
		gui::ScopedChild c{ "List" };

		int garagePresetCount = m_garagePresetState.size();
		ImGui::Text("%i Garage Presets:", garagePresetCount);
		ImGui::Separator();
		
		gui::spacing(2);
		
		std::unordered_set<int> indexesWithBindings;
		for (const auto& [idx, binding] : m_itemmodBindings)
			indexesWithBindings.insert(idx);

		for (int i = 0; i < garagePresetCount; ++i)
		{
			gui::ScopedID id{ i };

			const GaragePresetData& garagePreset = m_garagePresetState[i];
			bool bindingExists = indexesWithBindings.contains(i);
			std::string label;

			if (*m_showIndexes)
				label = std::format("[{}]\t{}", i, garagePreset.label);
			else
				label = garagePreset.label;

			if (garagePreset.equipped)
			{
				label += "\t<----";
				LinearColor col = *m_equippedPresetColor / 255;
				ImGui::PushStyleColor(ImGuiCol_Text, { col.R, col.G, col.B, col.A });

				const float oldScale = GetCurrentWindowFontScale();
				ImGui::SetWindowFontScale(oldScale * 1.2f);

				if (ImGui::Selectable(label.c_str(), i == m_guiSelectedPresetIndex))
					m_guiSelectedPresetIndex = i;

				ImGui::SetWindowFontScale(oldScale);
				ImGui::PopStyleColor();
			}
			else if (bindingExists)
			{
				LinearColor col = *m_bindingColor / 255;
				ImGui::PushStyleColor(ImGuiCol_Text, { col.R, col.G, col.B, col.A });

				if (ImGui::Selectable(label.c_str(), i == m_guiSelectedPresetIndex))
					m_guiSelectedPresetIndex = i;
			
				ImGui::PopStyleColor();
			}
			else
			{
				if (ImGui::Selectable(label.c_str(), i == m_guiSelectedPresetIndex))
					m_guiSelectedPresetIndex = i;
			}
		}
	}
}

void ItemmodPresetBinder::display_BindingInfo()
{
	if (m_garagePresetState.empty())
	{
		gui::spacing(8);
		ImGui::TextUnformatted("No garage presets found...\n\nInteract with the garage menu to refresh");
		return;
	}

	auto it = m_itemmodBindings.find(m_guiSelectedPresetIndex);
	if (it == m_itemmodBindings.end())
	{
		{
			gui::ScopedChild c{ "AddBinding", ImVec2(0, ImGui::GetContentRegionAvail().y * 0.1f) };

			if (ImGui::Button("Add binding", ImGui::GetContentRegionAvail()))
				addBinding();
		}
	}
	else
	{
		{
			gui::ScopedChild c{ "BindingInfo", ImVec2(0, ImGui::GetContentRegionAvail().y * 0.9f) };

			gui::spacing(4);

			ItemmodPresetBinding& binding = it->second;
			ImGui::Checkbox("Enabled", &binding.enabled);

			gui::spacing(6);

			ImGui::InputText("Itemmod code", &binding.code);

			gui::spacing(2);

			if (ImGui::Button("Use current itemmod code"))
			{
				auto itemmodCommand_cvar = cvarManager->getCvar(m_itemmodCommand);
				binding.code = itemmodCommand_cvar.getStringValue();
			}
		}

		gui::spacing(2);

		{
			gui::ScopedChild c{ "SaveOrDelete" };

			{
				gui::ScopedChild c{ "SaveButton", ImVec2(ImGui::GetContentRegionAvailWidth() * 0.75f, 0) };

				if (ImGui::Button("Save", ImGui::GetContentRegionAvail()))
				{
					gameWrapper->Execute([this](...)
					{
						writeBindingsToJson();
					});
				}
			}

			ImGui::SameLine();

			{
				gui::ScopedChild c{ "DeleteButton", ImGui::GetContentRegionAvail() };

				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));

				if (ImGui::Button("Delete Binding", ImGui::GetContentRegionAvail()))
				{
					gameWrapper->Execute([this](...)
					{
						deleteSelectedBinding();
						writeBindingsToJson();
					});
				}

				ImGui::PopStyleColor(3);
			}
		}
	}
}