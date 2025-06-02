#pragma once


#define CVAR(name) "ipb_" name	// apply prefix

namespace Cvars
{
	// bools
	constexpr const char* enabled =					CVAR("enabled");
	constexpr const char* showIndexes =				CVAR("show_indexes");

	// colors
	constexpr const char* bindingColor =			CVAR("binding_color");
	constexpr const char* equippedPresetColor =		CVAR("equipped_preset_color");
}

#undef CVAR