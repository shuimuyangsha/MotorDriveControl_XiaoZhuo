#include "mc_api.h"

int MC_StartMotor1(void)
{

	return MCI_StartMotor(pMCI[M1]);
}

int MC_StopMotor1(void)
{

	return MCI_StopMotor(pMCI[M1]);
}
