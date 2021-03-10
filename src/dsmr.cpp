#include <Arduino.h>
#include <sensateiotclient.h>
#include <stringstream.h>

#include "config.h"
#include "dsmr.h"

//#define DEBUG 1

#ifdef DEBUG
#include "testdata.h"

#define DEBUG_PRINTF Serial.printf
#define DSMR_STREAM DsmrStream
stringstream::StringStream DsmrStream;
#else
#define DsmrStream Serial
#define DEBUG_PRINTF(...)
#endif

enum DsmrState
{
	P1_MSG_S0,
	P1_MSG_S1,
	P1_MSG_S2
};

static DsmrState p1_msg_state = P1_MSG_S0;
static String currentFrame;

#ifdef DEBUG
static int dbgCounter = 0;
#endif


namespace smartenergy
{
	String getCurrentFrame()
	{
		return currentFrame;
	}

	static void p1_reset()
	{
		currentFrame.clear();
	}

	static void p1_store(char ch)
	{
		currentFrame.concat(ch);
	}

	void initDsmrInterface()
	{
		auto &conf = getConfiguration();
		SerialConfig setting;

		switch (conf.p1_baudrate)
		{
		case 9600:
			setting = SERIAL_7E1;
			break;

		default:
			setting = SERIAL_8N1;
			break;
		}

		Serial.flush();
		Serial.begin(conf.p1_baudrate, setting);
		Serial.swap();
	}

	bool readInterface()
	{
		bool retval = false;

		if (!DsmrStream.available()) {
			return false;
		}

		while (DsmrStream.available()) {
			char ch = DsmrStream.read();
			switch (p1_msg_state) {

			case P1_MSG_S0:
				if (ch == '/') {
					p1_msg_state = P1_MSG_S1;
					p1_reset();
					p1_store(ch);
				}

				break;

			case P1_MSG_S1:
				p1_store(ch);

				if (ch == '!') {
					p1_msg_state = P1_MSG_S2;
				}
				break;

			case P1_MSG_S2:
				p1_store(ch);

				if (ch == '\n') {
					p1_store('\0'); // Add 0 terminator
					p1_msg_state = P1_MSG_S0;
					return true;
				}

				break;

			default:
				DEBUG_PRINTF("%s: Oeps, something bad happend\n\r", __FUNCTION__);
				retval = false;
				break;
			}
		}

		return retval;
	}
} // namespace smartenergy
