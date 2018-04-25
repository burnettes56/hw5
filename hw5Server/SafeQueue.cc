/*
 * Author:  Hayden Burnette
 * Date  :  4/18/2018
 * Name  :  SafeQueue.cc
 * Purpose:
 *      The purpose of SafeQueue.cc
 *      is to make a queue that can
 *      be enqueued and dequeue safely.
 *      SafeQueue achieves this by implementing
 *      mutex and counting semaphores.
 *
 * Modified: 4/25/2018
 */

#include <stdlib.h>
#include <unistd.h>
#include <queue>
#include <pthread.h>
#include <semaphore.h>
#ifndef SAFEQUEUE_H

#include "SafeQueue.h"

#endif
using namespace std;
SafeQueue::SafeQueue() {
    pthread_mutex_init(&myMutex, NULL);                                 //initlazites mutex
    sem_init(&mySemaphore, 0, 0);                                       //makes program wait for semaphore
}

void SafeQueue::Enqueue(Message m) {
    pthread_mutex_lock(&myMutex);
    myQueue.push(m);
    pthread_mutex_unlock(&myMutex);
    sem_post(&mySemaphore);                                             //tells lets got of hold so it can can go.
}
Message SafeQueue::Dequeue() {
    Message m;
    sem_wait(&mySemaphore);                                             //if dequeue is called again,
    m = myQueue.front();                                                //      and queue is empty
    pthread_mutex_lock(&myMutex);                                       //      makes the program wait
    myQueue.pop();
    pthread_mutex_unlock(&myMutex);
    return m;
}
int SafeQueue::Count() {
    return myQueue.size();
}