#include "mc_api.h"

int MC_StartMotor1(void)
{
	MCI_StartMotor(01);
	HAL_Delay(SPEED_SETTING_RESPONSE_WAITTIME);
	MCI_StartMotor(02);
	HAL_Delay(SPEED_SETTING_RESPONSE_WAITTIME);
	return 0;
}

int MC_StopMotor1(void)
{
	
	MCI_StopMotor(01);
	HAL_Delay(SPEED_SETTING_RESPONSE_WAITTIME);
	MCI_StopMotor(02);
	HAL_Delay(SPEED_SETTING_RESPONSE_WAITTIME);
	return 0;
}
