#include "MixMaster.h"

bool mixMaster::ZeroDrip() {
	static bool ZeroDripState;
	static uint32_t lastZeroDripTime = 0;

	if( isPrimed && 
		selector != 0 && 
		!PailSensor.getState() && 
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
	}

	return ZeroDripState;
}