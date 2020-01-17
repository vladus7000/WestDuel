#pragma once

#include <Engine/Settings/Settings.hpp>

class WorldSettings : public Settings
{
public:
	WorldSettings() {}

	bool pause = false;
};