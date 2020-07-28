#define _CRT_SECURE_NO_WARNINGS

#include "logger.h"
#include <ctime>
#include <SCBW/scbwdata.h>

namespace GPTP {

GameLogger logger;

bool GameLogger::startGame() {

  #ifdef GPTP_LOGGING_ENABLED

  if (checkLogFile()) {
    time_t currentTime;
    time(&currentTime);

    logFile << "Game started on " << ctime(&currentTime) << std::endl;
  }

  lastUpdatedFrame = -1;  //So logging is possible on frame 0

  #endif

  return true;
}

bool GameLogger::endGame() {

  #ifdef GPTP_LOGGING_ENABLED

  if (checkLogFile()) {
    time_t currentTime;
    time(&currentTime);
    logFile << "Game ended on " << ctime(&currentTime) << std::endl;
  }
  
  #endif

  return true;
}

GameLogger& GameLogger::operator<<(std::ostream& (*func)(std::ostream&)) {

  #ifdef GPTP_LOGGING_ENABLED

  if (checkLogFile())
    if (updateFrame())
      logFile << func;

  #endif

  return *this;
}

bool GameLogger::checkLogFile() {

  #ifdef GPTP_LOGGING_ENABLED

  if (logFile.is_open())
    return true;

  time_t currentTime;
  time(&currentTime);

  char buffer[100];
  strftime(buffer, sizeof(buffer), "Game %Y-%m-%d %Hh %Mm %Ss.log",
    localtime(&currentTime));

  logFile.open(buffer);
  return !logFile.fail();

  #else

  return true;

  #endif
}

bool GameLogger::updateFrame() {

  #ifdef GPTP_LOGGING_ENABLED

  const int currentFrame = *elapsedTimeFrames;
    
  if (currentFrame > lastUpdatedFrame) {
    lastUpdatedFrame = currentFrame;
    logFile << "\nFrame " << currentFrame << ":\n";
  }

  #endif

  return true;
}

} //GPTP
