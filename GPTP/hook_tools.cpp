#include "hook_tools.h"

//Injects a relative CALL to [target] at the [position].
//Original function from BWAPI by Kovarex; Modified by pastelmind
void callPatch(const void* target, void* position, const unsigned int nops) {
  u8* const data = new u8[5 + nops];
  data[0] = 0xE8; //Relative CALL instruction
  const u32 address = (u32)target - (u32)position - 5;  //Relative address
  *(u32*)(&data[1]) = address;
  for (unsigned int i = 0; i < nops; ++i)
    data[5 + i] = 0x90; //NOP instruction
  memoryPatch(position, data, 5 + nops);
  delete [] data;
}

//Injects a relative JMP to [target] at the [position].
//Original function from BWAPI by Kovarex; Modified by pastelmind
void jmpPatch(const void* target, void* position, unsigned int nops) {
  u8* const data = new u8[5 + nops];
  data[0] = 0xE9; //Relative JMP instruction
  const s32 address = (s32)target - (s32)position - 5;  //Relative address
  *(s32*)(&data[1]) = address;
  for (unsigned int i = 0; i < nops; ++i)
    data[5 + i] = 0x90; //NOP instruction
  memoryPatch(position, data, 5 + nops);
  delete [] data;
}

//Inject an array of bytes, using the given length.
void memoryPatch(void* const address, const u8* data, const size_t size) {
	DWORD oldProt = 0;
	VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProt);
	memcpy(address, data, size);
	VirtualProtect(address, size, oldProt, &oldProt);
}
