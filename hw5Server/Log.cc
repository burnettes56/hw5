/*
 * Author:  Hayden Burnette
 * Date  :  2/04/2018
 * Name  :  log.cc
 * Purpose:
 *      The purpose of log.cc is to
 *      create a log file to store
 *      commands that have been sent
 *      for processing
 *
 * Modified: 4/25/2018
 */

// Uses C++ string class for convenience
#include <string>
// For the log file output
#include <fstream>
#include <time.h>
#ifndef LOG_H
#include "Log.h"
#endif

const string Log::DEFAULT_LOG_FILE_NAME = "log";
//------------
//constructors
//------------
/// Class Default Constructor:
///     Log()
/// \return new Log object
Log::Log()
{
    Log::setLogfileName(Log::DEFAULT_LOG_FILE_NAME);
}
/// Class char* Constructor:
///     Log(char* lname)
/// \param lname
/// \return new Log object
Log::Log(char* lname)
{
  string str(lname);
  Log::setLogfileName(lname);
}
/// Class string Constructor:
///     Log(string lname)
/// \param lname
/// \return new Log object
Log::Log(string lname)
{
  Log::setLogfileName(lname);
}
//------------
//setters
//------------
///sets the logfilename attribute
/// \param cname
 void Log::setLogfileName(string cname)
 {
   if(cname == "")
   {
     logfilename = Log::DEFAULT_LOG_FILE_NAME;
   }
   else
   {
     logfilename = cname; 
   }
 }
//------------
//getters
//------------
/// returns the logfilename attribute
/// \return string
string Log::getLogfileName()
{
  return logfilename;
}
/// return the Default logfileName (const string)
/// \return string
string Log::getDefaultLogfileName( )
{
  return Log::DEFAULT_LOG_FILE_NAME;
}
//------------
// actual functions that do cool things
//------------
/// opens a Log file
/// \return int
int Log::open()
{
  int success = -1;
  if(logF)
  {
     const char *path = logfilename.c_str();
     logF.open( path, fstream::out | ios_base::app);
     success = 0;
     logF << "BEGIN";
  }
  return success;
}
/// Closes a Log File
/// \return int
int Log::close()
{
  int success = -1;
  if(logF)
  {
     logF << "END " << Log::getTimeStamp() << endl;
     logF.close();
     success = 0;
  }
  return success;
}
/// write a record into a log
/// \param string s
/// \return int
int Log::writeLogRecord(string s)
{
  int success = -1;
  if(logF)
  {
     logF << s << endl;
     success = 0;
  }
  return success;
}
/// gets the system time stamp
/// \return string
string Log::getTimeStamp()
{
  time_t t;
  time(&t);
  string currtime = ctime(&t);
  return currtime;
}
