#include <Arduino.h>
#include "led.h"

#define RGB_R_PIN       D0  // Wemos D6 (GPIO12)
#define RGB_G_PIN       D4  // Wemos D1 (GPIO5)
#define RGB_B_PIN       D3  // Wemos D5 (GPIO14)

namespace smartenergy
{
	void flashLed(RgbColor color)
	{
		switch (color)
		{
		case RED:
			digitalWrite(RGB_R_PIN, ON);
			delay(50);
			digitalWrite(RGB_R_PIN, OFF);
			break;

		case GREEN:
			digitalWrite(RGB_G_PIN, ON);
			delay(50);
			digitalWrite(RGB_G_PIN, OFF);
			break;

		case BLUE:
			digitalWrite(RGB_B_PIN, ON);
			delay(50);
			digitalWrite(RGB_B_PIN, OFF);
			break;

		default:
			break;
		}
	}

	void smartLedColor(RgbColor color, RgbState state)
	{
		switch (color)
		{
		case RED:
			digitalWrite(RGB_R_PIN, state);
			digitalWrite(RGB_B_PIN, OFF);
			digitalWrite(RGB_G_PIN, OFF);
			break;

		case GREEN:
			digitalWrite(RGB_G_PIN, state);
			digitalWrite(RGB_B_PIN, OFF);
			digitalWrite(RGB_R_PIN, OFF);
			break;

		case BLUE:
			digitalWrite(RGB_B_PIN, state);
			digitalWrite(RGB_R_PIN, OFF);
			digitalWrite(RGB_G_PIN, OFF);
			break;
		default:
			break;
		}
	}
} // namespace smartenergy
