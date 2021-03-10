#pragma once

#include <Arduino.h>

namespace smartenergy
{
	extern void initDsmrInterface();
	extern String getCurrentFrame();
	extern bool readInterface();
	extern void initDsmrInterface();
}
