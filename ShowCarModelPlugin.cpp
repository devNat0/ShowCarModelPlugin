#include "pch.h"
#include "ShowCarModelPlugin.h"


BAKKESMOD_PLUGIN(ShowCarModelPlugin, "show player's car in game", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void ShowCarModelPlugin::onLoad()
{
	_globalCvarManager = cvarManager;
	DEBUGLOG("ShowCarModelPlugin debug mode enabled");

	this->loadCarModelMap();

	cvarManager->registerNotifier("scmp_loadInGamePlayerBodies", [this](std::vector<std::string> args)
		{ loadInGamePlayerBodies(); }, "", PERMISSION_ALL);

	cvarManager->registerNotifier("scmp_print", [this](std::vector<std::string> args)
		{ inGamePlayerBodiesAsString(); }, "", PERMISSION_ALL);

	gameWrapper->HookEvent("Function TAGame.GameEvent_TA.AddCar", [this](std::string eventName) {
		DEBUGLOG("scmp: AddCar hook");
		loadInGamePlayerBodies();
		});

	//cvarManager->registerCvar("scmp_enabled", "0", "Enable 'Show Car Model Plugin'", true, true, 0, true, 1)
	//	.addOnValueChanged([this](std::string oldValue, CVarWrapper cvar) {
	//	scmp_enabled = cvar.getBoolValue();
	//		});
}

/**
* Loads all vehicle bodies from the game as int CarID : string carBodyName map
* 
* https://gist.github.com/devNat0/4c4858f067f0d91649f2db00eea0d0ca
*/
void ShowCarModelPlugin::loadCarModelMap()
{
	ItemsWrapper items = gameWrapper->GetItemsWrapper();
	if (!items) {
		LOG("scmp: GetItemsWrapper failed");
		return;
	}
	auto products = items.GetAllProducts();
	for (auto product : products) {
		if (!product) continue;
		// slot index 0 = body slot, skip non-body slots
		if (product.GetSlot().GetSlotIndex() != 0) continue;

		std::string label = product.GetLabel().ToString();
		std::string formatted;
		for (const char& c : label)
		{
			if (std::isalnum(c)) formatted += c;
			if (c == ' ') formatted += ' ';
		}
		this->carModelMap[product.GetID()] = formatted;
	}
	LOG("scmp: carModelMap loaded");
}

/**
* load map of string playerName : carBodyName
*/
void ShowCarModelPlugin::loadInGamePlayerBodies()
{
	if (!gameWrapper->IsInGame() && !gameWrapper->IsInOnlineGame())
	{
		LOG("Not in game");
		return;
	}
	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (!server)
	{
		LOG("Not in server");
		return;
	}
	inGamePlayerBodies.clear();
	ArrayWrapper cars = server.GetCars();
	for (CarWrapper car : cars)
	{
		std::string playername = car.GetOwnerName();
		int carID = car.GetLoadoutBody();
		if (this->carModelMap.count(carID)) {
			inGamePlayerBodies[playername] = this->carModelMap[carID];
		}
		else {
			inGamePlayerBodies[playername] = std::to_string(carID);
		}
	}
}

// For debugging
std::string ShowCarModelPlugin::inGamePlayerBodiesAsString()
{
	std::ostringstream oss;
	for (auto& body : this->inGamePlayerBodies) {
		oss << body.first << ":" << body.second << ", \n";
	}
	return oss.str();
}

void ShowCarModelPlugin::RenderSettings()
{
	if (!gameWrapper->IsInGame() && !gameWrapper->IsInOnlineGame()) {
		ImGui::TextUnformatted("Waiting for game to start.");
		return;
	}

	ImGui::Columns(2, "player_bodies_table", true);
	ImGui::Text("Player"); ImGui::NextColumn();
	ImGui::Text("Car"); ImGui::NextColumn();
	ImGui::Separator();
	LOG("{}", this->inGamePlayerBodies.size());
	for (auto& body : this->inGamePlayerBodies) {
		ImGui::Text("%s", body.first); ImGui::NextColumn();
		ImGui::Text("%s", body.second); ImGui::NextColumn();
	}
	ImGui::Columns(1);
	ImGui::Separator();
	if (ImGui::Button("Reload")) {
		gameWrapper->Execute([this](GameWrapper* gw) {
			cvarManager->executeCommand("scmp_loadInGamePlayerBodies");
			});
	}
}
