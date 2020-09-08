#include "definitions.h"
#include "Plugin.h"
#include "hook_tools.h"

//Hook header files
#include "hooks/main/game_hooks.h"
#include "graphics/draw_hook.h"

#include "hooks/apply_upgrade_flags.h"
#include "hooks/attack_priority.h"
#include "hooks/bunker_hooks.h"
#include "hooks/cloak_nearby_units.h"
#include "hooks/cloak_tech.h"
#include "hooks/detector.h"
#include "hooks/harvest.h"
#include "hooks/rally_point.h"
#include "hooks/recharge_shields.h"
#include "hooks/stim_packs.h"
#include "hooks/tech_target_check.h"
#include "hooks/transfer_tech_upgrades.h"
#include "hooks/unit_speed.h"
#include "hooks/update_status_effects.h"
#include "hooks/update_unit_state.h"
#include "hooks/weapons/weapon_cooldown.h"
#include "hooks/weapons/weapon_damage.h"
#include "hooks/weapons/weapon_fire.h"

#include "hooks/unit_destructor_special.h"
#include "hooks/psi_field.h"

#include "hooks/unit_stats/armor_bonus.h"
#include "hooks/unit_stats/sight_range.h"
#include "hooks/unit_stats/max_energy.h"
#include "hooks/unit_stats/weapon_range.h"
#include "hooks/interface/status_display/weapon_armor_tooltip.h"

//in alphabetical order
#include "hooks/orders/base_orders/attack_orders.h"
#include "hooks/interface/btns_cond.h"
#include "hooks/orders/building_making/building_morph.h"
#include "hooks/interface/buttonsets.h"
#include "hooks/orders/building_making/building_protoss.h"
#include "hooks/orders/building_making/building_terran.h"
#include "hooks/recv_commands/burrow_tech.h"
#include "hooks/orders/spells/cast_order.h"
#include "hooks/recv_commands/CMDRECV_Build.h"
#include "hooks/recv_commands/CMDRECV_Cancel.h"
#include "hooks/recv_commands/CMDRECV_LiftOff.h"
#include "hooks/recv_commands/CMDRECV_MergeArchon.h"
#include "hooks/recv_commands/CMDRECV_Morph.h"
#include "hooks/recv_commands/CMDRECV_QueuedOrder.h"
#include "hooks/recv_commands/CMDRECV_ResearchUpgrade.h"
#include "hooks/recv_commands/CMDRECV_RightClick.h"
#include "hooks/recv_commands/CMDRECV_Selection.h"
#include "hooks/recv_commands/CMDRECV_SiegeTank.h"
#include "hooks/recv_commands/CMDRECV_Stimpack.h"
#include "hooks/recv_commands/CMDRECV_Stop.h"
#include "hooks/create_init_units.h"
#include "hooks/orders/base_orders/die_order.h"
#include "hooks/orders/enter_nydus.h"
#include "hooks/utils/ExtendSightLimit.h"
#include "hooks/orders/spells/feedback_spell.h"
#include "hooks/give_unit.h"
#include "hooks/orders/spells/hallucination_spell.h"
#include "hooks/orders/infestation.h"
#include "hooks/orders/larva_creep_spawn.h"
#include "hooks/orders/liftland.h"
#include "hooks/orders/load_unload_orders.h"
#include "hooks/load_unload_proc.h"
#include "hooks/orders/building_making/make_nydus_exit.h"
#include "hooks/orders/medic_orders.h"
#include "hooks/orders/merge_units.h"
#include "hooks/orders/spells/mindcontrol_spell.h"
#include "hooks/orders/base_orders/move_orders.h"
#include "hooks/orders/spells/nuke_orders.h"
#include "hooks/orders/0_orders/orders_root.h"
#include "hooks/orders/powerup.h"
#include "hooks/orders/spells/recall_spell.h"
#include "hooks/recv_commands/receive_command.h"
#include "hooks/orders/repair_order.h"
#include "hooks/utils/replace_unit.h"
#include "hooks/orders/research_upgrade_orders.h"
#include "hooks/orders/reset_collision.h"
#include "hooks/right_click_CMDACT.h"
#include "hooks/interface/select_larva.h"
#include "hooks/interface/selection.h"
#include "hooks/orders/shield_recharge_orders.h"
#include "hooks/orders/shrouded_order.h"
#include "hooks/orders/siege_transform.h"
#include "hooks/orders/spidermine.h"
#include "hooks/interface/status_display/stats_display_main.h"
#include "hooks/interface/status_display/stats_panel_display.h"
#include "hooks/interface/status_display/advanced/status_base_text.h"
#include "hooks/interface/status_display/advanced/status_buildmorphtrain.h"
#include "hooks/interface/status_display/advanced/status_nukesilo_resources.h"
#include "hooks/interface/status_display/advanced/status_research_upgrade.h"
#include "hooks/interface/status_display/advanced/status_supply_provider.h"
#include "hooks/interface/status_display/advanced/status_transport.h"
#include "hooks/orders/base_orders/stopholdpos_orders.h"
#include "hooks/recv_commands/train_cmd_receive.h"
#include "hooks/orders/unit_making/unit_morph.h"
#include "hooks/interface/status_display/unit_stat_act.h"
#include "hooks/interface/status_display/unit_stat_cond.h"
#include "hooks/interface/status_display/unit_stat_selection.h"
#include "hooks/orders/unit_making/unit_train.h"
#include "hooks/interface/updateSelectedUnitsData.h"
#include "hooks/utils/utils.h"
#include "hooks/interface/status_display/wireframe.h"
#include "hooks/weapons/wpnspellhit.h"

/// This function is called when the plugin is loaded into StarCraft.
/// You can enable/disable each group of hooks by commenting them.
/// You can also add custom modifications to StarCraft.exe by using:
///		memoryPatch(address_to_patch, value_to_patch_with);

BOOL WINAPI Plugin::InitializePlugin(IMPQDraftServer *lpMPQDraftServer) {

	//StarCraft.exe version check
	char exePath[300];
	const DWORD pathLen = GetModuleFileName(NULL, exePath, sizeof(exePath));

	if (pathLen == sizeof(exePath)) {
		MessageBox(NULL, "Error: Cannot check version of StarCraft.exe. The file path is too long.", NULL, MB_OK);
		return FALSE;
	}

	if (!checkStarCraftExeVersion(exePath))
		return FALSE;

	hooks::injectGameHooks();
	hooks::injectDrawHook();

	//in order of creation
	hooks::injectInfestationHooks();
	hooks::injectSiegeTransformHooks();
	hooks::injectButtonSetHooks();
	hooks::injectSelectMod();
	hooks::injectMergeUnitsHooks();
	hooks::injectLarvaCreepSpawnHooks();
	hooks::injectLiftLandHooks();
	hooks::injectAttackOrdersHooks();
	hooks::injectStopHoldPosOrdersHooks();
	hooks::injectRecallSpellHooks();
	hooks::injectEnterNydusHooks();
	hooks::injectCastOrderHooks();
	hooks::injectWpnSpellHitHooks();
	hooks::injectBuildingMorphHooks();
	hooks::injectMakeNydusExitHook();
	hooks::injectUnitMorphHooks();
	hooks::injectWireframeHook();
	hooks::injectDieOrderHook();
	hooks::injectBuildingTerranHook();
	hooks::injectBuildingProtossHooks();
	hooks::injectUnitTrainHooks();
	hooks::injectLoadUnloadProcHooks();
	hooks::injectLoadUnloadOrdersHooks();
	hooks::injectNukeOrdersHooks();
	hooks::injectBurrowTechHooks();
	hooks::injectResearchUpgradeOrdersHooks();
	hooks::injectMedicOrdersHooks();
	hooks::injectHallucinationSpellHook();
	hooks::injectFeedbackSpellHook();
	hooks::injectBtnsCondHook();
	hooks::injectRecvCmdHook();
	hooks::injectCreateInitUnitsHooks();
	hooks::injectGiveUnitHook();
	hooks::injectTrainCmdRecvHooks();
	hooks::injectCMDRECV_SiegeTankHooks();
	hooks::injectCMDRECV_MergeArchonHooks();
	hooks::injectCMDRECV_MorphHooks();
	hooks::injectCMDRECV_StopHooks();
	hooks::injectCMDRECV_CancelHooks();
	hooks::injectSelectLarvaHooks();
	hooks::injectRepairOrderHook();
	hooks::injectStatsPanelDisplayHook();
	hooks::injectUtilsHooks();
	hooks::injectMindControlSpellHook();
	hooks::injectCMDRECV_BuildHooks();
	hooks::injectExtendSightLimitMod();
	hooks::injectUpdateSelectedUnitsDataHook();
	hooks::injectStatsDisplayMainHook();
	hooks::injectUnitStatSelectionHooks();
	hooks::injectUnitStatCondHooks();
	hooks::injectUnitStatActHooks();
	hooks::injectStatusBaseTextHooks();
	hooks::injectStatusSupplyProviderHook();
	hooks::injectStatusResearchUpgradeHooks();
	hooks::injectStatusTransportHooks();
	hooks::injectStatusNukeSilo_Resources_Hooks();
	hooks::injectStatusBuildMorphTrain_Hooks();
	hooks::injectCMDRECV_SelectionHooks();
	hooks::injectCMDRECV_LiftOffHook();
	hooks::injectCMDRECV_ResearchUpgradeHooks();
	hooks::injectReplaceUnitWithTypeHook();
	hooks::injectCMDRECV_StimpackHook();
	hooks::injectCMDRECV_RightClickHooks();
	hooks::injectRightClickCMDACT_Hooks();
	hooks::injectCMDRECV_QueuedOrderHooks();
	hooks::injectResetCollisionHooks();
	hooks::injectOrdersRootHooks();
	hooks::injectShroudedOrderHook();
	hooks::injectMoveOrdersHooks();
	hooks::injectShieldRechargeOrdersHooks();
	hooks::injectSpiderMineHooks();
	hooks::injectPowerupOrderHooks();	

	hooks::injectApplyUpgradeFlags();
	hooks::injectAttackPriorityHooks();
	hooks::injectBunkerHooks();
	hooks::injectCloakNearbyUnits();
	hooks::injectCloakingTechHooks();
	hooks::injectDetectorHooks();
	hooks::injectHarvestResource();
	hooks::injectRallyHooks();
	hooks::injectRechargeShieldsHooks();
	hooks::injectStimPacksHooks();
	hooks::injectTechTargetCheckHooks();
	hooks::injectTransferTechAndUpgradesHooks();
	hooks::injectUnitSpeedHooks();
	hooks::injectUpdateStatusEffects();
	hooks::injectUpdateUnitState();
	hooks::injectWeaponCooldownHook();
	hooks::injectWeaponDamageHook();
	hooks::injectWeaponFireHooks();
	
	hooks::injectUnitDestructorSpecial();
	hooks::injectPsiFieldHooks();
	
	hooks::injectArmorBonusHook();
	hooks::injectSightRangeHook();
	hooks::injectUnitMaxEnergyHook();
	hooks::injectWeaponRangeHooks();
	
	hooks::injectUnitTooltipHook();

	//fixes to make sc1 campaign playable from firegraft/mpqgraft self-executables
	//jmpPatch((void*)0x150182D0, 0x004101AE);
	//jmpPatch((void*)0x15017DD0, 0x004100B2);
	//jmpPatch((void*)0x15017960, 0x004100C4);
	//jmpPatch((void*)0x15014A80, 0x004100BE);

	return TRUE;
}
