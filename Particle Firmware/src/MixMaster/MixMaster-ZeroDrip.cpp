#include "MixMaster.h"

bool mixMaster::ZeroDrip(bool runZeroDrip) {
	static bool ZeroDripState;
	static uint32_t lastZeroDripTime = 0;

	if( isPrimed && 
		selector != 0 &&
		runZeroDrip &&
		millis() - lastZeroDripTime > settings.zeroDripInterval) {
	  
			if(!ZeroDripState){

				enablePumps();
				delay(5);

				ResinPump.setMaxSpeed(600);
				HardenerPump.setMaxSpeed(600);
				ResinPump.setCurrentPosition(0);
				HardenerPump.setCurrentPosition(0);
				ResinPump.moveTo(-settings.zeroDripSteps);
				HardenerPump.moveTo(-settings.zeroDripSteps);

				ZeroDripState = true;

			} else {
				ResinPump.run();
				HardenerPump.run();

				if(!ResinPump.isRunning() && !HardenerPump.isRunning()){
					disablePumps();
					ZeroDripState = false;
					lastZeroDripTime = millis();
				}
			}
	} else {
		ZeroDripState = false;
	}

	return ZeroDripState;
}