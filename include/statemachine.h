#pragma once

namespace smartenergy
{
	enum DsmrSignal {
		CLOCK_PUBLISH,
		IDLE,
		NUM_SIGNALS
	};

	enum DsmrState {
		S_IDLE,
		S_PUBLISH,
		NUM_STATES
	};

	struct Transition {
		DsmrState state;
		void (*handler)();
	};

	extern void loop();
	extern void initFsm();
	extern void timerSignal();
	extern void readSignal();
}
