#include <Windows.h>
#include <chrono>
#include <thread>
#include <map>
#include <sstream>

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

__declspec(noinline) int Decode_Class(int Style)	// Decoding Style into Class ID
{
	if ((Style & 0xF0000000) == 0x20000000)
		return 7;

	if ((Style & 0xF0000000) == 0x30000000)
		return 8;

	if ((Style & 0xF0000000) == 0x40000000)
		return 9;

	return Style & 7;
}

class ClassName {
public:
	static std::string GetClassId(int value) {
		switch (value) {
		case 1:
			return "Warrior";
		case 2:
			return "Blader";
		case 3:
			return "Wizard";
		case 4:
			return "Force Archer";
		case 5:
			return "Force Shielder";
		case 6:
			return "Force Blader";
		case 7:
			return "Gladiator";
		case 8:
			return "Force Gunner";
		case 9:
			return "Dark Mage";
		default:
			return "Unknown Class";
		}
	}
};

class Nation {
public:
	static std::string GetNation(int value) {
		switch (value) {
		case 0:
			return "Neutral";
		case 1:
			return "Capella";
		case 2:
			return "Procyon";
		case 3:
			return "Game Master";
		default:
			return "Unknown Nation";
		}
	}
};