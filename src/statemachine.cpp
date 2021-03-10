#include <Arduino.h>
#include <fsm.h>
#include <sensateiotclient.h>

#include "config.h"
#include "led.h"
#include "dsmr.h"
#include "statemachine.h"

static String latestFrame;
static fsm::FSM<smartenergy::DsmrSignal,
				smartenergy::DsmrState,
				smartenergy::DsmrSignal::NUM_SIGNALS,
				smartenergy::DsmrState::NUM_STATES> dsmrFsm;

namespace smartenergy
{
	static void state_idle()
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

	static void sendMessage()
	{
		sensateiot::Message message;
		auto conf = smartenergy::getConfiguration();

		message.secret = conf.sensor_secret;
		message.sensorId = conf.sensor_id;
		message.latitude = conf.latitude;
		message.longitude = conf.longitude;
		message.data = latestFrame;

		sensateiot::httpClient.send(message);
	}

	static void state_publish()
	{
		if(latestFrame.length() > 0) {
			smartenergy::flashLed(RgbColor::GREEN);
			sendMessage();
			latestFrame.clear();
		}
	}

	void readSignal()
	{
		dsmrFsm.setTimeout(millis(), DsmrSignal::IDLE);
	}

	void timerSignal()
	{
		dsmrFsm.setTimeout(millis(), DsmrSignal::CLOCK_PUBLISH);
	}

	void loop()
	{
		dsmrFsm.loop();
	}

	void initFsm()
	{
		// Idle state
		dsmrFsm.addTransition(DsmrSignal::IDLE, DsmrState::S_IDLE, DsmrState::S_IDLE, state_idle);
		dsmrFsm.addTransition(DsmrSignal::CLOCK_PUBLISH, DsmrState::S_IDLE, DsmrState::S_PUBLISH, state_publish);

		// Publish state
		dsmrFsm.addTransition(DsmrSignal::IDLE, DsmrState::S_PUBLISH, DsmrState::S_IDLE, state_idle);
		dsmrFsm.addTransition(DsmrSignal::CLOCK_PUBLISH, DsmrState::S_PUBLISH, DsmrState::S_PUBLISH, state_publish);

#ifdef DEBUG
		DsmrStream = stringstream::StringStream(testData);
#else
		initDsmrInterface();
#endif
	}
} // namespace smartenergy
