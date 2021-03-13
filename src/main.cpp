#define WM_MDNS

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <Wire.h>

#include <sensateiotclient.h>

#include "version.h"
#include "statemachine.h"
#include "config.h"
#include "configserver.h"
#include "led.h"
#include "dsmr.h"
#include "bme280.h"

#define RST_PIN         D5  // Wemos D2 (GPIO4)
#define RGB_R_PIN       D0  // Wemos D6 (GPIO12)
#define RGB_G_PIN       D4  // Wemos D1 (GPIO5)
#define RGB_B_PIN       D3  // Wemos D5 (GPIO14)
#define TIMER_PIN       D6  // WeMos D8

static volatile uint32_t ticks;

static WiFiManager wifiManager;

static ICACHE_RAM_ATTR void handleInterrupt();

static void fatal()
{
	while(true) {
		smartenergy::flashLed(smartenergy::RED);
		delay(250);
	}
}

static void setClock()
{
	configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

	Serial.print("Waiting for NTP time sync: ");
	time_t now = time(nullptr);
	while (now < 8 * 3600 * 2)
	{
		delay(500);
		Serial.print(".");
		now = time(nullptr);
	}
	Serial.println("");
	struct tm timeinfo;
	gmtime_r(&now, &timeinfo);
	Serial.print("Current time: ");
	Serial.print(asctime(&timeinfo));
}

static void initPins()
{
	pinMode(D2, INPUT_PULLUP);
	pinMode(D1, INPUT_PULLUP);

	pinMode(RST_PIN, INPUT_PULLUP);
	pinMode(TIMER_PIN, INPUT);
	pinMode(RGB_R_PIN, OUTPUT);
	pinMode(RGB_G_PIN, OUTPUT);
	pinMode(RGB_B_PIN, OUTPUT);

	digitalWrite(RGB_R_PIN, HIGH);
	digitalWrite(RGB_G_PIN, HIGH);
	digitalWrite(RGB_B_PIN, HIGH);
}

static void initConfiguration()
{
	smartenergy::factoryResetIf();
	smartenergy::initConfiguration();
}

static void initDataClient()
{
	auto conf = smartenergy::getConfiguration();
	sensateiot::httpClient.setApiKey(conf.api_key);

	if(!sensateiot::httpClient.begin()) {
		Serial.println("Unable to start Sensate IoT client!");
		fatal();
	}
}

static void initDns()
{
	if(!MDNS.begin("smartmeter")) {
		Serial.println("Unable to start mDNS service.");
	}
}

static void initBme280()
{
	if(!smartenergy::setupBme280()) {
		fatal();
	}
}

void setup()
{
	Serial.begin(115200);
	ticks = 0;
	initPins();
	smartenergy::smartLedColor(smartenergy::RgbColor::RED, smartenergy::RgbState::ON);
	Wire.begin();
	initBme280();
	initConfiguration();

	auto conf = smartenergy::getConfiguration();

	Serial.println();
	Serial.printf("Starting Sensate IoT DSMR Firmware %s\n", VERSION);
	Serial.println("Loaded configuration:");
	Serial.printf("Baud rate: %i\n", conf.p1_baudrate);
	Serial.printf("Sensor ID: %s\n", conf.sensor_id);

	smartenergy::startConfigServer();
	delay(1000);
	setClock();
	initDns();
	initDataClient();
	smartenergy::initFsm();

	attachInterrupt(digitalPinToInterrupt(TIMER_PIN), handleInterrupt, FALLING);
}

static volatile int counter = 1;
static volatile int lastUpdate = 1;

static ICACHE_RAM_ATTR void handleInterrupt()
{
	counter += 1;
}

void loop()
{
	MDNS.update();

	smartenergy::readSignal();

	if(lastUpdate != counter) {
		smartenergy::timerSignal();
		lastUpdate = counter;
	}

	smartenergy::loop();
}
