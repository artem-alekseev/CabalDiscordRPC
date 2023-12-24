#include "function.h"
#include "discord_rpc.h"
#pragma comment(lib, "discord-rpc.lib")

#define DLLEXPORT __declspec(dllexport)
#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

time_t Elapsed_time = std::time(nullptr);

std::string locationPattern = "Location: ";
std::string characterPattern = "Character: ";
std::string nationPattern = "nation_";
std::string levelPattern = " - Lv. ";

BYTE mapId = 0;
BYTE partyFlag = 0;
BYTE isLoggedIn = 0;
char characterName[16] = "";
int partyCount = 0;

int class_addr = 0;
int nation_addr = 0;
int channel_addr = 0;
int level_addr = 0;

std::string location = "";
std::string character = "";

std::string class_name = "";
std::string char_class = "";
std::string nation_name = "";
std::string char_nation = "";

DWORD BaseAddress = 0xB93530;
DWORD PartyCountAddress = 0x01083248;
DWORD MapAddress = 0x00BA0760;
DWORD InGameOffset = 0x4810;
DWORD CharacterNameOffset = 0x3FB4;
DWORD CharacterStyle = 0x41A8;
DWORD NationOffset = 0x35C;
DWORD ChannelOffset = 0xC7B1A4;
DWORD LevelOffset = 0x4704;

void InitializeDiscord() {
	DiscordEventHandlers Handle;
	memset(&Handle, 0, sizeof(Handle));
	Discord_Initialize("1187488805515702362", &Handle, 1, NULL);
}

DWORD WINAPI DiscordInit() {
	InitializeDiscord();
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));

	while (true) {
		if (*(DWORD*)BaseAddress != 0 && *(DWORD*)(*(DWORD*)BaseAddress + InGameOffset) != 0)
		{
			mapId = *(DWORD*)MapAddress;
			partyCount = *(DWORD*)PartyCountAddress;
			channel_addr = *(DWORD*)(0x400000+ChannelOffset);
			level_addr = *(DWORD*)(*(DWORD*)(BaseAddress)+LevelOffset);
			nation_addr = *(DWORD*)(*(DWORD*)(BaseAddress)+NationOffset);
			nation_name = Nation::GetNation(nation_addr);
			class_addr = Decode_Class(*(DWORD*)(*(DWORD*)(BaseAddress)+CharacterStyle));
			class_name = ClassName::GetClassId(class_addr);

			memcpy(characterName, (DWORD*)(*(DWORD*)BaseAddress + CharacterNameOffset), 16);
			//character = characterPattern + characterName;
			character = characterName + levelPattern + std::to_string(level_addr);

			auto search = maps.find(mapId);
			if (search != maps.end())
				location = "CH: " + std::to_string(channel_addr) + " - " + search->second;
			else
				location = "Unknown Location";

			char_class = std::to_string(class_addr);

			if (class_addr != 0) {
				discordPresence.largeImageKey = char_class.c_str();
				discordPresence.largeImageText = class_name.c_str();
			}

			char_nation = nationPattern + std::to_string(nation_addr);

			if (nation_addr > 0 && nation_addr < 3) {
				discordPresence.smallImageKey = char_nation.c_str();
				discordPresence.smallImageText = nation_name.c_str();
			}

			//if (partyCount == 0)
			//	character.append(" Group State: Solo");
			//else
			//	character.append(" Group State: In party (" + std::to_string(partyCount + 1) + ")");
		}
		else 
		{
			character = "";
			location = "Currently not in game...";
			discordPresence.largeImageKey = "logo";
			discordPresence.largeImageText = " - ";
			discordPresence.smallImageKey = "logo";
			discordPresence.smallImageText = " - ";
		}

		discordPresence.details = character.c_str();
		discordPresence.state = location.c_str();
		discordPresence.startTimestamp = Elapsed_time;
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

