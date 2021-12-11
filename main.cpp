#include <main.hpp>

#include <sdk.hpp>
#include <interface.hpp>
#include <portal2.hpp>

// -- module includes --
#include <console.hpp>
#include <command.hpp>
#include <tier1.hpp>
#include <server.hpp>
#include <vscript.hpp>
#include <client.hpp>

#include <iostream>
#include <fstream>

Plugin plugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(Plugin, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, plugin);

Plugin::Plugin() {
	Portal2* portal2 = new Portal2();
	(void)portal2; // Janky ass hack so i don't need to set -Wno-unused-variable
}

CON_COMMAND(get_username, "get_username (index) - Gets player username by index (starting at 0)\n") {
	console->Print("name: %s\n", server->GetPlayerName(atoi(args[1])));
}

CON_COMMAND(vscript_run, "run vscript with run\n") {
	char* pszScript = (char*)args.m_pArgSBuffer;
	pszScript += 11;
	while(*pszScript == ' ') pszScript++;
	vscript->Run(vscript->g_pScriptVM, pszScript, true);
}

CON_COMMAND(before_the_burn, "whatever\n") {
	console->Print("%x\n", vscript->g_pScriptVM);
}

CON_COMMAND(crash_and_burn, "self explanatory...\n") {
	abort();
}

bool Plugin::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory) {
	console = new Console();
	if(!console->Init()) return false;

	console->Print("(slowly) loading...\n");

	tier1 = new Tier1();
	if(!tier1->Init()) return false;

	server = new Server();
	if(!server->Init()) return false;

	vscript = new VScript();
	if(!vscript->Init()) return false;

	client = new Client();
	if(!client->Init()) return false;

	Command::RegisterAll();

	return true;
}

void Plugin::Unload() {
	console->Print("Gracefully returning the game to it's original state.\n");
	console->Shutdown();
	client->Shutdown();
	vscript->Shutdown();
	server->Shutdown();
	Command::UnregisterAll();
	tier1->Shutdown(); // Do this one last so that it doesn't try to unregister without tier1 loaded...
}

const char* Plugin::GetPluginDescription() {
	return "P2 32 player mod plugin (indev)";
}

void Plugin::LevelInit(char const* pMapName) {
	vscript->hasToResetVM = true;
}

// Unused callbacks
void Plugin::Pause() {}
void Plugin::UnPause() {}
void Plugin::ServerActivate(void* pEdictList, int edictCount, int clientMax) {}
void Plugin::GameFrame(bool simulating) {}
void Plugin::LevelShutdown() {}
void Plugin::ClientFullyConnect(void* pEdict) {}
void Plugin::ClientActive(void* pEntity) {}
void Plugin::ClientDisconnect(void* pEntity) {}
void Plugin::ClientPutInServer(void* pEntity, char const* playername) {}
void Plugin::SetCommandClient(int index) {}
void Plugin::ClientSettingsChanged(void* pEdict) {}
int Plugin::ClientConnect(bool* bAllowConnect, void* pEntity, const char* pszName, const char* pszAddress, char* reject, int maxrejectlen) { return 0; }
int Plugin::ClientCommand(void* pEntity, const void*& args) { return 0; }
int Plugin::NetworkIDValidated(const char* pszUserName, const char* pszNetworkID) { return 0; }
void Plugin::OnQueryCvarValueFinished(int iCookie, void* pPlayerEntity, int eStatus, const char* pCvarName, const char* pCvarValue) {}
void Plugin::OnEdictAllocated(void* edict) {}
void Plugin::OnEdictFreed(const void* edict) {}