#include "bme280.h"

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

static Adafruit_BME280 bme;

namespace smartenergy
{
	bool setupBme280()
	{
		auto status = bme.begin(0x76);

		if (!status) {
			Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
			Serial.print("SensorID was: 0x");
			Serial.println(bme.sensorID(), 16);
			Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
			Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
			Serial.print("        ID of 0x60 represents a BME 280.\n");
			Serial.print("        ID of 0x61 represents a BME 680.\n");
			Serial.flush();
		}

		return status;
	}

	BmeResult readBme280()
	{
		BmeResult result;

		result.pressure = bme.readPressure();
		result.rh = bme.readHumidity();
		result.temperature = bme.readTemperature();

		return result;
	}
} // namespace smartenergy
