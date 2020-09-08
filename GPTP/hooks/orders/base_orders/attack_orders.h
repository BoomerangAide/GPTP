#pragma once
#include "../../../SCBW/structures/CUnit.h"

namespace hooks {

void orders_ReaverAttack1(CUnit* unit);		//65690
void orders_CarrierAttack1(CUnit* unit);	//65950
void orders_StrafeUnit2(CUnit* unit);		//65E00
void orders_StrafeUnit(CUnit* unit);		//65F60
void orders_Reaver(CUnit* unit);			//665D0
void orders_Carrier(CUnit* unit);			//666A0
void orders_TurretAttack(CUnit* unit);		//77980
void orders_AttackFixedRange(CUnit* unit);	//77D00
void orders_SapUnit(CUnit* unit);			//788E0
void orders_SapLocation(CUnit* unit);		//78A40
void orders_AttackMoveEP(CUnit* unit);		//78DE0
void orders_AttackMove(CUnit* unit);		//79040
void orders_TowerAttack(CUnit* unit);		//79150
void orders_AttackUnit(CUnit* unit);		//79BD0

void injectAttackOrdersHooks();

} //hooks
