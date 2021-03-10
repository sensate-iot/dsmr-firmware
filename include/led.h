#pragma once

namespace smartenergy
{
	enum RgbColor {
		RED = 0,
		GREEN,
		BLUE
	};

	enum RgbState {
		ON = 0,
		OFF
	};

	extern void flashLed(RgbColor color);
	extern void smartLedColor(RgbColor color, RgbState state);
}
