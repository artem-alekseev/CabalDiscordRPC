#include <Windows.h>
#include <chrono>
#include <thread>
#include <sstream>
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

std::string location = "";
std::string character = "";

const char* mapNames[] = { "Not in game world.","Bloody Ice", "Desert Scream", "Green Despair", "Port Lux", "Fort. Ruina","Lakeside",
"Undead Ground" ,"Forgotten Ruin","Mutant Forest" , "Pontus Ferrum","Porta Inferno","Arcane Trace" ,"Forest of Nostalgia",
"Art of War", "MAPHEL", "AUTRA","Senilinea", "OTAM","Jail","Chaos Arena","Tierra del Bruto","Tower of the Dead B3F","EMPTY SLOT",
"EMPTY SLOT", "Exilian Volcano","EMPTY SLOT", "Dungeon World 3", "Dungeon World 2", "Dungeon World 1", "Warp Center", "LBS", "Forgotten Temple B2F","Forbidden Island",
"Altar of Siena B1F","Altar of Siena B2F","Panic Cave (Easy)","Panic Cave (Normal)","Panic Cave (Hard)","Weakened Lake in Dusk","Weakened Ruina Station","Weakened Frozen Tower of Undead B1F",
"Steamer Crazy (Easy)","Steamer Crazy (Normal)","Steamer Crazy (Hard)","Illusion Castle Underworld","Catacomb Frost (Easy)","Catacomb Frost (Normal)","Catacomb Frost (Hard)" ,"Illusion Castle Radiant Hall","Abandoned City",
"Forbidden Island (Awakened)","Tower of the Dead B3F (Part2)","Legend Arena","Chaos Arena Lv.5","Chaos Arena Lv.6","Panic Cave (Premium)","Steamer Crazy (Premium)" ,"Catacomb Frost (Premium)","Glacies Inferna",
"Lava Hellfire (Easy)","Lava Hellfire (Normal)","Lava Hellfire (Hard)" ,"Lava Hellfire (Premium)","Bloody Ice","Unknown Maze","Fort. Ruina","Desert Scream","Maquinas Outpost","Warp Center",
"Green Despair","Port Lux","Forgotten Ruin","Lakeside","Hazardous Valley (Easy)","Hazardous Valley (Normal)","Hazardous Valley (Hard)","Chaos Infinity","Frozen Colosseum","Forgotten Temple B2F (Awakened)",
"Lava Hellfire (Awakened)","Panic Cave (Awakened)","Steamer Crazy (Awakened)","Catacomb Frost (Awakened)","Hazardous Valley (Awakened)","Dungeon World 4","Illusion Castle Underworld(Apocrypha)",
"Illusion Castle Radiant Hall(Apocrypha)","Edge of Phantom","Forgotten Temple B3F","Acheron Arena" ,"Devil's Tower" ,"Devil's Tower (Part2)","Pandemonium",
"Mirage Island","Chaos Arena Lv.7","Forest of Nostalgia","Flame Dimension","Flame Nest","Holiday Windhill","Labyrinth","Ancient Tomb","Dimensional clearance","Frozen Canyon",
"Holia Keldrasil","","","","EMPTY SLOT","","","Frozen Tower of Undead B1F","Ruina Station" ,"Forgotten Temple B1F","The Volcanic Citadel","Lake in Dusk"
};
const char* partyStatus[]{ " (Solo)", " (In party)" };

DWORD BaseAddress = 0xB93530;
DWORD InGameOffset = 0x4810;
DWORD CharacterNameOffset = 0x3FB4;

void InitializeDiscord() {
	DiscordEventHandlers Handle;
	memset(&Handle, 0, sizeof(Handle));
	Discord_Initialize("YOUR APPLICATION ID", &Handle, 1, NULL);
}

DWORD WINAPI DiscordInit() {
	InitializeDiscord();
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));

	while (true) {

		isLoggedIn = *(BYTE*)0x00107BE20;
		partyFlag = *(BYTE*)0x001081754;

		if (isLoggedIn)
			mapId = *(BYTE*)0x00BA0760;

		if (mapId > 0 && mapId < 116 && *(DWORD*)(*(DWORD*)BaseAddress + InGameOffset) != 0)
			location = locationPattern + mapNames[mapId];
		else
			location = locationPattern + " - ";

		if (*(DWORD*)BaseAddress != 0 && *(DWORD*)(*(DWORD*)BaseAddress + InGameOffset) != 0)
		{
			memcpy(characterName, (DWORD*)(*(DWORD*)BaseAddress + CharacterNameOffset), 16);

			character = characterPattern + characterName;

			if (partyFlag)
				character.append(partyStatus[1]);
			else
				character.append(partyStatus[0]);
		}
		else {
			character = characterPattern + " - ";
		}

		discordPresence.details = location.c_str();
		discordPresence.state = character.c_str();
		discordPresence.startTimestamp = Elapsed_time;
		discordPresence.largeImageKey = "IMAGE TAG";
		discordPresence.largeImageText = "IMAGE TEXT";
		//discordPresence.smallImageKey = "IMAGE TAG";
		//discordPresence.smallImageText = "IMAGE TEXT";
		discordPresence.instance = 1;
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