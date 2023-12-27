#include <Windows.h>
#include <chrono>
#include <thread>
#include <sstream>
#include <map>
#include "discord_rpc.h"
#pragma comment(lib, "discord-rpc.lib")

#define DLLEXPORT __declspec(dllexport)
#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

time_t Elapsed_time = std::time(nullptr);

std::string locationPattern = "Location: ";
std::string characterPattern = "Character: ";

BYTE mapId = 0;
BYTE partyFlag = 0;
BYTE isLoggedIn = 0;
char characterName[16] = "";
int partyCount = 0;

std::string location = "";
std::string character = "";

std::map<int, std::string> maps{
	{ 0, " - " },
	{ 1, "Bloody Ice" },
	{ 2, "Desert Scream" },
	{ 3, "Green Despair" },
	{ 4, "Port Lux" },
	{ 5, "Fort. Ruina" },
	{ 6, "Lakeside" },
	{ 7, "Undead Ground" },
	{ 8, "Forgotten Ruin" },
	{ 9, "Mutant Forest" },
	{ 10, "Pontus Ferrum" },
	{ 11, "Porta Inferno" },
	{ 12, "Arcane Trace" },
	{ 13, "Forest of Nostalgia" },
	{ 17, "Senilinea" },
};

DWORD BaseAddress = 0xB93530;
DWORD PartyCountAddress = 0x01083248;
DWORD MapAddress = 0x00BA0760;

DWORD InGameOffset = 0x4810;
DWORD CharacterNameOffset = 0x3FB4;

void InitializeDiscord() {
	DiscordEventHandlers Handle;
	memset(&Handle, 0, sizeof(Handle));
	Discord_Initialize("YOUR API KEY", &Handle, 1, NULL);
}

DWORD WINAPI DiscordInit() {
	InitializeDiscord();
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));

	discordPresence.startTimestamp = Elapsed_time;
	discordPresence.largeImageKey = "IMAGE TAG";
	discordPresence.largeImageText = "IMAGE TEXT";
	//discordPresence.smallImageKey = "IMAGE TAG";
	//discordPresence.smallImageText = "IMAGE TEXT";
	discordPresence.button1Label = "BTN ONE LABEL";
	discordPresence.button1Url = "https://example1.com";
	discordPresence.button2Label = "BTN TWO LABEL";
	discordPresence.button2Url = "https://example2.com";
	discordPresence.instance = 1;

	while (true) {
		if (*(DWORD*)BaseAddress != 0 && *(DWORD*)(*(DWORD*)BaseAddress + InGameOffset) != 0)
		{
			mapId = *(DWORD*)MapAddress;
			partyCount = *(DWORD*)PartyCountAddress;

			auto search = maps.find(mapId);

			if (search != maps.end())
				location = locationPattern + search->second;
			else
				location = locationPattern + " - ";
			
			memcpy(characterName, (DWORD*)(*(DWORD*)BaseAddress + CharacterNameOffset), 16);

			character = characterPattern + characterName;

			if (partyCount == 0)
				character.append(" Group State: Solo");
			else
				character.append(" Group State: In party (" + std::to_string(partyCount + 1) + ")");
			}
		else {
			location = locationPattern + " - ";
			character = characterPattern + " - ";
		}

		discordPresence.details = location.c_str();
		discordPresence.state = character.c_str();
		Discord_UpdatePresence(&discordPresence);

		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	}

	return 0;
}

void InitializeRPC() {
	auto hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DiscordInit, 0, 0, 0);
	SetThreadPriority(hThread, -1);
}

EXTERN_DLL_EXPORT BOOL APIENTRY DllMain(HMODULE hModule, DWORD  dwReason, LPVOID lpReserved)
{
	HMODULE hmd;
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
			InitializeRPC();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}