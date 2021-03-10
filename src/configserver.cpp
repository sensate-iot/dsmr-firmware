#define WM_MDNS

#include <Arduino.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

#include "config.h"
#include "led.h"

static constexpr int RST_PIN = D5;
static WiFiManager wifiManager;
static bool shouldSaveConfig;

static char latitude[LATITUDE_LENGTH];
static char longitude[LONGITUDE_LENGTH];
static char baud[P1_BAUDRATE_LENGTH];

namespace smartenergy
{
	static void enterApCallback(WiFiManager* instance)
	{
		MDNS.begin("smartmeter");
	}

	static void saveCfgCallback()
	{
		shouldSaveConfig = true;
	}

	static void indicateSuccess()
	{
		for(auto idx = 0; idx < 5; idx++) {
			flashLed(RgbColor::GREEN);
			delay(250);
		}

		smartLedColor(RgbColor::GREEN, ON);
		delay(2500);
		smartLedColor(RgbColor::GREEN, OFF);
	}

	void factoryResetIf()
	{
		shouldSaveConfig = false;

		if (digitalRead(RST_PIN) == HIGH)
		{
			return;
		}

		wifiManager.resetSettings();
		deleteAppConfig();

		/* Flash indicator LED */
		while (digitalRead(RST_PIN) == LOW)
		{
			flashLed(BLUE);
			delay(250);
		}

		ESP.reset();
	}

	void startConfigServer()
	{
		wifiManager.setHostname("smartmeter-config.local");
		wifiManager.setMinimumSignalQuality(20);
		wifiManager.setTimeout(300);
		wifiManager.setAPCallback(enterApCallback);
		wifiManager.setSaveConfigCallback(saveCfgCallback);

		auto& cfg = getConfiguration();

		WiFiManagerParameter wmp_text("<br/>Sensor settings:</br>");
		wifiManager.addParameter(&wmp_text);
		WiFiManagerParameter custom_id("sensor_id", "Sensor ID", cfg.sensor_id, SENSOR_ID_LENGTH);
		WiFiManagerParameter custom_secret("sensor_secret", "Sensor secret", cfg.sensor_secret, SENSOR_SECRET_LENGTH);
		WiFiManagerParameter custom_apikey("api_key", "API key", cfg.api_key, SENSOR_SECRET_LENGTH);
		WiFiManagerParameter custom_p1_baudrate("p1_baudrate", "Baudrate", baud, P1_BAUDRATE_LENGTH);
		WiFiManagerParameter custom_latitude("latitude", "Latitude", latitude, LATITUDE_LENGTH);
		WiFiManagerParameter custom_longitude("longitude", "Longitude", longitude, LONGITUDE_LENGTH);

		wifiManager.addParameter(&custom_p1_baudrate);
		wifiManager.addParameter(&custom_id);
		wifiManager.addParameter(&custom_secret);
		wifiManager.addParameter(&custom_apikey);
		wifiManager.addParameter(&custom_longitude);
		wifiManager.addParameter(&custom_latitude);

		smartLedColor(RgbColor::BLUE, RgbState::ON);

		if(!wifiManager.autoConnect("Smart Meter config")) {
			delay(1000);
			ESP.reset();
		}

		if(shouldSaveConfig) {
			String lat, lon, baudrate;

			lat = custom_latitude.getValue();
			lon = custom_longitude.getValue();
			baudrate = custom_p1_baudrate.getValue();

			strcpy(cfg.sensor_id, custom_id.getValue());
			strcpy(cfg.sensor_secret, custom_secret.getValue());
			strcpy(cfg.api_key, custom_apikey.getValue());

			cfg.longitude = lon.toDouble();
			cfg.latitude  = lat.toDouble();
			cfg.p1_baudrate = baudrate.toInt();

			writeAppConfig();
		}

		smartLedColor(RgbColor::BLUE, RgbState::OFF);
		indicateSuccess();
	}
}
