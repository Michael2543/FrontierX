#include "Include/NativeAppGlobals.h"
#include "Startup.h"

#include "DACOM.H"

#include <array>
#include <iostream>
#include <vector>

namespace
{
std::vector<int> calls;

struct StartupComponent final : GlobalComponent
{
	explicit StartupComponent(int value) : id(value) {}
	void Startup() override { calls.push_back(id); }
	void Initialize() override { calls.push_back(id + 100); }
	int id;
};

struct CleanupComponent final : IDAComponent
{
	explicit CleanupComponent(int value) : id(value) {}
	GENRESULT QueryInterface(const C8 *, void **instance) override
	{
		if (instance) *instance = nullptr;
		return GR_INTERFACE_UNSUPPORTED;
	}
	U32 AddRef() override { return 1; }
	U32 Release() override
	{
		calls.push_back(id + 200);
		return 0;
	}
	int id;
};
}

int main()
{
	// More than 26 registrations cross the original STARTUP_NODE boundary.
	std::array<StartupComponent *, 30> startup{};
	for (int i = 0; i < 30; ++i) startup[i] = new StartupComponent(i);
	CreateGlobalComponents();
	if (calls.size() != 60)
	{
		std::cerr << "App startup did not run both phases\n";
		return 1;
	}
	for (int i = 0; i < 30; ++i)
	{
		if (calls[i] != 29 - i || calls[30 + i] != 129 - i)
		{
			std::cerr << "App startup order changed\n";
			return 2;
		}
	}

	std::array<CleanupComponent *, 30> cleanup{};
	std::array<IDAComponent *, 30> slots{};
	for (int i = 0; i < 30; ++i)
	{
		cleanup[i] = new CleanupComponent(i);
		slots[i] = cleanup[i];
		AddToGlobalCleanupList(&slots[i]);
	}
	CleanupGlobals();
	if (calls.size() != 90)
	{
		std::cerr << "App cleanup did not release every component\n";
		return 3;
	}
	for (int i = 0; i < 30; ++i)
	{
		if (calls[60 + i] != 229 - i || slots[i])
		{
			std::cerr << "App cleanup order or pointer reset changed\n";
			return 4;
		}
	}
	for (auto *component : cleanup) delete component;
	for (auto *component : startup) delete component;
	std::cout << "30 App registrars: reverse Startup, Initialize and Release\n";
	return 0;
}
