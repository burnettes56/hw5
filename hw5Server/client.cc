/*
 * Author:  Hayden Burnette
 * Date  :  4/18/2018
 * Name  :  client.cc
 * Purpose:
 *      The purpose of client.cc
 *      is to drive the client end
 *      of our socket program
 *      Functionality includes:
 *          -Reading and writing
 *              command files
 *          -char processing
 *          -socket send/receive
 *          -use of Protected
 *              SafeQueues
 *
 * Modified: 4/25/2018
 */
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>
#include <queue>

#ifndef SAFEQUEUE_H

#include "SafeQueue.h"

#endif

using namespace std;
#define STRLEN 80
/// Holds all of the required Message data

const int BUFFERSIZE = 80; // 80 characters
//
//functions
//
void OpenCommandFile(fstream &, const char *);

void ReadCommandFile(fstream &, SafeQueue &);

void CloseCommandFile(fstream &);

Message loadMessage();

void ProcessCommandFile(fstream &, const char *, SafeQueue &);

//
//global variables
//
SafeQueue myQueue;
fstream f;

int main(int argc, char **argv) {
    char dashP[STRLEN] = "",
            dashS[STRLEN] = "",
            dashC[STRLEN] = "",
            hostname[STRLEN],
            portnum[STRLEN],
            dashChar;

    struct addrinfo *myinfo;
    int sockdesc;
    Message nextMsg;
    int connection;
    int value;
    //there are no commands then display a message
    if (argc <= 1) {
        cout << "\nUsage: hw1 –c <commandfilename> -s <servername> -p <portnumber>" << endl;
        exit(0);
    } // if

    //loop through -p, -s flags
    while ((dashChar = getopt(argc, argv, "p:s:c:")) != -1) {
        switch (dashChar) {
            case 'p': {
                strncpy(dashP, optarg, STRLEN - 1);
            }
                break;
            case 's': {
                strncpy(dashS, optarg, STRLEN - 1);
            }
                break;
            case 'c': {
                strncpy(dashC, optarg, STRLEN - 1);
            }
                break;
        } // switch
    } // while

    ProcessCommandFile(f, dashC, myQueue);
    // These are hard coded for simplicity.
    // In real life, you would either:
    // - prompt for these values
    // - read these values in from the command line
    // - look them up in an online broker
    //strcpy(hostname, "lovelace.etsu.edu");
    strcpy(hostname, dashS);
    // This value *MUST* be the value used by the server you're
    // trying to connect to.
    strcpy(portnum, dashP);

    //console buffer
    cout << "\n\n\n\n";

    // Use AF_UNIX for unix pathnames instead
    // Use SOCK_DGRAM for UDP datagrams
    sockdesc = socket(AF_INET, SOCK_STREAM, 0);
    if (sockdesc < 0) {
        cout << "Error creating socket" << endl;
        exit(0);
    }

    if (getaddrinfo(hostname, portnum, NULL, &myinfo) != 0) {
        cout << "Error getting address" << endl;
        exit(0);
    }

    // Attempt to connect to the host
    connection = connect(sockdesc, myinfo->ai_addr, myinfo->ai_addrlen);
    if (connection < 0) {
        cout << "Error in connect" << endl;
        cout << "I could not connect to " << "\'" << dashS << "\'" << " at Port: " << dashP << endl;
        exit(0);
    } else {
        cout << "I connected to " << "\'" << dashS << "\'" << " at Port: " << dashP << endl;
    }
    // Send a message
    do {
        //load the next Message to be sent
        nextMsg = loadMessage();
        //  This could be:
        //      value = send(sockdesc, &nextMsg, BUFFERSIZE, 0);
        value = write(sockdesc, (char *) &nextMsg, sizeof(Message));
        cout << "CLIENT HAS SENT MESSAGE WITH KEY: " << nextMsg.key << endl;
        // Get a return message
        value = read(sockdesc, (char *) &nextMsg, sizeof(Message));
        cout << "***RETURN MESSAGE PAYLOAD IS : " << nextMsg.payload << " IS AT KEY " << nextMsg.key << endl;
    } while (nextMsg.command != 'q' && nextMsg.command != 'Q');
    // Close the socket
    close(sockdesc);
}

/////////////////////////////////////////////////////////
//                Command File Methods                 //
/////////////////////////////////////////////////////////

/// Processes a command file
/// \param fstream &f
/// \param const char *filename_ptr
/// \param vector<Message> &msg
/// \return void
void ProcessCommandFile(fstream &f, const char *filename, SafeQueue &msg) {
    OpenCommandFile(f, filename);
    ReadCommandFile(f, msg);
    CloseCommandFile(f);
}

/// Opens a command file for processing
/// \param fstream &f
/// \param const char *filename
/// \return void
void OpenCommandFile(fstream &f, const char *filename) {
    f.open(filename, fstream::in);
    if (!f) {
        cout << "\nbad command file\n" << endl;
        exit(0);
    }
}

/// Read a command file
///     and fills a message vector
/// \param fstream &f
/// \param queue<Message> &msg
/// \return void
void ReadCommandFile(fstream &f, SafeQueue &msg) {
    string strs[3];
    int count = 0;
    int idcount = 0;
    Message m;
    string str;
    if (!f) {
        cout << "\nError! Please try again!\n" << endl;
        exit(0);
    }
    while (true) {
        if (f.eof())
            break;
        while (count < 3) {
            getline(f, str);
            strs[count] = str;
            count++;
        }
        m.id = ++idcount;
        m.command = strs[0][0];
        strcpy(m.key, strs[1].c_str());
        strcpy(m.payload, strs[2].c_str());
        msg.Enqueue(m);
        count = 0;
    }
}

/// closes a command file
/// \param fstream &f
/// \return void
void CloseCommandFile(fstream &f) {
    f.close();
}

/// loads the next message
///     from the message queue
///     for processing
/// \return Message
Message loadMessage() {
    return myQueue.Dequeue();
}

/////////////////////////////////////////////////////////
//             end of Command File Methods             //
/////////////////////////////////////////////////////////
