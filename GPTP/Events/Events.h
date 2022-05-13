#pragma once
#include "definitions.h"

#ifdef EVENTS_SYSTEM

/*
This is a non-Starcraft feature, an event system to make it easier
to centralize some modding.
By default, it is disabled by commenting out a line in definitions.h
to not interfere with mods not needing that much or working differently.
Events still need the corresponding hooks to be enabled.
*/

#include <map>
#include <list>
#include <vector>

namespace EventId {
enum Enum {
	/*Invalid value*/
	NO_EVENT = 0,
	/*hooks/interface/status_display/unit_stat_act.cpp 
	arg1: CUnit* unit, arg2: char** unitNamePtr
	*/
	STATUS_DISPLAYING_NAME,
	/*hooks/interface/status_display/advanced/status_base_text.cpp
	arg1: CUnit* unit, arg2: int* currentHP, arg3: int* maxHP, 
	arg4: int* currentShield, arg5: int* maxShieldPreMorph, arg6: int* maxShieldPostMorph
	Note:
	arg5 used to test if displaying values or not (no display if more than 4 digits), arg6 is value actually displayed if yes
	*/
	STATUS_DISPLAYING_HP_SHIELDS,
	/*hooks/interface/status_display/advanced/status_base_text.cpp
	arg1: CUnit* unit, arg2: char** buffer
	Note:
	Allow to replace the HP/Shields display text by a custom text, but
	it must use SC_sprintf_s to generate the replacement
	Just set *buffer to NULL for no text display.
	*/
	STATUS_DISPLAYING_HP_SHIELDS_OVERRIDE,
	/*hooks/interface/status_display/advanced/status_base_text.cpp
	arg1: CUnit* unit, arg2: char** buffer
	Note:
	Allow to replace the HP/Shields absence of display text by a
	custom text, but it must use SC_sprintf_s to generate the replacement
	Just set *buffer to NULL for no text display.
	*/
	STATUS_DISPLAYING_HP_SHIELDS_OVERRIDE_NOTEXT,
	/*hooks/interface/status_display/advanced/status_base_text.cpp
	arg1: CUnit* unit
	Note:
	Allow to cancel the normal energy/timer display and/or
	replace it by something else.
	*/
	STATUS_DISPLAYING_ENERGY_OR_TIMER_OVERRIDE,
	/*hooks/interface/status_display/advanced/status_base_text.cpp
	arg1: CUnit* unit, arg2: int* timerCurrent, arg3: int* timerMax
	Note:
	Allow to modify the values when trying to display a timer instead
	of energy (non-default).
	*/
	STATUS_DISPLAYING_ENERGY_OR_TIMER_TIMER,
	/*hooks/interface/status_display/advanced/status_base_text.cpp
	arg1: CUnit* unit, arg2: int* energyCurrent, arg3: int* energyMax
	Note:
	Allow to modify the values when trying to display energy rather
	than a timer (default).
	*/
	STATUS_DISPLAYING_ENERGY_OR_TIMER_ENERGY,
	/*hooks/interface/status_display/advanced/status_base_text.cpp
	arg1: CUnit* unit
	Note:
	Allow to cancel the normal status/ranking display and/or
	replace it by something else.
	*/
	STATUS_DISPLAYING_STATUS_OR_RANKING_OVERRIDE,
	/*hooks/interface/status_display/advanced/status_base_text.cpp
	arg1: CUnit* unit, arg2: char** textHallucination
	Note:
	Allow to replace the "Hallucination" text for hallucination.
	status.
	*/
	STATUS_DISPLAYING_STATUS_HALLUCINATION,
	/*hooks/interface/status_display/advanced/status_base_text.cpp
	arg1: CUnit* unit, arg2: char** textDisabled
	Note:
	Allow to replace the disabled text for disabled buildings.
	*/
	STATUS_DISPLAYING_STATUS_DISABLED,
	/*hooks/interface/status_display/advanced/status_base_text.cpp
	arg1: CUnit* unit, arg2: char** textAcidSpores, arg3: int* countAcidSpores
	Note:
	Allow to replace for the acid spore status the "acid spores" text 
	and the value displayed next to it.
	*/
	STATUS_DISPLAYING_STATUS_ACIDSPORES,
	/*hooks/interface/status_display/advanced/status_base_text.cpp
	arg1: CUnit* unit, arg2: char** textBlind
	Note:
	Allow to replace the "Blind" status text.
	*/
	STATUS_DISPLAYING_STATUS_BLIND,
	/*hooks/interface/status_display/advanced/status_base_text.cpp
	arg1: CUnit* unit, arg2: char** textParasite
	Note:
	Allow to replace the "Parasite Detected" status text.
	*/
	STATUS_DISPLAYING_STATUS_PARASITE,
	/*hooks/interface/status_display/advanced/status_base_text.cpp
	arg1: CUnit* unit, arg2: char** textDetector
	Note:
	Allow to replace the "Detector" status text.
	*/
	STATUS_DISPLAYING_STATUS_DETECTOR,
	/*hooks/interface/status_display/advanced/status_base_text.cpp
	arg1: CUnit* unit, arg2: char** textRank
	Note:
	Allow to replace the rank text. Otherwise used rank is the
	initial value of *textRank. If NULL, STATUS_DISPLAYING_STATUS_BLANK
	will be used instead.
	*/
	STATUS_DISPLAYING_STATUS_RANK,
	/*hooks/interface/status_display/advanced/status_base_text.cpp
	arg1: CUnit* unit, arg2: char** textRank
	Note:
	Allow to set a rank/status text when none of the above
	would have done it (including "rank" returning a NULL)
	*/
	STATUS_DISPLAYING_STATUS_BLANK,
	/*hooks/interface/status_display/advanced/status_base_text.cpp
	arg1: CUnit* unit, arg2: char** textKills, arg3: int* countKills
	Note:
	Allow to replace the "Kills:" text and the value displayed next to it.
	*/
	STATUS_DISPLAYING_KILLS,
	/*hooks/interface/status_display/advanced/status_base_text.cpp
	arg1: CUnit* unit
	Note:
	Allow to cancel the normal kills display and/or
	replace it by something else.
	This would also skip the logic where units without kills
	display hide the display instead of just displaying nothing.
	*/
	STATUS_DISPLAYING_KILLS_OVERRIDE,
	/*hooks/interface/status_display/stats_panel_display.cpp
	arg1: CUnit* unit, arg2: int* index
	Note:
	Allow to prevent,replace or force (by making the display
	while ignoring the armor type) the armor panel display.
	Remember index being 4 or more means all slots are already
	taken, and to increase index after taking a slot.
	*/
	STATUS_DISPLAYING_PANEL_ARMOR_OVERRIDE,
	/*hooks/interface/status_display/stats_panel_display.cpp
	arg1: CUnit* unit, arg2: int* index
	Note:
	Allow to prevent,replace or force (by making the display
	while ignoring if units got shields) the shield panel display.
	Remember index being 4 or more means all slots are already
	taken, and to increase index after taking a slot.
	*/
	STATUS_DISPLAYING_PANEL_SHIELD_OVERRIDE,
	/*hooks/interface/status_display/stats_panel_display.cpp
	arg1: CUnit* unit, arg2: int* index
	Note:
	Allow to prevent,replace or force the ground weapon panel display.
	Remember index being 4 or more means all slots are already
	taken, and to increase index after taking a slot.
	*/
	STATUS_DISPLAYING_PANEL_WEAPON_GROUND_OVERRIDE,
	/*hooks/interface/status_display/stats_panel_display.cpp
	arg1: CUnit* unit, arg2: int* index
	Note:
	Allow to prevent,replace or force the air weapon panel display.
	Remember index being 4 or more means all slots are already
	taken, and to increase index after taking a slot.
	*/
	STATUS_DISPLAYING_PANEL_WEAPON_AIR_OVERRIDE,
	/*hooks/interface/status_display/stats_panel_display.cpp
	arg1: CUnit* unit, arg2: int* index
	Note:
	Allow to prevent,replace or force the hangar display normally
	appearing for the protoss Reaver and Carrier types.
	Remember index being 4 or more means all slots are already
	taken, and to increase index after taking a slot.
	*/
	STATUS_DISPLAYING_PANEL_HANGAR_OVERRIDE,
	/*hooks/interface/status_display/stats_panel_display.cpp
	arg1: CUnit* unit, arg2: int* index
	Note:
	Allow to prevent,replace or force the spidermines display
	normally appearing for the terran Vulture types.
	Remember index being 4 or more means all slots are already
	taken, and to increase index after taking a slot.
	*/
	STATUS_DISPLAYING_PANEL_SPIDERMINES_OVERRIDE,
	/*hooks/interface/status_display/stats_panel_display.cpp
	arg1: CUnit* unit, arg2: int* index
	Note:
	Allow to prevent,replace or force the nukes display
	normally appearing for the terran Nuclear Silo.
	Remember index being 4 or more means all slots are already
	taken, and to increase index after taking a slot.
	*/
	STATUS_DISPLAYING_PANEL_NUKES_OVERRIDE,
	/*hooks/interface/status_display/stats_panel_display.cpp
	arg1: CUnit* unit, arg2: int* index
	Note:
	Event called if some panels slots are still available.
	Always happen with hallucinations.
	Remember to increase index after taking a slot, and that
	index becoming 4 means there is no more extra slot.
	*/
	STATUS_DISPLAYING_PANEL_EXTRAS,
	/*hooks/interface/status_display/advanced/status_nukesilo_resources.cpp
	hooks/interface/status_display/advanced/status_supply_provider.cpp
	arg1: CUnit* unit, arg2: int* index
	Note:
	Allow to prevent,replace or force (by making the display
	while ignoring if units got shields) the shield panel display.
	For supply and resource units, only 1 panel slot is avaiable,
	so index being 1 or more means all slots taken.
	*/
	STATUS_DISPLAYING_PANEL_SUPPLY_OR_RESOURCE_SHIELD_OVERRIDE,
	/*hooks/interface/status_display/advanced/status_nukesilo_resources.cpp
	hooks/interface/status_display/advanced/status_supply_provider.cpp
	arg1: CUnit* unit, arg2: int* index
	Note:
	Allow to prevent,replace or force (by making the display
	while ignoring if unit got an Overlord type id) the armor panel display.
	For supply and resource units, only 1 panel slot is avaiable,
	so index being 1 or more means all slots taken.
	*/
	STATUS_DISPLAYING_PANEL_SUPPLY_OR_RESOURCE_ARMOR_OVERRIDE,
	/*hooks/interface/status_display/panels/panels_generators.cpp
	hooks/interface/status_display/advanced/status_buildmorphtrain.cpp
	hooks/interface/status_display/advanced/status_nukesilo_resources.cpp
	hooks/interface/status_display/advanced/status_research_upgrade.cpp
	arg1: CUnit* unit, arg2: u16* panelGraphic, arg3: u16* iconId,
	arg4: u16* unknownPanelValue, arg5: u16* itemId, arg6: char** iconText
	Note:
	Change a panel display details, use all the arguments to
	try and figure the context.
	If iconText is NULL, it won't be used (since it is unused in
	many cases).
	*/
	STATUS_DISPLAYING_PANEL,
	/*hooks/interface/status_display/panels/panels_generators.cpp
	arg1: CUnit* unit, arg2: u16 iconId, arg3: int* count
	Note:
	Change a panel count (value displayed at bottom right) before
	it is used to generate a text through SC_sprintf_s.
	The count nature can be identified through the iconId.
	*/
	STATUS_DISPLAYING_PANEL_COUNT,
	/*hooks/interface/status_display/weapon_armor_tooltip.cpp
	arg1: CUnit* unit, arg2: u8 weaponId, arg3: char** weaponName,
	arg4: char** damageText, arg5: u16* baseDamage, arg6: u16* bonusDamage,
	arg7: char** perRocketText
	Note:
	Change the tooltip displayed for a weapon. Including the "Damage:"
	and "per rocket" texts of the Valkyrie.
	When bonusDamage is 0 (because no upgrade), it's not displayed.
	Remember the amounts are/should be affected by a damageFactor.
	*/
	STATUS_TOOLTIP_WEAPON,
	/*hooks/interface/status_display/weapon_armor_tooltip.cpp
	arg1: CUnit* unit, arg2: char** armorName, arg3: char** armorText, 
	arg4: u8* baseArmor, arg5: u8* bonusArmor
	Note:
	Change the tooltip displayed for an armor. Including the "Armor:"
	text before the value(s).
	When bonusArmor is 0 (because no upgrade), it's not displayed.
	*/
	STATUS_TOOLTIP_ARMOR,
	/*hooks/interface/status_display/weapon_armor_tooltip.cpp
	arg1: CUnit* unit, arg2: char** shieldName, arg3: char** shieldText, 
	arg4: u8* baseShield, arg5: u8* bonusShield
	Note:
	Change the tooltip displayed for a shield. Including the "Shield:"
	text before the value(s).
	When bonusShield is 0 (because no upgrade), it's not displayed.
	Default for baseShield is 0.
	*/
	STATUS_TOOLTIP_SHIELD,
	/*hooks/interface/status_display/weapon_armor_tooltip.cpp
	arg1: CUnit* unit, arg2: u16 iconUnitId, arg3: char** tooltipText
	Note:
	Allow to replace the normal tooltip generation for a special
	panel (nuke, interceptors, spider mines...) by setting a text,
	and cancelling other behaviors.
	*/
	STATUS_TOOLTIP_SPECIAL_OVERRIDE,
	/*hooks/interface/status_display/weapon_armor_tooltip.cpp
	arg1: CUnit* unit, arg2: u16 iconUnitId, arg3: char** tooltipText
	Note:
	Allow to check the result of a tooltip generation for a special
	panel (nuke, interceptors, spider mines...) and modify the text
	if needed.
	Default text is StringEmpty (from scbwdata) if nonstandard call.
	*/
	STATUS_TOOLTIP_SPECIAL,
	/*hooks/interface/status_display/unit_stat_act.cpp
	arg1: CUnit* unit, arg2: u16 elementIconId, arg3: char** progressText
	Note:
	Allow to set the text describing what kind of progress is made
	(usually "Morphing", "Building" or others).
	Can also be "Disabled" or "Unpowered" sometimes.
	*/
	STATUS_PROGRESS_TEXT,
	/*hooks/interface/status_display/advanced/status_buildmorphtrain.cpp
	arg1: CUnit* unit, arg2: u8 slotNumber, arg3: u16* panelGraphic,
	arg4: u16* iconId, arg5: u16* unknownPanelValue, arg6: u16* itemId,
	arg7: char** iconText
	Note:
	Change a queue panel display details, use all the arguments to
	try and figure the context.
	*/
	STATUS_DISPLAYING_QUEUE_PANEL,
	/*hooks/interface/status_display/panels/panels_generators.cpp
	arg1: CUnit* unit, arg2: u16 iconId, arg3: int* count
	Note:
	Change a queue panel count (value displayed at bottom right) before
	it is used to generate a text through SC_sprintf_s.
	Here it is the slot number.
	*/
	STATUS_DISPLAYING_QUEUE_PANEL_COUNT,
	/*hooks/interface/status_display/advanced/status_nukesilo_resources.cpp
	arg: CUnit* unit
	Note:
	Allow to completely cancel/replace a resource unit display of
	resource amount.
	*/
	STATUS_RESOURCES_TEXTS_OVERRIDE,
	/*hooks/interface/status_display/advanced/status_nukesilo_resources.cpp
	arg1: CUnit* unit, arg2: resourceName, arg3: resourceAmount
	Note:
	Allow to replace the text and amount for a resource current display.
	If resourceAmount is 0, resourceName will be the "Depleted" text instead.
	*/
	STATUS_RESOURCES_TEXT,
	/*hooks/interface/status_display/advanced/status_supply_provider.cpp
	arg1: CUnit* unit
	Note:
	Allow to completely cancel/replace a supply unit display of
	supplies.
	*/
	STATUS_SUPPLY_OVERRIDE,
	/*hooks/interface/status_display/advanced/status_supply_provider.cpp
	arg1: CUnit* unit, arg2: supplyText, arg3: supplyAmount
	Note:
	Allow to replace the text and amount for a supply provider current display
	of "supply used".
	*/
	STATUS_SUPPLY_USED_TEXT,
	/*hooks/interface/status_display/advanced/status_supply_provider.cpp
	arg1: CUnit* unit, arg2: supplyText, arg3: supplyAmount
	Note:
	Allow to replace the text and amount for a supply provider current display
	of "supply provided".
	*/
	STATUS_SUPPLY_PROVIDED_TEXT,
	/*hooks/interface/status_display/advanced/status_supply_provider.cpp
	arg1: CUnit* unit, arg2: supplyText, arg3: supplyAmount
	Note:
	Allow to replace the text and amount for a supply provider current display
	of "supply available".
	*/
	STATUS_SUPPLY_AVAILABLE_TEXT,
	/*hooks/interface/status_display/advanced/status_supply_provider.cpp
	arg1: CUnit* unit, arg2: supplyText, arg3: supplyAmount
	Note:
	Allow to replace the text and amount for a supply provider current display
	of "supply max".
	*/
	STATUS_SUPPLY_MAX_TEXT,
	/*hooks/interface/status_display/advanced/status_transport.cpp
	arg1 CUnit* unit, arg2: bool* unitSpaceProvidedLowerThan7
	Note:
	Display details change depending on capacity, this allow to
	ignore units_dat::SpaceProvided to evaluate capacity on
	another basis for that display choice.
	*/
	STATUS_TRANSPORT_CAPACITY_LOWER_THAN_7,
	/*hooks/interface/status_display/advanced/status_transport.cpp
	arg1 CUnit* unit, arg2: int* transport_space_for_drawing
	Note:
	Display details change depending on space occupied by loaded unit.
	This allow to make the drawing based on a space occupied of 1, 2
	or 4, the default value being in transport_space_for_drawing already.
	*/
	STATUS_TRANSPORT_CURRENT_SPACE_OCCUPIED_CHECK,
	/*hooks/interface/status_display/advanced/status_transport.cpp
	arg1: CUnit** unit, arg2: int transport_index, arg3: u16* dialog_graphic,
	arg4: char** pszText
	Note (EXPERIMENTAL):
	Change an unit dialog in a transport display details, use all the arguments to
	try and figure the context.
	Only "unit" and "dialog_graphic" are guaranteed to be compatible with normal code,
	though changing "unit" would be weird. "pszText" may have no effect, "transport_index"
	is read only.
	*/
	STATUS_TRANSPORT_LOADED_UNIT_DIALOG_DATA,
	/*hooks/interface/status_display/advanced/status_transport.cpp
	arg1: CUnit* transport, arg2: CUnit* loaded_unit, arg3: int* unitHP,
	Note:
	Change the cached amount of HP for display for a transport loaded unit.
	*/
	STATUS_TRANSPORT_LOADEDUNIT_HP_CACHE,
	/*hooks/interface/status_display/advanced/status_transport.cpp
	arg1: CUnit* transport, arg2: int* unitHP, arg3: unitEnergy, arg4: unitShields
	Note:
	Change the cached amount of HP,shields,energy for display for a transport.
	*/
	STATUS_TRANSPORT_STATS_CACHE,
	/*hooks/interface/status_display/unit_stat_act.cpp
	arg: CUnit* unit
	Note:
	Allow to cancel or replace the complete display of vitals
	(HP, Shield, Energy, Timer)
	*/
	STATUS_VITALS_OVERRIDE,
	/*hooks/interface/status_display/unit_stat_selection.cpp
	arg1: BinDlg* dialog
	Note:
	Allow to completely cancel/replace the display for a
	multi-units selection.
	*/
	STATUS_MULTISELECT_OVERRIDE,
	/*hooks/interface/status_display/unit_stat_selection.cpp
	arg1: CUnit** unit, arg2: int indexInSelection, arg3: u16* panelGraphic,
	arg4: u16* iconId, arg5: u16* unknownPanelValue, arg6: u16* itemId,
	arg7: char** iconText
	Note (EXPERIMENTAL):
	Change an unit in multi selection display details, use all the arguments to
	try and figure the context.
	Only "unit" and "iconId" are guaranteed to be compatible with normal code.
	*/
	STATUS_MULTISELECT_UNIT,
	/*hooks/interface/status_display/unit_stat_selection.cpp
	No arguments, use clientSelectionGroup
	Note:
	Allow to completely cancel/replace the check to decide to display
	the current selection.
	*/
	STATUS_SELECTION_UPDATE_COND_OVERRIDE,
	/*hooks/interface/status_display/unit_stat_selection.cpp
	arg1: CUnit* unit, arg2: bool* updateSelection
	Note:
	Allow to check specifics of a selected unit to decide if
	it requires the selection to update. If updateSelection
	remains false, does not override the behavior checking HP
	and Id of selection slot require an	update. 
	*/
	STATUS_SELECTION_UPDATE_COND_UNIT,
	/*hooks/interface/status_display/unit_stat_selection.cpp
	arg1: BinDlg* dialog, arg2: CUnit* unit, arg3 u16* dialogId
	Note:
	Allow to completely cancel/replace the wireframe update 
	processing part 1for an unit in a multi-units selection.
	WARNING: it is still only part 1 and should return a
	dialogId
	*/
	STATUS_MULTISELECT_UNIT_WIREFRAME_UPDATE_OVERRIDE,
	/*hooks/interface/status_display/unit_stat_selection.cpp
	arg1: CUnit* unit, arg2: bool* shieldsEnabled, arg3: u32* currentShield,
	arg4: u32* maxShield, arg5: u32* currentHP, arg6: u32* maxHP,
	arg7: u32* unitStatus, arg8: u32* unitProperties, arg9: u32* unitParasiteFlags,
	arg10: u32* unitWireframeRandomizer
	Note:
	Allow to modify the stats used to render an unit wireframe in
	a multi-selection.
	CUnit* is read-only, the rest are the values actually used
	by the inner processing and should be updated.
	*/
	STATUS_MULTISELECT_UNIT_WIREFRAME_UPDATE_UNITVALUES,
	/*hooks/interface/status_display/unit_stat_selection.cpp
	arg1: CUnit* unit, arg2: u16* dlgGraphic
	Note:
	Allow to modify the value of the dialog that will affect
	the background of the unit panel within selection, after it
	got set using values of STATUS_MULTISELECT_UNIT_WIREFRAME_UPDATE_UNITVALUES.
	*/
	STATUS_MULTISELECT_UNIT_WIREFRAME_BACKGROUND_UPDATE,
	/*hooks/interface/status_display/unit_stat_selection.cpp
	arg1: CUnit* unit, arg2: u16* dialogIconId
	Note:
	Allow to modify the value of the dialog that will affect
	the icon of the unit panel within selection, after it
	got set using values of STATUS_MULTISELECT_UNIT_WIREFRAME_UPDATE_UNITVALUES.
	*/
	STATUS_MULTISELECT_UNIT_WIREFRAME_ICON_UPDATE,
	/*hooks/interface/status_display/stats_display_main.cpp
	arg1: CUnit* unit, arg2: bool* canDisplay
	Note:
	Allow to completely cancel/replace the function that check
	the condition to display/refresh a single selected unit
	status.
	If not replacing, canDisplay will be overwritten.
	*/
	STATUS_SINGLE_SELECT_UPDATECOND_OVERRIDE,
	/*hooks/stim_packs.cpp
	arg1: bool* bFoundUnitAbleToStimpack
	Note:
	Allow to provide an alternative way to check if
	stimpack can be used (browsing clientSelectionGroup->unit[i]
	to check the active selection).
	If the boolean is set to true or the function return true,
	then normal checks will be skipped.
	*/
	STIMPACK_ACT_VALID_FOR_SELECTION_CHECK,
	/*hooks/stim_packs.cpp
	arg1: CUnit* unit
	Note:
	Allow to cancel/replace the check and action when the
	AI attempt to use stimpacks.
	*/
	STIMPACK_AI_USE_OVERRIDE,
	/*hooks/recv_commands/CMDRECV_Stimpack.cpp
	arg1: CUnit* unit, arg2: bool* bCanUseStims
	Note:
	Allow to cancel/replace the check for one unit when the
	player attempt to use stimpacks.
	If not replacing, then the mod conditions and normal
	conditions must both be fulfilled.
	The default value of bCanUseStims is true.
	*/
	STIMPACK_PLAYER_UNIT_CAN_USE,
	/*hooks/weapons/weapon_damage.cpp
	arg1: CUnit* target, arg2: s32 damage, arg3: CUnit* attacker,
	arg4: u32 attackingPlayerId
	Note:
	Allow to cancel/replace the generic hp damage inflicting function
	processing. Also used by stimpacks in CMDRECV_Stimpack.cpp and
	stim_packs.cpp.
	For weapon inflicted damage effected by shields, see UNIT_WEAPONDAMAGE.
	*/
	UNIT_DAMAGEHP_OVERRIDE,
	/*hooks/weapons/weapon_damage.cpp
	arg1: CUnit* target, arg2: s32* damage, arg3: CUnit* attacker,
	arg4: u32 attackingPlayerId, arg5: s32 initialDamage
	Note:
	Allow to modify the damage amount and/or do other things before kill
	check, also provide initial damage amount in case power overwhelming
	nullified it.
	*/
	UNIT_DAMAGEHP,
	/*hooks/weapons/weapon_damage.cpp
	arg1: CUnit* loaded_unit, arg2: s32 damage, arg3: CUnit* attacker,
	arg4: u32 attackingPlayerId, arg5: CUnit* transport
	Note:
	Allow to do things before an unit should be killed due to its transport
	being killed and cancel the death if overriden.
	Don't forget to set on the cancel parameter if the unit is somehow
	saved by the code executed, and to NOT set it, or set it to false, if
	the unit should still be killed.
	*/
	UNIT_KILLED_FROM_TRANSPORT_DEATH_OVERRIDE,
	/*hooks/weapons/weapon_damage.cpp
	arg1: CUnit* target, arg2: s32* damage, arg3: CUnit* attacker,
	arg4: u32 attackingPlayerId, arg5: weaponId, arg6: direction,
	arg7: dmgDivisor
	Note:
	Allow to cancel/replace the damage calculations on a weapon hit function
	processing.
	Damage can be overwritten if function is not cancelled, but if reduced
	to 0 or lower, it can still be increased later to affect shields.
	*/
	UNIT_WEAPONDAMAGE_OVERRIDE,
	/*hooks/weapons/weapon_damage.cpp
	arg1: CUnit* target, arg2: s32* damage, arg3: CUnit* attacker,
	arg4: u32 attackingPlayerId, arg5: s32 initialDamage, arg6:
	bool* bCreateOverlay
	Note:
	Allow to modify the damage inflicted to the shields of an unit.
	"damage" is the damage that would be applied after many
	calculations, "initialDamage" is the (read-only) damage that should have
	been applied to the unit (can have been modified by UNIT_WEAPONDAMAGE_OVERRIDE),
	and "bCreateOverlay" state if the "shield hit" overlay should be used (allow
	to use it even if the units shield were at 0 or damage type was "Independent").
	*/
	UNIT_DAMAGESHIELD,
	/*hooks/update_status_effects.cpp
	arg1: CUnit* target, arg2: s32* damage, arg3: bApplyDamage
	Note:
	When damage should be inflicted by Plague, allow to change the
	amount of damage or cancel the damage (cancellation normally happen
	when the damage would kill the unit).
	*/
	UNIT_PLAGUE_DAMAGE_PRECALCULATION,
	/*hooks/update_status_effects.cpp
	arg1: CUnit* target, arg2: s32* damage
	Note:
	When damage should be inflicted by Irradiation splash damage, 
	allow to change the amount of damage taken.
	*/
	UNIT_IRRADIATE_SPLASH_DAMAGE_PRECALCULATION,
	/*hooks/update_unit_state.cpp
	arg1: CUnit* target, arg2: s32* damage, arg3: bApplyDamage
	Note:
	When damage should be inflicted from burning to a terran low hp building, 
	allow to change the	amount of damage or cancel the damage (cancellation 
	normally happen when the unit HP isn't in the red).
	*/
	UNIT_BURNING_DAMAGE_PRECALCULATION,
	/*hooks/update_unit_state.cpp
	arg1: CUnit* target, arg2: s32* hp_planned_after_regen, arg3: bApplyRegen
	Note:
	Allow to apply/modify hp regeneration for an unit.
	"bApplyRegen" by default is set to the normal regen conditions
	(unit that can regen, have more than 0 hp but less than max HP).
	"hp_planned_after_regen" default is current HP + 4, thus may be changed
	later
	*/
	UNIT_HP_REGEN,
	/*hooks/orders/building_making/building_morph.cpp
	arg1: CUnit* morphingBuilding, arg2: s32 hpBeforeMorph, arg3: s32* hpToSet
	Note:
	Allow to change the HP a morphing building receive upon morph completion.
	"hpToSet" is initially newMaxHP - oldMaxHP + oldHP, or 256 if lower than 256.
	*/
	BUILDING_MORPH_HP_SET,
	/*hooks/orders/building_making/building_morph.cpp
	arg1: CUnit* morphingBuilding, arg2: s32 hpBeforeProgress, arg3: s32* hpToSet
	Note:
	Allow to change the HP a morphing building has after applying 
	morph progress hp gain.
	"hpToSet" is by default the unit HP + building->buildRepairHpGain 
	(that HpGain multiplied by 16 if OPERATION CWAL is in use).
	*/
	BUILDING_MORPH_HP_ADD,
	/*hooks/orders/infestation.cpp
	arg1: CUnit* infestedBuilding, arg2: CUnit* infestingUnit, arg3: u32 oldInfestedId
	arg4: s32 hpBeforeInfest, arg5: s32* hpToSet
	Note:
	Allow to change the HP an infested building receive upon the completion
	of infestation.
	"hpToSet" is initially units_dat::MaxHitPoints[infestedUnitNewId].
	*/
	BUILDING_INFESTED_HP_SET,
	/*hooks/orders/building_making/building_morph.cpp
	arg1: CUnit* cancelledBuilding, arg2: CUnit* worker, arg3: u32 oldId
	arg4: s32 hpBeforeCancel, arg5: s32* hpToSet
	Note:
	Allow to change the HP a drone appearing from a morph cancel get.
	"hpToSet" is initially cancelledBuilding->previousHp * 256.
	"cancelledBuilding" is either the "extractor" unit or identical to "worker".
	*/
	BUILDING_DRONE_SPAWNED_FROM_CANCELED_BUILDING_HP_SET,
	/*hooks/orders/building_making/make_nydus_exit.cpp
	arg1: CUnit* sourceNydus, arg2: CUnit* exitNydus, arg3: s32* hpToSet
	Note:
	Allow to change the HP a nydus canal exit receive on creation before
	starting morphing toward completion.
	"hpToSet" is initially 10 * maxHP /100, and won't go through a function
	to reduce it if higher than maxHP in the event scope.
	*/
	BUILDING_NYDUS_EXIT_INITIAL_HP_SET,
	/*hooks/orders/building_making/building_protoss.cpp
	arg1: CUnit* building, arg2: s32 hpBeforeProgress, arg3: s32* hpToSet,
	arg4: s32 shieldsBeforeProgress, arg5: s32* shieldsToSet
	Note:
	Allow to change the HP and Shields a protoss building has after applying 
	construction progress gains.
	"hpToSet" is by default the unit HP + building->buildRepairHpGain 
	(that HpGain multiplied by 16 if OPERATION CWAL is in use).
	"shieldsToSet" is by default unit Shield + building->shieldGain
	(that shieldGain multiplied by 16 if OPERATION CWAL is in use)
	or set to the unit max shield if above.
	*/
	BUILDING_PROTOSS_LIFE_ADD,
	/*hooks/orders/unit_making/unit_train.cpp
	arg1: CUnit* interceptor, arg2: CUnit* carrier, arg3: s32* hpToGain
	Note:
	Allow to change the HP gain applied to an interceptor/scarab during
	construction.
	"hpToGain" is by default unit->buildRepairHpGain 
	(that HpGain multiplied by 16 if OPERATION CWAL is in use).
	*/
	UNIT_INTERCEPTOR_CONSTRUCTION_HPGAIN,
	/*hooks/orders/unit_making/unit_train.cpp
	arg1: CUnit* interceptor, arg2: CUnit* carrier, arg3: s32 hpBeforeRepair,
	arg4: s32* hpToSet
	Note:
	Allow to change the HP an interceptor should receive after
	hp gain from repair.
	"hpToSet" is by default unit->hitPoints + 128
	*/
	UNIT_INTERCEPTOR_REPAIR_HP_SET,
	/*hooks/orders/repair_order.cpp
	arg1: CUnit* repairedBuilding, arg2: CUnit* worker, arg3: s32 hpBeforeProgress, arg4: s32* hpToSet
	Note:
	Allow to change the HP a repaired terran building has after applying 
	a repair hp gain.
	"hpToSet" is by default the unit HP + building->buildRepairHpGain 
	(that HpGain multiplied by 16 if OPERATION CWAL is in use).
	*/
	BUILDING_TERRAN_REPAIRED_HP_ADD,
	/*hooks/orders/building_making/building_terran.cpp
	arg1: CUnit* builtBuilding, arg2: CUnit* worker, arg3: s32* hpToGain
	Note:
	Allow to change the HP a built terran building get through an
	advance in the construction process.
	"hpToGain" is by default building->buildRepairHpGain 
	(that HpGain multiplied by 16 if OPERATION CWAL is in use).
	*/
	BUILDING_TERRAN_CONSTRUCTION_HP_ADD,
	/*hooks/orders/unit_making/unit_train.cpp
	arg1: CUnit* unit, arg2: CUnit* builder, arg3: s32* hpToGain
	Note:
	Allow to change the HP gain applied to an unit during
	training.
	"hpToGain" is by default unit->buildRepairHpGain 
	(that HpGain multiplied by 16 if OPERATION CWAL is in use).
	*/
	UNIT_TRAINED_HPGAIN,
	/*Special value for EventExists(eventId) function, to check if
	at least one event, any one, is registered
	*/
	ANY_EVENT = INT32_MAX
};
}

typedef bool (*func_pointer)(std::vector<int*>);

class EventManager {

	std::map<int, std::list<func_pointer>> registered_events;
	std::map<int, std::list<func_pointer>>::iterator registration_iterator;
	std::map<func_pointer, char*> named_events;

public:

	/// <summary>
	/// Register an event for the given hooked function.
	/// </summary>
	/// <param name="eventId">: Value from the EventId enumeration</param>
	/// <param name="function_to_call">: User-made function to call when the event happen.</param>
	static void RegisterEvent(int eventId, func_pointer function_to_call);

	/// <summary>
	/// Register an event for the given hooked function, and give the event a name.
	/// </summary>
	/// <param name="eventId">: Value from the EventId enumeration</param>
	/// <param name="function_to_call">: User-made function to call when the event happen.</param>
	/// <param name="event_name">: name for the event function</param>
	static void RegisterEventNamed(int eventId, func_pointer function_to_call, char* event_name);

	/// <summary>
	/// Unregister an event. Will also remove its name if it exist.
	/// </summary>
	/// <param name="eventId">: Value from the EventId enumeration</param>
	/// <param name="function_to_call">: User-made function called when the event happened.</param>
	static void UnregisterEvent(int eventId, func_pointer function_to_call);


	/// <summary>
	/// Check if any event is registered for this eventId.
	/// </summary>
	/// <param name="eventId">: Value from the EventId enumeration</param>
	/// <returns>true if at least 1 function would be called for that event.</returns>
	static bool EventExists(int eventId);

	/// <summary>
	/// Check if the specified function is set to be called among the event functions.
	/// </summary>
	/// <param name="eventId">: Value from the EventId enumeration</param>
	/// <param name="function_to_call">: User-made function possibily called when the event happen.</param>
	/// <returns>true if the function would be called, else false.</returns>
	static bool EventExists(int eventId, func_pointer function_to_call);

	/// <summary>
	/// Check if a function with the specified event name is set to be called among the event functions.
	/// Use is not recommended as it is much slower, but may have niche cases to fulfill.
	/// </summary>
	/// <param name="eventId">: Value from the EventId enumeration</param>
	/// <param name="event_name">: name of the event</param>
	/// <returns>true if the function would be called, else false.</returns>
	static bool EventExists(int eventId, char* event_name);

	/// <summary>
	/// Remove all functions call upon an event for the given eventId.
	/// Will also remove the function names registration accordingly.
	/// </summary>
	/// <param name="eventId">: Value from the EventId enumeration</param>
	static void ClearEvents(int eventId);


	/// <summary>
	/// Call all the events for the given function address.
	/// </summary>
	/// <param name="eventId">: Value from the EventId enumeration</param>
	/// <param name="parameters">Parameters needed for the event, converted to the int* type.</param>
	static void EventCalled(int eventId, std::vector<int*> parameters);

	/// <summary>
	/// Call all the events for the given function address, and set
	/// bCancelNormalBehavior to true if at least one function returned true.
	/// </summary>
	/// <param name="eventId">: Value from the EventId enumeration</param>
	/// <param name="bCancelNormalBehavior">If set to true by an event, the normal processing from the function
	/// that triggered the event will be skipped.</param>
	/// <param name="parameters">Parameters needed for the event, converted to the int* type.</param>
	/// <remarks>
	/// bCancelNormalBehavior does not stop events after one setting it to true got executed.
	/// </remarks>
	static void EventCalling(int eventId, bool* bCancelNormalBehavior, std::vector<int*> parameters);

};

#endif