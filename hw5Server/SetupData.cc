
/*                                          */
/* Author: Hayden Burnette                  */
/* Date  : 2/04/2018                        */
/*                                          */
/*                                          */

#include <string>
#include <fstream>
#include <map>
#include<unistd.h>
#include<iostream>
//define
#ifndef SETUPDATA_H

#include "SetupData.h"

#endif

/// Class Default Constructor:
///     SetupData()
/// \return new SetupData object
SetupData::SetupData() {
    SetupData::setPathname("");
    SetupData::setSetupfilename("");
}

/// Class Constructor:
///     SetupData(string pname, string sname)
/// \param string pname
/// \param string sname
/// \return new SetupData object
SetupData::SetupData(string pname, string sname) {
    SetupData::setPathname(pname);
    SetupData::setSetupfilename(sname);
}

/// sets the class attribute:
///     map<string pname,string pname>
/// \param string pname
/// \return void
void SetupData::setPathname(string pname) {
    values["pname"] = pname;
}

/// returns the path's name
/// \return string
string SetupData::getPathname() {
    return values["pname"];
}

/// sets the class attribute:
///     map<string sname,string sname>
///     which stores setup file's name
/// \param string sname
/// \return void
void SetupData::setSetupfilename(string sname) {
    if (sname == "") {
        values["sname"] = "setup";
    } else {
        values["sname"] = sname;
    }
}

/// returns the setup file's name
/// \return string
string SetupData::getSetupfilename() {
    return values["sname"];
}

/// sets the class attribute:
///     map<string lname,string lname>
///     which stores the log file's name
/// \param string lname
/// \return void
void SetupData::setLogfile(string lname) {
    values["lname"] = lname;
}

/// returns the log file's name
/// \return string
string SetupData::getLogfilename() {
    return values["lname"];
}

/// sets the class attribute:
///     map<string uname,string uname>
///     which stores the username
/// \param string uname
/// \return void
void SetupData::setUsername(string uname) {
    values["uname"] = uname;
}

/// returns the username
/// \return string
string SetupData::getUsername() {
    return values["uname"];
}

/// sets the portNumber from the set up file
/// \param string port
/// \return void
void SetupData::setPortNumber(string port) {
    values["port"] = port;
}

/// returns the port number
/// \return string
string SetupData::getPortNumber() {
    return values["port"];
}

/// opens a setup file for processing
/// \return int
int SetupData::open() {
    int success = 0;
    string str1 = values["pname"];
    const char *path = str1.c_str();
    string str2 = values["sname"];
    const char *setup = str2.c_str();
    if (chdir(path) != -1) {
        f.open(setup, fstream::in);
        if (!f) {
            //bad filename
            success = -2;
        }
    } else {
        //path failed return bad path
        success = -1;
    }
    return success;
}

/// reads a setup file
///     and sets logfile,
///     username,
///     and port number attributes
/// \return void
void SetupData::read() {
    string strs[3];
    int count = 0;
    string str;
    string str2;
    int strlength;
    while (true) {
        getline(f, str);
        if (f.eof())
            break;
        int posOfdata = str.find(' ') + 1;
        strlength = (sizeof str - 1);
        str2 = str.substr(posOfdata, (strlength - posOfdata));
        strs[count] = str2;
        count++;
    }
    //store values
    const char *conv0 = strs[0].c_str();
    SetupData::setLogfile(conv0);
    const char *conv1 = strs[1].c_str();
    SetupData::setUsername(conv1);
    const char *conv2 = strs[2].c_str();
    SetupData::setPortNumber(conv2);
}

/// prints the SetupData to the screen
/// \return void
void SetupData::print() {
    cout << "\nContents of setup file:\n" << endl;
    cout << "Log file: " << SetupData::getLogfilename() << endl;
    cout << "Username: " << SetupData::getUsername() << endl;
    cout << "Port Number " << SetupData::getPortNumber() << endl;
    cout << "\nEND OF FILE\n" << endl;
}

/// Closes a setup file
/// \return void
void SetupData::close() {
    if (f)
        f.close();
}

/// returns a string based
///     on a error message
/// \param int e
/// \return string
string SetupData::error(int e) {
    if (e == 0)
        return "No Errors detected!";
    else if (e == -1)
        return "Please check path!";
    else if (e == -2)
        return "Please check file setup file name!";
}
