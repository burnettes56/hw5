//
// Created by burnettes on 4/17/18.
//

#ifndef SAFEQUEUE_H
#define SAFEQUEUE_H

#include <stdlib.h>
#include <unistd.h>
#include <queue>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

struct Message {
    int id;
    char command;
    char key[16];
    char payload[128];
}; //end Message

class SafeQueue {
public:

    //constructors
    SafeQueue();

    //pubic methods
    void Enqueue(Message m);
    Message Dequeue();
    int Count();

private:
    queue<Message> myQueue;             //myQueue is initialized here
    pthread_mutex_t myMutex;            // = PTHREAD_MUTEX_INITIALIZER;
    sem_t mySemaphore;                  // call sem_init(&mySemaphore, 0, 0);
};


#endif //SAFEQUEUE_H
