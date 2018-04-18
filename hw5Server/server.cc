
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <sys/wait.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include <fstream>
#include <pthread.h>
#include <errno.h>

#ifndef SETUPDATA_H
#include "SetupData.h"
#endif
#ifndef SAFEQUEUE_H
#include "SafeQueue.h"
#endif
#ifndef LOG_H
#include "Log.h"
#endif

using namespace std;
#define STRLEN 32

//end Message

const int BUFFERSIZE = 80;

// Function declarations

void *handleRequest(void *arg);

void SendToHandler(Message);

void ProcessSetupFile(SetupData &);

void CreateHandlers();

void LogStart(Log &, SetupData &);

void LogCommand(Message, Log &);

void LogEnd(Log &);

void PrintError(int &, SetupData &);

// readn( ): reads n bytes from file descriptor fd
//    From Stevens, Unix Network Programming
int readn(int fd, void *vptr, int n);

//
//global variables
//

//0:return SafeQueue
//1:putStore SafeQueue
//2:number SafeQueue
//3:search SafeQueue
SafeQueue myQueues [4];
//0:Main thread - handleRequest
//1:Return thread - runReturn (not implemented)
//2:putStore thread - runPutStore
//3:number thread - runNumber
//4:search thread - runSearch
pthread_t threads [5];
int pipeLogger[2];
fstream f;
Log *log;
SetupData *data;

int main(int argc, char **argv) {
    int sockdesc;
    char dashP[STRLEN] = "", dashS[STRLEN] = "";
    struct addrinfo *myinfo;
    char portnum[BUFFERSIZE + 1];
    int connection;
    char c;

    if (argc <= 1) {
        cout << "\nUsage: hw1 –p <pathame> [-s <setupfilename>]" << endl;
        exit(0);
    } // if

    //loop through -p, -s flags
    while ((c = getopt(argc, argv, "p:s")) != -1) {
        switch (c) {
            case 'p': {
                strncpy(dashP, optarg, STRLEN - 1);
            }
                break;
            case 's': {
                strncpy(dashS, optarg, STRLEN - 1);
            }
                break;
        }; // switch
    } // while
    data = new SetupData(dashP, dashS);
    ProcessSetupFile(*data);
    strcpy(portnum, data->getPortNumber().c_str());
    log = new Log(data->getLogfilename());
    LogStart(*log, *data);
    system("mkdir \"./keys\" >> /dev/null 2>&1");

    // This number is hard-coded for simplicity
    // In real life, you would either:
    //   - use a dedicated port number (e.g., 80 == web server).
    //     You should be so lucky.
    //
    //   - use a command line parameter (e.g., -p 2000) and
    //     use getopt( ) to parse the command line args.
    //
    //   - use a port stepper, a loop that starts at some
    //     lower value (say, 2000), tries the getaddrinfo() and
    //     bind( ) calls - if successful, that's the port, else
    //     increment by one and try again, up to the upper value
    //     (say, 2500). Just remember the portnum is a C string
    //     but you count with int's. The lower and upper values
    //     could be passed in with command line args.

    // Use AF_UNIX for unix pathnames instead
    // Use SOCK_DGRAM for UDP datagrams
    sockdesc = socket(AF_INET, SOCK_STREAM, 0);
    if (sockdesc < 0) {
        cout << "Error creating socket" << endl;
        exit(0);
    }

    // Set up the address record
    if (getaddrinfo("0.0.0.0", portnum, NULL, &myinfo) != 0) {
        cout << "Error getting address" << endl;
        exit(0);
    }

    // Bind the socket to an address
    if (bind(sockdesc, myinfo->ai_addr, myinfo->ai_addrlen) < 0) {
        cout << "Error binding to socket" << endl;
        exit(0);
    }

    // Display the port number that was successful
    cout << "Bind successful, using portnum = " << portnum << endl;

    // Now listen to the socket
    if (listen(sockdesc, 1) < 0) {
        cout << "Error in listen" << endl;
        exit(0);
    }

    // Forever loop - kill manually
    // Accept a connect request and use connection, *NOT* sockdesc
    // (tcp handoff)
    for (;;) {
        cout << "Calling accept" << endl;
        connection = accept(sockdesc, NULL, NULL);
        if (connection < 0) {
            cout << "Error in accept" << endl;
            exit(0);
        } else {
            // Create a thread to handle the request
            // Show the connection number (your process's FILE
            // descriptor) for fun - the client could care less
            cout << "Calling pthread_create with connection = "
                 << connection << endl;
            pthread_create(&threads[0], NULL, handleRequest, (void *) &connection);
            cout << "After create" << endl;
        }
    } // for
}
/////////////////////////////////////////////////////////
//                  Pthreads Methods                   //
/////////////////////////////////////////////////////////

/// Creates a file in based on its file name
///     returns a bool if successful or not
/// \param fstream &f
/// \param const char *filename_ptr
/// \param Message &msg
/// \return bool
bool CreateFile(fstream &f, const char *filename_ptr, Message &msg) {
    f.open(filename_ptr, fstream::out);
    if (!f) {
        cout << "\nCould not create key file\n";
        return false;
    }
    f << msg.payload << endl;
    f.close();
    return true;
}
/// Method for pthreads to run
///      to process a PutStore Command
/// \param void*
/// \return void*
void *runPutStore(void *) {
    Message m;
    string newPayload;
    while (true) {
        m = myQueues[1].Dequeue();
        if (m.command == 'q' || m.command == 'Q')
            break;
        cout << "\nPut Store server has received msg #" << m.id << " key: " << m.key << ", Payload: "
             << m.payload << endl;
        string str = "./keys/" + string(m.key);
        const char *k = str.c_str();
        bool suc = CreateFile(f, k, m);
        if (!suc) {
            newPayload = "(PUT_STORE) DUPLICATE key: " + string(m.key);
            strcpy(m.payload, newPayload.c_str());
        } else {
            newPayload = "(PUT_STORE) OKAY";
            strcpy(m.payload, newPayload.c_str());
        }
        myQueues[0].Enqueue(m);
    }
    cout << "Put Store server has received 'quit' command, killing process " << endl;;
    pthread_exit(0);
}
/// Method for pthreads to run
///      to process a Search Command
/// \param void*
/// \return void*
void *runSearch(void *) {
    Message m;
    string str;
    string newPayload;
    while (true) {
        m = myQueues[3].Dequeue();
        if (m.command == 'q' || m.command == 'Q')
            break;
        cout << "\nSearch server has received msg #" << m.id << " key: " << m.key << ", Payload: " << m.payload << endl;
        system(string("find ./keys -name '" + string(m.key) + "' >> temp").c_str());
        f.open("temp", fstream::in);
        getline(f, newPayload);
        system("rm temp");
        f.close();
        if (newPayload == "")
            newPayload = "(SEARCH) FILE NOT FOUND WITH KEY: " + string(m.key);
        else {
            //get payload from key
            f.open(string("./keys/" + string(m.key)).c_str(), fstream::in);
            getline(f, str);
            f.close();
            newPayload = "(SEARCH) PAYLOAD AT KEY " + string(m.key) + " IS " + str;
        }
        strcpy(m.payload, newPayload.c_str());
        https://github.com/burnettes56/hw5.git
        myQueues[0].Enqueue(m);
    }
    cout << "Search server has received 'quit' command, killing process." << endl;;
    pthread_exit(0);
}
/// Method for pthreads to run
///      to process a Number Command
/// \param void*
/// \return void*
void *runNumber(void *) {
    Message m;
    string newPayload;
    while (true) {
        m = myQueues[2].Dequeue();
        if (m.command == 'q' || m.command == 'Q')
            break;
        cout << "\nNumber server has received msg #" << m.id << " key: " << m.key << ", Payload: " << m.payload << endl;
        system("ls ./keys | wc -l >> temp");
        f.open("temp", fstream::in);
        getline(f, newPayload);
        newPayload = "(NUMBER) THE NUMBER OF FILES STORED IS " + newPayload;
        strcpy(m.payload, newPayload.c_str());
        f.close();
        system("rm temp");
        myQueues[0].Enqueue(m);
    }
    cout << "Number server has received 'quit' command, killing process." << endl;
    pthread_exit(0);
}
/// Handles all return messages and
///     and sends messages back to
///     client
/// \param void*
/// \return void*
void *runReturn(void *arg) {
    Message m;
    int connection = *((int *) arg);
    while (m.command != 'q' && m.command != 'Q') {
        m = myQueues[0].Dequeue();
        write(connection, (char*)&m, sizeof(Message));
        cout << "Return server send back Message with ID: " << m.id << "\n" << endl;
    }
    cout << "Return server has received 'quit' command, killing process." << endl;
    //send last exit payload
    pthread_exit(0);
}


/// handles a single request from client
/// \param arg
/// \return void*
void *handleRequest(void *arg) {
    int connection;
    int value;
    char buffer[BUFFERSIZE + 1], str[BUFFERSIZE + 1];
    Message msgFromServer;
    pthread_detach(pthread_self());
    connection = *((int *) arg);
    // Show the actual FILE descriptor used
    cout << "Server thread, connection = " << connection << endl;

    //screen buffer
    cout << "\n\nStarting Command Processor...\n";
    cout << "Spinning Command Threads...\n\n";

    /////////////////////////////////////////////
    //     Child Code/Logger Thread Handler    //
    /////////////////////////////////////////////

    int forkMe = fork();
    if (forkMe < 0) {
        cout << "\nlogger fork failed to initalize. Please try again!" << endl;
        exit(0);
    } else if (forkMe == 0) {
        LogStart(*log, *data);
        //fork success
        while (msgFromServer.command != 'q' && msgFromServer.command != 'Q') {
            read(pipeLogger[0], (char *) &msgFromServer, sizeof(Message));
            cout << "\nLog server has logged msg #" << msgFromServer.id << " key: " << msgFromServer.key
                 << ", Payload: " << msgFromServer.payload;
            LogCommand(msgFromServer, *log);
        }
        //q or Q has been received so quit and close pipe
        cout << "Log server has received 'quit' command, killing process " << endl;
        close(pipeLogger[0]);
        close(pipeLogger[1]);
        LogEnd(*log);
        exit(0);
    } else {

        /////////////////////////////////////////////
        //     Parent Code/Main Thread Handler     //
        /////////////////////////////////////////////

        // Receive a message
        // Note: the next two lines are alternate methods for reading from
        // the socket.
        //value = recv(connection, buffer, 20, 0);
        //value = read(connection, buffer, BUFFERSIZE);
        value = readn(connection, (char *) &msgFromServer, sizeof(Message));
        if (value < 0) {
            cout << "Error on recv" << endl;
            exit(0);
        } else if (value == 0) {
            cout << "End of transmission" << endl;
            exit(0);
        }
        //create all commands handler threads
        pthread_create(&threads[1], NULL, runReturn, (void *) &connection);                  //initialize return thread
        CreateHandlers();
        cout << "Server received Message with ID: " << msgFromServer.id << " Sending to Command Processor." << endl;
        SendToHandler(msgFromServer);

        // Continue until the message command is q/Q(quit all servers)
        while (msgFromServer.command != 'q' && msgFromServer.command != 'Q') {
            // Get the next message
            value = readn(connection, (char *) &msgFromServer, sizeof(Message));
            if (value < 0) {
                cout << "Error on recv" << endl;
                exit(0);
            } else if (value == 0) {
                cout << "End of transmission" << endl;
                exit(0);
            }
            cout << "Server received Message with ID: " << msgFromServer.id << " Sending to Command Processor." << endl;
            SendToHandler(msgFromServer);
        } // while
        // Close the socket

        close(connection);
        pthread_join(threads[1], NULL);
        pthread_join(threads[2], NULL);
        pthread_join(threads[3], NULL);
        pthread_join(threads[4], NULL);
    }
    wait(&forkMe);
    cout << "\n\nwe ended\n" << endl;
    pthread_exit(0);
}

/////////////////////////////////////////////////////////
//              end of Pthreads Methods                //
/////////////////////////////////////////////////////////

/// Creates all commands handlers threads
/// \return void
void CreateHandlers() {
    pthread_create(&threads[2], NULL, runPutStore, NULL);                         //initialize putStore thread
    pthread_create(&threads[3], NULL, runNumber, NULL);                           //initialize number thread
    pthread_create(&threads[4], NULL, runSearch, NULL);                           //initialize search thread
}
/// Sends a message to the
///     correct server for
///     processing
/// \param Message m
/// \return void
void SendToHandler(Message msg) {
    LogCommand(msg, *log);
    write(pipeLogger[1], (char *) &msg, sizeof(Message));
    switch (msg.command) {
        //send message to put store server
        case 'P':
        case 'p': {
            myQueues[1].Enqueue(msg);
            cout << "We sent putstore a commmand to process";
        }
            break;
            //send message to search server
        case 'S':
        case 's': {
            myQueues[3].Enqueue(msg);
            cout << "We sent search a commmand to process";
        }
            break;
            //send message to put number server
        case 'N':
        case 'n': {
            myQueues[2].Enqueue(msg);
            cout << "We sent number a commmand to process";
        }
            break;
            //tell all processes to quit
        case 'q':
        case 'Q': {
            //end all
            myQueues[0].Enqueue(msg);
            myQueues[1].Enqueue(msg);
            myQueues[2].Enqueue(msg);
            myQueues[3].Enqueue(msg);
        }
            break;
            //if the command read is invalid, it just skips it
        default: {
            cout << "\n\'" << msg.command << "\'"
                 << " IS NOT A ACCEPTED. (ACCEPTED VALUES: p(put_Store), n(number), s(search), q(quit))";
        }
            break;
    }
}

/// Processes a setup file
/// \param SetupData &data
/// \param Log &log
/// \return void
void ProcessSetupFile(SetupData &data) {
    string stringData;
    int errorCode = data.open();
    PrintError(errorCode, data);
    //attempt to open file
    if (errorCode == -1) {
        exit(0);
    }
        //if open succeeded!
    else {
        data.read();                                                                           //read the file and store values
        data.close();                                                                          //close the file
    }
}

/// prints a Error message
///     returned by SetupData
/// \param int &e
/// \param SetupData &setup
/// \return void
void PrintError(int &e, SetupData &setup) {
    string errorCode = setup.error(e);
    cout << "Error Code: " << errorCode << endl;
}

//
//opens the log file for logging
//
void LogStart(Log &l, SetupData &s) {
    int success = l.open();
    if (success != 0) {
        cout << "Could not open set up file! Please check data and try again." << endl;
        exit(0);
    } else {
        string stringData =
                "\nLog File: " + s.getLogfilename()
                + "\nCommand File : " + s.getCommandfilename()
                + "\nUsername: " + s.getUsername()
                + "\nPort Number: " + s.getPortNumber();
        l.writeLogRecord(stringData);
    }
}

//
//log command file data
//
void LogCommand(Message m, Log &l) {
    string stringData =
            string("\nCommand: ") + m.command
            + "\nKey: " + m.key
            + "\nPayload: " + m.payload;
    l.writeLogRecord(stringData);
}

//
//closes the log file
//
void LogEnd(Log &l) {
    int success = l.close();
    if (success != 0) {
        cout << "Could not close set up file! Please check data and try again." << endl;
        exit(0);
    }
}


//This method was not implemented by Hayden Burnette
//  but was provided by Dr.Martin Barrett at ETSU
//  The comments below are comments from Dr. Barrett


/* Adapted from Stevens, Unix Network Programming, v.1 */
/* include readn */


int                  /* Read "n" bytes from a descriptor. */
readn(int fd, void *vptr, int n) {
    int nleft;
    int nread;
    char *ptr;

    ptr = (char *) vptr;
    nleft = n;
    while (nleft > 0) {
        if ((nread = read(fd, ptr, nleft)) < 0) {
            if (errno == EINTR)
                nread = 0;      /* and call read() again */
            else
                return (-1);
        } else if (nread == 0)
            break;            /* EOF */

        nleft -= nread;
        ptr += nread;
    }
    return (n - nleft);      /* return >= 0 */
}
/* end readn */

