#ifndef SETUPDATA_H
#define SETUPDATA_H

#include <string>
#include <fstream>
#include <map>
#include <iostream>
using namespace std;

// class SetupData
// Open, read, and display setup parameters
class SetupData {
public:
   // Public error codes
   static const int SUCCESS = 0;
   static const int BAD_PATHNAME = -1;
   static const int BAD_FILENAME = -2;

  // Constructors
  SetupData( );
  SetupData( string pname, string sname );

  // Getters and setters
  void setPathname(string pname);
  string getPathname();
  void setSetupfilename(string sname);
  string getSetupfilename();
  void setLogfile(string lname);
  string getLogfilename();
  void setUsername(string uname);
  string getUsername();
  void setPortNumber(string port);
  string getPortNumber();

  // Open, read, display, and close
  int open();
  void read( );
  void print( );
  void close( );

  // Error stringifier
  string error(int e);

private:
  map<string, string> values; // Setup values
  fstream f;
};

#endif
