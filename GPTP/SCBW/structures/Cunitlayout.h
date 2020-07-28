//This header file contains the binary layout of the CUnit structure.
//For the member functions, see CUnit.h
//Based on BWAPI's BW/CUnit.h

#include "Target.h"
#include "CList.h"

#pragma pack(1)

struct CImage;
struct CSprite;
struct COrder;
struct CUnit;

struct StoredUnit {
	union {
		struct {
			u16 index : 11;             // 1-based index in the unit table.
			u16 unitId : 5;
		}innerValues;
		u16 fullValue;
	};
};
C_ASSERT(sizeof(StoredUnit) == 2);

//From BWAPI/Source/BW/MovementFlags.h
/** Definition of Flags specifying movement type - used in BW#Unit#movementFlags */
namespace MovementFlags
{
enum Enum
{

	OrderedAtLeastOnce = 0x01, /**< @todo investigate more.
	* It get's to 0 after the first move-involved command - sometimes get's to 1 again
	*/
	Accelerating = 0x02, /**< Unit is accelerating */
	Braking = 0x04, /**< Not when unit reaches destination, just when you hit stop, or building is finished */
	StartingAttack = 0x08, /** Always 1 for a moment when the unit is starting new attack cycle*/
	Moving = 0x10, /**< Still moving, but not accelerating */
	Lifted = 0x20, /**< Lifted cc has this on (just before it moves) */
	unknown1 = 0x40, /**< The value is unknown, but it has something to do with changing direction& accelerating */
	_alwaysZero1 = 0x80,

	HoverUnit = 0xC1 // OrderedAtLeastOnce | unknown1 | _alwaysZero1

};
}

//Layout of the CUnit structure
//Comments marked with ++ added by RavenWolf
struct CUnitLayout {
/*0x000*/ CLink<CUnit> link;
/*0x008*/ s32       hitPoints;			  /// Hit points of unit, note that the displayed value in broodwar is ceil(healthPoints/256)
										  /// Officially called "xLife"
/*0x00C*/ CSprite   *sprite;
/*0x010*/ Target    moveTarget;           /// The position or unit to move to. It is NOT an order target.
/*0x018*/ Point16   nextMovementWaypoint; /// The next way point in the path the unit is following to get to its destination.
                                          /// Equal to moveToPos for air units since they don't need to navigate around buildings.
/*0x01C*/ Point16   nextTargetWaypoint;   /// The desired position
/*0x020*/ u8        movementFlags;		  /// Flags specifying movement type, MovementFlags::Enum contains possible explanations
/*0x021*/ u8        currentDirection1;	  /// The current direction the unit is facing
/*0x022*/ u8        flingyTurnSpeed;      //AKA flingy turn radius
/*0x023*/ u8        velocityDirection1;	  /**< This usually only differs from the currentDirection field for units that can accelerate
											* and travel in a different direction than they are facing. For example Mutalisks can change
											* the direction they are facing faster than then can change the direction they are moving.
											*/	
/*0x024*/ u16       flingyId;
/*0x026*/ u8        _unknown_0x026;
/*0x027*/ u8        flingyMovementType;
/*0x028*/ Point16   position;			  // Current position of the unit
/*0x02C*/ Point32   halt;
/*0x034*/ u32       flingyTopSpeed;       //top speed
/*0x038*/ u32       current_speed1;       //++current speed
/*0x03C*/ u32       current_speed2;       //++not sure if there are any differences with these 2 variables, they always have the same value
/*0x040*/ Point32   current_velocity;
/*0x048*/ u16       flingyAcceleration;
/*0x04A*/ u8        currentDirection2;    //?
/*0x04B*/ u8        velocityDirection2;   //pathing related, BWAPI know more about this

/*0x04C*/ u8        playerId;			  //Id of player controlling the unit
/*0x04D*/ u8        mainOrderId;
/*0x04E*/ u8        mainOrderState;       //Used by various orders
										  //< Additional order info (mostly unknown, wander property investigated so far) // officially "ubActionState"
										  /* 0x01 Moving/Following Order
										  0x02 No collide (Larva)?
										  0x04 Harvesting? Working?
										  0x08 Constructing Stationary
										  Note: BWAPI code writer don't actually think these are flags
										  */
/*0x04F*/ u8        orderSignal;          /*  0x01  Update building graphic/state
                                              0x02  Casting spell
                                              0x04  Reset collision? Always enabled for hallucination...
                                              0x10  Lift/Land state
                                          */
/*0x050*/ u16       orderUnitType;
/*0x052*/ u16       _unknown_0x052;       // 2-byte padding

/*0x054*/ u8        mainOrderTimer;		  // A timer for orders, example: time left before minerals are harvested
/*0x055*/ u8        groundWeaponCooldown;
/*0x056*/ u8        airWeaponCooldown;
/*0x057*/ u8        spellCooldown;
/*0x058*/ Target    orderTarget;		  // officially called ActionFocus

/*0x060*/ s32       shields;              // Although BWAPI says this is unsigned, StarCraft.exe uses signed comparisons for this value
										  // BW shows this value/256

/*0x064*/ u16       id;                   // Unit ID in units.dat
/*0x066*/ u16       _unknown_0x066;       // 2-byte padding
/*0x068*/ CLink<CUnit> player_link;
/*0x070*/ CUnit*    subunit;
/*0x074*/ COrder*   orderQueueHead;
/*0x078*/ COrder*   orderQueueTail;
/*0x07C*/ CUnit*    autoTargetUnit;       //The auto-acquired target (Note: This field is actually used for different targets internally, especially by the AI)
/*0x080*/ CUnit*    connectedUnit;        // ++Larva store a reference to the hatchery/Lair/Hive who created it
                                          // ++units inside transports store a reference to the transport
                                          // addons store a reference to its parent building (but not the other way around)
/*0x084*/ u8        orderQueueCount;	  // (may be count in addition to first since can be 2 when 3 orders are queued)
/*0x085*/ u8        orderQueueTimer;      // Cycles down from from 8 to 0 (inclusive). See also 0x122.
/*0x086*/ u8        _unknown_0x086;
/*0x087*/ u8        attackNotifyTimer;    // Prevent "Your forces are under attack." on every attack
/*0x088*/ u16       previousUnitType;      //++used by zerg buildings while morphing
/*0x08A*/ u8        lastEventTimer;       // countdown that stops being recent when it hits 0
/*0x08B*/ u8        lastEventColor;       // 17 = was completed (train, morph), 174 = was attacked
/*0x08C*/ u16       _unknown_0x08C;       // might have originally been RGB from lastEventColor
/*0x08E*/ u8        rankIncrease;		  // Adds this value to the unit's base rank
/*0x08F*/ u8        killCount;
/*0x090*/ u8        lastAttackingPlayer;  // the player that last attacked this unit
/*0x091*/ u8        secondaryOrderTimer;
/*0x092*/ u8        aiActionFlags;        // Used internally by the AI
/*0x093*/ u8        userActionFlags;      // some flags that change when the user interacts with the unit
                                          // 2 = issued an order, 3 = interrupted an order, 4 = hide self before death (self-destruct?)
/*0x094*/ u16       currentButtonSet;     // The u16 is a guess, used to be u8
/*0x096*/ bool      isCloaked;
/*0x097*/ u8        movementState;        // A value based on conditions related to pathing, see BWAPI Path.h or UnitMovement.cs for info
/*0x098*/ u16       buildQueue[5];        //++train queue for buildings and carrier/reaver (stores the unitid) / zerg eggs stores the id of the morphing unit in queue[0]
										  //Note that it doesn't begin with index 0, but with #buildQueueSlot index.
/*0x0A2*/ u16       energy;				  //< Energy Points // officially "xwMagic"
/*0x0A4*/ u8        buildQueueSlot;       //< Index of active unit in #buildQueue.
/*0x0A5*/ u8        targetOrderSpecial;   //< A byte used to determine the target ID for the unit (found in units in transports)
/*0x0A6*/ u8        secondaryOrderId;	  //< (Build addon verified) @todo verify (Cloak, Build, ExpandCreep suggested by EUDDB)
/*0x0A7*/ u8        buildingOverlayState; // 0 means the building has the largest amount of fire/blood
/*0x0A8*/ u16       buildRepairHpGain;    //++hp amount increased per frame when a unit/building its constructed/repaired
/*0x0AA*/ u16       shieldGain;			  //< Shield gain on construction (from BWAPI, was unknown)
/*0x0AC*/ u16       remainingBuildTime;   //Remaining bulding time; also used by powerups (flags) as the timer for returning to their original location.
/*0x0AE*/ u16       previousHp;           // The HP of the unit before it changed (example Drone->Hatchery, the Drone's HP will be stored here)
/*0x0B0*/ struct StoredUnit loadedUnit[8];		  // officially called "uwTransport[8]"
/*0x0C0*/ union {
            struct {
/*0x0C0*/     u8 spiderMineCount;
            } vulture;

            struct {
/*0x0C0*/     CUnit*  inHangarChild;      // First child inside the hangar
/*0x0C4*/     CUnit*  outHangarChild;     // First child outside the hangar
/*0x0C8*/     u8      inHangarCount;      // Number inside the hangar
/*0x0C9*/     u8      outHangarCount;     // Number outside the hangar
            } carrier; // also applies to reaver

            struct {
/*0x0C0*/     CUnit*  parent;
/*0x0C4*/     CLink<CUnit> hangar_link;
/*0x0CC*/     bool    isOutsideHangar;
            } interceptor;  // also applies to scarab

            struct {
/*0x0C0*/     u32 _unknown_00;
/*0x0C4*/     u32 _unknown_04;
/*0x0C8*/     u32 flagSpawnFrame; // flag beacons, the frame that the flag will spawn
            } beacon;

            struct {
/*0x0C0*/     CUnit*  addon;
/*0x0C4*/     u16     addonBuildType;
/*0x0C6*/     u16     upgradeResearchTime;
/*0x0C8*/     u8      techType;
/*0x0C9*/     u8      upgradeType;
/*0x0CA*/     u8      larvaTimer;
/*0x0CB*/     u8      landingTimer;
/*0x0CC*/     u8      creepTimer;
/*0x0CD*/     u8      upgradeLevel;
/*0x0CE*/     u16     _padding_0E;
/*0x0D0*/     union {
                struct {
/*0x0D0*/         u16       resourceAmount;
/*0x0D2*/         u8        resourceIscript;
/*0x0D3*/         u8        gatherQueueCount;
/*0x0D4*/         CUnit*    nextGatherer;   // pointer to the next worker unit waiting in line to gather
/*0x0D8*/         u8        resourceGroup;  
/*0x0D9*/         u8        resourceBelongsToAI;
                } resource;  /** When the unit is resource container */
/*0x0D0*/       CUnit*    nydusExit; /** connected nydus canal */
/*0x0D0*/       CUnit*    ghostNukeMissile; //confirmed to be CUnit* and not CSprite*
/*0x0D0*/       CSprite*  pylonAura;
                struct {
/*0x0D0*/         CUnit*  nuke;  // attached nuke
/*0x0D4*/         bool    isReady;
                } silo;
/*0x0D0*/       Box16     hatcheryHarvestValue; // wtf???
/*0x0D0*/       Point16   powerupOrigin;
              };
            } building;

            struct {
/*0x0C0*/     CUnit*  powerup;
/*0x0C4*/     Target  targetResource;
/*0x0CC*/     u16     repairResourceLossTimer;
/*0x0CE*/     bool    isCarryingSomething;
/*0x0CF*/     u8      resourceCarryAmount;
/*0x0D0*/     CUnit*  harvestTarget;
/*0x0D4*/     CLink<CUnit> harvest_link;  // When there is a gather conflict
            } worker;
          };

/*0x0DC*/ u32       status;             // See UnitStatus::Enum for more info
/*0x0E0*/ u8        resourceType;       // Type of resource chunk carried by this worker. 1 = vespene gas, 2 = mineral.
/*0x0E1*/ u8        wireframeRandomizer;
/*0x0E2*/ u8        secondaryOrderState;
/*0x0E3*/ u8        recentOrderTimer;   // Counts down from 15 to 0 when most orders are given, or when the unit moves after reaching a patrol location
/*0x0E4*/ s32       visibilityStatus;   // Flags specifying which players can detect this unit (cloaked/burrowed)
/*0x0E8*/ Point16   secondaryOrderPos;	// Unused according to BWAPI
/*0x0EC*/ CUnit*    currentBuildUnit;	// tied to secondary order according to BWAPI
/*0x0F0*/ CLink<CUnit> burrow_link;
/*0x0F8*/ union {
            Target rally;
            CLink<CUnit> psi_link;
          };
/*0x100*/ void*     path;						//use BW::Path* in BWAPI
/*0x104*/ u8        pathingCollisionInterval;  // unknown
/*0x105*/ u8        pathingFlags;       // 0x01 = uses pathing; 0x02 = ?; 0x04 = ?
/*0x106*/ u8        _unused_0x106;
/*0x107*/ Bool8      isBeingHealed;		// 1 if a medic is currently healing this unit
/*0x108*/ Box16     contourBounds;      // A rect that specifies the closest contour (collision) points

/*0x110*/ s16       removeTimer;        // Hallucination, Dark Swarm, Disruption Web, Broodling (but not Scanner Sweep according to BWAPI)
/*0x112*/ u16       defensiveMatrixHp;
/*0x114*/ u8        defensiveMatrixTimer;
/*0x115*/ u8        stimTimer;
/*0x116*/ u8        ensnareTimer;
/*0x117*/ u8        lockdownTimer;
/*0x118*/ u8        irradiateTimer;
/*0x119*/ u8        stasisTimer;
/*0x11A*/ u8        plagueTimer;
/*0x11B*/ u8        isUnderStorm;       // Used to tell if a unit is under psi storm	(is "stormTimer" in BWAPI)
/*0x11C*/ CUnit*    irradiatedBy;
/*0x120*/ u8        irradiatePlayerId;
/*0x121*/ u8        parasiteFlags;      // Each bit corrisponds to the player who has parasited this unit*/
/*0x122*/ u8        cycleCounter;       // counts/cycles up from 0 to 7 (inclusive). See also 0x85.
/*0x123*/ u8        isBlind;			// is bool in BWAPI
/*0x124*/ u8        maelstromTimer;
/*0x125*/ u8        unusedTimer;        // ?? Might be afterburner timer or ultralisk roar timer
/*0x126*/ u8        acidSporeCount;
/*0x127*/ u8        acidSporeTime[9];
/*0x130*/ u16       offsetIndex3by3;    // Cycles between 0-12 for each bullet fired by this unit (if it uses a "Attack 3x3 area" weapon)
/*0x132*/ u16       _padding_0x132;
/*0x134*/ void*     pAI;
/*0x138*/ u16       airStrength;
/*0x13A*/ u16       groundStrength;
/*0x13C*/ struct { u32 left, right, top, bottom; } finderIndex;  // Ordering for unit boundries in unit finder for binary search
/*0x14C*/ u8        repulseUnknown;
/*0x14D*/ u8        repulseAngle;       // updated only when air unit is being pushed
/*0x14E*/ Point8    driftPos;           // (mapsizex / 1.5 max)
};

C_ASSERT(sizeof(CUnitLayout) == 336);	//0x150
//static_assert(sizeof(CUnitLayout) == 336, "The size of the CUnitLayout structure is invalid");

#pragma pack()
