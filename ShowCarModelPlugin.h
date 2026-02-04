#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class ShowCarModelPlugin: public BakkesMod::Plugin::BakkesModPlugin
	,public SettingsWindowBase // Uncomment if you wanna render your own tab in the settings menu
{
	//Boilerplate
	void onLoad() override;

	std::map<int, std::string> carModelMap;
	std::map<std::string, std::string> inGamePlayerBodies;

	void loadCarModelMap();
	void loadInGamePlayerBodies();
	std::string inGamePlayerBodiesAsString();
	//void onUnload() override; // Uncomment and implement if you need a unload method
	//bool scmp_enabled;
public:
	void RenderSettings() override; // Uncomment if you wanna render your own tab in the settings menu
};
