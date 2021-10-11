#include "CMDRECV_SaveLoadWrappers.h"
#include <SCBW/api.h>
//#include <logger.h>
//
//#define WRITE_TO_LOG(x) GPTP::logger<<x<<std::endl

/*
Hooks to perform some action after a save/load was executed.
No helpers, no certainty that the save/load was successful (though
there's a hint for "load", but it may not be 100% reliable.
Expected thing is to write an additional file accompanying
the save (with a name based on the original save file name) with
custom elements that are not or may not be saved, then in load,
check if such a file exist and load its data in addition to the
rest.
Note: based on the logger code for testing provided, the filenames
don't include the extension (would be snx).
*/

namespace hooks {

void CMDRECV_SaveGameAfter(char* savefileName) {
	//WRITE_TO_LOG("Saved with file " << savefileName);
}

;
void CMDRECV_LoadGameAfter(char* savefileName, bool loadSuccessful) {
	//WRITE_TO_LOG("Load with file " << savefileName);
	//WRITE_TO_LOG("It may have been a " << (loadSuccessful ? "success." : "failure."));
}

;

} //namespace hooks