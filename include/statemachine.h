#pragma once

namespace smartenergy
{
	enum DsmrSignal {
		CLOCK,
		IDLE,
		NUM_SIGNALS
	};

	enum DsmrState {
		S_IDLE,
		S_MEASURE,
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
