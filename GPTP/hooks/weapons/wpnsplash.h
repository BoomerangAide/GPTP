//The header file for the Weapon Splash hooks module.
#pragma once
#include <SCBW/structures/CUnit.h>
#include <SCBW/structures/CBullet.h>

namespace hooks {

Bool32 function_0048ADB0(CBullet* bullet, u32 splashRadius, CUnit* unit);	//8ADB0
Bool32 AirSplashProc(CBullet* bullet, CUnit* unit);							//8B150
Bool32 function_0048B570(CBullet* bullet, CUnit* unit);						//8B570
Bool32 LurkerSplashProc(CBullet* bullet, CUnit* unit);						//8B680
Bool32 AirSplashProc2(CBullet* bullet, CUnit* unit);						//8B730
Bool32 SplashProc(CBullet* bullet, CUnit* unit);							//8B750

//The injector function for this hook module.
void injectWpnSplashHooks();

} //hooks