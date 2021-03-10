/*
 * Wrapper around the Adafruit BME280 library.
 * 
 * @author Michel Megens
 * @email  michel@michelmegens.net
 */

#pragma once

namespace smartenergy
{
	struct BmeResult {
		float rh;
		float pressure;
		float temperature;
	};

	extern BmeResult readBme280();
	extern bool setupBme280();
}
