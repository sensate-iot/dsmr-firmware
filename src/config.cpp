#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include <string.h>
#include <ArduinoJson.h>

#include "config.h"

static smartenergy::Configuration app_config;

namespace smartenergy
{
	void initConfiguration()
	{
		if (!readAppConfig(&app_config))
		{
			memset(app_config.sensor_id, 0, SENSOR_ID_LENGTH);
			memset(app_config.sensor_secret, 0, SENSOR_SECRET_LENGTH);
			memset(app_config.api_key, 0, SENSOR_SECRET_LENGTH);
			app_config.p1_baudrate = 115200;
			app_config.latitude = 0.0f;
			app_config.longitude = 0.0f;
			writeAppConfig();
		}
	}

	Configuration &getConfiguration()
	{
		return app_config;
	}

	bool deleteAppConfig()
	{
		if (!LittleFS.begin())
		{
			return false;
		}

		if (LittleFS.exists("/config.json"))
		{
			return false;
		}

		return LittleFS.remove("/config.json");
	}

	bool readAppConfig(Configuration *app_config)
	{
		bool retval = false;

		if (!LittleFS.begin())
		{
			return false;
		}

		if (!LittleFS.exists("/config.json"))
		{
			return false;
		}

		File file = LittleFS.open("/config.json", "r");

		if (file)
		{
			size_t size = file.size();
			std::unique_ptr<char[]> buf(new char[size]);

			file.readBytes(buf.get(), size);

			DynamicJsonDocument json(1024);
			DeserializationError error = deserializeJson(json, buf.get());

			if (!error)
			{
				strcpy(app_config->sensor_id, json["SENSOR_ID"]);
				strcpy(app_config->sensor_secret, json["SENSOR_SECRET"]);
				strcpy(app_config->api_key, json["API_KEY"]);

				app_config->p1_baudrate = json["BAUDRATE"].as<int>();
				app_config->latitude = json["LATITUDE"].as<double>();
				app_config->longitude = json["LONGITUDE"].as<double>();

				retval = true;
			}
		}

		return retval;
	}

	bool writeAppConfig()
	{
		bool retval = false;

		if (LittleFS.begin())
		{

			// Delete config if exists
			if (LittleFS.exists("/config.json"))
			{
				LittleFS.remove("/config.json");
			}

			DynamicJsonDocument doc(1024);
			doc["SENSOR_ID"] = app_config.sensor_id;
			doc["SENSOR_SECRET"] = app_config.sensor_secret;
			doc["API_KEY"] = app_config.api_key;
			doc["LATITUDE"] = app_config.latitude;
			doc["LONGITUDE"] = app_config.longitude;
			doc["BAUDRATE"] = app_config.p1_baudrate;

			File file = LittleFS.open("/config.json", "w");
			if (file)
			{
				serializeJson(doc, Serial);
				serializeJson(doc, file);

				file.close();
				retval = true;
			}
		}

		return retval;
	}
}
