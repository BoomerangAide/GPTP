/// Basic logging to text file supported by GPTP.
/// Note: If you build the plugin in Release mode, all logging will be disabled.
/// Made by pastelmind

#ifdef _DEBUG
//If this macro is not defined, GPTP::logger will be disabled.
#define GPTP_LOGGING_ENABLED
#endif

#include <fstream>

namespace GPTP {

class GameLogger {
  public:
    /// Writes a "game start" message to the log file and resets the internal
    /// frame timer.
    bool startGame();

    /// Writes a "game end" message to the log file.
    bool endGame();
    
    template <typename T>
    GameLogger& operator<<(const T& t);
    GameLogger& operator<<(std::ostream& (*func)(std::ostream&));

  private:
    bool checkLogFile();
    bool updateFrame();
    std::ofstream logFile;
#pragma warning(suppress:26495) //variable is initialized in startGame  
    int lastUpdatedFrame;
};

extern GameLogger logger;

} //GPTP


//-------- Template member function definitions --------//

namespace GPTP {

template <typename T>
GameLogger& GameLogger::operator<<(const T& t) {

  #ifdef GPTP_LOGGING_ENABLED

  if (checkLogFile())
    if (updateFrame())
      logFile << t;

  #endif

  return *this;
}

} //GPTP

