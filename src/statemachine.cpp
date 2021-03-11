#include <Arduino.h>
#include <ArduinoJson.h>

#include <fsm.h>
#include <sensateiotclient.h>

#include "config.h"
#include "led.h"
#include "bme280.h"
#include "dsmr.h"
#include "statemachine.h"

static String latestFrame;
static StaticJsonDocument<256> json;
static fsm::FSM<smartenergy::DsmrSignal,
				smartenergy::DsmrState,
				smartenergy::DsmrSignal::NUM_SIGNALS,
				smartenergy::DsmrState::NUM_STATES> dsmrFsm;

namespace smartenergy
{
	static void readEnvironmentSensor();

	static void stateIdle()
	{
		if(readInterface()) {
			flashLed(RgbColor::BLUE);

			/* Get and store the latest DSMR sample */
			latestFrame = getCurrentFrame();
		}

#ifdef DEBUG
		dbgCounter++;

		if((dbgCounter % 1000) == 0) {
			DsmrStream = stringstream::StringStream(testData);
		}
#endif
	}

	static void sendMessage(const String& data)
	{
		sensateiot::Message message;
		auto conf = smartenergy::getConfiguration();

		message.secret = conf.sensor_secret;
		message.sensorId = conf.sensor_id;
		message.latitude = conf.latitude;
		message.longitude = conf.longitude;
		message.data = data;

		sensateiot::httpClient.send(message);
	}

	static void stateMeasure()
	{
		String envData;

		readEnvironmentSensor();
		serializeJson(json, envData);
		sendMessage(envData);

		if(latestFrame.length() > 0) {
			smartenergy::flashLed(RgbColor::GREEN);
			sendMessage(latestFrame);
			latestFrame.clear();
		}
	}

	static void readEnvironmentSensor()
	{
		auto result = smartenergy::readBme280();

		json["rh"] = result.rh;
		json["p"]  = result.pressure;
		json["t"] = result.temperature;
	}

	void readSignal()
	{
		dsmrFsm.setTimeout(millis(), DsmrSignal::IDLE);
	}

	void timerSignal()
	{
		dsmrFsm.setTimeout(millis(), DsmrSignal::CLOCK);
	}

	void loop()
	{
		dsmrFsm.loop();
	}

	void initFsm()
	{
		// Idle state
		dsmrFsm.addTransition(DsmrSignal::IDLE, DsmrState::S_IDLE, DsmrState::S_IDLE, stateIdle);
		dsmrFsm.addTransition(DsmrSignal::CLOCK, DsmrState::S_IDLE, DsmrState::S_MEASURE, stateMeasure);

		// Publish state
		dsmrFsm.addTransition(DsmrSignal::IDLE, DsmrState::S_MEASURE, DsmrState::S_IDLE, stateIdle);
		dsmrFsm.addTransition(DsmrSignal::CLOCK, DsmrState::S_MEASURE, DsmrState::S_MEASURE, stateMeasure);

#ifdef DEBUG
		DsmrStream = stringstream::StringStream(testData);
#else
		initDsmrInterface();
#endif
	}
} // namespace smartenergy
