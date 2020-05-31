#pragma once

//General use
namespace UpgradeId {
enum Enum {
  TerranInfantryArmor   = 0x00,
  TerranVehiclePlating  = 0x01,
  TerranShipPlating     = 0x02,
  ZergCarapace          = 0x03,
  ZergFlyerCaparace     = 0x04,
  ProtossArmor          = 0x05,
  ProtossPlating        = 0x06,
  TerranInfantryWeapons = 0x07,
  TerranVehicleWeapons  = 0x08,
  TerranShipWeapons     = 0x09,
  ZergMeleeAttacks      = 0x0A,
  ZergMissileAttacks    = 0x0B,
  ZergFlyerAttacks      = 0x0C,
  ProtossGroundWeapons  = 0x0D,
  ProtossAirWeapons     = 0x0E,
  ProtossPlasmaShields  = 0x0F,
  U_238Shells           = 0x10,
  IonThrusters          = 0x11,
  UnusedBurstLasers     = 0x12,
  TitanReactor          = 0x13, // (SV +50)
  OcularImplants        = 0x14,
  MoebiusReactor        = 0x15, // (Ghost +50)
  ApolloReactor         = 0x16, // (Wraith +50)
  ColossusReactor       = 0x17, // (BC +50)
  VentralSacs           = 0x18,
  Antennae              = 0x19,
  PneumatizedCarapace   = 0x1A,
  MetabolicBoost        = 0x1B,
  AdrenalGlands         = 0x1C,
  MuscularAugments      = 0x1D,
  GroovedSpines         = 0x1E,
  GameteMeiosis         = 0x1F, // (Queen +50)
  MetasynapticNode      = 0x20, // (Defiler +50)
  SingularityCharge     = 0x21,
  LegEnhancements       = 0x22,
  ScarabDamage          = 0x23,
  ReaverCapacity        = 0x24,
  GraviticDrive         = 0x25,
  SensorArray           = 0x26,
  GraviticBoosters      = 0x27,
  KhaydarinAmulet       = 0x28, // (HT +50)
  ApialSensors          = 0x29,
  GraviticThrusters     = 0x2A,
  CarrierCapacity       = 0x2B,
  KhaydarinCore         = 0x2C, // (Arbiter +50)
  UnusedUpgrade45       = 0x2D,
  UnusedUpgrade46       = 0x2E,
  ArgusJewel            = 0x2F, // (Corsair +50)
  UnusedUpgrade48       = 0x30,
  ArgusTalisman         = 0x31, // (DA +50)
  UnusedUpgrade50       = 0x32,
  CaduceusReactor       = 0x33, // (Medic +50)
  ChitinousPlating      = 0x34,
  AnabolicSynthesis     = 0x35,
  CharonBooster         = 0x36,
  UnusedUpgrade55       = 0x37,
  UnusedUpgrade56       = 0x38,
  UnusedUpgrade57       = 0x39,
  UnusedUpgrade58       = 0x3A,
  UnusedUpgrade59       = 0x3B,
  GlobalUpgrade60       = 0x3C, // (unused 60)
  None                  = 0x3D
};
}

//For use with upgrades_dat::MaximumUpgSc[] and upgrades_dat::CurrentUpgSc[]
namespace ScUpgrade {
enum Enum {
  TerranInfantryArmor   = 0,
  TerranVehiclePlating  = 1,
  TerranShipPlating     = 2,
  ZergCarapace          = 3,
  ZergFlyerCaparace     = 4,
  ProtossArmor          = 5,
  ProtossPlating        = 6,
  TerranInfantryWeapons = 7,
  TerranVehicleWeapons  = 8,
  TerranShipWeapons     = 9,
  ZergMeleeAttacks      = 10,
  ZergMissileAttacks    = 11,
  ZergFlyerAttacks      = 12,
  ProtossGroundWeapons  = 13,
  ProtossAirWeapons     = 14,
  ProtossPlasmaShields  = 15,
  U_238Shells           = 16,
  IonThrusters          = 17,
  BurstLasers           = 18, // (Unused)
  TitanReactor          = 19, // (SV +50)
  OcularImplants        = 20,
  MoebiusReactor        = 21, // (Ghost +50)
  ApolloReactor         = 22, // (Wraith +50)
  ColossusReactor       = 23, // (BC +50)
  VentralSacs           = 24,
  Antennae              = 25,
  PneumatizedCarapace   = 26,
  MetabolicBoost        = 27,
  AdrenalGlands         = 28,
  MuscularAugments      = 29,
  GroovedSpines         = 30,
  GameteMeiosis         = 31, // (Queen +50)
  MetasynapticNode      = 32, // (Defiler +50)
  SingularityCharge     = 33,
  LegEnhancements       = 34,
  ScarabDamage          = 35,
  ReaverCapacity        = 36,
  GraviticDrive         = 37,
  SensorArray           = 38,
  GraviticBoosters      = 39,
  KhaydarinAmulet       = 40, // (HT +50)
  ApialSensors          = 41,
  GraviticThrusters     = 42,
  CarrierCapacity       = 43,
  KhaydarinCore         = 44, // (Arbiter +50)
  UnusedUpgrade45       = 45,
};
}

//For use with upgrades_dat::MaximumUpgBw[] and upgrades_dat::CurrentUpgBw[]
namespace BwUpgrade {
enum Enum {
  UnusedUpgrade46       = 0,
  ArgusJewel            = 1, // (Corsair +50)
  UnusedUpgrade48       = 2,
  ArgusTalisman         = 3, // (DA +50)
  UnusedUpgrade50       = 4,
  CaduceusReactor       = 5, // (Medic +50)
  ChitinousPlating      = 6,
  AnabolicSynthesis     = 7,
  CharonBooster         = 8,
  UnusedUpgrade55       = 9,
  UnusedUpgrade56       = 10,
  UnusedUpgrade57       = 11,
  UnusedUpgrade58       = 12,
  UnusedUpgrade59       = 13,
  GlobalUpgrade60       = 14, // (unused 60)
};
}
