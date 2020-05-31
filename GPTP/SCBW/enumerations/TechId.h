#pragma once

//General use
namespace TechId {
enum Enum {
  Stimpacks         = 0x00,
  Lockdown          = 0x01,
  EMPShockwave      = 0x02,
  SpiderMines       = 0x03,
  ScannerSweep      = 0x04, // (default)
  TankSiegeMode     = 0x05,
  DefensiveMatrix   = 0x06, // (default)
  Irradiate         = 0x07,
  YamatoGun         = 0x08,
  CloakingField     = 0x09,
  PersonnelCloaking = 0x0A,
  Burrowing         = 0x0B,
  Infestation       = 0x0C, // (default)
  SpawnBroodlings   = 0x0D,
  DarkSwarm         = 0x0E, // (default)
  Plague            = 0x0F,
  Consume           = 0x10,
  Ensnare           = 0x11,
  Parasite          = 0x12, // (default)
  PsionicStorm      = 0x13,
  Hallucination     = 0x14,
  Recall            = 0x15,
  StasisField       = 0x16,
  ArchonWarp        = 0x17, // (default)
  Restoration       = 0x18,
  DisruptionWeb     = 0x19,
  UnusedTech26      = 0x1A, // (unused)
  MindControl       = 0x1B,
  DarkArchonMeld    = 0x1C, // (default)
  Feedback          = 0x1D, // (default)
  OpticalFlare      = 0x1E,
  Maelstrom         = 0x1F,
  LurkerAspect      = 0x20,
  UnusedTech33      = 0x21, // (default; possibly liftoff)
  Healing           = 0x22, // (default)
  UnusedTech35      = 0x23, // (unused)
  UnusedTech36      = 0x24, // (unused)
  UnusedTech37      = 0x25, // (unused)
  UnusedTech38      = 0x26, // (unused)
  UnusedTech39      = 0x27, // (unused)
  UnusedTech40      = 0x28, // (unused)
  UnusedTech41      = 0x29, // (unused)
  UnusedTech42      = 0x2A, // (unused)
  UnusedTech43      = 0x2B, // (unused)
  None              = 0x2C,
  NuclearStrike		  = 0x2D, //based on bwapi/BWMemoryEdit/Enums/TechType.cs
  UnknownLastEntry  = 0x2E	//based on bwapi/BWMemoryEdit/Enums/TechType.cs
};
}


//For use with techdata_dat::AvailableSC[] and techdata_dat::ResearchedBw[]
namespace ScTech {
enum Enum {
  Stimpacks         = 0,
  Lockdown          = 1,
  EMPShockwave      = 2,
  SpiderMines       = 3,
  ScannerSweep      = 4, // (default)
  TankSiegeMode     = 5,
  DefensiveMatrix   = 6, // (default)
  Irradiate         = 7,
  YamatoGun         = 8,
  CloakingField     = 9,
  PersonnelCloaking = 10,
  Burrowing         = 11,
  Infestation       = 12, // (default)
  SpawnBroodlings   = 13,
  DarkSwarm         = 14, // (default)
  Plague            = 15,
  Consume           = 16,
  Ensnare           = 17,
  Parasite          = 18, // (default)
  PsionicStorm      = 19,
  Hallucination     = 20,
  Recall            = 21,
  StasisField       = 22,
  ArchonWarp        = 23, // (default)
};
}

//For use with techdata_dat::AvailableBW[] and techdata_dat::ResearchedBw[]
namespace BwTech {
enum Enum {
  Restoration       = 0,
  DisruptionWeb     = 1,
  UnusedTech26      = 2, // (unused)
  MindControl       = 3,
  DarkArchonMeld    = 4, // (default)
  Feedback          = 5, // (default)
  OpticalFlare      = 6,
  Maelstrom         = 7,
  LurkerAspect      = 8,
  UnusedTech33      = 9, // (default; possibly liftoff)
  Healing           = 10, // (default)
  UnusedTech35      = 11, // (unused)
  UnusedTech36      = 12, // (unused)
  UnusedTech37      = 13, // (unused)
  UnusedTech38      = 14, // (unused)
  UnusedTech39      = 15, // (unused)
  UnusedTech40      = 16, // (unused)
  UnusedTech41      = 17, // (unused)
  UnusedTech42      = 18, // (unused)
  UnusedTech43      = 19, // (unused)
};
}

