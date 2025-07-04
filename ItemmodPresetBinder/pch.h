#pragma once

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include "bakkesmod/plugin/bakkesmodplugin.h"

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_set>
#include <map>
#include <fstream>

#include <BakkesmodPluginTemplate/IMGUI/imgui.h>
#include <BakkesmodPluginTemplate/IMGUI/imgui_internal.h>
#include <BakkesmodPluginTemplate/IMGUI/imgui_stdlib.h>
#include <BakkesmodPluginTemplate/IMGUI/imgui_searchablecombo.h>
#include <BakkesmodPluginTemplate/IMGUI/imgui_rangeslider.h>

#include "logging.h"

#include <json/single_include/nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;